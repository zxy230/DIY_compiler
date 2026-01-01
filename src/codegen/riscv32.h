#ifndef RISCV32_H
#define RISCV32_H

#include "ir/tac.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cctype>
#include <algorithm>

class RISC32Generator {
public:
    RISC32Generator(ProgramIR* ir) : program_ir_(ir) {}

    std::string generate() {
        output_ = "";

        // Generate code for each function
        for (auto& func : program_ir_->functions) {
            generate_function(func.get());
        }

        return output_;
    }

private:
    ProgramIR* program_ir_;
    std::string output_;
    int func_counter_ = -1;
    int stack_size_ = 0;
    std::unordered_map<std::string, int> var_stack_offset_;

    void emit(const std::string& line) {
        output_ += line + "\n";
    }

    void generate_function(FunctionIR* func) {
        func_counter_++;
        int var_stack_size = 0;  // Stack space for variables (starts at 0)
        var_stack_offset_.clear();

        // First pass: collect stack slots for user variables FIRST
        // Then collect stack slots for temp variables that are actually used
        for (auto& instr : func->instrs) {
            // User variables (dest of STORE) - allocate stack slots FIRST
            if (instr.op == TacOp::STORE) {
                if (!is_temp_var(instr.dest) && var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                    var_stack_offset_[instr.dest] = var_stack_size + 4;  // +4 for ra
                    var_stack_size += 4;
                }
            }
        }
        // Second pass: temp variables that need stack slots
        // Only allocate for temps that are:
        // 1. LOAD dest (will be used later)
        // 2. LOAD_IMM dest (will be used later)
        // 3. Results of binary operations
        for (auto& instr : func->instrs) {
            // Skip LOAD_IMM temps if they're immediately moved to a0
            if (instr.op == TacOp::LOAD_IMM) {
                // Check if this temp is immediately used in a MOVE to a0
                auto it = std::next(std::find_if(func->instrs.begin(), func->instrs.end(),
                    [&](const TacInstr& i) { return &i == &instr; }));
                if (it != func->instrs.end() && it->op == TacOp::MOVE && it->dest == "a0" && it->src1 == instr.dest) {
                    // This temp is only used for MOVE to a0, don't allocate stack slot
                    continue;
                }
            }

            if (instr.op == TacOp::LOAD && is_temp_var(instr.dest) &&
                var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                var_stack_offset_[instr.dest] = var_stack_size + 4;  // +4 for ra
                var_stack_size += 4;
            }
            if (instr.op == TacOp::LOAD_IMM && is_temp_var(instr.dest) &&
                var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                var_stack_offset_[instr.dest] = var_stack_size + 4;  // +4 for ra
                var_stack_size += 4;
            }
            // Allocate stack slots for binary operation results (ADD, SUB, etc.)
            if ((instr.op == TacOp::ADD || instr.op == TacOp::SUB || instr.op == TacOp::MUL ||
                 instr.op == TacOp::DIV || instr.op == TacOp::MOD || instr.op == TacOp::LT ||
                 instr.op == TacOp::GT || instr.op == TacOp::LE || instr.op == TacOp::GE ||
                 instr.op == TacOp::EQ || instr.op == TacOp::NE || instr.op == TacOp::AND ||
                 instr.op == TacOp::OR) &&
                is_temp_var(instr.dest) &&
                var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                var_stack_offset_[instr.dest] = var_stack_size + 4;  // +4 for ra
                var_stack_size += 4;
            }
            // Allocate stack slots for CALL results
            if (instr.op == TacOp::CALL && is_temp_var(instr.dest) &&
                var_stack_offset_.find(instr.dest) == var_stack_offset_.end()) {
                var_stack_offset_[instr.dest] = var_stack_size + 4;  // +4 for ra
                var_stack_size += 4;
            }
        }

        // Stack size includes space for ra (4 bytes) + variable space
        // Only add ra space if we actually have variables
        stack_size_ = var_stack_size > 0 ? var_stack_size + 4 : 0;

        // Header
        emit(".text");
        emit("");
        emit(".globl " + func->name);
        emit(func->name + ":");

        // Prologue label
        emit("prologue_" + func->name + ":");

        // Prologue - only if we need stack space for user variables
        bool needs_frame = (stack_size_ > 0);
        if (needs_frame) {
            int frame_size = ((stack_size_ + 15) / 16) * 16;
            if (frame_size < 16) frame_size = 16;
            emit("    addi sp, sp, -" + std::to_string(frame_size));
            emit("    sw ra, 0(sp)");
        }

        // Generate body - use simple label
        emit("label" + std::to_string(func_counter_) + ":");

        for (auto& instr : func->instrs) {
            generate_instr(instr);
        }

        // Epilogue - only if we created a frame
        if (needs_frame) {
            int frame_size = ((stack_size_ + 15) / 16) * 16;
            if (frame_size < 16) frame_size = 16;
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
               s.substr(0, 9) == "continue";
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

    void generate_instr(const TacInstr& instr) {
        switch (instr.op) {
            case TacOp::LABEL:
                emit(instr.src2 + ":");
                break;

            case TacOp::LOAD_IMM: {
                emit("    li t0, " + instr.src1);
                // Store to stack slot if temp has one
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
                // Load from src into t0
                int src_offset = get_stack_offset(instr.src1);
                if (src_offset >= 0) {
                    emit("    lw t0, " + std::to_string(src_offset) + "(sp)");
                }
                // If src_offset < 0, the value is already in t0 (temp var case)
                // Store to dest's stack slot if it has one
                int dest_offset = get_stack_offset(instr.dest);
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
                    if (is_number(instr.src1)) {
                        emit("    li a0, " + instr.src1);
                    } else {
                        // Load src1 into a0 (may need to load from stack for temps)
                        load_src(instr.src1, "a0");
                    }
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

        // For temp variables
        if (is_temp_var(src)) {
            int offset = get_stack_offset(src);
            if (offset >= 0) {
                // Temp has a stack slot - load from stack
                emit("    lw " + reg + ", " + std::to_string(offset) + "(sp)");
            } else {
                // Temp has no stack slot - value is in t0, just move it
                if (reg != "t0") {
                    emit("    mv " + reg + ", t0");
                }
            }
            return;
        }

        // Load from stack for user variables
        int offset = get_stack_offset(src);
        if (offset >= 0) {
            emit("    lw " + reg + ", " + std::to_string(offset) + "(sp)");
        }
    }

    // Load source operand into register, preserving another register
    void load_src_preserve(const std::string& src, const std::string& reg, const std::string& preserve_reg) {
        if (is_label(src) || src.empty()) return;

        // Check if it's a number - no need to preserve when loading immediate
        if (is_number(src)) {
            emit("    li " + reg + ", " + src);
            return;
        }

        // For temp variables without stack slots (like results of LOAD_IMM),
        // the value is in t0, just move it if needed
        if (is_temp_var(src)) {
            int offset = get_stack_offset(src);
            if (offset < 0) {
                // Temp has no stack slot - value is in t0
                if (reg != "t0") {
                    emit("    mv " + reg + ", t0");
                }
                return;
            }
            // Temp has a stack slot - load from stack with preservation
            emit("    mv t6, " + preserve_reg);
            emit("    lw " + reg + ", " + std::to_string(offset) + "(sp)");
            emit("    mv " + preserve_reg + ", t6");
            return;
        }

        // Load from stack for user variables, preserving preserve_reg
        int offset = get_stack_offset(src);
        emit("    mv t6, " + preserve_reg);
        if (offset >= 0) {
            emit("    lw " + reg + ", " + std::to_string(offset) + "(sp)");
        }
        emit("    mv " + preserve_reg + ", t6");
    }

    // Store register value to destination
    void store_dest(const std::string& dest, const std::string& reg) {
        if (is_label(dest) || dest.empty()) return;
        // Note: we don't skip "a0" anymore - the caller should handle that if needed

        int offset = get_stack_offset(dest);
        if (offset >= 0) {
            emit("    sw " + reg + ", " + std::to_string(offset) + "(sp)");
        }
    }
};

#endif // RISCV32_H
