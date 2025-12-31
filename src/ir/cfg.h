#ifndef CFG_H
#define CFG_H

#include "tac.h"
#include <unordered_map>
#include <unordered_set>

// Helper: Check if a string is a temporary variable
inline bool is_temp(const std::string& s) {
    return !s.empty() && s[0] == '.';
}

// Helper: Check if a string is a number
inline bool is_number(const std::string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[0] == '-') i = 1;
    for (; i < s.size(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return !s.empty() && (s[0] != '-' || s.size() > 1);
}

// Helper: Get variables used/defined by an instruction
inline void instr_use_def(const TacInstr& instr,
                          std::unordered_set<std::string>& use,
                          std::unordered_set<std::string>& def) {
    use.clear();
    def.clear();

    switch (instr.op) {
        // Binary operations: dest = src1 op src2
        case TacOp::ADD:
        case TacOp::SUB:
        case TacOp::MUL:
        case TacOp::DIV:
        case TacOp::MOD:
        case TacOp::AND:
        case TacOp::OR:
        case TacOp::LT:
        case TacOp::GT:
        case TacOp::LE:
        case TacOp::GE:
        case TacOp::EQ:
        case TacOp::NE:
            if (!is_number(instr.src1)) use.insert(instr.src1);
            if (!is_number(instr.src2)) use.insert(instr.src2);
            if (!instr.dest.empty()) def.insert(instr.dest);
            break;

        // Unary operations: dest = op src1
        case TacOp::NOT:
        case TacOp::LOAD:
        case TacOp::LOAD_PARAM:
            if (!is_number(instr.src1)) use.insert(instr.src1);
            if (!instr.dest.empty()) def.insert(instr.dest);
            break;

        // Move: dest = src1
        case TacOp::MOVE:
            if (!is_number(instr.src1)) use.insert(instr.src1);
            if (!instr.dest.empty()) def.insert(instr.dest);
            break;

        // Load immediate: dest = imm
        case TacOp::LOAD_IMM:
            if (!instr.dest.empty()) def.insert(instr.dest);
            break;

        // Store: mem = src1
        case TacOp::STORE:
            if (!is_number(instr.src1)) use.insert(instr.src1);
            break;

        // Parameter: param reg = src1
        case TacOp::PARAM:
            if (!is_number(instr.src2)) use.insert(instr.src2);
            break;

        // Function call: dest = call name
        case TacOp::CALL:
            if (!instr.dest.empty()) def.insert(instr.dest);
            // Arguments in a0-a7 are clobbered
            for (int i = 0; i < 8; i++) {
                def.insert("a" + std::to_string(i));
            }
            break;

        // Branch: beqz src1, label (src1 used, no def)
        case TacOp::BEQZ:
        case TacOp::BNEZ:
            if (!is_number(instr.src1)) use.insert(instr.src1);
            break;

        // Label, Jump, Return: no use/def
        case TacOp::LABEL:
        case TacOp::JUMP:
        case TacOp::RET:
            break;

        // PHI function (for SSA)
        case TacOp::PHI:
            // src1 and src2 are from different predecessors
            if (!is_number(instr.src1)) use.insert(instr.src1);
            if (!is_number(instr.src2)) use.insert(instr.src2);
            if (!instr.dest.empty()) def.insert(instr.dest);
            break;
    }
}

// Build Control Flow Graph for a function
void FunctionIR::build_cfg() {
    blocks.clear();
    block_index.clear();

    if (instrs.empty()) return;

    // Find all label positions
    std::unordered_map<std::string, int> label_pos;
    for (int i = 0; i < (int)instrs.size(); i++) {
        if (instrs[i].op == TacOp::LABEL) {
            label_pos[instrs[i].src1] = i;
        }
    }

    // Create basic blocks
    int i = 0;
    while (i < (int)instrs.size()) {
        // Skip labels (they should be at the start of blocks)
        if (instrs[i].op == TacOp::LABEL) {
            i++;
            continue;
        }

        // Start a new block at current position
        std::string block_name = ".B" + std::to_string(blocks.size());
        BasicBlock block(block_name, i);

        // Collect instructions until we hit a terminator or next label
        while (i < (int)instrs.size()) {
            TacOp op = instrs[i].op;

            // Block ends at control flow instructions
            if (op == TacOp::JUMP || op == TacOp::BEQZ || op == TacOp::BNEZ ||
                op == TacOp::RET || op == TacOp::CALL) {
                // Include the terminator in this block
                block.instrs.push_back(instrs[i]);
                i++;
                break;
            }

            // Block also ends at labels (which start new blocks)
            if (op == TacOp::LABEL) {
                break;
            }

            block.instrs.push_back(instrs[i]);
            i++;
        }

        block.end_idx = i - 1;
        int block_idx = blocks.size();
        block_index[block_name] = block_idx;
        blocks.push_back(block);
    }

    // Build predecessor/successor relationships
    for (int b = 0; b < (int)blocks.size(); b++) {
        BasicBlock& block = blocks[b];

        if (block.instrs.empty()) continue;

        TacOp last_op = block.instrs.back().op;

        // Unconditional jump: jump label
        if (last_op == TacOp::JUMP) {
            std::string target = block.instrs.back().src1;
            if (label_pos.count(target)) {
                std::string target_block = ".B" + std::to_string(block_index.size());
                // Find which block contains this label
                for (int j = 0; j < (int)blocks.size(); j++) {
                    if (blocks[j].start_idx <= label_pos[target] &&
                        (j == (int)blocks.size() - 1 || blocks[j+1].start_idx > label_pos[target])) {
                        target_block = blocks[j].name;
                        blocks[j].predecessors.push_back(block.name);
                        break;
                    }
                }
                block.successors.push_back(target_block);
            }
        }
        // Conditional branch: beqz/bnez cond, label
        else if (last_op == TacOp::BEQZ || last_op == TacOp::BNEZ) {
            std::string target = block.instrs.back().src2;
            // Next block
            if (b + 1 < (int)blocks.size()) {
                block.successors.push_back(blocks[b + 1].name);
                blocks[b + 1].predecessors.push_back(block.name);
            }
            // Target block
            if (label_pos.count(target)) {
                for (int j = 0; j < (int)blocks.size(); j++) {
                    if (blocks[j].start_idx <= label_pos[target] &&
                        (j == (int)blocks.size() - 1 || blocks[j+1].start_idx > label_pos[target])) {
                        block.successors.push_back(blocks[j].name);
                        blocks[j].predecessors.push_back(block.name);
                        break;
                    }
                }
            }
        }
        // Function call - may have multiple successors (call + next)
        else if (last_op == TacOp::CALL) {
            // Next block
            if (b + 1 < (int)blocks.size()) {
                block.successors.push_back(blocks[b + 1].name);
                blocks[b + 1].predecessors.push_back(block.name);
            }
        }
        // Return - no successors
        else if (last_op == TacOp::RET) {
            // No successors
        }
        // Fall-through to next block
        else {
            if (b + 1 < (int)blocks.size()) {
                block.successors.push_back(blocks[b + 1].name);
                blocks[b + 1].predecessors.push_back(block.name);
            }
        }
    }
}

// Compute liveness for all variables in the function
void FunctionIR::compute_liveness() {
    if (blocks.empty()) return;

    // Collect all variables (temps and user vars)
    all_vars.clear();
    for (const auto& instr : instrs) {
        if (!instr.dest.empty() && !is_temp(instr.dest)) all_vars.insert(instr.dest);
        if (!instr.src1.empty() && !is_number(instr.src1) && !is_temp(instr.src1)) all_vars.insert(instr.src1);
        if (!instr.src2.empty() && !is_number(instr.src2) && !is_temp(instr.src2)) all_vars.insert(instr.src2);
    }
    // Add temps
    for (const auto& instr : instrs) {
        if (!instr.dest.empty() && is_temp(instr.dest)) all_vars.insert(instr.dest);
        if (!instr.src1.empty() && is_temp(instr.src1)) all_vars.insert(instr.src1);
        if (!instr.src2.empty() && is_temp(instr.src2)) all_vars.insert(instr.src2);
    }

    // Compute def and use for each block
    for (auto& block : blocks) {
        block.def.clear();
        block.use.clear();

        for (const auto& instr : block.instrs) {
            std::unordered_set<std::string> use, def;
            instr_use_def(instr, use, def);

            // For use: variable is used before being defined in this block
            for (const auto& v : use) {
                if (!block.def.count(v)) {
                    block.use.insert(v);
                }
            }
            // For def: all definitions
            block.def.insert(def.begin(), def.end());
        }
    }

    // Iterative liveness analysis (backward dataflow)
    bool changed = true;
    while (changed) {
        changed = false;

        for (auto& block : blocks) {
            std::set<std::string> old_live_in = block.live_in;
            std::set<std::string> old_live_out = block.live_out;

            // live_out[B] = union of live_in of all successors
            block.live_out.clear();
            for (const auto& succ_name : block.successors) {
                if (block_index.count(succ_name)) {
                    int succ_idx = block_index[succ_name];
                    block.live_out.insert(blocks[succ_idx].live_in.begin(),
                                         blocks[succ_idx].live_in.end());
                }
            }

            // live_in[B] = use[B] union (live_out[B] - def[B])
            block.live_in = block.use;
            for (const auto& v : block.live_out) {
                if (!block.def.count(v)) {
                    block.live_in.insert(v);
                }
            }

            // Check if anything changed
            if (block.live_in != old_live_in || block.live_out != old_live_out) {
                changed = true;
            }
        }
    }
}

#endif // CFG_H
