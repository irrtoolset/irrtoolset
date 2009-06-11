%option outfile="object_log.l.cc"
%option prefix="object_log"

NAME           [[:alpha:]]([[:alnum:]_-]*[[:alnum:]])?

%{
#include "config.h"
#include "List.hh"
#include "rpsl/object.hh"

#define LEXER_RETURN return
#define YY_DECL int _objectLog(List<Object> &log)	
#define YY_USER_ACTION scan_position += yyleng;
#define YYLESS(n) {scan_position -= yyleng - n; yyless(n); }

static int scan_position = 0;
static char *scan_text = NULL;

%}

%%
\n+          ;  // skip blank lines that do not delimit objects
^(%.*\n)+    ;  // skip comments that do not belong to an object
^(#.*\n)+    ;  // skip comments that do not belong to an object

^{NAME}:.*\n(.+\n)* { // object
   Buffer object(scan_text + scan_position - yyleng, yyleng);
   log.append(new Object(object));
}

%%
extern "C" {
int yywrap () {
   return 1;
}
}

void objectLog(const Buffer &buf, List<Object> &log) {
   YY_BUFFER_STATE log_buffer = yy_scan_bytes(buf.contents, buf.size);
   scan_position = 0;
   scan_text = buf.contents;
   _objectLog(log);
   yy_delete_buffer(log_buffer);
}
