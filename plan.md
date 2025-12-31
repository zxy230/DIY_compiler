改进plan:
一. 建议将项目前后端分离：

前端：
Lexical Analysis: Breaks code into meaningful tokens (words, symbols).
Syntax Analysis (Parsing): Checks if the code follows the language's grammar rules.
Semantic Analysis: Checks for meaning and logical errors (e.g., type mismatches).

后端：
Optimization: Improves the code for better performance (speed, size).
Code Generation: Produces the final target code (assembly, machine code). 

二、思考下面几个问题：
前端后端之间是使用什么数据结构连接的？
生成中间代码的形式是什么样的？
中间代码如何优化？如何表示（转变）为汇编码？
语法分析和语义分析应该是一起的:语法制导翻译!

数据驱动不同阶段：每一阶段的输入和输出都应清晰
词法分析：输入一段c的程序 输出：类似tokens
语法分析：输入tokens 输出语法树
语法树的构建可以采用自上而下也可以采用自下而上
建议采用自下而上（LR(1)文法）
无论哪种方法都要考虑toyc的文法（上下文无关文法）
Maybe:
编译单元 CompUnit → FuncDef+
语句 Stmt → Block | “;” | Expr “;” | ID “=” Expr “;”
           | “int” ID “=” Expr “;”
           | “if ” “(” Expr “)” Stmt (“else” Stmt)?
           | “while” “(” Expr “)” Stmt
           | “break” “;” | “continue” “;” | “return” Expr “;”
语句块 Block → “{” Stmt* “}”
函数定义 FuncDef → (“int” | “void”) ID “(” (Param (“,” Param)*)? “)” Block
形参 Param → “int” ID
表达式 Expr → LOrExpr
逻辑或表达式 LOrExpr → LAndExpr | LOrExpr “||” LAndExpr
逻辑与表达式 LAndExpr → RelExpr | LAndExpr “&&” RelExpr
关系表达式 RelExpr → AddExpr
                | RelExpr (“<” | “>” | “<=” | “>=” | “==” | “!=”) AddExpr
加减表达式 AddExpr → MulExpr
                | AddExpr (“+” | “-”) MulExpr
乘除模表达式 MulExpr → UnaryExpr
                | MulExpr (“*” | “/” | “%”) UnaryExpr
一元表达式 UnaryExpr → PrimaryExpr
                | (“+” | “-” | “!”) UnaryExpr
基本表达式 PrimaryExpr → ID | NUMBER | “(” Expr “)”
                | ID “(” (Expr (“,” Expr)*)? “)”

三、可以借鉴clang LLVM等c编译器项目的良好的设计思路，并提取出来，用于该项目的设计和改进

四、关于最后的代码生成（汇编代码生成），我得到的信息如下：不明确的地方（比如哪些数据应该放在栈中，哪些放在寄存器中，哪些栈空间由调用者分配，哪些由被调用者分配等等）这些内容我们可以先沟通再实现
必须实现寄存器分配（register allocation）。目标架构为 RISC-V 32-bit，请勿生成任何 64-bit 指令或假定 64-bit 寄存器宽度。

寄存器分配算法可选用图着色（graph coloring）或线性扫描（linear scan）等常见方法。参考教材：

《Compilers: Principles, Techniques, and Tools》（龙书）

《Modern Compiler Implementation in C》 （虎书）

在实现寄存器分配时，注意：通用寄存器数量有限（RV32 通常有 x1..x31，其中部分寄存器为约定用途），要正确处理调用约定（函数参数、返回值、被调保存、调用者保存寄存器）。同时考虑栈溢出/spill 处理和保存/恢复策略。