/* André Lourenço Albuquerque - 2022231505
Rodrigo José  Morenito Borges - 2022244993	*/
%{
    #include "y.tab.h"
    #include "ast.h"
    #include "semantics.h"
    #define YY_USER_ACTION { syn_line = yylloc.first_line= l; syn_column =yylloc.first_column= c; c += yyleng; }
    #define PASS_TOKEN() yylval.lexeme = strdup(yytext);
    int yyparse(void);
    void yyerror(char *);
    int syn_line = -1, syn_column = -1;
    int errors = 0;
    
    extern struct node *program;
    int l =1;
    int c=1;
    int prel;
    int prec;
    char str[4096];
    int str_index=0;
    int cancel=0;
    int semicolon_need=0;
    int comment_l_error=0;
    int comment_c_error=0;
    int string_start_l=0;
    int string_start_c=0;
    int commentline_c =0;
    int commentline_l =0;
    int option = 0;
    int eof = 0;
    int erros=0;
    void add_str(char *add, int length) {
        strncpy(&str[str_index], add, length);
        str_index += length;
    }
    void reset_str() {
        str_index = 0;
        str[0] = '\0';
    }
%}
%X COMMENT
%X STR
%X COMMENTLINE
digit   [0-9]
letter  [a-zA-Z_]
hexadecimal [0-9a-fA-F]
octal [0-7]
exp [eE][-+]?{digit}+
nextline "\n"|"\r"|"\r\n"
STRLIT     ([^\n\r\\"]|\\n|\\f|\\r|\\\\|\\\"|\\t)*

%%

"\n"|"\r"|"\r\n"            {
                            l++; c=1;
                            if (semicolon_need == 1) {
                                semicolon_need = 0; 
                                if(option){
                                     printf("SEMICOLON\n"); 
                                }
                                PASS_TOKEN();
                                return SEMICOLON;          
                            }
                            
                            }

" "|"\t"|"\f"                   { ; }

"("                        {if(option) printf("LPAR\n");semicolon_need = 0;return LPAR;}
")"                        {if(option) printf("RPAR\n");semicolon_need = 1;return RPAR;}
"{"                        {if(option) printf("LBRACE\n");semicolon_need = 0;return LBRACE;}
"}"                        {if(option) printf("RBRACE\n");semicolon_need = 1;return RBRACE;}
"<="                       {if(option) printf("LE\n");semicolon_need = 0;return LE;}
"["                        {if(option) printf("LSQ\n");semicolon_need = 0;return LSQ;}
"]"                        {if(option) printf("RSQ\n");semicolon_need = 1;return RSQ ;}
"<"                        {if(option) printf("LT\n");semicolon_need = 0;return LT;}
"%"                        {if(option) printf("MOD\n");semicolon_need = 0;return MOD ;}
"!="                       {if(option) printf("NE\n");semicolon_need = 0;return NE ;}
"!"                        {if(option) printf("NOT\n");semicolon_need = 0;return NOT ;}
"&&"                       {if(option) printf("AND\n");semicolon_need = 0;return AND ;}
"||"                       {if(option) printf("OR\n");semicolon_need = 0;return OR ;}
"="                        {if(option) printf("ASSIGN\n");semicolon_need = 0;return ASSIGN ;}
"=="                       {if(option) printf("EQ\n");semicolon_need = 0;return EQ ;}
">="                       {if(option) printf("GE\n");semicolon_need = 0;return GE ;}
">"                        {if(option) printf("GT\n");semicolon_need = 0;return GT ;}
";"                        {if(option) printf("SEMICOLON\n");semicolon_need = 0;return SEMICOLON ;}
","                        {if(option) printf("COMMA\n");semicolon_need = 0;return COMMA ;}
"_"                        {if(option) printf("BLANKID\n");semicolon_need = 0;return BLANKID ;}
"*"                        {if(option) printf("STAR\n");semicolon_need = 0;return STAR ;}
"/"                        {if(option) printf("DIV\n");semicolon_need = 0;return DIV;}
"+"                        {if(option) printf("PLUS\n");semicolon_need = 0;return PLUS ;}
"-"                        {if(option) printf("MINUS\n");semicolon_need = 0;return MINUS;}

"package"                  {if(option) printf("PACKAGE\n");semicolon_need = 0;return PACKAGE;}
"return"                   {if(option) printf("RETURN\n");semicolon_need = 1;return RETURN;}
"for"                      {if(option) printf("FOR\n");semicolon_need = 0;return FOR;}
"var"                      {if(option) printf("VAR\n");semicolon_need = 0;return VAR;}
"string"                   {if(option) printf("STRING\n");semicolon_need = 0;return STRING;}
"fmt.Println"              {if(option) printf("PRINT\n");semicolon_need = 0;return PRINT;}
"strconv.Atoi"             {if(option) printf("PARSEINT\n");semicolon_need = 0;return PARSEINT;}
"func"                     {if(option) printf("FUNC\n");semicolon_need = 0;return FUNC;}
"os.Args"                  {if(option) printf("CMDARGS\n");semicolon_need = 0;return CMDARGS;}
"break"                    {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"case"                     {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"chan"                     {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"const"                    {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"continue"                 {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"default"                  {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"defer"                    {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"fallthrough"              {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"go"                       {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"goto"                     {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"import"                   {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"interface"                {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"map"                      {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"range"                    {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"select"                   {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"struct"                   {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"switch"                   {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"type"                     {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"int"                      {if(option) printf("INT\n");semicolon_need = 0;return INT;}
"float32"                  {if(option) printf("FLOAT32\n");semicolon_need = 0;return FLOAT32;}
"bool"                     {if(option) printf("BOOL\n");semicolon_need = 0;return BOOL;}
"if"                       {if(option) printf("IF\n");semicolon_need = 0;return IF;}
"else"                     {if(option) printf("ELSE\n");semicolon_need = 0;return ELSE;}
"++"                       {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"--"                       {if(option) printf("RESERVED(%s)\n",yytext);semicolon_need = 0;PASS_TOKEN();return RESERVED;}
"0"{octal}+                {if(option) printf("NATURAL(%s)\n",yytext);semicolon_need = 1;PASS_TOKEN();return NATURAL ;}
"0"{octal}*{digit}+        {printf("Line %d, column %d: invalid octal constant (%s)\n", l, c, yytext);}
"0x"{hexadecimal}+         {if(option) printf("NATURAL(%s)\n",yytext);semicolon_need = 1;PASS_TOKEN(); return NATURAL;}
"0X"{hexadecimal}+         {if(option) printf("NATURAL(%s)\n",yytext);semicolon_need = 1;PASS_TOKEN(); return NATURAL;}
{digit}+"."{digit}*{exp}?  {if(option) printf("DECIMAL(%s)\n",yytext);semicolon_need = 1;PASS_TOKEN(); return DECIMAL;}
"."{digit}+{exp}?          {if(option) printf("DECIMAL(%s)\n",yytext);semicolon_need = 1;PASS_TOKEN(); return DECIMAL;}
{digit}+{exp}              {if(option) printf("DECIMAL(%s)\n",yytext);semicolon_need = 1;PASS_TOKEN(); return DECIMAL;}
{letter}+({digit}|{letter})*             {if(option) printf("IDENTIFIER(%s)\n",yytext);semicolon_need = 1;PASS_TOKEN();return IDENTIFIER; }
{digit}+                   {if(option) printf("NATURAL(%s)\n",yytext);semicolon_need = 1; PASS_TOKEN(); return NATURAL;}



"//"                         { BEGIN(COMMENTLINE);
                                commentline_l = l;
                                commentline_c = c;
                               }
<COMMENTLINE><<EOF>>         {BEGIN(INITIAL);printf("Line %d, column %d: unterminated comment\n", commentline_l, commentline_c);}
<COMMENTLINE>{nextline}         {BEGIN(INITIAL);
                                    if(semicolon_need==1) { 
                                        if(option){
                                        printf("SEMICOLON\n");}

                                        if(option==0){
                                            PASS_TOKEN();
                                            return SEMICOLON; 
                                        }
                                    }
                                    l++;semicolon_need=0;
                                    c=1;
                                }
<COMMENTLINE>.                 {;}


"/*" {  
        
        BEGIN(COMMENT); 
        comment_l_error=l;
        comment_c_error=c;
        ;
    }
<COMMENT>"\n"              { l++; c=1; } 
<COMMENT>. { ; }
<COMMENT><<EOF>> { 
    printf("Line %d, column %d: unterminated comment\n", comment_l_error, comment_c_error); 
    semicolon_need=0;
    BEGIN(INITIAL);
}
<COMMENT>"*/" { 
                BEGIN(INITIAL);;
              }





\"          {   
    BEGIN(STR);
    add_str(yytext,yyleng);
    string_start_l=l;
    string_start_c=c-yyleng;
    semicolon_need=1;
    cancel=0;
    }

<STR>{nextline} { 
    printf("Line %d, column %d: unterminated string literal\n", string_start_l, string_start_c);
    BEGIN(INITIAL);  
    l++;
    syn_line++;
    syn_column=1;
    c=1;
    reset_str();
    semicolon_need=0;
}

<STR>{STRLIT}    {
                        add_str(yytext,yyleng);
                    }
<STR>\\(.?)   {
                        printf("Line %d, column %d: invalid escape sequence (%s)\n", string_start_l, c-yyleng,yytext);                       
                        cancel=1;
                        str_index+=yyleng;
                     }
<STR>\"      {       
                        add_str(yytext,yyleng);
                        str[str_index]='\0';
                        if(cancel==0){
                            if(option){
                                printf("STRLIT(%s)\n",str);
                            }
                            else{
                                yylval.lexeme = strdup(str);
                                reset_str();
                                BEGIN(INITIAL); 
                                return STRLIT;
                            }
                            semicolon_need=1;
                        }
                        else{
                            semicolon_need=0;
                        }
                        reset_str();
                        BEGIN(INITIAL);
        }  
<STR><<EOF>> { 
    printf("Line %d, column %d: unterminated string literal\n", string_start_l, string_start_c);
    BEGIN(INITIAL);  
    semicolon_need=0; 
}

<<EOF>> {
    syn_column=1;
    syn_line++;
    if(semicolon_need==1){
        if(option==1){
        printf("SEMICOLON\n");
        }
        semicolon_need=0;
        return SEMICOLON;
    }
    return 0;
}
.           {printf("Line %d, column %d: illegal character (%s)\n",l,c,yytext);;}

%%
extern int yylex();



void yyerror(char *s) {
    erros++;
    if((yylval.lexeme != NULL) && (yylval.lexeme[0] == '"')){
        printf("Line %d, column %d: %s: %s\n", string_start_l, string_start_c, s, yylval.lexeme);
        free(yylval.lexeme);
        yylval.lexeme = NULL;
        reset_str();
    }
    else{
    printf("Line %d, column %d: %s: %s\n",syn_line,syn_column,s,yytext);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "-l") == 0) {
            option = 1;
        }

        yyparse();

        if (strcmp(argv[1], "-t") == 0 && erros == 0) {
            show(program, 0);
        } 
        
        if (strcmp(argv[1], "-s") == 0) {
            remove_extras(program);
            if (errors == 0) {
                errors += check_program(program);
                show_symbol_table();
                show_scopes();
                show(program, 0);
            }
        }
    }
    else{
        yyparse();
        remove_extras(program);
        if(errors==0){
            errors+=check_program(program);
        }
    }

    
    return 0;
}

int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}