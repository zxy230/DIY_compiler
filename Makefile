# ToyC Compiler Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I. -I./src -I./src/parser -I./src/semantic -I./src/ir -I./src/codegen -I./src/optimizer
LDFLAGS =

# Flex/Bison
FLEX = flex
BISON = bison

# Directories
SRC_DIR = src
LEXER_DIR = $(SRC_DIR)/lexer
PARSER_DIR = $(SRC_DIR)/parser
SEMANTIC_DIR = $(SRC_DIR)/semantic
IR_DIR = $(SRC_DIR)/ir
CODEGEN_DIR = $(SRC_DIR)/codegen
OPTIMIZER_DIR = $(SRC_DIR)/optimizer

# Source files
LEXER_SRC = $(LEXER_DIR)/lexer.l
PARSER_SRC = $(PARSER_DIR)/parser.y

# Generated files (in parser dir)
LEXER_OUT = $(LEXER_DIR)/lexer.yy.cpp
PARSER_CPP = $(PARSER_DIR)/parser.tab.cpp
PARSER_H = $(PARSER_DIR)/parser.tab.h

# C++ source files (excluding main.cpp which needs parser header)
CPP_SRCS = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))

# Object files (generated in src dir)
OBJS = $(CPP_SRCS:.cpp=.o) $(OPTIMIZER_DIR)/optimizer.o parser.tab.o lexer.yy.o main.o

# Target
TARGET = toycc

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(LEXER_OUT) $(PARSER_CPP) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Flex: lexer.l -> lexer.yy.cpp
$(LEXER_OUT): $(LEXER_SRC)
	$(FLEX) -o $@ $<

# Bison: parser.y -> parser.tab.cpp, parser.tab.h
# Run bison once and generate both files
$(PARSER_CPP) $(PARSER_H): $(PARSER_SRC)
	cd $(PARSER_DIR) && $(BISON) -d -o parser.tab.cpp parser.y
	# Rename .hpp to .h and add necessary includes
	mv $(PARSER_DIR)/parser.tab.hpp $(PARSER_H) 2>/dev/null || true
	sed -i '90 i #include <vector>\n#include <memory>\n#include "ast_node.h"' $(PARSER_H)
	# Fix include path in .cpp
	sed -i 's|#include "parser.tab.hpp"|#include "parser/parser.tab.h"|' $(PARSER_CPP)

# Compile C++ files
parser.tab.o: $(PARSER_CPP) $(PARSER_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

lexer.yy.o: $(LEXER_OUT) $(PARSER_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Object files go in src/
main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/codegen/riscv32.h $(SRC_DIR)/ir/tac.h $(SRC_DIR)/ir/builder.h $(SRC_DIR)/semantic/analyzer.h $(SRC_DIR)/optimizer/optimizer.h $(PARSER_H)
	$(CXX) $(CXXFLAGS) -c $< -o $@

optimizer.o: $(OPTIMIZER_DIR)/optimizer.cpp $(OPTIMIZER_DIR)/optimizer.h $(SRC_DIR)/ir/tac.h
	$(CXX) $(CXXFLAGS) -c $(OPTIMIZER_DIR)/optimizer.cpp -o $@

clean:
	rm -f $(TARGET) $(LEXER_OUT) $(PARSER_CPP) $(PARSER_H) *.o $(SRC_DIR)/*.o $(SRC_DIR)/ir/cfg.h.gch
