  /*
  *  The scanner definition for COOL.
  */

  /*
  *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
  *  output, so headers and global definitions are placed here to be visible
  * to the code in the file.  Don't remove anything that was here initially
  */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
* This change makes it possible to use this scanner in
* the Cool compiler.
*/
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
        if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
                YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
*  Add Your own definitions here
*/

%}

  /*
  * Define names for regular expressions here.
  */

IDSUFFIX        [a-zA-Z0-9_]

DARROW          =>

/* keywords */

CLASS           [Cc][Ll][Aa][Ss][Ss]
ELSE            [Ee][Ll][Ss][Ee]
IF              [Ii][Ff]
FI              [Ff][Ii]
IN              [Ii][Nn]
INHERITS        [Ii][Nn][Hh][Ee][Rr][Ii][Tt][Ss]
LET             [Ll][Ee][Tt]
LOOP            [Ll][Oo][Oo][Pp]
POOL            [Pp][Oo][Oo][Ll]
THEN            [Tt][Hh][Ee][Nn]
WHILE           [Ww][Hh][Ii][Ll][Ee]
CASE            [Cc][Aa][Ss][Ee]
ESAC            [Ee][Ss][Aa][Cc]
OF              [Oo][Ff]
NEW             [Nn][Ee][Ww]
ISVOID          [Ii][Ss][Vv][Oo][Ii][Dd]
NOT             [Nn][Oo][Tt]
TRUE            t[Rr][Uu][Ee]
FALSE           f[Aa][Ll][Ss][Ee]

%x COMMENT
%x SINGLE_STRING
  int nested_comments = 0;


%%

  /*
   * Newline
   */
"\n"           { curr_lineno++; }
[ \t\b\f\r\v]+     {  }

  /*
   *  Nested comments
   */

<COMMENT,INITIAL>"(*" { BEGIN(COMMENT); nested_comments++; }
<COMMENT>{
  "*)"    --nested_comments == 0 ? BEGIN(INITIAL) : 0;
  <<EOF>> {
    cool_yylval.error_msg = "EOF in comment";
    BEGIN(INITIAL);
    return (ERROR);
  }
  \n { curr_lineno++; }
  .  ;
}

"*)"  { cool_yylval.error_msg = "Unmatched *)"; return (ERROR); }

  /*
   * Single line comment
   */
"--".*         ;

  /*
   *  The multiple-character operators.
   */
{DARROW}    { return (DARROW); }
"."         { return ('.'); }
"@"         { return ('@'); }
"~"         { return ('~'); }
"*"         { return ('*'); }
"/"         { return ('/'); }
"+"         { return ('+'); }
"-"         { return ('-'); }
"<="        { return (LE); }
"<"         { return ('<'); }
"="         { return ('='); }
"<-"        { return (ASSIGN); }

";"         { return (';'); }
"("         { return ('('); }
")"         { return (')'); }
"{"         { return ('{'); }
"}"         { return ('}'); }
","         { return (','); }
":"         { return (':'); }


  /*
   * Keywords are case-insensitive except for the values true and false,
   * which must begin with a lower-case letter.
   */

{CLASS}     { return (CLASS); }
{ELSE}      { return (ELSE); }
{IF}        { return (IF); }
{FI}        { return (FI); }
{IN}        { return (IN); }
{INHERITS}  { return (INHERITS); }
{LET}       { return (LET); }
{LOOP}      { return (LOOP); }
{POOL}      { return (POOL); }
{THEN}      { return (THEN); }
{WHILE}     { return (WHILE); }
{CASE}      { return (CASE); }
{ESAC}      { return (ESAC); }
{OF}        { return (OF); }
{NEW}       { return (NEW); }
{ISVOID}    { return (ISVOID); }
{NOT}       { return (NOT); }

  /*
   *  Boolean constants
   */
{TRUE}      { cool_yylval.boolean = 1; return (BOOL_CONST); }
{FALSE}     { cool_yylval.boolean = 0; return (BOOL_CONST); }

  /*
   *  String constants (C syntax)
   *  Escape sequence \c is accepted for all characters c. Except for
   *  \n \t \b \f, the result is c.
   *
   */

\" { BEGIN(SINGLE_STRING); memset(string_buf_ptr = string_buf, 0, sizeof(string_buf)); }
<SINGLE_STRING>{
  \n {
        cool_yylval.error_msg = "Unterminated string constant";
        curr_lineno++;
        BEGIN(INITIAL);
        return (ERROR);
  }
  <<EOF>> {
        cool_yylval.error_msg = "EOF in string constant";
        BEGIN(INITIAL);
        return (ERROR);
  }
  ([^\\\"\n]|\\(.|\n))* {
        if (strlen(yytext) != (size_t)yyleng) {
          cool_yylval.error_msg = "String contains null character";
          string_buf_ptr = nullptr;
          return (ERROR);
        }
        // printf("-------%s_______\n", string_buf);
        // std::cout << yyleng << ' ' << str.length() << std::endl;
        string_buf_ptr = string_buf;
        for (auto it = yytext; *it != '\0'; it++, string_buf_ptr++) {
          if (*it == '\n') {
            curr_lineno++;
          }
          if (*it == '\\') {
            it++;
            switch(*it) {
              case 'n':
                *it = '\n';
                break;
              case 't':
                *it = '\t';
                break;
              case 'b':
                *it = '\b';
                break;
              case 'f':
                *it = '\f';
                break;
              default:
                break;
            }
          }
          *string_buf_ptr = *it;
        }
        *string_buf_ptr = '\0';
      }
  \"                        {
          BEGIN(INITIAL);
          if (string_buf_ptr != nullptr) {
            cool_yylval.symbol = stringtable.add_string(string_buf);
            return (STR_CONST);
          }
        }
  . ;
}

[a-z]{IDSUFFIX}*  { cool_yylval.symbol = idtable.add_string(yytext); return (OBJECTID); }
[A-Z]{IDSUFFIX}*  { cool_yylval.symbol = idtable.add_string(yytext); return (TYPEID); }

[0-9]+ { cool_yylval.symbol = inttable.add_string(yytext); return (INT_CONST); }

  /*
   *  Unknown token
   */
. { cool_yylval.error_msg = yytext; return (ERROR); }

%%
