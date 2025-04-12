#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "ast.h"
int check_program(struct node *program);

enum type category_type(enum category category);
// Lista de símbolos de uma certa função
struct symbol_list {
	char *identifier;
	enum type type;
	struct node *node;
    int is_param;
    int is_func;
    char *func_param; // Type dos parametros de uma função
    int used;
	struct symbol_list *next;
};

struct scopes_list {
    struct symbol_list *table;
    struct scopes_list *next;
    struct node *body;
    char *identifier;
};

int verify_var(struct node *var , struct symbol_list *scope);
char *get_type(char *type);
char* check_parameters(struct node *parameters,struct symbol_list *scope);
enum type check_call(struct node *call,struct symbol_list *scope);
void check_varDecl(struct node *varDecl,struct symbol_list *scope);
void check_function(struct node *funcDecl);
void check_function_body(struct node *function_body, struct symbol_list *scope);
void check_statements(struct node *statement,struct symbol_list *scope);
enum type check_expression(struct node *expression,struct symbol_list *scope);
struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, struct node *node, int is_param,int is_func,int used);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier);
void show_symbol_table();
void show_scopes();
void unused();
#endif
