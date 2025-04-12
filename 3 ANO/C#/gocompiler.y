/* START definitions section -- C code delimited by %{ ... %} and token declarations */

%{

#include "ast.h"
#include "y.tab.h"
#include <string.h>
void yyerror(char *s);


int yylex(void);


struct node *program;

%}
%union{
    char *lexeme;
    struct node *node;
}

%locations
%{
    #define LOCATE(node, line, column) { node->token_line = line; node->token_column = column;}
%}

%token PACKAGE BLANKID PARSEINT RETURN FOR VAR STRING FUNC CMDARGS INT FLOAT32 BOOL IF ELSE PRINT SEMICOLON COMMA LPAR RPAR LBRACE RBRACE LSQ RSQ LT LE GT GE ASSIGN EQ NE MOD NOT AND OR STAR DIV PLUS MINUS  
%token<lexeme> IDENTIFIER NATURAL DECIMAL STRLIT RESERVED 
%type<node> funcDeclaration_extras funcDeclaration_paramatros program parameters declarations funcBody funcInvocation_extra varDeclaration varSpec type funcDeclaration varsAndStatements statement parseArgs expr funcInvocation varSpec_extra statement_extra   statement_else expr_opc

%left       LOW
%nonassoc   IF ELSE FOR
%left       COMMA
%right      ASSIGN
%left       OR
%left       AND
%left       LT GT LE GE EQ NE
%left       PLUS MINUS
%left       STAR DIV MOD
%left       LBRACE RBRACE LPAR RPAR LSQ RSQ
%right      NOT 


/* START grammar rules section -- BNF grammar */

%%

//REGRAS
program: PACKAGE IDENTIFIER SEMICOLON declarations{
    $$ = program = newnode(Program,NULL);
    addchild($$,$4);
}
        

declarations: declarations varDeclaration SEMICOLON{
                $$ = $1;
                struct node *varDeclarations = newnode(Extra,NULL);
                addchild($$,varDeclarations);
                addchild(varDeclarations,$2);
}
            | declarations funcDeclaration SEMICOLON{
                $$ = $1;
                addchild($$,$2);
            }
            | {
                $$ = newnode(Extra, NULL); 
                 }
            ;

varDeclaration: VAR varSpec{
                $$=$2;
}
              | VAR LPAR varSpec SEMICOLON RPAR{
                $$=$3;
              }
              ;

varSpec: IDENTIFIER varSpec_extra type {
        $$ = newnode(Extra,NULL); // cria nó extra e mete como filho um nó com o tipo e o identifier respetivo,
        struct node *declaration = newnode(VarDecl,NULL);
        addchild(declaration,$3);
        addchild(declaration,newnode(Identifier,$1));
        addchild($$,declaration);
        LOCATE(getchild(declaration, 1), @1.first_line, @1.first_column);
        if($2!=NULL){ // mais que uma var para um type e vai buscar a partir da segunda declaração em Extra para o mesmo type
            enum category type_name =$3->category; //acessa e guarda o type
            struct node_list *cur_declaration = $2->children->next; // vai buscar o nó do identifier
            while(cur_declaration != NULL){
                struct node *declaration_node_atual = newnode(VarDecl, NULL); // Cria novo nó de declaração de variável
                struct node *identifier_node = cur_declaration->node->children->next->node;
                addchild(declaration_node_atual, newnode(type_name, NULL)); 
                addchild(declaration_node_atual, identifier_node); 
                addchild($$, declaration_node_atual); 
                cur_declaration = cur_declaration->next;
            } 
           
        } 
}
       ; 
 
varSpec_extra:varSpec_extra COMMA IDENTIFIER  {
            // adiciona os identifiers extras do mesmo type
    $$ = $1;
            struct node *declaration = newnode(VarDecl,NULL);
            addchild(declaration,newnode(Identifier,$3));
            addchild($$,declaration);
            LOCATE(getchild(declaration, 0), @3.first_line, @3.first_column);

      } 
             | {
                $$ = newnode(Extra, NULL);
                
                 }
             ;

type: INT {
            $$ = newnode(Int , NULL);
          }
    | FLOAT32{
            $$ = newnode(Float32 , NULL);
           }
    | BOOL {
            $$ = newnode(Bool , NULL);
          }
    | STRING{
            $$ = newnode(String , NULL);
          }
    ;

funcDeclaration: FUNC IDENTIFIER LPAR funcDeclaration_paramatros RPAR funcDeclaration_extras funcBody {     
                //criar no funcdecl
                $$ = newnode(FuncDecl,NULL);
                //criar novo nó filho de funcdecl (funcheader) que armazene name params etc
                struct node *funcHeader = newnode(FuncHeader,NULL);
                //add filho de decl
                addchild($$,funcHeader);
                //ao funcheader adicionar filho identifier
                addchild(funcHeader,newnode(Identifier,$2));
                //add tipo de retonro da funcao a funcheader
                addchild(funcHeader,$6);
                //add paramentros opiconais da funcao
                addchild(funcHeader,$4);
                //add funcbody
                addchild($$,$7);
                LOCATE(getchild(funcHeader,0),@2.first_line,@2.first_column);

                }
               ;


funcDeclaration_paramatros:parameters {
                    $$ = $1;
                    }
                | {
                    $$ = newnode(FuncParams, NULL);
                }
                ;



funcDeclaration_extras: type {
                            $$ = $1;
                        }
                | {
                        $$ = newnode(Extra, NULL); {}
                    }
                ;

parameters: IDENTIFIER type {
            $$ = newnode(FuncParams,NULL);
            struct node *new_parameter = newnode(ParamDecl,NULL);
            addchild(new_parameter,$2);
            addchild(new_parameter,newnode(Identifier,$1));
            addchild($$,new_parameter);
            LOCATE(getchild(new_parameter, 1), @1.first_line, @1.first_column);

}
           | parameters COMMA IDENTIFIER type {
            $$ = $1;
            struct node *new_parameter= newnode(ParamDecl,NULL);
            addchild(new_parameter,$4);
            addchild(new_parameter,newnode(Identifier,$3));
            addchild($$,new_parameter);
            LOCATE(getchild(new_parameter, 1), @3.first_line, @3.first_column);

           }
          ; 



funcBody: LBRACE varsAndStatements RBRACE {
            $$ = newnode(FuncBody,NULL);
            addchild($$,$2);
        }
        ;

varsAndStatements: varsAndStatements SEMICOLON {
                $$ = $1;
                 }
                 | varsAndStatements varDeclaration SEMICOLON {
                    $$ = $1;
                    //existindo decLARACAO DE variavel adicionar
                    addchild($$,$2);
                 }
                 | varsAndStatements statement SEMICOLON {
                    $$ = $1;
                    //existindo statement adicionar
                    addchild($$,$2);
                 }
                 | 
                 {
                    $$ = newnode(Extra, NULL);
                 }
                 ;


statement: IDENTIFIER ASSIGN expr {
          $$ = newnode(Assign,NULL);
          addchild($$,newnode(Identifier,$1));
          addchild($$,$3);
          LOCATE(getchild($$, 0), @1.first_line, @1.first_column);
          LOCATE($$, @2.first_line, @2.first_column);
         }
         | LBRACE statement_extra RBRACE {
            if(block_count($2)>1){
                $$=newnode(Block,NULL);
                addchild($$,$2);
            }
            else{ //se tiver só uma instrução
                $$=$2;
            }

         }
         | IF expr LBRACE statement_extra RBRACE statement_else %prec LOW /*associa o else ao if mais próximo*/ {
                $$ = newnode(If,NULL);
                struct node *blockIf =newnode(Block,NULL);
                struct node *blockElse =newnode(Block,NULL);
                // adiciona todos os elemetos/blocos do if para o seu nó
                //adiciona a condição
                addchild($$,$2);
                // instruções do if
                addchild(blockIf,$4);
                addchild($$,blockIf);

                //instruções do else
                addchild(blockElse,$6);
                addchild($$,blockElse);


         }
         | FOR expr_opc LBRACE statement_extra RBRACE {
            $$ = newnode(For,NULL);
            //add expressao que ta dentro do for
            addchild($$,$2);
            struct node* blockFor = newnode(Block,NULL);
            // o que está dentro do for é adiiconado
            addchild(blockFor,$4);
            //blockfor filho de for
            addchild($$,blockFor);

         }
         | RETURN expr_opc {
            $$ = newnode(Return,NULL);
            //return expr
            addchild($$,$2);
            LOCATE($$, @1.first_line, @1.first_column);
         }
         | funcInvocation {
                    $$=$1;
         }
         | parseArgs  {
                    $$=$1;
            }
         | PRINT LPAR expr RPAR {
            $$ = newnode(Print,NULL);
            //add expr a ser impresso
            addchild($$,$3);
            }
         | PRINT LPAR STRLIT RPAR {
            $$ = newnode(Print,NULL);
            //add strlit a ser impresso
            addchild($$,newnode(StrLit,$3));

         }
         | error {
                        $$ = newnode(Extra, NULL);
                    }
         ;

expr_opc: expr {
                $$ = $1; //retorna expr se exisitr
               }
              |  {
             $$ = newnode(Extra, NULL);
             }
              ;

statement_extra: statement_extra statement SEMICOLON {
                    if ($2->category != Extra || block_count($2) > 0) {
                        $$ = $1;
                        addchild($$, $2);
                    } else {
                        $$ = $1;
                    }
                }
               |{
                    $$ = newnode(Extra, NULL);
                }
               ;

statement_else:ELSE LBRACE statement_extra RBRACE {
                    $$ = $3;
                }
                |{
                    $$ = newnode(Extra, NULL);
                 }
                ;

parseArgs: IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ expr RSQ RPAR {
                $$ = newnode(ParseArgs,NULL);
                addchild($$,newnode(Identifier,$1));
                addchild($$,$9);
                LOCATE($$, @5.first_line, @5.first_column);
            }
         | IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR {
                        $$ = newnode(Extra, NULL);
                    }
          ;

funcInvocation: IDENTIFIER LPAR funcInvocation_extra RPAR{
                    $$ = newnode(Call,NULL);
                    addchild($$,newnode(Identifier,$1));
                    struct node *params = newnode(Extra, NULL);
                    addchild($$, params);
                    addchild($$, $3);
                    LOCATE(getchild($$, 0), @1.first_line, @1.first_column);
                }
              | IDENTIFIER LPAR error RPAR {
                        $$ = newnode(Extra, NULL);
                    }
              ;

funcInvocation_extra : expr {
                        $$ = newnode(Extra, NULL);
                        addchild($$, $1);
                     }
                    | expr COMMA funcInvocation_extra{
                        $$ = newnode(Extra, NULL);
                        addchild($$, $1);
                        addchild($$, $3);
                    }
                    | {
                        $$ = newnode(Extra, NULL);
                    }
                    ;


expr: expr OR expr  {
                    $$ = newnode(Or,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }       
    | expr AND expr {
                    $$ = newnode(And,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);

                    }       
    | expr LT expr {
                    $$ = newnode(Lt,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                     LOCATE($$, @2.first_line, @2.first_column);

                    }       
    | expr GT expr  {
                    $$ = newnode(Gt,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);

                    }      
    | expr EQ expr   {
                    $$ = newnode(Eq,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);

                    }     
    | expr NE expr  {
                    $$ = newnode(Ne,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }      
    | expr LE expr  {
                    $$ = newnode(Le,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }      
    | expr GE expr  {
                    $$ = newnode(Ge,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }      
    | expr PLUS expr {           
                    $$ = newnode(Add,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }       
    | expr MINUS expr {        
                    $$ = newnode(Sub,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }       
    | expr STAR expr {
                    $$ = newnode(Mul,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }       
    | expr DIV expr  {
                    $$ = newnode(Div,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }      
    | expr MOD expr  {
                    $$ = newnode(Mod,NULL);
                    addchild($$,$1);
                    addchild($$,$3);
                    LOCATE($$, @2.first_line, @2.first_column);
                    }      
    | NOT expr     %prec NOT  {                     
                    $$ = newnode(Not,NULL);
                    addchild($$,$2);
                    LOCATE($$, @1.first_line, @1.first_column);
                    }  
    | MINUS expr  %prec NOT {                        
                    $$ = newnode(Minus,NULL);
                    addchild($$,$2);
                    LOCATE($$, @1.first_line, @1.first_column);
                    }  
    | PLUS expr   %prec NOT {                        
                    $$ = newnode(Plus,NULL);
                    addchild($$,$2);
                    LOCATE($$, @1.first_line, @1.first_column);
                    } 
    | NATURAL {
                            $$ = newnode(Natural, $1);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
    | DECIMAL{
                            $$ = newnode(Decimal, $1);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
    | IDENTIFIER        {
                            $$ = newnode(Identifier, $1);
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
    | funcInvocation {
                            $$ = $1;
                            LOCATE($$, @1.first_line, @1.first_column);
                        }
    | LPAR expr RPAR {
                            $$ = $2;
                     }
    | LPAR error RPAR   {
                            $$ = newnode(Extra, NULL);
                        }
    ;
 
%%

/* START subroutines section */


// all needed functions are collected in the .l and ast.* files