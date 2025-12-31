#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir/tac.h"
#include <unordered_map>
#include <string>

class Optimizer {
public:
    // Run all optimizations on a program
    static void optimize(ProgramIR* program);

    // Individual optimization passes
    static void constant_propagation(ProgramIR* program);
    static void constant_folding(ProgramIR* program);
    static void dead_code_elimination(ProgramIR* program);
    static void algebraic_simplification(ProgramIR* program);

private:
    // Helper for constant folding a single instruction
    static bool try_fold_instruction(TacInstr& instr,
                                      const std::unordered_map<std::string, long long>& constants);

    // Helper for algebraic simplification
    static bool try_simplify_instruction(TacInstr& instr);

    // Process a single function
    static void process_function(FunctionIR* func);
};

#endif // OPTIMIZER_H
