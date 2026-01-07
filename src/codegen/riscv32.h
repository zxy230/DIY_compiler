#ifndef RISCV32_H
#define RISCV32_H

#include "ir/tac.h"
#include "codegen/allocator.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cctype>
#include <algorithm>
#include <sstream>

class RISC32Generator {
public:
    RISC32Generator(ProgramIR* ir) : program_ir_(ir) {}

    std::string generate() {
        output_ = "";

        // Generate code for each function
        for (auto& func : program_ir_->functions) {
            generate_function(func.get());
        }

        // Apply peephole optimizations to remove redundant instructions
        output_ = optimize_peephole_simple(output_);

        return output_;
    }

private:
    ProgramIR* program_ir_;
    std::string output_;

    void generate_function(FunctionIR* func) {
        // Skip internal functions starting with '.'
        if (!func->name.empty() && func->name[0] == '.') {
            return;
        }

        output_ += "\t.globl " + func->name + "\n";
        output_ += func->name + ":\n";

        // Generate prologue
        output_ += "prologue_" + func->name + ":\n";
        // Use s0 as frame pointer
        output_ += "\taddi s0, sp, 0\n";
        output_ += "\taddi sp, sp, -4\n";
        output_ += "\tsw ra, 0(sp)\n";

        // Allocate register and generate code
        LinearScanAllocator allocator(func);
        allocator.allocate();

        // Generate TAC instructions
        for (size_t i = 0; i < func->instrs.size(); i++) {
            const auto& instr = func->instrs[i];
            const auto& alloc = allocator.get_allocation(i);

            generate_instruction(instr, alloc);
        }

        // Generate epilogue
        output_ += "epilogue_" + func->name + ":\n";
        output_ += "\tlw ra, 0(sp)\n";
        output_ += "\taddi sp, sp, 4\n";
        output_ += "\tret\n";
    }

    void generate_instruction(const TacInstr& instr, const LinearScanAllocator::InstrAlloc& alloc) {
        // Handle labels
        if (instr.op == TacOp::LABEL) {
            output_ += instr.src2 + ":\n";
            return;
        }

        // Get register allocations (using const-safe methods)
        auto dest_it = alloc.reg_map.find(instr.dest);
        std::string dest_reg = dest_it != alloc.reg_map.end() ? dest_it->second : "";

        auto src1_it = alloc.reg_map.find(instr.src1);
        std::string src1_reg = src1_it != alloc.reg_map.end() ? src1_it->second : "";

        auto src2_it = alloc.reg_map.find(instr.src2);
        std::string src2_reg = src2_it != alloc.reg_map.end() ? src2_it->second : "";

        // Handle spilled variables
        auto dest_offset_it = alloc.spill_offset.find(instr.dest);
        int dest_offset = dest_offset_it != alloc.spill_offset.end() ? dest_offset_it->second : -1;

        auto src1_offset_it = alloc.spill_offset.find(instr.src1);
        int src1_offset = src1_offset_it != alloc.spill_offset.end() ? src1_offset_it->second : -1;

        switch (instr.op) {
            case TacOp::LOAD_IMM:
                if (is_number(instr.src1)) {
                    output_ += "\tli " + dest_reg + ", " + instr.src1 + "\n";
                } else if (src1_offset >= 0) {
                    output_ += "\tlw " + dest_reg + ", " + std::to_string(src1_offset) + "(s0)\n";
                } else {
                    output_ += "\taddi " + dest_reg + ", " + src1_reg + ", 0\n";
                }
                break;

            case TacOp::ADD:
                if (is_number(instr.src2)) {
                    output_ += "\taddi " + dest_reg + ", " + src1_reg + ", " + instr.src2 + "\n";
                } else {
                    output_ += "\tadd " + dest_reg + ", " + src1_reg + ", " + src2_reg + "\n";
                }
                break;

            case TacOp::SUB:
                if (is_number(instr.src2)) {
                    output_ += "\taddi " + dest_reg + ", " + src1_reg + ", -" + instr.src2 + "\n";
                } else {
                    output_ += "\tsub " + dest_reg + ", " + src1_reg + ", " + src2_reg + "\n";
                }
                break;

            case TacOp::MUL:
                output_ += "\tmul " + dest_reg + ", " + src1_reg + ", " + src2_reg + "\n";
                break;

            case TacOp::DIV:
                output_ += "\tdiv " + dest_reg + ", " + src1_reg + ", " + src2_reg + "\n";
                break;

            case TacOp::MOD:
                output_ += "\trem " + dest_reg + ", " + src1_reg + ", " + src2_reg + "\n";
                break;

            case TacOp::EQ:
            case TacOp::NE:
            case TacOp::LT:
            case TacOp::GT:
            case TacOp::LE:
            case TacOp::GE: {
                // Compare and set dest to 0 or 1
                std::string cmpInstr = "";
                std::string resultInstr = "";  // Instruction to convert result (seqz/snez)
                bool isImm = is_number(instr.src2);

                if (instr.op == TacOp::EQ) {
                    cmpInstr = isImm ? "xori" : "sub";
                }
                else if (instr.op == TacOp::NE) {
                    cmpInstr = isImm ? "xori" : "sub";
                    resultInstr = "seqz";  // NE: result is 1 if xori result is 0
                }
                else if (instr.op == TacOp::LT) {
                    cmpInstr = "slt";
                }
                else if (instr.op == TacOp::GE) {
                    cmpInstr = "slt";
                    resultInstr = "seqz";  // GE: result is 1 if slt result is 0
                }
                else if (instr.op == TacOp::LE) {
                    cmpInstr = "slt";
                    std::swap(src1_reg, src2_reg);  // LE: a <= b is b < a
                }
                else if (instr.op == TacOp::GT) {
                    cmpInstr = "slt";
                    std::swap(src1_reg, src2_reg);  // GT: a > b is b < a
                    resultInstr = "seqz";  // GT: result is 1 if slt result is 0
                }

                if (isImm && (instr.op == TacOp::EQ || instr.op == TacOp::NE)) {
                    // For EQ/NE with immediate, use xori
                    output_ += "\t" + cmpInstr + " " + dest_reg + ", " + src1_reg + ", " + instr.src2 + "\n";
                    if (!resultInstr.empty()) {
                        output_ += "\t" + resultInstr + " " + dest_reg + ", " + dest_reg + "\n";
                    }
                } else if (!resultInstr.empty()) {
                    // For comparisons that need result inversion (NE, GT, GE with registers)
                    output_ += "\t" + cmpInstr + " " + dest_reg + ", " + src1_reg + ", " + src2_reg + "\n";
                    output_ += "\t" + resultInstr + " " + dest_reg + ", " + dest_reg + "\n";
                } else {
                    // LT, LE, EQ with registers - no inversion needed
                    output_ += "\t" + cmpInstr + " " + dest_reg + ", " + src1_reg + ", " + src2_reg + "\n";
                }
                break;
            }

            case TacOp::LOAD:
                if (src1_offset >= 0) {
                    output_ += "\tlw " + dest_reg + ", " + std::to_string(src1_offset) + "(s0)\n";
                }
                break;

            case TacOp::LOAD_PARAM:
                // Load parameter from argument register (a0-a7) to destination
                // src1 = register name (e.g., "a0", "a1"), dest = temp variable
                if (!instr.src1.empty()) {
                    output_ += "\taddi " + dest_reg + ", " + instr.src1 + ", 0\n";
                }
                break;

            case TacOp::STORE:
                if (dest_offset >= 0) {
                    if (is_number(instr.src1)) {
                        output_ += "\tli t0, " + instr.src1 + "\n";
                        output_ += "\tsw t0, " + std::to_string(dest_offset) + "(s0)\n";
                    } else if (src1_offset >= 0) {
                        output_ += "\tlw t0, " + std::to_string(src1_offset) + "(s0)\n";
                        output_ += "\tsw t0, " + std::to_string(dest_offset) + "(s0)\n";
                    } else {
                        output_ += "\tsw " + src1_reg + ", " + std::to_string(dest_offset) + "(s0)\n";
                    }
                }
                break;

            case TacOp::BEQZ:
            case TacOp::BNEZ: {
                std::string condReg;
                if (is_number(instr.src1)) {
                    output_ += "\tli t0, " + instr.src1 + "\n";
                    condReg = "t0";
                } else if (src1_offset >= 0) {
                    output_ += "\tlw t0, " + std::to_string(src1_offset) + "(s0)\n";
                    condReg = "t0";
                } else {
                    condReg = src1_reg;
                }

                std::string jumpLabel = instr.src2;  // Label is in src2 for BEQZ/BNEZ
                if (instr.op == TacOp::BEQZ) {
                    output_ += "\tbeqz " + condReg + ", " + jumpLabel + "\n";
                } else {
                    output_ += "\tbnez " + condReg + ", " + jumpLabel + "\n";
                }
                break;
            }

            case TacOp::JUMP:
                output_ += "\tj " + instr.src2 + "\n";  // Label is in src2 for JUMP
                break;

            case TacOp::RET:
                if (!instr.src1.empty()) {
                    if (is_number(instr.src1)) {
                        output_ += "\tli a0, " + instr.src1 + "\n";
                    } else {
                        auto src1_offset_it = alloc.spill_offset.find(instr.src1);
                        int src1_offset = src1_offset_it != alloc.spill_offset.end() ? src1_offset_it->second : -1;
                        if (src1_offset >= 0) {
                            output_ += "\tlw a0, " + std::to_string(src1_offset) + "(s0)\n";
                        } else {
                            auto it = alloc.reg_map.find(instr.src1);
                            std::string src_reg = it != alloc.reg_map.end() ? it->second : "";
                            output_ += "\taddi a0, " + src_reg + ", 0\n";
                        }
                    }
                }
                break;

            case TacOp::PHI:
                // PHI nodes are handled during SSA transformation
                break;

            case TacOp::MOVE: {
                // Move src1 to dest
                // dest = destination register (e.g., "a0" for return value)
                // src1 = source value
                if (is_number(instr.src1)) {
                    output_ += "\tli " + instr.dest + ", " + instr.src1 + "\n";
                } else {
                    auto it = alloc.reg_map.find(instr.src1);
                    std::string src_reg = it != alloc.reg_map.end() ? it->second : "";
                    output_ += "\taddi " + instr.dest + ", " + src_reg + ", 0\n";
                }
                break;
            }

            case TacOp::PARAM: {
                // Load argument into specified register (a0, a1, etc.)
                // dest = register name (a0, a1, etc.)
                // src1 = argument value
                if (is_number(instr.src1)) {
                    output_ += "\tli " + instr.dest + ", " + instr.src1 + "\n";
                } else {
                    auto it = alloc.reg_map.find(instr.src1);
                    std::string src_reg = it != alloc.reg_map.end() ? it->second : "";
                    output_ += "\taddi " + instr.dest + ", " + src_reg + ", 0\n";
                }
                break;
            }

            case TacOp::CALL: {
                // Function name is in src1 for CALL
                // Arguments are already loaded via PARAM instructions
                output_ += "\tcall " + instr.src1 + "\n";

                // Move return value to destination
                if (!instr.dest.empty()) {
                    if (dest_offset >= 0) {
                        output_ += "\tsw a0, " + std::to_string(dest_offset) + "(s0)\n";
                    } else {
                        output_ += "\taddi " + dest_reg + ", a0, 0\n";
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    bool is_number(const std::string& s) const {
        if (s.empty()) return false;
        size_t i = 0;
        if (s[0] == '-') {
            if (s.length() == 1) return false;
            i = 1;
        }
        for (; i < s.size(); i++) {
            if (!isdigit(s[i])) return false;
        }
        return !s.empty() && (s[0] != '-' || s.size() > 1);
    }

    // Simple peephole optimizer - removes redundant mv/addi instructions
    std::string optimize_peephole_simple(const std::string& code) {
        std::vector<std::string> lines;
        std::stringstream ss(code);
        std::string line;
        while (std::getline(ss, line)) {
            lines.push_back(line);
        }

        // Track register equivalence: reg_equiv[reg] = parent_reg
        // If reg_equiv[reg] = parent, it means reg is an alias for parent
        std::unordered_map<std::string, std::string> reg_equiv;

        // Track value origin: value_origin[reg] = original_reg_that_holds_the_value
        // Keyed by the ORIGINAL register name, not the alias
        std::unordered_map<std::string, std::string> value_origin;

        // Find the ultimate source of a register (follow alias chain)
        auto find_alias = [&](const std::string& reg, auto&& find_ref) -> std::string {
            if (reg_equiv.find(reg) == reg_equiv.end()) {
                return reg;
            }
            std::string src = find_ref(reg_equiv[reg], find_ref);
            reg_equiv[reg] = src;  // Path compression
            return src;
        };

        // Find the original source of a register's value
        // Follows alias chain to find the ultimate register, then looks up its origin
        auto find_origin = [&](const std::string& reg) -> std::string {
            // First, find the ultimate alias by following the chain without path compression
            std::string alias = reg;
            std::string current = reg;
            while (reg_equiv.find(current) != reg_equiv.end()) {
                current = reg_equiv[current];
            }
            alias = current;
            // Look up origin using the alias
            if (value_origin.find(alias) != value_origin.end()) {
                return value_origin[alias];
            }
            // If no origin, return the final alias
            return alias;
        };

        // Make dest an alias of src
        auto make_alias = [&](const std::string& dest, const std::string& src) {
            std::string dest_alias = find_alias(dest, find_alias);
            std::string src_alias = find_alias(src, find_alias);

            if (dest_alias != src_alias) {
                // Different aliases: link dest_alias to src_alias
                reg_equiv[dest_alias] = src_alias;
            }
            // Set origin for the ORIGINAL dest to track where its value came from
            if (value_origin.find(dest) == value_origin.end() || dest_alias != dest) {
                // Find the ultimate alias of src WITHOUT path compression
                std::string src_origin = src;
                std::string current = src;
                while (reg_equiv.find(current) != reg_equiv.end()) {
                    current = reg_equiv[current];
                }
                src_origin = current;
                // But if src has an origin, use that instead
                if (value_origin.find(src) != value_origin.end()) {
                    src_origin = value_origin[src];
                }
                value_origin[dest] = src_origin;
            }
        };

        // Set a register's value origin (called for li, arithmetic, etc.)
        auto set_origin = [&](const std::string& reg, const std::string& origin) {
            value_origin[reg] = origin;
        };

        // First pass: build equivalence and origin tracking
        for (size_t i = 0; i < lines.size(); i++) {
            std::string& current = lines[i];

            // Skip labels and empty lines (after trimming whitespace)
            std::string trimmed = current;
            trimmed.erase(0, trimmed.find_first_not_of(" \t"));
            if (trimmed.empty() || trimmed.back() == ':') {
                continue;
            }

            std::string instr;
            std::stringstream ss2(current);
            ss2 >> instr;

            if (instr == "li") {
                std::string reg, imm;
                ss2 >> reg >> imm;
                reg.erase(std::remove(reg.begin(), reg.end(), ','), reg.end());
                set_origin(reg, reg);
            }
            else if (instr == "add" || instr == "sub" || instr == "mul" ||
                     instr == "div" || instr == "rem" || instr == "slt" ||
                     instr == "sgt" || instr == "and" || instr == "or") {
                std::string dest;
                ss2 >> dest;
                dest.erase(std::remove(dest.begin(), dest.end(), ','), dest.end());
                reg_equiv.erase(dest);
                set_origin(dest, dest);
            }
            else if (instr == "addi") {
                std::string dest, src, imm;
                ss2 >> dest >> src >> imm;
                dest.erase(std::remove(dest.begin(), dest.end(), ','), dest.end());
                src.erase(std::remove(src.begin(), src.end(), ','), src.end());
                imm.erase(std::remove(imm.begin(), imm.end(), ','), imm.end());

                if (imm == "0" && dest != src) {
                    make_alias(dest, src);
                } else {
                    reg_equiv.erase(dest);
                    set_origin(dest, dest);
                }
            }
            else if (instr == "xori" || instr == "seqz" || instr == "snez" ||
                     instr == "lui" || instr == "lw") {
                std::string reg;
                ss2 >> reg;
                reg.erase(std::remove(reg.begin(), reg.end(), ','), reg.end());
                reg_equiv.erase(reg);
                set_origin(reg, reg);
            }
        }

        // Second pass: remove redundant instructions based on origin tracking
        for (size_t i = 0; i < lines.size(); i++) {
            std::string& current = lines[i];
            std::string trimmed = current;
            trimmed.erase(0, trimmed.find_first_not_of(" \t"));
            if (trimmed.empty() || trimmed.back() == ':') continue;

            std::stringstream ss2(current);
            std::string instr;
            ss2 >> instr;

            if (instr == "addi") {
                std::string dest, src, imm;
                ss2 >> dest >> src >> imm;
                dest.erase(std::remove(dest.begin(), dest.end(), ','), dest.end());
                src.erase(std::remove(src.begin(), src.end(), ','), src.end());
                imm.erase(std::remove(imm.begin(), imm.end(), ','), imm.end());

                if (imm == "0") {
                    // Don't remove addi to a0 - it's needed for return value
                    if (dest == "a0") {
                        continue;
                    }
                    std::string origin_dest = find_origin(dest);
                    std::string origin_src = find_origin(src);

                    if (origin_dest == origin_src) {
                        lines[i] = "";
                    }
                }
            }
        }

        // Remove empty lines
        std::vector<std::string> result;
        for (const auto& l : lines) {
            if (!l.empty()) {
                result.push_back(l);
            }
        }

        // Reconstruct
        std::string output;
        for (const auto& l : result) {
            output += l + "\n";
        }

        return output;
    }
};

#endif // RISCV32_H
