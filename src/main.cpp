#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <cstring>
#include <stdexcept>

// Include generated parser header
#include "parser/parser.tab.h"
#include "parser/ast_node.h"

// Components
#include "semantic/symbol.h"
#include "semantic/analyzer.h"
#include "ir/builder.h"
#include "codegen/riscv32.h"

// External declarations
extern std::unique_ptr<Program> ast_root;
extern FILE* yyin;
extern int yyparse();

// Command line options
bool opt_enabled = false;
std::string input_file;

void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " [-opt] [input_file]\n";
    std::cerr << "  -opt    Enable optimizations\n";
    std::cerr << "  input   Input file (default: stdin)\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-opt") == 0) {
            opt_enabled = true;
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        }
    }

    // Open input file or use stdin
    if (!input_file.empty()) {
        yyin = fopen(input_file.c_str(), "r");
        if (!yyin) {
            std::cerr << "Error: Cannot open input file: " << input_file << "\n";
            return 1;
        }
    }

    // Parse the input
    if (yyparse() != 0) {
        std::cerr << "Parsing failed\n";
        return 1;
    }

    // Close input file
    if (yyin) {
        fclose(yyin);
    }

    if (!ast_root) {
        std::cerr << "Error: No AST generated\n";
        return 1;
    }

    try {
        // Semantic analysis
        SemanticAnalyzer analyzer;
        analyzer.analyze(ast_root.get());

        // IR generation
        IRBuilder ir_builder;
        ProgramIR* ir = ir_builder.build(ast_root.get());

        // Code generation
        RISC32Generator generator(ir);
        std::string asm_code = generator.generate();

        // Output
        std::cout << ".text\n\n";
        std::cout << ".globl main\n\n";
        std::cout << asm_code;

    } catch (const SemanticError& e) {
        std::cerr << "Semantic error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
