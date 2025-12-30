#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// TAC operation types
enum class TacOp {
    // Arithmetic
    ADD, SUB, MUL, DIV, MOD,
    // Logical
    AND, OR, NOT,
    // Comparison
    LT, GT, LE, GE, EQ, NE,
    // Memory
    LOAD, STORE,
    // Control flow
    LABEL, JUMP, BEQZ, BNEZ, CALL, RET,
    // Function
    PARAM, MOVE,
    // Constant
    LOAD_IMM,
    // Phi function (for SSA)
    PHI
};

struct TacInstr {
    TacOp op;
    std::string dest;      // Result variable
    std::string src1;      // First source
    std::string src2;      // Second source (can be empty for unary ops)
    std::string comment;   // For debugging

    TacInstr(TacOp operation, const std::string& d = "",
             const std::string& s1 = "", const std::string& s2 = "")
        : op(operation), dest(d), src1(s1), src2(s2) {}

    std::string to_string() const;
};

class FunctionIR {
public:
    std::string name;
    std::vector<TacInstr> instrs;
    std::vector<std::string> params;
    int stack_size = 0;
    bool is_void = false;

    std::string next_temp() {
        return ".t" + std::to_string(temp_count_++);
    }

    std::string next_label() {
        return ".L" + std::to_string(label_count_++);
    }

private:
    int temp_count_ = 0;
    int label_count_ = 0;
};

class ProgramIR {
public:
    std::vector<std::unique_ptr<FunctionIR>> functions;
    std::unordered_map<std::string, int> global_vars;

    FunctionIR* get_function(const std::string& name) {
        for (auto& f : functions) {
            if (f->name == name) return f.get();
        }
        return nullptr;
    }
};

#endif // TAC_H
