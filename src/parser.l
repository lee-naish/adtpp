

%{
#define YY_NO_INPUT
#include <stdio.h>
#include "y.tab.h"
%}

%option nounput

%%
data			return DATA;
type		return INST;
function		return FUNC;
instance           return FINST;
func           return HOF;
[a-zA-Z_][a-zA-Z0-9_]*    yylval.string=strdup(yytext); return WORD;
\(			return FBRACE;
\)			return SBRACE;
\,			return COMMA;
\{                      return OBRACE;
\}                      return EBRACE;
;                       return SEMICOLON;
:			return COLON;
\<			return OCHEV;
\>			return ECHEV;
=			return DATAASS;
\n                      /* ignore EOL */;
[ \t]+                  /* ignore whitespace */;
\/\*.*\*\/              /* ignore original style C comments (like this) */
\/\/.*\n                // ignore "end of line" C comments (like this)
%%
