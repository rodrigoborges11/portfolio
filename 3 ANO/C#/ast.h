#ifndef _AST_H
#define _AST_H


enum category { Program, Function, Parameters, Parameter, Arguments, Integer, Double, Identifier, Natural, Decimal, Call, If, Add, Sub, Mul, Div , Extra ,Int , Float32 , Bool , String , Minus , Plus , Not , Mod , Ge , Gt , Le , Ne , Eq , Lt , Or , And  , FuncDecl , FuncHeader , FuncParams , FuncBody , ParamDecl , Return , Print , StrLit , Assign ,VarDecl , ParseArgs , For , Block};
#define name { "Program", "Function", "Parameters", "Parameter", "Arguments", "Integer", "Double", "Identifier", "Natural", "Decimal", "Call", "If", "Add", "Sub", "Mul", "Div ", "Extra ","Int" , "Float32" , "Bool ", "String ", "Minus ", "Plus" , "Not" , "Mod ", "Ge ", "Gt" , "Le ", "Ne" , "Eq" , "Lt" , "Or" , "And"  , "FuncDecl ", "FuncHeader" , "FuncParams" , "FuncBody" , "ParamDecl" , "Return" , "Print" , "StrLit" , "Assign" ,"VarDecl" , "ParseArgs" , "For" , "Block"}
enum type {integer,float32 , bool , string , undef , none};
#define name_type { "int", "float32", "bool", "string", "undef", "none" };

struct node {
    enum category category;
    char *token;
    enum type type;
    struct node_list *children;
    int token_line , token_column;
    char *parameters;
};

struct node_list {
    struct node *node;
    struct node_list *next;
};

struct node *newnode(enum category, char *token);
const char* category_to_string(enum category cat);
void addchild(struct node *parent, struct node *child);
int block_count(struct node *block);
struct node *getchild(struct node *parent, int position);
void remove_extras(struct node *cur_node);
void show(struct node *node, int depth);
void remove_first(struct node *parent);
#endif

