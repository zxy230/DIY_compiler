#ifndef RISCV32_H
#define RISCV32_H

#include "ir/tac.h"
#include <string>
#include <vector>
#include <unordered_map>

class RISC32Generator {
public:
    RISC32Generator(ProgramIR* ir) : program_ir_(ir) {}

    std::string generate() {
        output_ = "";

        // Generate each function
        for (auto& func : program_ir_->functions) {
            generate_function(func.get());
        }

        return output_;
    }

private:
    ProgramIR* program_ir_;
    std::string output_;
    std::unordered_map<std::string, std::string> var_to_reg_;
    std::vector<std::string> free_regs_ = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};
    int stack_offset_ = 0;
    int temp_counter_ = 0;

    void emit(const std::string& line) {
        output_ += line + "\n";
    }

    void generate_function(FunctionIR* func) {
        emit("");
        emit(func->name + ":");

        // Prologue
        emit("    # Prologue");
        emit("    addi sp, sp, -" + std::to_string(calculate_frame_size(func)));
        emit("    sw ra, " + std::to_string(0) + "(sp)");

        // Save caller-saved registers we'll use
        emit("    # Save registers");

        // Generate body
        generate_instrs(func);

        // Epilogue
        emit("    # Epilogue");
        emit("    lw ra, " + std::to_string(0) + "(sp)");
        emit("    addi sp, sp, " + std::to_string(calculate_frame_size(func)));
        emit("    ret");
    }

    int calculate_frame_size(FunctionIR* func) {
        // Estimate: ra (4) + locals + spill area
        // For simplicity, return 32 (8 words) as minimum
        return 32;
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

            case TacOp::LOAD_IMM:
                emit("    li t0, " + instr.src1);
                emit("    sw t0, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::ADD:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    add t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::SUB:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    sub t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::MUL:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    mul t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::DIV:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    div t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::MOD:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    rem t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::LT:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    slt t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::GT:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    sgt t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::LE:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    neg t2, t0");  // -t0
                emit("    slt t2, t1, t2");  // t1 < -t0  => t1 <= t0
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::GE:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    slt t2, t0, t1");  // t0 < t1 => t0 >= t1 is false
                emit("    xori t2, t2, 1");  // negate
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::EQ:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    sub t2, t0, t1");
                emit("    seqz t2, t2");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::NE:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    sub t2, t0, t1");
                emit("    snez t2, t2");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::AND:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    and t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::OR:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    lw t1, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    or t2, t0, t1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::NOT:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    xori t2, t0, 1");
                emit("    sw t2, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::LOAD:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    sw t0, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::STORE:
                emit("    lw t0, " + get_stack_offset(instr.src2) + "(sp)");
                emit("    sw t0, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::BEQZ:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    beqz t0, " + instr.src2);
                break;

            case TacOp::BNEZ:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    bnez t0, " + instr.src2);
                break;

            case TacOp::JUMP:
                emit("    j " + instr.src2);
                break;

            case TacOp::CALL:
                emit("    call " + instr.src1);
                emit("    sw a0, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::MOVE:
                emit("    lw t0, " + get_stack_offset(instr.src1) + "(sp)");
                emit("    sw t0, " + get_stack_offset(instr.dest) + "(sp)");
                break;

            case TacOp::RET:
                emit("    # Return");
                break;

            case TacOp::PARAM:
                // Handled separately during call
                break;

            default:
                emit("    # Unknown op: " + std::to_string((int)instr.op));
                break;
        }
    }

    std::string get_stack_offset(const std::string& var) {
        // Simple: all variables at fixed offsets
        // In a real implementation, we'd track var offsets properly
        if (var == "a0" || var == "a1" || var == "a2" || var == "a3") {
            return "0";  // Parameters at known offsets
        }
        // For temps and locals, use a simple hash
        static int offset = 4;
        return std::to_string(offset);
    }
};

#endif // RISCV32_H
