#ifndef RISCV32_H
#define RISCV32_H

#include "ir/tac.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cctype>

class RISC32Generator {
public:
    RISC32Generator(ProgramIR* ir) : program_ir_(ir) {}

    std::string generate() {
        output_ = "";
        func_counter_ = 0;

        for (auto& func : program_ir_->functions) {
            generate_function(func.get());
        }

        return output_;
    }

private:
    ProgramIR* program_ir_;
    std::string output_;
    int func_counter_ = 0;
    int label_counter_ = 0;
    int stack_size_ = 0;

    // Register tracking - only track non-temp vars in t registers
    std::vector<std::string> free_regs_;
    std::unordered_map<std::string, std::string> var_reg_;  // var -> register
    std::unordered_map<std::string, int> var_stack_offset_;  // var -> stack offset
    std::unordered_map<std::string, long long> immediate_val_;  // temp -> immediate value
    std::unordered_map<std::string, std::string> temp_source_;  // temp -> source variable (for LOAD)

    void emit(const std::string& line) {
        output_ += line + "\n";
    }

    void generate_function(FunctionIR* func) {
        func_counter_++;
        label_counter_ = 0;
        stack_size_ = 0;
        var_reg_.clear();
        var_stack_offset_.clear();
        immediate_val_.clear();
        temp_source_.clear();
        free_regs_ = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};

        // Count stack slots needed
        for (auto& instr : func->instrs) {
            if (instr.op == TacOp::STORE) {
                if (!is_temp_var(instr.dest) && var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                    var_stack_offset_[instr.dest] = stack_size_;
                    stack_size_ += 4;
                }
            }
        }

        // Header
        emit(".text");
        emit("");
        emit(".globl " + func->name);
        emit(func->name + ":");

        // Prologue
        std::string prologue_label = "prologue_" + func->name;
        emit(prologue_label + ":");

        if (stack_size_ > 0 || func->is_void == false) {
            int frame_size = (stack_size_ > 0) ? stack_size_ : 4;
            frame_size = ((frame_size + 15) / 16) * 16;
            emit("    addi sp, sp, -" + std::to_string(frame_size));
            if (stack_size_ > 0) {
                emit("    sw ra, 0(sp)");
            }
        }

        // Generate body
        std::string entry_label = "label" + std::to_string(label_counter_++);
        emit(entry_label + ":");

        generate_instrs(func);

        // Epilogue
        std::string ret_label = "ret_" + func->name;
        emit(ret_label + ":");
        if (stack_size_ > 0 || func->is_void == false) {
            int frame_size = (stack_size_ > 0) ? stack_size_ : 4;
            frame_size = ((frame_size + 15) / 16) * 16;
            if (stack_size_ > 0) {
                emit("    lw ra, 0(sp)");
            }
            emit("    addi sp, sp, " + std::to_string(frame_size));
        }
        emit("    ret");
    }

    bool is_temp_var(const std::string& var) {
        return !var.empty() && var[0] == '.';
    }

    bool is_label(const std::string& s) {
        if (s.empty()) return false;
        return s.substr(0, 2) == ".L" || s.substr(0, 5) == "label" ||
               s.substr(0, 3) == "ret" || s.substr(0, 8) == "prologue" ||
               s.substr(0, 5) == "break" || s.substr(0, 9) == "continue";
    }

    bool is_number(const std::string& s) {
        if (s.empty()) return false;
        size_t i = 0;
        if (s[0] == '-') {
            if (s.length() == 1) return false;
            i = 1;
        }
        for (; i < s.length(); i++) {
            if (!isdigit(s[i])) return false;
        }
        return true;
    }

    // Load a value into a register
    void load_to_reg(const std::string& var, const std::string& reg) {
        if (is_label(var)) return;
        if (var.empty()) return;

        // Check if it's in a register
        auto reg_it = var_reg_.find(var);
        if (reg_it != var_reg_.end()) {
            if (reg_it->second != reg) {
                emit("    mv " + reg + ", " + reg_it->second);
            }
            return;
        }

        // Check if it's a stack variable - load from stack
        auto stack_it = var_stack_offset_.find(var);
        if (stack_it != var_stack_offset_.end()) {
            emit("    lw " + reg + ", " + std::to_string(stack_it->second) + "(sp)");
            return;
        }

        // Check if it's a known immediate value
        auto imm_it = immediate_val_.find(var);
        if (imm_it != immediate_val_.end()) {
            emit("    li " + reg + ", " + std::to_string(imm_it->second));
            return;
        }

        // Check if it's a numeric string
        if (is_number(var)) {
            emit("    li " + reg + ", " + var);
            return;
        }

        // For temp vars, check if we know the source (for LOAD results)
        if (is_temp_var(var)) {
            auto src_it = temp_source_.find(var);
            if (src_it != temp_source_.end()) {
                // Reload from the original source
                auto src_stack_it = var_stack_offset_.find(src_it->second);
                if (src_stack_it != var_stack_offset_.end()) {
                    emit("    lw " + reg + ", " + std::to_string(src_stack_it->second) + "(sp)");
                    return;
                }
            }
            // Temp var not found - emit a safe default
            emit("    li " + reg + ", 0");
            return;
        }

        // Unknown variable
        emit("    li " + reg + ", 0");
    }

    // Store from a register to a variable's stack slot
    void store_to_var(const std::string& var, const std::string& reg) {
        if (is_label(var)) return;
        if (var == "a0") return;

        auto it = var_stack_offset_.find(var);
        if (it != var_stack_offset_.end()) {
            emit("    sw " + reg + ", " + std::to_string(it->second) + "(sp)");
        }
    }

    void generate_instrs(FunctionIR* func) {
        for (auto& instr : func->instrs) {
            generate_instr(instr);
        }
    }

    void generate_instr(const TacInstr& instr) {
        switch (instr.op) {
            case TacOp::LABEL:
                emit(instr.src2 + ":");
                break;

            case TacOp::LOAD_IMM: {
                long long val = std::stoll(instr.src1);
                immediate_val_[instr.dest] = val;
                emit("    li t0, " + instr.src1);
                // Clear any temp vars that were tracked in t0, since t0 now has a new value
                std::vector<std::string> to_remove;
                for (auto& p : var_reg_) {
                    if (is_temp_var(p.first) && p.second == "t0") {
                        to_remove.push_back(p.first);
                    }
                }
                for (auto& key : to_remove) {
                    var_reg_.erase(key);
                }
                break;
            }

            case TacOp::ADD: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    add t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::SUB: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    sub t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::MUL: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    mul t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::DIV: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    div t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::MOD: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    rem t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::LT: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    slt t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::GT: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    sgt t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::LE: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    slt t2, t1, t0");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::GE: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    slt t2, t0, t1");
                emit("    xori t2, t2, 1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::EQ: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    sub t2, t0, t1");
                emit("    seqz t2, t2");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::NE: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    sub t2, t0, t1");
                emit("    snez t2, t2");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::AND: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    and t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::OR: {
                load_to_reg(instr.src1, "t0");
                load_to_reg(instr.src2, "t1");
                emit("    or t2, t0, t1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::NOT: {
                load_to_reg(instr.src1, "t0");
                emit("    xori t2, t0, 1");
                var_reg_[instr.dest] = "t2";
                break;
            }

            case TacOp::LOAD: {
                load_to_reg(instr.src1, "t0");
                // Track the source of this temp var so we can reload if needed
                if (is_temp_var(instr.dest) && var_stack_offset_.find(instr.src1) != var_stack_offset_.end()) {
                    temp_source_[instr.dest] = instr.src1;
                }
                break;
            }

            case TacOp::STORE: {
                load_to_reg(instr.src2, "t0");
                store_to_var(instr.dest, "t0");
                // Don't track stack variables in t0 - t0 is a scratch register
                // that gets overwritten by li instructions
                break;
            }

            case TacOp::BEQZ: {
                load_to_reg(instr.src1, "t0");
                emit("    beqz t0, " + instr.src2);
                break;
            }

            case TacOp::BNEZ: {
                load_to_reg(instr.src1, "t0");
                emit("    bnez t0, " + instr.src2);
                break;
            }

            case TacOp::JUMP: {
                emit("    j " + instr.src2);
                break;
            }

            case TacOp::CALL: {
                // Save any stack variables that are in t registers
                for (auto& p : var_reg_) {
                    store_to_var(p.first, p.second);
                }
                emit("    call " + instr.src1);
                // Track call result in a0
                if (!instr.dest.empty() && is_temp_var(instr.dest)) {
                    var_reg_[instr.dest] = "a0";
                }
                break;
            }

            case TacOp::MOVE: {
                if (instr.dest == "a0") {
                    load_to_reg(instr.src1, "a0");
                } else {
                    load_to_reg(instr.src1, "t0");
                    store_to_var(instr.dest, "t0");
                    // Don't track stack variables in t0
                }
                break;
            }

            case TacOp::RET: {
                break;
            }

            case TacOp::PARAM: {
                int param_idx = std::stoi(instr.dest.substr(1));
                load_to_reg(instr.src1, "a" + std::to_string(param_idx));
                break;
            }

            default:
                break;
        }
    }
};

#endif // RISCV32_H
