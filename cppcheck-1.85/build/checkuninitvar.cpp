#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %name% [
static bool match1(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: %type% %var% ;
static bool match2(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [({:]
static bool match3(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("({:", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% =
static bool match4(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %name% ) (
static bool match5(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) =
static bool match6(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: ] [
static bool match7(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] =|{
static bool match8(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: for (
static bool match9(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %type% * %name% [,)]
static bool match10(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}] %varid% = %name% (
static bool match11(const Token* tok, const unsigned int varid) {
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
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [?:]
static bool match12(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("?:", tok->str()[0]))
        return false;
    return true;
}
// pattern: *|[
static bool match13(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: ,|;|{|}|try
static bool match14(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("try"))))
        return false;
    return true;
}
// pattern: [;{}] %var% = - %name% ;
static bool match15(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if (
static bool match16(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ==|!=
static bool match17(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    return true;
}
// pattern: .
static bool match18(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: [;{}.] %name% = - %name% ;
static bool match19(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}.", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}.] %name% = %num% ;
static bool match20(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}.", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: } else {
static bool match21(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: [;{}.] %var% = - %name% ;
static bool match22(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}.", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}.] %var% = %num% ;
static bool match23(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}.", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: = {
static bool match24(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: %varid%
template<class T> static T * findmatch25(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: & %varid%
template<class T> static T * findmatch26(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        continue;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: sizeof|typeof|offsetof|decltype (
static bool match27(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sizeof")) || (tok->str()==MatchCompiler::makeConstString("typeof")) || (tok->str()==MatchCompiler::makeConstString("offsetof")) || (tok->str()==MatchCompiler::makeConstString("decltype"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: for|while (
static bool match28(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: do {
static bool match29(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("do")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } while (
static bool match30(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% {
static bool match31(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ) {
static bool match32(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ( {
static bool match33(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: asm|setjmp (
static bool match34(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("asm")) || (tok->str()==MatchCompiler::makeConstString("setjmp"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [;{}] %name% :
static bool match35(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: return|break|continue|throw|goto
static bool match36(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("goto"))))
        return false;
    return true;
}
// pattern: %name% . %name% ;|%cop%
static bool match37(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: = %name% (
static bool match38(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ;
static bool match39(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: = new
static bool match40(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    return true;
}
// pattern: = new %type% (
static bool match41(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: = new %type% [
static bool match42(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ( )
static bool match43(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [(,] %name% [,)]
static bool match44(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: ( ! %name% %oror%
static bool match45(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    return true;
}
// pattern: %name% . %name%
static bool match46(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: sizeof|decltype|offsetof (
static bool match47(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sizeof")) || (tok->str()==MatchCompiler::makeConstString("decltype")) || (tok->str()==MatchCompiler::makeConstString("offsetof"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: sizeof|typeof (
static bool match48(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sizeof")) || (tok->str()==MatchCompiler::makeConstString("typeof"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: asm ( %str% ) ;
static bool match49(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("asm")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: sizeof (
static bool match50(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% =
static bool match51(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: return|delete %var% !!=
static bool match52(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("delete"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("="))
        return false;
    return true;
}
// pattern: typeof|__alignof__ ( * %name%
static bool match53(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("typeof")) || (tok->str()==MatchCompiler::makeConstString("__alignof__"))))
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
    return true;
}
// pattern: !!& %var% .
static bool match54(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("&"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: [,(] %name% . %name%
static bool match55(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",(", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: . %name%
static bool match56(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [,)]
static bool match57(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% . %name% (
static bool match58(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [(,] & %name% [,)]
static bool match59(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: ++|--|%cop%
static bool match60(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: & %var% =|( *
static bool match61(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: <<
static bool match62(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<<")))
        return false;
    return true;
}
// pattern: std ::
static bool match63(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: [(,=?:]
static bool match64(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,=?:", tok->str()[0]))
        return false;
    return true;
}
// pattern: = %name% ;|%cop%
static bool match65(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: %name%|*
static bool match66(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: &
static bool match67(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: %name% (
static bool match68(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% [
static bool match69(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: [|.
static bool match70(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: if|while|switch (
static bool match71(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("switch"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [=,(]
static bool match72(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=,(", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name%|.|::
static bool match73(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: [;{}]
static bool match74(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,]
static bool match75(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] &
static bool match76(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: [;{}(]
static bool match77(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}(", tok->str()[0]))
        return false;
    return true;
}
// pattern: if|for|while|switch
static bool match78(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("switch"))))
        return false;
    return true;
}
// pattern: %type% %name%| [,)]
static bool match79(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && (tok->isName()))
        tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: const %type% & %name% [,)]
static bool match80(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: const struct| %type% * %name% [,)]
static bool match81(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: const %type% %name% [
static bool match82(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] [,)]
static bool match83(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: if|while|for
static bool match84(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("for"))))
        return false;
    return true;
}
// pattern: [=.[]
static bool match85(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=.[", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,=] &
static bool match86(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,=", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: [|=
static bool match87(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: [(,] %name% . %name% [,)]
static bool match88(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
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
// pattern: %var% . %name% (
static bool match89(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [(,] & %name%
static bool match90(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: const struct| %type% * const| %name% [,)]
static bool match91(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: = * (
static bool match92(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: = %name% ;
static bool match93(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: *
static bool match94(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: = * (| &
static bool match95(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
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


//---------------------------------------------------------------------------
#include "checkuninitvar.h"

#include "astutils.h"
#include "checknullpointer.h"   // CheckNullPointer::isPointerDeref
#include "errorlogger.h"
#include "library.h"
#include "mathlib.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "valueflow.h"

#include <tinyxml2.h>

#include <cassert>
#include <cstddef>
#include <list>
#include <map>
#include <stack>
#include <utility>
//---------------------------------------------------------------------------

// Register this check class (by creating a static instance of it)
namespace {
    CheckUninitVar instance;
}

//---------------------------------------------------------------------------

// CWE ids used:
static const struct CWE CWE476(476U);  // NULL Pointer Dereference
static const struct CWE CWE676(676U);
static const struct CWE CWE908(908U);
static const struct CWE CWE825(825U);

void CheckUninitVar::check()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    std::set<std::string> arrayTypeDefs;
    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match1(tok) && tok->variable() && match2(tok->variable()->typeStartToken()))
            arrayTypeDefs.insert(tok->variable()->typeStartToken()->str());
    }

    // check every executable scope
    for (std::list<Scope>::const_iterator scope = symbolDatabase->scopeList.begin(); scope != symbolDatabase->scopeList.end(); ++scope) {
        if (scope->isExecutable()) {
            checkScope(&*scope, arrayTypeDefs);
        }
    }
}

void CheckUninitVar::checkScope(const Scope* scope, const std::set<std::string> &arrayTypeDefs)
{
    for (std::list<Variable>::const_iterator i = scope->varlist.begin(); i != scope->varlist.end(); ++i) {
        if ((mTokenizer->isCPP() && i->type() && !i->isPointer() && i->type()->needInitialization != Type::True) ||
            i->isStatic() || i->isExtern() || i->isReference())
            continue;

        // don't warn for try/catch exception variable
        if (i->isThrow())
            continue;

        if (match3(i->nameToken()->next()))
            continue;

        if (match4(i->nameToken())) { // Variable is initialized, but Rhs might be not
            checkRhs(i->nameToken(), *i, NO_ALLOC, 0U, emptyString);
            continue;
        }
        if (match5(i->nameToken()) && match6(i->nameToken()->linkAt(2))) { // Function pointer is initialized, but Rhs might be not
            checkRhs(i->nameToken()->linkAt(2)->next(), *i, NO_ALLOC, 0U, emptyString);
            continue;
        }

        if (i->isArray() || i->isPointerToArray()) {
            const Token *tok = i->nameToken()->next();
            if (i->isPointerToArray())
                tok = tok->next();
            while (match7(tok->link()))
                tok = tok->link()->next();
            if (match8(tok->link()))
                continue;
        }

        bool stdtype = mTokenizer->isC() && arrayTypeDefs.find(i->typeStartToken()->str()) == arrayTypeDefs.end();
        const Token* tok = i->typeStartToken();
        for (; tok != i->nameToken() && tok->str() != MatchCompiler::makeConstString("<"); tok = tok->next()) {
            if (tok->isStandardType() || tok->isEnumType())
                stdtype = true;
        }
        if (i->isArray() && !stdtype)
            continue;

        while (tok && tok->str() != MatchCompiler::makeConstString(";"))
            tok = tok->next();
        if (!tok)
            continue;

        if (tok->astParent() && match9(tok->astParent()->previous()) &&
            checkLoopBody(tok->astParent()->link()->next(), *i, i->isArray() ? ARRAY : NO_ALLOC, emptyString, true))
            continue;

        if (i->isArray()) {
            Alloc alloc = ARRAY;
            const std::map<unsigned int, VariableValue> variableValue;
            checkScopeForVariable(tok, *i, nullptr, nullptr, &alloc, emptyString, variableValue);
            continue;
        }
        if (stdtype || i->isPointer()) {
            Alloc alloc = NO_ALLOC;
            const std::map<unsigned int, VariableValue> variableValue;
            checkScopeForVariable(tok, *i, nullptr, nullptr, &alloc, emptyString, variableValue);
        }
        if (i->type())
            checkStruct(tok, *i);
    }

    if (scope->function) {
        for (unsigned int i = 0; i < scope->function->argCount(); i++) {
            const Variable *arg = scope->function->getArgumentVar(i);
            if (arg && arg->declarationId() && match10(arg->typeStartToken())) {
                // Treat the pointer as initialized until it is assigned by malloc
                for (const Token *tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
                    if (match11(tok, arg->declarationId()) &&
                        mSettings->library.returnuninitdata.count(tok->strAt(3)) == 1U) {
                        if (arg->typeStartToken()->strAt(-1) == MatchCompiler::makeConstString("struct") || (arg->type() && arg->type()->isStructType()))
                            checkStruct(tok, *arg);
                        else if (arg->typeStartToken()->isStandardType() || arg->typeStartToken()->isEnumType()) {
                            Alloc alloc = NO_ALLOC;
                            const std::map<unsigned int, VariableValue> variableValue;
                            checkScopeForVariable(tok->next(), *arg, nullptr, nullptr, &alloc, emptyString, variableValue);
                        }
                    }
                }
            }
        }
    }
}

void CheckUninitVar::checkStruct(const Token *tok, const Variable &structvar)
{
    const Token *typeToken = structvar.typeStartToken();
    const SymbolDatabase * symbolDatabase = mTokenizer->getSymbolDatabase();
    for (std::size_t j = 0U; j < symbolDatabase->classAndStructScopes.size(); ++j) {
        const Scope *scope2 = symbolDatabase->classAndStructScopes[j];
        if (scope2->className == typeToken->str() && scope2->numConstructors == 0U) {
            for (std::list<Variable>::const_iterator it = scope2->varlist.begin(); it != scope2->varlist.end(); ++it) {
                const Variable &var = *it;

                if (var.isStatic() || var.hasDefault() || var.isArray() ||
                    (!mTokenizer->isC() && var.isClass() && (!var.type() || var.type()->needInitialization != Type::True)))
                    continue;

                // is the variable declared in a inner union?
                bool innerunion = false;
                for (auto it2 = scope2->nestedList.cbegin(); it2 != scope2->nestedList.cend(); ++it2) {
                    const Scope &innerScope = **it2;
                    if (innerScope.type == Scope::eUnion) {
                        if (var.typeStartToken()->linenr() >= innerScope.bodyStart->linenr() &&
                            var.typeStartToken()->linenr() <= innerScope.bodyEnd->linenr()) {
                            innerunion = true;
                            break;
                        }
                    }
                }

                if (!innerunion) {
                    Alloc alloc = NO_ALLOC;
                    const Token *tok2 = tok;
                    if (tok->str() == MatchCompiler::makeConstString("}"))
                        tok2 = tok2->next();
                    const std::map<unsigned int, VariableValue> variableValue;
                    checkScopeForVariable(tok2, structvar, nullptr, nullptr, &alloc, var.name(), variableValue);
                }
            }
        }
    }
}

static VariableValue operator!(VariableValue v)
{
    v.notEqual = !v.notEqual;
    return v;
}
static bool operator==(const VariableValue & v, MathLib::bigint i)
{
    return v.notEqual ? (i != v.value) : (i == v.value);
}
static bool operator!=(const VariableValue & v, MathLib::bigint i)
{
    return v.notEqual ? (i == v.value) : (i != v.value);
}

static void conditionAlwaysTrueOrFalse(const Token *tok, const std::map<unsigned int, VariableValue> &variableValue, bool *alwaysTrue, bool *alwaysFalse)
{
    if (!tok)
        return;

    if (tok->isName() || tok->str() == MatchCompiler::makeConstString(".")) {
        while (tok && tok->str() == MatchCompiler::makeConstString("."))
            tok = tok->astOperand2();
        const std::map<unsigned int, VariableValue>::const_iterator it = variableValue.find(tok ? tok->varId() : ~0U);
        if (it != variableValue.end()) {
            *alwaysTrue = (it->second != 0LL);
            *alwaysFalse = (it->second == 0LL);
        }
    }

    else if (tok->isComparisonOp()) {
        if (tok->values().size() == 1U && tok->values().front().isKnown()) {
            if (tok->values().front().intvalue)
                *alwaysTrue = true;
            else
                *alwaysFalse = true;
            return;
        }

        const Token *vartok, *numtok;
        if (tok->astOperand2() && tok->astOperand2()->isNumber()) {
            vartok = tok->astOperand1();
            numtok = tok->astOperand2();
        } else if (tok->astOperand1() && tok->astOperand1()->isNumber()) {
            vartok = tok->astOperand2();
            numtok = tok->astOperand1();
        } else {
            return;
        }

        while (vartok && vartok->str() == MatchCompiler::makeConstString("."))
            vartok = vartok->astOperand2();

        const std::map<unsigned int, VariableValue>::const_iterator it = variableValue.find(vartok ? vartok->varId() : ~0U);
        if (it == variableValue.end())
            return;

        if (tok->str() == MatchCompiler::makeConstString("=="))
            *alwaysTrue  = (it->second == MathLib::toLongNumber(numtok->str()));
        else if (tok->str() == MatchCompiler::makeConstString("!="))
            *alwaysTrue  = (it->second != MathLib::toLongNumber(numtok->str()));
        else
            return;
        *alwaysFalse = !(*alwaysTrue);
    }

    else if (tok->str() == MatchCompiler::makeConstString("!")) {
        bool t=false,f=false;
        conditionAlwaysTrueOrFalse(tok->astOperand1(), variableValue, &t, &f);
        if (t||f) {
            *alwaysTrue = !t;
            *alwaysFalse = !f;
        }
    }

    else if (tok->str() == MatchCompiler::makeConstString("||")) {
        bool t1=false, f1=false;
        conditionAlwaysTrueOrFalse(tok->astOperand1(), variableValue, &t1, &f1);
        bool t2=false, f2=false;
        if (!t1)
            conditionAlwaysTrueOrFalse(tok->astOperand2(), variableValue, &t2, &f2);
        *alwaysTrue = (t1 || t2);
        *alwaysFalse = (f1 && f2);
    }

    else if (tok->str() == MatchCompiler::makeConstString("&&")) {
        bool t1=false, f1=false;
        conditionAlwaysTrueOrFalse(tok->astOperand1(), variableValue, &t1, &f1);
        bool t2=false, f2=false;
        if (!f1)
            conditionAlwaysTrueOrFalse(tok->astOperand2(), variableValue, &t2, &f2);
        *alwaysTrue = (t1 && t2);
        *alwaysFalse = (f1 || f2);
    }
}

static bool isVariableUsed(const Token *tok, const Variable& var)
{
    if (!tok)
        return false;
    if (tok->str() == MatchCompiler::makeConstString("&") && !tok->astOperand2())
        return false;
    if (tok->isConstOp())
        return isVariableUsed(tok->astOperand1(),var) || isVariableUsed(tok->astOperand2(),var);
    if (tok->varId() != var.declarationId())
        return false;
    if (!var.isArray())
        return true;

    const Token *parent = tok->astParent();
    while (match12(parent))
        parent = parent->astParent();
    // no dereference, then array is not "used"
    if (!match13(parent))
        return false;
    const Token *parent2 = parent->astParent();
    // TODO: handle function calls. There is a TODO assertion in TestUninitVar::uninitvar_arrays
    return !parent2 || parent2->isConstOp() || (parent2->str() == MatchCompiler::makeConstString("=") && parent2->astOperand2() == parent);
}

bool CheckUninitVar::checkScopeForVariable(const Token *tok, const Variable& var, bool * const possibleInit, bool * const noreturn, Alloc* const alloc, const std::string &membervar, std::map<unsigned int, VariableValue> variableValue)
{
    const bool suppressErrors(possibleInit && *possibleInit);  // Assume that this is a variable delaratkon, rather than a fundef
    const bool printDebug = mSettings->debugwarnings;

    if (possibleInit)
        *possibleInit = false;

    unsigned int number_of_if = 0;

    if (var.declarationId() == 0U)
        return true;

    for (; tok; tok = tok->next()) {
        // End of scope..
        if (tok->str() == MatchCompiler::makeConstString("}")) {
            if (number_of_if && possibleInit)
                *possibleInit = true;

            // might be a noreturn function..
            if (mTokenizer->IsScopeNoReturn(tok)) {
                if (noreturn)
                    *noreturn = true;
                return false;
            }

            break;
        }

        // Unconditional inner scope or try..
        if (tok->str() == MatchCompiler::makeConstString("{") && match14(tok->previous())) {
            if (checkScopeForVariable(tok->next(), var, possibleInit, noreturn, alloc, membervar, variableValue))
                return true;
            tok = tok->link();
            continue;
        }

        // assignment with nonzero constant..
        if (match15(tok->previous()))
            variableValue[tok->varId()] = !VariableValue(0);

        // Inner scope..
        else if (match16(tok)) {
            bool alwaysTrue = false;
            bool alwaysFalse = false;

            conditionAlwaysTrueOrFalse(tok->next()->astOperand2(), variableValue, &alwaysTrue, &alwaysFalse);

            // initialization / usage in condition..
            if (!alwaysTrue && checkIfForWhileHead(tok->next(), var, suppressErrors, bool(number_of_if == 0), *alloc, membervar))
                return true;

            // checking if a not-zero variable is zero => bail out
            unsigned int condVarId = 0;
            VariableValue condVarValue(0);
            const Token *condVarTok = nullptr;
            if (alwaysFalse)
                ;
            else if (match16(tok) &&
                     astIsVariableComparison(tok->next()->astOperand2(), "!=", "0", &condVarTok)) {
                const std::map<unsigned int,VariableValue>::const_iterator it = variableValue.find(condVarTok->varId());
                if (it != variableValue.end() && it->second != 0)
                    return true;   // this scope is not fully analysed => return true
                else {
                    condVarId = condVarTok->varId();
                    condVarValue = !VariableValue(0);
                }
            } else if (match16(tok) && match17(tok->next()->astOperand2())) {
                const Token *condition = tok->next()->astOperand2();
                const Token *lhs = condition->astOperand1();
                const Token *rhs = condition->astOperand2();
                const Token *vartok = rhs && rhs->isNumber() ? lhs : rhs;
                const Token *numtok = rhs && rhs->isNumber() ? rhs : lhs;
                while (match18(vartok))
                    vartok = vartok->astOperand2();
                if (vartok && vartok->varId() && numtok) {
                    const std::map<unsigned int,VariableValue>::const_iterator it = variableValue.find(vartok->varId());
                    if (it != variableValue.end() && it->second != MathLib::toLongNumber(numtok->str()))
                        return true;   // this scope is not fully analysed => return true
                    else {
                        condVarId = vartok->varId();
                        condVarValue = VariableValue(MathLib::toLongNumber(numtok->str()));
                        if (condition->str() == MatchCompiler::makeConstString("!="))
                            condVarValue = !condVarValue;
                    }
                }
            }

            // goto the {
            tok = tok->next()->link()->next();

            if (!tok)
                break;
            if (tok->str() == MatchCompiler::makeConstString("{")) {
                bool possibleInitIf((!alwaysTrue && number_of_if > 0) || suppressErrors);
                bool noreturnIf = false;
                const bool initif = !alwaysFalse && checkScopeForVariable(tok->next(), var, &possibleInitIf, &noreturnIf, alloc, membervar, variableValue);

                // bail out for such code:
                //    if (a) x=0;    // conditional initialization
                //    if (b) return; // cppcheck doesn't know if b can be false when a is false.
                //    x++;           // it's possible x is always initialized
                if (!alwaysTrue && noreturnIf && number_of_if > 0) {
                    if (printDebug) {
                        std::string condition;
                        for (const Token *tok2 = tok->linkAt(-1); tok2 != tok; tok2 = tok2->next()) {
                            condition += tok2->str();
                            if (tok2->isName() && tok2->next()->isName())
                                condition += ' ';
                        }
                        reportError(tok, Severity::debug, "debug", "bailout uninitialized variable checking for '" + var.name() + "'. can't determine if this condition can be false when previous condition is false: " + condition);
                    }
                    return true;
                }

                if (alwaysTrue && (initif || noreturnIf))
                    return true;

                std::map<unsigned int, VariableValue> varValueIf;
                if (!alwaysFalse && !initif && !noreturnIf) {
                    for (const Token *tok2 = tok; tok2 && tok2 != tok->link(); tok2 = tok2->next()) {
                        if (match19(tok2))
                            varValueIf[tok2->next()->varId()] = !VariableValue(0);
                        else if (match20(tok2))
                            varValueIf[tok2->next()->varId()] = VariableValue(MathLib::toLongNumber(tok2->strAt(3)));
                    }
                }

                if (initif && condVarId > 0U)
                    variableValue[condVarId] = !condVarValue;

                // goto the }
                tok = tok->link();

                if (!match21(tok)) {
                    if (initif || possibleInitIf) {
                        ++number_of_if;
                        if (number_of_if >= 2)
                            return true;
                    }
                } else {
                    // goto the {
                    tok = tok->tokAt(2);

                    bool possibleInitElse((!alwaysFalse && number_of_if > 0) || suppressErrors);
                    bool noreturnElse = false;
                    const bool initelse = !alwaysTrue && checkScopeForVariable(tok->next(), var, &possibleInitElse, &noreturnElse, alloc, membervar, variableValue);

                    std::map<unsigned int, VariableValue> varValueElse;
                    if (!alwaysTrue && !initelse && !noreturnElse) {
                        for (const Token *tok2 = tok; tok2 && tok2 != tok->link(); tok2 = tok2->next()) {
                            if (match22(tok2))
                                varValueElse[tok2->next()->varId()] = !VariableValue(0);
                            else if (match23(tok2))
                                varValueElse[tok2->next()->varId()] = VariableValue(MathLib::toLongNumber(tok2->strAt(3)));
                        }
                    }

                    if (initelse && condVarId > 0U && !noreturnIf && !noreturnElse)
                        variableValue[condVarId] = condVarValue;

                    // goto the }
                    tok = tok->link();

                    if ((alwaysFalse || initif || noreturnIf) &&
                        (alwaysTrue || initelse || noreturnElse))
                        return true;

                    if (initif || initelse || possibleInitElse)
                        ++number_of_if;
                    if (!initif && !noreturnIf)
                        variableValue.insert(varValueIf.begin(), varValueIf.end());
                    if (!initelse && !noreturnElse)
                        variableValue.insert(varValueElse.begin(), varValueElse.end());
                }
            }
        }

        // = { .. }
        else if (match24(tok)) {
            // end token
            const Token *end = tok->next()->link();

            // If address of variable is taken in the block then bail out
            if (var.isPointer() || var.isArray()) {
                if (findmatch25(tok->tokAt(2), end, var.declarationId()) )
                    return true;
            } else if (findmatch26(tok->tokAt(2), end, var.declarationId()) ) {
                return true;
            }

            // Skip block
            tok = end;
            continue;
        }

        // skip sizeof / offsetof
        if (match27(tok))
            tok = tok->next()->link();

        // for/while..
        else if (match28(tok) || match29(tok)) {
            const bool forwhile = match28(tok);

            // is variable initialized in for-head?
            if (forwhile && checkIfForWhileHead(tok->next(), var, tok->str() == MatchCompiler::makeConstString("for"), false, *alloc, membervar))
                return true;

            // goto the {
            const Token *tok2 = forwhile ? tok->next()->link()->next() : tok->next();

            if (tok2 && tok2->str() == MatchCompiler::makeConstString("{")) {
                const bool init = checkLoopBody(tok2, var, *alloc, membervar, (number_of_if > 0) || suppressErrors);

                // variable is initialized in the loop..
                if (init)
                    return true;

                // is variable used in for-head?
                bool initcond = false;
                if (!suppressErrors) {
                    const Token *startCond = forwhile ? tok->next() : tok->next()->link()->tokAt(2);
                    initcond = checkIfForWhileHead(startCond, var, false, bool(number_of_if == 0), *alloc, membervar);
                }

                // goto "}"
                tok = tok2->link();

                // do-while => goto ")"
                if (!forwhile) {
                    // Assert that the tokens are '} while ('
                    if (!match30(tok)) {
                        if (printDebug)
                            reportError(tok,Severity::debug,"","assertion failed '} while ('");
                        break;
                    }

                    // Goto ')'
                    tok = tok->linkAt(2);

                    if (!tok)
                        // bailout : invalid code / bad tokenizer
                        break;

                    if (initcond)
                        // variable is initialized in while-condition
                        return true;
                }
            }
        }

        // Unknown or unhandled inner scope
        else if (match32(tok) || (match31(tok) && tok->str() != MatchCompiler::makeConstString("try"))) {
            if (tok->str() == MatchCompiler::makeConstString("struct") || tok->str() == MatchCompiler::makeConstString("union")) {
                tok = tok->linkAt(1);
                continue;
            }
            return true;
        }

        // bailout if there is ({
        if (match33(tok)) {
            return true;
        }

        // bailout if there is assembler code or setjmp
        if (match34(tok)) {
            return true;
        }

        // bailout if there is a goto label
        if (match35(tok)) {
            return true;
        }

        if (tok->str() == MatchCompiler::makeConstString("?")) {
            if (!tok->astOperand2())
                return true;
            const bool used1 = isVariableUsed(tok->astOperand2()->astOperand1(), var);
            const bool used0 = isVariableUsed(tok->astOperand2()->astOperand2(), var);
            const bool err = (number_of_if == 0) ? (used1 || used0) : (used1 && used0);
            if (err)
                uninitvarError(tok, var.nameToken()->str(), *alloc);

            // Todo: skip expression if there is no error
            return true;
        }

        if (match36(tok)) {
            if (noreturn)
                *noreturn = true;

            tok = tok->next();
            while (tok && tok->str() != MatchCompiler::makeConstString(";")) {
                // variable is seen..
                if (tok->varId() == var.declarationId()) {
                    if (!membervar.empty()) {
                        if (match37(tok) && tok->strAt(2) == membervar)
                            uninitStructMemberError(tok, tok->str() + "." + membervar);
                        else
                            return true;
                    }

                    // Use variable
                    else if (!suppressErrors && isVariableUsage(tok, var.isPointer(), *alloc))
                        uninitvarError(tok, tok->str(), *alloc);

                    else
                        // assume that variable is assigned
                        return true;
                }

                else if (match27(tok))
                    tok = tok->linkAt(1);

                else if (tok->str() == MatchCompiler::makeConstString("?")) {
                    if (!tok->astOperand2())
                        return true;
                    const bool used1 = isVariableUsed(tok->astOperand2()->astOperand1(), var);
                    const bool used0 = isVariableUsed(tok->astOperand2()->astOperand2(), var);
                    const bool err = (number_of_if == 0) ? (used1 || used0) : (used1 && used0);
                    if (err)
                        uninitvarError(tok, var.nameToken()->str(), *alloc);
                    return true;
                }

                tok = tok->next();
            }

            return (noreturn == nullptr);
        }

        // variable is seen..
        if (tok->varId() == var.declarationId()) {
            // calling function that returns uninit data through pointer..
            if (var.isPointer() &&
                match38(tok->next()) &&
                match39(tok->linkAt(3)) &&
                mSettings->library.returnuninitdata.count(tok->strAt(2)) > 0U) {
                *alloc = NO_CTOR_CALL;
                continue;
            }
            if (var.isPointer() && (var.typeStartToken()->isStandardType() || var.typeStartToken()->isEnumType() || (var.type() && var.type()->needInitialization == Type::True)) && match40(tok->next())) {
                *alloc = CTOR_CALL;

                // type has constructor(s)
                if (var.typeScope() && var.typeScope()->numConstructors > 0)
                    return true;

                // standard or enum type: check if new initializes the allocated memory
                if (var.typeStartToken()->isStandardType() || var.typeStartToken()->isEnumType()) {
                    // scalar new with initialization
                    if (match41(tok->next()))
                        return true;

                    // array new
                    if (match42(tok->next())) {
                        const Token* tokClosingBracket=tok->linkAt(4);
                        // array new with initialization
                        if (tokClosingBracket && match43(tokClosingBracket->next()))
                            return true;
                    }
                }

                continue;
            }


            if (!membervar.empty()) {
                if (isMemberVariableAssignment(tok, membervar)) {
                    checkRhs(tok, var, *alloc, number_of_if, membervar);
                    return true;
                }

                if (isMemberVariableUsage(tok, var.isPointer(), *alloc, membervar))
                    uninitStructMemberError(tok, tok->str() + "." + membervar);

                else if (match44(tok->previous()))
                    return true;

            } else {
                // Use variable
                if (!suppressErrors && isVariableUsage(tok, var.isPointer(), *alloc))
                    uninitvarError(tok, tok->str(), *alloc);

                else {
                    if (tok->strAt(1) == MatchCompiler::makeConstString("="))
                        checkRhs(tok, var, *alloc, number_of_if, emptyString);

                    // assume that variable is assigned
                    return true;
                }
            }
        }
    }

    return false;
}

bool CheckUninitVar::checkIfForWhileHead(const Token *startparentheses, const Variable& var, bool suppressErrors, bool isuninit, Alloc alloc, const std::string &membervar)
{
    const Token * const endpar = startparentheses->link();
    if (match45(startparentheses) && startparentheses->tokAt(2)->getValue(0))
        suppressErrors = true;
    for (const Token *tok = startparentheses->next(); tok && tok != endpar; tok = tok->next()) {
        if (tok->varId() == var.declarationId()) {
            if (match46(tok)) {
                if (membervar.empty())
                    return true;
                if (tok->strAt(2) == membervar) {
                    if (isMemberVariableAssignment(tok, membervar))
                        return true;

                    if (!suppressErrors && isMemberVariableUsage(tok, var.isPointer(), alloc, membervar))
                        uninitStructMemberError(tok, tok->str() + "." + membervar);
                }
                continue;
            }

            if (isVariableUsage(tok, var.isPointer(), alloc)) {
                if (suppressErrors)
                    continue;
                uninitvarError(tok, tok->str(), alloc);
            }
            return true;
        }
        if (match47(tok))
            tok = tok->next()->link();
        if ((!isuninit || !membervar.empty()) && tok->str() == MatchCompiler::makeConstString("&&"))
            suppressErrors = true;
    }
    return false;
}

bool CheckUninitVar::checkLoopBody(const Token *tok, const Variable& var, const Alloc alloc, const std::string &membervar, const bool suppressErrors)
{
    const Token *usetok = nullptr;

    assert(tok->str() == MatchCompiler::makeConstString("{"));

    for (const Token * const end = tok->link(); tok != end; tok = tok->next()) {
        if (match48(tok)) {
            tok = tok->next()->link();
            continue;
        }

        if (match49(tok))
            return true;

        if (tok->varId() != var.declarationId())
            continue;

        if (!membervar.empty()) {
            if (isMemberVariableAssignment(tok, membervar)) {
                bool assign = true;
                bool rhs = false;
                // Used for tracking if an ")" is inner or outer
                const Token *rpar = nullptr;
                for (const Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString("="))
                        rhs = true;

                    // Look at inner expressions but not outer expressions
                    if (!rpar && tok2->str() == MatchCompiler::makeConstString("("))
                        rpar = tok2->link();
                    else if (tok2->str() == MatchCompiler::makeConstString(")")) {
                        // No rpar => this is an outer right parenthesis
                        if (!rpar)
                            break;
                        if (rpar == tok2)
                            rpar = nullptr;
                    }

                    if (tok2->str() == MatchCompiler::makeConstString(";") || (!rpar && tok2->str() == MatchCompiler::makeConstString(",")))
                        break;
                    if (rhs && tok2->varId() == var.declarationId() && isMemberVariableUsage(tok2, var.isPointer(), alloc, membervar)) {
                        assign = false;
                        break;
                    }
                }
                if (assign)
                    return true;
            }

            if (isMemberVariableUsage(tok, var.isPointer(), alloc, membervar))
                usetok = tok;
            else if (match44(tok->previous()))
                return true;
        } else {
            if (isVariableUsage(tok, var.isPointer(), alloc))
                usetok = tok;
            else if (tok->strAt(1) == MatchCompiler::makeConstString("=")) {
                // Is var used in rhs?
                bool rhs = false;
                std::stack<const Token *> tokens;
                tokens.push(tok->next()->astOperand2());
                while (!tokens.empty()) {
                    const Token *t = tokens.top();
                    tokens.pop();
                    if (!t)
                        continue;
                    if (t->varId() == var.declarationId()) {
                        // var is used in rhs
                        rhs = true;
                        break;
                    }
                    if (match50(t->previous()))
                        continue;
                    tokens.push(t->astOperand1());
                    tokens.push(t->astOperand2());
                }
                if (!rhs)
                    return true;
            } else {
                return true;
            }
        }
    }

    if (!suppressErrors && usetok) {
        if (membervar.empty())
            uninitvarError(usetok, usetok->str(), alloc);
        else
            uninitStructMemberError(usetok, usetok->str() + "." + membervar);
        return true;
    }

    return false;
}

void CheckUninitVar::checkRhs(const Token *tok, const Variable &var, Alloc alloc, unsigned int number_of_if, const std::string &membervar)
{
    bool rhs = false;
    unsigned int indent = 0;
    while (nullptr != (tok = tok->next())) {
        if (tok->str() == MatchCompiler::makeConstString("="))
            rhs = true;
        else if (rhs && tok->varId() == var.declarationId()) {
            if (membervar.empty() && isVariableUsage(tok, var.isPointer(), alloc))
                uninitvarError(tok, tok->str(), alloc);
            else if (!membervar.empty() && isMemberVariableUsage(tok, var.isPointer(), alloc, membervar))
                uninitStructMemberError(tok, tok->str() + "." + membervar);
            else if (match51(tok))
                break;
        } else if (tok->str() == MatchCompiler::makeConstString(";") || (indent==0 && tok->str() == MatchCompiler::makeConstString(",")))
            break;
        else if (tok->str() == MatchCompiler::makeConstString("("))
            ++indent;
        else if (tok->str() == MatchCompiler::makeConstString(")")) {
            if (indent == 0)
                break;
            --indent;
        } else if (tok->str() == MatchCompiler::makeConstString("?") && tok->astOperand2()) {
            const bool used1 = isVariableUsed(tok->astOperand2()->astOperand1(), var);
            const bool used0 = isVariableUsed(tok->astOperand2()->astOperand2(), var);
            const bool err = (number_of_if == 0) ? (used1 || used0) : (used1 && used0);
            if (err)
                uninitvarError(tok, var.nameToken()->str(), alloc);
            break;
        } else if (match50(tok))
            tok = tok->next()->link();
    }
}

bool CheckUninitVar::isVariableUsage(const Token *vartok, bool pointer, Alloc alloc) const
{
    if (alloc == NO_ALLOC && ((match52(vartok->previous())) || (vartok->strAt(-1) == MatchCompiler::makeConstString("]") && vartok->linkAt(-1)->strAt(-1) == MatchCompiler::makeConstString("delete"))))
        return true;

    // Passing variable to typeof/__alignof__
    if (match53(vartok->tokAt(-3)))
        return false;

    // Accessing Rvalue member using "." or "->"
    if (match54(vartok->previous())) {
        // Is struct member passed to function?
        if (!pointer && match55(vartok->previous())) {
            // TODO: there are FN currently:
            // - should only return false if struct member is (or might be) array.
            // - should only return false if function argument is (or might be) non-const pointer or reference
            const Token *tok2 = vartok->next();
            do {
                tok2 = tok2->tokAt(2);
            } while (match56(tok2));
            if (match57(tok2))
                return false;
        } else if (pointer && alloc != CTOR_CALL && match58(vartok)) {
            return true;
        }

        bool assignment = false;
        const Token* parent = vartok->astParent();
        while (parent) {
            if (parent->str() == MatchCompiler::makeConstString("=")) {
                assignment = true;
                break;
            }
            if (alloc != NO_ALLOC && parent->str() == MatchCompiler::makeConstString("(")) {
                if (!mSettings->library.isFunctionConst(parent->strAt(-1), true)) {
                    assignment = true;
                    break;
                }
            }
            parent = parent->astParent();
        }
        if (!assignment)
            return true;
    }

    // Passing variable to function..
    if (match44(vartok->previous()) || match59(vartok->tokAt(-2))) {
        const int use = isFunctionParUsage(vartok, pointer, alloc);
        if (use >= 0)
            return (use>0);
    }

    if (match60(vartok->previous())) {
        if (mTokenizer->isCPP() && alloc == ARRAY && match61(vartok->tokAt(-4)))
            return false;

        if (isLikelyStreamRead(mTokenizer->isCPP(), vartok->previous()))
            return false;

        if (mTokenizer->isCPP() && match62(vartok->previous())) {
            const Token* tok2 = vartok->previous();

            // Looks like stream operator, but could also initialize the variable. Check lhs.
            do {
                tok2 = tok2->astOperand1();
            } while (match62(tok2));
            if (tok2 && tok2->strAt(-1) == MatchCompiler::makeConstString("::"))
                tok2 = tok2->previous();
            if (tok2 && (match63(tok2->previous()) || (tok2->variable() && tok2->variable()->isStlType()) || tok2->isStandardType() || tok2->isEnumType()))
                return true;

            const Variable *var = vartok->tokAt(-2)->variable();
            return (var && (var->typeStartToken()->isStandardType() || var->typeStartToken()->isEnumType()));
        }

        // is there something like: ; "*((&var ..expr.. ="  => the variable is assigned
        if (vartok->previous()->str() == MatchCompiler::makeConstString("&") && !vartok->previous()->astOperand2())
            return false;

        // bailout to avoid fp for 'int x = 2 + x();' where 'x()' is a unseen preprocessor macro (seen in linux)
        if (!pointer && vartok->next() && vartok->next()->str() == MatchCompiler::makeConstString("("))
            return false;

        if (vartok->previous()->str() != MatchCompiler::makeConstString("&") || !match64(vartok->tokAt(-2))) {
            if (alloc != NO_ALLOC && vartok->previous()->str() == MatchCompiler::makeConstString("*")) {
                // TestUninitVar::isVariableUsageDeref()
                const Token *parent = vartok->previous()->astParent();
                if (parent && parent->str() == MatchCompiler::makeConstString("=") && parent->astOperand1() == vartok->previous())
                    return false;
                if (vartok->variable() && vartok->variable()->dimensions().size() >= 2)
                    return false;
                return true;
            }
            return alloc == NO_ALLOC;
        }
    }

    if (alloc == NO_ALLOC && match65(vartok->previous())) {
        // taking reference?
        const Token *prev = vartok->tokAt(-2);
        while (match66(prev))
            prev = prev->previous();
        if (!match67(prev))
            return true;
    }

    bool unknown = false;
    if (pointer && alloc == NO_ALLOC && CheckNullPointer::isPointerDeRef(vartok, unknown)) {
        // function parameter?
        bool functionParameter = false;
        if (match68(vartok->tokAt(-2)) || vartok->previous()->str() == MatchCompiler::makeConstString(","))
            functionParameter = true;

        // if this is not a function parameter report this dereference as variable usage
        if (!functionParameter)
            return true;
    } else if (alloc != NO_ALLOC && match69(vartok)) {
        const Token *parent = vartok->next()->astParent();
        while (match70(parent))
            parent = parent->astParent();
        if (match67(parent) && !parent->astOperand2())
            return false;
        if (parent && match71(parent->previous()))
            return true;
        if (match72(parent))
            return false;
        return true;
    }

    if (mTokenizer->isCPP() && match62(vartok->next())) {
        // Is this calculation done in rhs?
        const Token *tok = vartok;
        while (match73(tok))
            tok = tok->previous();
        if (match74(tok))
            return false;

        // Is variable a known POD type then this is a variable usage,
        // otherwise we assume it's not.
        return (vartok->valueType() && vartok->valueType()->isIntegral());
    }

    if (alloc == NO_ALLOC && vartok->next() && vartok->next()->isOp() && !vartok->next()->isAssignmentOp())
        return true;

    if (vartok->strAt(1) == MatchCompiler::makeConstString("]"))
        return true;

    return false;
}

/***
 * Is function parameter "used" so a "usage of uninitialized variable" can
 * be written? If parameter is passed "by value" then it is "used". If it
 * is passed "by reference" then it is not necessarily "used".
 * @return  -1 => unknown   0 => not used   1 => used
 */
int CheckUninitVar::isFunctionParUsage(const Token *vartok, bool pointer, Alloc alloc) const
{
    if (!match75(vartok->previous()) && !match76(vartok->tokAt(-2)))
        return -1;

    // locate start parentheses in function call..
    unsigned int argumentNumber = 0;
    const Token *start = vartok;
    while (start && !match77(start)) {
        if (start->str() == MatchCompiler::makeConstString(")"))
            start = start->link();
        else if (start->str() == MatchCompiler::makeConstString(","))
            ++argumentNumber;
        start = start->previous();
    }
    if (!start)
        return -1;

    if (match32(start->link()) && match78(start->previous()))
        return (!pointer || alloc == NO_ALLOC);

    // is this a function call?
    if (match68(start->previous())) {
        const bool address(vartok->previous()->str() == MatchCompiler::makeConstString("&"));
        const bool array(vartok->variable() && vartok->variable()->isArray());
        // check how function handle uninitialized data arguments..
        const Function *func = start->previous()->function();
        if (func) {
            const Variable *arg = func->getArgumentVar(argumentNumber);
            if (arg) {
                const Token *argStart = arg->typeStartToken();
                if (!address && !array && match79(argStart))
                    return 1;
                if (pointer && !address && alloc == NO_ALLOC && match10(argStart))
                    return 1;
                while (argStart->previous() && argStart->previous()->isName())
                    argStart = argStart->previous();
                if (match80(argStart)) {
                    // If it's a record it's ok to pass a partially uninitialized struct.
                    if (vartok->variable() && vartok->variable()->valueType() && vartok->variable()->valueType()->type == ValueType::Type::RECORD)
                        return -1;
                    return 1;
                }
                if ((pointer || address) && alloc == NO_ALLOC && match81(argStart))
                    return 1;
                if ((pointer || address) && match82(argStart) && match83(argStart->linkAt(3)))
                    return 1;
            }

        } else if (match84(start->previous())) {
            // control-flow statement reading the variable "by value"
            return alloc == NO_ALLOC;
        } else {
            const bool isnullbad = mSettings->library.isnullargbad(start->previous(), argumentNumber + 1);
            if (pointer && !address && isnullbad && alloc == NO_ALLOC)
                return 1;
            const bool isuninitbad = mSettings->library.isuninitargbad(start->previous(), argumentNumber + 1);
            if (alloc != NO_ALLOC)
                return isnullbad && isuninitbad;
            return isuninitbad && (!address || isnullbad);
        }
    }

    // unknown
    return -1;
}

bool CheckUninitVar::isMemberVariableAssignment(const Token *tok, const std::string &membervar) const
{
    if (match46(tok) && tok->strAt(2) == membervar) {
        if (match85(tok->tokAt(3)))
            return true;
        else if (match86(tok->tokAt(-2)))
            return true;
        else if (isLikelyStreamRead(mTokenizer->isCPP(), tok->previous()))
            return true;
        else if ((tok->previous() && tok->previous()->isConstOp()) || match87(tok->previous()))
            ; // member variable usage
        else if (tok->tokAt(3)->isConstOp())
            ; // member variable usage
        else if (match88(tok->previous()) &&
                 1 == isFunctionParUsage(tok, false, NO_ALLOC)) {
            return false;
        } else
            return true;
    } else if (tok->strAt(1) == MatchCompiler::makeConstString("="))
        return true;
    else if (match89(tok)) {
        const Token *ftok = tok->tokAt(2);
        if (!ftok->function() || !ftok->function()->isConst())
            // TODO: Try to determine if membervar is assigned in method
            return true;
    } else if (tok->strAt(-1) == MatchCompiler::makeConstString("&")) {
        if (match90(tok->tokAt(-2))) {
            // locate start parentheses in function call..
            unsigned int argumentNumber = 0;
            const Token *ftok = tok;
            while (ftok && !match77(ftok)) {
                if (ftok->str() == MatchCompiler::makeConstString(")"))
                    ftok = ftok->link();
                else if (ftok->str() == MatchCompiler::makeConstString(","))
                    ++argumentNumber;
                ftok = ftok->previous();
            }

            // is this a function call?
            ftok = ftok ? ftok->previous() : nullptr;
            if (match68(ftok)) {
                // check how function handle uninitialized data arguments..
                const Function *function = ftok->function();
                const Variable *arg      = function ? function->getArgumentVar(argumentNumber) : nullptr;
                const Token *argStart    = arg ? arg->typeStartToken() : nullptr;
                while (argStart && argStart->previous() && argStart->previous()->isName())
                    argStart = argStart->previous();
                if (match91(argStart))
                    return false;
            }

            else if (ftok && match92(ftok->previous()))
                return false;
        }
        return true;
    }
    return false;
}

bool CheckUninitVar::isMemberVariableUsage(const Token *tok, bool isPointer, Alloc alloc, const std::string &membervar) const
{
    if (match88(tok->previous()) &&
        tok->strAt(2) == membervar) {
        const int use = isFunctionParUsage(tok, isPointer, alloc);
        if (use == 1)
            return true;
    }

    if (isMemberVariableAssignment(tok, membervar))
        return false;

    if (match46(tok) && tok->strAt(2) == membervar && !(tok->tokAt(-2)->variable() && tok->tokAt(-2)->variable()->isReference()))
        return true;
    else if (!isPointer && match44(tok->previous()) && isVariableUsage(tok, isPointer, alloc))
        return true;

    else if (!isPointer && match93(tok->previous()))
        return true;

    // = *(&var);
    else if (!isPointer &&
             match67(tok->astParent()) &&
             match94(tok->astParent()->astParent()) &&
             match95(tok->astParent()->astParent()->astParent()) &&
             tok->astParent()->astParent()->astParent()->astOperand2() == tok->astParent()->astParent())
        return true;

    else if (mSettings->experimental &&
             !isPointer &&
             match59(tok->tokAt(-2)) &&
             isVariableUsage(tok, isPointer, alloc))
        return true;

    return false;
}

void CheckUninitVar::uninitstringError(const Token *tok, const std::string &varname, bool strncpy_)
{
    reportError(tok, Severity::error, "uninitstring", "$symbol:" + varname + "\nDangerous usage of '$symbol'" + (strncpy_ ? " (strncpy doesn't always null-terminate it)." : " (not null-terminated)."), CWE676, false);
}

void CheckUninitVar::uninitdataError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::error, "uninitdata", "$symbol:" + varname + "\nMemory is allocated but not initialized: $symbol", CWE908, false);
}

void CheckUninitVar::uninitvarError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::error, "uninitvar", "$symbol:" + varname + "\nUninitialized variable: $symbol", CWE908, false);
}

void CheckUninitVar::uninitStructMemberError(const Token *tok, const std::string &membername)
{
    reportError(tok,
                Severity::error,
                "uninitStructMember",
                "$symbol:" + membername + "\nUninitialized struct member: $symbol", CWE908, false);
}

void CheckUninitVar::valueFlowUninit()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    // check every executable scope
    for (std::list<Scope>::const_iterator scope = symbolDatabase->scopeList.begin(); scope != symbolDatabase->scopeList.end(); ++scope) {
        if (!scope->isExecutable())
            continue;
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (match50(tok)) {
                tok = tok->linkAt(1);
                continue;
            }
            if (!tok->variable() || tok->values().size() != 1U)
                continue;
            const ValueFlow::Value &v = tok->values().front();
            if (v.valueType != ValueFlow::Value::UNINIT || v.isInconclusive())
                continue;
            if (!isVariableUsage(tok, tok->variable()->isPointer(), NO_ALLOC))
                continue;
            uninitvarError(tok, tok->str());
        }
    }
}

void CheckUninitVar::deadPointer()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    // check every executable scope
    for (std::list<Scope>::const_iterator scope = symbolDatabase->scopeList.begin(); scope != symbolDatabase->scopeList.end(); ++scope) {
        if (!scope->isExecutable())
            continue;
        // Dead pointers..
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (tok->variable() &&
                tok->variable()->isPointer() &&
                isVariableUsage(tok, true, NO_ALLOC)) {
                const Token *alias = tok->getValueTokenDeadPointer();
                if (alias) {
                    deadPointerError(tok,alias);
                }
            }
        }
    }
}

void CheckUninitVar::deadPointerError(const Token *pointer, const Token *alias)
{
    const std::string strpointer(pointer ? pointer->str() : std::string("pointer"));
    const std::string stralias(alias ? alias->expressionString() : std::string("&x"));

    reportError(pointer,
                Severity::error,
                "deadpointer",
                "$symbol:" + strpointer + "\nDead pointer usage. Pointer '$symbol' is dead if it has been assigned '" + stralias + "' at line " + MathLib::toString(alias ? alias->linenr() : 0U) + ".", CWE825, false);
}
