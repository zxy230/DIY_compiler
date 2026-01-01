#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include "ast_node.h"
#include "ir/tac.h"
#include "ir/cfg.h"
#include "symbol.h"
#include <stack>
#include <unordered_map>

class IRBuilder
{
public:
    IRBuilder()
    {
        current_func_ = nullptr;
    }

    ProgramIR *build(Program *ast)
    {
        program_ir_ = std::make_unique<ProgramIR>();

        // Create IR for each function
        for (auto &func : ast->funcs)
        {
            auto func_ir = std::make_unique<FunctionIR>();
            func_ir->name = func->func_name;
            func_ir->is_void = (func->return_type == "void");
            program_ir_->functions.push_back(std::move(func_ir));
        }

        // Build IR for each function
        for (auto &func : ast->funcs)
        {
            build_function(func.get());
        }

        // Build CFG and compute liveness for all functions
        for (auto& func_ir : program_ir_->functions) {
            func_ir->build_cfg();
            func_ir->compute_liveness();
        }

        return program_ir_.get();
    }

private:
    std::unique_ptr<ProgramIR> program_ir_;
    FunctionIR *current_func_ = nullptr;
    std::unordered_map<std::string, int> var_offset_;
    int next_offset_ = 4; // Start after ra (4 bytes)
    int scope_level_ = 0; // Track current scope level for unique variable names

    // Stack for tracking block scope (for proper nested variable handling)
    // Each entry is a list of (var_name, old_offset) pairs for variables that were shadowed
    std::vector<std::vector<std::pair<std::string, int>>> block_scope_stack_;

    // Stack for nested loop break/continue labels
    struct LoopLabels {
        std::string break_label;
        std::string continue_label;
    };
    std::stack<LoopLabels> loop_labels_stack_;

    void build_function(FuncDef *func)
    {
        current_func_ = program_ir_->get_function(func->func_name);
        var_offset_.clear();
        next_offset_ = 4; // Start after ra (4 bytes)

        // Process parameters: allocate stack slots and load from a0-a7
        int param_idx = 0;
        for (auto &param : func->params)
        {
            if (param->type == NodeType::Param)
            {
                auto p = static_cast<Param *>(param.get());
                // Allocate stack slot for parameter
                var_offset_[p->param_name] = next_offset_;
                next_offset_ += 4;

                // Generate code to load parameter from argument register
                // Use special notation: @paramN to indicate it's from aN register
                std::string result = current_func_->next_temp();
                std::string arg_reg = "a" + std::to_string(param_idx);
                emit(TacOp::LOAD_PARAM, result, arg_reg, ""); // New opcode for param load
                emit(TacOp::STORE, p->param_name, result, "");

                param_idx++;
            }
        }

        // Process declarations and statements
        build_block(func->body.get());

        current_func_ = nullptr;
    }

    void build_block(Block *block)
    {
        // Enter new block scope
        scope_level_++;
        block_scope_stack_.push_back(std::vector<std::pair<std::string, int>>());
        int saved_offset = next_offset_;
        #// std::cerr << "[DEBUG] Entering block scope " << scope_level_ << ", saved_offset=" << saved_offset << std::endl;

        for (auto &stmt : block->stmts)
        {
            build_stmt(stmt.get());
        }

        // Exit block scope: restore shadowed variables
        #// std::cerr << "[DEBUG] Exiting block, restoring:" << std::endl;
        for (const auto& pair : block_scope_stack_.back()) {
            if (pair.second >= 0) {
                // Restore the outer scope's offset for this variable
                #// std::cerr << "[DEBUG]   " << pair.first << " -> " << pair.second << std::endl;
                var_offset_[pair.first] = pair.second;
            } else {
                // Variable didn't exist in outer scope, remove it
                #// std::cerr << "[DEBUG]   " << pair.first << " (remove)" << std::endl;
                var_offset_.erase(pair.first);
            }
        }
        // Restore offset to what it was at block entry (discard block-local allocations)
        next_offset_ = saved_offset;
        #// std::cerr << "[DEBUG] Restored next_offset_=" << next_offset_ << std::endl;
        block_scope_stack_.pop_back();
        scope_level_--;
    }

    void build_stmt(StmtNode *stmt)
    {
        switch (stmt->type)
        {
        case NodeType::Block:
            build_block(static_cast<Block *>(stmt));
            break;
        case NodeType::DeclStmt:
            build_decl(static_cast<DeclStmt *>(stmt));
            break;
        case NodeType::AssignStmt:
            build_assign(static_cast<AssignStmt *>(stmt));
            break;
        case NodeType::ExprStmt:
            if (static_cast<ExprStmt *>(stmt)->expr)
            {
                build_expr(static_cast<ExprStmt *>(stmt)->expr.get());
            }
            break;
        case NodeType::IfStmt:
            build_if(static_cast<IfStmt *>(stmt));
            break;
        case NodeType::WhileStmt:
            build_while(static_cast<WhileStmt *>(stmt));
            break;
        case NodeType::ReturnStmt:
            build_return(static_cast<ReturnStmt *>(stmt));
            break;
        case NodeType::BreakStmt:
            if (!loop_labels_stack_.empty()) {
                emit(TacOp::JUMP, "", "", loop_labels_stack_.top().break_label);
            }
            break;
        case NodeType::ContinueStmt:
            if (!loop_labels_stack_.empty()) {
                emit(TacOp::JUMP, "", "", loop_labels_stack_.top().continue_label);
            }
            break;
        default:
            break;
        }
    }

    void build_decl(DeclStmt *stmt)
    {
        // Create unique variable name for nested scopes
        std::string unique_name = stmt->var_name;
        if (scope_level_ > 0) {
            unique_name = stmt->var_name + ".s" + std::to_string(scope_level_);
        }

        // Save old offset if variable exists (for shadowing)
        int old_offset = -1;
        if (var_offset_.find(unique_name) != var_offset_.end()) {
            old_offset = var_offset_[unique_name];
            #// std::cerr << "[DEBUG] Declaring " << stmt->var_name << " (as " << unique_name << ") shadows old offset " << old_offset << std::endl;
        } else {
            #// std::cerr << "[DEBUG] Declaring " << stmt->var_name << " (as " << unique_name << ") at new offset " << next_offset_ << std::endl;
        }

        // Assign stack offset
        var_offset_[unique_name] = next_offset_;
        #// std::cerr << "[DEBUG]   -> Assigned offset " << next_offset_ << " to " << unique_name << std::endl;

        // Track this variable in the current block scope with its old offset
        if (!block_scope_stack_.empty()) {
            block_scope_stack_.back().push_back({unique_name, old_offset});
        }

        next_offset_ += 4;

        // Build initializer
        std::string init_val = build_expr(stmt->init_expr.get());

        // Store to stack using unique name
        emit(TacOp::STORE, unique_name, init_val, "");
    }

    void build_assign(AssignStmt *stmt)
    {
        std::string value = build_expr(stmt->value.get());
        std::string unique_name = resolve_var_name(stmt->var_name);
        emit(TacOp::STORE, unique_name, value, "");
    }

    void build_if(IfStmt *stmt)
    {
        std::string cond = build_expr(stmt->cond.get());
        std::string end_label = current_func_->next_label();

        if (stmt->else_stmt) {
            // if ... else ...: need else_label
            std::string else_label = current_func_->next_label();
            emit(TacOp::BEQZ, "", cond, else_label);
            build_stmt(stmt->then_stmt.get());
            emit(TacOp::JUMP, "", "", end_label);
            emit(TacOp::LABEL, "", "", else_label);
            build_stmt(stmt->else_stmt.get());
        } else {
            // if ... without else: jump to end if condition is false
            emit(TacOp::BEQZ, "", cond, end_label);
            build_stmt(stmt->then_stmt.get());
            // No need to jump to end - just fall through
        }
        emit(TacOp::LABEL, "", "", end_label);
    }

    void build_while(WhileStmt *stmt)
    {
        std::string loop_start = current_func_->next_label();
        std::string loop_end = current_func_->next_label();

        // Push break/continue labels for nested loops
        LoopLabels labels{loop_end, loop_start};
        loop_labels_stack_.push(labels);

        emit(TacOp::LABEL, "", "", loop_start);
        std::string cond = build_expr(stmt->cond.get());
        emit(TacOp::BEQZ, "", cond, loop_end);
        build_stmt(stmt->body.get());
        emit(TacOp::JUMP, "", "", loop_start);
        emit(TacOp::LABEL, "", "", loop_end);

        // Pop labels
        loop_labels_stack_.pop();
    }

    // Resolve variable name to its unique scoped name
    std::string resolve_var_name(const std::string& var_name) {
        // First try the scoped name at current scope level
        if (scope_level_ > 0) {
            std::string scoped_name = var_name + ".s" + std::to_string(scope_level_);
            if (var_offset_.find(scoped_name) != var_offset_.end()) {
                return scoped_name;
            }
        }
        // If not found at current level, search through outer scopes
        // Start from scope_level_ - 1 and go down to 1
        for (int level = scope_level_ - 1; level >= 1; level--) {
            std::string scoped_name = var_name + ".s" + std::to_string(level);
            if (var_offset_.find(scoped_name) != var_offset_.end()) {
                return scoped_name;
            }
        }
        // Fall back to the base name (for params or undeclared variables)
        return var_name;
    }

    void build_return(ReturnStmt *stmt)
    {
        if (stmt->value)
        {
            std::string ret_val = build_expr(stmt->value.get());
            emit(TacOp::MOVE, "a0", ret_val, "");
        }
        emit(TacOp::RET, "", "", "");
    }

    std::string build_expr(ExprNode *expr)
    {
        switch (expr->type)
        {
        case NodeType::BinaryExpr:
        {
            auto e = static_cast<BinaryExpr *>(expr);
            std::string left = build_expr(e->left.get());
            std::string right = build_expr(e->right.get());
            std::string result = current_func_->next_temp();

            TacOp op;
            switch (e->op)
            {
            case OpType::Add:
                op = TacOp::ADD;
                break;
            case OpType::Sub:
                op = TacOp::SUB;
                break;
            case OpType::Mul:
                op = TacOp::MUL;
                break;
            case OpType::Div:
                op = TacOp::DIV;
                break;
            case OpType::Mod:
                op = TacOp::MOD;
                break;
            case OpType::Lt:
                op = TacOp::LT;
                break;
            case OpType::Gt:
                op = TacOp::GT;
                break;
            case OpType::Le:
                op = TacOp::LE;
                break;
            case OpType::Ge:
                op = TacOp::GE;
                break;
            case OpType::Eq:
                op = TacOp::EQ;
                break;
            case OpType::Ne:
                op = TacOp::NE;
                break;
            case OpType::And:
                op = TacOp::AND;
                break;
            case OpType::Or:
                op = TacOp::OR;
                break;
            default:
                op = TacOp::ADD;
                break;
            }
            emit(op, result, left, right);
            return result;
        }
        case NodeType::UnaryExpr:
        {
            auto e = static_cast<UnaryExpr *>(expr);
            std::string operand = build_expr(e->operand.get());
            std::string result = current_func_->next_temp();

            if (e->op == OpType::Neg)
            {
                emit(TacOp::LOAD_IMM, result, "0", "");
                emit(TacOp::SUB, result, result, operand);
            }
            else if (e->op == OpType::Not)
            {
                emit(TacOp::NOT, result, operand, "");
            }
            return result;
        }
        case NodeType::CallExpr:
        {
            auto e = static_cast<CallExpr *>(expr);
            int i = 0;
            for (auto &arg : e->args)
            {
                std::string arg_val = build_expr(arg.get());
                emit(TacOp::PARAM, "a" + std::to_string(i), arg_val, "");
                i++;
            }
            std::string result = current_func_->next_temp();
            emit(TacOp::CALL, result, e->func_name, "");
            return result;
        }
        case NodeType::VarExpr:
        {
            auto e = static_cast<VarExpr *>(expr);
            std::string result = current_func_->next_temp();
            std::string unique_name = resolve_var_name(e->var_name);
            emit(TacOp::LOAD, result, unique_name, "");
            return result;
        }
        case NodeType::ConstExpr:
        {
            auto e = static_cast<ConstExpr *>(expr);
            std::string result = current_func_->next_temp();
            emit(TacOp::LOAD_IMM, result, std::to_string(e->const_value), "");
            return result;
        }
        default:
            return "0";
        }
    }

    void emit(TacOp op, const std::string &dest, const std::string &src1, const std::string &src2)
    {
        if (current_func_)
        {
            #// std::cerr << "[DEBUG-EMIT] " << (int)op << " " << dest << ", " << src1 << ", " << src2 << std::endl;
            current_func_->instrs.emplace_back(op, dest, src1, src2);
        }
    }
};

#endif // IR_BUILDER_H
