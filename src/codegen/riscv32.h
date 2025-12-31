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
    int stack_size_ = 0;
    std::unordered_map<std::string, int> var_stack_offset_;

    void emit(const std::string& line) {
        output_ += line + "\n";
    }

    void generate_function(FunctionIR* func) {
        func_counter_++;
        stack_size_ = 0;
        var_stack_offset_.clear();

        // First pass: count stack slots needed for all variables and temps
        for (auto& instr : func->instrs) {
            // User variables (dest of STORE)
            if (instr.op == TacOp::STORE) {
                if (!is_temp_var(instr.dest) && var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                    var_stack_offset_[instr.dest] = stack_size_;
                    stack_size_ += 4;
                }
            }
            // Temp variables (dest of most ops)
            if (is_temp_var(instr.dest) && var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                var_stack_offset_[instr.dest] = stack_size_;
                stack_size_ += 4;
            }
        }

        // Header
        emit(".text");
        emit("");
        emit(".globl " + func->name);
        emit(func->name + ":");

        // Prologue
        bool needs_frame = (stack_size_ > 0 || func->is_void == false);
        if (needs_frame) {
            int frame_size = (stack_size_ > 0) ? stack_size_ : 4;
            frame_size = ((frame_size + 15) / 16) * 16;
            emit("    addi sp, sp, -" + std::to_string(frame_size));
            emit("    sw ra, 0(sp)");
        }

        // Generate body
        std::string entry_label = ".Lentry_" + func->name + "_" + std::to_string(func_counter_);
        emit(entry_label + ":");

        generate_instrs(func);

        // Epilogue
        std::string ret_label = "ret_" + func->name;
        emit(ret_label + ":");
        if (needs_frame) {
            int frame_size = (stack_size_ > 0) ? stack_size_ : 4;
            frame_size = ((frame_size + 15) / 16) * 16;
            emit("    lw ra, 0(sp)");
            emit("    addi sp, sp, " + std::to_string(frame_size));
        }
        emit("    ret");
    }

    bool is_temp_var(const std::string& var) {
        return !var.empty() && var[0] == '.';
    }

    bool is_label(const std::string& s) {
        if (s.empty()) return false;
        return s.substr(0, 2) == ".L" ||
               s.substr(0, 5) == "label" ||
               s.substr(0, 3) == "ret" ||
               s.substr(0, 8) == "prologue" ||
               s.substr(0, 5) == "break" ||
               s.substr(0, 9) == "continue" ||
               s.substr(0, 7) == ".Lentry";
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

    // Get stack offset for a variable
    int get_stack_offset(const std::string& var) {
        auto it = var_stack_offset_.find(var);
        if (it != var_stack_offset_.end()) {
            return it->second;
        }
        return -1;
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
                emit("    li t0, " + instr.src1);
                // Store to stack
                int offset = get_stack_offset(instr.dest);
                if (offset >= 0) {
                    emit("    sw t0, " + std::to_string(offset) + "(sp)");
                }
                break;
            }

            case TacOp::ADD: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    add t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::SUB: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    sub t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::MUL: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    mul t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::DIV: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    div t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::MOD: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    rem t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::LT: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    slt t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::GT: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    sgt t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::LE: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    slt t2, t1, t0");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::GE: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    slt t2, t0, t1");
                emit("    xori t2, t2, 1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::EQ: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    sub t2, t0, t1");
                emit("    seqz t2, t2");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::NE: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    sub t2, t0, t1");
                emit("    snez t2, t2");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::AND: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    and t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::OR: {
                load_src(instr.src1, "t0");
                load_src_preserve(instr.src2, "t1", "t0");
                emit("    or t2, t0, t1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::NOT: {
                load_src(instr.src1, "t0");
                emit("    xori t2, t0, 1");
                store_dest(instr.dest, "t2");
                break;
            }

            case TacOp::LOAD: {
                // Load from src variable into dest temp
                // Both have pre-allocated stack slots
                int src_offset = get_stack_offset(instr.src1);
                int dest_offset = get_stack_offset(instr.dest);
                if (src_offset >= 0) {
                    emit("    lw t0, " + std::to_string(src_offset) + "(sp)");
                } else {
                    emit("    lw t0, 0(sp)");
                }
                if (dest_offset >= 0) {
                    emit("    sw t0, " + std::to_string(dest_offset) + "(sp)");
                }
                break;
            }

            case TacOp::LOAD_PARAM: {
                emit("    mv t0, " + instr.src1);
                store_dest(instr.dest, "t0");
                break;
            }

            case TacOp::STORE: {
                load_src(instr.src1, "t0");
                store_dest(instr.dest, "t0");
                break;
            }

            case TacOp::BEQZ: {
                load_src(instr.src1, "t0");
                emit("    beqz t0, " + instr.src2);
                break;
            }

            case TacOp::BNEZ: {
                load_src(instr.src1, "t0");
                emit("    bnez t0, " + instr.src2);
                break;
            }

            case TacOp::JUMP: {
                emit("    j " + instr.src2);
                break;
            }

            case TacOp::CALL: {
                emit("    call " + instr.src1);
                store_dest(instr.dest, "a0");
                break;
            }

            case TacOp::MOVE: {
                if (instr.dest == "a0") {
                    load_src(instr.src1, "a0");
                } else {
                    load_src(instr.src1, "t0");
                    store_dest(instr.dest, "t0");
                }
                break;
            }

            case TacOp::RET: {
                break;
            }

            case TacOp::PARAM: {
                int param_idx = std::stoi(instr.dest.substr(1));
                load_src(instr.src1, "a" + std::to_string(param_idx));
                break;
            }

            default:
                break;
        }
    }

    // Load source operand into register
    void load_src(const std::string& src, const std::string& reg) {
        if (is_label(src) || src.empty()) return;

        // Check if it's a number
        if (is_number(src)) {
            emit("    li " + reg + ", " + src);
            return;
        }

        // Load from stack
        int offset = get_stack_offset(src);
        if (offset >= 0) {
            emit("    lw " + reg + ", " + std::to_string(offset) + "(sp)");
        } else {
            // Default: load from stack offset 0
            emit("    lw " + reg + ", 0(sp)");
        }
    }

    // Load source operand into register, preserving another register
    void load_src_preserve(const std::string& src, const std::string& reg, const std::string& preserve_reg) {
        if (is_label(src) || src.empty()) return;

        // Check if it's a number
        if (is_number(src)) {
            // Save preserve_reg first, then load immediate
            emit("    mv t6, " + preserve_reg);
            emit("    li " + reg + ", " + src);
            emit("    mv " + preserve_reg + ", t6");
            return;
        }

        // Load from stack, preserving preserve_reg
        int offset = get_stack_offset(src);
        if (offset >= 0) {
            emit("    mv t6, " + preserve_reg);
            emit("    lw " + reg + ", " + std::to_string(offset) + "(sp)");
            emit("    mv " + preserve_reg + ", t6");
        } else {
            emit("    mv t6, " + preserve_reg);
            emit("    lw " + reg + ", 0(sp)");
            emit("    mv " + preserve_reg + ", t6");
        }
    }

    // Store register value to destination
    void store_dest(const std::string& dest, const std::string& reg) {
        if (is_label(dest) || dest.empty()) return;
        if (dest == "a0") return;  // Return value register

        int offset = get_stack_offset(dest);
        if (offset >= 0) {
            emit("    sw " + reg + ", " + std::to_string(offset) + "(sp)");
        }
    }
};

#endif // RISCV32_H
