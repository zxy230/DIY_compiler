/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser/parser.y"

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


#line 148 "src/parser/parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INT = 3,                        /* INT  */
  YYSYMBOL_VOID = 4,                       /* VOID  */
  YYSYMBOL_IF = 5,                         /* IF  */
  YYSYMBOL_ELSE = 6,                       /* ELSE  */
  YYSYMBOL_WHILE = 7,                      /* WHILE  */
  YYSYMBOL_BREAK = 8,                      /* BREAK  */
  YYSYMBOL_CONTINUE = 9,                   /* CONTINUE  */
  YYSYMBOL_RETURN = 10,                    /* RETURN  */
  YYSYMBOL_ID = 11,                        /* ID  */
  YYSYMBOL_NUMBER = 12,                    /* NUMBER  */
  YYSYMBOL_PLUS = 13,                      /* PLUS  */
  YYSYMBOL_MINUS = 14,                     /* MINUS  */
  YYSYMBOL_MUL = 15,                       /* MUL  */
  YYSYMBOL_DIV = 16,                       /* DIV  */
  YYSYMBOL_MOD = 17,                       /* MOD  */
  YYSYMBOL_LT = 18,                        /* LT  */
  YYSYMBOL_GT = 19,                        /* GT  */
  YYSYMBOL_LE = 20,                        /* LE  */
  YYSYMBOL_GE = 21,                        /* GE  */
  YYSYMBOL_EQ = 22,                        /* EQ  */
  YYSYMBOL_NE = 23,                        /* NE  */
  YYSYMBOL_AND = 24,                       /* AND  */
  YYSYMBOL_OR = 25,                        /* OR  */
  YYSYMBOL_NOT = 26,                       /* NOT  */
  YYSYMBOL_ASSIGN = 27,                    /* ASSIGN  */
  YYSYMBOL_LPAREN = 28,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 29,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 30,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 31,                    /* RBRACE  */
  YYSYMBOL_COMMA = 32,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 33,                 /* SEMICOLON  */
  YYSYMBOL_YYACCEPT = 34,                  /* $accept  */
  YYSYMBOL_CompUnit = 35,                  /* CompUnit  */
  YYSYMBOL_FuncDef = 36,                   /* FuncDef  */
  YYSYMBOL_FuncType = 37,                  /* FuncType  */
  YYSYMBOL_FuncFParams = 38,               /* FuncFParams  */
  YYSYMBOL_FuncFParam = 39,                /* FuncFParam  */
  YYSYMBOL_Block = 40,                     /* Block  */
  YYSYMBOL_BlockItems = 41,                /* BlockItems  */
  YYSYMBOL_Stmt = 42,                      /* Stmt  */
  YYSYMBOL_Expr = 43,                      /* Expr  */
  YYSYMBOL_LOrExpr = 44,                   /* LOrExpr  */
  YYSYMBOL_LAndExpr = 45,                  /* LAndExpr  */
  YYSYMBOL_RelExpr = 46,                   /* RelExpr  */
  YYSYMBOL_AddExpr = 47,                   /* AddExpr  */
  YYSYMBOL_MulExpr = 48,                   /* MulExpr  */
  YYSYMBOL_UnaryExpr = 49,                 /* UnaryExpr  */
  YYSYMBOL_PrimaryExpr = 50,               /* PrimaryExpr  */
  YYSYMBOL_FuncRParams = 51                /* FuncRParams  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   142

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  56
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  110

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   288


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   115,   115,   119,   129,   137,   147,   148,   153,   157,
     165,   175,   178,   184,   188,   197,   200,   204,   207,   211,
     215,   218,   221,   224,   227,   230,   233,   240,   245,   246,
     253,   254,   261,   262,   265,   268,   271,   274,   277,   284,
     285,   288,   295,   296,   299,   302,   309,   310,   313,   316,
     323,   327,   330,   333,   338,   348,   352
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INT", "VOID", "IF",
  "ELSE", "WHILE", "BREAK", "CONTINUE", "RETURN", "ID", "NUMBER", "PLUS",
  "MINUS", "MUL", "DIV", "MOD", "LT", "GT", "LE", "GE", "EQ", "NE", "AND",
  "OR", "NOT", "ASSIGN", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "COMMA",
  "SEMICOLON", "$accept", "CompUnit", "FuncDef", "FuncType", "FuncFParams",
  "FuncFParam", "Block", "BlockItems", "Stmt", "Expr", "LOrExpr",
  "LAndExpr", "RelExpr", "AddExpr", "MulExpr", "UnaryExpr", "PrimaryExpr",
  "FuncRParams", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-34)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      91,   -34,   -34,    41,   -34,    -3,   -34,   -34,   -23,     4,
      -1,   -18,    22,   -34,   -34,     6,   -34,   -18,    35,    32,
      28,    49,    13,    52,    14,    -6,   -34,    89,    89,    89,
      89,   -34,   -34,   -34,    50,   -34,    71,    81,    84,    51,
     114,   109,   -34,   -34,   -34,   -34,    83,    89,    89,   -34,
     -34,    88,   -34,    98,    89,    85,   -34,   -34,   -34,   103,
     -34,   -34,   -34,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,   104,   105,   -34,
     102,   -34,   -34,    36,   -34,    84,    51,   114,   114,   114,
     114,   114,   114,   109,   109,   -34,   -34,   -34,   106,    79,
      79,   -34,   -34,    89,   -34,   130,   -34,   -34,    79,   -34
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     6,     7,     0,     3,     0,     1,     2,     0,     0,
       0,     0,     0,     8,    10,     0,     5,     0,     0,     0,
       0,     0,     0,     0,     0,    50,    51,     0,     0,     0,
       0,    12,    16,    15,     0,    14,     0,    27,    28,    30,
      32,    39,    42,    46,     4,     9,     0,     0,     0,    23,
      24,    50,    26,     0,     0,     0,    47,    48,    49,     0,
      11,    13,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
       0,    54,    55,     0,    52,    29,    31,    33,    34,    35,
      36,    37,    38,    40,    41,    43,    44,    45,     0,     0,
       0,    18,    53,     0,    19,    20,    22,    56,     0,    21
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -34,   -34,   134,   -34,   -34,   120,    18,   -34,   -33,   -24,
     -34,    77,    78,    53,    58,   -25,   -34,   -34
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     3,     4,     5,    12,    13,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    83
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      53,    61,    56,    57,    58,     9,    59,    10,     8,    19,
      14,    20,    15,    21,    22,    23,    24,    25,    26,    27,
      28,    54,    55,    77,    78,    51,    26,    27,    28,    16,
      80,    82,    29,    11,    30,    44,    15,    31,    10,    32,
      29,     6,    30,    46,     1,     2,    49,    52,    95,    96,
      97,    17,    98,    19,    18,    20,    47,    21,    22,    23,
      24,    25,    26,    27,    28,   102,   105,   106,   103,    65,
      66,    67,    68,    69,    70,   109,    29,    48,    30,   107,
      15,    60,    19,    32,    20,    50,    21,    22,    23,    24,
      25,    26,    27,    28,     1,     2,    51,    26,    27,    28,
      51,    26,    27,    28,    62,    29,    63,    30,    64,    15,
      76,    29,    32,    30,    81,    29,    55,    30,    87,    88,
      89,    90,    91,    92,    73,    74,    75,    71,    72,    93,
      94,    79,    84,    99,   100,   101,   108,     7,    45,   104,
      85,     0,    86
};

static const yytype_int8 yycheck[] =
{
      24,    34,    27,    28,    29,    28,    30,     3,    11,     3,
      11,     5,    30,     7,     8,     9,    10,    11,    12,    13,
      14,    27,    28,    47,    48,    11,    12,    13,    14,    11,
      54,    55,    26,    29,    28,    17,    30,    31,     3,    33,
      26,     0,    28,    11,     3,     4,    33,    33,    73,    74,
      75,    29,    76,     3,    32,     5,    28,     7,     8,     9,
      10,    11,    12,    13,    14,    29,    99,   100,    32,    18,
      19,    20,    21,    22,    23,   108,    26,    28,    28,   103,
      30,    31,     3,    33,     5,    33,     7,     8,     9,    10,
      11,    12,    13,    14,     3,     4,    11,    12,    13,    14,
      11,    12,    13,    14,    33,    26,    25,    28,    24,    30,
      27,    26,    33,    28,    29,    26,    28,    28,    65,    66,
      67,    68,    69,    70,    15,    16,    17,    13,    14,    71,
      72,    33,    29,    29,    29,    33,     6,     3,    18,    33,
      63,    -1,    64
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,    35,    36,    37,     0,    36,    11,    28,
       3,    29,    38,    39,    11,    30,    40,    29,    32,     3,
       5,     7,     8,     9,    10,    11,    12,    13,    14,    26,
      28,    31,    33,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    40,    39,    11,    28,    28,    33,
      33,    11,    33,    43,    27,    28,    49,    49,    49,    43,
      31,    42,    33,    25,    24,    18,    19,    20,    21,    22,
      23,    13,    14,    15,    16,    17,    27,    43,    43,    33,
      43,    29,    43,    51,    29,    45,    46,    47,    47,    47,
      47,    47,    47,    48,    48,    49,    49,    49,    43,    29,
      29,    33,    29,    32,    33,    42,    42,    43,     6,    42
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    34,    35,    35,    36,    36,    37,    37,    38,    38,
      39,    40,    40,    41,    41,    42,    42,    42,    42,    42,
      42,    42,    42,    42,    42,    42,    42,    43,    44,    44,
      45,    45,    46,    46,    46,    46,    46,    46,    46,    47,
      47,    47,    48,    48,    48,    48,    49,    49,    49,    49,
      50,    50,    50,    50,    50,    51,    51
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     6,     5,     1,     1,     1,     3,
       2,     3,     2,     2,     1,     1,     1,     2,     4,     5,
       5,     7,     5,     2,     2,     3,     2,     1,     1,     3,
       1,     3,     1,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     1,     2,     2,     2,
       1,     1,     3,     4,     3,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* CompUnit: CompUnit FuncDef  */
#line 115 "src/parser/parser.y"
                       {
        (yyvsp[-1].program)->funcs.push_back(std::unique_ptr<FuncDef>((yyvsp[0].func)));
        (yyval.program) = (yyvsp[-1].program);
    }
#line 1259 "src/parser/parser.tab.cpp"
    break;

  case 3: /* CompUnit: FuncDef  */
#line 119 "src/parser/parser.y"
              {
        auto prog = std::make_unique<Program>();
        prog->funcs.push_back(std::unique_ptr<FuncDef>((yyvsp[0].func)));
        (yyval.program) = prog.get();
        ast_root = std::move(prog);
    }
#line 1270 "src/parser/parser.tab.cpp"
    break;

  case 4: /* FuncDef: FuncType ID LPAREN FuncFParams RPAREN Block  */
#line 129 "src/parser/parser.y"
                                                  {
        (yyval.func) = new FuncDef(*(yyvsp[-4].str), *(yyvsp[-4].str));
        (yyval.func)->return_type = *(yyvsp[-5].str);
        (yyval.func)->params = std::move(*(yyvsp[-2].param_list));
        (yyval.func)->body = std::unique_ptr<Block>((yyvsp[0].block));
        delete (yyvsp[-5].str);
        delete (yyvsp[-4].str);
    }
#line 1283 "src/parser/parser.tab.cpp"
    break;

  case 5: /* FuncDef: FuncType ID LPAREN RPAREN Block  */
#line 137 "src/parser/parser.y"
                                      {
        (yyval.func) = new FuncDef(*(yyvsp[-3].str), *(yyvsp[-3].str));
        (yyval.func)->return_type = *(yyvsp[-4].str);
        (yyval.func)->body = std::unique_ptr<Block>((yyvsp[0].block));
        delete (yyvsp[-4].str);
        delete (yyvsp[-3].str);
    }
#line 1295 "src/parser/parser.tab.cpp"
    break;

  case 6: /* FuncType: INT  */
#line 147 "src/parser/parser.y"
          { (yyval.str) = new std::string("int"); }
#line 1301 "src/parser/parser.tab.cpp"
    break;

  case 7: /* FuncType: VOID  */
#line 148 "src/parser/parser.y"
           { (yyval.str) = new std::string("void"); }
#line 1307 "src/parser/parser.tab.cpp"
    break;

  case 8: /* FuncFParams: FuncFParam  */
#line 153 "src/parser/parser.y"
                 {
        (yyval.param_list) = new std::vector<std::unique_ptr<ASTNode>>();
        (yyval.param_list)->push_back(std::unique_ptr<ASTNode>((yyvsp[0].node)));
    }
#line 1316 "src/parser/parser.tab.cpp"
    break;

  case 9: /* FuncFParams: FuncFParams COMMA FuncFParam  */
#line 157 "src/parser/parser.y"
                                   {
        (yyvsp[-2].param_list)->push_back(std::unique_ptr<ASTNode>((yyvsp[0].node)));
        (yyval.param_list) = (yyvsp[-2].param_list);
    }
#line 1325 "src/parser/parser.tab.cpp"
    break;

  case 10: /* FuncFParam: INT ID  */
#line 165 "src/parser/parser.y"
             {
        auto param = new ASTNode(NodeType::Param);
        param->line_no = yylineno;
        // Store the name somewhere - using a special wrapper
        (yyval.node) = param;
    }
#line 1336 "src/parser/parser.tab.cpp"
    break;

  case 11: /* Block: LBRACE BlockItems RBRACE  */
#line 175 "src/parser/parser.y"
                               {
        (yyval.block) = (yyvsp[-1].block);
    }
#line 1344 "src/parser/parser.tab.cpp"
    break;

  case 12: /* Block: LBRACE RBRACE  */
#line 178 "src/parser/parser.y"
                    {
        (yyval.block) = make_block().release();
    }
#line 1352 "src/parser/parser.tab.cpp"
    break;

  case 13: /* BlockItems: BlockItems Stmt  */
#line 184 "src/parser/parser.y"
                      {
        (yyvsp[-1].block)->stmts.push_back(std::unique_ptr<StmtNode>((yyvsp[0].stmt)));
        (yyval.block) = (yyvsp[-1].block);
    }
#line 1361 "src/parser/parser.tab.cpp"
    break;

  case 14: /* BlockItems: Stmt  */
#line 188 "src/parser/parser.y"
           {
        auto block = make_block();
        block->stmts.push_back(std::unique_ptr<StmtNode>((yyvsp[0].stmt)));
        (yyval.block) = block.release();
    }
#line 1371 "src/parser/parser.tab.cpp"
    break;

  case 15: /* Stmt: Block  */
#line 197 "src/parser/parser.y"
            {
        (yyval.stmt) = (yyvsp[0].block);
    }
#line 1379 "src/parser/parser.tab.cpp"
    break;

  case 16: /* Stmt: SEMICOLON  */
#line 200 "src/parser/parser.y"
                {
        // Empty statement - create a null statement
        (yyval.stmt) = new StmtNode(NodeType::ExprStmt);
    }
#line 1388 "src/parser/parser.tab.cpp"
    break;

  case 17: /* Stmt: Expr SEMICOLON  */
#line 204 "src/parser/parser.y"
                     {
        (yyval.stmt) = make_expr_stmt(std::unique_ptr<ExprNode>((yyvsp[-1].expr))).release();
    }
#line 1396 "src/parser/parser.tab.cpp"
    break;

  case 18: /* Stmt: ID ASSIGN Expr SEMICOLON  */
#line 207 "src/parser/parser.y"
                               {
        (yyval.stmt) = make_assign(*(yyvsp[-3].str), std::unique_ptr<ExprNode>((yyvsp[-1].expr))).release();
        delete (yyvsp[-3].str);
    }
#line 1405 "src/parser/parser.tab.cpp"
    break;

  case 19: /* Stmt: INT ID ASSIGN Expr SEMICOLON  */
#line 211 "src/parser/parser.y"
                                   {
        (yyval.stmt) = make_decl(*(yyvsp[-3].str), std::unique_ptr<ExprNode>((yyvsp[-1].expr))).release();
        delete (yyvsp[-3].str);
    }
#line 1414 "src/parser/parser.tab.cpp"
    break;

  case 20: /* Stmt: IF LPAREN Expr RPAREN Stmt  */
#line 215 "src/parser/parser.y"
                                 {
        (yyval.stmt) = make_if(std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<StmtNode>((yyvsp[0].stmt))).release();
    }
#line 1422 "src/parser/parser.tab.cpp"
    break;

  case 21: /* Stmt: IF LPAREN Expr RPAREN Stmt ELSE Stmt  */
#line 218 "src/parser/parser.y"
                                           {
        (yyval.stmt) = make_if(std::unique_ptr<ExprNode>((yyvsp[-4].expr)), std::unique_ptr<StmtNode>((yyvsp[-2].stmt)), std::unique_ptr<StmtNode>((yyvsp[0].stmt))).release();
    }
#line 1430 "src/parser/parser.tab.cpp"
    break;

  case 22: /* Stmt: WHILE LPAREN Expr RPAREN Stmt  */
#line 221 "src/parser/parser.y"
                                    {
        (yyval.stmt) = make_while(std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<StmtNode>((yyvsp[0].stmt))).release();
    }
#line 1438 "src/parser/parser.tab.cpp"
    break;

  case 23: /* Stmt: BREAK SEMICOLON  */
#line 224 "src/parser/parser.y"
                      {
        (yyval.stmt) = new BreakStmt();
    }
#line 1446 "src/parser/parser.tab.cpp"
    break;

  case 24: /* Stmt: CONTINUE SEMICOLON  */
#line 227 "src/parser/parser.y"
                         {
        (yyval.stmt) = new ContinueStmt();
    }
#line 1454 "src/parser/parser.tab.cpp"
    break;

  case 25: /* Stmt: RETURN Expr SEMICOLON  */
#line 230 "src/parser/parser.y"
                            {
        (yyval.stmt) = make_return(std::unique_ptr<ExprNode>((yyvsp[-1].expr))).release();
    }
#line 1462 "src/parser/parser.tab.cpp"
    break;

  case 26: /* Stmt: RETURN SEMICOLON  */
#line 233 "src/parser/parser.y"
                       {
        (yyval.stmt) = make_return().release();
    }
#line 1470 "src/parser/parser.tab.cpp"
    break;

  case 27: /* Expr: LOrExpr  */
#line 240 "src/parser/parser.y"
              { (yyval.expr) = (yyvsp[0].expr); }
#line 1476 "src/parser/parser.tab.cpp"
    break;

  case 28: /* LOrExpr: LAndExpr  */
#line 245 "src/parser/parser.y"
               { (yyval.expr) = (yyvsp[0].expr); }
#line 1482 "src/parser/parser.tab.cpp"
    break;

  case 29: /* LOrExpr: LOrExpr OR LAndExpr  */
#line 246 "src/parser/parser.y"
                          {
        (yyval.expr) = make_binary(OpType::Or, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1490 "src/parser/parser.tab.cpp"
    break;

  case 30: /* LAndExpr: RelExpr  */
#line 253 "src/parser/parser.y"
              { (yyval.expr) = (yyvsp[0].expr); }
#line 1496 "src/parser/parser.tab.cpp"
    break;

  case 31: /* LAndExpr: LAndExpr AND RelExpr  */
#line 254 "src/parser/parser.y"
                           {
        (yyval.expr) = make_binary(OpType::And, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1504 "src/parser/parser.tab.cpp"
    break;

  case 32: /* RelExpr: AddExpr  */
#line 261 "src/parser/parser.y"
              { (yyval.expr) = (yyvsp[0].expr); }
#line 1510 "src/parser/parser.tab.cpp"
    break;

  case 33: /* RelExpr: RelExpr LT AddExpr  */
#line 262 "src/parser/parser.y"
                         {
        (yyval.expr) = make_binary(OpType::Lt, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1518 "src/parser/parser.tab.cpp"
    break;

  case 34: /* RelExpr: RelExpr GT AddExpr  */
#line 265 "src/parser/parser.y"
                         {
        (yyval.expr) = make_binary(OpType::Gt, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1526 "src/parser/parser.tab.cpp"
    break;

  case 35: /* RelExpr: RelExpr LE AddExpr  */
#line 268 "src/parser/parser.y"
                         {
        (yyval.expr) = make_binary(OpType::Le, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1534 "src/parser/parser.tab.cpp"
    break;

  case 36: /* RelExpr: RelExpr GE AddExpr  */
#line 271 "src/parser/parser.y"
                         {
        (yyval.expr) = make_binary(OpType::Ge, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1542 "src/parser/parser.tab.cpp"
    break;

  case 37: /* RelExpr: RelExpr EQ AddExpr  */
#line 274 "src/parser/parser.y"
                         {
        (yyval.expr) = make_binary(OpType::Eq, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1550 "src/parser/parser.tab.cpp"
    break;

  case 38: /* RelExpr: RelExpr NE AddExpr  */
#line 277 "src/parser/parser.y"
                         {
        (yyval.expr) = make_binary(OpType::Ne, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1558 "src/parser/parser.tab.cpp"
    break;

  case 39: /* AddExpr: MulExpr  */
#line 284 "src/parser/parser.y"
              { (yyval.expr) = (yyvsp[0].expr); }
#line 1564 "src/parser/parser.tab.cpp"
    break;

  case 40: /* AddExpr: AddExpr PLUS MulExpr  */
#line 285 "src/parser/parser.y"
                           {
        (yyval.expr) = make_binary(OpType::Add, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1572 "src/parser/parser.tab.cpp"
    break;

  case 41: /* AddExpr: AddExpr MINUS MulExpr  */
#line 288 "src/parser/parser.y"
                            {
        (yyval.expr) = make_binary(OpType::Sub, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1580 "src/parser/parser.tab.cpp"
    break;

  case 42: /* MulExpr: UnaryExpr  */
#line 295 "src/parser/parser.y"
                { (yyval.expr) = (yyvsp[0].expr); }
#line 1586 "src/parser/parser.tab.cpp"
    break;

  case 43: /* MulExpr: MulExpr MUL UnaryExpr  */
#line 296 "src/parser/parser.y"
                            {
        (yyval.expr) = make_binary(OpType::Mul, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1594 "src/parser/parser.tab.cpp"
    break;

  case 44: /* MulExpr: MulExpr DIV UnaryExpr  */
#line 299 "src/parser/parser.y"
                            {
        (yyval.expr) = make_binary(OpType::Div, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1602 "src/parser/parser.tab.cpp"
    break;

  case 45: /* MulExpr: MulExpr MOD UnaryExpr  */
#line 302 "src/parser/parser.y"
                            {
        (yyval.expr) = make_binary(OpType::Mod, std::unique_ptr<ExprNode>((yyvsp[-2].expr)), std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1610 "src/parser/parser.tab.cpp"
    break;

  case 46: /* UnaryExpr: PrimaryExpr  */
#line 309 "src/parser/parser.y"
                  { (yyval.expr) = (yyvsp[0].expr); }
#line 1616 "src/parser/parser.tab.cpp"
    break;

  case 47: /* UnaryExpr: PLUS UnaryExpr  */
#line 310 "src/parser/parser.y"
                     {
        (yyval.expr) = make_unary(OpType::Pos, std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1624 "src/parser/parser.tab.cpp"
    break;

  case 48: /* UnaryExpr: MINUS UnaryExpr  */
#line 313 "src/parser/parser.y"
                      {
        (yyval.expr) = make_unary(OpType::Neg, std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1632 "src/parser/parser.tab.cpp"
    break;

  case 49: /* UnaryExpr: NOT UnaryExpr  */
#line 316 "src/parser/parser.y"
                    {
        (yyval.expr) = make_unary(OpType::Not, std::unique_ptr<ExprNode>((yyvsp[0].expr))).release();
    }
#line 1640 "src/parser/parser.tab.cpp"
    break;

  case 50: /* PrimaryExpr: ID  */
#line 323 "src/parser/parser.y"
         {
        (yyval.expr) = make_var(*(yyvsp[0].str)).release();
        delete (yyvsp[0].str);
    }
#line 1649 "src/parser/parser.tab.cpp"
    break;

  case 51: /* PrimaryExpr: NUMBER  */
#line 327 "src/parser/parser.y"
             {
        (yyval.expr) = make_const((yyvsp[0].num)).release();
    }
#line 1657 "src/parser/parser.tab.cpp"
    break;

  case 52: /* PrimaryExpr: LPAREN Expr RPAREN  */
#line 330 "src/parser/parser.y"
                         {
        (yyval.expr) = (yyvsp[-1].expr);
    }
#line 1665 "src/parser/parser.tab.cpp"
    break;

  case 53: /* PrimaryExpr: ID LPAREN FuncRParams RPAREN  */
#line 333 "src/parser/parser.y"
                                   {
        (yyval.expr) = make_call(*(yyvsp[-3].str), *(yyvsp[-1].expr_list)).release();
        delete (yyvsp[-3].str);
        delete (yyvsp[-1].expr_list);
    }
#line 1675 "src/parser/parser.tab.cpp"
    break;

  case 54: /* PrimaryExpr: ID LPAREN RPAREN  */
#line 338 "src/parser/parser.y"
                       {
        auto args = new std::vector<std::unique_ptr<ExprNode>>();
        (yyval.expr) = make_call(*(yyvsp[-2].str), *args).release();
        delete (yyvsp[-2].str);
        delete args;
    }
#line 1686 "src/parser/parser.tab.cpp"
    break;

  case 55: /* FuncRParams: Expr  */
#line 348 "src/parser/parser.y"
           {
        (yyval.expr_list) = new std::vector<std::unique_ptr<ExprNode>>();
        (yyval.expr_list)->push_back(std::unique_ptr<ExprNode>((yyvsp[0].expr)));
    }
#line 1695 "src/parser/parser.tab.cpp"
    break;

  case 56: /* FuncRParams: FuncRParams COMMA Expr  */
#line 352 "src/parser/parser.y"
                             {
        (yyvsp[-2].expr_list)->push_back(std::unique_ptr<ExprNode>((yyvsp[0].expr)));
        (yyval.expr_list) = (yyvsp[-2].expr_list);
    }
#line 1704 "src/parser/parser.tab.cpp"
    break;


#line 1708 "src/parser/parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 358 "src/parser/parser.y"

