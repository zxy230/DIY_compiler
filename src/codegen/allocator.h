#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "ir/tac.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>

// RISC-V 32-bit register categories
namespace RegCategory {
    constexpr int ARG = 0;    // a0-a7: 参数寄存器
    constexpr int TEMP = 1;   // t0-t6: 调用者保存临时寄存器
    constexpr int SAVE = 2;   // s0-s11: 被调用者保存寄存器
}

// Register information
struct RegInfo {
    std::string name;
    int category;
    bool caller_save;  // true = caller save, false = callee save
    bool allocatable;  // false for zero (x0), ra, sp, gp, tp
};

class LinearScanAllocator {
public:
    // Register allocation result for each instruction position
    struct InstrAlloc {
        std::unordered_map<std::string, std::string> reg_map;  // var -> register
        std::unordered_map<std::string, int> spill_offset;     // var -> stack offset
    };

    LinearScanAllocator(FunctionIR* func)
        : func_(func) {
        init_registers();
    }

    // Main allocation function
    void allocate() {
        // Reset allocation state
        allocation_.clear();
        allocation_.resize(func_->instrs.size());

        // Compute live ranges for all variables
        compute_live_ranges();

        // Perform linear scan allocation
        linear_scan();

        // Print allocation for debugging
        print_allocation();
    }

    // Get allocation for a specific instruction
    const InstrAlloc& get_allocation(int idx) const {
        return allocation_[idx];
    }

    // Check if a variable is allocated to a register (not spilled)
    bool is_in_register(const std::string& var, int idx) const {
        auto it = allocation_[idx].reg_map.find(var);
        return it != allocation_[idx].reg_map.end();
    }

    // Get register for a variable at a specific instruction
    std::string get_register(const std::string& var, int idx) const {
        auto it = allocation_[idx].reg_map.find(var);
        if (it != allocation_[idx].reg_map.end()) {
            return it->second;
        }
        return "";  // Spilled
    }

    // Get spill offset for a variable at a specific instruction
    int get_spill_offset(const std::string& var, int idx) const {
        auto it = allocation_[idx].spill_offset.find(var);
        if (it != allocation_[idx].spill_offset.end()) {
            return it->second;
        }
        return -1;  // Not spilled
    }

private:
    FunctionIR* func_;
    std::vector<InstrAlloc> allocation_;

    // Available registers
    std::vector<RegInfo> all_regs_;
    std::vector<RegInfo> allocatable_regs_;

    // Active intervals for linear scan
    struct Interval {
        std::string var;
        int start;        // First instruction where live
        int end;          // Last instruction where live
        std::string reg;  // Assigned register (empty if spilled)
        int spill_offset; // Stack offset if spilled
    };

    std::vector<Interval> intervals_;
    std::set<size_t> active_;  // Set of active interval indices

    void init_registers() {
        // RISC-V 32-bit registers
        all_regs_ = {
            {"zero", RegCategory::TEMP, true, false},  // x0 - hardwired 0
            {"ra", RegCategory::TEMP, false, false},   // x1 - return address
            {"sp", RegCategory::TEMP, false, false},   // x2 - stack pointer
            {"gp", RegCategory::TEMP, false, false},   // x3 - global pointer
            {"tp", RegCategory::TEMP, false, false},   // x4 - thread pointer
            {"t0", RegCategory::TEMP, true, true},     // x5
            {"t1", RegCategory::TEMP, true, true},     // x6
            {"t2", RegCategory::TEMP, true, true},     // x7
            {"s0", RegCategory::SAVE, false, true},    // x8 - saved register
            {"s1", RegCategory::SAVE, false, true},    // x9
            {"a0", RegCategory::ARG, true, true},      // x10 - also return value
            {"a1", RegCategory::ARG, true, true},      // x11
            {"a2", RegCategory::ARG, true, true},      // x12
            {"a3", RegCategory::ARG, true, true},      // x13
            {"a4", RegCategory::ARG, true, true},      // x14
            {"a5", RegCategory::ARG, true, true},      // x15
            {"a6", RegCategory::ARG, true, true},      // x16
            {"a7", RegCategory::ARG, true, true},      // x17
            {"s2", RegCategory::SAVE, false, true},    // x18
            {"s3", RegCategory::SAVE, false, true},    // x19
            {"s4", RegCategory::SAVE, false, true},    // x20
            {"s5", RegCategory::SAVE, false, true},    // x21
            {"s6", RegCategory::SAVE, false, true},    // x22
            {"s7", RegCategory::SAVE, false, true},    // x23
            {"s8", RegCategory::SAVE, false, true},    // x24
            {"s9", RegCategory::SAVE, false, true},    // x25
            {"s10", RegCategory::SAVE, false, true},   // x26
            {"s11", RegCategory::SAVE, false, true},   // x27
            {"t3", RegCategory::TEMP, true, true},     // x28
            {"t4", RegCategory::TEMP, true, true},     // x29
            {"t5", RegCategory::TEMP, true, true},     // x30
            {"t6", RegCategory::TEMP, true, true},     // x31
        };

        // Only allocatable registers
        for (const auto& r : all_regs_) {
            if (r.allocatable) {
                allocatable_regs_.push_back(r);
            }
        }
    }

    // Compute live ranges for all variables
    void compute_live_ranges() {
        intervals_.clear();

        // For each variable, compute its live range
        for (const auto& var : func_->all_vars) {
            int start = -1;
            int end = -1;

            // Find first and last use
            for (int i = 0; i < (int)func_->instrs.size(); i++) {
                const auto& instr = func_->instrs[i];

                // Check if variable is used
                bool used = false;
                if (!is_number(instr.src1) && instr.src1 == var) used = true;
                if (!is_number(instr.src2) && instr.src2 == var) used = true;
                if (instr.dest == var) used = true;

                // For CALL, all arg registers are clobbered
                if (instr.op == TacOp::CALL) {
                    for (int a = 0; a < 8; a++) {
                        if ("a" + std::to_string(a) == var) {
                            // a0-a7 are clobbered by call
                            // Check if they're defined or used
                            if (instr.dest == var) used = true;
                        }
                    }
                }

                if (used) {
                    if (start == -1) start = i;
                    end = i;
                }
            }

            if (start != -1) {
                Interval interval;
                interval.var = var;
                interval.start = start;
                interval.end = end;
                interval.reg = "";
                interval.spill_offset = -1;
                intervals_.push_back(interval);
            }
        }

        // Sort by start position
        std::sort(intervals_.begin(), intervals_.end(),
                  [](const Interval& a, const Interval& b) {
                      return a.start < b.start;
                  });
    }

    // Linear scan register allocation - improved version
    void linear_scan() {
        // active_ will store indices of intervals that are currently active
        active_.clear();
        std::vector<bool> reg_used_(allocatable_regs_.size(), false);
        int next_spill_offset = 4;  // Start after ra

        for (size_t i = 0; i < intervals_.size(); i++) {
            // Remove expired intervals from active set
            // An interval expires if its end < current interval's start
            std::set<size_t> to_remove;
            for (size_t idx : active_) {
                if (intervals_[idx].end < intervals_[i].start) {
                    // Free the register
                    for (size_t r = 0; r < allocatable_regs_.size(); r++) {
                        if (allocatable_regs_[r].name == intervals_[idx].reg) {
                            reg_used_[r] = false;
                            break;
                        }
                    }
                    to_remove.insert(idx);
                }
            }
            for (size_t idx : to_remove) {
                active_.erase(idx);
            }

            // Find a free register or spill
            Interval& interval = intervals_[i];
            int reg_idx = -1;

            for (size_t r = 0; r < allocatable_regs_.size(); r++) {
                if (!reg_used_[r]) {
                    reg_idx = r;
                    break;
                }
            }

            if (reg_idx != -1) {
                // Allocate register
                interval.reg = allocatable_regs_[reg_idx].name;
                reg_used_[reg_idx] = true;
                // Add to active set
                active_.insert(i);
            } else {
                // Spill: find interval in active set that ends latest
                int spill_idx = -1;
                int latest_end = -1;
                for (size_t idx : active_) {
                    if (intervals_[idx].end > latest_end) {
                        // Prefer to spill non-argument registers
                        bool is_arg = intervals_[idx].var.find("a") == 0 &&
                                     intervals_[idx].var.size() == 2 &&
                                     isdigit(intervals_[idx].var[1]);
                        if (!is_arg) {
                            latest_end = intervals_[idx].end;
                            spill_idx = idx;
                        }
                    }
                }

                if (spill_idx >= 0 && latest_end > interval.end) {
                    // Spill the existing interval
                    intervals_[spill_idx].spill_offset = next_spill_offset;
                    next_spill_offset += 4;

                    // Free the register from spilled interval
                    for (size_t r = 0; r < allocatable_regs_.size(); r++) {
                        if (allocatable_regs_[r].name == intervals_[spill_idx].reg) {
                            reg_used_[r] = false;
                            break;
                        }
                    }
                    // Allocate register to current interval
                    interval.reg = intervals_[spill_idx].reg;
                    reg_used_[allocatable_regs_.size() - 1] = true;
                    // Add to active set
                    active_.insert(i);
                } else {
                    // Must spill current interval - do NOT add to active set
                    interval.spill_offset = next_spill_offset;
                    next_spill_offset += 4;
                }
            }
        }

        // Build per-instruction allocation
        for (int i = 0; i < (int)func_->instrs.size(); i++) {
            for (const auto& interval : intervals_) {
                if (i >= interval.start && i <= interval.end) {
                    if (!interval.reg.empty()) {
                        allocation_[i].reg_map[interval.var] = interval.reg;
                    }
                    if (interval.spill_offset >= 0) {
                        allocation_[i].spill_offset[interval.var] = interval.spill_offset;
                    }
                }
            }
        }
    }

    void print_allocation() {
        std::cerr << "=== Register Allocation ===\n";
        for (size_t i = 0; i < intervals_.size(); i++) {
            const auto& interval = intervals_[i];
            std::cerr << interval.var << ": [" << interval.start << ", " << interval.end << "] -> ";
            if (!interval.reg.empty()) {
                std::cerr << interval.reg;
            } else {
                std::cerr << "spill(" << interval.spill_offset << ")";
            }
            std::cerr << "\n";
        }
        std::cerr << "\n";
    }

    // Helper: check if string is a number
    bool is_number(const std::string& s) const {
        if (s.empty()) return false;
        size_t i = 0;
        if (s[0] == '-') i = 1;
        for (; i < s.size(); i++) {
            if (!isdigit(s[i])) return false;
        }
        return !s.empty() && (s[0] != '-' || s.size() > 1);
    }
};

#endif // ALLOCATOR_H
