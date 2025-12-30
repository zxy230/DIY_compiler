#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

// Symbol types
enum class SymbolKind {
    Variable,
    Function,
    Parameter
};

// Basic types
enum class TypeKind {
    Int,
    Void
};

struct Type {
    TypeKind kind;
    bool is_const;

    Type(TypeKind k = TypeKind::Int, bool c = false) : kind(k), is_const(c) {}

    std::string to_string() const {
        return kind == TypeKind::Int ? "int" : "void";
    }

    bool operator==(const Type& other) const {
        return kind == other.kind && is_const == other.is_const;
    }
};

// Symbol information
struct Symbol {
    std::string name;
    SymbolKind kind;
    Type type;
    int offset;           // Stack offset for variables/params
    int scope_level;      // Scope level where defined
    int func_index;       // For params: index in function param list
    bool is_global;      // Whether this is a global variable

    Symbol(const std::string& n, SymbolKind k, const Type& t, int lvl = 0)
        : name(n), kind(k), type(t), offset(0), scope_level(lvl), func_index(-1), is_global(false) {}
};

class SymbolTable {
public:
    // Enter a new scope
    void enter_scope() {
        scope_stack_.emplace_back();
        scope_level_++;
    }

    // Exit current scope
    void exit_scope() {
        if (!scope_stack_.empty()) {
            scope_stack_.pop_back();
            scope_level_--;
        }
    }

    // Get current scope level
    int current_scope() const { return scope_level_; }

    // Declare a symbol in current scope
    bool declare(const Symbol& sym) {
        auto& current = scope_stack_.back();
        if (current.find(sym.name) != current.end()) {
            return false;  // Already declared
        }
        Symbol new_sym = sym;  // Make a copy
        new_sym.scope_level = scope_level_;
        current[sym.name] = std::make_shared<Symbol>(new_sym);
        return true;
    }

    // Lookup a symbol (search from innermost to outermost scope)
    std::shared_ptr<Symbol> lookup(const std::string& name) {
        for (auto it = scope_stack_.rbegin(); it != scope_stack_.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return found->second;
            }
        }
        return nullptr;  // Not found
    }

    // Check if symbol exists in current scope
    bool exists_in_current(const std::string& name) {
        auto& current = scope_stack_.back();
        return current.find(name) != current.end();
    }

    // Get all symbols in current scope (for cleanup)
    std::vector<std::shared_ptr<Symbol>> get_current_symbols() {
        std::vector<std::shared_ptr<Symbol>> result;
        if (!scope_stack_.empty()) {
            for (auto& p : scope_stack_.back()) {
                result.push_back(p.second);
            }
        }
        return result;
    }

    // Clear all scopes
    void clear() {
        scope_stack_.clear();
        scope_level_ = -1;  // Start at -1 so first enter_scope gives 0
    }

private:
    std::vector<std::unordered_map<std::string, std::shared_ptr<Symbol>>> scope_stack_;
    int scope_level_ = -1;
};

// Exception for semantic errors
class SemanticError : public std::runtime_error {
public:
    SemanticError(const std::string& msg, int line = 0)
        : std::runtime_error(msg + (line > 0 ? " at line " + std::to_string(line) : "")) {}
};

#endif // SYMBOL_H
