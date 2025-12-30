#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include "ast_node.h"
#include "ir/tac.h"
#include "symbol.h"
#include <stack>
#include <unordered_map>

class IRBuilder {
public:
    IRBuilder() {
        current_func_ = nullptr;
    }

    ProgramIR* build(Program* ast) {
        program_ir_ = std::make_unique<ProgramIR>();

        // Create IR for each function
        for (auto& func : ast->funcs) {
            auto func_ir = std::make_unique<FunctionIR>();
            func_ir->name = func->func_name;
            func_ir->is_void = (func->return_type == "void");
            program_ir_->functions.push_back(std::move(func_ir));
        }

        // Build IR for each function
        for (auto& func : ast->funcs) {
            build_function(func.get());
        }

        return program_ir_.get();
    }

private:
    std::unique_ptr<ProgramIR> program_ir_;
    FunctionIR* current_func_ = nullptr;
    std::unordered_map<std::string, int> var_offset_;
    int next_offset_ = 4;  // Start after ra (4 bytes)

    void build_function(FuncDef* func) {
        current_func_ = program_ir_->get_function(func->func_name);

        // Process declarations and statements
        build_block(func->body.get());

        current_func_ = nullptr;
    }

    void build_block(Block* block) {
        for (auto& stmt : block->stmts) {
            build_stmt(stmt.get());
        }
    }

    void build_stmt(StmtNode* stmt) {
        switch (stmt->type) {
            case NodeType::Block:
                build_block(static_cast<Block*>(stmt));
                break;
            case NodeType::DeclStmt:
                build_decl(static_cast<DeclStmt*>(stmt));
                break;
            case NodeType::AssignStmt:
                build_assign(static_cast<AssignStmt*>(stmt));
                break;
            case NodeType::ExprStmt:
                if (static_cast<ExprStmt*>(stmt)->expr) {
                    build_expr(static_cast<ExprStmt*>(stmt)->expr.get());
                }
                break;
            case NodeType::IfStmt:
                build_if(static_cast<IfStmt*>(stmt));
                break;
            case NodeType::WhileStmt:
                build_while(static_cast<WhileStmt*>(stmt));
                break;
            case NodeType::ReturnStmt:
                build_return(static_cast<ReturnStmt*>(stmt));
                break;
            case NodeType::BreakStmt:
                emit(TacOp::JUMP, "", "", "break_end");
                break;
            case NodeType::ContinueStmt:
                emit(TacOp::JUMP, "", "", "continue_start");
                break;
            default:
                break;
        }
    }

    void build_decl(DeclStmt* stmt) {
        // Assign stack offset
        var_offset_[stmt->var_name] = next_offset_;
        next_offset_ += 4;

        // Build initializer
        std::string init_val = build_expr(stmt->init_expr.get());

        // Store to stack
        emit(TacOp::STORE, stmt->var_name, init_val, "");
    }

    void build_assign(AssignStmt* stmt) {
        std::string value = build_expr(stmt->value.get());
        emit(TacOp::STORE, stmt->var_name, value, "");
    }

    void build_if(IfStmt* stmt) {
        std::string cond = build_expr(stmt->cond.get());
        std::string else_label = current_func_->next_label();
        std::string end_label = current_func_->next_label();

        emit(TacOp::BEQZ, "", cond, else_label);
        build_stmt(stmt->then_stmt.get());
        emit(TacOp::JUMP, "", "", end_label);
        emit(TacOp::LABEL, "", "", else_label);
        if (stmt->else_stmt) {
            build_stmt(stmt->else_stmt.get());
        }
        emit(TacOp::LABEL, "", "", end_label);
    }

    void build_while(WhileStmt* stmt) {
        std::string loop_start = current_func_->next_label();
        std::string loop_end = current_func_->next_label();

        emit(TacOp::LABEL, "", "", loop_start);
        std::string cond = build_expr(stmt->cond.get());
        emit(TacOp::BEQZ, "", cond, loop_end);
        build_stmt(stmt->body.get());
        emit(TacOp::JUMP, "", "", loop_start);
        emit(TacOp::LABEL, "", "", loop_end);
    }

    void build_return(ReturnStmt* stmt) {
        if (stmt->value) {
            std::string ret_val = build_expr(stmt->value.get());
            emit(TacOp::MOVE, "a0", ret_val, "");
        }
        emit(TacOp::RET, "", "", "");
    }

    std::string build_expr(ExprNode* expr) {
        switch (expr->type) {
            case NodeType::BinaryExpr: {
                auto e = static_cast<BinaryExpr*>(expr);
                std::string left = build_expr(e->left.get());
                std::string right = build_expr(e->right.get());
                std::string result = current_func_->next_temp();

                TacOp op;
                switch (e->op) {
                    case OpType::Add: op = TacOp::ADD; break;
                    case OpType::Sub: op = TacOp::SUB; break;
                    case OpType::Mul: op = TacOp::MUL; break;
                    case OpType::Div: op = TacOp::DIV; break;
                    case OpType::Mod: op = TacOp::MOD; break;
                    case OpType::Lt:  op = TacOp::LT; break;
                    case OpType::Gt:  op = TacOp::GT; break;
                    case OpType::Le:  op = TacOp::LE; break;
                    case OpType::Ge:  op = TacOp::GE; break;
                    case OpType::Eq:  op = TacOp::EQ; break;
                    case OpType::Ne:  op = TacOp::NE; break;
                    case OpType::And: op = TacOp::AND; break;
                    case OpType::Or:  op = TacOp::OR; break;
                    default: op = TacOp::ADD; break;
                }
                emit(op, result, left, right);
                return result;
            }
            case NodeType::UnaryExpr: {
                auto e = static_cast<UnaryExpr*>(expr);
                std::string operand = build_expr(e->operand.get());
                std::string result = current_func_->next_temp();

                if (e->op == OpType::Neg) {
                    emit(TacOp::LOAD_IMM, result, "0", "");
                    emit(TacOp::SUB, result, result, operand);
                } else if (e->op == OpType::Not) {
                    emit(TacOp::NOT, result, operand, "");
                }
                return result;
            }
            case NodeType::CallExpr: {
                auto e = static_cast<CallExpr*>(expr);
                int i = 0;
                for (auto& arg : e->args) {
                    std::string arg_val = build_expr(arg.get());
                    emit(TacOp::PARAM, "a" + std::to_string(i), arg_val, "");
                    i++;
                }
                std::string result = current_func_->next_temp();
                emit(TacOp::CALL, result, e->func_name, "");
                return result;
            }
            case NodeType::VarExpr: {
                auto e = static_cast<VarExpr*>(expr);
                std::string result = current_func_->next_temp();
                emit(TacOp::LOAD, result, e->var_name, "");
                return result;
            }
            case NodeType::ConstExpr: {
                auto e = static_cast<ConstExpr*>(expr);
                std::string result = current_func_->next_temp();
                emit(TacOp::LOAD_IMM, result, std::to_string(e->const_value), "");
                return result;
            }
            default:
                return "0";
        }
    }

    void emit(TacOp op, const std::string& dest, const std::string& src1, const std::string& src2) {
        if (current_func_) {
            current_func_->instrs.emplace_back(op, dest, src1, src2);
        }
    }
};

#endif // IR_BUILDER_H
