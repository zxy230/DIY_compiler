#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>

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
    // Parameter load (Phase 1: handle function parameters)
    LOAD_PARAM,
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

    inline std::string to_string() const {
        static const char* op_names[] = {
            "ADD", "SUB", "MUL", "DIV", "MOD",
            "AND", "OR", "NOT",
            "LT", "GT", "LE", "GE", "EQ", "NE",
            "LOAD", "STORE", "LOAD_PARAM",
            "LABEL", "JUMP", "BEQZ", "BNEZ", "CALL", "RET",
            "PARAM", "MOVE",
            "LOAD_IMM", "PHI"
        };
        std::string s = op_names[static_cast<int>(op)];
        if (!dest.empty()) s += " " + dest;
        if (!src1.empty()) s += ", " + src1;
        if (!src2.empty()) s += ", " + src2;
        return s;
    }
};

// Basic Block structure for Control Flow Graph
struct BasicBlock {
    std::string name;
    int start_idx;           // Start instruction index
    int end_idx;             // End instruction index (inclusive)
    std::vector<std::string> predecessors;   // Names of predecessor blocks
    std::vector<std::string> successors;     // Names of successor blocks
    std::vector<TacInstr> instrs;            // Instructions in this block

    // Liveness analysis results
    std::set<std::string> live_in;           // Variables live at block entry
    std::set<std::string> live_out;          // Variables live at block exit
    std::set<std::string> def;               // Variables defined (written) in block
    std::set<std::string> use;               // Variables used (read) before definition

    BasicBlock(const std::string& n = "", int start = 0)
        : name(n), start_idx(start), end_idx(-1) {}
};

class FunctionIR {
public:
    std::string name;
    std::vector<TacInstr> instrs;
    std::vector<std::string> params;
    int stack_size = 0;
    bool is_void = false;

    // Control Flow Graph
    std::vector<BasicBlock> blocks;
    std::unordered_map<std::string, int> block_index;  // Block name -> index

    // Liveness analysis
    std::set<std::string> all_vars;  // All variables in function

    std::string next_temp() {
        return ".t" + std::to_string(temp_count_++);
    }

    std::string next_label() {
        // Include function name to make labels unique across functions
        return ".L" + name + "_" + std::to_string(label_count_++);
    }

    int get_label_count() const { return label_count_; }

    // CFG building methods
    void build_cfg();
    void compute_liveness();

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
