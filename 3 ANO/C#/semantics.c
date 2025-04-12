#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"
// check funcdecl , vardecl , funcheader , funcbody , expressions
extern char *t_names[];
char *name_types[] = name_type;

int semantic_errors = 0; 
struct symbol_list *symbol_table;
struct scopes_list *list_scopes;
char *category_operator ( enum category category){
    switch(category){
        case Lt:
            return "<";
        case Gt:
            return ">";
        case Le:
            return "<=";
        case Ge:
            return ">=";
        case Add:
            return "+";
        case Sub:
            return "-";
        case Mul:
            return "*";
        case Div:
            return "/";
        case Mod:
            return "%";
        case Not:
            return "!";
        case Minus:
            return "-";
        case Plus:
            return "+";
        case Or:
            return "||";
        case And:
            return "&&";
        case Eq:
            return "==";
        case Ne:
            return "!=";
        default:
            return "";
    }
}
enum type category_type(enum category category){
    switch(category){
        case Int:
        case Natural:
            return integer;
        case Float32:
        case Decimal:
            return float32;
        case Bool:
            return bool;
        case String:
        case StrLit:
            return string;
        default:
            return undef;

    }
}

// semantic analysis begins here, with the AST root node

int check_program(struct node *program) {
    //tabela global
    symbol_table = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    symbol_table->identifier = "global";
    symbol_table->next = NULL;
    list_scopes = NULL;
    struct node_list *child = program->children;
    child=child->next;
    while(child != NULL){
        struct node *child_node = child->node;
        enum category child_category = child_node->category;
        if (child_category == VarDecl){
            check_varDecl(child_node,symbol_table);
        }
        if (child_category == FuncDecl) {
            check_function(child_node);
        }
        child=child->next;
        
    }
    struct scopes_list *scope=list_scopes;
    while (scope!=NULL)
    {
        check_function_body(scope->body,scope->table);
        scope=scope->next;
    }
    
    unused();
    return semantic_errors;
}
void check_function(struct node *function) {
    if(function == NULL){
    return;
   }
    struct node *funcheader = getchild(function, 0); // vai buscar o nó do FuncHeader
    struct node *id = getchild(funcheader,0); // vai buscar o identifier do Funccheader
    if (search_symbol(symbol_table,id->token)!=NULL)
    {
        semantic_errors++;
        printf("Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
        return;
    }
    
    enum type type_return; // tipo de return da função
    struct node *type_return_node = getchild(funcheader,1);
    struct node *func_params;
    if (type_return_node->category == FuncParams){
        type_return = none;
        func_params = getchild(funcheader,1);
    }
    else{
        type_return = category_type(type_return_node->category);
        func_params = getchild(funcheader,2);
    }
    insert_symbol(symbol_table,id->token,type_return,function,0,1,1);

    // criar scope fucncap
    struct symbol_list *scope_function = (struct symbol_list *)malloc(sizeof(struct symbol_list));
    scope_function->identifier = strdup(id->token);
    scope_function->next = NULL;

    //adicionar type de retorno ao scope

    insert_symbol(scope_function,"return",type_return,NULL,0,0,1);

    struct scopes_list *scope_entry = malloc(sizeof(struct scopes_list));
    scope_entry->table = scope_function;
    scope_entry->identifier = strdup(id->token);
    scope_entry->next = NULL;

    if (list_scopes != NULL) {
        struct scopes_list *current = list_scopes;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = scope_entry;
    } else {
        list_scopes = scope_entry   ;
    }

    
     char *params = check_parameters(func_params,scope_function);

     struct node *body = getchild(function,1);

     scope_entry->body = body;
     
     struct symbol_list *function_symbol = search_symbol(symbol_table,id->token);
     function_symbol->func_param = strdup(params);    
}

void check_function_body(struct node *function_body, struct symbol_list *scope){
    if (function_body==NULL)
    {
        return;
    }
    struct node_list *child= function_body->children;
    child=child->next;
    while (child!=NULL)
    {
        
        struct node *node=child->node;
        if (node->category==VarDecl)
        {
            check_varDecl(node,scope);
        }else{
            check_statements(node,scope);
        }
        child=child->next; 
    }
      
}

char* check_parameters(struct node *parameters,struct symbol_list *scope){
    if(parameters == NULL){
        return "";
    }
    char *types_params = (char *)malloc(4096);
    struct node_list *parameter = parameters->children;
    while ((parameter = parameter->next) !=NULL){
        struct node *param = parameter->node;
        struct node *type = getchild(param,0);
        struct node *id = getchild(param , 1);

        if (search_symbol(scope,id->token)==NULL){
            enum category cat = type->category;
            enum type ty = category_type(cat);
            insert_symbol(scope,id->token,ty,param,1,1,0);
            
        }else {
            semantic_errors++;
            printf("Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
        }
        strcat(types_params, t_names[category_type(type->category)]);
        strcat(types_params, ",");
    }
    // Retira a ultima virgula (int,)
    int len = strlen(types_params);
    if (len > 0) {
        types_params[len - 1] = '\0';
    }
    return types_params;
}
void check_varDecl(struct node *varDecl,struct symbol_list *scope){
    struct node *type= getchild(varDecl,0); // Primeiro filho
    struct node *id = getchild(varDecl,1);
    if (search_symbol(scope,id->token)!=NULL)
    {
        semantic_errors++;
        printf("Line %d, column %d: Symbol %s already defined\n", id->token_line, id->token_column, id->token);
        return;
    }
    enum category typeCategory= type->category;
    enum type varType = category_type(typeCategory);
    insert_symbol(scope, id->token, varType, varDecl, 0, 0,0);
}





void check_statements(struct node *statement,struct symbol_list *scope){

    enum category category = statement->category;
    if (category==ParseArgs){
        struct node *first = getchild(statement,0) ;
        struct node *second = getchild(statement,1);
        enum type first_type =  check_expression(first,scope);
        enum type second_type = check_expression(second,scope);
        if((second_type != integer && second_type != string) || (first_type != integer )){
            semantic_errors++;
            printf("Line %d, column %d: Operator strconv.Atoi cannot be applied to types %s, %s\n", statement->token_line, statement->token_column, name_types[first_type], name_types[second_type]);
        }
        statement->type = second_type;
    }
    if (category==Return)
    {
        if(statement==NULL){
            return;
        }
        enum type return_type;
        struct node *return_expression = getchild(statement,0);
        if (return_expression!=NULL)
        {
            return_type = check_expression(return_expression,scope);
            if(return_type != search_symbol(scope, "return")->type){
                semantic_errors++;
                printf("Line %d, column %d: Incompatible type %s in return statement\n",return_expression->token_line, return_expression->token_column , name_types[return_type]);
              
            }
        }else{
            return_type = none;
            if(search_symbol(scope,"return")->type !=none){
                semantic_errors++;
                printf("Line %d, column %d: Incompatible type void in return statement\n",return_expression->token_line, return_expression->token_column);
            }
        }
        
    }
    if (category == Print){
        struct node *first =getchild(statement,0);
        enum type first_type=check_expression(first,scope);
        if (first_type==undef || first_type==none)
        {
            semantic_errors++;
            printf("Line %d, column %d: Incompatible type %s in fmt.Println statement\n", first->token_line, first->token_column, name_types[first_type]);
        }
        
    }

    if (category == If){
        if(statement == NULL){
            return ;
        }

        struct node *body = getchild(statement , 0);
        struct node *block_if = getchild(statement , 1);
        struct node *block_else = getchild(statement , 2);

        enum type type = check_expression(body,scope);
        if (type!=bool)
        {
            semantic_errors++;
            printf("Line %d, column %d: Incompatible type %s in if statement\n", body->token_line, body->token_column, name_types[type]);
        }
        

        if(block_if !=  NULL){
            check_statements(block_if,scope);
        }
        if(block_else !=  NULL){
            check_statements(block_else,scope);
        }
    }

    if (category == Assign){
        struct node *first = getchild(statement,0) ;
        struct node *second = getchild(statement,1);
        enum type first_type;
        enum type second_type;
        if (!verify_var(first,scope)){
            first_type = undef;
        }else{
            struct symbol_list *symbol;
            if (search_symbol(scope,first->token)==NULL){
                symbol = search_symbol(symbol_table,first->token);
            }else{
                symbol = search_symbol(scope,first->token);
            }
            symbol->used = 1;
            first_type = symbol->type;
        }
        
        first->type = first_type;
        second_type = check_expression(second,scope);
        if((first_type == undef) || (second_type == undef) || (first_type != second_type) ){
            semantic_errors++;
            printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", statement->token_line, statement->token_column, name_types[first_type], name_types[second_type]);
        }
        second->type = second_type;
        statement->type = first_type;
        
    }

   if(category == For){
        if (statement==NULL)
        {
            return;
        }
        struct node *expression =getchild(statement,0);
        struct node *block_for;
        if (expression->category==Block)
        {
            block_for=expression;
            expression=NULL;
        }
        else
        {
            block_for=getchild(statement,1);
        }
        
        enum type type =none;
        

        if (expression!=NULL)
        {
           type=check_expression(expression,scope);               
        }
        if ((type!=bool && type !=none) || type==undef)
        {
            semantic_errors++;
            printf("Line %d, column %d: Incompatible type %s in for statement\n", expression->token_line, expression->token_column, name_types[type]);
        }
        
    
        if (block_for != NULL){
            check_statements(block_for,scope);
        }
        
    }
    
    if (category==Call)
    {
        enum type type_call = check_call(statement,scope);
        statement->type=type_call;
    }

    if(category == Block){
        struct node_list *child = statement->children;
        while ((child = child->next ) != NULL){
            struct node *node = child->node;
            check_statements(node, scope);
        }
    }
    

    
}


enum type check_expression(struct node *expression,struct symbol_list *scope){
    if(expression == NULL){
        return undef;
    }

    enum category category = expression->category;
    enum type expression_type;
    
    expression_type=none;
    
    if (category == And || category == Or ){
        struct node *first = getchild(expression,0);
        struct node *second = getchild(expression,1);
        enum type first_type = check_expression(first,scope);
        enum type second_type = check_expression(second,scope);
        if((first_type != bool) || (second_type != bool)){
            semantic_errors++;
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, category_operator(expression->category),name_types[first_type], name_types[second_type]);
        }
        expression_type = bool;
        expression->type = expression_type;
        return expression_type;
    }
   
    if (category == Le || category == Lt || category == Ge || category == Gt){
        struct node *first = getchild(expression,0);
        struct node *second = getchild(expression,1);
        enum type first_type = check_expression(first,scope);
        enum type second_type = check_expression(second,scope);
        if((first_type != second_type) || !(first_type == integer || first_type == float32)){
            semantic_errors++;
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, category_operator(expression->category),name_types[first_type], name_types[second_type]);
        }
        expression_type = bool;
        expression->type = expression_type;
        return expression_type;
    }
    name_types[none] = "void";
    if (category == Ne || category == Eq){
        struct node *first = getchild(expression,0);
        struct node *second = getchild(expression,1);
        enum type first_type=check_expression(first,scope);
        enum type second_type=check_expression(second,scope);
        if((first_type != second_type) || !(first_type == integer || first_type == float32 || first_type == string || first_type == bool)){
            semantic_errors++;
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",expression->token_line, expression->token_column,category_operator(expression->category),name_types[first_type], name_types[second_type]);
        }
        expression_type = bool;
        expression->type = expression_type;
        return expression_type;
    }

    if(category == Mul || category == Add || category == Sub || category == Div || category == Mod ){
        struct node *first = getchild(expression,0);
        struct node *second = getchild(expression,1);
        enum type first_type = check_expression(first,scope);
        enum type second_type = check_expression(second,scope);
        if((first_type != second_type) || !(first_type == integer || first_type == float32) ){
            semantic_errors++;
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", expression->token_line, expression->token_column, category_operator(expression->category), name_types[first_type], name_types[second_type]);
            expression_type = undef;
        }else {
            expression_type = first_type;
        }        
        expression->type = expression_type ;
        return expression_type;
        
    }

    

    if(category==Natural || category == Decimal){
        expression_type = category_type(category);
        expression->type = expression_type;
        return expression_type;
    }

    if (category==Identifier)
    {
        if (verify_var(expression,scope)){
            struct symbol_list *symbol;
            if ((symbol=search_symbol(scope,expression->token))!=NULL)
            {
                expression_type=symbol->type;
            }else
            {
                symbol=search_symbol(symbol_table,expression->token);
                expression_type = symbol->type;
            }

            symbol->used = 1;
        }else {
            expression_type = undef;
        }
        expression->type = expression_type;
        return expression_type;
        
    }
    if (category==Minus || category==Plus)
    {
        struct node *first =getchild(expression,0);
        enum type first_type = check_expression(first,scope);
        if(first_type == integer || first_type == float32){
            expression_type=first_type;
        }
        else{
            semantic_errors++;
            printf("Line %d, column %d: Operator %s cannot be applied to type %s\n", expression->token_line, expression->token_column, category_operator(expression->category), name_types[first_type]);
            expression_type=undef;
        }
        expression->type=expression_type;
        return expression_type;
    }
    
    if (category==Not)
    {
        struct node *first = getchild(expression,0);
        enum type verify = check_expression(first,scope);
        if(verify != bool){
            semantic_errors++;
            printf("Line %d, column %d: Operator ! cannot be applied to type %s\n", expression->token_line, expression->token_column, name_types[verify]);
        }
        expression_type = bool;
        expression->type = expression_type;
        return expression_type;
    }
    
    if(category==StrLit){
        expression_type = string;
        expression->type = expression_type;
        return expression_type;
    }
    
    if (category == Call){
        expression_type = check_call(expression,scope);
        expression->type = expression_type;
        return expression_type;
    }


    expression->type = expression_type;
    return expression_type;


}



// insert a new symbol in the list, unless it is already there
struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, struct node *node, int is_param, int is_func , int used) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->next = NULL;
    new->is_param=is_param;
    new->is_func=is_func;
    new->func_param =NULL;
    new->used = used;
    if(is_func){
        new->func_param = (char *)malloc(4096);
    }
    struct symbol_list *symbol = table;
    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    /* insert new symbol at the tail of the list */
            break;
        } else if(strcmp(symbol->next->identifier, identifier) == 0) {
            free(new);
            return NULL;           /* return NULL if symbol is already inserted */
        }
        symbol = symbol->next;
    }
    return new;
}

// look up a symbol by its identifier
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier) {
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next)
        if(strcmp(symbol->identifier, identifier) == 0)
            return symbol;
    return NULL;
}

char *get_param_type(char *type){
    
    struct symbol_list *symbol = search_symbol(symbol_table,type);
    if (symbol == NULL)
    {
        return "";
    }

    if(symbol->func_param == NULL){
        return "";
    }
    
    char *typ = (char *)malloc(4096);
    strcpy(typ, symbol->func_param);
    return typ;
}

int verify_var(struct node *var , struct symbol_list *scope){
    
    if ( ((search_symbol(scope, var->token) == NULL) && (search_symbol(symbol_table, var->token) == NULL))
        ||  ((search_symbol(scope, var->token) == NULL) && ((search_symbol(symbol_table, var->token) != NULL && search_symbol(symbol_table, var->token)->is_func)))){
        semantic_errors++; 
        printf("Line %d, column %d: Cannot find symbol %s\n", var->token_line, var->token_column, var->token);   
        return 0;
    }

    return 1;
    
}
enum type check_call(struct node *call,struct symbol_list *scope){
    if (call==NULL)
    {
        return undef;
    }
    
    struct node *id = getchild(call,0);
    char *call_param= (char *)malloc(4096); // tipos dos argumentos da funcinvocation
    call_param[0]='\0';
    struct node_list *current_arg= call->children->next;
    current_arg=current_arg->next;
    int function_called=1; 
    while (current_arg!=NULL)
        {
            struct node *argument=current_arg->node;
            enum type argument_type = check_expression(argument,scope);
            char *argument_type_string =t_names[argument_type];
            strcat(call_param,argument_type_string);
            strcat(call_param,",");
            current_arg=current_arg->next;
        }
    if(strlen(call_param)>0){
        call_param[strlen(call_param)-1]='\0';
    }
        
        char *function_param=get_param_type(id->token);
        enum type type_return;
        struct symbol_list *function=search_symbol(symbol_table,id->token);
        if(function==NULL){
            type_return=undef;
            printf("Line %d, column %d: Cannot find symbol %s(%s)\n",id->token_line, id->token_column,id->token, call_param);
            semantic_errors++;
            function_called=0;
        }
        else{
            if (function->is_func==0 || strcmp(function_param,call_param) != 0)
            {
                function_called=0;
                type_return=undef;
                printf("Line %d, column %d: Cannot find symbol %s(%s)\n",id->token_line, id->token_column,id->token, call_param);
                semantic_errors++;
            }
            else{
                type_return=function->type;
            }   
        }
        if (function_called)
        {
            id->parameters=strdup(call_param);
        }
        else{
            id->parameters=strdup("undef");        
            }
        return type_return;
}

void unused(){
    struct symbol_list *symbol;

    struct scopes_list *scope = list_scopes;

    while(scope!=NULL){
        symbol = scope->table->next;
        while(symbol!=NULL){
            if(!symbol->is_param && symbol->used == 0){
                struct node *node = symbol->node;
                struct node *id = getchild(node,1);
                printf("Line %d, column %d: Symbol %s declared but never used\n", id->token_line, id->token_column, id->token);
                
            }
            symbol = symbol->next;
        }
        scope = scope->next;
    }
}

void show_symbol_table() {
    printf("===== Global Symbol Table =====\n");
    struct symbol_list *symbol;
    for(symbol = symbol_table->next; symbol != NULL; symbol = symbol->next){
       if (symbol->node->category== VarDecl)
       {
        printf("%s\t\t%s\n",symbol->identifier,t_names[symbol->type]);
       }
       if (symbol->node->category== FuncDecl)
       {
        printf("%s\t(%s)\t%s\n", symbol->identifier,get_param_type(symbol->identifier),t_names[symbol->type]);
       }
       
    }
    printf("\n");
}

void show_scopes(){
    struct scopes_list *scope = list_scopes;
    while (scope!=NULL)
    {
        printf("===== Function %s(%s) Symbol Table =====\n",scope->identifier, get_param_type(scope->identifier));
        struct symbol_list *symbol = scope->table;
        symbol=symbol->next;
        while (symbol!=NULL)
        {
            printf("%s\t\t%s", symbol->identifier, t_names[symbol->type]);
            if (symbol->is_param) {
                printf("\tparam");
            }
            symbol=symbol->next;
            printf("\n");
        }
        
        printf("\n");
        scope=scope->next; 
    }
    
}
