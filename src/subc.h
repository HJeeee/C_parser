#ifndef __SUBC_H__
#define __SUBC_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    T_INT,
    T_CHAR,
    T_ARRAY,
    T_POINTER,
    T_STRUCT,
    T_FUNCTION,
    T_ERROR
} Type;

typedef struct TypeInfo {
    Type type;
    struct TypeInfo* element_type;
    int array_size;
    char* struct_name;
    struct SymbolTable* struct_table;
} TypeInfo;

typedef struct {
    TypeInfo* return_type;
    char** param_names;
    TypeInfo** param_types;
    int param_count;
} FuncInfo;

typedef struct ParamData {
    char* name;
    TypeInfo* type;
} ParamData;

typedef struct ParamListData {
    ParamData** params;
    int count;
} ParamListData;

typedef struct SymbolEntry {
    char* name;
    TypeInfo* type;
    FuncInfo* func_info;
    struct SymbolEntry* next;
} SymbolEntry;

typedef struct SymbolTable {
    SymbolEntry* entries;
    struct SymbolTable* parent;
} SymbolTable;

extern char* current_filename;
extern SymbolTable* current_scope;
extern TypeInfo* current_function_return_type;

SymbolTable* create_symbol_table(SymbolTable* parent);
void push_scope(void);
void pop_scope(void);
SymbolEntry* lookup_symbol(char* name);
SymbolEntry* lookup_symbol_current_scope(char* name);
void insert_symbol(char* name, TypeInfo* type, FuncInfo* func_info);

TypeInfo* create_basic_type(Type type);
TypeInfo* create_pointer_type(TypeInfo* base_type);
TypeInfo* create_array_type(TypeInfo* element_type, int size);
TypeInfo* create_struct_type(char* name);
int is_compatible_type(TypeInfo* t1, TypeInfo* t2);
int is_arithmetic_type(TypeInfo* type);
int is_pointer_type(TypeInfo* type);
int is_array_type(TypeInfo* type);
int is_struct_type(TypeInfo* type);

void error_preamble(void);
void error_undeclared(void);
void error_redeclaration(void);
void error_assignable(void);
void error_incompatible(void);
void error_null(void);
void error_binary(void);
void error_unary(void);
void error_comparable(void);
void error_indirection(void);
void error_addressof(void);
void error_struct(void);
void error_strurctp(void);
void error_member(void);
void error_array(void);
void error_subscript(void);
void error_incomplete(void);
void error_return(void);
void error_function(void);
void error_arguments(void);

int get_lineno(void);

// Memory tracking functions
char* tracked_strdup(const char* str);
void cleanup_all_memory(void);
void untrack_pointer(void* ptr);
char* copy_string(const char* str);

#endif