#include "optimizer/optimizer.h"
#include <algorithm>
#include <cctype>
#include <unordered_set>

// Check if a string is a number
static bool is_number(const std::string& s) {
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

// Convert string to long long
static long long to_longlong(const std::string& s) {
    return std::stoll(s);
}

// Convert long long to string
static std::string to_string_ll(long long v) {
    return std::to_string(v);
}

// Check if a variable name (temporary)
static bool is_temp_var(const std::string& var) {
    return !var.empty() && var[0] == '.';
}

void Optimizer::optimize(ProgramIR* program) {
    // Run optimizations in order
    constant_propagation(program);
    constant_folding(program);
    algebraic_simplification(program);
    dead_code_elimination(program);
}

void Optimizer::process_function(FunctionIR* func) {
    // Placeholder for function-level processing
    (void)func;
}

void Optimizer::constant_propagation(ProgramIR* program) {
    for (auto& func : program->functions) {
        std::unordered_map<std::string, long long> constants;

        for (auto& instr : func->instrs) {
            switch (instr.op) {
                case TacOp::LOAD_IMM: {
                    // LOAD_IMM dest, value -> dest = value
                    if (is_number(instr.src1)) {
                        constants[instr.dest] = to_longlong(instr.src1);
                    }
                    break;
                }

                case TacOp::ADD:
                case TacOp::SUB:
                case TacOp::MUL:
                case TacOp::DIV:
                case TacOp::MOD: {
                    // Binary ops: dest = src1 op src2
                    // Try to replace with constants
                    long long const1 = 0, const2 = 0;
                    bool has1 = is_number(instr.src1) ||
                               (constants.find(instr.src1) != constants.end() &&
                                !is_temp_var(instr.src1));
                    bool has2 = is_number(instr.src2) ||
                               (constants.find(instr.src2) != constants.end() &&
                                !is_temp_var(instr.src2));

                    if (has1) {
                        if (is_number(instr.src1)) const1 = to_longlong(instr.src1);
                        else if (constants.find(instr.src1) != constants.end())
                            const1 = constants[instr.src1];
                    }
                    if (has2) {
                        if (is_number(instr.src2)) const2 = to_longlong(instr.src2);
                        else if (constants.find(instr.src2) != constants.end())
                            const2 = constants[instr.src2];
                    }

                    if (has1 && has2) {
                        // Both operands are constants, can fold
                        long long result = 0;
                        switch (instr.op) {
                            case TacOp::ADD: result = const1 + const2; break;
                            case TacOp::SUB: result = const1 - const2; break;
                            case TacOp::MUL: result = const1 * const2; break;
                            case TacOp::DIV: result = const1 / const2; break;
                            case TacOp::MOD: result = const1 % const2; break;
                            default: break;
                        }
                        // Replace with LOAD_IMM
                        instr.op = TacOp::LOAD_IMM;
                        instr.src1 = to_string_ll(result);
                        instr.src2 = "";
                        constants[instr.dest] = result;
                    } else if (has1 && is_number(instr.src1)) {
                        // First operand is a constant number
                        // Keep as is for now
                    } else if (has2 && is_number(instr.src2)) {
                        // Second operand is a constant number
                        // Keep as is for now
                    }
                    break;
                }

                case TacOp::STORE: {
                    // STORE dest, src -> dest = src
                    // If src is a constant, update the map
                    if (constants.find(instr.src1) != constants.end()) {
                        constants[instr.dest] = constants[instr.src1];
                    } else if (is_number(instr.src1)) {
                        constants[instr.dest] = to_longlong(instr.src1);
                    }
                    break;
                }

                case TacOp::MOVE: {
                    // MOVE dest, src -> dest = src
                    if (constants.find(instr.src1) != constants.end()) {
                        constants[instr.dest] = constants[instr.src1];
                    } else if (is_number(instr.src1)) {
                        constants[instr.dest] = to_longlong(instr.src1);
                    }
                    break;
                }

                case TacOp::LOAD: {
                    // LOAD dest, src -> dest = *src (we don't track pointer values)
                    break;
                }

                default:
                    break;
            }
        }
    }
}

void Optimizer::constant_folding(ProgramIR* program) {
    for (auto& func : program->functions) {
        std::unordered_map<std::string, long long> const_values;

        for (auto& instr : func->instrs) {
            // First, propagate known constant values
            if (instr.op == TacOp::LOAD_IMM && is_number(instr.src1)) {
                const_values[instr.dest] = to_longlong(instr.src1);
            }

            // Try to fold the current instruction
            try_fold_instruction(instr, const_values);
        }
    }
}

bool Optimizer::try_fold_instruction(TacInstr& instr,
                                      const std::unordered_map<std::string, long long>& constants) {
    long long result = 0;
    bool can_fold = false;

    switch (instr.op) {
        case TacOp::ADD: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
            }
            if (has2) {
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
            }

            if (has1 && has2) {
                result = v1 + v2;
                can_fold = true;
            }
            break;
        }

        case TacOp::SUB: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 - v2;
                can_fold = true;
            }
            break;
        }

        case TacOp::MUL: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 * v2;
                can_fold = true;
            }
            break;
        }

        case TacOp::DIV: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                if (v2 != 0) {
                    result = v1 / v2;
                    can_fold = true;
                }
            }
            break;
        }

        case TacOp::MOD: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                if (v2 != 0) {
                    result = v1 % v2;
                    can_fold = true;
                }
            }
            break;
        }

        case TacOp::NOT: {
            long long v = 0;
            bool has = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            if (has) {
                if (is_number(instr.src1)) v = to_longlong(instr.src1);
                else v = constants.at(instr.src1);
                result = v == 0 ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::LT: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 < v2 ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::GT: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 > v2 ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::LE: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 <= v2 ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::GE: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 >= v2 ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::EQ: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 == v2 ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::NE: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = v1 != v2 ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::AND: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = (v1 != 0 && v2 != 0) ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        case TacOp::OR: {
            long long v1 = 0, v2 = 0;
            bool has1 = is_number(instr.src1) || constants.find(instr.src1) != constants.end();
            bool has2 = is_number(instr.src2) || constants.find(instr.src2) != constants.end();

            if (has1 && has2) {
                if (is_number(instr.src1)) v1 = to_longlong(instr.src1);
                else v1 = constants.at(instr.src1);
                if (is_number(instr.src2)) v2 = to_longlong(instr.src2);
                else v2 = constants.at(instr.src2);
                result = (v1 != 0 || v2 != 0) ? 1 : 0;
                can_fold = true;
            }
            break;
        }

        default:
            break;
    }

    if (can_fold) {
        instr.op = TacOp::LOAD_IMM;
        instr.src1 = to_string_ll(result);
        instr.src2 = "";
        return true;
    }

    return false;
}

void Optimizer::algebraic_simplification(ProgramIR* program) {
    for (auto& func : program->functions) {
        for (auto& instr : func->instrs) {
            try_simplify_instruction(instr);
        }
    }
}

bool Optimizer::try_simplify_instruction(TacInstr& instr) {
    switch (instr.op) {
        // x + 0 = x
        case TacOp::ADD: {
            if (is_number(instr.src2) && to_longlong(instr.src2) == 0) {
                instr.op = TacOp::MOVE;
                instr.src2 = "";
                return true;
            }
            if (is_number(instr.src1) && to_longlong(instr.src1) == 0) {
                instr.op = TacOp::MOVE;
                instr.src1 = instr.src2;
                instr.src2 = "";
                return true;
            }
            break;
        }

        // x - 0 = x
        case TacOp::SUB: {
            if (is_number(instr.src2) && to_longlong(instr.src2) == 0) {
                instr.op = TacOp::MOVE;
                instr.src2 = "";
                return true;
            }
            // x - x = 0
            if (instr.src1 == instr.src2) {
                instr.op = TacOp::LOAD_IMM;
                instr.src1 = "0";
                instr.src2 = "";
                return true;
            }
            break;
        }

        // x * 1 = x
        case TacOp::MUL: {
            if (is_number(instr.src2)) {
                long long v = to_longlong(instr.src2);
                if (v == 1) {
                    instr.op = TacOp::MOVE;
                    instr.src2 = "";
                    return true;
                }
                if (v == 0) {
                    instr.op = TacOp::LOAD_IMM;
                    instr.src1 = "0";
                    instr.src2 = "";
                    return true;
                }
            }
            if (is_number(instr.src1)) {
                long long v = to_longlong(instr.src1);
                if (v == 1) {
                    instr.op = TacOp::MOVE;
                    instr.src1 = instr.src2;
                    instr.src2 = "";
                    return true;
                }
                if (v == 0) {
                    instr.op = TacOp::LOAD_IMM;
                    instr.src1 = "0";
                    instr.src2 = "";
                    return true;
                }
            }
            break;
        }

        // x / 1 = x
        case TacOp::DIV: {
            if (is_number(instr.src2) && to_longlong(instr.src2) == 1) {
                instr.op = TacOp::MOVE;
                instr.src2 = "";
                return true;
            }
            break;
        }

        // x % 1 = 0
        case TacOp::MOD: {
            if (is_number(instr.src2) && to_longlong(instr.src2) == 1) {
                instr.op = TacOp::LOAD_IMM;
                instr.src1 = "0";
                instr.src2 = "";
                return true;
            }
            break;
        }

        // x * 0 = 0 (already handled in MUL above)

        default:
            break;
    }

    return false;
}

void Optimizer::dead_code_elimination(ProgramIR* program) {
    for (auto& func : program->functions) {
        // Track which temporaries are used
        std::unordered_set<std::string> used_temps;

        // First pass: find all used temporaries
        for (auto& instr : func->instrs) {
            // Check src1
            if (is_temp_var(instr.src1)) {
                used_temps.insert(instr.src1);
            }
            // Check src2
            if (is_temp_var(instr.src2)) {
                used_temps.insert(instr.src2);
            }
            // For STORE, the dest (variable name) might be used later
            if (instr.op == TacOp::STORE && !is_temp_var(instr.dest)) {
                // User variables are tracked differently
            }
        }

        // Second pass: remove dead code
        // A temp assignment is dead if:
        // 1. It's a temp variable (starts with '.`)
        // 2. The temp is never used as src1 or src2 in subsequent instructions
        // 3. It's not the return value (a0)

        std::vector<TacInstr> new_instrs;
        std::unordered_set<std::string> defined_temps;

        for (auto& instr : func->instrs) {
            bool keep = true;

            // Check if this instruction defines a temp
            if (is_temp_var(instr.dest)) {
                // If this temp was already defined and not used in between, previous def is dead
                // For now, just check if the current def is used later
                if (used_temps.find(instr.dest) == used_temps.end()) {
                    // Also check if it's not a0 (return value)
                    if (instr.dest != "a0") {
                        // Check if it's used in any LOAD
                        bool is_loaded = false;
                        for (auto& other : func->instrs) {
                            if (other.op == TacOp::LOAD && other.src1 == instr.dest) {
                                is_loaded = true;
                                break;
                            }
                        }
                        if (!is_loaded) {
                            keep = false;
                        }
                    }
                }
            }

            // Remove redundant MOVE x, x
            if (instr.op == TacOp::MOVE && instr.src1 == instr.dest) {
                keep = false;
            }

            if (keep) {
                new_instrs.push_back(instr);
            }
        }

        func->instrs = std::move(new_instrs);
    }
}
