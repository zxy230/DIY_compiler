#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "ir/tac.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <climits>
#include <cctype>

// RISC-V 32-bit register categories
namespace RegCategory
{
    constexpr int ARG = 0;   // a0-a7: 参数寄存器
    constexpr int TEMP = 1;  // t0-t6, t3-t6: 调用者保存临时寄存器
    constexpr int SAVE = 2;  // s0-s11: 被调用者保存寄存器
}

// Register allocation priorities
namespace RegPriority
{
    constexpr int HIGH = 0;    // 高优先级：短期变量，尽量分配寄存器
    constexpr int NORMAL = 1;  // 正常优先级
    constexpr int LOW = 2;     // 低优先级：长期变量或频繁存取的变量
}

// Register information
struct RegInfo
{
    std::string name;
    int category;
    bool caller_save; // true = caller save, false = callee save
    bool allocatable; // false for zero (x0), ra, sp, gp, tp
};

class LinearScanAllocator
{
public:
    // Register allocation result for each instruction position
    struct InstrAlloc
    {
        std::unordered_map<std::string, std::string> reg_map;    // var -> register
        std::unordered_map<std::string, int> spill_offset;       // var -> stack offset
    };

    LinearScanAllocator(FunctionIR *func)
        : func_(func)
    {
        init_registers();
        // 只分配可用的临时寄存器和保存寄存器
        // 排除a0-a7（用于参数和返回值）和特殊寄存器
        // 也排除s0（用作帧指针）
        available_regs_.clear();
        for (const auto &r : all_regs_)
        {
            if (r.allocatable && r.category != RegCategory::ARG && r.name != "s0")
            {
                available_regs_.push_back(r);
            }
        }
    }

    // Main allocation function - 优化版本，优先使用寄存器
    void allocate()
    {
        // Reset allocation state
        allocation_.clear();
        allocation_.resize(func_->instrs.size());

        // Compute live ranges for all variables
        compute_live_ranges();

        // Perform linear scan allocation with register priority
        linear_scan();

        // Print allocation for debugging (可选)
        // print_allocation();
    }

    // Get allocation for a specific instruction
    const InstrAlloc &get_allocation(int idx) const
    {
        return allocation_[idx];
    }

    // Check if a variable is allocated to a register (not spilled)
    bool is_in_register(const std::string &var, int idx) const
    {
        auto it = allocation_[idx].reg_map.find(var);
        return it != allocation_[idx].reg_map.end();
    }

    // Get register for a variable at a specific instruction
    std::string get_register(const std::string &var, int idx) const
    {
        auto it = allocation_[idx].reg_map.find(var);
        if (it != allocation_[idx].reg_map.end())
        {
            return it->second;
        }
        return ""; // Spilled
    }

    // Get spill offset for a variable at a specific instruction
    int get_spill_offset(const std::string &var, int idx) const
    {
        auto it = allocation_[idx].spill_offset.find(var);
        if (it != allocation_[idx].spill_offset.end())
        {
            return it->second;
        }
        return -1; // Not spilled
    }

    // Get all spilled variables (for stack allocation)
    std::vector<std::string> get_spilled_vars() const
    {
        std::vector<std::string> spilled;
        for (const auto &interval : intervals_)
        {
            if (interval.reg.empty() && interval.spill_offset >= 0)
            {
                spilled.push_back(interval.var);
            }
        }
        return spilled;
    }

    // Get number of spilled variables
    int get_spill_count() const
    {
        int count = 0;
        for (const auto &interval : intervals_)
        {
            if (interval.reg.empty() && interval.spill_offset >= 0)
            {
                count++;
            }
        }
        return count;
    }

private:
    FunctionIR *func_;
    std::vector<InstrAlloc> allocation_;

    // All registers
    std::vector<RegInfo> all_regs_;
    // Available registers for allocation (excluding ARG category)
    std::vector<RegInfo> available_regs_;

    // Active intervals for linear scan
    struct Interval
    {
        std::string var;
        int start;         // First instruction where live
        int end;           // Last instruction where live
        int length;        // Live range length (end - start)
        std::string reg;   // Assigned register (empty if spilled)
        int spill_offset;  // Stack offset if spilled
        bool is_user_var;  // Is this a user-defined variable?
    };

    std::vector<Interval> intervals_;
    std::set<size_t> active_; // Set of active interval indices

    void init_registers()
    {
        // RISC-V 32-bit registers
        all_regs_ = {
            {"zero", RegCategory::TEMP, true, false}, // x0 - hardwired 0
            {"ra", RegCategory::TEMP, false, false},  // x1 - return address
            {"sp", RegCategory::TEMP, false, false},  // x2 - stack pointer
            {"gp", RegCategory::TEMP, false, false},  // x3 - global pointer
            {"tp", RegCategory::TEMP, false, false},  // x4 - thread pointer
            {"t0", RegCategory::TEMP, true, true},    // x5
            {"t1", RegCategory::TEMP, true, true},    // x6
            {"t2", RegCategory::TEMP, true, true},    // x7
            {"s0", RegCategory::SAVE, false, true},   // x8 - saved register (frame pointer)
            {"s1", RegCategory::SAVE, false, true},   // x9
            {"a0", RegCategory::ARG, true, true},     // x10 - also return value
            {"a1", RegCategory::ARG, true, true},     // x11
            {"a2", RegCategory::ARG, true, true},     // x12
            {"a3", RegCategory::ARG, true, true},     // x13
            {"a4", RegCategory::ARG, true, true},     // x14
            {"a5", RegCategory::ARG, true, true},     // x15
            {"a6", RegCategory::ARG, true, true},     // x16
            {"a7", RegCategory::ARG, true, true},     // x17
            {"s2", RegCategory::SAVE, false, true},   // x18
            {"s3", RegCategory::SAVE, false, true},   // x19
            {"s4", RegCategory::SAVE, false, true},   // x20
            {"s5", RegCategory::SAVE, false, true},   // x21
            {"s6", RegCategory::SAVE, false, true},   // x22
            {"s7", RegCategory::SAVE, false, true},   // x23
            {"s8", RegCategory::SAVE, false, true},   // x24
            {"s9", RegCategory::SAVE, false, true},   // x25
            {"s10", RegCategory::SAVE, false, true},  // x26
            {"s11", RegCategory::SAVE, false, true},  // x27
            {"t3", RegCategory::TEMP, true, true},    // x28
            {"t4", RegCategory::TEMP, true, true},    // x29
            {"t5", RegCategory::TEMP, true, true},    // x30
            {"t6", RegCategory::TEMP, true, true},    // x31
        };
    }

    // Check if variable is a user-defined variable (not temp)
    bool is_user_var(const std::string &var) const
    {
        return !var.empty() && var[0] != '.';
    }

    // Compute live ranges for all variables - 改进版本
    void compute_live_ranges()
    {
        intervals_.clear();

        // 首先收集所有变量
        std::unordered_set<std::string> all_vars;
        for (const auto &instr : func_->instrs)
        {
            if (!is_number(instr.dest))
                all_vars.insert(instr.dest);
            if (!is_number(instr.src1))
                all_vars.insert(instr.src1);
            if (!is_number(instr.src2))
                all_vars.insert(instr.src2);
        }

        // 对每个变量计算其活跃范围
        for (const auto &var : all_vars)
        {
            int start = -1;
            int end = -1;

            // 寻找第一次和最后一次使用
            for (int i = 0; i < (int)func_->instrs.size(); i++)
            {
                const auto &instr = func_->instrs[i];

                // 检查变量是否被使用
                bool used = false;
                if (!is_number(instr.src1) && instr.src1 == var)
                    used = true;
                if (!is_number(instr.src2) && instr.src2 == var)
                    used = true;
                if (instr.dest == var)
                    used = true;

                // 对于CALL，所有参数寄存器可能被修改
                if (instr.op == TacOp::CALL)
                {
                    // 检查是否是a0-a7
                    if (var.length() == 2 && var[0] == 'a' && isdigit(var[1]))
                    {
                        int arg_idx = var[1] - '0';
                        if (arg_idx >= 0 && arg_idx <= 7)
                        {
                            // a0-a7在调用后可能被修改
                            if (instr.dest == var)
                                used = true;
                        }
                    }
                }

                if (used)
                {
                    if (start == -1)
                        start = i;
                    end = i;
                }
            }

            if (start != -1)
            {
                Interval interval;
                interval.var = var;
                interval.start = start;
                interval.end = end;
                interval.length = end - start;
                interval.reg = "";
                interval.spill_offset = -1;
                interval.is_user_var = is_user_var(var);
                intervals_.push_back(interval);
            }
        }

        // 按开始位置排序（线性扫描的标准做法）
        std::sort(intervals_.begin(), intervals_.end(),
                  [](const Interval &a, const Interval &b)
                  {
                      return a.start < b.start;
                  });
    }

    // 线性扫描寄存器分配 - 寄存器优先版本
    // 核心原则：所有变量默认分配到寄存器，只有在寄存器不够时才溢出
    void linear_scan()
    {
        active_.clear();
        std::vector<bool> reg_used_(available_regs_.size(), false);
        int next_spill_offset = 4; // 从4开始（0位置留给ra）

        // 按活跃范围排序（线性扫描的标准做法）
        std::sort(intervals_.begin(), intervals_.end(),
                  [](const Interval &a, const Interval &b)
                  {
                      return a.start < b.start;
                  });

        for (size_t i = 0; i < intervals_.size(); i++)
        {
            Interval &interval = intervals_[i];

            // 1. 从活动集中移除已过期的区间
            std::set<size_t> to_remove;
            for (size_t idx : active_)
            {
                // 如果区间结束 < 当前区间开始，则该寄存器可以释放
                if (intervals_[idx].end < interval.start)
                {
                    // 释放寄存器
                    for (size_t r = 0; r < available_regs_.size(); r++)
                    {
                        if (available_regs_[r].name == intervals_[idx].reg)
                        {
                            reg_used_[r] = false;
                            break;
                        }
                    }
                    to_remove.insert(idx);
                }
            }
            for (size_t idx : to_remove)
            {
                active_.erase(idx);
            }

            // 2. 尝试分配寄存器（总是优先使用寄存器）
            int reg_idx = -1;

            // 查找空闲寄存器
            for (size_t r = 0; r < available_regs_.size(); r++)
            {
                if (!reg_used_[r])
                {
                    reg_idx = r;
                    break;
                }
            }

            if (reg_idx != -1)
            {
                // 分配寄存器
                interval.reg = available_regs_[reg_idx].name;
                reg_used_[reg_idx] = true;
                active_.insert(i);
            }
            else
            {
                // 所有寄存器都被占用，必须选择一个变量溢出
                // 溢出策略：优先溢出临时变量，保留用户变量在寄存器中

                int spill_idx = -1;
                int best_score = INT_MIN;

                for (size_t idx : active_)
                {
                    const auto &active_interval = intervals_[idx];

                    // 计算溢出分数（分数越高越应该被溢出）
                    int score = 0;

                    // 临时变量优先溢出（+100分）
                    if (!active_interval.is_user_var)
                        score += 100;

                    // 用户变量尽量保留（-200分）
                    if (active_interval.is_user_var)
                        score -= 200;

                    // 活跃范围越短，越优先溢出（短生命期变量溢出代价更低）
                    int live_range = active_interval.end - active_interval.start;
                    score += live_range / 10;

                    // 已经溢出过的变量再次溢出代价较低（+20分）
                    if (active_interval.spill_offset >= 0)
                        score += 20;

                    // 选择分数最高的（最应该被溢出的）
                    if (score > best_score)
                    {
                        best_score = score;
                        spill_idx = idx;
                    }
                }

                if (spill_idx >= 0)
                {
                    // 溢出选中的变量
                    intervals_[spill_idx].spill_offset = next_spill_offset;
                    next_spill_offset += 4;

                    // 释放其寄存器
                    for (size_t r = 0; r < available_regs_.size(); r++)
                    {
                        if (available_regs_[r].name == intervals_[spill_idx].reg)
                        {
                            reg_used_[r] = false;
                            break;
                        }
                    }

                    // 将释放的寄存器分配给当前变量
                    interval.reg = intervals_[spill_idx].reg;
                    reg_used_[allocatable_index(interval.reg)] = true;
                    active_.insert(i);
                }
                else
                {
                    // 极端情况：当前变量必须溢出
                    interval.spill_offset = next_spill_offset;
                    next_spill_offset += 4;
                }
            }
        }

        // 构建每条指令的分配信息
        for (int i = 0; i < (int)func_->instrs.size(); i++)
        {
            for (const auto &interval : intervals_)
            {
                if (i >= interval.start && i <= interval.end)
                {
                    // 只有在有寄存器分配时才添加到reg_map
                    if (!interval.reg.empty())
                    {
                        allocation_[i].reg_map[interval.var] = interval.reg;
                    }
                    // 只有在有溢出偏移时才添加到spill_offset
                    if (interval.spill_offset >= 0)
                    {
                        allocation_[i].spill_offset[interval.var] = interval.spill_offset;
                    }
                }
            }
        }
    }

    // 辅助函数：根据寄存器名获取其在available_regs_中的索引
    int allocatable_index(const std::string &reg_name)
    {
        for (size_t r = 0; r < available_regs_.size(); r++)
        {
            if (available_regs_[r].name == reg_name)
            {
                return r;
            }
        }
        return -1;
    }

    void print_allocation()
    {
        std::cerr << "=== Register Allocation ===\n";
        int spilled = 0;
        for (size_t i = 0; i < intervals_.size(); i++)
        {
            const auto &interval = intervals_[i];
            std::cerr << interval.var << ": [" << interval.start << ", " << interval.end << "] ";
            if (!interval.reg.empty())
            {
                std::cerr << "-> " << interval.reg;
            }
            else
            {
                std::cerr << "-> spill(" << interval.spill_offset << ")";
                spilled++;
            }
            std::cerr << "\n";
        }
        std::cerr << "Total: " << intervals_.size() << " vars, " << spilled << " spilled\n\n";
    }

    // Helper: check if string is a number
    bool is_number(const std::string &s) const
    {
        if (s.empty())
            return false;
        size_t i = 0;
        if (s[0] == '-')
        {
            if (s.length() == 1)
                return false;
            i = 1;
        }
        for (; i < s.size(); i++)
        {
            if (!isdigit(s[i]))
                return false;
        }
        return !s.empty() && (s[0] != '-' || s.size() > 1);
    }
};

#endif // ALLOCATOR_H
