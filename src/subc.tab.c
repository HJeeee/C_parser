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
#line 6 "subc.y"


/* Prologue section */

#include "subc.h"
#include <stdlib.h>

int   yylex ();
int   yyerror (char* s);
int   get_lineno();

// 전역 변수 정의
char* current_filename = NULL;
SymbolTable* current_scope = NULL;
TypeInfo* current_function_return_type = NULL;

// 메모리 정리 함수 선언
void cleanup_all_memory(void);
void* tracked_malloc(size_t size);
char* tracked_strdup(const char* str);
void* tracked_realloc(void* ptr, size_t size);

// 모든 플랫폼에서 메모리 추적 사용
#define MALLOC(size) tracked_malloc(size)
#define STRDUP(str) tracked_strdup(str)
#define REALLOC(ptr, size) tracked_realloc(ptr, size)
#define TRACKED_FREE(ptr) do { if (ptr) { untrack_pointer(ptr); free(ptr); } } while(0)

// 유틸리티 함수들
char* copy_string(const char* str) {
    char* new_str = (char*)MALLOC(strlen(str) + 1);
    strcpy(new_str, str);
    return new_str;
}

// 심볼 테이블 관리 함수들 구현
SymbolTable* create_symbol_table(SymbolTable* parent) {
    SymbolTable* table = (SymbolTable*)MALLOC(sizeof(SymbolTable));
    table->entries = NULL;
    table->parent = parent;
    return table;
}

void push_scope(void) {
    current_scope = create_symbol_table(current_scope);
}

void pop_scope(void) {
    if (current_scope) {
        SymbolTable* parent = current_scope->parent;
        current_scope = parent;
    }
}

SymbolEntry* lookup_symbol(char* name) {
    if (!name) return NULL;
    
    SymbolTable* scope = current_scope;
    while (scope) {
        SymbolEntry* entry = scope->entries;
        while (entry) {
            if (strcmp(entry->name, name) == 0) {
                return entry;
            }
            entry = entry->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

SymbolEntry* lookup_symbol_current_scope(char* name) {
    if (!name || !current_scope) return NULL;
    
    SymbolEntry* entry = current_scope->entries;
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void insert_symbol(char* name, TypeInfo* type, FuncInfo* func_info) {
    SymbolEntry* entry = (SymbolEntry*)MALLOC(sizeof(SymbolEntry));
    entry->name = copy_string(name);
    entry->type = type;
    entry->func_info = func_info;
    entry->next = current_scope->entries;
    current_scope->entries = entry;
}

// 타입 관리 함수들 구현
TypeInfo* create_basic_type(Type type) {
    TypeInfo* info = (TypeInfo*)MALLOC(sizeof(TypeInfo));
    info->type = type;
    info->element_type = NULL;
    info->array_size = 0;
    info->struct_name = NULL;
    info->struct_table = NULL;
    return info;
}

TypeInfo* create_error_type(void) {
    return create_basic_type(T_ERROR);
}

TypeInfo* create_pointer_type(TypeInfo* base_type) {
    TypeInfo* info = (TypeInfo*)MALLOC(sizeof(TypeInfo));
    info->type = T_POINTER;
    info->element_type = base_type;
    info->array_size = 0;
    info->struct_name = NULL;
    info->struct_table = NULL;
    return info;
}

TypeInfo* create_array_type(TypeInfo* element_type, int size) {
    TypeInfo* info = (TypeInfo*)MALLOC(sizeof(TypeInfo));
    info->type = T_ARRAY;
    info->element_type = element_type;
    info->array_size = size;
    info->struct_name = NULL;
    info->struct_table = NULL;
    return info;
}

TypeInfo* create_struct_type(char* name) {
    TypeInfo* info = (TypeInfo*)MALLOC(sizeof(TypeInfo));
    info->type = T_STRUCT;
    info->element_type = NULL;
    info->array_size = 0;
    info->struct_name = copy_string(name);
    info->struct_table = create_symbol_table(NULL);
    return info;
}

// 구조체 멤버를 찾는 헬퍼 함수
SymbolEntry* lookup_struct_member(TypeInfo* struct_type, char* member_name) {
    if (!struct_type || !is_struct_type(struct_type) || !struct_type->struct_table) {
        return NULL;
    }
    
    SymbolEntry* entry = struct_type->struct_table->entries;
    while (entry) {
        if (strcmp(entry->name, member_name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

int is_compatible_type(TypeInfo* t1, TypeInfo* t2) {
    if (!t1 || !t2) return 0;
    
    if (t1->type != t2->type) return 0;
    
    switch (t1->type) {
        case T_INT:
        case T_CHAR:
            return 1;
        case T_POINTER:
        case T_ARRAY:
            return is_compatible_type(t1->element_type, t2->element_type);
        case T_STRUCT:
            return strcmp(t1->struct_name, t2->struct_name) == 0;
        default:
            return 0;
    }
}

int is_arithmetic_type(TypeInfo* type) {
    return type && (type->type == T_INT || type->type == T_CHAR);
}

int is_pointer_type(TypeInfo* type) {
    return type && type->type == T_POINTER;
}

int is_array_type(TypeInfo* type) {
    return type && type->type == T_ARRAY;
}

int is_struct_type(TypeInfo* type) {
    return type && type->type == T_STRUCT;
}


#line 262 "subc.tab.c"

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

#include "subc.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_3_ = 3,                         /* ','  */
  YYSYMBOL_4_ = 4,                         /* '='  */
  YYSYMBOL_LOGICAL_OR = 5,                 /* LOGICAL_OR  */
  YYSYMBOL_LOGICAL_AND = 6,                /* LOGICAL_AND  */
  YYSYMBOL_EQUOP = 7,                      /* EQUOP  */
  YYSYMBOL_RELOP = 8,                      /* RELOP  */
  YYSYMBOL_9_ = 9,                         /* '+'  */
  YYSYMBOL_10_ = 10,                       /* '-'  */
  YYSYMBOL_11_ = 11,                       /* '*'  */
  YYSYMBOL_12_ = 12,                       /* '/'  */
  YYSYMBOL_13_ = 13,                       /* '%'  */
  YYSYMBOL_14_ = 14,                       /* '!'  */
  YYSYMBOL_15_ = 15,                       /* '&'  */
  YYSYMBOL_INCOP = 16,                     /* INCOP  */
  YYSYMBOL_DECOP = 17,                     /* DECOP  */
  YYSYMBOL_18_ = 18,                       /* '['  */
  YYSYMBOL_19_ = 19,                       /* ']'  */
  YYSYMBOL_20_ = 20,                       /* '('  */
  YYSYMBOL_21_ = 21,                       /* ')'  */
  YYSYMBOL_22_ = 22,                       /* '.'  */
  YYSYMBOL_STRUCTOP = 23,                  /* STRUCTOP  */
  YYSYMBOL_IF = 24,                        /* IF  */
  YYSYMBOL_ELSE = 25,                      /* ELSE  */
  YYSYMBOL_STRUCT = 26,                    /* STRUCT  */
  YYSYMBOL_RETURN = 27,                    /* RETURN  */
  YYSYMBOL_WHILE = 28,                     /* WHILE  */
  YYSYMBOL_FOR = 29,                       /* FOR  */
  YYSYMBOL_BREAK = 30,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 31,                  /* CONTINUE  */
  YYSYMBOL_SYM_NULL = 32,                  /* SYM_NULL  */
  YYSYMBOL_TYPE = 33,                      /* TYPE  */
  YYSYMBOL_INTEGER_CONST = 34,             /* INTEGER_CONST  */
  YYSYMBOL_CHAR_CONST = 35,                /* CHAR_CONST  */
  YYSYMBOL_ID = 36,                        /* ID  */
  YYSYMBOL_STRING = 37,                    /* STRING  */
  YYSYMBOL_38_ = 38,                       /* ';'  */
  YYSYMBOL_39_ = 39,                       /* '{'  */
  YYSYMBOL_40_ = 40,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_program = 42,                   /* program  */
  YYSYMBOL_43_1 = 43,                      /* $@1  */
  YYSYMBOL_ext_def_list = 44,              /* ext_def_list  */
  YYSYMBOL_ext_def = 45,                   /* ext_def  */
  YYSYMBOL_type_specifier = 46,            /* type_specifier  */
  YYSYMBOL_struct_specifier = 47,          /* struct_specifier  */
  YYSYMBOL_48_2 = 48,                      /* $@2  */
  YYSYMBOL_func_decl = 49,                 /* func_decl  */
  YYSYMBOL_pointers = 50,                  /* pointers  */
  YYSYMBOL_param_list = 51,                /* param_list  */
  YYSYMBOL_param_decl = 52,                /* param_decl  */
  YYSYMBOL_def_list = 53,                  /* def_list  */
  YYSYMBOL_def = 54,                       /* def  */
  YYSYMBOL_compound_stmt = 55,             /* compound_stmt  */
  YYSYMBOL_56_3 = 56,                      /* $@3  */
  YYSYMBOL_stmt_list = 57,                 /* stmt_list  */
  YYSYMBOL_stmt = 58,                      /* stmt  */
  YYSYMBOL_expr_e = 59,                    /* expr_e  */
  YYSYMBOL_expr = 60,                      /* expr  */
  YYSYMBOL_binary = 61,                    /* binary  */
  YYSYMBOL_unary = 62,                     /* unary  */
  YYSYMBOL_args = 63                       /* args  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   227

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  76
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  149

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   278


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
       2,     2,     2,    14,     2,     2,     2,    13,    15,     2,
      20,    21,    11,     9,     3,    10,    22,    12,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    38,
       2,     4,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    18,     2,    19,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    39,     2,    40,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     5,     6,
       7,     8,    16,    17,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   239,   239,   239,   248,   249,   253,   265,   274,   275,
     281,   291,   295,   295,   317,   330,   357,   398,   399,   403,
     410,   419,   426,   436,   437,   441,   451,   463,   463,   471,
     472,   476,   477,   488,   489,   490,   491,   492,   493,   494,
     495,   499,   500,   504,   522,   526,   540,   551,   562,   573,
     584,   595,   606,   607,   610,   616,   617,   618,   619,   620,
     621,   641,   650,   653,   665,   677,   689,   701,   713,   725,
     738,   755,   772,   813,   831,   837,   843
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
  "\"end of file\"", "error", "\"invalid token\"", "','", "'='",
  "LOGICAL_OR", "LOGICAL_AND", "EQUOP", "RELOP", "'+'", "'-'", "'*'",
  "'/'", "'%'", "'!'", "'&'", "INCOP", "DECOP", "'['", "']'", "'('", "')'",
  "'.'", "STRUCTOP", "IF", "ELSE", "STRUCT", "RETURN", "WHILE", "FOR",
  "BREAK", "CONTINUE", "SYM_NULL", "TYPE", "INTEGER_CONST", "CHAR_CONST",
  "ID", "STRING", "';'", "'{'", "'}'", "$accept", "program", "$@1",
  "ext_def_list", "ext_def", "type_specifier", "struct_specifier", "$@2",
  "func_decl", "pointers", "param_list", "param_decl", "def_list", "def",
  "compound_stmt", "$@3", "stmt_list", "stmt", "expr_e", "expr", "binary",
  "unary", "args", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-101)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -101,     4,  -101,  -101,    12,   -30,  -101,  -101,    18,     8,
      25,    38,  -101,     7,  -101,  -101,  -101,  -101,    10,  -101,
    -101,    55,    87,  -101,    12,    50,    97,  -101,    18,  -101,
      14,  -101,    18,  -101,    64,  -101,    69,    78,    12,  -101,
      81,   125,   125,   125,   125,   125,   125,   125,    98,   125,
     101,   108,   100,   105,  -101,  -101,  -101,  -101,  -101,  -101,
    -101,  -101,  -101,   106,   158,   133,  -101,   128,  -101,   -11,
     174,   174,   174,   174,   174,   174,   126,    45,   125,   110,
     125,   125,  -101,  -101,  -101,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,  -101,  -101,   125,     5,   116,
     118,   124,   138,  -101,  -101,  -101,   172,  -101,   195,   157,
    -101,   176,   174,   191,   197,   202,    60,    60,  -101,  -101,
    -101,  -101,   198,  -101,  -101,    15,  -101,  -101,   199,   200,
      95,    95,   125,  -101,   125,  -101,  -101,   182,   196,  -101,
     184,  -101,  -101,    95,   125,  -101,   203,    95,  -101
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     5,     1,     3,     0,    10,     4,    18,    11,
       0,    14,    17,     0,     8,    27,     9,    12,     0,    24,
      24,     0,     0,     6,    30,     0,     0,    15,    18,    11,
       0,    19,    18,    23,     0,    13,     0,     0,     0,    16,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    74,    57,    58,    60,    59,    35,
      28,    36,    29,     0,    44,    52,     7,    21,    20,     0,
      61,    68,    62,    67,    65,    66,     0,    52,     0,     0,
       0,    42,    33,    34,    31,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    63,    64,     0,     0,     0,
       0,     0,     0,    25,    55,    56,     0,    32,     0,     0,
      41,    54,    52,    53,    46,    45,    47,    48,    49,    50,
      51,    43,     0,    73,    75,     0,    70,    71,     0,     0,
       0,     0,    42,    69,     0,    72,    22,     0,    37,    39,
       0,    76,    26,     0,    42,    38,     0,     0,    40
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -101,  -101,  -101,  -101,  -101,    -1,   219,  -101,  -101,   -27,
    -101,   187,   206,  -101,   217,  -101,  -101,   -61,  -100,   -47,
      88,   -33,  -101
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,     4,     7,    28,    29,    20,    10,    13,
      30,    31,    24,    33,    61,    19,    34,    62,   109,    63,
      64,    65,   125
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      76,    37,    79,     8,     3,    40,    11,   102,    70,    71,
      72,    73,    74,    75,    77,    41,    42,    38,   134,    43,
      44,    45,    46,    32,    32,    47,   123,   103,    21,    12,
      22,   106,   140,   108,   110,    39,   135,    54,     5,    55,
      56,    57,    58,    18,   146,     6,    14,   121,    23,    94,
     122,   124,   112,   112,   112,   112,   112,   112,   112,   112,
     112,    95,    96,    97,    15,    98,   105,    99,   100,   138,
     139,    91,    92,    93,    41,    42,     5,    17,    43,    44,
      45,    46,   145,     6,    47,   110,   148,   141,    48,    26,
      35,    49,    50,    51,    52,    53,    54,   110,    55,    56,
      57,    58,    59,    15,    60,    41,    42,    66,    27,    43,
      44,    45,    46,     5,    67,    47,    36,    69,    78,    48,
       6,    80,    49,    50,    51,    52,    53,    54,    81,    55,
      56,    57,    58,    59,    15,    41,    42,    94,    82,    43,
      44,    45,    46,    83,    84,    47,   101,   104,   107,    95,
      96,    97,   126,    98,   127,    99,   100,    54,   128,    55,
      56,    57,    58,    85,    86,    87,    88,    89,    90,    91,
      92,    93,   129,   111,   113,   114,   115,   116,   117,   118,
     119,   120,    86,    87,    88,    89,    90,    91,    92,    93,
      95,    96,    97,   130,    98,   132,    99,   100,    87,    88,
      89,    90,    91,    92,    93,    88,    89,    90,    91,    92,
      93,    89,    90,    91,    92,    93,   131,   133,   136,   137,
     142,   143,   144,     9,   147,    68,    25,    16
};

static const yytype_uint8 yycheck[] =
{
      47,    28,    49,     4,     0,    32,    36,    18,    41,    42,
      43,    44,    45,    46,    47,    10,    11,     3,     3,    14,
      15,    16,    17,    24,    25,    20,    21,    38,    18,    11,
      20,    78,   132,    80,    81,    21,    21,    32,    26,    34,
      35,    36,    37,    36,   144,    33,    38,    94,    38,     4,
      97,    98,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    16,    17,    18,    39,    20,    21,    22,    23,   130,
     131,    11,    12,    13,    10,    11,    26,    39,    14,    15,
      16,    17,   143,    33,    20,   132,   147,   134,    24,    34,
      40,    27,    28,    29,    30,    31,    32,   144,    34,    35,
      36,    37,    38,    39,    40,    10,    11,    38,    21,    14,
      15,    16,    17,    26,    36,    20,    19,    36,    20,    24,
      33,    20,    27,    28,    29,    30,    31,    32,    20,    34,
      35,    36,    37,    38,    39,    10,    11,     4,    38,    14,
      15,    16,    17,    38,    38,    20,    18,    21,    38,    16,
      17,    18,    36,    20,    36,    22,    23,    32,    34,    34,
      35,    36,    37,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    34,    85,    86,    87,    88,    89,    90,    91,
      92,    93,     6,     7,     8,     9,    10,    11,    12,    13,
      16,    17,    18,    21,    20,    38,    22,    23,     7,     8,
       9,    10,    11,    12,    13,     8,     9,    10,    11,    12,
      13,     9,    10,    11,    12,    13,    21,    19,    19,    19,
      38,    25,    38,     4,    21,    38,    20,    10
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    42,    43,     0,    44,    26,    33,    45,    46,    47,
      49,    36,    11,    50,    38,    39,    55,    39,    36,    56,
      48,    18,    20,    38,    53,    53,    34,    21,    46,    47,
      51,    52,    46,    54,    57,    40,    19,    50,     3,    21,
      50,    10,    11,    14,    15,    16,    17,    20,    24,    27,
      28,    29,    30,    31,    32,    34,    35,    36,    37,    38,
      40,    55,    58,    60,    61,    62,    38,    36,    52,    36,
      62,    62,    62,    62,    62,    62,    60,    62,    20,    60,
      20,    20,    38,    38,    38,     5,     6,     7,     8,     9,
      10,    11,    12,    13,     4,    16,    17,    18,    20,    22,
      23,    18,    18,    38,    21,    21,    60,    38,    60,    59,
      60,    61,    62,    61,    61,    61,    61,    61,    61,    61,
      61,    60,    60,    21,    60,    63,    36,    36,    34,    34,
      21,    21,    38,    19,     3,    21,    19,    19,    58,    58,
      59,    60,    38,    25,    38,    58,    59,    21,    58
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    43,    42,    44,    44,    45,    45,    45,    45,
      46,    46,    48,    47,    47,    49,    49,    50,    50,    51,
      51,    52,    52,    53,    53,    54,    54,    56,    55,    57,
      57,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    59,    59,    60,    60,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    63,    63
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     0,     4,     7,     2,     2,
       1,     1,     0,     6,     2,     5,     6,     1,     0,     1,
       3,     3,     6,     2,     0,     4,     7,     0,     5,     2,
       0,     2,     3,     2,     2,     1,     1,     5,     7,     5,
       9,     1,     0,     3,     1,     3,     3,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     2,     2,     4,
       3,     3,     4,     3,     1,     1,     3
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
  case 2: /* $@1: %empty  */
#line 239 "subc.y"
      { 
        // 초기 전역 스코프 생성
        current_scope = create_symbol_table(NULL);
        // 프로그램 종료 시 메모리 정리 등록
        atexit(cleanup_all_memory);
    }
#line 1424 "subc.tab.c"
    break;

  case 6: /* ext_def: type_specifier pointers ID ';'  */
#line 253 "subc.y"
                                     {
        if (lookup_symbol_current_scope((yyvsp[-1].stringVal))) {
            error_redeclaration();
        } else {
            TypeInfo* type = (yyvsp[-3].typeInfo);
            if ((yyvsp[-2].typeInfo)) {
                type = create_pointer_type((yyvsp[-3].typeInfo));
            }
            insert_symbol((yyvsp[-1].stringVal), type, NULL);
        }
        TRACKED_FREE((yyvsp[-1].stringVal));
    }
#line 1441 "subc.tab.c"
    break;

  case 7: /* ext_def: type_specifier pointers ID '[' INTEGER_CONST ']' ';'  */
#line 265 "subc.y"
                                                           {
        if (lookup_symbol_current_scope((yyvsp[-4].stringVal))) {
            error_redeclaration();
        } else {
            TypeInfo* type = create_array_type((yyvsp[-6].typeInfo), (yyvsp[-2].intVal));
            insert_symbol((yyvsp[-4].stringVal), type, NULL);
        }
        TRACKED_FREE((yyvsp[-4].stringVal));
    }
#line 1455 "subc.tab.c"
    break;

  case 9: /* ext_def: func_decl compound_stmt  */
#line 275 "subc.y"
                              {
        current_function_return_type = NULL;
    }
#line 1463 "subc.tab.c"
    break;

  case 10: /* type_specifier: TYPE  */
#line 281 "subc.y"
           {
        if (strcmp((yyvsp[0].stringVal), "int") == 0) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (strcmp((yyvsp[0].stringVal), "char") == 0) {
            (yyval.typeInfo) = create_basic_type(T_CHAR);
        } else {
            (yyval.typeInfo) = create_basic_type(T_INT); // 기본값
        }
        TRACKED_FREE((yyvsp[0].stringVal));
    }
#line 1478 "subc.tab.c"
    break;

  case 11: /* type_specifier: struct_specifier  */
#line 291 "subc.y"
                       { (yyval.typeInfo) = (yyvsp[0].typeInfo); }
#line 1484 "subc.tab.c"
    break;

  case 12: /* $@2: %empty  */
#line 295 "subc.y"
                    {
        // 구조체 재선언 검사
        SymbolEntry* existing = lookup_symbol_current_scope((yyvsp[-1].stringVal));
        if (existing) {
            error_redeclaration();
        }
        push_scope();
    }
#line 1497 "subc.tab.c"
    break;

  case 13: /* struct_specifier: STRUCT ID '{' $@2 def_list '}'  */
#line 302 "subc.y"
                   {
        TypeInfo* type = create_struct_type((yyvsp[-4].stringVal));
        // 현재 스코프를 구조체 테이블로 저장하기 전에 백업
        SymbolTable* struct_table = current_scope;
        pop_scope(); // 부모 스코프로 복원
        type->struct_table = struct_table; // 백업된 테이블 할당
        
        // 전역 스코프에 구조체 타입 등록
        if (!lookup_symbol_current_scope((yyvsp[-4].stringVal))) {
            insert_symbol((yyvsp[-4].stringVal), type, NULL);
        }
        
        (yyval.typeInfo) = type;
        TRACKED_FREE((yyvsp[-4].stringVal));
    }
#line 1517 "subc.tab.c"
    break;

  case 14: /* struct_specifier: STRUCT ID  */
#line 317 "subc.y"
                {
        SymbolEntry* entry = lookup_symbol((yyvsp[0].stringVal));
        if (entry && is_struct_type(entry->type)) {
            (yyval.typeInfo) = entry->type;
        } else {
            error_incomplete();
            (yyval.typeInfo) = create_struct_type((yyvsp[0].stringVal));
        }
        TRACKED_FREE((yyvsp[0].stringVal));
    }
#line 1532 "subc.tab.c"
    break;

  case 15: /* func_decl: type_specifier pointers ID '(' ')'  */
#line 330 "subc.y"
                                         {
        // Declare the function symbol in the current scope
        FuncInfo* func = (FuncInfo*)MALLOC(sizeof(FuncInfo));
        func->return_type = (yyvsp[-3].typeInfo) ? create_pointer_type((yyvsp[-4].typeInfo)) : (yyvsp[-4].typeInfo);
        func->param_types = NULL;
        func->param_count = 0;
        func->param_names = NULL;

        TypeInfo* type = (TypeInfo*)MALLOC(sizeof(TypeInfo));
        type->type = T_FUNCTION;
        type->element_type = func->return_type;
        type->array_size = 0;
        type->struct_name = STRDUP((yyvsp[-2].stringVal));
        type->struct_table = NULL;

        if (lookup_symbol_current_scope((yyvsp[-2].stringVal))) {
            error_redeclaration();
        } else {
            insert_symbol((yyvsp[-2].stringVal), type, func);
        }
        current_function_return_type = func->return_type;
        TRACKED_FREE((yyvsp[-2].stringVal));

        push_scope();

        (yyval.funcInfo) = func; 
    }
#line 1564 "subc.tab.c"
    break;

  case 16: /* func_decl: type_specifier pointers ID '(' param_list ')'  */
#line 357 "subc.y"
                                                    {
        ParamListData* pld = (yyvsp[-1].paramListData);

        FuncInfo* func = (FuncInfo*)MALLOC(sizeof(FuncInfo));
        func->return_type = (yyvsp[-4].typeInfo) ? create_pointer_type((yyvsp[-5].typeInfo)) : (yyvsp[-5].typeInfo);
        func->param_types = (TypeInfo**)MALLOC(pld->count * sizeof(TypeInfo*));
        func->param_names = (char**)MALLOC(pld->count * sizeof(char*));
        func->param_count = pld->count;

        for (int i = 0; i < pld->count; i++) {
            func->param_types[i] = pld->params[i]->type;
            func->param_names[i] = STRDUP(pld->params[i]->name);
        }

        TypeInfo* type = (TypeInfo*)MALLOC(sizeof(TypeInfo));
        type->type = T_FUNCTION;
        type->element_type = func->return_type;
        type->array_size = 0;
        type->struct_name = STRDUP((yyvsp[-3].stringVal));
        type->struct_table = NULL;

        if (lookup_symbol_current_scope((yyvsp[-3].stringVal))) {
            error_redeclaration();
        } else {
            insert_symbol((yyvsp[-3].stringVal), type, func);
        }
        current_function_return_type = func->return_type;
        TRACKED_FREE((yyvsp[-3].stringVal));

        push_scope(); 


        for (int i = 0; i < func->param_count; i++) {
            insert_symbol(func->param_names[i], func->param_types[i], NULL);
        }

        (yyval.funcInfo) = func; 
    }
#line 1607 "subc.tab.c"
    break;

  case 17: /* pointers: '*'  */
#line 398 "subc.y"
          { (yyval.typeInfo) = (TypeInfo*)1; }
#line 1613 "subc.tab.c"
    break;

  case 18: /* pointers: %empty  */
#line 399 "subc.y"
             { (yyval.typeInfo) = NULL; }
#line 1619 "subc.tab.c"
    break;

  case 19: /* param_list: param_decl  */
#line 403 "subc.y"
                 {
        ParamListData* pld = (ParamListData*)MALLOC(sizeof(ParamListData));
        pld->params = (ParamData**)MALLOC(sizeof(ParamData*));
        pld->params[0] = (yyvsp[0].paramData);
        pld->count = 1;
        (yyval.paramListData) = pld;
    }
#line 1631 "subc.tab.c"
    break;

  case 20: /* param_list: param_list ',' param_decl  */
#line 410 "subc.y"
                                {
        (yyvsp[-2].paramListData)->params = REALLOC((yyvsp[-2].paramListData)->params, ((yyvsp[-2].paramListData)->count + 1) * sizeof(ParamData*));
        (yyvsp[-2].paramListData)->params[(yyvsp[-2].paramListData)->count] = (yyvsp[0].paramData);
        (yyvsp[-2].paramListData)->count++;
        (yyval.paramListData) = (yyvsp[-2].paramListData);
    }
#line 1642 "subc.tab.c"
    break;

  case 21: /* param_decl: type_specifier pointers ID  */
#line 419 "subc.y"
                                 {
        ParamData* pd = (ParamData*)MALLOC(sizeof(ParamData));
        pd->name = STRDUP((yyvsp[0].stringVal));
        pd->type = (yyvsp[-1].typeInfo) ? create_pointer_type((yyvsp[-2].typeInfo)) : (yyvsp[-2].typeInfo);
        (yyval.paramData) = pd;
        TRACKED_FREE((yyvsp[0].stringVal));
    }
#line 1654 "subc.tab.c"
    break;

  case 22: /* param_decl: type_specifier pointers ID '[' INTEGER_CONST ']'  */
#line 426 "subc.y"
                                                       {
        ParamData* pd = (ParamData*)MALLOC(sizeof(ParamData));
        pd->name = STRDUP((yyvsp[-3].stringVal));
        pd->type = create_array_type((yyvsp[-5].typeInfo), (yyvsp[-1].intVal));
        (yyval.paramData) = pd;
        TRACKED_FREE((yyvsp[-3].stringVal));
    }
#line 1666 "subc.tab.c"
    break;

  case 25: /* def: type_specifier pointers ID ';'  */
#line 441 "subc.y"
                                     {
        if (lookup_symbol_current_scope((yyvsp[-1].stringVal))) {
            error_redeclaration();
        } else {
            TypeInfo* type = (yyvsp[-3].typeInfo);
            if ((yyvsp[-2].typeInfo)) type = create_pointer_type((yyvsp[-3].typeInfo));
            insert_symbol((yyvsp[-1].stringVal), type, NULL);
        }
        TRACKED_FREE((yyvsp[-1].stringVal));
    }
#line 1681 "subc.tab.c"
    break;

  case 26: /* def: type_specifier pointers ID '[' INTEGER_CONST ']' ';'  */
#line 451 "subc.y"
                                                           {
        if (lookup_symbol_current_scope((yyvsp[-4].stringVal))) {
            error_redeclaration();
        } else {
            TypeInfo* type = create_array_type((yyvsp[-6].typeInfo), (yyvsp[-2].intVal));
            insert_symbol((yyvsp[-4].stringVal), type, NULL);
        }
        TRACKED_FREE((yyvsp[-4].stringVal));
    }
#line 1695 "subc.tab.c"
    break;

  case 27: /* $@3: %empty  */
#line 463 "subc.y"
          {
        push_scope();
    }
#line 1703 "subc.tab.c"
    break;

  case 28: /* compound_stmt: '{' $@3 def_list stmt_list '}'  */
#line 465 "subc.y"
                             { 
        pop_scope(); 
    }
#line 1711 "subc.tab.c"
    break;

  case 31: /* stmt: expr ';'  */
#line 476 "subc.y"
               { (yyval.typeInfo) = (yyvsp[-1].typeInfo); }
#line 1717 "subc.tab.c"
    break;

  case 32: /* stmt: RETURN expr ';'  */
#line 477 "subc.y"
                      {
        if (!current_function_return_type) {
            error_return();
            (yyval.typeInfo) = create_basic_type(T_INT); // 에러 복구
        } else if (!(yyvsp[-1].typeInfo) || !is_compatible_type(current_function_return_type, (yyvsp[-1].typeInfo))) {
            error_return();
            (yyval.typeInfo) = create_basic_type(T_INT); // 에러 복구
        } else {
            (yyval.typeInfo) = current_function_return_type;
        }
    }
#line 1733 "subc.tab.c"
    break;

  case 33: /* stmt: BREAK ';'  */
#line 488 "subc.y"
                { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1739 "subc.tab.c"
    break;

  case 34: /* stmt: CONTINUE ';'  */
#line 489 "subc.y"
                   { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1745 "subc.tab.c"
    break;

  case 35: /* stmt: ';'  */
#line 490 "subc.y"
          { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1751 "subc.tab.c"
    break;

  case 36: /* stmt: compound_stmt  */
#line 491 "subc.y"
                    { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1757 "subc.tab.c"
    break;

  case 37: /* stmt: IF '(' expr ')' stmt  */
#line 492 "subc.y"
                                    { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1763 "subc.tab.c"
    break;

  case 38: /* stmt: IF '(' expr ')' stmt ELSE stmt  */
#line 493 "subc.y"
                                     { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1769 "subc.tab.c"
    break;

  case 39: /* stmt: WHILE '(' expr ')' stmt  */
#line 494 "subc.y"
                              { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1775 "subc.tab.c"
    break;

  case 40: /* stmt: FOR '(' expr_e ';' expr_e ';' expr_e ')' stmt  */
#line 495 "subc.y"
                                                    { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1781 "subc.tab.c"
    break;

  case 41: /* expr_e: expr  */
#line 499 "subc.y"
           { (yyval.typeInfo) = (yyvsp[0].typeInfo); }
#line 1787 "subc.tab.c"
    break;

  case 42: /* expr_e: %empty  */
#line 500 "subc.y"
             { (yyval.typeInfo) = NULL; }
#line 1793 "subc.tab.c"
    break;

  case 43: /* expr: unary '=' expr  */
#line 504 "subc.y"
                     {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if ((yyvsp[-2].typeInfo)->type == T_ERROR || (yyvsp[0].typeInfo)->type == T_ERROR) {
            (yyval.typeInfo) = create_error_type(); 
        } else if ((yyvsp[-2].typeInfo)->type == T_FUNCTION) {
            error_assignable();
            (yyval.typeInfo) = create_error_type();
        } else if ((yyvsp[-2].typeInfo)->type == T_ARRAY) {
            error_assignable();
            (yyval.typeInfo) = create_error_type();
        } else if (!is_compatible_type((yyvsp[-2].typeInfo), (yyvsp[0].typeInfo))) {
            error_incompatible();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = (yyvsp[-2].typeInfo);
        }
    }
#line 1816 "subc.tab.c"
    break;

  case 44: /* expr: binary  */
#line 522 "subc.y"
             { (yyval.typeInfo) = (yyvsp[0].typeInfo) ? (yyvsp[0].typeInfo) : create_basic_type(T_INT); }
#line 1822 "subc.tab.c"
    break;

  case 45: /* binary: binary RELOP binary  */
#line 526 "subc.y"
                          {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (is_pointer_type((yyvsp[-2].typeInfo)) && is_pointer_type((yyvsp[0].typeInfo)) && !is_compatible_type((yyvsp[-2].typeInfo), (yyvsp[0].typeInfo))) {
            error_comparable();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_arithmetic_type((yyvsp[-2].typeInfo)) || !is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            (yyval.typeInfo) = create_basic_type(T_INT);
        }
    }
#line 1841 "subc.tab.c"
    break;

  case 46: /* binary: binary EQUOP binary  */
#line 540 "subc.y"
                          {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_compatible_type((yyvsp[-2].typeInfo), (yyvsp[0].typeInfo))) {
            error_comparable();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            (yyval.typeInfo) = create_basic_type(T_INT);
        }
    }
#line 1857 "subc.tab.c"
    break;

  case 47: /* binary: binary '+' binary  */
#line 551 "subc.y"
                        {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            error_binary();
            (yyval.typeInfo) = create_error_type();
        } else if (!is_arithmetic_type((yyvsp[-2].typeInfo)) || !is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_binary();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = (yyvsp[-2].typeInfo);
        }
    }
#line 1873 "subc.tab.c"
    break;

  case 48: /* binary: binary '-' binary  */
#line 562 "subc.y"
                        {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_arithmetic_type((yyvsp[-2].typeInfo)) || !is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            (yyval.typeInfo) = (yyvsp[-2].typeInfo);
        }
    }
#line 1889 "subc.tab.c"
    break;

  case 49: /* binary: binary '*' binary  */
#line 573 "subc.y"
                        {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_arithmetic_type((yyvsp[-2].typeInfo)) || !is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            (yyval.typeInfo) = (yyvsp[-2].typeInfo);
        }
    }
#line 1905 "subc.tab.c"
    break;

  case 50: /* binary: binary '/' binary  */
#line 584 "subc.y"
                        {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_arithmetic_type((yyvsp[-2].typeInfo)) || !is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            (yyval.typeInfo) = (yyvsp[-2].typeInfo);
        }
    }
#line 1921 "subc.tab.c"
    break;

  case 51: /* binary: binary '%' binary  */
#line 595 "subc.y"
                        {
        if (!(yyvsp[-2].typeInfo) || !(yyvsp[0].typeInfo)) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_arithmetic_type((yyvsp[-2].typeInfo)) || !is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_binary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            (yyval.typeInfo) = (yyvsp[-2].typeInfo);
        }
    }
#line 1937 "subc.tab.c"
    break;

  case 52: /* binary: unary  */
#line 606 "subc.y"
                      { (yyval.typeInfo) = (yyvsp[0].typeInfo) ? (yyvsp[0].typeInfo) : create_basic_type(T_INT); }
#line 1943 "subc.tab.c"
    break;

  case 53: /* binary: binary LOGICAL_AND binary  */
#line 607 "subc.y"
                                {
        (yyval.typeInfo) = create_basic_type(T_INT);
    }
#line 1951 "subc.tab.c"
    break;

  case 54: /* binary: binary LOGICAL_OR binary  */
#line 610 "subc.y"
                               {
        (yyval.typeInfo) = create_basic_type(T_INT);
    }
#line 1959 "subc.tab.c"
    break;

  case 55: /* unary: '(' expr ')'  */
#line 616 "subc.y"
                   { (yyval.typeInfo) = (yyvsp[-1].typeInfo) ? (yyvsp[-1].typeInfo) : create_basic_type(T_INT); }
#line 1965 "subc.tab.c"
    break;

  case 56: /* unary: '(' unary ')'  */
#line 617 "subc.y"
                    { (yyval.typeInfo) = (yyvsp[-1].typeInfo) ? (yyvsp[-1].typeInfo) : create_basic_type(T_INT); }
#line 1971 "subc.tab.c"
    break;

  case 57: /* unary: INTEGER_CONST  */
#line 618 "subc.y"
                    { (yyval.typeInfo) = create_basic_type(T_INT); }
#line 1977 "subc.tab.c"
    break;

  case 58: /* unary: CHAR_CONST  */
#line 619 "subc.y"
                 { (yyval.typeInfo) = create_basic_type(T_CHAR); }
#line 1983 "subc.tab.c"
    break;

  case 59: /* unary: STRING  */
#line 620 "subc.y"
             { (yyval.typeInfo) = create_pointer_type(create_basic_type(T_CHAR)); }
#line 1989 "subc.tab.c"
    break;

  case 60: /* unary: ID  */
#line 621 "subc.y"
         {
        SymbolEntry* entry = lookup_symbol((yyvsp[0].stringVal));
        if (!entry) {
            error_undeclared();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            if (entry->type->type == T_FUNCTION) {
                TypeInfo* type = (TypeInfo*)MALLOC(sizeof(TypeInfo));
                type->type = T_FUNCTION;
                type->element_type = entry->func_info->return_type;
                type->array_size = 0;
                type->struct_name = STRDUP((yyvsp[0].stringVal));
                type->struct_table = NULL;
                (yyval.typeInfo) = type;
            } else {
                (yyval.typeInfo) = entry->type;
            }
        }
        TRACKED_FREE((yyvsp[0].stringVal));
    }
#line 2014 "subc.tab.c"
    break;

  case 61: /* unary: '-' unary  */
#line 641 "subc.y"
                {
        if (!(yyvsp[0].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_unary();
            (yyval.typeInfo) = create_basic_type(T_INT);
        }
        (yyval.typeInfo) = (yyvsp[0].typeInfo);
    }
#line 2028 "subc.tab.c"
    break;

  case 62: /* unary: '!' unary  */
#line 650 "subc.y"
                {
        (yyval.typeInfo) = create_basic_type(T_INT);
    }
#line 2036 "subc.tab.c"
    break;

  case 63: /* unary: unary INCOP  */
#line 653 "subc.y"
                  {
        if (!(yyvsp[-1].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if ((yyvsp[-1].typeInfo)->type == T_ERROR) {
            (yyval.typeInfo) = create_error_type(); 
        } else if (!is_arithmetic_type((yyvsp[-1].typeInfo))) {
            error_unary();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = (yyvsp[-1].typeInfo);
        }
    }
#line 2053 "subc.tab.c"
    break;

  case 64: /* unary: unary DECOP  */
#line 665 "subc.y"
                  {
        if (!(yyvsp[-1].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if ((yyvsp[-1].typeInfo)->type == T_ERROR) {
            (yyval.typeInfo) = create_error_type(); 
        } else if (!is_arithmetic_type((yyvsp[-1].typeInfo))) {
            error_unary();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = (yyvsp[-1].typeInfo);
        }
    }
#line 2070 "subc.tab.c"
    break;

  case 65: /* unary: INCOP unary  */
#line 677 "subc.y"
                  {
        if (!(yyvsp[0].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if ((yyvsp[0].typeInfo)->type == T_ERROR) {
            (yyval.typeInfo) = create_error_type(); 
        } else if (!is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_unary();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = (yyvsp[0].typeInfo);
        }
    }
#line 2087 "subc.tab.c"
    break;

  case 66: /* unary: DECOP unary  */
#line 689 "subc.y"
                  {
        if (!(yyvsp[0].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if ((yyvsp[0].typeInfo)->type == T_ERROR) {
            (yyval.typeInfo) = create_error_type(); 
        } else if (!is_arithmetic_type((yyvsp[0].typeInfo))) {
            error_unary();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = (yyvsp[0].typeInfo);
        }
    }
#line 2104 "subc.tab.c"
    break;

  case 67: /* unary: '&' unary  */
#line 701 "subc.y"
                {
        if (!(yyvsp[0].typeInfo)) {
            (yyval.typeInfo) = create_pointer_type(create_basic_type(T_INT));
        } else if ((yyvsp[0].typeInfo)->type == T_ERROR) {
            (yyval.typeInfo) = create_error_type(); 
        } else if ((yyvsp[0].typeInfo)->type == T_ARRAY) {
            error_addressof();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = create_pointer_type((yyvsp[0].typeInfo));
        }
    }
#line 2121 "subc.tab.c"
    break;

  case 68: /* unary: '*' unary  */
#line 713 "subc.y"
                {
        if (!(yyvsp[0].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if ((yyvsp[0].typeInfo)->type == T_ERROR) {
            (yyval.typeInfo) = create_error_type(); 
        } else if (!is_pointer_type((yyvsp[0].typeInfo))) {
            error_indirection();
            (yyval.typeInfo) = create_error_type();
        } else {
            (yyval.typeInfo) = (yyvsp[0].typeInfo)->element_type;
        }
    }
#line 2138 "subc.tab.c"
    break;

  case 69: /* unary: unary '[' expr ']'  */
#line 725 "subc.y"
                         {
        if (!(yyvsp[-3].typeInfo) || !(yyvsp[-1].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_array_type((yyvsp[-3].typeInfo))) {
            error_array();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            if (!is_arithmetic_type((yyvsp[-1].typeInfo))) {
                error_subscript();
            }
            (yyval.typeInfo) = (yyvsp[-3].typeInfo)->element_type;
        }
    }
#line 2156 "subc.tab.c"
    break;

  case 70: /* unary: unary '.' ID  */
#line 738 "subc.y"
                   {
        if (!(yyvsp[-2].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_struct_type((yyvsp[-2].typeInfo))) {
            error_struct();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            SymbolEntry* member = lookup_struct_member((yyvsp[-2].typeInfo), (yyvsp[0].stringVal));
            if (!member) {
                error_member();
                (yyval.typeInfo) = create_basic_type(T_INT);
            } else {
                (yyval.typeInfo) = member->type;
            }
        }
        TRACKED_FREE((yyvsp[0].stringVal));
    }
#line 2178 "subc.tab.c"
    break;

  case 71: /* unary: unary STRUCTOP ID  */
#line 755 "subc.y"
                        {
        if (!(yyvsp[-2].typeInfo)) {
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (!is_pointer_type((yyvsp[-2].typeInfo)) || !is_struct_type((yyvsp[-2].typeInfo)->element_type)) {
            error_strurctp();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            SymbolEntry* member = lookup_struct_member((yyvsp[-2].typeInfo)->element_type, (yyvsp[0].stringVal));
            if (!member) {
                error_member();
                (yyval.typeInfo) = create_basic_type(T_INT);
            } else {
                (yyval.typeInfo) = member->type;
            }
        }
        TRACKED_FREE((yyvsp[0].stringVal));
    }
#line 2200 "subc.tab.c"
    break;

  case 72: /* unary: unary '(' args ')'  */
#line 772 "subc.y"
                         {
        SymbolEntry* entry = NULL;
        if ((yyvsp[-3].typeInfo) && (yyvsp[-3].typeInfo)->type == T_FUNCTION) {
            entry = lookup_symbol((yyvsp[-3].typeInfo)->struct_name);
        } else {
            error_function();
            (yyval.typeInfo) = create_basic_type(T_INT);
        }
        if (!entry || !entry->func_info) {
            error_function();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else {
            int param_count = entry->func_info->param_count;
            int arg_count = 0;
            int has_null = 0;
            if ((yyvsp[-1].typeInfoList)) {
                while ((yyvsp[-1].typeInfoList)[arg_count]) {
                    if ((yyvsp[-1].typeInfoList)[arg_count] == NULL) has_null = 1;
                    arg_count++;
                }
            }
            if (has_null) {
                error_arguments();
                (yyval.typeInfo) = entry->func_info->return_type;
            } else if (param_count != arg_count) {
                error_arguments();
                (yyval.typeInfo) = entry->func_info->return_type;
            } else {
                int ok = 1;
                for (int i=0; i<param_count; i++) {
                    if (!is_compatible_type(entry->func_info->param_types[i], (yyvsp[-1].typeInfoList)[i])) ok = 0;
                }
                if (!ok) {
                    error_arguments();
                    (yyval.typeInfo) = entry->func_info->return_type;
                }
                (yyval.typeInfo) = entry->func_info->return_type;
            }
        }

    }
#line 2246 "subc.tab.c"
    break;

  case 73: /* unary: unary '(' ')'  */
#line 813 "subc.y"
                    {
        SymbolEntry* entry = NULL;
        if ((yyvsp[-2].typeInfo) && (yyvsp[-2].typeInfo)->type == T_FUNCTION) {
            entry = lookup_symbol((yyvsp[-2].typeInfo)->struct_name);
        } else {
            error_function();
            (yyval.typeInfo) = create_basic_type(T_INT);
        }
        if (!entry || !entry->func_info) {
            error_function();
            (yyval.typeInfo) = create_basic_type(T_INT);
        } else if (entry->func_info->param_count > 0) {
            error_arguments();
            (yyval.typeInfo) = entry->func_info->return_type;
        } else {
            (yyval.typeInfo) = entry->func_info->return_type;
        }
    }
#line 2269 "subc.tab.c"
    break;

  case 74: /* unary: SYM_NULL  */
#line 831 "subc.y"
               {
        (yyval.typeInfo) = create_pointer_type(create_basic_type(T_INT));
    }
#line 2277 "subc.tab.c"
    break;

  case 75: /* args: expr  */
#line 837 "subc.y"
           {
        TypeInfo** list = (TypeInfo**)MALLOC(2 * sizeof(TypeInfo*));
        list[0] = (yyvsp[0].typeInfo);
        list[1] = NULL;
        (yyval.typeInfoList) = list;
    }
#line 2288 "subc.tab.c"
    break;

  case 76: /* args: args ',' expr  */
#line 843 "subc.y"
                    {
        int n = 0;
        while ((yyvsp[-2].typeInfoList)[n]) n++;
        (yyvsp[-2].typeInfoList) = REALLOC((yyvsp[-2].typeInfoList), (n+2)*sizeof(TypeInfo*));
        (yyvsp[-2].typeInfoList)[n] = (yyvsp[0].typeInfo);
        (yyvsp[-2].typeInfoList)[n+1] = NULL;
        (yyval.typeInfoList) = (yyvsp[-2].typeInfoList);
    }
#line 2301 "subc.tab.c"
    break;


#line 2305 "subc.tab.c"

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

#line 853 "subc.y"


/* Epilogue section */

void error_preamble(void) {
    printf("%s:%d: error: ", current_filename ? current_filename : "stdin", get_lineno());
}

int yyerror(char* s) {
    error_preamble();
    printf("%s\n", s);
    return 0;
}

void error_undeclared(void) {
    error_preamble();
    printf("use of undeclared identifier\n");
}

void error_redeclaration(void) {
    error_preamble();
    printf("redeclaration\n");
}

void error_assignable(void) {
    error_preamble();
    printf("lvalue is not assignable\n");
}

void error_incompatible(void) {
    error_preamble();
    printf("incompatible types for assignment operation\n");
}

void error_null(void) {
    error_preamble();
    printf("cannot assign 'NULL' to non-pointer type\n");
}

void error_binary(void) {
    error_preamble();
    printf("invalid operands to binary expression\n");
}

void error_unary(void) {
    error_preamble();
    printf("invalid argument type to unary expression\n");
}

void error_comparable(void) {
    error_preamble();
    printf("types are not comparable in binary expression\n");
}

void error_indirection(void) {
    error_preamble();
    printf("indirection requires pointer operand\n");
}

void error_addressof(void) {
    error_preamble();
    printf("cannot take the address of an rvalue\n");
}

void error_struct(void) {
    error_preamble();
    printf("member reference base type is not a struct\n");
}

void error_strurctp(void){
    error_preamble();
    printf("member reference base type is not a struct pointer\n");
}

void error_member(void) {
    error_preamble();
    printf("no such member in struct\n");
}

void error_array(void) {
    error_preamble();
    printf("subscripted value is not an array\n");
}

void error_subscript(void) {
    error_preamble();
    printf("array subscript is not an integer\n");
}

void error_incomplete(void) {
    error_preamble();
    printf("incomplete type\n");
}

void error_return(void) {
    error_preamble();
    printf("incompatible return types\n");
}

void error_function(void) {
    error_preamble();
    printf("not a function\n");
}

void error_arguments(void) {
    error_preamble();
    printf("incompatible arguments in function call\n");
}

// 메모리 추적을 위한 리스트들
static void** allocated_pointers = NULL;
static int allocated_count = 0;
static int allocated_capacity = 0;

// 메모리 할당 추적 함수
void* tracked_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) return NULL;
    
    // 할당된 포인터를 추적 배열에 추가
    if (allocated_count >= allocated_capacity) {
        allocated_capacity = allocated_capacity ? allocated_capacity * 2 : 64;
        allocated_pointers = realloc(allocated_pointers, allocated_capacity * sizeof(void*));
    }
    allocated_pointers[allocated_count++] = ptr;
    return ptr;
}

// 추적되는 strdup
char* tracked_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* new_str = (char*)malloc(len);
    if (new_str) {
        strcpy(new_str, str);
        // 추적 배열에 추가
        if (allocated_count >= allocated_capacity) {
            allocated_capacity = allocated_capacity ? allocated_capacity * 2 : 64;
            allocated_pointers = realloc(allocated_pointers, allocated_capacity * sizeof(void*));
        }
        allocated_pointers[allocated_count++] = new_str;
    }
    return new_str;
}

// 추적되는 realloc
void* tracked_realloc(void* ptr, size_t size) {
    // 기존 포인터를 추적 목록에서 제거하고 새 포인터를 추가
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr) return NULL;
    
    // 기존 포인터를 찾아서 새 포인터로 교체
    for (int i = 0; i < allocated_count; i++) {
        if (allocated_pointers[i] == ptr) {
            allocated_pointers[i] = new_ptr;
            return new_ptr;
        }
    }
    
    // 기존 포인터를 찾지 못한 경우 새로 추가
    if (allocated_count >= allocated_capacity) {
        allocated_capacity = allocated_capacity ? allocated_capacity * 2 : 64;
        allocated_pointers = realloc(allocated_pointers, allocated_capacity * sizeof(void*));
    }
    allocated_pointers[allocated_count++] = new_ptr;
    return new_ptr;
}

// 추적 목록에서 포인터 제거
void untrack_pointer(void* ptr) {
    for (int i = 0; i < allocated_count; i++) {
        if (allocated_pointers[i] == ptr) {
            // 마지막 요소를 현재 위치로 이동
            allocated_pointers[i] = allocated_pointers[--allocated_count];
            return;
        }
    }
}

// 추적된 메모리 전체 해제
void cleanup_all_memory(void) {
    for (int i = 0; i < allocated_count; i++) {
        if (allocated_pointers[i]) {
            free(allocated_pointers[i]);
        }
    }
    
    if (allocated_pointers) {
        free(allocated_pointers);
        allocated_pointers = NULL;
    }
    
    allocated_count = 0;
    allocated_capacity = 0;
    current_scope = NULL;
    current_function_return_type = NULL;
}
