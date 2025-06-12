/*
 * File Name    : subc.y
 * Description  : a skeleton bison input
 */

%{

/* Prologue section */

#include "subc.h"

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

// 플랫폼별 메모리 관리 매크로
#ifdef __linux__
    #define MALLOC(size) malloc(size)
    #define STRDUP(str) strdup(str)  
    #define REALLOC(ptr, size) realloc(ptr, size)
#else
    #define MALLOC(size) tracked_malloc(size)
    #define STRDUP(str) tracked_strdup(str)
    #define REALLOC(ptr, size) tracked_realloc(ptr, size)
#endif

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

%}

/* Bison declarations section */

/* yylval types */
%union {
    int     intVal;
    char*   stringVal;
    TypeInfo* typeInfo;
    FuncInfo* funcInfo;
    TypeInfo** typeInfoList;
    ParamData* paramData;
    ParamListData* paramListData;
}

/* Precedences and Associativities */
%left   ','
%right  '='
%left   LOGICAL_OR
%left   LOGICAL_AND
%left   EQUOP
%left   RELOP
%left   '+' '-'
%left   '*' '/' '%'
%right  '!' '&' INCOP DECOP
%left   '[' ']' '(' ')' '.' STRUCTOP

%precedence IF
%precedence ELSE

/* Tokens and Types */
%token STRUCT RETURN WHILE FOR BREAK CONTINUE SYM_NULL
%token<stringVal> TYPE
%token<intVal> INTEGER_CONST CHAR_CONST
%token<stringVal> ID STRING
%type<typeInfo> type_specifier struct_specifier pointers expr unary binary expr_e stmt
%type<funcInfo> func_decl
%type<typeInfoList> args
%type<paramData> param_decl
%type<paramListData> param_list

/* Grammar rules */
%%
program
    : { 
        // 초기 전역 스코프 생성
        current_scope = create_symbol_table(NULL);
    } ext_def_list
    ;

ext_def_list
    : ext_def_list ext_def 
    | %empty 
    ;

ext_def
    : type_specifier pointers ID ';' {
        if (lookup_symbol_current_scope($3)) {
            error_redeclaration();
        } else {
            TypeInfo* type = $1;
            if ($2) {
                type = create_pointer_type($1);
            }
            insert_symbol($3, type, NULL);
        }
    }
    | type_specifier pointers ID '[' INTEGER_CONST ']' ';' {
        if (lookup_symbol_current_scope($3)) {
            error_redeclaration();
        } else {
            TypeInfo* type = create_array_type($1, $5);
            insert_symbol($3, type, NULL);
        }
    }
    | struct_specifier ';'
    | func_decl compound_stmt {
        current_function_return_type = NULL;
    }
    ;

type_specifier
    : TYPE {
        if (strcmp($1, "int") == 0) {
            $$ = create_basic_type(T_INT);
        } else if (strcmp($1, "char") == 0) {
            $$ = create_basic_type(T_CHAR);
        } else {
            $$ = create_basic_type(T_INT); // 기본값
        }
        free($1);
    }
    | struct_specifier { $$ = $1; }
    ;

struct_specifier
    : STRUCT ID '{' {
        // 구조체 재선언 검사
        SymbolEntry* existing = lookup_symbol_current_scope($2);
        if (existing) {
            error_redeclaration();
        }
        push_scope();
    } def_list '}' {
        TypeInfo* type = create_struct_type($2);
        // 현재 스코프를 구조체 테이블로 저장하기 전에 백업
        SymbolTable* struct_table = current_scope;
        pop_scope(); // 부모 스코프로 복원
        type->struct_table = struct_table; // 백업된 테이블 할당
        
        // 전역 스코프에 구조체 타입 등록
        if (!lookup_symbol_current_scope($2)) {
            insert_symbol($2, type, NULL);
        }
        
        $$ = type;
        free($2);
    }
    | STRUCT ID {
        SymbolEntry* entry = lookup_symbol($2);
        if (entry && is_struct_type(entry->type)) {
            $$ = entry->type;
        } else {
            error_incomplete();
            $$ = create_struct_type($2);
        }
        free($2);
    }
    ;

func_decl
    : type_specifier pointers ID '(' ')' {
        // Declare the function symbol in the current scope
        FuncInfo* func = (FuncInfo*)MALLOC(sizeof(FuncInfo));
        func->return_type = $2 ? create_pointer_type($1) : $1;
        func->param_types = NULL;
        func->param_count = 0;
        func->param_names = NULL;

        TypeInfo* type = (TypeInfo*)MALLOC(sizeof(TypeInfo));
        type->type = T_FUNCTION;
        type->element_type = func->return_type;
        type->array_size = 0;
        type->struct_name = STRDUP($3);
        type->struct_table = NULL;

        if (lookup_symbol_current_scope($3)) {
            error_redeclaration();
        } else {
            insert_symbol($3, type, func);
        }
        current_function_return_type = func->return_type;
        free($3);

        push_scope();

        $$ = func; 
    }
    | type_specifier pointers ID '(' param_list ')' {
        ParamListData* pld = $5;

        FuncInfo* func = (FuncInfo*)MALLOC(sizeof(FuncInfo));
        func->return_type = $2 ? create_pointer_type($1) : $1;
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
        type->struct_name = STRDUP($3);
        type->struct_table = NULL;

        if (lookup_symbol_current_scope($3)) {
            error_redeclaration();
        } else {
            insert_symbol($3, type, func);
        }
        current_function_return_type = func->return_type;
        free($3);

        push_scope(); 


        for (int i = 0; i < func->param_count; i++) {
            insert_symbol(func->param_names[i], func->param_types[i], NULL);
        }

        $$ = func; 
    }
    ;

pointers
    : '*' { $$ = (TypeInfo*)1; }  // 포인터 존재 여부만 표시
    | %empty { $$ = NULL; }
    ;

param_list
    : param_decl {
        ParamListData* pld = (ParamListData*)MALLOC(sizeof(ParamListData));
        pld->params = (ParamData**)MALLOC(sizeof(ParamData*));
        pld->params[0] = $1;
        pld->count = 1;
        $$ = pld;
    }
    | param_list ',' param_decl {
        $1->params = REALLOC($1->params, ($1->count + 1) * sizeof(ParamData*));
        $1->params[$1->count] = $3;
        $1->count++;
        $$ = $1;
    }
    ;

param_decl
    : type_specifier pointers ID {
        ParamData* pd = (ParamData*)MALLOC(sizeof(ParamData));
        pd->name = STRDUP($3);
        pd->type = $2 ? create_pointer_type($1) : $1;
        $$ = pd;
        free($3);
    }
    | type_specifier pointers ID '[' INTEGER_CONST ']' {
        ParamData* pd = (ParamData*)MALLOC(sizeof(ParamData));
        pd->name = STRDUP($3);
        pd->type = create_array_type($1, $5);
        $$ = pd;
        free($3);
    }
    ;

def_list
    : def_list def
    | %empty
    ;

def
    : type_specifier pointers ID ';' {
        if (lookup_symbol_current_scope($3)) {
            error_redeclaration();
        } else {
            TypeInfo* type = $1;
            if ($2) type = create_pointer_type($1);
            insert_symbol($3, type, NULL);
        }
    }
    | type_specifier pointers ID '[' INTEGER_CONST ']' ';' {
        if (lookup_symbol_current_scope($3)) {
            error_redeclaration();
        } else {
            TypeInfo* type = create_array_type($1, $5);
            insert_symbol($3, type, NULL);
        }
    }
    ;

compound_stmt
    : '{' {
        push_scope();
    } def_list stmt_list '}' { 
        pop_scope(); 
    }
    ;

stmt_list
    : stmt_list stmt
    | %empty
    ;

stmt
    : expr ';' { $$ = $1; }
    | RETURN expr ';' {
        if (!current_function_return_type) {
            error_return();
            $$ = create_basic_type(T_INT); // 에러 복구
        } else if (!$2 || !is_compatible_type(current_function_return_type, $2)) {
            error_return();
            $$ = create_basic_type(T_INT); // 에러 복구
        } else {
            $$ = current_function_return_type;
        }
    }
    | BREAK ';' { $$ = create_basic_type(T_INT); }
    | CONTINUE ';' { $$ = create_basic_type(T_INT); }
    | ';' { $$ = create_basic_type(T_INT); }
    | compound_stmt { $$ = create_basic_type(T_INT); }
    | IF '(' expr ')' stmt %prec IF { $$ = create_basic_type(T_INT); }
    | IF '(' expr ')' stmt ELSE stmt { $$ = create_basic_type(T_INT); }
    | WHILE '(' expr ')' stmt { $$ = create_basic_type(T_INT); }
    | FOR '(' expr_e ';' expr_e ';' expr_e ')' stmt { $$ = create_basic_type(T_INT); }
    ;

expr_e
    : expr { $$ = $1; }
    | %empty { $$ = NULL; }
    ;

expr
    : unary '=' expr {
        if (!$1 || !$3) {
            $$ = create_basic_type(T_INT);
        } else if ($1->type == T_ERROR || $3->type == T_ERROR) {
            $$ = create_error_type(); 
        } else if ($1->type == T_FUNCTION) {
            error_assignable();
            $$ = create_error_type();
        } else if ($1->type == T_ARRAY) {
            error_assignable();
            $$ = create_error_type();
        } else if (!is_compatible_type($1, $3)) {
            error_incompatible();
            $$ = create_error_type();
        } else {
            $$ = $1;
        }
    }
    | binary { $$ = $1 ? $1 : create_basic_type(T_INT); }
    ;

binary
    : binary RELOP binary {
        if (!$1 || !$3) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else if (is_pointer_type($1) && is_pointer_type($3) && !is_compatible_type($1, $3)) {
            error_comparable();
            $$ = create_basic_type(T_INT);
        } else if (!is_arithmetic_type($1) || !is_arithmetic_type($3)) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else {
            $$ = create_basic_type(T_INT);
        }
    }
    | binary EQUOP binary {
        if (!$1 || !$3) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else if (!is_compatible_type($1, $3)) {
            error_comparable();
            $$ = create_basic_type(T_INT);
        } else {
            $$ = create_basic_type(T_INT);
        }
    }
    | binary '+' binary {
        if (!$1 || !$3) {
            error_binary();
            $$ = create_error_type();
        } else if (!is_arithmetic_type($1) || !is_arithmetic_type($3)) {
            error_binary();
            $$ = create_error_type();
        } else {
            $$ = $1;
        }
    }
    | binary '-' binary {
        if (!$1 || !$3) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else if (!is_arithmetic_type($1) || !is_arithmetic_type($3)) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else {
            $$ = $1;
        }
    }
    | binary '*' binary {
        if (!$1 || !$3) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else if (!is_arithmetic_type($1) || !is_arithmetic_type($3)) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else {
            $$ = $1;
        }
    }
    | binary '/' binary {
        if (!$1 || !$3) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else if (!is_arithmetic_type($1) || !is_arithmetic_type($3)) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else {
            $$ = $1;
        }
    }
    | binary '%' binary {
        if (!$1 || !$3) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else if (!is_arithmetic_type($1) || !is_arithmetic_type($3)) {
            error_binary();
            $$ = create_basic_type(T_INT);
        } else {
            $$ = $1;
        }
    }
    | unary %prec '=' { $$ = $1 ? $1 : create_basic_type(T_INT); }
    | binary LOGICAL_AND binary {
        $$ = create_basic_type(T_INT);
    }
    | binary LOGICAL_OR binary {
        $$ = create_basic_type(T_INT);
    }
    ;

unary
    : '(' expr ')' { $$ = $2 ? $2 : create_basic_type(T_INT); }
    | '(' unary ')' { $$ = $2 ? $2 : create_basic_type(T_INT); }
    | INTEGER_CONST { $$ = create_basic_type(T_INT); }
    | CHAR_CONST { $$ = create_basic_type(T_CHAR); }
    | STRING { $$ = create_pointer_type(create_basic_type(T_CHAR)); }
    | ID {
        SymbolEntry* entry = lookup_symbol($1);
        if (!entry) {
            error_undeclared();
            $$ = create_basic_type(T_INT);
        } else {
            if (entry->type->type == T_FUNCTION) {
                TypeInfo* type = (TypeInfo*)MALLOC(sizeof(TypeInfo));
                type->type = T_FUNCTION;
                type->element_type = entry->func_info->return_type;
                type->array_size = 0;
                type->struct_name = STRDUP($1);
                type->struct_table = NULL;
                $$ = type;
            } else {
                $$ = entry->type;
            }
        }
        free($1);
    }
    | '-' unary {
        if (!$2) {
            $$ = create_basic_type(T_INT);
        } else if (!is_arithmetic_type($2)) {
            error_unary();
            $$ = create_basic_type(T_INT);
        }
        $$ = $2;
    }
    | '!' unary {
        $$ = create_basic_type(T_INT);
    }
    | unary INCOP {
        if (!$1) {
            $$ = create_basic_type(T_INT);
        } else if ($1->type == T_ERROR) {
            $$ = create_error_type(); 
        } else if (!is_arithmetic_type($1)) {
            error_unary();
            $$ = create_error_type();
        } else {
            $$ = $1;
        }
    }
    | unary DECOP {
        if (!$1) {
            $$ = create_basic_type(T_INT);
        } else if ($1->type == T_ERROR) {
            $$ = create_error_type(); 
        } else if (!is_arithmetic_type($1)) {
            error_unary();
            $$ = create_error_type();
        } else {
            $$ = $1;
        }
    }
    | INCOP unary {
        if (!$2) {
            $$ = create_basic_type(T_INT);
        } else if ($2->type == T_ERROR) {
            $$ = create_error_type(); 
        } else if (!is_arithmetic_type($2)) {
            error_unary();
            $$ = create_error_type();
        } else {
            $$ = $2;
        }
    }
    | DECOP unary {
        if (!$2) {
            $$ = create_basic_type(T_INT);
        } else if ($2->type == T_ERROR) {
            $$ = create_error_type(); 
        } else if (!is_arithmetic_type($2)) {
            error_unary();
            $$ = create_error_type();
        } else {
            $$ = $2;
        }
    }
    | '&' unary {
        if (!$2) {
            $$ = create_pointer_type(create_basic_type(T_INT));
        } else if ($2->type == T_ERROR) {
            $$ = create_error_type(); 
        } else if ($2->type == T_ARRAY) {
            error_addressof();
            $$ = create_error_type();
        } else {
            $$ = create_pointer_type($2);
        }
    }
    | '*' unary {
        if (!$2) {
            $$ = create_basic_type(T_INT);
        } else if ($2->type == T_ERROR) {
            $$ = create_error_type(); 
        } else if (!is_pointer_type($2)) {
            error_indirection();
            $$ = create_error_type();
        } else {
            $$ = $2->element_type;
        }
    }
    | unary '[' expr ']' {
        if (!$1 || !$3) {
            $$ = create_basic_type(T_INT);
        } else if (!is_array_type($1)) {
            error_array();
            $$ = create_basic_type(T_INT);
        } else {
            if (!is_arithmetic_type($3)) {
                error_subscript();
            }
            $$ = $1->element_type;
        }
    }
    | unary '.' ID {
        if (!$1) {
            $$ = create_basic_type(T_INT);
        } else if (!is_struct_type($1)) {
            error_struct();
            $$ = create_basic_type(T_INT);
        } else {
            SymbolEntry* member = lookup_struct_member($1, $3);
            if (!member) {
                error_member();
                $$ = create_basic_type(T_INT);
            } else {
                $$ = member->type;
            }
        }
        free($3);
    }
    | unary STRUCTOP ID {
        if (!$1) {
            $$ = create_basic_type(T_INT);
        } else if (!is_pointer_type($1) || !is_struct_type($1->element_type)) {
            error_strurctp();
            $$ = create_basic_type(T_INT);
        } else {
            SymbolEntry* member = lookup_struct_member($1->element_type, $3);
            if (!member) {
                error_member();
                $$ = create_basic_type(T_INT);
            } else {
                $$ = member->type;
            }
        }
        free($3);
    }
    | unary '(' args ')' {
        SymbolEntry* entry = NULL;
        if ($1 && $1->type == T_FUNCTION) {
            entry = lookup_symbol($1->struct_name);
        } else {
            error_function();
            $$ = create_basic_type(T_INT);
        }
        if (!entry || !entry->func_info) {
            error_function();
            $$ = create_basic_type(T_INT);
        } else {
            int param_count = entry->func_info->param_count;
            int arg_count = 0;
            int has_null = 0;
            if ($3) {
                while ($3[arg_count]) {
                    if ($3[arg_count] == NULL) has_null = 1;
                    arg_count++;
                }
            }
            if (has_null) {
                error_arguments();
                $$ = entry->func_info->return_type;
            } else if (param_count != arg_count) {
                error_arguments();
                $$ = entry->func_info->return_type;
            } else {
                int ok = 1;
                for (int i=0; i<param_count; i++) {
                    if (!is_compatible_type(entry->func_info->param_types[i], $3[i])) ok = 0;
                }
                if (!ok) {
                    error_arguments();
                    $$ = entry->func_info->return_type;
                }
                $$ = entry->func_info->return_type;
            }
        }

    }
    | unary '(' ')' {
        SymbolEntry* entry = NULL;
        if ($1 && $1->type == T_FUNCTION) {
            entry = lookup_symbol($1->struct_name);
        } else {
            error_function();
            $$ = create_basic_type(T_INT);
        }
        if (!entry || !entry->func_info) {
            error_function();
            $$ = create_basic_type(T_INT);
        } else if (entry->func_info->param_count > 0) {
            error_arguments();
            $$ = entry->func_info->return_type;
        } else {
            $$ = entry->func_info->return_type;
        }
    }
    | SYM_NULL {
        $$ = create_pointer_type(create_basic_type(T_INT));
    }
    ;

args
    : expr {
        TypeInfo** list = (TypeInfo**)MALLOC(2 * sizeof(TypeInfo*));
        list[0] = $1;
        list[1] = NULL;
        $$ = list;
    }
    | args ',' expr {
        int n = 0;
        while ($1[n]) n++;
        $1 = REALLOC($1, (n+2)*sizeof(TypeInfo*));
        $1[n] = $3;
        $1[n+1] = NULL;
        $$ = $1;
    }
    ;

%%

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
