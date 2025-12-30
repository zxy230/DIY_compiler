#ifndef ANALYZER_H
#define ANALYZER_H

#include "ast_node.h"
#include "symbol.h"
#include <iostream>
#include <unordered_map>

class SemanticAnalyzer {
public:
    SemanticAnalyzer() {
        symbol_table_.clear();
        symbol_table_.enter_scope();  // Global scope

        // Pre-declare test framework functions
        declare_intrinsic("EVAL", TypeKind::Int);
        declare_intrinsic("randint", TypeKind::Int);
        declare_intrinsic("EXEC", TypeKind::Void);
    }

    void declare_intrinsic(const std::string& name, TypeKind ret_type) {
        if (!symbol_table_.exists_in_current(name)) {
            Symbol sym(name, SymbolKind::Function, Type(ret_type));
            sym.is_global = true;
            symbol_table_.declare(sym);
        }
    }

    void analyze(Program* program) {

        // First pass: collect function declarations
        for (auto& func : program->funcs) {
            analyze_func_header(func.get());
        }

        // Second pass: analyze function bodies
        for (auto& func : program->funcs) {
            analyze_func_body(func.get());
        }
    }

private:
    SymbolTable symbol_table_;

    void analyze_func_header(FuncDef* func) {
        if (symbol_table_.exists_in_current(func->func_name)) {
            throw SemanticError("Function '" + func->func_name + "' already declared");
        }

        TypeKind ret_type = (func->return_type == "int") ? TypeKind::Int : TypeKind::Void;
        Symbol sym(func->func_name, SymbolKind::Function, Type(ret_type));
        sym.is_global = true;

        if (!symbol_table_.declare(sym)) {
            throw SemanticError("Failed to declare function '" + func->func_name + "'");
        }
    }

    void analyze_func_body(FuncDef* func) {
        symbol_table_.enter_scope();

        // Analyze function body
        analyze_block(func->body.get());

        symbol_table_.exit_scope();
    }

    void analyze_block(Block* block) {
        symbol_table_.enter_scope();
        for (auto& stmt : block->stmts) {
            analyze_stmt(stmt.get());
        }
        symbol_table_.exit_scope();
    }

    void analyze_stmt(StmtNode* stmt) {
        switch (stmt->type) {
            case NodeType::Block:
                analyze_block(static_cast<Block*>(stmt));
                break;
            case NodeType::DeclStmt:
                analyze_decl(static_cast<DeclStmt*>(stmt));
                break;
            case NodeType::AssignStmt:
                analyze_assign(static_cast<AssignStmt*>(stmt));
                break;
            case NodeType::ExprStmt:
                if (static_cast<ExprStmt*>(stmt)->expr) {
                    analyze_expr(static_cast<ExprStmt*>(stmt)->expr.get());
                }
                break;
            case NodeType::IfStmt:
                analyze_if(static_cast<IfStmt*>(stmt));
                break;
            case NodeType::WhileStmt:
                analyze_while(static_cast<WhileStmt*>(stmt));
                break;
            case NodeType::ReturnStmt:
                // Check return type matches function return type
                break;
            default:
                break;
        }
    }

    void analyze_decl(DeclStmt* stmt) {
        if (symbol_table_.exists_in_current(stmt->var_name)) {
            throw SemanticError("Variable '" + stmt->var_name + "' already declared");
        }

        Symbol sym(stmt->var_name, SymbolKind::Variable, Type(TypeKind::Int));
        if (!symbol_table_.declare(sym)) {
            throw SemanticError("Failed to declare variable '" + stmt->var_name + "'");
        }

        // Analyze initializer
        analyze_expr(stmt->init_expr.get());
    }

    void analyze_assign(AssignStmt* stmt) {
        auto sym = symbol_table_.lookup(stmt->var_name);
        if (!sym) {
            throw SemanticError("Undeclared variable '" + stmt->var_name + "'");
        }
        analyze_expr(stmt->value.get());
    }

    void analyze_if(IfStmt* stmt) {
        analyze_expr(stmt->cond.get());
        analyze_stmt(stmt->then_stmt.get());
        if (stmt->else_stmt) {
            analyze_stmt(stmt->else_stmt.get());
        }
    }

    void analyze_while(WhileStmt* stmt) {
        analyze_expr(stmt->cond.get());
        analyze_stmt(stmt->body.get());
    }

    void analyze_expr(ExprNode* expr) {
        switch (expr->type) {
            case NodeType::BinaryExpr: {
                auto e = static_cast<BinaryExpr*>(expr);
                analyze_expr(e->left.get());
                analyze_expr(e->right.get());
                break;
            }
            case NodeType::UnaryExpr: {
                auto e = static_cast<UnaryExpr*>(expr);
                analyze_expr(e->operand.get());
                break;
            }
            case NodeType::CallExpr: {
                auto e = static_cast<CallExpr*>(expr);
                auto sym = symbol_table_.lookup(e->func_name);
                if (!sym || sym->kind != SymbolKind::Function) {
                    throw SemanticError("Undeclared function '" + e->func_name + "'");
                }
                for (auto& arg : e->args) {
                    analyze_expr(arg.get());
                }
                break;
            }
            case NodeType::VarExpr: {
                auto e = static_cast<VarExpr*>(expr);
                auto sym = symbol_table_.lookup(e->var_name);
                if (!sym) {
                    throw SemanticError("Undeclared variable '" + e->var_name + "'");
                }
                break;
            }
            case NodeType::ConstExpr:
                // Constants are always valid
                break;
            default:
                break;
        }
    }
};

#endif // ANALYZER_H
