#include <stdlib.h>
#include <stdio.h>
#include "ast.h" 
#include <string.h>
// create a node of a given category with a given lexical symbol
struct node *newnode(enum category category, char *token) {
    struct node *new = malloc(sizeof(struct node));
    new->category = category;
    new->token = token;
    new->type=none;
    new->children = malloc(sizeof(struct node_list));
    new->children->node = NULL;
    new->children->next = NULL;
    new->parameters=NULL;
    return new;
}

// append a node to the list of children of the node node
void addchild(struct node *node, struct node *child) {
    if(node == NULL || child == NULL){
        return;
    }
    struct node_list *new = malloc(sizeof(struct node_list));
    new->node = child;
    new->next = NULL;
    struct node_list *children = node->children;
    while(children->next != NULL)
        children = children->next;
    children->next = new;
}

// get a pointer to a specific child, numbered 0, 1, 2, ...
struct node *getchild(struct node *parent, int position) {
    struct node_list *children = parent->children;
    while((children = children->next) != NULL)
        if(position-- == 0)
            return children->node;
    return NULL;
}
char *t_names[] = name_type;
void remove_first(struct node *parent) {
    struct node_list *children = parent->children;
    if(children->next == NULL)
        return;
    struct node_list *new = children->next;
    parent->children = new;
    free(children);
}
// Conta o número de filhos de um nó (usado para verificar quantos elementos há em um bloco)
int block_count(struct node *block) {
    if (block == NULL || block->children == NULL) {
        return 0;  // Sem filhos
    }

    int count = 0;
    struct node_list *current = block->children->next;  // Inicia na primeira criança real (após a inicialização)

    while (current != NULL) {
        count++;
        current = current->next;  // Passa para o próximo filho na lista
    }

    return count;  // Retorna o número total de filhos
}


void remove_extras(struct node *parent) {
    if (parent == NULL || parent->children == NULL) {
        return;
    }

    struct node_list *anterior = parent->children; // Ponteiro para a lista de filhos
    struct node_list *atual = anterior->next;  // Ponteiro para o primeiro filho

    while (atual != NULL) {
        if (atual->node->category == Extra) {
            // Se o nó atual é da categoria "Extra", processa seus filhos
            struct node_list *filhos_extra = atual->node->children->next;

            if (filhos_extra != NULL) {
                // Caso o nó "Extra" tenha filhos
                struct node_list *ultimo_filho_extra = filhos_extra;
                while (ultimo_filho_extra->next != NULL) {
                    // Localiza o último filho da lista de filhos do nó "Extra"
                    ultimo_filho_extra = ultimo_filho_extra->next;
                }

                // Conecta o nó anterior ao primeiro filho do "Extra"
                anterior->next = filhos_extra;

                // Conecta o último filho do "Extra" ao próximo nó após ele
                ultimo_filho_extra->next = atual->next;
            } else {
                // Caso o nó "Extra" não tenha filhos, apenas pula o nó
                anterior->next = atual->next;
            }

            // Libera a memória alocada para o nó "Extra" e sua lista de filhos
            free(atual->node->children);
            free(atual->node);

            struct node_list *liberar = atual;
            atual = anterior->next; // Atualiza o ponteiro para o próximo nó
            free(liberar); // Libera a memória do nó atual
        } else {
            // Se o nó atual não é "Extra", processa seus filhos recursivamente
            remove_extras(atual->node);

            // Move os ponteiros para o próximo nó
            anterior = atual;
            atual = atual->next;
        }
    }
}


void show(struct node *node, int depth) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("..");
    }

    printf("%s", category_to_string(node->category));
    if (node->token != NULL) {
        printf("(%s)", node->token);
    }
    if (node->type!=none)
    {
        printf(" - %s",t_names[node->type]);
    }
    if (node->parameters!=NULL)
    {
        if(strcmp(node->parameters,"undef")==0){
            printf(" - undef");
        }
        else
        {
            printf(" - (%s)",node->parameters);
        }
        
    }
    

    

    printf("\n");

    struct node_list *child = node->children;
    while (child != NULL) {
        show(child->node, depth + 1);
        child = child->next;
    }
}



const char* category_to_string(enum category cat) {
    switch (cat) {
        case Program: return "Program";
        case Function: return "Function";
        case Parameters: return "Parameters";
        case Parameter: return "Parameter";
        case Arguments: return "Arguments";
        case Integer: return "Integer";
        case Double: return "Double";
        case Identifier: return "Identifier";
        case Natural: return "Natural";
        case Decimal: return "Decimal";
        case Call: return "Call";
        case If: return "If";
        case Add: return "Add";
        case Sub: return "Sub";
        case Mul: return "Mul";
        case Div: return "Div";
        case Extra: return "Extra";
        case Int: return "Int";
        case Float32: return "Float32";
        case Bool: return "Bool";
        case String: return "String";
        case Minus: return "Minus";
        case Plus: return "Plus";
        case Not: return "Not";
        case Mod: return "Mod";
        case Ge: return "Ge";
        case Gt: return "Gt";
        case Le: return "Le";
        case Ne: return "Ne";
        case Eq: return "Eq";
        case Lt: return "Lt";
        case Or: return "Or";
        case And: return "And";
        case FuncDecl: return "FuncDecl";
        case FuncHeader: return "FuncHeader";
        case FuncParams: return "FuncParams";
        case FuncBody: return "FuncBody";
        case ParamDecl: return "ParamDecl";
        case ParseArgs: return "ParseArgs";
        case Return: return "Return";
        case Print: return "Print";
        case Block: return "Block";
        case For: return "For";
        case StrLit: return "StrLit";
        case Assign: return "Assign";
        case VarDecl: return "VarDecl";
        default: return "Unknown";
    }
    

}



