#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %name% ::|. %type%
static bool match1(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: realloc ( %any% ,
static bool match2(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("realloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: ( std| ::| nothrow )
static bool match3(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("std"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("nothrow")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: open|openat|creat|mkstemp|mkostemp|socket (
static bool match4(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("open")) || (tok->str()==MatchCompiler::makeConstString("openat")) || (tok->str()==MatchCompiler::makeConstString("creat")) || (tok->str()==MatchCompiler::makeConstString("mkstemp")) || (tok->str()==MatchCompiler::makeConstString("mkostemp")) || (tok->str()==MatchCompiler::makeConstString("socket"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: popen (
static bool match5(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("popen")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% ( %varid% [,)]
static bool match6(const Token* tok, const unsigned int varid) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: .|::
static bool match7(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: %name% (
static bool match8(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: fcloseall ( )
static bool match9(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("fcloseall")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %name% .|::
static bool match10(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: %varid% )|,|-
static bool match11(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: , 0 )
static bool match12(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %varid% =
static bool match13(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: = %varid% ;
static bool match14(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [(,] %varid% [,)]
static bool match15(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] & %varid% [.,)]
static bool match16(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(".,)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}] %varid% .
static bool match17(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: void
static bool match18(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("void")))
        return false;
    return true;
}
// pattern: free ( * %name% )
static bool match19(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("free")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: * %name% =
static bool match20(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: &&|;
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: UNLIKELY|LIKELY
static bool match22(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("UNLIKELY")) || (tok->str()==MatchCompiler::makeConstString("LIKELY"))))
        return false;
    return true;
}
// pattern: if (
static bool match23(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ; alloc ; }
static bool match24(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: ; dealloc ; }
static bool match25(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: (
template<class T> static T * findmatch26(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %varid% [,()]
static bool match27(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",()", tok->str()[0]))
        return false;
    return true;
}
// pattern: dealloc
template<class T> static T * findmatch28(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: use
template<class T> static T * findmatch29(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: &use
template<class T> static T * findmatch30(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&use")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: & %varid% [,()]
static bool match31(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",()", tok->str()[0]))
        return false;
    return true;
}
// pattern: %varid% . %name% [,)]
static bool match32(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}]
static bool match33(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: [{};]
static bool match34(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    return true;
}
// pattern: [{};] :: %name%
static bool match35(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% ::|. %name%
static bool match36(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: continue|break|return|throw|goto|do|else
static bool match37(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("goto")) || (tok->str()==MatchCompiler::makeConstString("do")) || (tok->str()==MatchCompiler::makeConstString("else"))))
        return false;
    return true;
}
// pattern: [;{}] __cppcheck_lock|__cppcheck_unlock ( ) ;
static bool match38(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("__cppcheck_lock")) || (tok->str()==MatchCompiler::makeConstString("__cppcheck_unlock"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %varid% = close ( %varid% )
static bool match39(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("close")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [;{}] %varid% = memcpy|memmove|memset|strcpy|strncpy|strcat|strncat ( %varid% ,
static bool match40(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("memcpy")) || (tok->str()==MatchCompiler::makeConstString("memmove")) || (tok->str()==MatchCompiler::makeConstString("memset")) || (tok->str()==MatchCompiler::makeConstString("strcpy")) || (tok->str()==MatchCompiler::makeConstString("strncpy")) || (tok->str()==MatchCompiler::makeConstString("strcat")) || (tok->str()==MatchCompiler::makeConstString("strncat"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: [(;{}] %varid% =
static bool match41(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr("(;{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: asprintf|vasprintf ( & %varid% ,
static bool match42(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("asprintf")) || (tok->str()==MatchCompiler::makeConstString("vasprintf"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: asprintf|vasprintf (
static bool match43(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("asprintf")) || (tok->str()==MatchCompiler::makeConstString("vasprintf"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: malloc ( %num% )
static bool match44(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("malloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: new struct| %type% [(;]
static bool match45(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("(;", tok->str()[0]))
        return false;
    return true;
}
// pattern: new ( nothrow ) struct| %type%
static bool match46(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("nothrow")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: new ( std :: nothrow ) struct| %type%
static bool match47(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("nothrow")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: = new
static bool match48(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    return true;
}
// pattern: %name%|::|(|[
static bool match49(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: (|[
static bool match50(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %varid%
template<class T> static T * findmatch51(T * start_tok, unsigned int varid) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %name% = %type% (
static bool match52(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [=+(,] %varid%
static bool match53(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr("=+(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: [(,]
static bool match54(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    return true;
}
// pattern: %op%|;|,|{|}|) ::| %name%
static bool match55(const Token* tok) {
    if (!tok || !(tok->isOp() || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ( ::| %name%
static bool match56(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %varid% ?
static bool match57(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("?")))
        return false;
    return true;
}
// pattern: %var% =
static bool match58(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: close|pclose|fclose|closedir ( %varid% )
static bool match59(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("close")) || (tok->str()==MatchCompiler::makeConstString("pclose")) || (tok->str()==MatchCompiler::makeConstString("fclose")) || (tok->str()==MatchCompiler::makeConstString("closedir"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %varid% !=|>=
static bool match60(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString(">="))))
        return false;
    return true;
}
// pattern: ! %varid%
static bool match61(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: (|, &| %varid% ,|)
static bool match62(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: switch (
static bool match63(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("switch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: case %any% :
static bool match64(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: while (
static bool match65(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: for ( ; ; )
static bool match66(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( 0 <= %varid% )
static bool match67(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( %varid% >= 0 )
static bool match68(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( %varid% != -1 )
static bool match69(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( -1 != %varid% )
static bool match70(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( %varid% == -1 )
static bool match71(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( -1 == %varid% )
static bool match72(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( %varid% < 0 )
static bool match73(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( 0 > %varid% )
static bool match74(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: while ( %varid% )
static bool match75(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: std :: auto_ptr <
static bool match76(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("auto_ptr")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > ( %varid% )
static bool match77(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: return strcpy|strncpy|memcpy ( %varid%
static bool match78(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("strcpy")) || (tok->str()==MatchCompiler::makeConstString("strncpy")) || (tok->str()==MatchCompiler::makeConstString("memcpy"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: &|(
static bool match79(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: try|throw|catch
static bool match80(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("try")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("catch"))))
        return false;
    return true;
}
// pattern: = {
static bool match81(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: & %name% = %varid% ;
static bool match82(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [)=] %varid% [+;)]
static bool match83(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr(")=", tok->str()[0]))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("+;)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% + %varid%
static bool match84(const Token* tok, const unsigned int varid) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: << %varid% ;
static bool match85(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<<")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: = strcpy|strcat|memmove|memcpy ( %varid% ,
static bool match86(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("strcpy")) || (tok->str()==MatchCompiler::makeConstString("strcat")) || (tok->str()==MatchCompiler::makeConstString("memmove")) || (tok->str()==MatchCompiler::makeConstString("memcpy"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: [;{}] %name% [ %varid% ]
static bool match87(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: ;|{|}|=|(|,|%cop% %varid% .|[
static bool match88(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(",")) || tok->isConstOp()))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: ) {
static bool match89(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: if|for|while|switch
static bool match90(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("switch"))))
        return false;
    return true;
}
// pattern: setjmp|longjmp
static bool match91(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("setjmp")) || (tok->str()==MatchCompiler::makeConstString("longjmp"))))
        return false;
    return true;
}
// pattern: %name% ( close|fclose|pclose ( %varid% ) ) ;
static bool match92(const Token* tok, const unsigned int varid) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("close")) || (tok->str()==MatchCompiler::makeConstString("fclose")) || (tok->str()==MatchCompiler::makeConstString("pclose"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ( *| %name%
static bool match93(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ) (
static bool match94(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: . %name%
static bool match95(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [;{]
static bool match96(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{", tok->str()[0]))
        return false;
    return true;
}
// pattern: [=(,] & (| %varid% [.[,)]
static bool match97(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr("=(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(".[,)", tok->str()[0]))
        return false;
    return true;
}
// pattern: callfunc alloc ;
static bool match98(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("callfunc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: assign|callfunc|use assign|callfunc|use|}
static bool match99(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("assign")) || (tok->str()==MatchCompiler::makeConstString("callfunc")) || (tok->str()==MatchCompiler::makeConstString("use"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("assign")) || (tok->str()==MatchCompiler::makeConstString("callfunc")) || (tok->str()==MatchCompiler::makeConstString("use")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    return true;
}
// pattern: ; {
static bool match100(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: [;{}] } %any%
static bool match101(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    return true;
}
// pattern: [;{}] callfunc ; }
static bool match102(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("callfunc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: ;| if return ;
static bool match103(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString(";"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: while1 {
static bool match104(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: [{};] if|ifv|else { continue ; }
static bool match105(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("ifv")) || (tok->str()==MatchCompiler::makeConstString("else"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("continue")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: while1 { if { dealloc ; return ; } }
static bool match106(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: [;{}] ;
static bool match107(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: { }
static bool match108(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: { %name% ; }
static bool match109(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: { %name% %name% ; }
static bool match110(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: if if|callfunc
static bool match111(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("callfunc"))))
        return false;
    return true;
}
// pattern: if|if(var) { if return use ; }
static bool match112(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("if(var)"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: if ; !!else
static bool match113(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("else"))
        return false;
    return true;
}
// pattern: if %name% ; else %name% ;
static bool match114(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if continue ; if continue ;
static bool match115(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("continue")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("continue")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] if return ; alloc|return ;
static bool match116(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("alloc")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] if alloc ; else return ;
static bool match117(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if ; else %name% ;
static bool match118(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if ; else
static bool match119(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    return true;
}
// pattern: if return ; else|if return|continue|break ;
static bool match120(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("else")) || (tok->str()==MatchCompiler::makeConstString("if"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if continue|break ; if|else return ;
static bool match121(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("else"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if continue|break|return ; else
static bool match122(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    return true;
}
// pattern: [;{}] if { dealloc|assign|use ; return ; }
static bool match123(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("dealloc")) || (tok->str()==MatchCompiler::makeConstString("assign")) || (tok->str()==MatchCompiler::makeConstString("use"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: if {| alloc ;
template<class T> static T * findmatch124(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        continue;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("{"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: if { dealloc|assign ; callfunc ; }
static bool match125(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("dealloc")) || (tok->str()==MatchCompiler::makeConstString("assign"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("callfunc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: else|return
static bool match126(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("else")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    return true;
}
// pattern: [;{}] alloc ; while(!var) alloc ;
static bool match127(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while(!var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ifv return ;
static bool match128(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("ifv")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] if(var) assign|dealloc|use ; !!else
static bool match129(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if(var)")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("assign")) || (tok->str()==MatchCompiler::makeConstString("dealloc")) || (tok->str()==MatchCompiler::makeConstString("use"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("else"))
        return false;
    return true;
}
// pattern: ; if(!var) alloc ; !!else
static bool match130(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if(!var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("else"))
        return false;
    return true;
}
// pattern: ; if(!var) exit ;
static bool match131(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if(!var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("exit")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if(var)|if(!var)|ifv ;
static bool match132(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if(var)")) || (tok->str()==MatchCompiler::makeConstString("if(!var)")) || (tok->str()==MatchCompiler::makeConstString("ifv"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: else ;
static bool match133(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: while1 if| continue| ;
static bool match134(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while1")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("if"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("continue"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: while1 if break ;
static bool match135(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("break")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: alloc ; if return use ; !!else
static bool match136(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("else"))
        return false;
    return true;
}
// pattern: [;{}] alloc|dealloc|use|callfunc ; exit ;
static bool match137(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("alloc")) || (tok->str()==MatchCompiler::makeConstString("dealloc")) || (tok->str()==MatchCompiler::makeConstString("use")) || (tok->str()==MatchCompiler::makeConstString("callfunc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("exit")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: alloc|dealloc|use ; if(var) exit ;
static bool match138(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("alloc")) || (tok->str()==MatchCompiler::makeConstString("dealloc")) || (tok->str()==MatchCompiler::makeConstString("use"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if(var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("exit")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if exit ;
static bool match139(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("exit")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: dealloc ; alloc ; if break|continue ;
static bool match140(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] if break ; break ;
static bool match141(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("break")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("break")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: do { dealloc ; alloc ; } while(var) ;
static bool match142(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("do")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while(var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if(!var) { alloc ; dealloc ; }
static bool match143(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if(!var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: do { alloc ; }
static bool match144(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("do")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: loop break|continue ;
static bool match145(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: loop|do ;
static bool match146(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("loop")) || (tok->str()==MatchCompiler::makeConstString("do"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: loop if break|continue ; !!else
static bool match147(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("else"))
        return false;
    return true;
}
// pattern: loop { if break|continue ; !!else
static bool match148(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("else"))
        return false;
    return true;
}
// pattern: ; loop ;
static bool match149(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: loop loop
static bool match150(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    return true;
}
// pattern: loop if return
static bool match151(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    return true;
}
// pattern: loop|while1 { dealloc ; alloc ; }
static bool match152(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("loop")) || (tok->str()==MatchCompiler::makeConstString("while1"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: loop { use ; callfunc ; }
static bool match153(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("callfunc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; if(!var) return ;
static bool match154(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if(!var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] return use ; %name%
static bool match155(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: if(var) return use ; !!else
static bool match156(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if(var)")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("else"))
        return false;
    return true;
}
// pattern: [;{}] alloc ; dealloc ; alloc ;
static bool match157(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] use ; dealloc ;
static bool match158(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: use use
static bool match159(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    return true;
}
// pattern: use use_
static bool match160(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use_")))
        return false;
    return true;
}
// pattern: use_ use
static bool match161(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use_")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    return true;
}
// pattern: use & use
static bool match162(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    return true;
}
// pattern: & use use
static bool match163(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    return true;
}
// pattern: [;{}] use ; if| use ;
static bool match164(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("if"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] use ; return use ;
static bool match165(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: try ; catch exit ;
static bool match166(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("try")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("catch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("exit")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: case ; case ;
static bool match167(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: switch {
static bool match168(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("switch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: return !!;
static bool match169(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(";"))
        return false;
    return true;
}
// pattern: if return|break use| ;
static bool match170(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("use"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: case|default
static bool match171(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("default"))))
        return false;
    return true;
}
// pattern: break|return ;
static bool match172(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: break ;
static bool match173(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("break")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: alloc ; if|if(var)|ifv break|continue|return ;
static bool match174(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("if(var)")) || (tok->str()==MatchCompiler::makeConstString("ifv"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: loop alloc ;
template<class T> static T * findmatch175(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: alloc ; if|if(var)|ifv return ;
template<class T> static T * findmatch176(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("if(var)")) || (tok->str()==MatchCompiler::makeConstString("ifv"))))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: alloc ; alloc|assign|return callfunc| ;
template<class T> static T * findmatch177(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("alloc")) || (tok->str()==MatchCompiler::makeConstString("assign")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        continue;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("callfunc"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: alloc ; loop|while1 {| alloc ;
template<class T> static T * findmatch178(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("loop")) || (tok->str()==MatchCompiler::makeConstString("while1"))))
        continue;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("{"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; alloc ; if assign ;
template<class T> static T * findmatch179(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("assign")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; alloc ; if dealloc ; }
template<class T> static T * findmatch180(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; alloc ; if dealloc ; return ;
template<class T> static T * findmatch181(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: alloc ; }
template<class T> static T * findmatch182(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: dealloc|use
template<class T> static T * findmatch183(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("dealloc")) || (tok->str()==MatchCompiler::makeConstString("use"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: exit|callfunc ; }
static bool match184(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("exit")) || (tok->str()==MatchCompiler::makeConstString("callfunc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: [;{}] dealloc ; use_ ;
template<class T> static T * findmatch185(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use_")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: loop use_ {
static bool match186(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("loop")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use_")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: alloc
template<class T> static T * findmatch187(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: goto
template<class T> static T * findmatch188(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("goto")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: dealloc ; dealloc ;
template<class T> static T * findmatch189(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: alloc ; }
static bool match190(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; dealloc ; }
static bool match191(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; return use ; }
static bool match192(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; use ; }
static bool match193(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; use ; return ; }
static bool match194(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; dealloc ; return ; }
static bool match195(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: if alloc ; dealloc ; }
static bool match196(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: if alloc ; return use ; }
static bool match197(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: if alloc ; use ; }
static bool match198(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("use")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; ifv return ; dealloc ; }
static bool match199(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("ifv")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: alloc ; if return ; dealloc; }
static bool match200(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("alloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("dealloc;")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: %name% = realloc|g_try_realloc ( %name% ,
static bool match201(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("realloc")) || (tok->str()==MatchCompiler::makeConstString("g_try_realloc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: %name% = %varid% ;
template<class T> static T * findmatch202(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !tok->isName())
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        continue;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: [{};] %varid% = %name% [;=]
template<class T> static T * findmatch203(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        continue;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        continue;
    tok = tok->next();
    if (!tok || !tok->isName())
        continue;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";=", tok->str()[0]))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; if (
static bool match204(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: * %name% = realloc|g_try_realloc ( * %name% ,
static bool match205(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("realloc")) || (tok->str()==MatchCompiler::makeConstString("g_try_realloc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: %name% = * %varid% ;
template<class T> static T * findmatch206(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !tok->isName())
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        continue;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: [{};] * %varid% = %name% [;=]
template<class T> static T * findmatch207(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        continue;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        continue;
    tok = tok->next();
    if (!tok || !tok->isName())
        continue;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";=", tok->str()[0]))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; if ( ! * %varid% ) {
static bool match208(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ) ;
static bool match209(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: {|}|; %name% (
static bool match210(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: :|, %varid% (
static bool match211(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [{};] %name% (
static bool match212(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %type% :: %varid% =
static bool match213(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: = %varid% [;=]
static bool match214(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";=", tok->str()[0]))
        return false;
    return true;
}
// pattern: %varid% = malloc|kmalloc (
static bool match215(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("malloc")) || (tok->str()==MatchCompiler::makeConstString("kmalloc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [;{}] %varid% . %var% =
static bool match216(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: free|kfree ( %varid% )
static bool match217(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("free")) || (tok->str()==MatchCompiler::makeConstString("kfree"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: free|kfree ( %var% . %varid% )
static bool match218(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("free")) || (tok->str()==MatchCompiler::makeConstString("kfree"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: return %varid% ;
static bool match219(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: return & %varid%
static bool match220(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: return %varid% . %var%
static bool match221(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: = %var% . %varid% ;
static bool match222(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [(,] &| %varid% [,)]
static bool match223(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] &| %varid% . %name% [,)]
static bool match224(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] %name% (
static bool match225(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) [,)]
static bool match226(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: !!= %name% (
static bool match227(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("="))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: }|;
template<class T> static T * findmatch228(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %name%
static bool match229(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %comp%|!
static bool match230(const Token* tok) {
    if (!tok || !(tok->isComparisonOp() || (tok->str()==MatchCompiler::makeConstString("!"))))
        return false;
    return true;
}
// pattern: shared_ptr|unique_ptr <
static bool match231(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("shared_ptr")) || (tok->str()==MatchCompiler::makeConstString("unique_ptr"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > ( new %name%
static bool match232(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: > (
static bool match233(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2018 Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "checkmemoryleak.h"

#include "astutils.h"
#include "library.h"
#include "mathlib.h"
#include "settings.h"
#include "standards.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "tokenlist.h"
#include "utils.h"
#include "valueflow.h"

#include <algorithm>
#include <cstddef>
#include <set>
#include <stack>

//---------------------------------------------------------------------------

// Register this check class (by creating a static instance of it)
namespace {
    CheckMemoryLeakInFunction instance1;
    CheckMemoryLeakInClass instance2;
    CheckMemoryLeakStructMember instance3;
    CheckMemoryLeakNoVar instance4;
}

// CWE ID used:
static const CWE CWE398(398U);  // Indicator of Poor Code Quality
static const CWE CWE401(401U);  // Improper Release of Memory Before Removing Last Reference ('Memory Leak')
static const CWE CWE771(771U);  // Missing Reference to Active Allocated Resource
static const CWE CWE772(772U);  // Missing Release of Resource after Effective Lifetime

/**
 * Count function parameters
 * \param tok Function name token before the '('
 */
static unsigned int countParameters(const Token *tok)
{
    tok = tok->tokAt(2);
    if (tok->str() == MatchCompiler::makeConstString(")"))
        return 0;

    unsigned int numpar = 1;
    while (nullptr != (tok = tok->nextArgument()))
        numpar++;

    return numpar;
}


/** List of functions that can be ignored when searching for memory leaks.
 * These functions don't take the address of the given pointer
 * This list contains function names with const parameters e.g.: atof(const char *)
 * TODO: This list should be replaced by <leak-ignore/> in .cfg files.
 */
static const std::set<std::string> call_func_white_list = {
    "_open", "_wopen", "access", "adjtime", "asctime_r", "asprintf", "chdir", "chmod", "chown"
    , "creat", "ctime_r", "execl", "execle", "execlp", "execv", "execve", "fchmod", "fcntl"
    , "fdatasync", "fclose", "flock", "fmemopen", "fnmatch", "fopen", "fopencookie", "for", "free"
    , "freopen", "fseeko", "fstat", "fsync", "ftello", "ftruncate", "getgrnam", "gethostbyaddr", "gethostbyname"
    , "getnetbyname", "getopt", "getopt_long", "getprotobyname", "getpwnam", "getservbyname", "getservbyport"
    , "glob", "gmtime", "gmtime_r", "if", "index", "inet_addr", "inet_aton", "inet_network", "initgroups"
    , "ioctl", "link", "localtime_r", "lockf", "lseek", "lstat", "mkdir", "mkfifo", "mknod", "mkstemp"
    , "obstack_printf", "obstack_vprintf", "open", "opendir", "parse_printf_format", "pathconf"
    , "perror", "popen", "posix_fadvise", "posix_fallocate", "pread", "psignal", "pwrite", "read", "readahead"
    , "readdir", "readdir_r", "readlink", "readv", "realloc", "regcomp", "return", "rewinddir", "rindex"
    , "rmdir", "scandir", "seekdir", "setbuffer", "sethostname", "setlinebuf", "sizeof", "strdup"
    , "stat", "stpcpy", "strcasecmp", "stricmp", "strncasecmp", "switch"
    , "symlink", "sync_file_range", "telldir", "tempnam", "time", "typeid", "unlink"
    , "utime", "utimes", "vasprintf", "while", "wordexp", "write", "writev"
};

//---------------------------------------------------------------------------

bool CheckMemoryLeak::isclass(const Token *tok, unsigned int varid) const
{
    if (tok->isStandardType())
        return false;

    const Variable * var = mTokenizer_->getSymbolDatabase()->getVariableFromVarId(varid);

    // return false if the type is a simple record type without side effects
    // a type that has no side effects (no constructors and no members with constructors)
    /** @todo false negative: check base class for side effects */
    /** @todo false negative: check constructors for side effects */
    if (var && var->typeScope() && var->typeScope()->numConstructors == 0 &&
        (var->typeScope()->varlist.empty() || var->type()->needInitialization == Type::True) &&
        var->type()->derivedFrom.empty())
        return false;

    return true;
}
//---------------------------------------------------------------------------

CheckMemoryLeak::AllocType CheckMemoryLeak::getAllocationType(const Token *tok2, unsigned int varid, std::list<const Function*> *callstack) const
{
    // What we may have...
    //     * var = (char *)malloc(10);
    //     * var = new char[10];
    //     * var = strdup("hello");
    //     * var = strndup("hello", 3);
    if (tok2 && tok2->str() == MatchCompiler::makeConstString("(")) {
        tok2 = tok2->link();
        tok2 = tok2 ? tok2->next() : nullptr;
    }
    if (! tok2)
        return No;
    if (tok2->str() == MatchCompiler::makeConstString("::"))
        tok2 = tok2->next();
    if (! tok2->isName())
        return No;

    if (!match1(tok2)) {
        // Using realloc..
        if (varid && match2(tok2) && tok2->tokAt(2)->varId() != varid)
            return Malloc;

        if (mTokenizer_->isCPP() && tok2->str() == MatchCompiler::makeConstString("new")) {
            if (tok2->strAt(1) == MatchCompiler::makeConstString("(") && !match3(tok2->next()))
                return No;
            if (tok2->astOperand1() && (tok2->astOperand1()->str() == MatchCompiler::makeConstString("[") || (tok2->astOperand1()->astOperand1() && tok2->astOperand1()->astOperand1()->str() == MatchCompiler::makeConstString("["))))
                return NewArray;
            return New;
        }

        if (mSettings_->standards.posix) {
            if (match4(tok2)) {
                // simple sanity check of function parameters..
                // TODO: Make such check for all these functions
                const unsigned int num = countParameters(tok2);
                if (tok2->str() == MatchCompiler::makeConstString("open") && num != 2 && num != 3)
                    return No;

                // is there a user function with this name?
                if (tok2->function())
                    return No;
                return Fd;
            }

            if (match5(tok2))
                return Pipe;
        }

        // Does tok2 point on a Library allocation function?
        const int alloctype = mSettings_->library.alloc(tok2, -1);
        if (alloctype > 0) {
            if (alloctype == mSettings_->library.deallocId("free"))
                return Malloc;
            if (alloctype == mSettings_->library.deallocId("fclose"))
                return File;
            return Library::ismemory(alloctype) ? OtherMem : OtherRes;
        }
    }

    while (match1(tok2))
        tok2 = tok2->tokAt(2);

    // User function
    const Function* func = tok2->function();
    if (func == nullptr)
        return No;

    // Prevent recursion
    if (callstack && std::find(callstack->begin(), callstack->end(), func) != callstack->end())
        return No;

    std::list<const Function*> cs;
    if (!callstack)
        callstack = &cs;

    callstack->push_back(func);
    return functionReturnType(func, callstack);
}


CheckMemoryLeak::AllocType CheckMemoryLeak::getReallocationType(const Token *tok2, unsigned int varid)
{
    // What we may have...
    //     * var = (char *)realloc(..;
    if (tok2 && tok2->str() == MatchCompiler::makeConstString("(")) {
        tok2 = tok2->link();
        tok2 = tok2 ? tok2->next() : nullptr;
    }
    if (! tok2)
        return No;

    if (varid > 0 && ! match6(tok2, varid))
        return No;

    if (tok2->str() == MatchCompiler::makeConstString("realloc"))
        return Malloc;

    return No;
}


CheckMemoryLeak::AllocType CheckMemoryLeak::getDeallocationType(const Token *tok, unsigned int varid) const
{
    if (mTokenizer_->isCPP() && tok->str() == MatchCompiler::makeConstString("delete") && tok->astOperand1()) {
        const Token* vartok = tok->astOperand1();
        if (match7(vartok))
            vartok = vartok->astOperand2();

        if (vartok && vartok->varId() == varid) {
            if (tok->strAt(1) == MatchCompiler::makeConstString("["))
                return NewArray;
            return New;
        }
    }

    if (tok->str() == MatchCompiler::makeConstString("::"))
        tok = tok->next();

    if (match8(tok)) {
        if (match9(tok))
            return File;

        int argNr = 1;
        for (const Token* tok2 = tok->tokAt(2); tok2; tok2 = tok2->nextArgument()) {
            const Token* vartok = tok2;
            while (match10(vartok))
                vartok = vartok->tokAt(2);

            if (match11(vartok, varid)) {
                if (tok->str() == MatchCompiler::makeConstString("realloc") && match12(vartok->next()))
                    return Malloc;

                if (mSettings_->standards.posix) {
                    if (tok->str() == MatchCompiler::makeConstString("close"))
                        return Fd;
                    if (tok->str() == MatchCompiler::makeConstString("pclose"))
                        return Pipe;
                }

                // Does tok point on a Library deallocation function?
                const int dealloctype = mSettings_->library.dealloc(tok, argNr);
                if (dealloctype > 0) {
                    if (dealloctype == mSettings_->library.deallocId("free"))
                        return Malloc;
                    if (dealloctype == mSettings_->library.deallocId("fclose"))
                        return File;
                    return Library::ismemory(dealloctype) ? OtherMem : OtherRes;
                }
            }
            argNr++;
        }
    }

    return No;
}

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------

void CheckMemoryLeak::memoryLeak(const Token *tok, const std::string &varname, AllocType alloctype) const
{
    if (alloctype == CheckMemoryLeak::File ||
        alloctype == CheckMemoryLeak::Pipe ||
        alloctype == CheckMemoryLeak::Fd   ||
        alloctype == CheckMemoryLeak::OtherRes)
        resourceLeakError(tok, varname);
    else
        memleakError(tok, varname);
}
//---------------------------------------------------------------------------

void CheckMemoryLeak::reportErr(const Token *tok, Severity::SeverityType severity, const std::string &id, const std::string &msg, const CWE &cwe) const
{
    std::list<const Token *> callstack;

    if (tok)
        callstack.push_back(tok);

    reportErr(callstack, severity, id, msg, cwe);
}

void CheckMemoryLeak::reportErr(const std::list<const Token *> &callstack, Severity::SeverityType severity, const std::string &id, const std::string &msg, const CWE &cwe) const
{
    const ErrorLogger::ErrorMessage errmsg(callstack, mTokenizer_ ? &mTokenizer_->list : nullptr, severity, id, msg, cwe, false);
    if (mErrorLogger_)
        mErrorLogger_->reportErr(errmsg);
    else
        Check::reportError(errmsg);
}

void CheckMemoryLeak::memleakError(const Token *tok, const std::string &varname) const
{
    reportErr(tok, Severity::error, "memleak", "$symbol:" + varname + "\nMemory leak: $symbol", CWE(401U));
}

void CheckMemoryLeak::memleakUponReallocFailureError(const Token *tok, const std::string &varname) const
{
    reportErr(tok, Severity::error, "memleakOnRealloc", "$symbol:" + varname + "\nCommon realloc mistake: \'$symbol\' nulled but not freed upon failure", CWE(401U));
}

void CheckMemoryLeak::resourceLeakError(const Token *tok, const std::string &varname) const
{
    std::string errmsg("Resource leak");
    if (!varname.empty())
        errmsg = "$symbol:" + varname + '\n' + errmsg + ": $symbol";
    reportErr(tok, Severity::error, "resourceLeak", errmsg, CWE(775U));
}

void CheckMemoryLeak::deallocDeallocError(const Token *tok, const std::string &varname) const
{
    reportErr(tok, Severity::error, "deallocDealloc", "$symbol:" + varname + "\nDeallocating a deallocated pointer: $symbol", CWE(415U));
}

void CheckMemoryLeak::deallocuseError(const Token *tok, const std::string &varname) const
{
    reportErr(tok, Severity::error, "deallocuse", "$symbol:" + varname + "\nDereferencing '$symbol' after it is deallocated / released", CWE(416U));
}

void CheckMemoryLeak::mismatchSizeError(const Token *tok, const std::string &sz) const
{
    reportErr(tok, Severity::error, "mismatchSize", "The allocated size " + sz + " is not a multiple of the underlying type's size.", CWE(131U));
}

void CheckMemoryLeak::mismatchAllocDealloc(const std::list<const Token *> &callstack, const std::string &varname) const
{
    reportErr(callstack, Severity::error, "mismatchAllocDealloc", "$symbol:" + varname + "\nMismatching allocation and deallocation: $symbol", CWE(762U));
}

CheckMemoryLeak::AllocType CheckMemoryLeak::functionReturnType(const Function* func, std::list<const Function*> *callstack) const
{
    if (!func || !func->hasBody())
        return No;

    // Get return pointer..
    unsigned int varid = 0;
    for (const Token *tok2 = func->functionScope->bodyStart; tok2 != func->functionScope->bodyEnd; tok2 = tok2->next()) {
        if (tok2->str() == MatchCompiler::makeConstString("return")) {
            const AllocType allocType = getAllocationType(tok2->next(), 0, callstack);
            if (allocType != No)
                return allocType;

            if (tok2->scope() != func->functionScope || !tok2->astOperand1())
                return No;
            const Token* tok = tok2->astOperand1();
            if (match7(tok))
                tok = tok->astOperand2() ? tok->astOperand2() : tok->astOperand1();
            if (tok)
                varid = tok->varId();
            break;
        }
    }

    // Not returning pointer value..
    if (varid == 0)
        return No;

    // If variable is not local then alloctype shall be "No"
    // Todo: there can be false negatives about mismatching allocation/deallocation.
    //       => Generate "alloc ; use ;" if variable is not local?
    const Variable *var = mTokenizer_->getSymbolDatabase()->getVariableFromVarId(varid);
    if (!var || !var->isLocal() || var->isStatic())
        return No;

    // Check if return pointer is allocated..
    AllocType allocType = No;
    for (const Token* tok = func->functionScope->bodyStart; tok != func->functionScope->bodyEnd; tok = tok->next()) {
        if (match13(tok, varid)) {
            allocType = getAllocationType(tok->tokAt(2), varid, callstack);
        }
        if (match14(tok, varid)) {
            return No;
        }
        if (!mTokenizer_->isC() && match15(tok, varid)) {
            return No;
        }
        if (match16(tok, varid)) {
            return No;
        }
        if (match17(tok, varid)) {
            return No;
        }
        if (allocType == No && tok->str() == MatchCompiler::makeConstString("return"))
            return No;
    }

    return allocType;
}


const char *CheckMemoryLeak::functionArgAlloc(const Function *func, unsigned int targetpar, AllocType &allocType) const
{
    allocType = No;

    if (!func || !func->functionScope)
        return "";

    if (!match18(func->retDef))
        return "";

    std::list<Variable>::const_iterator arg = func->argumentList.begin();
    for (; arg != func->argumentList.end(); ++arg) {
        if (arg->index() == targetpar-1)
            break;
    }
    if (arg == func->argumentList.end())
        return "";

    // Is **
    if (!arg->isPointer())
        return "";
    const Token* tok = arg->typeEndToken();
    tok = tok->previous();
    if (tok->str() != MatchCompiler::makeConstString("*"))
        return "";

    // Check if pointer is allocated.
    bool realloc = false;
    for (tok = func->functionScope->bodyStart; tok && tok != func->functionScope->bodyEnd; tok = tok->next()) {
        if (tok->varId() == arg->declarationId()) {
            if (match19(tok->tokAt(-3))) {
                realloc = true;
                allocType = No;
            } else if (match20(tok->previous())) {
                allocType = getAllocationType(tok->tokAt(2), arg->declarationId());
                if (allocType == No) {
                    allocType = getReallocationType(tok->tokAt(2), arg->declarationId());
                }
                if (allocType != No) {
                    if (realloc)
                        return "realloc";
                    return "alloc";
                }
            } else {
                // unhandled variable usage: bailout
                return "";
            }
        }
    }

    return "";
}


static bool notvar(const Token *tok, unsigned int varid)
{
    if (!tok)
        return false;
    if (match21(tok))
        return notvar(tok->astOperand1(),varid) || notvar(tok->astOperand2(),varid);
    if (tok->str() == MatchCompiler::makeConstString("(") && match22(tok->astOperand1()))
        return notvar(tok->astOperand2(), varid);
    const Token *vartok = astIsVariableComparison(tok, "==", "0");
    return vartok && (vartok->varId() == varid);
}

static bool ifvar(const Token *tok, unsigned int varid, const std::string &comp, const std::string &rhs)
{
    if (!match23(tok))
        return false;
    const Token *condition = tok->next()->astOperand2();
    if (condition && condition->str() == MatchCompiler::makeConstString("(") && match22(condition->astOperand1()))
        condition = condition->astOperand2();
    if (!condition || condition->str() == MatchCompiler::makeConstString("&&"))
        return false;

    const Token *vartok = astIsVariableComparison(condition, comp, rhs);
    return (vartok && vartok->varId() == varid);
}

static bool alwaysTrue(const Token *tok)
{
    if (!tok)
        return false;
    if (tok->values().size() == 1U &&
        tok->values().front().isKnown() &&
        tok->values().front().intvalue != 0)
        return true;
    if (tok->str() == MatchCompiler::makeConstString("||"))
        return alwaysTrue(tok->astOperand1()) || alwaysTrue(tok->astOperand2());
    if (tok->str() == MatchCompiler::makeConstString("true"))
        return true;
    return false;
}

bool CheckMemoryLeakInFunction::test_white_list(const std::string &funcname, const Settings *settings, bool cpp)
{
    return ((call_func_white_list.find(funcname)!=call_func_white_list.end()) || settings->library.isLeakIgnore(funcname) || (cpp && funcname == MatchCompiler::makeConstString("delete")));
}

namespace {
    const std::set<std::string> call_func_keywords = {
        "asprintf"
        , "delete"
        , "fclose"
        , "for"
        , "free"
        , "if"
        , "realloc"
        , "return"
        , "switch"
        , "while"
        , "sizeof"
    };
}

const char * CheckMemoryLeakInFunction::call_func(const Token *tok, std::list<const Token *> callstack, const unsigned int varid, AllocType &alloctype, AllocType &dealloctype, bool &allocpar, unsigned int sz)
{
    if (test_white_list(tok->str(), mSettings, mTokenizer->isCPP())) {
        if (call_func_keywords.find(tok->str())!=call_func_keywords.end())
            return nullptr;

        // is the varid a parameter?
        for (const Token *tok2 = tok->tokAt(2); tok2 && tok2 != tok->linkAt(1); tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("(")) {
                tok2 = tok2->nextArgument();
                if (!tok2)
                    break;
            }
            if (tok2->varId() == varid) {
                if (tok->strAt(-1) == MatchCompiler::makeConstString("."))
                    return "use";
                else if (tok2->strAt(1) == MatchCompiler::makeConstString("="))
                    return "assign";
                else if (tok->str()==MatchCompiler::makeConstString("printf"))
                    return "use"; // <- it is not certain printf dereference the pointer TODO: check the format string
                else
                    return "use_";
            }
        }

        return nullptr;
    }

    if (mSettings->library.isnoreturn(tok) && tok->strAt(-1) != MatchCompiler::makeConstString("="))
        return "exit";

    if (varid > 0 && (getReallocationType(tok, varid) != No || getDeallocationType(tok, varid) != No))
        return nullptr;

    if (callstack.size() > 2)
        return "dealloc_";

    const std::string& funcname(tok->str());
    for (const Token *tok2 : callstack) {
        if (tok2 && tok2->str() == funcname)
            return "recursive";
    }
    callstack.push_back(tok);

    // lock/unlock..
    if (varid == 0) {
        const Function* func = tok->function();
        if (!func || !func->hasBody())
            return nullptr;

        Token *ftok = getcode(func->functionScope->bodyStart->next(), callstack, 0, alloctype, dealloctype, false, 1);
        simplifycode(ftok);
        const char *ret = nullptr;
        if (match24(ftok))
            ret = "alloc";
        else if (match25(ftok))
            ret = "dealloc";
        TokenList::deleteTokens(ftok);
        return ret;
    }

    // how many parameters is there in the function call?
    const unsigned int numpar = countParameters(tok);
    if (numpar == 0) {
        // Taking return value => it is not a noreturn function
        if (tok->strAt(-1) == MatchCompiler::makeConstString("="))
            return nullptr;

        // Function is not noreturn
        if (tok->function() && tok->function()->functionScope) {
            std::string temp;
            if (!mSettings->library.isScopeNoReturn(tok->function()->functionScope->bodyEnd, &temp) && temp.empty())
                return nullptr;
        } else if (mSettings->library.isnotnoreturn(tok))
            return nullptr;

        return "callfunc";
    }

    unsigned int par = 0;

    const bool dot(tok->previous()->str() == MatchCompiler::makeConstString("."));
    const bool eq(tok->previous()->str() == MatchCompiler::makeConstString("="));

    const Token *functok = tok;

    tok = findmatch26(tok) ;
    if (tok)
        tok = tok->next();

    for (; tok; tok = tok->nextArgument()) {
        ++par;
        if (match27(tok, varid)) {
            if (dot)
                return "use";

            const Function* function = functok->function();
            if (!function)
                return "use";

            // how many parameters does the function want?
            if (numpar != function->argCount()) // TODO: Handle default parameters
                return "recursive";

            if (!function->functionScope)
                return "use";
            const Variable* param = function->getArgumentVar(par-1);
            if (!param || !param->nameToken())
                return "use";
            Token *func = getcode(function->functionScope->bodyStart->next(), callstack, param->declarationId(), alloctype, dealloctype, false, sz);
            //simplifycode(func);
            const Token *func_ = func;
            while (func_ && func_->str() == MatchCompiler::makeConstString(";"))
                func_ = func_->next();

            const char *ret = nullptr;
            /** @todo handle "goto" */
            if (findmatch28(func_) )
                ret = "dealloc";
            else if (findmatch29(func_) )
                ret = "use";
            else if (findmatch30(func_) )
                ret = "&use";

            TokenList::deleteTokens(func);
            return ret;
        }
        if (match31(tok, varid)) {
            const Function *func = functok->function();
            if (func == nullptr)
                continue;
            AllocType a;
            const char *ret = functionArgAlloc(func, par, a);

            if (a != No) {
                if (alloctype == No)
                    alloctype = a;
                else if (alloctype != a)
                    alloctype = Many;
                allocpar = true;
                return ret;
            }
        }
        if (match32(tok, varid))
            return "use";
    }
    return (eq || mSettings->experimental) ? nullptr : "callfunc";
}

template<typename T>
static void addtoken(Token **rettail, const Token *tok, T&& str)
{
    (*rettail)->insertToken(str);
    (*rettail) = (*rettail)->next();
    (*rettail)->linenr(tok->linenr());
    (*rettail)->fileIndex(tok->fileIndex());
}


Token *CheckMemoryLeakInFunction::getcode(const Token *tok, std::list<const Token *> callstack, const unsigned int varid, CheckMemoryLeak::AllocType &alloctype, CheckMemoryLeak::AllocType &dealloctype, bool classmember, unsigned int sz)
{
    // variables whose value depends on if(!var). If one of these variables
    // is used in a if-condition then generate "ifv" instead of "if".
    std::set<unsigned int> extravar;

    // The first token should be ";"
    Token* rethead = new Token();
    rethead->str(";");
    rethead->linenr(tok->linenr());
    rethead->fileIndex(tok->fileIndex());
    Token* rettail = rethead;

    int indentlevel = 0;
    int parlevel = 0;
    for (; tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("{")) {
            addtoken(&rettail, tok, "{");
            ++indentlevel;
        } else if (tok->str() == MatchCompiler::makeConstString("}")) {
            addtoken(&rettail, tok, "}");
            if (indentlevel <= 0)
                break;
            --indentlevel;
        }

        else if (tok->str() == MatchCompiler::makeConstString("("))
            ++parlevel;
        else if (tok->str() == MatchCompiler::makeConstString(")"))
            --parlevel;

        if (parlevel == 0 && tok->str() == MatchCompiler::makeConstString(";"))
            addtoken(&rettail, tok, ";");

        // Start of new statement.. check if the statement has anything interesting
        if (varid > 0 && parlevel == 0 && match33(tok)) {
            if (match34(tok->next()))
                continue;

            // function calls are interesting..
            const Token *tok2 = tok;
            if (match35(tok2))
                tok2 = tok2->next();
            while (match36(tok2->next()))
                tok2 = tok2->tokAt(2);
            if (match8(tok2->next()))
                ;

            else if (match37(tok->next()))
                ;

            else {
                const Token *skipToToken = nullptr;

                // scan statement for interesting keywords / varid
                for (tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString(";")) {
                        // nothing interesting found => skip this statement
                        skipToToken = tok2->previous();
                        break;
                    }

                    if (tok2->varId() == varid ||
                        tok2->str() == MatchCompiler::makeConstString(":") || tok2->str() == MatchCompiler::makeConstString("{") || tok2->str() == MatchCompiler::makeConstString("}")) {
                        break;
                    }
                }

                if (skipToToken) {
                    tok = skipToToken;
                    continue;
                }
            }
        }

        if (varid == 0) {
            if (!callstack.empty() && match38(tok)) {
                // Type of leak = Resource leak
                alloctype = dealloctype = CheckMemoryLeak::File;

                if (tok->next()->str() == MatchCompiler::makeConstString("__cppcheck_lock")) {
                    addtoken(&rettail, tok, "alloc");
                } else {
                    addtoken(&rettail, tok, "dealloc");
                }

                tok = tok->tokAt(3);
                continue;
            }

            if (match23(tok)) {
                addtoken(&rettail, tok, "if");
                tok = tok->next()->link();
                continue;
            }
        } else {

            if (match39(tok, varid)) {
                addtoken(&rettail, tok, "dealloc");
                addtoken(&rettail, tok, ";");
                addtoken(&rettail, tok, "assign");
                addtoken(&rettail, tok, ";");
                tok = tok->tokAt(5);
                continue;
            }

            // var = strcpy|.. ( var ,
            if (match40(tok, varid)) {
                tok = tok->linkAt(4);
                continue;
            }

            if (match41(tok->previous(), varid) ||
                match42(tok, varid)) {
                CheckMemoryLeak::AllocType alloc;

                if (match43(tok)) {
                    // todo: check how the return value is used.
                    if (!match33(tok->previous())) {
                        TokenList::deleteTokens(rethead);
                        return nullptr;
                    }
                    alloc = Malloc;
                    tok = tok->next()->link();
                } else {
                    alloc = getAllocationType(tok->tokAt(2), varid);
                }

                if (sz > 1 &&
                    match44(tok->tokAt(2)) &&
                    (MathLib::toLongNumber(tok->strAt(4)) % long(sz)) != 0) {
                    mismatchSizeError(tok->tokAt(4), tok->strAt(4));
                }

                if (alloc == CheckMemoryLeak::No) {
                    alloc = getReallocationType(tok->tokAt(2), varid);
                    if (alloc != CheckMemoryLeak::No) {
                        addtoken(&rettail, tok, "realloc");
                        addtoken(&rettail, tok, ";");
                        tok = tok->tokAt(2);
                        if (match8(tok))
                            tok = tok->next()->link();
                        continue;
                    }
                }

                // don't check classes..
                if (alloc == CheckMemoryLeak::New) {
                    if (match45(tok->tokAt(2))) {
                        const int offset = tok->strAt(3) == MatchCompiler::makeConstString("struct") ? 1 : 0;
                        if (isclass(tok->tokAt(3 + offset), varid)) {
                            alloc = No;
                        }
                    } else if (match46(tok->tokAt(2))) {
                        const int offset = tok->strAt(6) == MatchCompiler::makeConstString("struct") ? 1 : 0;
                        if (isclass(tok->tokAt(6 + offset), varid)) {
                            alloc = No;
                        }
                    } else if (match47(tok->tokAt(2))) {
                        const int offset = tok->strAt(8) == MatchCompiler::makeConstString("struct") ? 1 : 0;
                        if (isclass(tok->tokAt(8 + offset), varid)) {
                            alloc = No;
                        }
                    }

                    if (match48(tok->next())) {
                        tok = tok->tokAt(2);
                        while (match49(tok->next())) {
                            if (match50(tok->next()))
                                tok = tok->linkAt(1);
                            else
                                tok = tok->next();
                        }
                    }

                    if (alloc == No && alloctype == No)
                        alloctype = CheckMemoryLeak::New;
                }

                if (alloc != No) {
                    addtoken(&rettail, tok, "alloc");

                    if (alloctype != No && alloctype != alloc)
                        alloc = Many;

                    if (alloc != Many && dealloctype != No && dealloctype != Many && dealloctype != alloc) {
                        callstack.push_back(tok);
                        mismatchAllocDealloc(callstack, findmatch51(mTokenizer->tokens(), varid) ->str());
                        callstack.pop_back();
                    }

                    alloctype = alloc;

                    if (match52(tok)) {
                        tok = tok->linkAt(3);
                        continue;
                    }
                }

                // assignment..
                else {
                    // is the pointer in rhs?
                    bool rhs = false;
                    bool trailingSemicolon = false;
                    bool used = false;
                    for (const Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                        if (tok2->str() == MatchCompiler::makeConstString(";")) {
                            trailingSemicolon = true;
                            if (rhs)
                                tok = tok2;
                            break;
                        }

                        if (!used && !rhs) {
                            if (match53(tok2, varid)) {
                                if (match54(tok2)) {
                                    used = true;
                                    addtoken(&rettail, tok, "use");
                                    addtoken(&rettail, tok, ";");
                                }
                                rhs = true;
                            }
                        }
                    }

                    if (!used) {
                        if (!rhs)
                            addtoken(&rettail, tok, "assign");
                        else {
                            addtoken(&rettail, tok, "use_");
                            if (trailingSemicolon)
                                addtoken(&rettail, tok, ";");
                        }
                    }
                    continue;
                }
            }

            if (match55(tok->previous()) || (match56(tok->previous()) && (!rettail || rettail->str() != MatchCompiler::makeConstString("loop")))) {
                if (tok->str() == MatchCompiler::makeConstString("::"))
                    tok = tok->next();

                if (match57(tok, varid))
                    tok = tok->tokAt(2);

                AllocType dealloc = getDeallocationType(tok, varid);

                if (dealloc != No && tok->str() == MatchCompiler::makeConstString("fcloseall") && alloctype != dealloc)
                    ;

                else if (dealloc != No) {
                    addtoken(&rettail, tok, "dealloc");

                    if (dealloctype != No && dealloctype != dealloc)
                        dealloc = Many;

                    if (dealloc != Many && alloctype != No && alloctype != Many && alloctype != dealloc) {
                        callstack.push_back(tok);
                        mismatchAllocDealloc(callstack, findmatch51(mTokenizer->tokens(), varid) ->str());
                        callstack.pop_back();
                    }
                    dealloctype = dealloc;

                    if (tok->strAt(2) == MatchCompiler::makeConstString("("))
                        tok = tok->linkAt(2);
                    continue;
                }
            }

            // if else switch
            if (match23(tok)) {
                if (alloctype == Fd) {
                    if (ifvar(tok, varid, ">",  "-1") ||
                        ifvar(tok, varid, ">=", "0") ||
                        ifvar(tok, varid, ">",  "0") ||
                        ifvar(tok, varid, "!=", "-1")) {
                        addtoken(&rettail, tok, "if(var)");
                        tok = tok->next()->link();
                        continue;
                    } else if (ifvar(tok, varid, "==", "-1") ||
                               ifvar(tok, varid, "<", "0")) {
                        addtoken(&rettail, tok, "if(!var)");
                        tok = tok->next()->link();
                        continue;
                    }
                }

                if (ifvar(tok, varid, "!=", "0")) {
                    addtoken(&rettail, tok, "if(var)");

                    // Make sure the "use" will not be added
                    tok = tok->next()->link();
                    continue;
                } else if (ifvar(tok, varid, "==", "0")) {
                    addtoken(&rettail, tok, "if(!var)");

                    // parse the if-body.
                    // if a variable is assigned then add variable to "extravar".
                    for (const Token *tok2 = tok->next()->link()->tokAt(2); tok2; tok2 = tok2->next()) {
                        if (tok2->str() == MatchCompiler::makeConstString("{"))
                            tok2 = tok2->link();
                        else if (tok2->str() == MatchCompiler::makeConstString("}"))
                            break;
                        else if (match58(tok2))
                            extravar.insert(tok2->varId());
                    }

                    tok = tok->next()->link();
                    continue;
                } else {
                    // Check if the condition depends on var or extravar somehow..
                    bool dep = false;
                    const Token* const end = tok->linkAt(1);
                    for (const Token *tok2 = tok->next(); tok2 != end; tok2 = tok2->next()) {
                        if (match59(tok2, varid)) {
                            addtoken(&rettail, tok, "dealloc");
                            addtoken(&rettail, tok, ";");
                            dep = true;
                            break;
                        } else if (alloctype == Fd && match60(tok2, varid)) {
                            dep = true;
                        } else if (match61(tok2, varid)) {
                            dep = true;
                        } else if (match8(tok2) && !test_white_list(tok2->str(), mSettings, mTokenizer->isCPP())) {
                            bool use = false;
                            for (const Token *tok3 = tok2->tokAt(2); tok3; tok3 = tok3->nextArgument()) {
                                if (match62(tok3->previous(), varid)) {
                                    use = true;
                                    break;
                                }
                            }
                            if (use) {
                                addtoken(&rettail, tok, "use");
                                addtoken(&rettail, tok, ";");
                                dep = false;
                                break;
                            }
                        } else if (tok2->varId() && extravar.find(tok2->varId()) != extravar.end()) {
                            dep = true;
                        } else if (tok2->varId() == varid &&
                                   (tok2->next()->isConstOp() || tok2->previous()->isConstOp()))
                            dep = true;
                    }

                    if (notvar(tok->next()->astOperand2(), varid))
                        addtoken(&rettail, tok, "if(!var)");
                    else
                        addtoken(&rettail, tok, (dep ? "ifv" : "if"));

                    tok = tok->next()->link();
                    continue;
                }
            }
        }

        if ((tok->str() == MatchCompiler::makeConstString("else")) || (tok->str() == MatchCompiler::makeConstString("switch"))) {
            addtoken(&rettail, tok, tok->str());
            if (match63(tok))
                tok = tok->next()->link();
            continue;
        }

        if ((tok->str() == MatchCompiler::makeConstString("case"))) {
            addtoken(&rettail, tok, "case");
            addtoken(&rettail, tok, ";");
            if (match64(tok))
                tok = tok->tokAt(2);
            continue;
        }

        if ((tok->str() == MatchCompiler::makeConstString("default"))) {
            addtoken(&rettail, tok, "default");
            addtoken(&rettail, tok, ";");
            continue;
        }

        // Loops..
        else if ((tok->str() == MatchCompiler::makeConstString("for")) || (tok->str() == MatchCompiler::makeConstString("while"))) {
            const Token* const end = tok->linkAt(1);

            if ((match65(tok) && alwaysTrue(tok->next()->astOperand2())) ||
                match66(tok)) {
                addtoken(&rettail, tok, "while1");
                tok = end;
                continue;
            }

            else if (varid && getDeallocationType(tok->tokAt(2), varid) != No) {
                addtoken(&rettail, tok, "dealloc");
                addtoken(&rettail, tok, ";");
            }

            else if (alloctype == Fd && varid) {
                if (match67(tok, varid) ||
                    match68(tok, varid) ||
                    match69(tok, varid) ||
                    match70(tok, varid)) {
                    addtoken(&rettail, tok, "while(var)");
                    tok = end;
                    continue;
                } else if (match71(tok, varid) ||
                           match72(tok, varid) ||
                           match73(tok, varid) ||
                           match74(tok, varid)) {
                    addtoken(&rettail, tok, "while(!var)");
                    tok = end;
                    continue;
                }
            }

            else if (varid && match75(tok, varid)) {
                addtoken(&rettail, tok, "while(var)");
                tok = end;
                continue;
            } else if (varid && match65(tok) && notvar(tok->next()->astOperand2(), varid)) {
                addtoken(&rettail, tok, "while(!var)");
                tok = end;
                continue;
            }

            addtoken(&rettail, tok, "loop");

            if (varid > 0 && notvar(tok->next()->astOperand2(), varid))
                addtoken(&rettail, tok, "!var");

            continue;
        }
        if ((tok->str() == MatchCompiler::makeConstString("do"))) {
            addtoken(&rettail, tok, "do");
            continue;
        }

        // continue / break..
        else if (tok->str() == MatchCompiler::makeConstString("continue")) {
            addtoken(&rettail, tok, "continue");
        } else if (tok->str() == MatchCompiler::makeConstString("break")) {
            addtoken(&rettail, tok, "break");
        } else if (tok->str() == MatchCompiler::makeConstString("goto")) {
            addtoken(&rettail, tok, "goto");
        }

        // Return..
        else if (tok->str() == MatchCompiler::makeConstString("return")) {
            addtoken(&rettail, tok, "return");
            if (varid == 0) {
                addtoken(&rettail, tok, ";");
                while (tok && tok->str() != MatchCompiler::makeConstString(";"))
                    tok = tok->next();
                if (!tok)
                    break;
                continue;
            }

            // Returning a auto_ptr of this allocated variable..
            if (match76(tok->next())) {
                const Token *tok2 = tok->linkAt(4);
                if (match77(tok2, varid)) {
                    addtoken(&rettail, tok, "use");
                    tok = tok2->tokAt(3);
                }
            }

            else if (varid && match78(tok, varid)) {
                addtoken(&rettail, tok, "use");
                tok = tok->tokAt(2);
            }

            else {
                bool use = false;

                std::stack<const Token *> functions;

                for (const Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString(";")) {
                        tok = tok2;
                        break;
                    }

                    if (tok2->str() == MatchCompiler::makeConstString("("))
                        functions.push(tok2->previous());
                    else if (!functions.empty() && tok2->str() == MatchCompiler::makeConstString(")"))
                        functions.pop();

                    if (tok2->varId() == varid) {
                        // Read data..
                        if (!match79(tok2->previous()) &&
                            tok2->strAt(1) == MatchCompiler::makeConstString("[")) {
                            ;
                        } else if (functions.empty() ||
                                   !test_white_list(functions.top()->str(), mSettings, mTokenizer->isCPP()) ||
                                   getDeallocationType(functions.top(),varid) != AllocType::No) {
                            use = true;
                        }
                    }
                }
                if (use)
                    addtoken(&rettail, tok, "use");
                addtoken(&rettail, tok, ";");
            }
        }

        // throw..
        else if (mTokenizer->isCPP() && match80(tok)) {
            addtoken(&rettail, tok, tok->str());
            if (tok->strAt(1) == MatchCompiler::makeConstString("("))
                tok = tok->next()->link();
        }

        // Assignment..
        if (varid) {
            if (match81(tok)) {
                const Token* const end2 = tok->linkAt(1);
                bool use = false;
                for (const Token *tok2 = tok; tok2 != end2; tok2 = tok2->next()) {
                    if (tok2->varId() == varid) {
                        use = true;
                        break;
                    }
                }

                if (use) {
                    addtoken(&rettail, tok, "use");
                    addtoken(&rettail, tok, ";");
                    tok = tok->next()->link();
                    continue;
                }
            }

            if (match82(tok, varid)) {
                while (rethead->next())
                    rethead->deleteNext();
                return rethead;
            }
            if (match83(tok, varid) ||
                (match84(tok, varid) &&
                 tok->strAt(3) != MatchCompiler::makeConstString("[") &&
                 tok->strAt(3) != MatchCompiler::makeConstString(".")) ||
                match85(tok, varid) ||
                match86(tok, varid) ||
                match87(tok, varid)) {
                addtoken(&rettail, tok, "use");
            } else if (match88(tok->previous(), varid)) {
                // warning is written for "dealloc ; use_ ;".
                // but this use doesn't affect the leak-checking
                addtoken(&rettail, tok, "use_");
            }
        }

        // Investigate function calls..
        if (match8(tok)) {
            // A function call should normally be followed by ";"
            if (match89(tok->next()->link())) {
                if (!match90(tok)) {
                    addtoken(&rettail, tok, "exit");
                    addtoken(&rettail, tok, ";");
                    tok = tok->next()->link();
                    continue;
                }
            }

            // Calling setjmp / longjmp => bail out
            else if (match91(tok)) {
                while (rethead->next())
                    rethead->deleteNext();
                return rethead;
            }

            // Inside class function.. if the var is passed as a parameter then
            // just add a "::use"
            // The "::use" means that a member function was probably called but it wasn't analysed further
            else if (classmember) {
                if (mSettings->library.isnoreturn(tok))
                    addtoken(&rettail, tok, "exit");

                else if (!test_white_list(tok->str(), mSettings, mTokenizer->isCPP())) {
                    const Token* const end2 = tok->linkAt(1);
                    for (const Token *tok2 = tok->tokAt(2); tok2 != end2; tok2 = tok2->next()) {
                        if (tok2->varId() == varid) {
                            addtoken(&rettail, tok, "::use");
                            break;
                        }
                    }
                }
            }

            else {
                if (varid > 0 && match92(tok, varid)) {
                    addtoken(&rettail, tok, "dealloc");
                    tok = tok->next()->link();
                    continue;
                }

                bool allocpar = false;
                const char *str = call_func(tok, callstack, varid, alloctype, dealloctype, allocpar, sz);
                if (str) {
                    if (allocpar) {
                        addtoken(&rettail, tok, str);
                        tok = tok->next()->link();
                    } else if (varid == 0 || str != std::string("alloc")) {
                        addtoken(&rettail, tok, str);
                    } else if (match13(tok->tokAt(-2), varid)) {
                        addtoken(&rettail, tok, str);
                    }
                } else if (varid > 0 &&
                           getReallocationType(tok, varid) != No &&
                           tok->tokAt(2)->varId() == varid) {
                    addtoken(&rettail, tok, "if");
                    addtoken(&rettail, tok, "{");
                    addtoken(&rettail, tok, "dealloc");
                    addtoken(&rettail, tok, ";");
                    addtoken(&rettail, tok, "}");
                    tok = tok->next()->link();
                    continue;
                }
            }
        }

        // Callback..
        if (match93(tok) && match94(tok->link())) {
            const Token *tok2 = tok->next();
            if (tok2->str() == MatchCompiler::makeConstString("*"))
                tok2 = tok2->next();
            tok2 = tok2->next();

            while (match95(tok2))
                tok2 = tok2->tokAt(2);

            if (match94(tok2)) {
                for (; tok2; tok2 = tok2->next()) {
                    if (match96(tok2))
                        break;
                    else if (tok2->varId() == varid) {
                        addtoken(&rettail, tok, "use");
                        break;
                    }
                }
            }
        }

        // Linux lists..
        if (varid > 0 && match97(tok, varid)) {
            // Is variable passed to a "leak-ignore" function?
            bool leakignore = false;
            if (match54(tok)) {
                const Token *parent = tok;
                while (parent && parent->str() != MatchCompiler::makeConstString("("))
                    parent = parent->astParent();
                if (parent && parent->astOperand1() && parent->astOperand1()->isName()) {
                    const std::string &functionName = parent->astOperand1()->str();
                    if (mSettings->library.isLeakIgnore(functionName))
                        leakignore = true;
                }
            }
            // Not passed to "leak-ignore" function, add "&use".
            if (!leakignore)
                addtoken(&rettail, tok, "&use");
        }
    }

    for (Token *tok1 = rethead; tok1; tok1 = tok1->next()) {
        if (match98(tok1)) {
            tok1->deleteThis();
            tok1->insertToken("use");
            tok1->insertToken(";");
        }
    }

    return rethead;
}




void CheckMemoryLeakInFunction::simplifycode(Token *tok) const
{
    if (mTokenizer->isCPP()) {
        // Replace "throw" that is not in a try block with "return"
        int indentlevel = 0;
        int trylevel = -1;
        for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("{"))
                ++indentlevel;
            else if (tok2->str() == MatchCompiler::makeConstString("}")) {
                --indentlevel;
                if (indentlevel <= trylevel)
                    trylevel = -1;
            } else if (trylevel == -1 && tok2->str() == MatchCompiler::makeConstString("try"))
                trylevel = indentlevel;
            else if (trylevel == -1 && tok2->str() == MatchCompiler::makeConstString("throw"))
                tok2->str("return");
        }
    }

    const bool printExperimental = mSettings->experimental;

    // Insert extra ";"
    for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
        if (!tok2->previous() || match33(tok2->previous())) {
            if (match99(tok2)) {
                tok2->insertToken(";");
            }
        }
    }

    // remove redundant braces..
    for (Token *start = tok; start; start = start->next()) {
        if (match100(start)) {
            // the "link" doesn't work here. Find the end brace..
            unsigned int indent = 0;
            for (Token *end = start; end; end = end->next()) {
                if (end->str() == MatchCompiler::makeConstString("{"))
                    ++indent;
                else if (end->str() == MatchCompiler::makeConstString("}")) {
                    if (indent <= 1) {
                        // If the start/end braces are redundant, delete them
                        if (indent == 1 && match101(end->previous())) {
                            start->deleteNext();
                            end->deleteThis();
                        }
                        break;
                    }
                    --indent;
                }
            }
        }
    }

    // reduce the code..
    // it will be reduced in N passes. When a pass completes without any
    // simplifications the loop is done.
    bool done = false;
    while (! done) {
        //tok->printOut("simplifycode loop..");
        done = true;

        // reduce callfunc
        for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("callfunc")) {
                if (!match102(tok2->previous()))
                    tok2->deleteThis();
            }
        }

        // If the code starts with "if return ;" then remove it
        if (match103(tok)) {
            tok->deleteNext();
            tok->deleteThis();
            if (tok->str() == MatchCompiler::makeConstString("return"))
                tok->deleteThis();
            if (tok->strAt(1) == MatchCompiler::makeConstString("else"))
                tok->deleteNext();
        }

        // simplify "while1" contents..
        for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
            if (match104(tok2)) {
                unsigned int innerIndentlevel = 0;
                for (Token *tok3 = tok2->tokAt(2); tok3; tok3 = tok3->next()) {
                    if (tok3->str() == MatchCompiler::makeConstString("{"))
                        ++innerIndentlevel;
                    else if (tok3->str() == MatchCompiler::makeConstString("}")) {
                        if (innerIndentlevel == 0)
                            break;
                        --innerIndentlevel;
                    }
                    while (innerIndentlevel == 0 && match105(tok3)) {
                        tok3->deleteNext(5);
                        if (tok3->strAt(1) == MatchCompiler::makeConstString("else"))
                            tok3->deleteNext();
                    }
                }

                if (match106(tok2)) {
                    tok2->str(";");
                    tok2->deleteNext(3);
                    tok2->tokAt(4)->deleteNext(2);
                }
            }
        }

        // Main inner simplification loop
        for (Token *tok2 = tok; tok2; tok2 = tok2 ? tok2->next() : nullptr) {
            // Delete extra ";"
            while (match107(tok2)) {
                tok2->deleteNext();
                done = false;
            }

            // Replace "{ }" with ";"
            if (match108(tok2->next())) {
                tok2->deleteNext(2);
                tok2->insertToken(";");
                done = false;
            }

            // Delete braces around a single instruction..
            if (match109(tok2->next())) {
                tok2->deleteNext();
                tok2->tokAt(2)->deleteNext();
                done = false;
            }
            if (match110(tok2->next())) {
                tok2->deleteNext();
                tok2->tokAt(3)->deleteNext();
                done = false;
            }

            // Reduce "if if|callfunc" => "if"
            else if (match111(tok2)) {
                tok2->deleteNext();
                done = false;
            }

            // outer/inner if blocks. Remove outer condition..
            else if (match112(tok2->next())) {
                tok2->deleteNext(2);
                tok2->tokAt(4)->deleteNext();
                done = false;
            }

            else if (tok2->next() && tok2->next()->str() == MatchCompiler::makeConstString("if")) {
                // Delete empty if that is not followed by an else
                if (match113(tok2->next())) {
                    tok2->deleteNext();
                    done = false;
                }

                // Reduce "if X ; else X ;" => "X ;"
                else if (match114(tok2->next()) &&
                         tok2->strAt(2) == tok2->strAt(5)) {
                    tok2->deleteNext(4);
                    done = false;
                }

                // Reduce "if continue ; if continue ;" => "if continue ;"
                else if (match115(tok2->next())) {
                    tok2->deleteNext(3);
                    done = false;
                }

                // Reduce "if return ; alloc ;" => "alloc ;"
                else if (match116(tok2)) {
                    tok2->deleteNext(3);
                    done = false;
                }

                // "[;{}] if alloc ; else return ;" => "[;{}] alloc ;"
                else if (match117(tok2)) {
                    // Remove "if"
                    tok2->deleteNext();
                    // Remove "; else return"
                    tok2->next()->deleteNext(3);
                    done = false;
                }

                // Reduce "if ; else %name% ;" => "if %name% ;"
                else if (match118(tok2->next())) {
                    tok2->next()->deleteNext(2);
                    done = false;
                }

                // Reduce "if ; else" => "if"
                else if (match119(tok2->next())) {
                    tok2->next()->deleteNext(2);
                    done = false;
                }

                // Reduce "if return ; else|if return|continue ;" => "if return ;"
                else if (match120(tok2->next())) {
                    tok2->tokAt(3)->deleteNext(3);
                    done = false;
                }

                // Reduce "if continue|break ; else|if return ;" => "if return ;"
                else if (match121(tok2->next())) {
                    tok2->next()->deleteNext(3);
                    done = false;
                }

                // Remove "else" after "if continue|break|return"
                else if (match122(tok2->next())) {
                    tok2->tokAt(3)->deleteNext();
                    done = false;
                }

                // Delete "if { dealloc|assign|use ; return ; }"
                else if (match123(tok2) &&
                         !findmatch124(tok) ) {
                    tok2->deleteNext(7);
                    if (tok2->strAt(1) == MatchCompiler::makeConstString("else"))
                        tok2->deleteNext();
                    done = false;
                }

                // Remove "if { dealloc ; callfunc ; } !!else|return"
                else if (match125(tok2->next()) &&
                         !match126(tok2->tokAt(8))) {
                    tok2->deleteNext(7);
                    done = false;
                }

                continue;
            }

            // Reduce "alloc while(!var) alloc ;" => "alloc ;"
            if (match127(tok2)) {
                tok2->deleteNext(3);
                done = false;
            }

            // Reduce "ifv return;" => "if return use;"
            if (match128(tok2)) {
                tok2->str("if");
                tok2->next()->insertToken("use");
                done = false;
            }

            // Reduce "if(var) dealloc ;" and "if(var) use ;" that is not followed by an else..
            if (match129(tok2)) {
                tok2->deleteNext();
                done = false;
            }

            // Reduce "; if(!var) alloc ; !!else" => "; dealloc ; alloc ;"
            if (match130(tok2)) {
                // Remove the "if(!var)"
                tok2->deleteNext();

                // Insert "dealloc ;" before the "alloc ;"
                tok2->insertToken(";");
                tok2->insertToken("dealloc");

                done = false;
            }

            // Reduce "if(!var) exit ;" => ";"
            if (match131(tok2)) {
                tok2->deleteNext(2);
                done = false;
            }

            // Reduce "if* ;"..
            if (match132(tok2->next())) {
                // Followed by else..
                if (tok2->strAt(3) == MatchCompiler::makeConstString("else")) {
                    tok2 = tok2->next();
                    if (tok2->str() == MatchCompiler::makeConstString("if(var)"))
                        tok2->str("if(!var)");
                    else if (tok2->str() == MatchCompiler::makeConstString("if(!var)"))
                        tok2->str("if(var)");

                    // remove the "; else"
                    tok2->deleteNext(2);
                } else {
                    // remove the "if*"
                    tok2->deleteNext();
                }
                done = false;
            }

            // Reduce "else ;" => ";"
            if (match133(tok2->next())) {
                tok2->deleteNext();
                done = false;
            }

            // Reduce "while1 continue| ;" => "use ;"
            if (match134(tok2)) {
                tok2->str("use");
                while (tok2->strAt(1) != MatchCompiler::makeConstString(";"))
                    tok2->deleteNext();
                done = false;
            }

            // Reduce "while1 if break ;" => ";"
            if (match135(tok2)) {
                tok2->str(";");
                tok2->deleteNext(2);
                done = false;
            }

            // Delete if block: "alloc; if return use ;"
            if (match136(tok2)) {
                tok2->deleteNext(4);
                done = false;
            }

            // Reduce "alloc|dealloc|use|callfunc ; exit ;" => "; exit ;"
            if (match137(tok2)) {
                tok2->deleteNext();
                done = false;
            }

            // Reduce "alloc|dealloc|use ; if(var) exit ;"
            if (match138(tok2)) {
                tok2->deleteThis();
                done = false;
            }

            // Remove "if exit ;"
            if (match139(tok2)) {
                tok2->deleteNext();
                tok2->deleteThis();
                done = false;
            }

            // Remove the "if break|continue ;" that follows "dealloc ; alloc ;"
            if (!printExperimental && match140(tok2)) {
                tok2->tokAt(3)->deleteNext(2);
                done = false;
            }

            // if break ; break ; => break ;
            if (match141(tok2->previous())) {
                tok2->deleteNext(3);
                done = false;
            }

            // Reduce "do { dealloc ; alloc ; } while(var) ;" => ";"
            if (match142(tok2->next())) {
                tok2->deleteNext(8);
                done = false;
            }

            // Ticket #7745
            // Delete "if (!var) { alloc ; dealloc }" blocks
            if (match143(tok2->next())) {
                tok2->deleteNext(7);
                done = false;
            }

            // Reduce "do { alloc ; } " => "alloc ;"
            /** @todo If the loop "do { alloc ; }" can be executed twice, reduce it to "loop alloc ;" */
            if (match144(tok2->next())) {
                tok2->deleteNext(2);
                tok2->tokAt(2)->deleteNext();
                done = false;
            }

            // Reduce "loop break ; => ";"
            if (match145(tok2->next())) {
                tok2->deleteNext(2);
                done = false;
            }

            // Reduce "loop|do ;" => ";"
            if (match146(tok2)) {
                tok2->deleteThis();
                done = false;
            }

            // Reduce "loop if break|continue ; !!else" => ";"
            if (match147(tok2->next())) {
                tok2->deleteNext(3);
                done = false;
            }

            // Reduce "loop { if break|continue ; !!else" => "loop {"
            if (match148(tok2)) {
                tok2->next()->deleteNext(3);
                done = false;
            }

            // Replace "do ; loop ;" with ";"
            if (match149(tok2)) {
                tok2->deleteNext(2);
                done = false;
            }

            // Replace "loop loop .." with "loop .."
            if (match150(tok2)) {
                tok2->deleteThis();
                done = false;
            }

            // Replace "loop if return ;" with "if return ;"
            if (match151(tok2->next())) {
                tok2->deleteNext();
                done = false;
            }

            // Reduce "loop|while1 { dealloc ; alloc ; }"
            if (match152(tok2)) {
                // delete "{"
                tok2->deleteNext();
                // delete "loop|while1"
                tok2->deleteThis();

                // delete "}"
                tok2->tokAt(3)->deleteNext();

                done = false;
            }

            // loop { use ; callfunc ; }  =>  use ;
            // assume that the "callfunc" is not noreturn
            if (match153(tok2)) {
                tok2->deleteNext(6);
                tok2->str("use");
                tok2->insertToken(";");
                done = false;
            }

            // Delete if block in "alloc ; if(!var) return ;"
            if (match154(tok2)) {
                tok2->deleteNext(3);
                done = false;
            }

            // Reduce "[;{}] return use ; %name%" => "[;{}] return use ;"
            if (match155(tok2)) {
                tok2->tokAt(3)->deleteNext();
                done = false;
            }

            // Reduce "if(var) return use ;" => "return use ;"
            if (match156(tok2->next())) {
                tok2->deleteNext();
                done = false;
            }

            // malloc - realloc => alloc ; dealloc ; alloc ;
            // Reduce "[;{}] alloc ; dealloc ; alloc ;" => "[;{}] alloc ;"
            if (match157(tok2)) {
                tok2->deleteNext(4);
                done = false;
            }

            // use; dealloc; => dealloc;
            if (match158(tok2)) {
                tok2->deleteNext(2);
                done = false;
            }

            // use use => use
            while (match159(tok2)) {
                tok2->deleteNext();
                done = false;
            }

            // use use_ => use
            if (match160(tok2)) {
                tok2->deleteNext();
                done = false;
            }

            // use_ use => use
            if (match161(tok2)) {
                tok2->deleteThis();
                done = false;
            }

            // use & use => use
            while (match162(tok2)) {
                tok2->deleteNext(2);
                done = false;
            }

            // & use use => use
            while (match163(tok2)) {
                tok2->deleteThis();
                tok2->deleteThis();
                done = false;
            }

            // use; if| use; => use;
            while (match164(tok2)) {
                Token *t = tok2->tokAt(2);
                t->deleteNext(2+(t->str()==MatchCompiler::makeConstString("if") ? 1 : 0));
                done = false;
            }

            // Delete first part in "use ; return use ;"
            if (match165(tok2)) {
                tok2->deleteNext(2);
                done = false;
            }

            // try/catch
            if (match166(tok2)) {
                tok2->deleteNext(3);
                tok2->deleteThis();
                done = false;
            }

            // Delete second case in "case ; case ;"
            while (match167(tok2)) {
                tok2->deleteNext(2);
                done = false;
            }

            // Replace switch with if (if not complicated)
            if (match168(tok2)) {
                // Right now, I just handle if there are a few case and perhaps a default.
                bool valid = false;
                bool incase = false;
                for (const Token * _tok = tok2->tokAt(2); _tok; _tok = _tok->next()) {
                    if (_tok->str() == MatchCompiler::makeConstString("{"))
                        break;

                    else if (_tok->str() == MatchCompiler::makeConstString("}")) {
                        valid = true;
                        break;
                    }

                    else if (_tok->str() == MatchCompiler::makeConstString("switch"))
                        break;

                    else if (_tok->str() == MatchCompiler::makeConstString("loop"))
                        break;

                    else if (incase && _tok->str() == MatchCompiler::makeConstString("case"))
                        break;

                    else if (match169(_tok))
                        break;

                    if (match170(_tok))
                        _tok = _tok->tokAt(2);

                    incase = incase || (_tok->str() == MatchCompiler::makeConstString("case"));
                    incase = incase && (_tok->str() != MatchCompiler::makeConstString("break") && _tok->str() != MatchCompiler::makeConstString("return"));
                }

                if (!incase && valid) {
                    done = false;
                    tok2->str(";");
                    tok2->deleteNext();
                    tok2 = tok2->next();
                    bool first = true;
                    while (match171(tok2)) {
                        const bool def(tok2->str() == MatchCompiler::makeConstString("default"));
                        tok2->str(first ? "if" : "}");
                        if (first) {
                            first = false;
                            tok2->insertToken("{");
                        } else {
                            // Insert "else [if] {
                            tok2->insertToken("{");
                            if (! def)
                                tok2->insertToken("if");
                            tok2->insertToken("else");
                            tok2 = tok2->next();
                        }
                        while (tok2) {
                            if (tok2->str() == MatchCompiler::makeConstString("}"))
                                break;
                            if (match172(tok2))
                                break;
                            if (match170(tok2))
                                tok2 = tok2->tokAt(2);
                            else
                                tok2 = tok2->next();
                        }
                        if (match173(tok2)) {
                            tok2->str(";");
                            tok2 = tok2->tokAt(2);
                        } else if (tok2 && tok2->str() == MatchCompiler::makeConstString("return")) {
                            tok2 = tok2->tokAt(2);
                        }
                    }
                }
            }
        }

        // If "--all" is given, remove all "callfunc"..
        if (done &&  printExperimental) {
            for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
                if (tok2->str() == MatchCompiler::makeConstString("callfunc")) {
                    tok2->deleteThis();
                    done = false;
                }
            }
        }
    }
}



const Token *CheckMemoryLeakInFunction::findleak(const Token *tokens)
{
    const Token *result;

    if (match174(tokens)) {
        return tokens->tokAt(3);
    }

    if ((result = findmatch175(tokens) ) != nullptr) {
        return result;
    }

    if ((result = findmatch176(tokens) ) != nullptr) {
        return result->tokAt(3);
    }

    if ((result = findmatch177(tokens) ) != nullptr) {
        return result->tokAt(2);
    }

    if ((result = findmatch178(tokens) ) != nullptr) {
        return result->tokAt(3 + (result->strAt(3) == MatchCompiler::makeConstString("{")));
    }

    if ((result = findmatch179(tokens) ) != nullptr) {
        return result->tokAt(4);
    }

    if (((result = findmatch180(tokens) ) != nullptr) ||
        ((result = findmatch181(tokens) ) != nullptr)) {
        return result->tokAt(6);
    }

    if ((result = findmatch182(tokens) ) != nullptr) {
        if (result->tokAt(3) == nullptr)
            return result->tokAt(2);
    }

    // No deallocation / usage => report leak at the last token
    if (!findmatch183(tokens) ) {
        const Token *last = tokens;
        while (last->next())
            last = last->next();

        // not a leak if exit is called before the end of the function
        if (!match184(last->tokAt(-2)))
            return last;
    }

    return nullptr;
}


// Check for memory leaks for a function variable.
void CheckMemoryLeakInFunction::checkScope(const Token *startTok, const std::string &varname, unsigned int varid, bool classmember, unsigned int sz)
{
    const std::list<const Token *> callstack;

    AllocType alloctype = No;
    AllocType dealloctype = No;

    const Token *result;

    Token *tok = getcode(startTok, callstack, varid, alloctype, dealloctype, classmember, sz);
    //tok->printOut((std::string("Checkmemoryleak: getcode result for: ") + varname).c_str());

    const bool use_addr = bool(findmatch30(tok)  != nullptr);

    // Simplify the code and check if freed memory is used..
    for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
        while (match107(tok2))
            tok2->deleteNext();
    }
    if ((result = findmatch185(tok) ) != nullptr) {
        deallocuseError(result->tokAt(3), varname);
    }

    // Replace "&use" with "use". Replace "use_" with ";"
    for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
        if (tok2->str() == MatchCompiler::makeConstString("&use"))
            tok2->str("use");
        else if (tok2->str() == MatchCompiler::makeConstString("use_"))
            tok2->str(";");
        else if (match186(tok2))
            tok2->deleteNext();
        else if (tok2->str() == MatchCompiler::makeConstString("::use"))    // Some kind of member function usage. Not analyzed very well.
            tok2->str("use");
        else if (tok2->str() == MatchCompiler::makeConstString("recursive"))
            tok2->str("use");
        else if (tok2->str() == MatchCompiler::makeConstString("dealloc_"))
            tok2->str("dealloc");
        else if (tok2->str() == MatchCompiler::makeConstString("realloc")) {
            tok2->str("dealloc");
            tok2->insertToken("alloc");
            tok2->insertToken(";");
        }
    }

    // If the variable is not allocated at all => no memory leak
    if (findmatch187(tok)  == nullptr) {
        TokenList::deleteTokens(tok);
        return;
    }

    simplifycode(tok);

    if (mSettings->debugwarnings) {
        tok->printOut(("Checkmemoryleak: simplifycode result for: " + varname).c_str());
    }

    // If the variable is not allocated at all => no memory leak
    if (findmatch187(tok)  == nullptr) {
        TokenList::deleteTokens(tok);
        return;
    }

    /** @todo handle "goto" */
    if (findmatch188(tok) ) {
        TokenList::deleteTokens(tok);
        return;
    }

    if ((result = findleak(tok)) != nullptr) {
        memoryLeak(result, varname, alloctype);
    }

    else if (!use_addr && (result = findmatch189(tok) ) != nullptr) {
        deallocDeallocError(result->tokAt(2), varname);
    }

    // detect cases that "simplifycode" don't handle well..
    else if (tok && mSettings->debugwarnings) {
        Token *first = tok;
        while (first && first->str() == MatchCompiler::makeConstString(";"))
            first = first->next();

        bool noerr = false;
        noerr = noerr || match190(first);
        noerr = noerr || match191(first);
        noerr = noerr || match192(first);
        noerr = noerr || match193(first);
        noerr = noerr || match194(first);
        noerr = noerr || match195(first);
        noerr = noerr || match196(first);
        noerr = noerr || match197(first);
        noerr = noerr || match198(first);
        noerr = noerr || match199(first);
        noerr = noerr || match200(first);

        // Unhandled case..
        if (!noerr)
            reportError(first, Severity::debug, "debug",
                        "inconclusive leak of " + varname + ": " + tok->stringifyList(false, false, false, false, false, nullptr, nullptr));
    }

    TokenList::deleteTokens(tok);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Check for memory leaks due to improper realloc() usage.
//   Below, "a" may be set to null without being freed if realloc() cannot
//   allocate the requested memory:
//     a = malloc(10); a = realloc(a, 100);
//---------------------------------------------------------------------------

static bool isNoArgument(const SymbolDatabase* symbolDatabase, unsigned int varid)
{
    const Variable* var = symbolDatabase->getVariableFromVarId(varid);
    return var && !var->isArgument();
}

void CheckMemoryLeakInFunction::checkReallocUsage()
{
    // only check functions
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {

        // Search for the "var = realloc(var, 100" pattern within this function
        for (const Token *tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (tok->varId() > 0 &&
                match201(tok) &&
                tok->varId() == tok->tokAt(4)->varId() &&
                isNoArgument(symbolDatabase, tok->varId())) {
                // Check that another copy of the pointer wasn't saved earlier in the function
                if (findmatch202(scope->bodyStart, tok, tok->varId())  ||
                    findmatch203(scope->bodyStart, tok, tok->varId()) )
                    continue;

                const Token* tokEndRealloc = tok->linkAt(3);
                // Check that the allocation isn't followed immediately by an 'if (!var) { error(); }' that might handle failure
                if (match204(tokEndRealloc->next()) &&
                    notvar(tokEndRealloc->tokAt(3)->astOperand2(), tok->varId())) {
                    const Token* tokEndBrace = tokEndRealloc->linkAt(3)->linkAt(1);
                    if (tokEndBrace && mTokenizer->IsScopeNoReturn(tokEndBrace))
                        continue;
                }

                memleakUponReallocFailureError(tok, tok->str());
            } else if (tok->next()->varId() > 0 &&
                       (match205(tok) &&
                        tok->next()->varId() == tok->tokAt(6)->varId()) &&
                       isNoArgument(symbolDatabase, tok->next()->varId())) {
                // Check that another copy of the pointer wasn't saved earlier in the function
                if (findmatch206(scope->bodyStart, tok, tok->next()->varId())  ||
                    findmatch207(scope->bodyStart, tok, tok->next()->varId()) )
                    continue;

                const Token* tokEndRealloc = tok->linkAt(4);
                // Check that the allocation isn't followed immediately by an 'if (!var) { error(); }' that might handle failure
                if (match208(tokEndRealloc->next(), tok->next()->varId())) {
                    const Token* tokEndBrace = tokEndRealloc->linkAt(8);
                    if (tokEndBrace && match209(tokEndBrace->tokAt(-2)) &&
                        match210(tokEndBrace->linkAt(-2)->tokAt(-2)))
                        continue;
                }
                memleakUponReallocFailureError(tok->next(), tok->strAt(1));
            }
        }
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Checks for memory leaks inside function..
//---------------------------------------------------------------------------

static bool isInMemberFunc(const Scope* scope)
{
    while (scope->nestedIn && !scope->functionOf)
        scope = scope->nestedIn;

    return (scope->functionOf != nullptr);
}

void CheckMemoryLeakInFunction::check()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    // Check locking/unlocking of global resources..
    for (const Scope * scope : symbolDatabase->functionScopes) {
        if (!scope->hasInlineOrLambdaFunction())
            checkScope(scope->bodyStart->next(), emptyString, 0, scope->functionOf != nullptr, 1);
    }

    // Check variables..
    for (const Variable* var : symbolDatabase->variableList()) {
        if (!var || (!var->isLocal() && !var->isArgument()) || var->isStatic() || !var->scope())
            continue;

        if (var->isReference())
            continue;

        if (!var->isPointer() && var->typeStartToken()->str() != MatchCompiler::makeConstString("int"))
            continue;

        // check for known class without implementation (forward declaration)
        if (var->isPointer() && var->type() && !var->typeScope())
            continue;

        if (var->scope()->hasInlineOrLambdaFunction())
            continue;

        unsigned int sz = mTokenizer->sizeOfType(var->typeStartToken());
        if (sz < 1)
            sz = 1;

        if (var->isArgument())
            checkScope(var->scope()->bodyStart->next(), var->name(), var->declarationId(), isInMemberFunc(var->scope()), sz);
        else
            checkScope(var->nameToken(), var->name(), var->declarationId(), isInMemberFunc(var->scope()), sz);
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Checks for memory leaks in classes..
//---------------------------------------------------------------------------


void CheckMemoryLeakInClass::check()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    // only check classes and structures
    for (const Scope * scope : symbolDatabase->classAndStructScopes) {
        for (const Variable &var : scope->varlist) {
            if (!var.isStatic() && var.isPointer()) {
                // allocation but no deallocation of private variables in public function..
                const Token *tok = var.typeStartToken();
                // Either it is of standard type or a non-derived type
                if (tok->isStandardType() || (var.type() && var.type()->derivedFrom.empty())) {
                    if (var.isPrivate())
                        checkPublicFunctions(scope, var.nameToken());

                    variable(scope, var.nameToken());
                }
            }
        }
    }
}


void CheckMemoryLeakInClass::variable(const Scope *scope, const Token *tokVarname)
{
    const std::string& varname = tokVarname->str();
    const unsigned int varid = tokVarname->varId();
    const std::string& classname = scope->className;

    // Check if member variable has been allocated and deallocated..
    CheckMemoryLeak::AllocType Alloc = CheckMemoryLeak::No;
    CheckMemoryLeak::AllocType Dealloc = CheckMemoryLeak::No;

    bool allocInConstructor = false;
    bool deallocInDestructor = false;

    // Inspect member functions
    for (const Function &func : scope->functionList) {
        const bool constructor = func.isConstructor();
        const bool destructor = func.isDestructor();
        if (!func.hasBody()) {
            if (destructor) { // implementation for destructor is not seen => assume it deallocates all variables properly
                deallocInDestructor = true;
                Dealloc = CheckMemoryLeak::Many;
            }
            continue;
        }
        bool body = false;
        const Token *end = func.functionScope->bodyEnd;
        for (const Token *tok = func.arg->link(); tok != end; tok = tok->next()) {
            if (tok == func.functionScope->bodyStart)
                body = true;
            else {
                if (!body) {
                    if (!match211(tok, varid))
                        continue;
                }

                // Allocate..
                if (!body || match13(tok, varid)) {
                    // var1 = var2 = ...
                    // bail out
                    if (tok->strAt(-1) == MatchCompiler::makeConstString("="))
                        return;

                    // Foo::var1 = ..
                    // bail out when not same class
                    if (tok->strAt(-1) == MatchCompiler::makeConstString("::") &&
                        tok->strAt(-2) != scope->className)
                        return;

                    AllocType alloc = getAllocationType(tok->tokAt(body ? 2 : 3), 0);
                    if (alloc != CheckMemoryLeak::No) {
                        if (constructor)
                            allocInConstructor = true;

                        if (Alloc != No && Alloc != alloc)
                            alloc = CheckMemoryLeak::Many;

                        if (alloc != CheckMemoryLeak::Many && Dealloc != CheckMemoryLeak::No && Dealloc != CheckMemoryLeak::Many && Dealloc != alloc) {
                            std::list<const Token *> callstack;
                            callstack.push_back(tok);
                            mismatchAllocDealloc(callstack, classname + "::" + varname);
                        }

                        Alloc = alloc;
                    }
                }

                if (!body)
                    continue;

                // Deallocate..
                AllocType dealloc = getDeallocationType(tok, varid);
                // some usage in the destructor => assume it's related
                // to deallocation
                if (destructor && tok->str() == varname)
                    dealloc = CheckMemoryLeak::Many;
                if (dealloc != CheckMemoryLeak::No) {
                    if (destructor)
                        deallocInDestructor = true;

                    // several types of allocation/deallocation?
                    if (Dealloc != CheckMemoryLeak::No && Dealloc != dealloc)
                        dealloc = CheckMemoryLeak::Many;

                    if (dealloc != CheckMemoryLeak::Many && Alloc != CheckMemoryLeak::No &&  Alloc != Many && Alloc != dealloc) {
                        std::list<const Token *> callstack;
                        callstack.push_back(tok);
                        mismatchAllocDealloc(callstack, classname + "::" + varname);
                    }

                    Dealloc = dealloc;
                }

                // Function call .. possible deallocation
                else if (match212(tok->previous())) {
                    if (!CheckMemoryLeakInFunction::test_white_list(tok->str(), mSettings, mTokenizer->isCPP())) {
                        return;
                    }
                }
            }
        }
    }

    if (allocInConstructor && !deallocInDestructor) {
        unsafeClassError(tokVarname, classname, classname + "::" + varname /*, Alloc*/);
    } else if (Alloc != CheckMemoryLeak::No && Dealloc == CheckMemoryLeak::No) {
        unsafeClassError(tokVarname, classname, classname + "::" + varname /*, Alloc*/);
    }
}

void CheckMemoryLeakInClass::unsafeClassError(const Token *tok, const std::string &classname, const std::string &varname)
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    reportError(tok, Severity::style, "unsafeClassCanLeak",
                "$symbol:" + classname + "\n"
                "$symbol:" + varname + "\n"
                "Class '" + classname + "' is unsafe, '" + varname + "' can leak by wrong usage.\n"
                "The class '" + classname + "' is unsafe, wrong usage can cause memory/resource leaks for '" + varname + "'. This can for instance be fixed by adding proper cleanup in the destructor.", CWE398, false);
}


void CheckMemoryLeakInClass::checkPublicFunctions(const Scope *scope, const Token *classtok)
{
    // Check that public functions deallocate the pointers that they allocate.
    // There is no checking how these functions are used and therefore it
    // isn't established if there is real leaks or not.
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const unsigned int varid = classtok->varId();

    // Parse public functions..
    // If they allocate member variables, they should also deallocate
    for (const Function &func : scope->functionList) {
        if ((func.type == Function::eFunction || func.type == Function::eOperatorEqual) &&
            func.access == Public && func.hasBody()) {
            const Token *tok2 = func.functionScope->bodyStart->next();
            if (match13(tok2, varid)) {
                const CheckMemoryLeak::AllocType alloc = getAllocationType(tok2->tokAt(2), varid);
                if (alloc != CheckMemoryLeak::No)
                    publicAllocationError(tok2, tok2->str());
            } else if (match213(tok2, varid) &&
                       tok2->str() == scope->className) {
                const CheckMemoryLeak::AllocType alloc = getAllocationType(tok2->tokAt(4), varid);
                if (alloc != CheckMemoryLeak::No)
                    publicAllocationError(tok2, tok2->strAt(2));
            }
        }
    }
}

void CheckMemoryLeakInClass::publicAllocationError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::warning, "publicAllocationError", "$symbol:" + varname + "\nPossible leak in public function. The pointer '$symbol' is not deallocated before it is allocated.", CWE398, false);
}


void CheckMemoryLeakStructMember::check()
{
    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Variable* var : symbolDatabase->variableList()) {
        if (!var || !var->isLocal() || var->isStatic())
            continue;
        if (var->typeEndToken()->isStandardType())
            continue;
        checkStructVariable(var);
    }
}

bool CheckMemoryLeakStructMember::isMalloc(const Variable *variable)
{
    const unsigned int declarationId(variable->declarationId());
    bool alloc = false;
    for (const Token *tok2 = variable->nameToken(); tok2 && tok2 != variable->scope()->bodyEnd; tok2 = tok2->next()) {
        if (match214(tok2, declarationId)) {
            return false;
        } else if (match215(tok2, declarationId)) {
            alloc = true;
        }
    }
    return alloc;
}

void CheckMemoryLeakStructMember::checkStructVariable(const Variable * const variable)
{
    // Is struct variable a pointer?
    if (variable->isPointer()) {
        // Check that variable is allocated with malloc
        if (!isMalloc(variable))
            return;
    } else if (!mTokenizer->isC() && (!variable->typeScope() || variable->typeScope()->getDestructor())) {
        // For non-C code a destructor might cleanup members
        return;
    }

    // Check struct..
    unsigned int indentlevel2 = 0;
    for (const Token *tok2 = variable->nameToken(); tok2 && tok2 != variable->scope()->bodyEnd; tok2 = tok2->next()) {
        if (tok2->str() == MatchCompiler::makeConstString("{"))
            ++indentlevel2;

        else if (tok2->str() == MatchCompiler::makeConstString("}")) {
            if (indentlevel2 == 0)
                break;
            --indentlevel2;
        }

        // Unknown usage of struct
        /** @todo Check how the struct is used. Only bail out if necessary */
        else if (match15(tok2, variable->declarationId()))
            break;

        // Struct member is allocated => check if it is also properly deallocated..
        else if (match216(tok2->previous(), variable->declarationId())) {
            if (getAllocationType(tok2->tokAt(4), tok2->tokAt(2)->varId()) == AllocType::No)
                continue;

            const unsigned int structid(variable->declarationId());
            const unsigned int structmemberid(tok2->tokAt(2)->varId());

            // This struct member is allocated.. check that it is deallocated
            unsigned int indentlevel3 = indentlevel2;
            for (const Token *tok3 = tok2; tok3; tok3 = tok3->next()) {
                if (tok3->str() == MatchCompiler::makeConstString("{"))
                    ++indentlevel3;

                else if (tok3->str() == MatchCompiler::makeConstString("}")) {
                    if (indentlevel3 == 0) {
                        memoryLeak(tok3, variable->name() + "." + tok2->strAt(2), Malloc);
                        break;
                    }
                    --indentlevel3;
                }

                // Deallocating the struct member..
                else if (getDeallocationType(tok3, structmemberid) != AllocType::No) {
                    // If the deallocation happens at the base level, don't check this member anymore
                    if (indentlevel3 == 0)
                        break;

                    // deallocating and then returning from function in a conditional block =>
                    // skip ahead out of the block
                    bool ret = false;
                    while (tok3) {
                        if (tok3->str() == MatchCompiler::makeConstString("return"))
                            ret = true;
                        else if (tok3->str() == MatchCompiler::makeConstString("{") || tok3->str() == MatchCompiler::makeConstString("}"))
                            break;
                        tok3 = tok3->next();
                    }
                    if (!ret || !tok3 || tok3->str() != MatchCompiler::makeConstString("}"))
                        break;
                    --indentlevel3;
                    continue;
                }

                // Deallocating the struct..
                else if (match217(tok3, structid)) {
                    if (indentlevel2 == 0)
                        memoryLeak(tok3, variable->name() + "." + tok2->strAt(2), Malloc);
                    break;
                }

                // failed allocation => skip code..
                else if (match23(tok3) &&
                         notvar(tok3->next()->astOperand2(), structmemberid)) {
                    // Goto the ")"
                    tok3 = tok3->next()->link();

                    // make sure we have ") {".. it should be
                    if (!match89(tok3))
                        break;

                    // Goto the "}"
                    tok3 = tok3->next()->link();
                }

                // succeeded allocation
                else if (ifvar(tok3, structmemberid, "!=", "0")) {
                    // goto the ")"
                    tok3 = tok3->next()->link();

                    // check if the variable is deallocated or returned..
                    unsigned int indentlevel4 = 0;
                    for (const Token *tok4 = tok3; tok4; tok4 = tok4->next()) {
                        if (tok4->str() == MatchCompiler::makeConstString("{"))
                            ++indentlevel4;
                        else if (tok4->str() == MatchCompiler::makeConstString("}")) {
                            --indentlevel4;
                            if (indentlevel4 == 0)
                                break;
                        } else if (match218(tok4, structmemberid)) {
                            break;
                        }
                    }

                    // was there a proper deallocation?
                    if (indentlevel4 > 0)
                        break;
                }

                // Returning from function..
                else if (tok3->str() == MatchCompiler::makeConstString("return")) {
                    // Returning from function without deallocating struct member?
                    if (!match219(tok3, structid) &&
                        !match220(tok3, structid) &&
                        !(match221(tok3, structid) && tok3->tokAt(3)->varId() == structmemberid)) {
                        memoryLeak(tok3, variable->name() + "." + tok2->strAt(2), Malloc);
                    }
                    break;
                }

                // struct assignment..
                else if (match14(tok3, structid)) {
                    break;
                } else if (match222(tok3, structmemberid)) {
                    break;
                }

                // goto isn't handled well.. bail out even though there might be leaks
                else if (tok3->str() == MatchCompiler::makeConstString("goto"))
                    break;

                // using struct in a function call..
                else if (match8(tok3)) {
                    // Calling non-function / function that doesn't deallocate?
                    if (CheckMemoryLeakInFunction::test_white_list(tok3->str(), mSettings, mTokenizer->isCPP()))
                        continue;

                    // Check if the struct is used..
                    bool deallocated = false;
                    const Token* const end4 = tok3->linkAt(1);
                    for (const Token *tok4 = tok3; tok4 != end4; tok4 = tok4->next()) {
                        if (match223(tok4, structid)) {
                            /** @todo check if the function deallocates the memory */
                            deallocated = true;
                            break;
                        }

                        if (match224(tok4, structid)) {
                            /** @todo check if the function deallocates the memory */
                            deallocated = true;
                            break;
                        }
                    }

                    if (deallocated)
                        break;
                }
            }
        }
    }
}



void CheckMemoryLeakNoVar::check()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    // only check functions
    for (const Scope * scope : symbolDatabase->functionScopes) {

        // Checks if a call to an allocation function like malloc() is made and its return value is not assigned.
        checkForUnusedReturnValue(scope);

        // Checks to see if a function is called with memory allocated for an argument that
        // could be leaked if a function called for another argument throws.
        checkForUnsafeArgAlloc(scope);

        // parse the executable scope until tok is reached...
        for (const Token *tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            // allocating memory in parameter for function call..
            if (!(match225(tok) && match226(tok->linkAt(2))))
                continue;
            if (getAllocationType(tok->next(), 0) == No)
                continue;
            // locate outer function call..
            const Token* tok3 = tok;
            while (tok3 && tok3->astParent() && tok3->str() == MatchCompiler::makeConstString(","))
                tok3 = tok3->astParent();
            if (!tok3 || tok3->str() != MatchCompiler::makeConstString("("))
                continue;
            // Is it a function call..
            if (!match227(tok3->tokAt(-2)))
                continue;
            const std::string& functionName = tok3->strAt(-1);
            if ((mTokenizer->isCPP() && functionName == MatchCompiler::makeConstString("delete")) ||
                functionName == MatchCompiler::makeConstString("free") ||
                functionName == MatchCompiler::makeConstString("fclose") ||
                functionName == MatchCompiler::makeConstString("realloc"))
                break;
            if (CheckMemoryLeakInFunction::test_white_list(functionName, mSettings, mTokenizer->isCPP())) {
                functionCallLeak(tok, tok->strAt(1), functionName);
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------
// Checks if a call to an allocation function like malloc() is made and its return value is not assigned.
//---------------------------------------------------------------------------
void CheckMemoryLeakNoVar::checkForUnusedReturnValue(const Scope *scope)
{
    for (const Token *tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
        if (!match8(tok))
            continue;

        if (tok->varId())
            continue;

        const AllocType allocType = getAllocationType(tok, 0);
        if (allocType == No)
            continue;

        if (tok != tok->next()->astOperand1())
            continue;

        // get ast parent, skip casts
        const Token *parent = tok->next()->astParent();
        while (parent && parent->str() == MatchCompiler::makeConstString("(") && !parent->astOperand2())
            parent = parent->astParent();

        if (!parent) {
            // Check if we are in a C++11 constructor
            const Token * closingBrace = findmatch228(tok) ;
            if (closingBrace->str() == MatchCompiler::makeConstString("}") && match229(closingBrace->link()->tokAt(-1)))
                continue;
            returnValueNotUsedError(tok, tok->str());
        } else if (match230(parent)) {
            returnValueNotUsedError(tok, tok->str());
        }
    }
}

//---------------------------------------------------------------------------
// Check if an exception could cause a leak in an argument constructed with
// shared_ptr/unique_ptr. For example, in the following code, it is possible
// that if g() throws an exception, the memory allocated by "new int(42)"
// could be leaked. See stackoverflow.com/questions/19034538/
// why-is-there-memory-leak-while-using-shared-ptr-as-a-function-parameter
//
// void x() {
//    f(shared_ptr<int>(new int(42)), g());
// }
//---------------------------------------------------------------------------
void CheckMemoryLeakNoVar::checkForUnsafeArgAlloc(const Scope *scope)
{
    // This test only applies to C++ source
    if (!mTokenizer->isCPP() || !mSettings->inconclusive || !mSettings->isEnabled(Settings::WARNING))
        return;

    for (const Token *tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
        if (match8(tok)) {
            const Token *endParamToken = tok->next()->link();
            const Token* pointerType = nullptr;
            const Token* functionCalled = nullptr;

            // Scan through the arguments to the function call
            for (const Token *tok2 = tok->tokAt(2); tok2 && tok2 != endParamToken; tok2 = tok2->nextArgument()) {
                const Function *func = tok2->function();
                const bool isNothrow = func && (func->isAttributeNothrow() || func->isThrow());

                if (match231(tok2) && tok2->next()->link() && match232(tok2->next()->link())) {
                    pointerType = tok2;
                } else if (!isNothrow) {
                    if (match8(tok2))
                        functionCalled = tok2;
                    else if (tok2->isName() && tok2->next()->link() && match233(tok2->next()->link()))
                        functionCalled = tok2;
                }
            }

            if (pointerType && functionCalled) {
                std::string functionName = functionCalled->str();
                if (functionCalled->strAt(1) == MatchCompiler::makeConstString("<")) {
                    functionName += '<';
                    for (const Token* tok2 = functionCalled->tokAt(2); tok2 != functionCalled->next()->link(); tok2 = tok2->next())
                        functionName += tok2->str();
                    functionName += '>';
                }
                std::string objectTypeName;
                for (const Token* tok2 = pointerType->tokAt(2); tok2 != pointerType->next()->link(); tok2 = tok2->next())
                    objectTypeName += tok2->str();

                unsafeArgAllocError(tok, functionName, pointerType->str(), objectTypeName);
            }
        }
    }
}

void CheckMemoryLeakNoVar::functionCallLeak(const Token *loc, const std::string &alloc, const std::string &functionCall)
{
    reportError(loc, Severity::error, "leakNoVarFunctionCall", "Allocation with " + alloc + ", " + functionCall + " doesn't release it.", CWE772, false);
}

void CheckMemoryLeakNoVar::returnValueNotUsedError(const Token *tok, const std::string &alloc)
{
    reportError(tok, Severity::error, "leakReturnValNotUsed", "$symbol:" + alloc + "\nReturn value of allocation function '$symbol' is not stored.", CWE771, false);
}

void CheckMemoryLeakNoVar::unsafeArgAllocError(const Token *tok, const std::string &funcName, const std::string &ptrType, const std::string& objType)
{
    const std::string factoryFunc = ptrType == MatchCompiler::makeConstString("shared_ptr") ? "make_shared" : "make_unique";
    reportError(tok, Severity::warning, "leakUnsafeArgAlloc",
                "$symbol:" + funcName + "\n"
                "Unsafe allocation. If $symbol() throws, memory could be leaked. Use " + factoryFunc + "<" + objType + ">() instead.",
                CWE401,
                true); // Inconclusive because funcName may never throw
}
