# ToyC Compiler Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I. -I./src -I./src/parser -I./src/semantic -I./src/ir -I./src/codegen
LDFLAGS = -lfl

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

# C++ source files
CPP_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
CPP_SRCS += $(wildcard $(SEMANTIC_DIR)/*.h)
CPP_SRCS += $(wildcard $(IR_DIR)/*.h)
CPP_SRCS += $(wildcard $(CODEGEN_DIR)/*.h)
CPP_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(CPP_SRCS))

# Object files
OBJS = $(CPP_SRCS:.cpp=.o) main.o parser.tab.o lexer.yy.o

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
$(PARSER_CPP) $(PARSER_H): $(PARSER_SRC)
	cd $(PARSER_DIR) && $(BISON) -d -o parser.tab.cpp parser.y
	mv $(PARSER_DIR)/parser.tab.hpp $(PARSER_H) 2>/dev/null || true

# Compile C++ files
parser.tab.o: $(PARSER_CPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

lexer.yy.o: $(LEXER_OUT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(LEXER_OUT) $(PARSER_CPP) $(PARSER_H) *.o $(SRC_DIR)/*.o
