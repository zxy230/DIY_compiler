%{
#include <memory>
#include <string>
#include <vector>
#include <cstdio>
#include "ast_node.h"

// External lexer function
extern int yylex();
extern int yylineno;
extern char* yytext;

// Error reporting
void yyerror(const char* s) {
    fprintf(stderr, "Line %d: %s near '%s'\n", yylineno, s, yytext);
}

// Global AST root
std::unique_ptr<Program> ast_root;

// Helper functions for creating nodes
std::unique_ptr<ExprNode> make_const(int val) {
    return std::make_unique<ConstExpr>(val);
}

std::unique_ptr<ExprNode> make_var(const std::string& name) {
    return std::make_unique<VarExpr>(name);
}

std::unique_ptr<ExprNode> make_call(const std::string& name, std::vector<std::unique_ptr<ExprNode>>& args) {
    auto call = std::make_unique<CallExpr>(name);
    call->args = std::move(args);
    return std::move(call);
}

std::unique_ptr<ExprNode> make_binary(OpType op,
    std::unique_ptr<ExprNode> left,
    std::unique_ptr<ExprNode> right) {
    return std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
}

std::unique_ptr<ExprNode> make_unary(OpType op, std::unique_ptr<ExprNode> operand) {
    return std::make_unique<UnaryExpr>(op, std::move(operand));
}

std::unique_ptr<StmtNode> make_decl(const std::string& name, std::unique_ptr<ExprNode> init) {
    return std::make_unique<DeclStmt>(name, std::move(init));
}

std::unique_ptr<StmtNode> make_assign(const std::string& name, std::unique_ptr<ExprNode> val) {
    return std::make_unique<AssignStmt>(name, std::move(val));
}

std::unique_ptr<StmtNode> make_expr_stmt(std::unique_ptr<ExprNode> expr) {
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<StmtNode> make_if(std::unique_ptr<ExprNode> cond,
    std::unique_ptr<StmtNode> then_stmt,
    std::unique_ptr<StmtNode> else_stmt = nullptr) {
    return std::make_unique<IfStmt>(std::move(cond), std::move(then_stmt), std::move(else_stmt));
}

std::unique_ptr<StmtNode> make_while(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> body) {
    return std::make_unique<WhileStmt>(std::move(cond), std::move(body));
}

std::unique_ptr<StmtNode> make_return(std::unique_ptr<ExprNode> val = nullptr) {
    return std::make_unique<ReturnStmt>(std::move(val));
}

std::unique_ptr<Block> make_block() {
    return std::make_unique<Block>();
}

%}

// Bison declarations
%union {
    int num;
    std::string* str;
    std::vector<std::unique_ptr<ExprNode>>* expr_list;
    std::vector<std::unique_ptr<ASTNode>>* param_list;
    ASTNode* node;
    ExprNode* expr;
    StmtNode* stmt;
    Block* block;
    FuncDef* func;
    Program* program;
}

%token INT VOID IF ELSE WHILE BREAK CONTINUE RETURN
%token <str> ID
%token <num> NUMBER

%token PLUS MINUS MUL DIV MOD LT GT LE GE EQ NE AND OR NOT ASSIGN
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMICOLON

%type <program> CompUnit
%type <func> FuncDef
%type <str> FuncType
%type <param_list> FuncFParams
%type <node> FuncFParam
%type <block> Block BlockItems
%type <stmt> Stmt
%type <expr> Expr LOrExpr LAndExpr RelExpr AddExpr MulExpr UnaryExpr PrimaryExpr
%type <expr_list> FuncRParams

%start CompUnit

%%

// CompUnit -> FuncDef+
CompUnit
    : CompUnit FuncDef {
        $1->funcs.push_back(std::unique_ptr<FuncDef>($2));
        $$ = $1;
    }
    | FuncDef {
        auto prog = std::make_unique<Program>();
        prog->funcs.push_back(std::unique_ptr<FuncDef>($1));
        $$ = prog.get();
        ast_root = std::move(prog);
    }
    ;

// FuncDef -> FuncType ID '(' FuncFParams? ')' Block
FuncDef
    : FuncType ID LPAREN FuncFParams RPAREN Block {
        $$ = new FuncDef(*$2, *$2);
        $$->return_type = *$1;
        $$->params = std::move(*$4);
        $$->body = std::unique_ptr<Block>($6);
        delete $1;
        delete $2;
    }
    | FuncType ID LPAREN RPAREN Block {
        $$ = new FuncDef(*$2, *$2);
        $$->return_type = *$1;
        $$->body = std::unique_ptr<Block>($5);
        delete $1;
        delete $2;
    }
    ;

FuncType
    : INT { $$ = new std::string("int"); }
    | VOID { $$ = new std::string("void"); }
    ;

// FuncFParams -> FuncFParam (',' FuncFParam)*
FuncFParams
    : FuncFParam {
        $$ = new std::vector<std::unique_ptr<ASTNode>>();
        $$->push_back(std::unique_ptr<ASTNode>($1));
    }
    | FuncFParams COMMA FuncFParam {
        $1->push_back(std::unique_ptr<ASTNode>($3));
        $$ = $1;
    }
    ;

// FuncFParam -> 'int' ID
FuncFParam
    : INT ID {
        auto param = new ASTNode(NodeType::Param);
        param->line_no = yylineno;
        // Store the name somewhere - using a special wrapper
        $$ = param;
    }
    ;

// Block -> '{' Stmt* '}'
Block
    : LBRACE BlockItems RBRACE {
        $$ = $2;
    }
    | LBRACE RBRACE {
        $$ = make_block().release();
    }
    ;

BlockItems
    : BlockItems Stmt {
        $1->stmts.push_back(std::unique_ptr<StmtNode>($2));
        $$ = $1;
    }
    | Stmt {
        auto block = make_block();
        block->stmts.push_back(std::unique_ptr<StmtNode>($1));
        $$ = block.release();
    }
    ;

// Stmt -> Block | ';' | Expr ';' | ID '=' Expr ';' | 'int' ID '=' Expr ';' | 'if' '(' Expr ')' Stmt ('else' Stmt)? | 'while' '(' Expr ')' Stmt | 'break' ';' | 'continue' ';' | 'return' Expr ';'
Stmt
    : Block {
        $$ = $1;
    }
    | SEMICOLON {
        // Empty statement - create a null statement
        $$ = new StmtNode(NodeType::ExprStmt);
    }
    | Expr SEMICOLON {
        $$ = make_expr_stmt(std::unique_ptr<ExprNode>($1)).release();
    }
    | ID ASSIGN Expr SEMICOLON {
        $$ = make_assign(*$1, std::unique_ptr<ExprNode>($3)).release();
        delete $1;
    }
    | INT ID ASSIGN Expr SEMICOLON {
        $$ = make_decl(*$2, std::unique_ptr<ExprNode>($4)).release();
        delete $2;
    }
    | IF LPAREN Expr RPAREN Stmt {
        $$ = make_if(std::unique_ptr<ExprNode>($3), std::unique_ptr<StmtNode>($5)).release();
    }
    | IF LPAREN Expr RPAREN Stmt ELSE Stmt {
        $$ = make_if(std::unique_ptr<ExprNode>($3), std::unique_ptr<StmtNode>($5), std::unique_ptr<StmtNode>($7)).release();
    }
    | WHILE LPAREN Expr RPAREN Stmt {
        $$ = make_while(std::unique_ptr<ExprNode>($3), std::unique_ptr<StmtNode>($5)).release();
    }
    | BREAK SEMICOLON {
        $$ = new BreakStmt();
    }
    | CONTINUE SEMICOLON {
        $$ = new ContinueStmt();
    }
    | RETURN Expr SEMICOLON {
        $$ = make_return(std::unique_ptr<ExprNode>($2)).release();
    }
    | RETURN SEMICOLON {
        $$ = make_return().release();
    }
    ;

// Expr -> LOrExpr
Expr
    : LOrExpr { $$ = $1; }
    ;

// LOrExpr -> LAndExpr | LOrExpr '||' LAndExpr
LOrExpr
    : LAndExpr { $$ = $1; }
    | LOrExpr OR LAndExpr {
        $$ = make_binary(OpType::Or, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    ;

// LAndExpr -> RelExpr | LAndExpr '&&' RelExpr
LAndExpr
    : RelExpr { $$ = $1; }
    | LAndExpr AND RelExpr {
        $$ = make_binary(OpType::And, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    ;

// RelExpr -> AddExpr | RelExpr ('<' | '>' | '<=' | '>=' | '==' | '!=') AddExpr
RelExpr
    : AddExpr { $$ = $1; }
    | RelExpr LT AddExpr {
        $$ = make_binary(OpType::Lt, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | RelExpr GT AddExpr {
        $$ = make_binary(OpType::Gt, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | RelExpr LE AddExpr {
        $$ = make_binary(OpType::Le, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | RelExpr GE AddExpr {
        $$ = make_binary(OpType::Ge, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | RelExpr EQ AddExpr {
        $$ = make_binary(OpType::Eq, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | RelExpr NE AddExpr {
        $$ = make_binary(OpType::Ne, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    ;

// AddExpr -> MulExpr | AddExpr ('+' | '-') MulExpr
AddExpr
    : MulExpr { $$ = $1; }
    | AddExpr PLUS MulExpr {
        $$ = make_binary(OpType::Add, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | AddExpr MINUS MulExpr {
        $$ = make_binary(OpType::Sub, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    ;

// MulExpr -> UnaryExpr | MulExpr ('*' | '/' | '%') UnaryExpr
MulExpr
    : UnaryExpr { $$ = $1; }
    | MulExpr MUL UnaryExpr {
        $$ = make_binary(OpType::Mul, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | MulExpr DIV UnaryExpr {
        $$ = make_binary(OpType::Div, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    | MulExpr MOD UnaryExpr {
        $$ = make_binary(OpType::Mod, std::unique_ptr<ExprNode>($1), std::unique_ptr<ExprNode>($3)).release();
    }
    ;

// UnaryExpr -> PrimaryExpr | ('+' | '-' | '!') UnaryExpr
UnaryExpr
    : PrimaryExpr { $$ = $1; }
    | PLUS UnaryExpr {
        $$ = make_unary(OpType::Pos, std::unique_ptr<ExprNode>($2)).release();
    }
    | MINUS UnaryExpr {
        $$ = make_unary(OpType::Neg, std::unique_ptr<ExprNode>($2)).release();
    }
    | NOT UnaryExpr {
        $$ = make_unary(OpType::Not, std::unique_ptr<ExprNode>($2)).release();
    }
    ;

// PrimaryExpr -> ID | NUMBER | '(' Expr ')' | ID '(' FuncRParams? ')'
PrimaryExpr
    : ID {
        $$ = make_var(*$1).release();
        delete $1;
    }
    | NUMBER {
        $$ = make_const($1).release();
    }
    | LPAREN Expr RPAREN {
        $$ = $2;
    }
    | ID LPAREN FuncRParams RPAREN {
        $$ = make_call(*$1, *$3).release();
        delete $1;
        delete $3;
    }
    | ID LPAREN RPAREN {
        auto args = new std::vector<std::unique_ptr<ExprNode>>();
        $$ = make_call(*$1, *args).release();
        delete $1;
        delete args;
    }
    ;

// FuncRParams -> Expr (',' Expr)*
FuncRParams
    : Expr {
        $$ = new std::vector<std::unique_ptr<ExprNode>>();
        $$->push_back(std::unique_ptr<ExprNode>($1));
    }
    | FuncRParams COMMA Expr {
        $1->push_back(std::unique_ptr<ExprNode>($3));
        $$ = $1;
    }
    ;

%%
