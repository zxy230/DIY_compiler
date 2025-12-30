#ifndef AST_NODE_H
#define AST_NODE_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations
class ASTNode;
class ExprNode;
class StmtNode;

// Base node types
enum class NodeType {
    Program,
    FuncDef,
    Block,
    DeclStmt,
    AssignStmt,
    ExprStmt,
    IfStmt,
    WhileStmt,
    ReturnStmt,
    BreakStmt,
    ContinueStmt,
    BinaryExpr,
    UnaryExpr,
    CallExpr,
    VarExpr,
    ConstExpr,
    Param
};

// Operator types
enum class OpType {
    Add, Sub, Mul, Div, Mod,
    Lt, Gt, Le, Ge, Eq, Ne,
    And, Or,
    Neg, Pos, Not
};

class ASTNode {
public:
    NodeType type;
    int line_no;
    ASTNode(NodeType t) : type(t), line_no(0) {}
    virtual ~ASTNode() = default;
};

class ExprNode : public ASTNode {
public:
    std::string value_name;  // For code generation
    bool is_constant;
    int const_value;

    ExprNode(NodeType t) : ASTNode(t), is_constant(false), const_value(0) {}
};

class StmtNode : public ASTNode {
public:
    StmtNode(NodeType t) : ASTNode(t) {}
};

// Expression nodes
class BinaryExpr : public ExprNode {
public:
    OpType op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;

    BinaryExpr(OpType oper, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : ExprNode(NodeType::BinaryExpr), op(oper), left(std::move(l)), right(std::move(r)) {}
};

class UnaryExpr : public ExprNode {
public:
    OpType op;
    std::unique_ptr<ExprNode> operand;

    UnaryExpr(OpType oper, std::unique_ptr<ExprNode> opnd)
        : ExprNode(NodeType::UnaryExpr), op(oper), operand(std::move(opnd)) {}
};

class CallExpr : public ExprNode {
public:
    std::string func_name;
    std::vector<std::unique_ptr<ExprNode>> args;

    CallExpr(const std::string& name) : ExprNode(NodeType::CallExpr), func_name(name) {}
};

class VarExpr : public ExprNode {
public:
    std::string var_name;

    VarExpr(const std::string& name) : ExprNode(NodeType::VarExpr), var_name(name) {}
};

class ConstExpr : public ExprNode {
public:
    ConstExpr(int val) : ExprNode(NodeType::ConstExpr) {
        is_constant = true;
        const_value = val;
    }
};

// Statement nodes
class DeclStmt : public StmtNode {
public:
    std::string var_name;
    std::unique_ptr<ExprNode> init_expr;

    DeclStmt(const std::string& name, std::unique_ptr<ExprNode> init)
        : StmtNode(NodeType::DeclStmt), var_name(name), init_expr(std::move(init)) {}
};

class AssignStmt : public StmtNode {
public:
    std::string var_name;
    std::unique_ptr<ExprNode> value;

    AssignStmt(const std::string& name, std::unique_ptr<ExprNode> val)
        : StmtNode(NodeType::AssignStmt), var_name(name), value(std::move(val)) {}
};

class ExprStmt : public StmtNode {
public:
    std::unique_ptr<ExprNode> expr;

    ExprStmt(std::unique_ptr<ExprNode> e) : StmtNode(NodeType::ExprStmt), expr(std::move(e)) {}
};

class IfStmt : public StmtNode {
public:
    std::unique_ptr<ExprNode> cond;
    std::unique_ptr<StmtNode> then_stmt;
    std::unique_ptr<StmtNode> else_stmt;  // May be nullptr

    IfStmt(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> t, std::unique_ptr<StmtNode> e = nullptr)
        : StmtNode(NodeType::IfStmt), cond(std::move(c)), then_stmt(std::move(t)), else_stmt(std::move(e)) {}
};

class WhileStmt : public StmtNode {
public:
    std::unique_ptr<ExprNode> cond;
    std::unique_ptr<StmtNode> body;

    WhileStmt(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> b)
        : StmtNode(NodeType::WhileStmt), cond(std::move(c)), body(std::move(b)) {}
};

class ReturnStmt : public StmtNode {
public:
    std::unique_ptr<ExprNode> value;  // nullptr for void return

    ReturnStmt(std::unique_ptr<ExprNode> v = nullptr) : StmtNode(NodeType::ReturnStmt), value(std::move(v)) {}
};

class BreakStmt : public StmtNode {
public:
    BreakStmt() : StmtNode(NodeType::BreakStmt) {}
};

class ContinueStmt : public StmtNode {
public:
    ContinueStmt() : StmtNode(NodeType::ContinueStmt) {}
};

class Block : public StmtNode {
public:
    std::vector<std::unique_ptr<StmtNode>> stmts;

    Block() : StmtNode(NodeType::Block) {}
};

class FuncDef : public ASTNode {
public:
    std::string return_type;  // "int" or "void"
    std::string func_name;
    std::vector<std::unique_ptr<ASTNode>> params;
    std::unique_ptr<Block> body;

    FuncDef(const std::string& ret, const std::string& name)
        : ASTNode(NodeType::FuncDef), return_type(ret), func_name(name), body(nullptr) {}
};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<FuncDef>> funcs;

    Program() : ASTNode(NodeType::Program) {}
};

#endif // AST_NODE_H
