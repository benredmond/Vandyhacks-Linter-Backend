#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: [;{}] %varid% = %var% ;
static bool match1(const Token* tok, const unsigned int varid) {
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
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] %var% =
static bool match2(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: [;{}] const| %type% %var% (
static bool match3(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) {
static bool match4(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: %oror%|&&|?|:
static bool match5(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: ( %name%
static bool match6(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name%
static bool match7(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [&*]
static bool match8(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("&*", tok->str()[0]))
        return false;
    return true;
}
// pattern: &&
static bool match9(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    return true;
}
// pattern: ||
static bool match10(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    return true;
}
// pattern: [{}()[]]
static bool match11(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{}()[]", tok->str()[0]))
        return false;
    return true;
}
// pattern: %num% [,}]
static bool match12(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",}", tok->str()[0]))
        return false;
    return true;
}
// pattern: sizeof (
static bool match13(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% ::|.|[
static bool match14(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: sizeof ( %var% ) / sizeof (
static bool match15(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("/")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: * %varid% )
static bool match16(const Token* tok, const unsigned int varid) {
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
    return true;
}
// pattern: %varid% [ 0 ] )
static bool match17(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %var% )
static bool match18(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: false|true
static bool match19(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("false")) || (tok->str()==MatchCompiler::makeConstString("true"))))
        return false;
    return true;
}
// pattern: [(,] NULL [,)]
static bool match20(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("NULL")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: =
static bool match21(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: const %type% %var% [ %num%| ] = {
static bool match22(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (tok && (tok->isNumber()))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: const char %var% [ %num%| ] = %str% ;
static bool match23(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("char")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (tok && (tok->isNumber()))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: if (
static bool match24(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: } else { if (
static bool match25(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% =
static bool match26(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: ++|--|&
static bool match27(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: & %name% =
static bool match28(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: [(,]
static bool match29(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    return true;
}
// pattern: !!* %name% =
static bool match30(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("*"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: [;{}] %name% ++|-- ;
static bool match31(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] ++|-- %name% ;
static bool match32(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ++|-- %name%
static bool match33(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% ++|--
static bool match34(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: [;{}] %var% %assign%
static bool match35(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: sizeof|.
static bool match36(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sizeof")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: %name%|.
static bool match37(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: [?:]
static bool match38(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("?:", tok->str()[0]))
        return false;
    return true;
}
// pattern: while (
static bool match39(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: } while (
static bool match40(const Token* tok) {
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
// pattern: do {
static bool match41(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("do")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ) { break
static bool match42(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("break")))
        return false;
    return true;
}
// pattern: sizeof|typeof|typeid (
static bool match43(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sizeof")) || (tok->str()==MatchCompiler::makeConstString("typeof")) || (tok->str()==MatchCompiler::makeConstString("typeid"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [;{}] %name% :
static bool match44(const Token* tok) {
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
// pattern: %varid%
template<class T> static T * findmatch45(T * start_tok, const Token * end, unsigned int varid) {
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
// pattern: %name% = %num% ;
static bool match46(const Token* tok) {
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
// pattern: do
static bool match47(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("do")))
        return false;
    return true;
}
// pattern: for|while (
static bool match48(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: )|else|do {
static bool match49(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("else")) || (tok->str()==MatchCompiler::makeConstString("do"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: if|for|while (
static bool match50(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: return|break|continue|goto
static bool match51(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("goto"))))
        return false;
    return true;
}
// pattern: %name% (
static bool match52(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: if|while ( %name% %oror%|&&|)
static bool match53(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: %oror%|&& %name% %oror%|&&|)
static bool match54(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: [!?]
static bool match55(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("!?", tok->str()[0]))
        return false;
    return true;
}
// pattern: for (
static bool match56(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: <|>
static bool match57(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: <=|>=
static bool match58(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString(">="))))
        return false;
    return true;
}
// pattern: {
static bool match59(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: return|continue|break|throw|goto
template<class T> static T * findmatch60(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("goto"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: else|) {
static bool match61(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("else")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } else {
static bool match62(const Token* tok) {
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
// pattern: else {
static bool match63(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: = [
static bool match64(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ) !!{
static bool match65(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("{"))
        return false;
    return true;
}
// pattern: for|while
static bool match66(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    return true;
}
// pattern: %oror%|&&|?|;
static bool match67(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: %varid% !!=
template<class T> static T * findmatch68(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("="))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %op%
static bool match69(const Token* tok) {
    if (!tok || !tok->isOp())
        return false;
    return true;
}
// pattern: ;
static bool match70(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %varid% =
static bool match71(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: [;}] break ;
static bool match72(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("break")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: assert|ASSERT (
static bool match73(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("assert")) || (tok->str()==MatchCompiler::makeConstString("ASSERT"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ;
static bool match74(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: break|continue|goto
static bool match75(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("goto"))))
        return false;
    return true;
}
// pattern: !!)
static bool match76(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString(")"))
        return false;
    return true;
}
// pattern: return|throw
static bool match77(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: !!* %name% %assign%
static bool match78(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("*"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: sizeof|typeof|typeid
static bool match79(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sizeof")) || (tok->str()==MatchCompiler::makeConstString("typeof")) || (tok->str()==MatchCompiler::makeConstString("typeid"))))
        return false;
    return true;
}
// pattern: ++|--|?|:|;|,
static bool match80(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: %oror%|&&|:
static bool match81(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: ++|--
static bool match82(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: ] (
static bool match83(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: std :: move ( %var% )
static bool match84(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("move")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: std :: forward <
static bool match85(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("forward")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > ( %var% )
static bool match86(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %varid% . %name% (
static bool match87(const Token* tok, const unsigned int varid) {
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% . reset|clear (
static bool match88(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("reset")) || (tok->str()==MatchCompiler::makeConstString("clear"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: bool|_Bool
static bool match89(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("_Bool"))))
        return false;
    return true;
}
// pattern: = & %var% ;
static bool match90(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ==|!=|>=|<=
static bool match91(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("<="))))
        return false;
    return true;
}
// pattern: >
static bool match92(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    return true;
}
// pattern: <
static bool match93(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: %oror%|&&
static bool match94(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    return true;
}
// pattern: if|while ( %var% [)=]
static bool match95(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(")=", tok->str()[0]))
        return false;
    return true;
}
// pattern: ==|>=|<=|!
static bool match96(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString("!"))))
        return false;
    return true;
}
// pattern: %name%|!=
static bool match97(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    return true;
}
// pattern: ++|--|=
static bool match98(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: %varid%
static bool match99(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: if|while (
static bool match100(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ==|>=|<=|!|>|<
static bool match101(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString("!")) || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: %name%|!=|>|<
static bool match102(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: ==|!
static bool match103(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!"))))
        return false;
    return true;
}
// pattern: ) ; }
static bool match104(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: %type%| %var% =
static bool match105(const Token* tok) {
    if (tok && ((tok->isName() && tok->varId()==0U && !tok->isKeyword())))
        tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %var% =
static bool match106(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %num% ;
static bool match107(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name%|%num%|%or%|+|-|*|/|&|[|]|(
static bool match108(const Token* tok) {
    if (!tok || !(tok->isName() || tok->isNumber() || (tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") ) || (tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-")) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("/")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: %varid% <|<=|!=
static bool match109(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    return true;
}
// pattern: %num% ;|%oror%
static bool match110(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    return true;
}
// pattern: ; %varid% ++ ) {
static bool match111(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("++")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ; ++ %varid% ) {
static bool match112(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("++")))
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
// pattern: ) (
static bool match113(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: continue|break|return
template<class T> static T * findmatch114(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: switch ( %var% ) {
static bool match115(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("switch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: case %num% :
static bool match116(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: ;| case
static bool match117(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString(";"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    return true;
}
// pattern: break ;
static bool match118(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("break")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ;| case %num% :
static bool match119(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString(";"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: arg1
template<class T> static T * findmatch120(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("arg1")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: strlen ( arg1 )
static bool match121(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strlen")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("arg1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [!<>=] =
static bool match122(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("!<>=", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %comp%|%oror%|&&|!
static bool match123(const Token* tok) {
    if (!tok || !(tok->isComparisonOp() || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("!"))))
        return false;
    return true;
}
// pattern: %var% = %num%|%str% [,)]
static bool match124(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || (tok->tokType()==Token::eString)))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: { return
static bool match125(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    return true;
}
// pattern: return
template<class T> static T * findmatch126(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %type% %name% ,|)
static bool match127(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: [;{}] %type%
static bool match128(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %name%|::|*
static bool match129(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: %var% ;
static bool match130(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %varid% ; %varid% =
static bool match131(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %oror%|&&|?
static bool match132(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("?"))))
        return false;
    return true;
}
// pattern: [{}]
static bool match133(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: return|break|continue
static bool match134(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    return true;
}
// pattern: %name% . %name% (
static bool match135(const Token* tok) {
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
// pattern: %name% ;
static bool match136(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] %var% = %str% ;
static bool match137(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: !|==|!=
static bool match138(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("!")) || (tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    return true;
}
// pattern: %name%|%num% %name%|%num%
static bool match139(const Token* tok) {
    if (!tok || !(tok->isName() || tok->isNumber()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || tok->isNumber()))
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

/**
 * @brief This is the ValueFlow component in Cppcheck.
 *
 * Each @sa Token in the token list has a list of values. These are
 * the "possible" values for the Token at runtime.
 *
 * In the --debug and --debug-normal output you can see the ValueFlow data. For example:
 *
 *     int f()
 *     {
 *         int x = 10;
 *         return 4 * x + 2;
 *     }
 *
 * The --debug-normal output says:
 *
 *     ##Value flow
 *     Line 3
 *       10 always 10
 *     Line 4
 *       4 always 4
 *       * always 40
 *       x always 10
 *       + always 42
 *       2 always 2
 *
 * All value flow analysis is executed in the ValueFlow::setValues() function. The ValueFlow analysis is executed after the tokenizer/ast/symboldatabase/etc..
 * The ValueFlow analysis is done in a series of valueFlow* function calls, where each such function call can only use results from previous function calls.
 * The function calls should be arranged so that valueFlow* that do not require previous ValueFlow information should be first.
 *
 * Type of analysis
 * ================
 *
 * This is "flow sensitive" value flow analysis. We _usually_ track the value for 1 variable at a time.
 *
 * How are calculations handled
 * ============================
 *
 * Here is an example code:
 *
 *   x = 3 + 4;
 *
 * The valueFlowNumber set the values for the "3" and "4" tokens by calling setTokenValue().
 * The setTokenValue() handle the calculations automatically. When both "3" and "4" have values, the "+" can be calculated. setTokenValue() recursively calls itself when parents in calculations can be calculated.
 *
 * Forward / Reverse flow analysis
 * ===============================
 *
 * In forward value flow analysis we know a value and see what happens when we are stepping the program forward. Like normal execution.
 * The valueFlowForward is used in this analysis.
 *
 * In reverse value flow analysis we know the value of a variable at line X. And try to "execute backwards" to determine possible values before line X.
 * The valueFlowReverse is used in this analysis.
 *
 *
 */



#include "valueflow.h"

#include "astutils.h"
#include "errorlogger.h"
#include "library.h"
#include "mathlib.h"
#include "platform.h"
#include "settings.h"
#include "standards.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenlist.h"
#include "utils.h"
#include "path.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <stack>
#include <vector>

namespace {
    struct ProgramMemory {
        std::map<unsigned int, ValueFlow::Value> values;

        void setValue(unsigned int varid, const ValueFlow::Value &value) {
            values[varid] = value;
        }

        bool getIntValue(unsigned int varid, MathLib::bigint* result) const {
            const std::map<unsigned int, ValueFlow::Value>::const_iterator it = values.find(varid);
            const bool found = it != values.end() && it->second.isIntValue();
            if (found)
                *result = it->second.intvalue;
            return found;
        }

        void setIntValue(unsigned int varid, MathLib::bigint value) {
            values[varid] = ValueFlow::Value(value);
        }

        bool getTokValue(unsigned int varid, const Token** result) const {
            const std::map<unsigned int, ValueFlow::Value>::const_iterator it = values.find(varid);
            const bool found = it != values.end() && it->second.isTokValue();
            if (found)
                *result = it->second.tokvalue;
            return found;
        }

        bool hasValue(unsigned int varid) {
            return values.find(varid) != values.end();
        }

        void swap(ProgramMemory &pm) {
            values.swap(pm.values);
        }

        void clear() {
            values.clear();
        }

        bool empty() const {
            return values.empty();
        }
    };
}

static void execute(const Token *expr,
                    ProgramMemory * const programMemory,
                    MathLib::bigint *result,
                    bool *error);

static void bailoutInternal(TokenList *tokenlist, ErrorLogger *errorLogger, const Token *tok, const std::string &what, const std::string &file, int line, const std::string &function)
{
    std::list<ErrorLogger::ErrorMessage::FileLocation> callstack(1, ErrorLogger::ErrorMessage::FileLocation(tok, tokenlist));
    ErrorLogger::ErrorMessage errmsg(callstack, tokenlist->getSourceFilePath(), Severity::debug,
                                     Path::stripDirectoryPart(file) + ":" + MathLib::toString(line) + ":" + function + " bailout: " + what, "valueFlowBailout", false);
    errorLogger->reportErr(errmsg);
}

#if (defined __cplusplus) && __cplusplus >= 201103L
#define bailout(tokenlist, errorLogger, tok, what) bailoutInternal(tokenlist, errorLogger, tok, what, __FILE__, __LINE__, __func__)
#elif (defined __GNUC__) || (defined __clang__) || (defined _MSC_VER)
#define bailout(tokenlist, errorLogger, tok, what) bailoutInternal(tokenlist, errorLogger, tok, what, __FILE__, __LINE__, __FUNCTION__)
#else
#define bailout(tokenlist, errorLogger, tok, what) bailoutInternal(tokenlist, errorLogger, tok, what, __FILE__, __LINE__, "(valueFlow)")
#endif

static void changeKnownToPossible(std::list<ValueFlow::Value> &values)
{
    std::list<ValueFlow::Value>::iterator it;
    for (it = values.begin(); it != values.end(); ++it)
        it->changeKnownToPossible();
}

/**
 * Is condition always false when variable has given value?
 * \param condition   top ast token in condition
 * \param programMemory   program memory
 */
static bool conditionIsFalse(const Token *condition, const ProgramMemory &programMemory)
{
    if (!condition)
        return false;
    if (condition->str() == MatchCompiler::makeConstString("&&")) {
        return conditionIsFalse(condition->astOperand1(), programMemory) ||
               conditionIsFalse(condition->astOperand2(), programMemory);
    }
    ProgramMemory progmem(programMemory);
    MathLib::bigint result = 0;
    bool error = false;
    execute(condition, &progmem, &result, &error);
    return !error && result == 0;
}

/**
 * Is condition always true when variable has given value?
 * \param condition   top ast token in condition
 * \param programMemory   program memory
 */
static bool conditionIsTrue(const Token *condition, const ProgramMemory &programMemory)
{
    if (!condition)
        return false;
    if (condition->str() == MatchCompiler::makeConstString("||")) {
        return conditionIsTrue(condition->astOperand1(), programMemory) ||
               conditionIsTrue(condition->astOperand2(), programMemory);
    }
    ProgramMemory progmem(programMemory);
    bool error = false;
    MathLib::bigint result = 0;
    execute(condition, &progmem, &result, &error);
    return !error && result == 1;
}

/**
 * Get program memory by looking backwards from given token.
 */
static ProgramMemory getProgramMemory(const Token *tok, unsigned int varid, const ValueFlow::Value &value)
{
    ProgramMemory programMemory;
    programMemory.setValue(varid, value);
    if (value.varId)
        programMemory.setIntValue(value.varId, value.varvalue);
    const ProgramMemory programMemory1(programMemory);
    int indentlevel = 0;
    for (const Token *tok2 = tok; tok2; tok2 = tok2->previous()) {
        if (match1(tok2, varid)) {
            const Token *vartok = tok2->tokAt(3);
            programMemory.setValue(vartok->varId(), value);
        } else if (match2(tok2) ||
                   match3(tok2)) {
            const Token *vartok = tok2->next();
            while (vartok->next()->isName())
                vartok = vartok->next();
            if (!programMemory.hasValue(vartok->varId())) {
                MathLib::bigint result = 0;
                bool error = false;
                execute(vartok->next()->astOperand2(), &programMemory, &result, &error);
                if (!error)
                    programMemory.setIntValue(vartok->varId(), result);
            }
        }

        if (tok2->str() == MatchCompiler::makeConstString("{")) {
            if (indentlevel <= 0)
                break;
            --indentlevel;
        }
        if (tok2->str() == MatchCompiler::makeConstString("}")) {
            const Token *cond = tok2->link();
            cond = match4(cond->previous()) ? cond->linkAt(-1) : nullptr;
            if (cond && conditionIsFalse(cond->astOperand2(), programMemory1))
                tok2 = cond->previous();
            else if (cond && conditionIsTrue(cond->astOperand2(), programMemory1)) {
                ++indentlevel;
                continue;
            } else
                break;
        }
    }
    return programMemory;
}

/**
 * Should value be skipped because it's hidden inside && || or ?: expression.
 * Example: ((x!=NULL) && (*x == 123))
 * If 'valuetok' points at the x in '(*x == 123)'. Then the '&&' will be returned.
 * @param valuetok original variable token
 * @return NULL=>don't skip, non-NULL=>The operator token that cause the skip. For instance the '&&'.
 * */
static const Token * skipValueInConditionalExpression(const Token * const valuetok)
{
    // Walk up the ast
    const Token *prev = valuetok;
    for (const Token *tok = valuetok->astParent(); tok; tok = tok->astParent()) {
        const bool prevIsLhs = (prev == tok->astOperand1());
        prev = tok;

        if (prevIsLhs || !match5(tok))
            continue;

        // Is variable protected in LHS..
        std::stack<const Token *> tokens;
        tokens.push(tok->astOperand1());
        while (!tokens.empty()) {
            const Token * const tok2 = tokens.top();
            tokens.pop();
            if (!tok2 || tok2->str() == MatchCompiler::makeConstString("."))
                continue;
            // A variable is seen..
            if (tok2 != valuetok && tok2->variable() && (tok2->varId() == valuetok->varId() || !tok2->variable()->isArgument())) {
                // TODO: limit this bailout
                return tok;
            }
            tokens.push(tok2->astOperand2());
            tokens.push(tok2->astOperand1());
        }

    }
    return nullptr;
}

static bool bailoutSelfAssignment(const Token * const tok)
{
    const Token *parent = tok;
    while (parent) {
        const Token *op = parent;
        parent = parent->astParent();

        // Assignment where lhs variable exists in rhs => return true
        if (parent                         != nullptr      &&
            parent->astOperand2()          == op           &&
            parent->astOperand1()          != nullptr      &&
            parent->str()                  == MatchCompiler::makeConstString("=")) {
            for (const Token *lhs = parent->astOperand1(); lhs; lhs = lhs->astOperand1()) {
                if (lhs->varId() == tok->varId())
                    return true;
                if (lhs->astOperand2() && lhs->astOperand2()->varId() == tok->varId())
                    return true;
            }
        }
    }
    return false;
}

static ValueFlow::Value castValue(ValueFlow::Value value, const ValueType::Sign sign, unsigned int bit)
{
    if (value.isFloatValue()) {
        value.valueType = ValueFlow::Value::INT;
        if (value.floatValue >= std::numeric_limits<int>::min() && value.floatValue <= std::numeric_limits<int>::max()) {
            value.intvalue = value.floatValue;
        } else { // don't perform UB
            value.intvalue = 0;
        }
    }
    if (bit < MathLib::bigint_bits) {
        const MathLib::biguint one = 1;
        value.intvalue &= (one << bit) - 1;
        if (sign == ValueType::Sign::SIGNED && value.intvalue & (one << (bit - 1))) {
            value.intvalue |= ~((one << bit) - 1ULL);
        }
    }
    return value;
}

/** set ValueFlow value and perform calculations if possible */
static void setTokenValue(Token* tok, const ValueFlow::Value &value, const Settings *settings)
{
    if (!tok->addValue(value))
        return;

    // Don't set parent for uninitialized values
    if (value.isUninitValue())
        return;

    Token *parent = const_cast<Token*>(tok->astParent());
    if (!parent)
        return;

    if (parent->str() == MatchCompiler::makeConstString("(") && !parent->astOperand2() && match6(parent)) {
        const ValueType &valueType = ValueType::parseDecl(parent->next(), settings);
        if (valueType.pointer)
            setTokenValue(parent,value,settings);
        else if (valueType.type == ValueType::Type::CHAR)
            setTokenValue(parent, castValue(value, valueType.sign, settings->char_bit), settings);
        else if (valueType.type == ValueType::Type::SHORT)
            setTokenValue(parent, castValue(value, valueType.sign, settings->short_bit), settings);
        else if (valueType.type == ValueType::Type::INT)
            setTokenValue(parent, castValue(value, valueType.sign, settings->int_bit), settings);
        else if (valueType.type == ValueType::Type::LONG)
            setTokenValue(parent, castValue(value, valueType.sign, settings->long_bit), settings);
        else if (valueType.type == ValueType::Type::LONGLONG)
            setTokenValue(parent, castValue(value, valueType.sign, settings->long_long_bit), settings);
        else if (value.isIntValue()) {
            const long long charMax = settings->signedCharMax();
            const long long charMin = settings->signedCharMin();
            if (charMin <= value.intvalue && value.intvalue <= charMax) {
                // unknown type, but value is small so there should be no truncation etc
                setTokenValue(parent,value,settings);
            }
        }
    }

    else if (parent->str() == MatchCompiler::makeConstString(":")) {
        setTokenValue(parent,value,settings);
    }

    else if (parent->str() == MatchCompiler::makeConstString("?") && tok->str() == MatchCompiler::makeConstString(":") && tok == parent->astOperand2() && parent->astOperand1()) {
        // is condition always true/false?
        if (parent->astOperand1()->hasKnownValue()) {
            const ValueFlow::Value &condvalue = parent->astOperand1()->values().front();
            const bool cond(condvalue.isTokValue() || (condvalue.isIntValue() && condvalue.intvalue != 0));
            if (cond && !tok->astOperand1()) { // true condition, no second operator
                setTokenValue(parent, condvalue, settings);
            } else {
                const Token *op = cond ? tok->astOperand1() : tok->astOperand2();
                if (!op) // #7769 segmentation fault at setTokenValue()
                    return;
                const std::list<ValueFlow::Value> &values = op->values();
                if (std::find(values.begin(), values.end(), value) != values.end())
                    setTokenValue(parent, value, settings);
            }
        } else {
            // is condition only depending on 1 variable?
            std::stack<const Token*> tokens;
            tokens.push(parent->astOperand1());
            unsigned int varId = 0;
            while (!tokens.empty()) {
                const Token *t = tokens.top();
                tokens.pop();
                if (!t)
                    continue;
                tokens.push(t->astOperand1());
                tokens.push(t->astOperand2());
                if (t->varId()) {
                    if (varId > 0 || value.varId != 0U)
                        return;
                    varId = t->varId();
                } else if (t->str() == MatchCompiler::makeConstString("(") && match7(t->previous()))
                    return; // function call
            }

            ValueFlow::Value v(value);
            v.conditional = true;
            v.changeKnownToPossible();

            if (varId)
                v.varId = varId;

            setTokenValue(parent, v, settings);
        }
    }

    // Calculations..
    else if ((parent->isArithmeticalOp() || parent->isComparisonOp() || (parent->tokType() == Token::eBitOp) || (parent->tokType() == Token::eLogicalOp)) &&
             parent->astOperand1() &&
             parent->astOperand2()) {
        const bool known = (parent->astOperand1()->hasKnownValue() ||
                            parent->astOperand2()->hasKnownValue());

        // known result when a operand is 0.
        if (match8(parent) && value.isKnown() && value.isIntValue() && value.intvalue==0) {
            setTokenValue(parent, value, settings);
            return;
        }

        // known result when a operand is true.
        if (match9(parent) && value.isKnown() && value.isIntValue() && value.intvalue==0) {
            setTokenValue(parent, value, settings);
            return;
        }

        // known result when a operand is false.
        if (match10(parent) && value.isKnown() && value.isIntValue() && value.intvalue!=0) {
            setTokenValue(parent, value, settings);
            return;
        }

        for (const ValueFlow::Value &value1 : parent->astOperand1()->values()) {
            if (!value1.isIntValue() && !value1.isFloatValue() && !value1.isTokValue())
                continue;
            if (value1.isTokValue() && (!parent->isComparisonOp() || value1.tokvalue->tokType() != Token::eString))
                continue;
            for (const ValueFlow::Value &value2 : parent->astOperand2()->values()) {
                if (!value2.isIntValue() && !value2.isFloatValue() && !value2.isTokValue())
                    continue;
                if (value2.isTokValue() && (!parent->isComparisonOp() || value2.tokvalue->tokType() != Token::eString || value1.isTokValue()))
                    continue;
                if (known || value1.varId == 0U || value2.varId == 0U ||
                    (value1.varId == value2.varId && value1.varvalue == value2.varvalue && value1.isIntValue() && value2.isIntValue())) {
                    ValueFlow::Value result(0);
                    result.condition = value1.condition ? value1.condition : value2.condition;
                    result.setInconclusive(value1.isInconclusive() | value2.isInconclusive());
                    result.varId = (value1.varId != 0U) ? value1.varId : value2.varId;
                    result.varvalue = (result.varId == value1.varId) ? value1.varvalue : value2.varvalue;
                    result.errorPath = (value1.errorPath.empty() ? value2 : value1).errorPath;
                    if (value1.valueKind == value2.valueKind)
                        result.valueKind = value1.valueKind;
                    const float floatValue1 = value1.isIntValue() ? value1.intvalue : value1.floatValue;
                    const float floatValue2 = value2.isIntValue() ? value2.intvalue : value2.floatValue;
                    switch (parent->str()[0]) {
                    case '+':
                        if (value1.isTokValue() || value2.isTokValue())
                            break;
                        if (value1.isFloatValue() || value2.isFloatValue()) {
                            result.valueType = ValueFlow::Value::FLOAT;
                            result.floatValue = floatValue1 + floatValue2;
                        } else {
                            result.intvalue = value1.intvalue + value2.intvalue;
                        }
                        setTokenValue(parent, result, settings);
                        break;
                    case '-':
                        if (value1.isTokValue() || value2.isTokValue())
                            break;
                        if (value1.isFloatValue() || value2.isFloatValue()) {
                            result.valueType = ValueFlow::Value::FLOAT;
                            result.floatValue = floatValue1 - floatValue2;
                        } else {
                            result.intvalue = value1.intvalue - value2.intvalue;
                        }
                        setTokenValue(parent, result, settings);
                        break;
                    case '*':
                        if (value1.isTokValue() || value2.isTokValue())
                            break;
                        if (value1.isFloatValue() || value2.isFloatValue()) {
                            result.valueType = ValueFlow::Value::FLOAT;
                            result.floatValue = floatValue1 * floatValue2;
                        } else {
                            result.intvalue = value1.intvalue * value2.intvalue;
                        }
                        setTokenValue(parent, result, settings);
                        break;
                    case '/':
                        if (value1.isTokValue() || value2.isTokValue() || value2.intvalue == 0)
                            break;
                        if (value1.isFloatValue() || value2.isFloatValue()) {
                            result.valueType = ValueFlow::Value::FLOAT;
                            result.floatValue = floatValue1 / floatValue2;
                        } else {
                            result.intvalue = value1.intvalue / value2.intvalue;
                        }
                        setTokenValue(parent, result, settings);
                        break;
                    case '%':
                        if (!value1.isIntValue() || !value2.isIntValue())
                            break;
                        if (value2.intvalue == 0)
                            break;
                        result.intvalue = value1.intvalue % value2.intvalue;
                        setTokenValue(parent, result, settings);
                        break;
                    case '=':
                        if (parent->str() == MatchCompiler::makeConstString("==")) {
                            if ((value1.isIntValue() && value2.isTokValue()) ||
                                (value1.isTokValue() && value2.isIntValue())) {
                                result.intvalue = 0;
                                setTokenValue(parent, result, settings);
                            } else if (value1.isIntValue() && value2.isIntValue()) {
                                result.intvalue = value1.intvalue == value2.intvalue;
                                setTokenValue(parent, result, settings);
                            }
                        }
                        break;
                    case '!':
                        if (parent->str() == MatchCompiler::makeConstString("!=")) {
                            if ((value1.isIntValue() && value2.isTokValue()) ||
                                (value1.isTokValue() && value2.isIntValue())) {
                                result.intvalue = 1;
                                setTokenValue(parent, result, settings);
                            } else if (value1.isIntValue() && value2.isIntValue()) {
                                result.intvalue = value1.intvalue != value2.intvalue;
                                setTokenValue(parent, result, settings);
                            }
                        }
                        break;
                    case '>': {
                        const bool f = value1.isFloatValue() || value2.isFloatValue();
                        if (!f && !value1.isIntValue() && !value2.isIntValue())
                            break;
                        if (parent->str() == MatchCompiler::makeConstString(">"))
                            result.intvalue = f ? (floatValue1 > floatValue2) : (value1.intvalue > value2.intvalue);
                        else if (parent->str() == MatchCompiler::makeConstString(">="))
                            result.intvalue = f ? (floatValue1 >= floatValue2) : (value1.intvalue >= value2.intvalue);
                        else if (!f && parent->str() == MatchCompiler::makeConstString(">>") && value1.intvalue >= 0 && value2.intvalue >= 0 && value2.intvalue < MathLib::bigint_bits)
                            result.intvalue = value1.intvalue >> value2.intvalue;
                        else
                            break;
                        setTokenValue(parent, result, settings);
                        break;
                    }
                    case '<': {
                        const bool f = value1.isFloatValue() || value2.isFloatValue();
                        if (!f && !value1.isIntValue() && !value2.isIntValue())
                            break;
                        if (parent->str() == MatchCompiler::makeConstString("<"))
                            result.intvalue = f ? (floatValue1 < floatValue2) : (value1.intvalue < value2.intvalue);
                        else if (parent->str() == MatchCompiler::makeConstString("<="))
                            result.intvalue = f ? (floatValue1 <= floatValue2) : (value1.intvalue <= value2.intvalue);
                        else if (!f && parent->str() == MatchCompiler::makeConstString("<<") && value1.intvalue >= 0 && value2.intvalue >= 0 && value2.intvalue < MathLib::bigint_bits)
                            result.intvalue = value1.intvalue << value2.intvalue;
                        else
                            break;
                        setTokenValue(parent, result, settings);
                        break;
                    }
                    case '&':
                        if (!value1.isIntValue() || !value2.isIntValue())
                            break;
                        if (parent->str() == MatchCompiler::makeConstString("&"))
                            result.intvalue = value1.intvalue & value2.intvalue;
                        else
                            result.intvalue = value1.intvalue && value2.intvalue;
                        setTokenValue(parent, result, settings);
                        break;
                    case '|':
                        if (!value1.isIntValue() || !value2.isIntValue())
                            break;
                        if (parent->str() == MatchCompiler::makeConstString("|"))
                            result.intvalue = value1.intvalue | value2.intvalue;
                        else
                            result.intvalue = value1.intvalue || value2.intvalue;
                        setTokenValue(parent, result, settings);
                        break;
                    case '^':
                        if (!value1.isIntValue() || !value2.isIntValue())
                            break;
                        result.intvalue = value1.intvalue ^ value2.intvalue;
                        setTokenValue(parent, result, settings);
                        break;
                    default:
                        // unhandled operator, do nothing
                        break;
                    }
                }
            }
        }
    }

    // !
    else if (parent->str() == MatchCompiler::makeConstString("!")) {
        for (const ValueFlow::Value &val : tok->values()) {
            if (!val.isIntValue())
                continue;
            ValueFlow::Value v(val);
            v.intvalue = !v.intvalue;
            setTokenValue(parent, v, settings);
        }
    }

    // ~
    else if (parent->str() == MatchCompiler::makeConstString("~")) {
        for (const ValueFlow::Value &val : tok->values()) {
            if (!val.isIntValue())
                continue;
            ValueFlow::Value v(val);
            v.intvalue = ~v.intvalue;
            unsigned int bits = 0;
            if (settings &&
                tok->valueType() &&
                tok->valueType()->sign == ValueType::Sign::UNSIGNED &&
                tok->valueType()->pointer == 0) {
                if (tok->valueType()->type == ValueType::Type::INT)
                    bits = settings->int_bit;
                else if (tok->valueType()->type == ValueType::Type::LONG)
                    bits = settings->long_bit;
            }
            if (bits > 0 && bits < MathLib::bigint_bits)
                v.intvalue &= (((MathLib::biguint)1)<<bits) - 1;
            setTokenValue(parent, v, settings);
        }
    }

    // unary minus
    else if (parent->isUnaryOp("-")) {
        for (const ValueFlow::Value &val : tok->values()) {
            if (!val.isIntValue() && !val.isFloatValue())
                continue;
            ValueFlow::Value v(val);
            if (v.isIntValue())
                v.intvalue = -v.intvalue;
            else
                v.floatValue = -v.floatValue;
            setTokenValue(parent, v, settings);
        }
    }

    // Array element
    else if (parent->str() == MatchCompiler::makeConstString("[") && parent->isBinaryOp()) {
        for (const ValueFlow::Value &value1 : parent->astOperand1()->values()) {
            if (!value1.isTokValue())
                continue;
            for (const ValueFlow::Value &value2 : parent->astOperand2()->values()) {
                if (!value2.isIntValue())
                    continue;
                if (value1.varId == 0U || value2.varId == 0U ||
                    (value1.varId == value2.varId && value1.varvalue == value2.varvalue)) {
                    ValueFlow::Value result(0);
                    result.condition = value1.condition ? value1.condition : value2.condition;
                    result.setInconclusive(value1.isInconclusive() | value2.isInconclusive());
                    result.varId = (value1.varId != 0U) ? value1.varId : value2.varId;
                    result.varvalue = (result.varId == value1.varId) ? value1.intvalue : value2.intvalue;
                    if (value1.valueKind == value2.valueKind)
                        result.valueKind = value1.valueKind;
                    if (value1.tokvalue->tokType() == Token::eString) {
                        const std::string s = value1.tokvalue->strValue();
                        const MathLib::bigint index = value2.intvalue;
                        if (index == s.size()) {
                            result.intvalue = 0;
                            setTokenValue(parent, result, settings);
                        } else if (index >= 0 && index < s.size()) {
                            result.intvalue = s[index];
                            setTokenValue(parent, result, settings);
                        }
                    } else if (value1.tokvalue->str() == MatchCompiler::makeConstString("{")) {
                        MathLib::bigint index = value2.intvalue;
                        const Token *element = value1.tokvalue->next();
                        while (index > 0 && element->str() != MatchCompiler::makeConstString("}")) {
                            if (element->str() == MatchCompiler::makeConstString(","))
                                --index;
                            if (match11(element))
                                break;
                            element = element->next();
                        }
                        if (match12(element)) {
                            result.intvalue = MathLib::toLongNumber(element->str());
                            setTokenValue(parent, result, settings);
                        }
                    }
                }
            }
        }
    }
}

static unsigned int getSizeOfType(const Token *typeTok, const Settings *settings)
{
    const std::string &typeStr = typeTok->str();
    if (typeStr == MatchCompiler::makeConstString("char"))
        return 1;
    else if (typeStr == MatchCompiler::makeConstString("short"))
        return settings->sizeof_short;
    else if (typeStr == MatchCompiler::makeConstString("int"))
        return settings->sizeof_int;
    else if (typeStr == MatchCompiler::makeConstString("long"))
        return typeTok->isLong() ? settings->sizeof_long_long : settings->sizeof_long;
    else if (typeStr == MatchCompiler::makeConstString("wchar_t"))
        return settings->sizeof_wchar_t;
    else
        return 0;
}

// Handle various constants..
static Token * valueFlowSetConstantValue(const Token *tok, const Settings *settings, bool cpp)
{
    if ((tok->isNumber() && MathLib::isInt(tok->str())) || (tok->tokType() == Token::eChar)) {
        ValueFlow::Value value(MathLib::toLongNumber(tok->str()));
        if (!tok->isTemplateArg())
            value.setKnown();
        setTokenValue(const_cast<Token *>(tok), value, settings);
    } else if (tok->isNumber() && MathLib::isFloat(tok->str())) {
        ValueFlow::Value value;
        value.valueType = ValueFlow::Value::FLOAT;
        value.floatValue = MathLib::toDoubleNumber(tok->str());
        if (!tok->isTemplateArg())
            value.setKnown();
        setTokenValue(const_cast<Token *>(tok), value, settings);
    } else if (tok->enumerator() && tok->enumerator()->value_known) {
        ValueFlow::Value value(tok->enumerator()->value);
        if (!tok->isTemplateArg())
            value.setKnown();
        setTokenValue(const_cast<Token *>(tok), value, settings);
    } else if (tok->str() == MatchCompiler::makeConstString("NULL") || (cpp && tok->str() == MatchCompiler::makeConstString("nullptr"))) {
        ValueFlow::Value value(0);
        if (!tok->isTemplateArg())
            value.setKnown();
        setTokenValue(const_cast<Token *>(tok), value, settings);
    } else if (match13(tok)) {
        const Token *tok2 = tok->tokAt(2);
        // skip over tokens to find variable or type
        while (match14(tok2)) {
            if (tok2->next()->str() == MatchCompiler::makeConstString("["))
                tok2 = tok2->linkAt(1)->next();
            else
                tok2 = tok2->tokAt(2);
        }
        if (tok2->enumerator() && tok2->enumerator()->scope) {
            long long size = settings->sizeof_int;
            const Token * type = tok2->enumerator()->scope->enumType;
            if (type) {
                size = getSizeOfType(type, settings);
            }
            ValueFlow::Value value(size);
            if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                value.setKnown();
            setTokenValue(const_cast<Token *>(tok), value, settings);
            setTokenValue(const_cast<Token *>(tok->next()), value, settings);
        } else if (tok2->type() && tok2->type()->isEnumType()) {
            long long size = settings->sizeof_int;
            if (tok2->type()->classScope) {
                const Token * type = tok2->type()->classScope->enumType;
                if (type) {
                    size = getSizeOfType(type, settings);
                }
            }
            ValueFlow::Value value(size);
            if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                value.setKnown();
            setTokenValue(const_cast<Token *>(tok), value, settings);
            setTokenValue(const_cast<Token *>(tok->next()), value, settings);
        } else if (match15(tok) && tok->next()->astParent() == tok->tokAt(4)) {
            // Get number of elements in array
            const Token *sz1 = tok->tokAt(2);
            const Token *sz2 = tok->tokAt(7);
            const unsigned int varid1 = sz1->varId();
            if (varid1 &&
                sz1->variable() &&
                sz1->variable()->isArray() &&
                !sz1->variable()->dimensions().empty() &&
                sz1->variable()->dimensionKnown(0) &&
                (match16(sz2, varid1) || match17(sz2, varid1))) {
                ValueFlow::Value value(sz1->variable()->dimension(0));
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->tokAt(4)), value, settings);
            }
        } else if (match18(tok2)) {
            const Variable *var = tok2->variable();
            // only look for single token types (no pointers or references yet)
            if (var && var->typeStartToken() == var->typeEndToken()) {
                // find the size of the type
                size_t size = 0;
                if (var->isEnumType()) {
                    size = settings->sizeof_int;
                    if (var->type()->classScope && var->type()->classScope->enumType)
                        size = getSizeOfType(var->type()->classScope->enumType, settings);
                } else if (!var->type()) {
                    size = getSizeOfType(var->typeStartToken(), settings);
                }
                // find the number of elements
                size_t count = 1;
                for (size_t i = 0; i < var->dimensions().size(); ++i) {
                    if (var->dimensionKnown(i))
                        count *= var->dimension(i);
                    else
                        count = 0;
                }
                if (size && count > 0) {
                    ValueFlow::Value value(count * size);
                    if (settings->platformType != cppcheck::Platform::Unspecified)
                        value.setKnown();
                    setTokenValue(const_cast<Token *>(tok), value, settings);
                    setTokenValue(const_cast<Token *>(tok->next()), value, settings);
                }
            }
        } else if (!tok2->type()) {
            const ValueType &vt = ValueType::parseDecl(tok2,settings);
            if (vt.pointer) {
                ValueFlow::Value value(settings->sizeof_pointer);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            } else if (vt.type == ValueType::Type::CHAR) {
                ValueFlow::Value value(1);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            } else if (vt.type == ValueType::Type::SHORT) {
                ValueFlow::Value value(settings->sizeof_short);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            } else if (vt.type == ValueType::Type::INT) {
                ValueFlow::Value value(settings->sizeof_int);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            } else if (vt.type == ValueType::Type::LONG) {
                ValueFlow::Value value(settings->sizeof_long);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            } else if (vt.type == ValueType::Type::LONGLONG) {
                ValueFlow::Value value(settings->sizeof_long_long);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            } else if (vt.type == ValueType::Type::FLOAT) {
                ValueFlow::Value value(settings->sizeof_float);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            } else if (vt.type == ValueType::Type::DOUBLE) {
                ValueFlow::Value value(settings->sizeof_double);
                if (!tok2->isTemplateArg() && settings->platformType != cppcheck::Platform::Unspecified)
                    value.setKnown();
                setTokenValue(const_cast<Token *>(tok->next()), value, settings);
            }
        }
        // skip over enum
        tok = tok->linkAt(1);
    }
    return tok->next();
}


static void valueFlowNumber(TokenList *tokenlist)
{
    for (Token *tok = tokenlist->front(); tok;) {
        tok = valueFlowSetConstantValue(tok, tokenlist->getSettings(), tokenlist->isCPP());
    }

    if (tokenlist->isCPP()) {
        for (Token *tok = tokenlist->front(); tok; tok = tok->next()) {
            if (tok->isName() && !tok->varId() && match19(tok)) {
                ValueFlow::Value value(tok->str() == MatchCompiler::makeConstString("true"));
                if (!tok->isTemplateArg())
                    value.setKnown();
                setTokenValue(tok, value, tokenlist->getSettings());
            } else if (match20(tok)) {
                // NULL function parameters are not simplified in the
                // normal tokenlist
                ValueFlow::Value value(0);
                if (!tok->isTemplateArg())
                    value.setKnown();
                setTokenValue(tok->next(), value, tokenlist->getSettings());
            }
        }
    }
}

static void valueFlowString(TokenList *tokenlist)
{
    for (Token *tok = tokenlist->front(); tok; tok = tok->next()) {
        if (tok->tokType() == Token::eString) {
            ValueFlow::Value strvalue;
            strvalue.valueType = ValueFlow::Value::TOK;
            strvalue.tokvalue = tok;
            strvalue.setKnown();
            setTokenValue(tok, strvalue, tokenlist->getSettings());
        }
    }
}

static void valueFlowArray(TokenList *tokenlist)
{
    std::map<unsigned int, const Token *> constantArrays;

    for (Token *tok = tokenlist->front(); tok; tok = tok->next()) {
        if (tok->varId() > 0U) {
            const std::map<unsigned int, const Token *>::const_iterator it = constantArrays.find(tok->varId());
            if (it != constantArrays.end()) {
                ValueFlow::Value value;
                value.valueType = ValueFlow::Value::TOK;
                value.tokvalue = it->second;
                value.setKnown();
                setTokenValue(tok, value, tokenlist->getSettings());
            }

            // pointer = array
            else if (tok->variable() &&
                     tok->variable()->isArray() &&
                     match21(tok->astParent()) &&
                     tok == tok->astParent()->astOperand2() &&
                     tok->astParent()->astOperand1() &&
                     tok->astParent()->astOperand1()->variable() &&
                     tok->astParent()->astOperand1()->variable()->isPointer()) {
                ValueFlow::Value value;
                value.valueType = ValueFlow::Value::TOK;
                value.tokvalue = tok;
                value.setKnown();
                setTokenValue(tok, value, tokenlist->getSettings());
            }
            continue;
        }

        if (match22(tok)) {
            const Token *vartok = tok->tokAt(2);
            const Token *rhstok = vartok->next()->link()->tokAt(2);
            constantArrays[vartok->varId()] = rhstok;
            tok = rhstok->link();
            continue;
        }

        else if (match23(tok)) {
            const Token *vartok = tok->tokAt(2);
            const Token *strtok = vartok->next()->link()->tokAt(2);
            constantArrays[vartok->varId()] = strtok;
            tok = strtok->next();
            continue;
        }
    }
}

static void valueFlowPointerAlias(TokenList *tokenlist)
{
    for (Token *tok = tokenlist->front(); tok; tok = tok->next()) {
        // not address of
        if (!tok->isUnaryOp("&"))
            continue;

        // parent should be a '='
        if (!match21(tok->astParent()))
            continue;

        // child should be some buffer or variable
        const Token *vartok = tok->astOperand1();
        while (vartok) {
            if (vartok->str() == MatchCompiler::makeConstString("["))
                vartok = vartok->astOperand1();
            else if (vartok->str() == MatchCompiler::makeConstString(".") || vartok->str() == MatchCompiler::makeConstString("::"))
                vartok = vartok->astOperand2();
            else
                break;
        }
        if (!(vartok && vartok->variable() && !vartok->variable()->isPointer()))
            continue;

        ValueFlow::Value value;
        value.valueType = ValueFlow::Value::TOK;
        value.tokvalue = tok;
        setTokenValue(tok, value, tokenlist->getSettings());
    }
}

static void valueFlowBitAnd(TokenList *tokenlist)
{
    for (Token *tok = tokenlist->front(); tok; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("&"))
            continue;

        if (tok->hasKnownValue())
            continue;

        if (!tok->astOperand1() || !tok->astOperand2())
            continue;

        MathLib::bigint number;
        if (MathLib::isInt(tok->astOperand1()->str()))
            number = MathLib::toLongNumber(tok->astOperand1()->str());
        else if (MathLib::isInt(tok->astOperand2()->str()))
            number = MathLib::toLongNumber(tok->astOperand2()->str());
        else
            continue;

        int bit = 0;
        while (bit <= (MathLib::bigint_bits - 2) && ((((MathLib::bigint)1) << bit) < number))
            ++bit;

        if ((((MathLib::bigint)1) << bit) == number) {
            setTokenValue(tok, ValueFlow::Value(0), tokenlist->getSettings());
            setTokenValue(tok, ValueFlow::Value(number), tokenlist->getSettings());
        }
    }
}

static void valueFlowOppositeCondition(SymbolDatabase *symboldatabase, const Settings *settings)
{
    for (const Scope &scope : symboldatabase->scopeList) {
        if (scope.type != Scope::eIf)
            continue;
        Token *tok = const_cast<Token *>(scope.classDef);
        if (!match24(tok))
            continue;
        const Token *cond1 = tok->next()->astOperand2();
        if (!cond1 || !cond1->isComparisonOp())
            continue;
        const bool cpp = symboldatabase->isCPP();
        Token *tok2 = tok->linkAt(1);
        while (match4(tok2)) {
            tok2 = tok2->linkAt(1);
            if (!match25(tok2))
                break;
            const Token *ifOpenBraceTok = tok2->tokAt(4);
            const Token *cond2 = ifOpenBraceTok->astOperand2();
            if (!cond2 || !cond2->isComparisonOp())
                continue;
            if (isOppositeCond(true, cpp, cond1, cond2, settings->library, true, true)) {
                ValueFlow::Value value(1);
                value.setKnown();
                setTokenValue(const_cast<Token*>(cond2), value, settings);
            }
            tok2 = ifOpenBraceTok->link();
        }
    }
}

static void valueFlowGlobalStaticVar(TokenList *tokenList, const Settings *settings)
{
    // Get variable values...
    std::map<const Variable *, ValueFlow::Value> vars;
    for (const Token *tok = tokenList->front(); tok; tok = tok->next()) {
        if (!tok->variable())
            continue;
        // Initialization...
        if (tok == tok->variable()->nameToken() &&
            tok->variable()->isStatic() &&
            !tok->variable()->isConst() &&
            tok->valueType() &&
            tok->valueType()->isIntegral() &&
            tok->valueType()->pointer == 0 &&
            tok->valueType()->constness == 0 &&
            match26(tok) &&
            tok->next()->astOperand2() &&
            tok->next()->astOperand2()->hasKnownIntValue()) {
            vars[tok->variable()] = tok->next()->astOperand2()->values().front();
        } else {
            // If variable is written anywhere in TU then remove it from vars
            if (!tok->astParent())
                continue;
            if (match27(tok->astParent()) && !tok->astParent()->astOperand2())
                vars.erase(tok->variable());
            else if (tok->astParent()->isAssignmentOp()) {
                if (tok == tok->astParent()->astOperand1())
                    vars.erase(tok->variable());
                else if (tokenList->isCPP() && match28(tok->astParent()->tokAt(-2)))
                    vars.erase(tok->variable());
            } else if (isLikelyStreamRead(tokenList->isCPP(), tok->astParent())) {
                vars.erase(tok->variable());
            } else if (match29(tok->astParent()))
                vars.erase(tok->variable());
        }
    }

    // Set values..
    for (Token *tok = tokenList->front(); tok; tok = tok->next()) {
        if (!tok->variable())
            continue;
        std::map<const Variable *, ValueFlow::Value>::const_iterator var = vars.find(tok->variable());
        if (var == vars.end())
            continue;
        setTokenValue(tok, var->second, settings);
    }
}

static void valueFlowReverse(TokenList *tokenlist,
                             Token *tok,
                             const Token * const varToken,
                             ValueFlow::Value val,
                             ValueFlow::Value val2,
                             ErrorLogger *errorLogger,
                             const Settings *settings)
{
    const MathLib::bigint    num        = val.intvalue;
    const Variable * const   var        = varToken->variable();
    if (!var)
        return;

    const unsigned int       varid      = varToken->varId();
    const Token * const      startToken = var->nameToken();

    for (Token *tok2 = tok->previous(); ; tok2 = tok2->previous()) {
        if (!tok2 ||
            tok2 == startToken ||
            (tok2->str() == MatchCompiler::makeConstString("{") && tok2->scope()->type == Scope::ScopeType::eFunction)) {
            break;
        }

        if (tok2->varId() == varid) {
            // bailout: assignment
            if (match30(tok2->previous())) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "assignment of " + tok2->str());
                break;
            }

            // increment/decrement
            int inc = 0;
            if (match31(tok2->previous()))
                inc = (tok2->strAt(1)==MatchCompiler::makeConstString("++")) ? -1 : 1;
            else if (match32(tok2->tokAt(-2)))
                inc = (tok2->strAt(-1)==MatchCompiler::makeConstString("++")) ? -1 : 1;
            else if (match33(tok2->previous()) || match34(tok2)) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "increment/decrement of " + tok2->str());
                break;
            }
            if (inc != 0) {
                val.intvalue += inc;
                const std::string info(tok2->str() + " is " + std::string(inc==1 ? "decremented" : "incremented") + ", before this " + (inc==1?"decrement":"increment") + " the value is " + val.infoString());
                val.errorPath.emplace_back(tok2, info);
            }

            // compound assignment
            if (match35(tok2->previous()) && tok2->next()->str() != MatchCompiler::makeConstString("=")) {
                const Token * const assignToken = tok2->next();
                const Token * const rhsToken = assignToken->astOperand2();
                if (!rhsToken || !rhsToken->hasKnownIntValue()) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "compound assignment, rhs value is not known");
                    break;
                }
                const MathLib::bigint rhsValue =  rhsToken->values().front().intvalue;
                if (assignToken->str() == MatchCompiler::makeConstString("+="))
                    val.intvalue -= rhsValue;
                else if (assignToken->str() == MatchCompiler::makeConstString("-="))
                    val.intvalue += rhsValue;
                else if (assignToken->str() == MatchCompiler::makeConstString("*=") && rhsValue != 0)
                    val.intvalue /= rhsValue;
                else {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "compound assignment " + tok2->str());
                    break;
                }

                const std::string info("Compound assignment '" + assignToken->str() + "', before assignment value is " + val.infoString());
                val.errorPath.emplace_back(tok2, info);
            }

            // bailout: variable is used in rhs in assignment to itself
            if (bailoutSelfAssignment(tok2)) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "variable " + tok2->str() + " is used in rhs in assignment to itself");
                break;
            }

            if (match36(tok2->previous())) {
                const Token *prev = tok2->previous();
                while (match37(prev) && prev->str() != MatchCompiler::makeConstString("sizeof"))
                    prev = prev->previous();
                if (prev && prev->str() == MatchCompiler::makeConstString("sizeof"))
                    continue;
            }

            // assigned by subfunction?
            bool inconclusive = false;
            if (isVariableChangedByFunctionCall(tok2, settings, &inconclusive)) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "possible assignment of " + tok2->str() + " by subfunction");
                break;
            }
            val.setInconclusive(inconclusive);
            val2.setInconclusive(inconclusive);

            // skip if variable is conditionally used in ?: expression
            if (const Token *parent = skipValueInConditionalExpression(tok2)) {
                if (settings->debugwarnings)
                    bailout(tokenlist,
                            errorLogger,
                            tok2,
                            "no simplification of " + tok2->str() + " within " + (match38(parent) ? "?:" : parent->str()) + " expression");
                continue;
            }

            // do-while condition, break in the loop body
            {
                const Token *parent = tok2->astParent();
                while (parent && !match39(parent->previous()))
                    parent = parent->astParent();
                if (parent && match40(parent->tokAt(-2)) && match41(parent->linkAt(-2)->previous())) {
                    bool breakBailout = false;
                    for (const Token *iftok = parent->linkAt(-2); iftok != parent; iftok = iftok->next()) {
                        if (!match24(iftok))
                            continue;
                        if (!match42(iftok->linkAt(1)))
                            continue;
                        ProgramMemory programMemory;
                        programMemory.setIntValue(varid, num);
                        if (conditionIsTrue(iftok->next()->astOperand2(), programMemory)) {
                            breakBailout = true;
                            break;
                        }
                    }
                    if (breakBailout) {
                        if (settings->debugwarnings)
                            bailout(tokenlist,
                                    errorLogger,
                                    tok2,
                                    "no simplification of " + tok2->str() + " in do-while condition since there is a break in the loop body");
                        break;
                    }
                }
            }

            setTokenValue(tok2, val, settings);
            if (val2.condition)
                setTokenValue(tok2,val2, settings);
            if (tok2 == var->nameToken())
                break;
        }

        // skip sizeof etc..
        if (tok2->str() == MatchCompiler::makeConstString(")") && match43(tok2->link()->previous()))
            tok2 = tok2->link();

        // goto label
        if (match44(tok2)) {
            if (settings->debugwarnings)
                bailout(tokenlist, errorLogger, tok2->next(), "variable " + var->name() + " stopping on goto label");
            break;
        }

        if (tok2->str() == MatchCompiler::makeConstString("}")) {
            const Token *vartok = findmatch45(tok2->link(), tok2, varid) ;
            while (match46(vartok) && !vartok->tokAt(2)->getValue(num))
                vartok = findmatch45(vartok->next(), tok2, varid) ;
            if (vartok) {
                if (settings->debugwarnings) {
                    std::string errmsg = "variable ";
                    errmsg += var->name() + " ";
                    errmsg += "stopping on }";
                    bailout(tokenlist, errorLogger, tok2, errmsg);
                }
                break;
            } else {
                tok2 = tok2->link();
            }
        } else if (tok2->str() == MatchCompiler::makeConstString("{")) {
            // if variable is assigned in loop don't look before the loop
            if (tok2->previous() &&
                (match47(tok2->previous()) ||
                 (tok2->strAt(-1) == MatchCompiler::makeConstString(")") && match48(tok2->linkAt(-1)->previous())))) {

                const Token *start = tok2;
                const Token *end   = start->link();
                if (isVariableChanged(start,end,varid,var->isGlobal(),settings, tokenlist->isCPP())) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " is assigned in loop. so valueflow analysis bailout when start of loop is reached.");
                    break;
                }
            }

            // Global variable : stop when leaving the function scope
            if (!var->isLocal()) {
                if (!match49(tok2->previous()))
                    break;
                if ((tok2->previous()->str() == MatchCompiler::makeConstString(")")) &&
                    !match50(tok2->linkAt(-1)->previous()))
                    break;
            }
        } else if (tok2->str() == MatchCompiler::makeConstString(";")) {
            const Token *parent = tok2->previous();
            while (parent && !match51(parent))
                parent = parent->astParent();
            // reaching a break/continue/return
            if (parent) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " stopping on " + parent->str());
                break;
            }
        }

        if (match52(tok2) && !match4(tok2->linkAt(1))) {
            // bailout: global non-const variables
            if (!(var->isLocal() || var->isArgument()) && !var->isConst()) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok, "global variable " + var->name());
                return;
            }
        }
    }
}

static void valueFlowBeforeCondition(TokenList *tokenlist, SymbolDatabase *symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    for (const Scope * scope : symboldatabase->functionScopes) {
        for (Token* tok = const_cast<Token*>(scope->bodyStart); tok != scope->bodyEnd; tok = tok->next()) {
            MathLib::bigint num = 0;
            const Token *vartok = nullptr;
            if (tok->isComparisonOp() && tok->astOperand1() && tok->astOperand2()) {
                if (tok->astOperand1()->isName() && tok->astOperand2()->hasKnownIntValue()) {
                    vartok = tok->astOperand1();
                    num = tok->astOperand2()->values().front().intvalue;
                } else if (tok->astOperand1()->hasKnownIntValue() && tok->astOperand2()->isName()) {
                    vartok = tok->astOperand2();
                    num = tok->astOperand1()->values().front().intvalue;
                } else {
                    continue;
                }
            } else if (match53(tok->previous()) ||
                       match54(tok)) {
                vartok = tok->next();
                num = 0;
            } else if (match55(tok) && match7(tok->astOperand1())) {
                vartok = tok->astOperand1();
                num = 0;
            } else {
                continue;
            }

            unsigned int varid = vartok->varId();
            const Variable * const var = vartok->variable();

            if (varid == 0U || !var)
                continue;

            // bailout: for/while-condition, variable is changed in while loop
            for (const Token *tok2 = tok; tok2; tok2 = tok2->astParent()) {
                if (tok2->astParent() || tok2->str() != MatchCompiler::makeConstString("(") || !match4(tok2->link()))
                    continue;

                // Variable changed in 3rd for-expression
                if (match56(tok2->previous())) {
                    if (tok2->astOperand2() && tok2->astOperand2()->astOperand2() && isVariableChanged(tok2->astOperand2()->astOperand2(), tok2->link(), varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                        varid = 0U;
                        if (settings->debugwarnings)
                            bailout(tokenlist, errorLogger, tok, "variable " + var->name() + " used in loop");
                    }
                }

                // Variable changed in loop code
                if (match48(tok2->previous())) {
                    const Token * const start = tok2->link()->next();
                    const Token * const end   = start->link();

                    if (isVariableChanged(start,end,varid,var->isGlobal(),settings, tokenlist->isCPP())) {
                        varid = 0U;
                        if (settings->debugwarnings)
                            bailout(tokenlist, errorLogger, tok, "variable " + var->name() + " used in loop");
                    }
                }

                // if,macro => bailout
                else if (match24(tok2->previous()) && tok2->previous()->isExpandedMacro()) {
                    varid = 0U;
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok, "variable " + var->name() + ", condition is defined in macro");
                }
            }
            if (varid == 0U)
                continue;

            // extra logic for unsigned variables 'i>=1' => possible value can also be 0
            if (match57(tok)) {
                if (num != 0)
                    continue;
                if (!var->typeStartToken()->isUnsigned())
                    continue;
            }
            ValueFlow::Value val(tok, num);
            val.varId = varid;
            ValueFlow::Value val2;
            if (num==1U && match58(tok)) {
                if (var->typeStartToken()->isUnsigned()) {
                    val2 = ValueFlow::Value(tok,0);
                    val2.varId = varid;
                }
            }
            valueFlowReverse(tokenlist,
                             tok,
                             vartok,
                             val,
                             val2,
                             errorLogger,
                             settings);

        }
    }
}

static void removeValues(std::list<ValueFlow::Value> &values, const std::list<ValueFlow::Value> &valuesToRemove)
{
    for (std::list<ValueFlow::Value>::iterator it = values.begin(); it != values.end();) {
        bool found = false;
        for (const ValueFlow::Value &v2 : valuesToRemove) {
            if (it->intvalue == v2.intvalue) {
                found = true;
                break;
            }
        }
        if (found)
            values.erase(it++);
        else
            ++it;
    }
}

static void valueFlowAST(Token *tok, unsigned int varid, const ValueFlow::Value &value, const Settings *settings)
{
    if (!tok)
        return;
    if (tok->varId() == varid)
        setTokenValue(tok, value, settings);
    valueFlowAST(const_cast<Token*>(tok->astOperand1()), varid, value, settings);
    if (tok->str() == MatchCompiler::makeConstString("&&") && tok->astOperand1() && tok->astOperand1()->getValue(0)) {
        ProgramMemory pm;
        pm.setValue(varid,value);
        if (conditionIsFalse(tok->astOperand1(), pm))
            return;
    } else if (tok->str() == MatchCompiler::makeConstString("||") && tok->astOperand1()) {
        const std::list<ValueFlow::Value> &values = tok->astOperand1()->values();
        bool nonzero = false;
        for (const ValueFlow::Value &v : values) {
            if (v.intvalue != 0) {
                nonzero = true;
                break;
            }
        }
        if (!nonzero)
            return;
        ProgramMemory pm;
        pm.setValue(varid,value);
        if (conditionIsTrue(tok->astOperand1(), pm))
            return;
    }
    valueFlowAST(const_cast<Token*>(tok->astOperand2()), varid, value, settings);
}

/** if known variable is changed in loop body, change it to a possible value */
static void handleKnownValuesInLoop(const Token                 *startToken,
                                    const Token                 *endToken,
                                    std::list<ValueFlow::Value> *values,
                                    unsigned int                varid,
                                    bool                        globalvar,
                                    const Settings              *settings)
{
    bool isChanged = false;
    for (std::list<ValueFlow::Value>::iterator it = values->begin(); it != values->end(); ++it) {
        if (it->isKnown()) {
            if (!isChanged) {
                if (!isVariableChanged(startToken, endToken, varid, globalvar, settings, true))
                    break;
                isChanged = true;
            }

            it->setPossible();
        }
    }
}

static bool evalAssignment(ValueFlow::Value &lhsValue, const std::string &assign, const ValueFlow::Value &rhsValue)
{
    if (lhsValue.isIntValue()) {
        if (assign == MatchCompiler::makeConstString("+="))
            lhsValue.intvalue += rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("-="))
            lhsValue.intvalue -= rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("*="))
            lhsValue.intvalue *= rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("/=")) {
            if (rhsValue.intvalue == 0)
                return false;
            else
                lhsValue.intvalue /= rhsValue.intvalue;
        } else if (assign == MatchCompiler::makeConstString("%=")) {
            if (rhsValue.intvalue == 0)
                return false;
            else
                lhsValue.intvalue %= rhsValue.intvalue;
        } else if (assign == MatchCompiler::makeConstString("&="))
            lhsValue.intvalue &= rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("|="))
            lhsValue.intvalue |= rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("^="))
            lhsValue.intvalue ^= rhsValue.intvalue;
        else
            return false;
    } else if (lhsValue.isFloatValue()) {
        if (assign == MatchCompiler::makeConstString("+="))
            lhsValue.floatValue += rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("-="))
            lhsValue.floatValue -= rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("*="))
            lhsValue.floatValue *= rhsValue.intvalue;
        else if (assign == MatchCompiler::makeConstString("/="))
            lhsValue.floatValue /= rhsValue.intvalue;
        else
            return false;
    } else {
        return false;
    }
    return true;
}

static bool isEscapeScope(const Token* tok, TokenList * tokenlist)
{
    if (!match59(tok))
        return false;
    return findmatch60(tok, tok->link())  ||
           (tokenlist && tokenlist->getSettings()->library.isScopeNoReturn(tok->link(), nullptr));
}

static bool valueFlowForward(Token * const               startToken,
                             const Token * const         endToken,
                             const Variable * const      var,
                             const unsigned int          varid,
                             std::list<ValueFlow::Value> values,
                             const bool                  constValue,
                             const bool                  subFunction,
                             TokenList * const           tokenlist,
                             ErrorLogger * const         errorLogger,
                             const Settings * const      settings)
{
    int indentlevel = 0;
    unsigned int number_of_if = 0;
    int varusagelevel = -1;
    bool returnStatement = false;  // current statement is a return, stop analysis at the ";"
    bool read = false;  // is variable value read?

    if (values.empty())
        return true;

    for (Token *tok2 = startToken; tok2 && tok2 != endToken; tok2 = tok2->next()) {
        if (indentlevel >= 0 && tok2->str() == MatchCompiler::makeConstString("{"))
            ++indentlevel;
        else if (indentlevel >= 0 && tok2->str() == MatchCompiler::makeConstString("}")) {
            --indentlevel;
            if (indentlevel <= 0 && isReturnScope(tok2) && match61(tok2->link()->previous())) {
                const Token *condition = tok2->link();
                const bool iselse = match62(condition->tokAt(-2));
                if (iselse)
                    condition = condition->linkAt(-2);
                if (condition && match4(condition->previous()))
                    condition = condition->linkAt(-1)->astOperand2();
                else
                    condition = nullptr;
                if (!condition) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " valueFlowForward, bailing out since it's unknown if conditional return is executed");
                    return false;
                }

                bool bailoutflag = false;
                const Token * const start1 = iselse ? tok2->link()->linkAt(-2) : nullptr;
                for (std::list<ValueFlow::Value>::iterator it = values.begin(); it != values.end();) {
                    if (!iselse && conditionIsTrue(condition, getProgramMemory(condition->astParent(), varid, *it))) {
                        bailoutflag = true;
                        break;
                    }
                    if (iselse && conditionIsFalse(condition, getProgramMemory(condition->astParent(), varid, *it))) {
                        bailoutflag = true;
                        break;
                    }
                    if (iselse && it->isPossible() && isVariableChanged(start1, start1->link(), varid, var->isGlobal(), settings, tokenlist->isCPP()))
                        values.erase(it++);
                    else
                        ++it;
                }
                if (bailoutflag) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " valueFlowForward, conditional return is assumed to be executed");
                    return false;
                }

                if (values.empty())
                    return true;
            } else if (indentlevel <= 0 &&
                       match63(tok2->link()->previous()) &&
                       !isReturnScope(tok2->link()->tokAt(-2)) &&
                       isVariableChanged(tok2->link(), tok2, varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                changeKnownToPossible(values);
            }
        }

        // skip lambda functions
        // TODO: handle lambda functions
        if (match64(tok2)) {
            Token *lambdaEndToken = const_cast<Token *>(findLambdaEndToken(tok2->next()));
            if (lambdaEndToken) {
                tok2 = lambdaEndToken;
                continue;
            }
        }

        if (match44(tok2) || tok2->str() == MatchCompiler::makeConstString("case")) {
            changeKnownToPossible(values);
            tok2 = tok2->tokAt(2);
            continue;
        }

        else if ((var->isGlobal() || tok2->str() == MatchCompiler::makeConstString("asm")) && match52(tok2) && match65(tok2->linkAt(1))) {
            return false;
        }

        // Skip sizeof etc
        else if (match43(tok2))
            tok2 = tok2->linkAt(1);

        else if (match63(tok2)) {
            // Should scope be skipped because variable value is checked?
            bool skipelse = false;
            const Token *condition = tok2->linkAt(-1);
            condition = condition ? condition->linkAt(-1) : nullptr;
            condition = condition ? condition->astOperand2() : nullptr;
            for (const ValueFlow::Value &v : values) {
                if (conditionIsTrue(condition, getProgramMemory(tok2, varid, v))) {
                    skipelse = true;
                    break;
                }
            }
            if (skipelse) {
                tok2 = tok2->linkAt(1);
                continue;
            }
        }

        else if (match41(tok2)) {
            const Token *start = tok2->next();
            const Token *end   = start->link();
            if (match40(end))
                end = end->linkAt(2);

            if (isVariableChanged(start, end, varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " valueFlowForward, assignment in do-while");
                return false;
            }

            handleKnownValuesInLoop(start, end, &values, varid, var->isGlobal(), settings);
        }

        // conditional block of code that assigns variable..
        else if (!tok2->varId() && match52(tok2) && match4(tok2->linkAt(1))) {
            // is variable changed in condition?
            if (isVariableChanged(tok2->next(), tok2->next()->link(), varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " valueFlowForward, assignment in condition");
                return false;
            }

            // if known variable is changed in loop body, change it to a possible value..
            if (match66(tok2))
                handleKnownValuesInLoop(tok2, tok2->linkAt(1)->linkAt(1), &values, varid, var->isGlobal(), settings);

            // Set values in condition
            for (Token* tok3 = tok2->tokAt(2); tok3 != tok2->next()->link(); tok3 = tok3->next()) {
                if (tok3->varId() == varid) {
                    for (const ValueFlow::Value &v : values)
                        setTokenValue(tok3, v, settings);
                } else if (match67(tok3)) {
                    break;
                }
            }

            const Token * const condTok = tok2->next()->astOperand2();
            const bool condAlwaysTrue = (condTok && condTok->hasKnownIntValue() && condTok->values().front().intvalue != 0);
            const bool condAlwaysFalse = (condTok && condTok->hasKnownIntValue() && condTok->values().front().intvalue == 0);

            // Should scope be skipped because variable value is checked?
            std::list<ValueFlow::Value> truevalues;
            std::list<ValueFlow::Value> falsevalues;
            for (const ValueFlow::Value &v : values) {
                if (condAlwaysTrue) {
                    truevalues.push_back(v);
                    continue;
                }
                if (condAlwaysFalse) {
                    falsevalues.push_back(v);
                    continue;
                }
                const ProgramMemory &programMemory = getProgramMemory(tok2, varid, v);
                if (subFunction && conditionIsTrue(condTok, programMemory))
                    truevalues.push_back(v);
                else if (!subFunction && !conditionIsFalse(condTok, programMemory))
                    truevalues.push_back(v);
                if (condAlwaysFalse)
                    falsevalues.push_back(v);
                else if (conditionIsFalse(condTok, programMemory))
                    falsevalues.push_back(v);
                else if (!subFunction && !conditionIsTrue(condTok, programMemory))
                    falsevalues.push_back(v);
            }
            if (truevalues.size() != values.size() || condAlwaysTrue) {
                // '{'
                const Token * const startToken1 = tok2->linkAt(1)->next();

                bool vfresult = valueFlowForward(startToken1->next(),
                                                 startToken1->link(),
                                                 var,
                                                 varid,
                                                 truevalues,
                                                 constValue,
                                                 subFunction,
                                                 tokenlist,
                                                 errorLogger,
                                                 settings);

                if (!condAlwaysFalse && isVariableChanged(startToken1, startToken1->link(), varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                    removeValues(values, truevalues);
                    changeKnownToPossible(values);
                }

                // goto '}'
                tok2 = startToken1->link();

                if (isReturnScope(tok2) || !vfresult) {
                    if (condAlwaysTrue)
                        return false;
                    removeValues(values, truevalues);
                }

                if (match62(tok2)) {
                    const Token * const startTokenElse = tok2->tokAt(2);

                    vfresult = valueFlowForward(startTokenElse->next(),
                                                startTokenElse->link(),
                                                var,
                                                varid,
                                                falsevalues,
                                                constValue,
                                                subFunction,
                                                tokenlist,
                                                errorLogger,
                                                settings);

                    if (!condAlwaysTrue && isVariableChanged(startTokenElse, startTokenElse->link(), varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                        removeValues(values, falsevalues);
                        changeKnownToPossible(values);
                    }

                    // goto '}'
                    tok2 = startTokenElse->link();

                    if (isReturnScope(tok2) || !vfresult) {
                        if (condAlwaysFalse)
                            return false;
                        removeValues(values, falsevalues);
                    }
                }

                continue;
            }

            Token * const start = tok2->linkAt(1)->next();
            Token * const end   = start->link();
            const bool varusage = (indentlevel >= 0 && constValue && number_of_if == 0U) ?
                                  isVariableChanged(start,end,varid,var->isGlobal(),settings, tokenlist->isCPP()) :
                                  (nullptr != findmatch45(start, end, varid) );
            if (!read) {
                read = bool(nullptr != findmatch68(tok2, end, varid) );
            }
            if (varusage) {
                varusagelevel = indentlevel;

                if (indentlevel < 0 && tok2->str() == MatchCompiler::makeConstString("switch"))
                    return false;

                // TODO: don't check noreturn scopes
                if (read && (number_of_if > 0U || findmatch45(tok2, start, varid) )) {
                    // Set values in condition
                    const Token * const condend = tok2->linkAt(1);
                    for (Token *condtok = tok2; condtok != condend; condtok = condtok->next()) {
                        if (condtok->varId() == varid) {
                            for (const ValueFlow::Value &v : values)
                                setTokenValue(condtok, v, settings);
                        }
                        if (match67(condtok))
                            break;
                    }
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " is assigned in conditional code");
                    return false;
                }

                if (var->isStatic()) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " bailout when conditional code that contains var is seen");
                    return false;
                }

                // Forward known values in the else branch
                if (match62(end)) {
                    std::list<ValueFlow::Value> knownValues;
                    std::copy_if(values.begin(), values.end(), std::back_inserter(knownValues), std::mem_fn(&ValueFlow::Value::isKnown));
                    valueFlowForward(end->tokAt(2),
                                     end->linkAt(2),
                                     var,
                                     varid,
                                     knownValues,
                                     constValue,
                                     subFunction,
                                     tokenlist,
                                     errorLogger,
                                     settings);
                }

                // Remove conditional values
                std::list<ValueFlow::Value>::iterator it;
                for (it = values.begin(); it != values.end();) {
                    if (it->condition || it->conditional)
                        values.erase(it++);
                    else {
                        it->changeKnownToPossible();
                        ++it;
                    }
                }
            }

            // stop after conditional return scopes that are executed
            if (isReturnScope(end)) {
                std::list<ValueFlow::Value>::iterator it;
                for (it = values.begin(); it != values.end();) {
                    if (conditionIsTrue(tok2->next()->astOperand2(), getProgramMemory(tok2, varid, *it)))
                        values.erase(it++);
                    else
                        ++it;
                }
                if (values.empty())
                    return false;
            }

            // noreturn scopes..
            if ((number_of_if > 0 || findmatch45(tok2, start, varid) ) &&
                (isEscapeScope(start, tokenlist) ||
                 (match62(end) && isEscapeScope(end->tokAt(2), tokenlist)))) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + ". noreturn conditional scope.");
                return false;
            }

            if (isVariableChanged(start, end, varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                if ((!read || number_of_if == 0) &&
                    match24(tok2) &&
                    !(match62(end) &&
                      isEscapeScope(end->tokAt(2), tokenlist))) {
                    ++number_of_if;
                    tok2 = end;
                } else {
                    // loop that conditionally set variable and then break => either loop condition is
                    // redundant or the variable can be unchanged after the loop.
                    bool loopCondition = false;
                    if (match39(tok2) && match69(tok2->next()->astOperand2()))
                        loopCondition = true;
                    else if (match56(tok2) &&
                             match70(tok2->next()->astOperand2()) &&
                             match70(tok2->next()->astOperand2()->astOperand2()) &&
                             match69(tok2->next()->astOperand2()->astOperand2()->astOperand1()))
                        loopCondition = true;

                    bool bail = true;
                    if (loopCondition) {
                        const Token *tok3 = findmatch45(start, end, varid) ;
                        if (match71(tok3, varid) &&
                            tok3->scope()->bodyEnd                &&
                            match72(tok3->scope()->bodyEnd->tokAt(-3)) &&
                            !findmatch45(tok3->next(), end, varid) ) {
                            bail = false;
                            tok2 = end;
                        }
                    }

                    if (bail) {
                        if (settings->debugwarnings)
                            bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " is assigned in conditional code");
                        return false;
                    }
                }
            }
        }

        else if (match73(tok2) && match74(tok2->linkAt(1))) {
            const Token * const arg = tok2->next()->astOperand2();
            if (arg != nullptr && arg->str() != MatchCompiler::makeConstString(",")) {
                // Should scope be skipped because variable value is checked?
                for (std::list<ValueFlow::Value>::iterator it = values.begin(); it != values.end();) {
                    if (conditionIsFalse(arg, getProgramMemory(tok2, varid, *it)))
                        values.erase(it++);
                    else
                        ++it;
                }
            }
        }

        else if (tok2->str() == MatchCompiler::makeConstString("}") && indentlevel == varusagelevel) {
            ++number_of_if;

            // Set "conditional" flag for all values
            std::list<ValueFlow::Value>::iterator it;
            for (it = values.begin(); it != values.end(); ++it) {
                it->conditional = true;
                it->changeKnownToPossible();
            }

            if (match62(tok2))
                tok2 = tok2->linkAt(2);
        }

        else if (match75(tok2)) {
            const Scope *scope = tok2->scope();
            if (indentlevel > 0) {
                const Token *tok3 = tok2->tokAt(2);
                int indentlevel2 = indentlevel;
                while (indentlevel2 > 0 &&
                       tok3->str() == MatchCompiler::makeConstString("}") &&
                       match76(tok3->link()->previous())) {
                    indentlevel2--;
                    tok3 = tok3->next();
                    if (tok3 && tok3->str() == MatchCompiler::makeConstString(";"))
                        tok3 = tok3->next();
                }
                if (indentlevel2 > 0)
                    continue;
                scope = tok3->scope();
                indentlevel = 0;
            }
            if (tok2->str() == MatchCompiler::makeConstString("break")) {
                if (scope && scope->type == Scope::eSwitch) {
                    tok2 = const_cast<Token *>(scope->bodyEnd);
                    if (tok2 == endToken)
                        break;
                    --indentlevel;
                    changeKnownToPossible(values);
                    continue;
                }
            }
            if (settings->debugwarnings)
                bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + ". noreturn conditional scope.");
            return false;
        }

        else if (indentlevel <= 0 && match77(tok2))
            returnStatement = true;

        else if (returnStatement && tok2->str() == MatchCompiler::makeConstString(";"))
            return false;

        // If a ? is seen and it's known that the condition is true/false..
        else if (tok2->str() == MatchCompiler::makeConstString("?")) {
            const Token *condition = tok2->astOperand1();
            const Token *op2 = tok2->astOperand2();
            if (!condition || !op2) // Ticket #6713
                continue;

            if (condition->hasKnownIntValue()) {
                const ValueFlow::Value &condValue = condition->values().front();
                const Token *expr = (condValue.intvalue != 0) ? op2->astOperand1() : op2->astOperand2();
                for (const ValueFlow::Value &v : values)
                    valueFlowAST(const_cast<Token*>(expr), varid, v, settings);
                if (isVariableChangedByFunctionCall(expr, varid, settings, nullptr))
                    changeKnownToPossible(values);
            } else {
                for (const ValueFlow::Value &v : values) {
                    const ProgramMemory programMemory(getProgramMemory(tok2, varid, v));
                    if (conditionIsTrue(condition, programMemory))
                        valueFlowAST(const_cast<Token*>(op2->astOperand1()), varid, v, settings);
                    else if (conditionIsFalse(condition, programMemory))
                        valueFlowAST(const_cast<Token*>(op2->astOperand2()), varid, v, settings);
                    else
                        valueFlowAST(const_cast<Token*>(op2), varid, v, settings);
                }
                if (isVariableChangedByFunctionCall(op2, varid, settings, nullptr))
                    changeKnownToPossible(values);
            }

            // Skip conditional expressions..
            const Token * const questionToken = tok2;
            while (tok2->astOperand1() || tok2->astOperand2()) {
                if (tok2->astOperand2())
                    tok2 = const_cast<Token*>(tok2->astOperand2());
                else if (tok2->isUnaryPreOp())
                    tok2 = const_cast<Token*>(tok2->astOperand1());
                else
                    break;
            }
            tok2 = tok2->next();

            if (isVariableChanged(questionToken, questionToken->astOperand2(), varid, false, settings, tokenlist->isCPP()) &&
                isVariableChanged(questionToken->astOperand2(), tok2, varid, false, settings, tokenlist->isCPP())) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "variable " + var->name() + " valueFlowForward, assignment in condition");
                return false;

            }
        }

        else if (tok2->varId() == varid) {
            // compound assignment, known value in rhs
            if (match78(tok2->previous()) &&
                tok2->next()->str() != MatchCompiler::makeConstString("=") &&
                tok2->next()->astOperand2() &&
                tok2->next()->astOperand2()->hasKnownIntValue()) {

                const ValueFlow::Value &rhsValue = tok2->next()->astOperand2()->values().front();
                const std::string &assign = tok2->next()->str();
                std::list<ValueFlow::Value>::iterator it;
                // Erase values that are not int values..
                for (it = values.begin(); it != values.end();) {
                    if (!evalAssignment(*it, assign, rhsValue)) {
                        it = values.erase(it);
                    } else {
                        const std::string info("Compound assignment '" + assign + "', assigned value is " + it->infoString());
                        it->errorPath.emplace_back(tok2, info);

                        ++it;
                    }

                }
                if (values.empty()) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "compound assignment of " + tok2->str());
                    return false;
                }
            }

            // bailout: assignment
            else if (match78(tok2->previous())) {
                // simplify rhs
                std::stack<Token *> rhs;
                rhs.push(const_cast<Token *>(tok2->next()->astOperand2()));
                while (!rhs.empty()) {
                    Token *rtok = rhs.top();
                    rhs.pop();
                    if (!rtok)
                        continue;
                    if (rtok->str() == MatchCompiler::makeConstString("(") && match79(rtok->astOperand1()))
                        continue;
                    if (match80(rtok))
                        continue;
                    if (rtok->varId() == varid) {
                        for (const ValueFlow::Value &v : values)
                            setTokenValue(rtok, v, settings);
                    }
                    rhs.push(const_cast<Token *>(rtok->astOperand1()));
                    rhs.push(const_cast<Token *>(rtok->astOperand2()));
                }
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "assignment of " + tok2->str());
                return false;
            }

            // bailout: possible assignment using >>
            if (isLikelyStreamRead(tokenlist->isCPP(), tok2->previous())) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "Possible assignment of " + tok2->str() + " using " + tok2->strAt(-1));
                return false;
            }

            // skip if variable is conditionally used in ?: expression
            if (const Token *parent = skipValueInConditionalExpression(tok2)) {
                if (settings->debugwarnings)
                    bailout(tokenlist,
                            errorLogger,
                            tok2,
                            "no simplification of " + tok2->str() + " within " + (match38(parent) ? "?:" : parent->str()) + " expression");
                const Token *astTop = parent->astTop();
                if (match56(astTop->astOperand1()))
                    tok2 = const_cast<Token*>(astTop->link());
                continue;
            }

            {
                // Is variable usage protected by && || ?:
                const Token *tok3 = tok2;
                const Token *parent = tok3->astParent();
                while (parent && !match81(parent)) {
                    tok3 = parent;
                    parent = parent->astParent();
                }
                const bool conditional = parent && (parent->str() == MatchCompiler::makeConstString(":") || parent->astOperand2() == tok3);

                for (const ValueFlow::Value &v : values) {
                    if (!conditional || !v.conditional)
                        setTokenValue(tok2, v, settings);
                }
            }

            // increment/decrement
            if (match33(tok2->previous()) || match34(tok2)) {
                std::list<ValueFlow::Value>::iterator it;
                // Erase values that are not int values..
                for (it = values.begin(); it != values.end();) {
                    if (!it->isIntValue())
                        it = values.erase(it);
                    else
                        ++it;
                }
                if (values.empty()) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "increment/decrement of " + tok2->str());
                    return false;
                }
                const bool pre   = match82(tok2->previous());
                Token * const op = pre ? tok2->previous() : tok2->next();
                const bool inc   = (op->str() == MatchCompiler::makeConstString("++"));
                // Perform increment/decrement..
                for (it = values.begin(); it != values.end(); ++it) {
                    if (!pre)
                        setTokenValue(op, *it, settings);
                    it->intvalue += (inc ? 1 : -1);
                    if (pre)
                        setTokenValue(op, *it, settings);
                    const std::string info(tok2->str() + " is " + std::string(inc ? "incremented" : "decremented") + "', new value is " + it->infoString());
                    it->errorPath.emplace_back(tok2, info);
                }
            }

            // bailout if address of var is taken..
            if (tok2->astParent() && tok2->astParent()->isUnaryOp("&")) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "Taking address of " + tok2->str());
                return false;
            }

            // bailout if reference is created..
            if (tok2->astParent() && match28(tok2->astParent()->tokAt(-2))) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "Reference of " + tok2->str());
                return false;
            }

            // assigned by subfunction?
            bool inconclusive = false;
            if (isVariableChangedByFunctionCall(tok2, settings, &inconclusive)) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "possible assignment of " + tok2->str() + " by subfunction");
                return false;
            }
            if (inconclusive) {
                for (ValueFlow::Value &v : values)
                    v.setInconclusive();
            }
            if (tok2->strAt(1) == MatchCompiler::makeConstString(".") && tok2->next()->originalName() != MatchCompiler::makeConstString("->")) {
                if (settings->inconclusive) {
                    for (ValueFlow::Value &v : values)
                        v.setInconclusive();
                } else {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "possible assignment of " + tok2->str() + " by member function");
                    return false;
                }
            }
        }

        // Lambda function
        if (match64(tok2) &&
            match83(tok2->linkAt(1)) &&
            match4(tok2->linkAt(1)->linkAt(1))) {
            const Token *bodyStart = tok2->linkAt(1)->linkAt(1)->next();
            if (isVariableChanged(bodyStart, bodyStart->link(), varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "valueFlowForward, " + var->name() + " is changed in lambda function");
                return false;
            }
        }
    }
    return true;
}

static bool isStdMoveOrStdForwarded(Token * tok, ValueFlow::Value::MoveKind * moveKind, Token ** varTok = nullptr)
{
    if (tok->str() != MatchCompiler::makeConstString("std"))
        return false;
    ValueFlow::Value::MoveKind kind = ValueFlow::Value::NonMovedVariable;
    Token * variableToken = nullptr;
    if (match84(tok)) {
        variableToken = tok->tokAt(4);
        kind = ValueFlow::Value::MovedVariable;
    } else if (match85(tok)) {
        const Token * const leftAngle = tok->tokAt(3);
        Token * rightAngle = leftAngle->link();
        if (match86(rightAngle)) {
            variableToken = rightAngle->tokAt(2);
            kind = ValueFlow::Value::ForwardedVariable;
        }
    }
    if (!variableToken)
        return false;
    if (variableToken->strAt(2) == MatchCompiler::makeConstString(".")) // Only partially moved
        return false;

    if (moveKind != nullptr)
        *moveKind = kind;
    if (varTok != nullptr)
        *varTok = variableToken;
    return true;
}

static bool isOpenParenthesisMemberFunctionCallOfVarId(const Token * openParenthesisToken, unsigned int varId)
{
    const Token * varTok = openParenthesisToken->tokAt(-3);
    return match87(varTok, varId) &&
           varTok->next()->originalName() == emptyString;
}

static const Token * nextAfterAstRightmostLeaf(Token const * tok)
{
    const Token * rightmostLeaf = tok;
    if (!rightmostLeaf || !rightmostLeaf->astOperand1())
        return nullptr;
    do {
        if (rightmostLeaf->astOperand2())
            rightmostLeaf = rightmostLeaf->astOperand2();
        else
            rightmostLeaf = rightmostLeaf->astOperand1();
    } while (rightmostLeaf->astOperand1());
    return rightmostLeaf->next();
}

static const Token * findOpenParentesisOfMove(const Token * moveVarTok)
{
    const Token * tok = moveVarTok;
    while (tok && tok->str() != MatchCompiler::makeConstString("("))
        tok = tok->previous();
    return tok;
}

static const Token * findEndOfFunctionCallForParameter(const Token * parameterToken)
{
    if (!parameterToken)
        return nullptr;
    const Token * parent = parameterToken->astParent();
    while (parent && !parent->isOp() && parent->str() != MatchCompiler::makeConstString("("))
        parent = parent->astParent();
    if (!parent)
        return nullptr;
    return nextAfterAstRightmostLeaf(parent);
}

static void valueFlowAfterMove(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    if (!tokenlist->isCPP() || settings->standards.cpp < Standards::CPP11)
        return;
    for (const Scope * scope : symboldatabase->functionScopes) {
        if (!scope)
            continue;
        const Token * start = scope->bodyStart;
        if (scope->function) {
            const Token * memberInitializationTok = scope->function->constructorMemberInitialization();
            if (memberInitializationTok)
                start = memberInitializationTok;
        }

        for (Token* tok = const_cast<Token*>(start); tok != scope->bodyEnd; tok = tok->next()) {
            Token * varTok;
            if (match88(tok) && tok->next()->originalName() == emptyString) {
                varTok = tok;
                ValueFlow::Value value;
                value.valueType = ValueFlow::Value::MOVED;
                value.moveKind = ValueFlow::Value::NonMovedVariable;
                value.errorPath.emplace_back(tok, "Calling " + tok->next()->expressionString() + " makes " + tok->str() + " 'non-moved'");
                value.setKnown();
                std::list<ValueFlow::Value> values;
                values.push_back(value);

                const Variable *var = varTok->variable();
                if (!var || (!var->isLocal() && !var->isArgument()))
                    continue;
                const unsigned int varId = varTok->varId();
                const Token * const endOfVarScope = var->typeStartToken()->scope()->bodyEnd;
                setTokenValue(varTok, value, settings);
                valueFlowForward(varTok->next(), endOfVarScope, var, varId, values, false, false, tokenlist, errorLogger, settings);
                continue;
            }
            ValueFlow::Value::MoveKind moveKind;
            if (!isStdMoveOrStdForwarded(tok, &moveKind, &varTok))
                continue;
            const unsigned int varId = varTok->varId();
            // x is not MOVED after assignment if code is:  x = ... std::move(x) .. ;
            const Token *parent = tok->astParent();
            while (parent && parent->str() != MatchCompiler::makeConstString("=") && parent->str() != MatchCompiler::makeConstString("return") &&
                   !(parent->str() == MatchCompiler::makeConstString("(") && isOpenParenthesisMemberFunctionCallOfVarId(parent, varId)))
                parent = parent->astParent();
            if (parent &&
                (parent->str() == MatchCompiler::makeConstString("return") || // MOVED in return statement
                 parent->str() == MatchCompiler::makeConstString("("))) // MOVED in self assignment, isOpenParenthesisMemberFunctionCallOfVarId == true
                continue;
            if (parent && parent->astOperand1()->varId() == varId)
                continue;
            const Variable *var = varTok->variable();
            if (!var)
                continue;
            const Token * const endOfVarScope = var->typeStartToken()->scope()->bodyEnd;

            ValueFlow::Value value;
            value.valueType = ValueFlow::Value::MOVED;
            value.moveKind = moveKind;
            if (moveKind == ValueFlow::Value::MovedVariable)
                value.errorPath.emplace_back(tok, "Calling std::move(" + varTok->str() + ")");
            else // if (moveKind == ValueFlow::Value::ForwardedVariable)
                value.errorPath.emplace_back(tok, "Calling std::forward(" + varTok->str() + ")");
            value.setKnown();
            std::list<ValueFlow::Value> values;
            values.push_back(value);
            const Token * openParentesisOfMove = findOpenParentesisOfMove(varTok);
            const Token * endOfFunctionCall = findEndOfFunctionCallForParameter(openParentesisOfMove);
            if (endOfFunctionCall)
                valueFlowForward(const_cast<Token *>(endOfFunctionCall), endOfVarScope, var, varId, values, false, false, tokenlist, errorLogger, settings);
        }
    }
}

static void valueFlowAfterAssign(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    for (const Scope * scope : symboldatabase->functionScopes) {
        std::set<unsigned int> aliased;
        for (Token* tok = const_cast<Token*>(scope->bodyStart); tok != scope->bodyEnd; tok = tok->next()) {
            // Alias
            if (tok->isUnaryOp("&")) {
                aliased.insert(tok->astOperand1()->varId());
                continue;
            }

            // Assignment
            if ((tok->str() != MatchCompiler::makeConstString("=")) || (tok->astParent()))
                continue;

            // Lhs should be a variable
            if (!tok->astOperand1() || !tok->astOperand1()->varId())
                continue;
            const unsigned int varid = tok->astOperand1()->varId();
            if (aliased.find(varid) != aliased.end())
                continue;
            const Variable *var = tok->astOperand1()->variable();
            if (!var || (!var->isLocal() && !var->isGlobal() && !var->isArgument()))
                continue;

            const Token * const endOfVarScope = var->typeStartToken()->scope()->bodyEnd;

            // Rhs values..
            if (!tok->astOperand2() || tok->astOperand2()->values().empty())
                continue;

            std::list<ValueFlow::Value> values = tok->astOperand2()->values();
            for (std::list<ValueFlow::Value>::iterator it = values.begin(); it != values.end(); ++it) {
                const std::string info = "Assignment '" + tok->expressionString() + "', assigned value is " + it->infoString();
                it->errorPath.emplace_back(tok->astOperand2(), info);
            }
            const bool constValue = tok->astOperand2()->isNumber();

            if (tokenlist->isCPP() && match89(var->typeStartToken())) {
                std::list<ValueFlow::Value>::iterator it;
                for (it = values.begin(); it != values.end(); ++it) {
                    if (it->isIntValue())
                        it->intvalue = (it->intvalue != 0);
                    if (it->isTokValue())
                        it ->intvalue = (it->tokvalue != 0);
                }
            }

            // Static variable initialisation?
            if (var->isStatic() && var->nameToken() == tok->astOperand1())
                changeKnownToPossible(values);

            // Skip RHS
            const Token * nextExpression = nextAfterAstRightmostLeaf(tok);

            valueFlowForward(const_cast<Token *>(nextExpression), endOfVarScope, var, varid, values, constValue, false, tokenlist, errorLogger, settings);
        }
    }
}

static void valueFlowAfterCondition(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    for (const Scope * scope : symboldatabase->functionScopes) {
        std::set<unsigned> aliased;
        for (Token* tok = const_cast<Token*>(scope->bodyStart); tok != scope->bodyEnd; tok = tok->next()) {
            const Token * vartok = nullptr;
            const Token * numtok = nullptr;
            const Token * lowertok = nullptr;
            const Token * uppertok = nullptr;

            if (match90(tok))
                aliased.insert(tok->tokAt(2)->varId());

            // Comparison
            if (match91(tok)) {
                if (!tok->astOperand1() || !tok->astOperand2())
                    continue;
                if (tok->astOperand1()->hasKnownIntValue()) {
                    numtok = tok->astOperand1();
                    vartok = tok->astOperand2();
                } else {
                    numtok = tok->astOperand2();
                    vartok = tok->astOperand1();
                }
                if (vartok->str() == MatchCompiler::makeConstString("=") && vartok->astOperand1() && vartok->astOperand2())
                    vartok = vartok->astOperand1();
                if (!vartok->isName())
                    continue;
            } else if (match92(tok)) {
                if (!tok->astOperand1() || !tok->astOperand2())
                    continue;
                if (tok->astOperand1()->hasKnownIntValue()) {
                    uppertok = tok->astOperand1();
                    vartok = tok->astOperand2();
                } else {
                    lowertok = tok->astOperand2();
                    vartok = tok->astOperand1();
                }
                if (vartok->str() == MatchCompiler::makeConstString("=") && vartok->astOperand1() && vartok->astOperand2())
                    vartok = vartok->astOperand1();
                if (!vartok->isName())
                    continue;
            } else if (match93(tok)) {
                if (!tok->astOperand1() || !tok->astOperand2())
                    continue;
                if (tok->astOperand1()->hasKnownIntValue()) {
                    lowertok = tok->astOperand1();
                    vartok = tok->astOperand2();
                } else {
                    uppertok = tok->astOperand2();
                    vartok = tok->astOperand1();
                }
                if (vartok->str() == MatchCompiler::makeConstString("=") && vartok->astOperand1() && vartok->astOperand2())
                    vartok = vartok->astOperand1();
                if (!vartok->isName())
                    continue;
            } else if (tok->str() == MatchCompiler::makeConstString("!")) {
                vartok = tok->astOperand1();
                numtok = nullptr;
                if (!vartok || !vartok->isName())
                    continue;

            } else if (tok->isName() &&
                       (match94(tok->astParent()) ||
                        match95(tok->tokAt(-2)))) {
                vartok = tok;
                numtok = nullptr;

            } else {
                continue;
            }

            if (numtok && !numtok->hasKnownIntValue())
                continue;
            if (lowertok && !lowertok->hasKnownIntValue())
                continue;
            if (uppertok && !uppertok->hasKnownIntValue())
                continue;

            const unsigned int varid = vartok->varId();
            if (varid == 0U)
                continue;
            const Variable *var = vartok->variable();
            if (!var || !(var->isLocal() || var->isGlobal() || var->isArgument()))
                continue;
            if (aliased.find(varid) != aliased.end()) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, vartok, "variable is aliased so we just skip all valueflow after condition");
                continue;
            }
            std::list<ValueFlow::Value> true_values;
            std::list<ValueFlow::Value> false_values;
            // TODO: We should add all known values
            if (numtok) {
                false_values.emplace_back(tok, numtok->values().front().intvalue);
                true_values.emplace_back(tok, numtok->values().front().intvalue);
            } else if (lowertok) {
                long long v = lowertok->values().front().intvalue;
                true_values.emplace_back(tok, v+1);
                false_values.emplace_back(tok, v);

            } else if (uppertok) {
                long long v = uppertok->values().front().intvalue;
                true_values.emplace_back(tok, v-1);
                false_values.emplace_back(tok, v);

            } else {
                true_values.emplace_back(tok, 0LL);
                false_values.emplace_back(tok, 0LL);

            }

            if (match94(tok->astParent())) {
                Token *parent = const_cast<Token*>(tok->astParent());
                const std::string &op(parent->str());

                if (parent->astOperand1() == tok &&
                    ((op == MatchCompiler::makeConstString("&&") && match96(tok)) ||
                     (op == MatchCompiler::makeConstString("||") && match97(tok)))) {
                    for (; parent && parent->str() == op; parent = const_cast<Token*>(parent->astParent())) {
                        std::stack<Token *> tokens;
                        tokens.push(const_cast<Token*>(parent->astOperand2()));
                        bool assign = false;
                        while (!tokens.empty()) {
                            Token *rhstok = tokens.top();
                            tokens.pop();
                            if (!rhstok)
                                continue;
                            tokens.push(const_cast<Token*>(rhstok->astOperand1()));
                            tokens.push(const_cast<Token*>(rhstok->astOperand2()));
                            if (rhstok->varId() == varid)
                                setTokenValue(rhstok, true_values.front(), settings);
                            else if (match98(rhstok) && match99(rhstok->astOperand1(), varid)) {
                                assign = true;
                                break;
                            }
                        }
                        if (assign)
                            break;
                        while (parent->astParent() && parent == parent->astParent()->astOperand2())
                            parent = const_cast<Token*>(parent->astParent());
                    }
                }
            }

            const Token *top = tok->astTop();
            if (top && match100(top->previous()) && !top->previous()->isExpandedMacro()) {
                // does condition reassign variable?
                if (tok != top->astOperand2() &&
                    match94(top->astOperand2()) &&
                    isVariableChanged(top, top->link(), varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok, "assignment in condition");
                    continue;
                }

                // start token of conditional code
                Token *startTokens[] = { nullptr, nullptr };

                // based on the comparison, should we check the if or while?
                bool check_if = false;
                bool check_else = false;
                if (match101(tok))
                    check_if = true;
                if (match102(tok))
                    check_else = true;

                if (!check_if && !check_else)
                    continue;

                // if astParent is "!" we need to invert codeblock
                {
                    const Token *parent = tok->astParent();
                    while (parent && parent->str() == MatchCompiler::makeConstString("&&"))
                        parent = parent->astParent();
                    if (parent && parent->str() == MatchCompiler::makeConstString("!")) {
                        check_if = !check_if;
                        check_else = !check_else;
                    }
                }

                // determine startToken(s)
                if (check_if && match4(top->link()))
                    startTokens[0] = top->link()->next();
                if (check_else && match62(top->link()->linkAt(1)))
                    startTokens[1] = top->link()->linkAt(1)->tokAt(2);

                bool bail = false;

                for (int i=0; i<2; i++) {
                    const Token * const startToken = startTokens[i];
                    if (!startToken)
                        continue;
                    std::list<ValueFlow::Value> & values = (i==0 ? true_values : false_values);
                    if (values.size() == 1U && match103(tok)) {
                        const Token *parent = tok->astParent();
                        while (parent && parent->str() == MatchCompiler::makeConstString("&&"))
                            parent = parent->astParent();
                        if (parent && parent->str() == MatchCompiler::makeConstString("("))
                            values.front().setKnown();
                    }

                    valueFlowForward(startTokens[i]->next(), startTokens[i]->link(), var, varid, values, true, false, tokenlist, errorLogger, settings);
                    values.front().setPossible();
                    if (isVariableChanged(startTokens[i], startTokens[i]->link(), varid, var->isGlobal(), settings, tokenlist->isCPP())) {
                        // TODO: The endToken should not be startTokens[i]->link() in the valueFlowForward call
                        if (settings->debugwarnings)
                            bailout(tokenlist, errorLogger, startTokens[i]->link(), "valueFlowAfterCondition: " + var->name() + " is changed in conditional block");
                        bail = true;
                        break;
                    }
                }
                if (bail)
                    continue;

                // After conditional code..
                if (match4(top->link())) {
                    Token *after = top->link()->linkAt(1);
                    std::string unknownFunction;
                    if (settings->library.isScopeNoReturn(after, &unknownFunction)) {
                        if (settings->debugwarnings && !unknownFunction.empty())
                            bailout(tokenlist, errorLogger, after, "possible noreturn scope");
                        continue;
                    }

                    const bool dead_if = isReturnScope(after);
                    bool dead_else = false;

                    if (match62(after)) {
                        after = after->linkAt(2);
                        if (match104(after->tokAt(-2))) {
                            if (settings->debugwarnings)
                                bailout(tokenlist, errorLogger, after, "possible noreturn scope");
                            continue;
                        }
                        dead_else = isReturnScope(after);
                    }

                    std::list<ValueFlow::Value> * values = nullptr;
                    if (!dead_if && check_if)
                        values = &true_values;
                    else if (!dead_else && check_else)
                        values = &false_values;

                    if (values) {
                        // TODO: constValue could be true if there are no assignments in the conditional blocks and
                        //       perhaps if there are no && and no || in the condition
                        bool constValue = false;
                        valueFlowForward(after->next(), top->scope()->bodyEnd, var, varid, *values, constValue, false, tokenlist, errorLogger, settings);
                    }
                }
            }
        }
    }
}

static void execute(const Token *expr,
                    ProgramMemory * const programMemory,
                    MathLib::bigint *result,
                    bool *error)
{
    if (!expr)
        *error = true;

    else if (expr->hasKnownIntValue()) {
        *result = expr->values().front().intvalue;
    }

    else if (expr->isNumber()) {
        *result = MathLib::toLongNumber(expr->str());
        if (MathLib::isFloat(expr->str()))
            *error = true;
    }

    else if (expr->varId() > 0) {
        if (!programMemory->getIntValue(expr->varId(), result))
            *error = true;
    }

    else if (expr->isComparisonOp()) {
        MathLib::bigint result1(0), result2(0);
        execute(expr->astOperand1(), programMemory, &result1, error);
        execute(expr->astOperand2(), programMemory, &result2, error);
        if (expr->str() == MatchCompiler::makeConstString("<"))
            *result = result1 < result2;
        else if (expr->str() == MatchCompiler::makeConstString("<="))
            *result = result1 <= result2;
        else if (expr->str() == MatchCompiler::makeConstString(">"))
            *result = result1 > result2;
        else if (expr->str() == MatchCompiler::makeConstString(">="))
            *result = result1 >= result2;
        else if (expr->str() == MatchCompiler::makeConstString("=="))
            *result = result1 == result2;
        else if (expr->str() == MatchCompiler::makeConstString("!="))
            *result = result1 != result2;
    }

    else if (expr->str() == MatchCompiler::makeConstString("=")) {
        execute(expr->astOperand2(), programMemory, result, error);
        if (!*error && expr->astOperand1() && expr->astOperand1()->varId())
            programMemory->setIntValue(expr->astOperand1()->varId(), *result);
        else
            *error = true;
    }

    else if (match82(expr)) {
        if (!expr->astOperand1() || expr->astOperand1()->varId() == 0U)
            *error = true;
        else {
            long long intValue;
            if (!programMemory->getIntValue(expr->astOperand1()->varId(), &intValue))
                *error = true;
            else {
                if (intValue == 0 &&
                    expr->str() == MatchCompiler::makeConstString("--") &&
                    expr->astOperand1()->variable() &&
                    expr->astOperand1()->variable()->typeStartToken()->isUnsigned())
                    *error = true; // overflow
                *result = intValue + (expr->str() == MatchCompiler::makeConstString("++") ? 1 : -1);
                programMemory->setIntValue(expr->astOperand1()->varId(), *result);
            }
        }
    }

    else if (expr->isArithmeticalOp() && expr->astOperand1() && expr->astOperand2()) {
        MathLib::bigint result1(0), result2(0);
        execute(expr->astOperand1(), programMemory, &result1, error);
        execute(expr->astOperand2(), programMemory, &result2, error);
        if (expr->str() == MatchCompiler::makeConstString("+"))
            *result = result1 + result2;
        else if (expr->str() == MatchCompiler::makeConstString("-"))
            *result = result1 - result2;
        else if (expr->str() == MatchCompiler::makeConstString("*")) {
            if (result2 && (result1 > std::numeric_limits<MathLib::bigint>::max()/result2))
                *error = true;
            else
                *result = result1 * result2;
        } else if (result2 == 0)
            *error = true;
        else if (expr->str() == MatchCompiler::makeConstString("/"))
            *result = result1 / result2;
        else if (expr->str() == MatchCompiler::makeConstString("%"))
            *result = result1 % result2;
        else if (expr->str() == MatchCompiler::makeConstString("<<"))  {
            if (result2 < 0 || result1 < 0 || result2 >= MathLib::bigint_bits)  { // don't perform UB
                *error= true;
            } else {
                *result = result1 << result2;
            }
        } else if (expr->str() == MatchCompiler::makeConstString(">>")) {
            if (result2 < 0) { // don't perform UB
                *error=true;
            } else {
                *result = result1 >> result2;
            }
        }
    }

    else if (expr->str() == MatchCompiler::makeConstString("&&")) {
        bool error1 = false;
        execute(expr->astOperand1(), programMemory, result, &error1);
        if (!error1 && *result == 0)
            *result = 0;
        else {
            bool error2 = false;
            execute(expr->astOperand2(), programMemory, result, &error2);
            if (error1 && error2)
                *error = true;
            if (error2)
                *result = 1;
            else
                *result = !!*result;
        }
    }

    else if (expr->str() == MatchCompiler::makeConstString("||")) {
        execute(expr->astOperand1(), programMemory, result, error);
        if (*result == 0 && *error == false)
            execute(expr->astOperand2(), programMemory, result, error);
    }

    else if (expr->str() == MatchCompiler::makeConstString("!")) {
        execute(expr->astOperand1(), programMemory, result, error);
        *result = !(*result);
    }

    else if (expr->str() == MatchCompiler::makeConstString(",") && expr->astOperand1() && expr->astOperand2()) {
        execute(expr->astOperand1(), programMemory, result, error);
        execute(expr->astOperand2(), programMemory, result, error);
    }

    else if (expr->str() == MatchCompiler::makeConstString("[") && expr->astOperand1() && expr->astOperand2()) {
        const Token *tokvalue = nullptr;
        if (!programMemory->getTokValue(expr->astOperand1()->varId(), &tokvalue)) {
            if (expr->astOperand1()->values().size() != 1U || !expr->astOperand1()->values().front().isTokValue()) {
                *error = true;
                return;
            }
            tokvalue = expr->astOperand1()->values().front().tokvalue;
        }
        if (!tokvalue || !tokvalue->isLiteral()) {
            *error = true;
            return;
        }
        const std::string strValue = tokvalue->strValue();
        MathLib::bigint index = 0;
        execute(expr->astOperand2(), programMemory, &index, error);
        if (index >= 0 && index < strValue.size())
            *result = strValue[index];
        else if (index == strValue.size())
            *result = 0;
        else
            *error = true;
    }

    else
        *error = true;
}

static bool valueFlowForLoop1(const Token *tok, unsigned int * const varid, MathLib::bigint * const num1, MathLib::bigint * const num2, MathLib::bigint * const numAfter)
{
    tok = tok->tokAt(2);
    if (!match105(tok))
        return false;
    const Token * const vartok = match106(tok) ? tok : tok->next();
    *varid = vartok->varId();
    tok = vartok->tokAt(2);
    const Token * const num1tok = match107(tok) ? tok : nullptr;
    if (num1tok)
        *num1 = MathLib::toLongNumber(num1tok->str());
    while (match108(tok))
        tok = (tok->str() == MatchCompiler::makeConstString("(")) ? tok->link()->next() : tok->next();
    if (!tok || tok->str() != MatchCompiler::makeConstString(";"))
        return false;
    tok = tok->next();
    const Token *num2tok = nullptr;
    if (match109(tok, vartok->varId())) {
        tok = tok->next();
        num2tok = tok->astOperand2();
        if (num2tok && num2tok->str() == MatchCompiler::makeConstString("(") && !num2tok->astOperand2())
            num2tok = num2tok->astOperand1();
        if (!match110(num2tok)) // TODO: || enlarges the scope of the condition, so it should not cause FP, but it should no lnger be part of this pattern as soon as valueFlowForLoop2 can handle an unknown RHS of || better
            num2tok = nullptr;
    }
    if (!num2tok)
        return false;
    *num2 = MathLib::toLongNumber(num2tok->str()) - ((tok->str()==MatchCompiler::makeConstString("<=")) ? 0 : 1);
    *numAfter = *num2 + 1;
    if (!num1tok)
        *num1 = *num2;
    while (tok && tok->str() != MatchCompiler::makeConstString(";"))
        tok = tok->next();
    if (!match111(tok, vartok->varId()) && !match112(tok, vartok->varId()))
        return false;
    return true;
}

static bool valueFlowForLoop2(const Token *tok,
                              ProgramMemory *memory1,
                              ProgramMemory *memory2,
                              ProgramMemory *memoryAfter)
{
    // for ( firstExpression ; secondExpression ; thirdExpression )
    const Token *firstExpression  = tok->next()->astOperand2()->astOperand1();
    const Token *secondExpression = tok->next()->astOperand2()->astOperand2()->astOperand1();
    const Token *thirdExpression = tok->next()->astOperand2()->astOperand2()->astOperand2();

    ProgramMemory programMemory;
    MathLib::bigint result(0);
    bool error = false;
    execute(firstExpression, &programMemory, &result, &error);
    if (error)
        return false;
    execute(secondExpression, &programMemory, &result, &error);
    if (result == 0) // 2nd expression is false => no looping
        return false;
    if (error) {
        // If a variable is reassigned in second expression, return false
        std::stack<const Token *> tokens;
        tokens.push(secondExpression);
        while (!tokens.empty()) {
            const Token *t = tokens.top();
            tokens.pop();
            if (!t)
                continue;
            if (t->str() == MatchCompiler::makeConstString("=") && t->astOperand1() && programMemory.hasValue(t->astOperand1()->varId()))
                // TODO: investigate what variable is assigned.
                return false;
            tokens.push(t->astOperand1());
            tokens.push(t->astOperand2());
        }
    }

    ProgramMemory startMemory(programMemory);
    ProgramMemory endMemory;

    unsigned int maxcount = 10000;
    while (result != 0 && !error && --maxcount) {
        endMemory = programMemory;
        execute(thirdExpression, &programMemory, &result, &error);
        if (!error)
            execute(secondExpression, &programMemory, &result, &error);
    }

    memory1->swap(startMemory);
    if (!error) {
        memory2->swap(endMemory);
        memoryAfter->swap(programMemory);
    }

    return true;
}

static void valueFlowForLoopSimplify(Token * const bodyStart, const unsigned int varid, bool globalvar, const MathLib::bigint value, TokenList *tokenlist, ErrorLogger *errorLogger, const Settings *settings)
{
    const Token * const bodyEnd = bodyStart->link();

    // Is variable modified inside for loop
    if (isVariableChanged(bodyStart, bodyEnd, varid, globalvar, settings, tokenlist->isCPP()))
        return;

    for (Token *tok2 = bodyStart->next(); tok2 != bodyEnd; tok2 = tok2->next()) {
        if (tok2->varId() == varid) {
            const Token * parent = tok2->astParent();
            while (parent) {
                const Token * const p = parent;
                parent = parent->astParent();
                if (!parent || parent->str() == MatchCompiler::makeConstString(":"))
                    break;
                if (parent->str() == MatchCompiler::makeConstString("?")) {
                    if (parent->astOperand2() != p)
                        parent = nullptr;
                    break;
                }
            }
            if (parent) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "For loop variable " + tok2->str() + " stopping on ?");
                continue;
            }

            ValueFlow::Value value1(value);
            value1.varId = tok2->varId();
            setTokenValue(tok2, value1, settings);
        }

        if (match94(tok2)) {
            const ProgramMemory programMemory(getProgramMemory(tok2->astTop(), varid, ValueFlow::Value(value)));
            if ((tok2->str() == MatchCompiler::makeConstString("&&") && !conditionIsTrue(tok2->astOperand1(), programMemory)) ||
                (tok2->str() == MatchCompiler::makeConstString("||") && !conditionIsFalse(tok2->astOperand1(), programMemory))) {
                // Skip second expression..
                const Token *parent = tok2;
                while (parent && parent->str() == tok2->str())
                    parent = parent->astParent();
                // Jump to end of condition
                if (parent && parent->str() == MatchCompiler::makeConstString("(")) {
                    tok2 = parent->link();
                    // cast
                    if (match113(tok2))
                        tok2 = tok2->linkAt(1);
                }
            }

        }
        if ((tok2->str() == MatchCompiler::makeConstString("&&") && conditionIsFalse(tok2->astOperand1(), getProgramMemory(tok2->astTop(), varid, ValueFlow::Value(value)))) ||
            (tok2->str() == MatchCompiler::makeConstString("||") && conditionIsTrue(tok2->astOperand1(), getProgramMemory(tok2->astTop(), varid, ValueFlow::Value(value)))))
            break;

        else if (match4(tok2) && findmatch45(tok2->link(), tok2, varid) ) {
            if (findmatch114(tok2) ) {
                if (settings->debugwarnings)
                    bailout(tokenlist, errorLogger, tok2, "For loop variable bailout on conditional continue|break|return");
                break;
            }
            if (settings->debugwarnings)
                bailout(tokenlist, errorLogger, tok2, "For loop variable skipping conditional scope");
            tok2 = tok2->next()->link();
            if (match62(tok2)) {
                if (findmatch114(tok2) ) {
                    if (settings->debugwarnings)
                        bailout(tokenlist, errorLogger, tok2, "For loop variable bailout on conditional continue|break|return");
                    break;
                }

                tok2 = tok2->linkAt(2);
            }
        }

        else if (match4(tok2)) {
            if (settings->debugwarnings)
                bailout(tokenlist, errorLogger, tok2, "For loop skipping {} code");
            tok2 = tok2->linkAt(1);
            if (match62(tok2))
                tok2 = tok2->linkAt(2);
        }
    }
}

static void valueFlowForLoopSimplifyAfter(Token *fortok, unsigned int varid, const MathLib::bigint num, TokenList *tokenlist, ErrorLogger *errorLogger, const Settings *settings)
{
    const Token *vartok = nullptr;
    for (const Token *tok = fortok; tok; tok = tok->next()) {
        if (tok->varId() == varid) {
            vartok = tok;
            break;
        }
    }
    if (!vartok || !vartok->variable())
        return;

    const Variable *var = vartok->variable();
    const Token *endToken = nullptr;
    if (var->isLocal())
        endToken = var->typeStartToken()->scope()->bodyEnd;
    else
        endToken = fortok->scope()->bodyEnd;

    std::list<ValueFlow::Value> values;
    values.emplace_back(num);
    values.back().errorPath.emplace_back(fortok,"After for loop, " + var->name() + " has value " + values.back().infoString());

    valueFlowForward(fortok->linkAt(1)->linkAt(1)->next(),
                     endToken,
                     var,
                     varid,
                     values,
                     false,
                     false,
                     tokenlist,
                     errorLogger,
                     settings);
}

static void valueFlowForLoop(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    for (const Scope &scope : symboldatabase->scopeList) {
        if (scope.type != Scope::eFor)
            continue;

        Token* tok = const_cast<Token*>(scope.classDef);
        Token* const bodyStart = const_cast<Token*>(scope.bodyStart);

        if (!match70(tok->next()->astOperand2()) ||
            !match70(tok->next()->astOperand2()->astOperand2()))
            continue;

        unsigned int varid(0);
        MathLib::bigint num1(0), num2(0), numAfter(0);

        if (valueFlowForLoop1(tok, &varid, &num1, &num2, &numAfter)) {
            if (num1 <= num2) {
                valueFlowForLoopSimplify(bodyStart, varid, false, num1, tokenlist, errorLogger, settings);
                valueFlowForLoopSimplify(bodyStart, varid, false, num2, tokenlist, errorLogger, settings);
                valueFlowForLoopSimplifyAfter(tok, varid, numAfter, tokenlist, errorLogger, settings);
            } else
                valueFlowForLoopSimplifyAfter(tok, varid, num1, tokenlist, errorLogger, settings);
        } else {
            ProgramMemory mem1, mem2, memAfter;
            if (valueFlowForLoop2(tok, &mem1, &mem2, &memAfter)) {
                std::map<unsigned int, ValueFlow::Value>::const_iterator it;
                for (it = mem1.values.begin(); it != mem1.values.end(); ++it) {
                    if (!it->second.isIntValue())
                        continue;
                    valueFlowForLoopSimplify(bodyStart, it->first, false, it->second.intvalue, tokenlist, errorLogger, settings);
                }
                for (it = mem2.values.begin(); it != mem2.values.end(); ++it) {
                    if (!it->second.isIntValue())
                        continue;
                    valueFlowForLoopSimplify(bodyStart, it->first, false, it->second.intvalue, tokenlist, errorLogger, settings);
                }
                for (it = memAfter.values.begin(); it != memAfter.values.end(); ++it) {
                    if (!it->second.isIntValue())
                        continue;
                    valueFlowForLoopSimplifyAfter(tok, it->first, it->second.intvalue, tokenlist, errorLogger, settings);
                }
            }
        }
    }
}

static void valueFlowInjectParameter(TokenList* tokenlist, ErrorLogger* errorLogger, const Settings* settings, const Variable* arg, const Scope* functionScope, const std::list<ValueFlow::Value>& argvalues)
{
    // Is argument passed by value or const reference, and is it a known non-class type?
    if (arg->isReference() && !arg->isConst() && !arg->isClass())
        return;

    // Set value in function scope..
    const unsigned int varid2 = arg->declarationId();
    if (!varid2)
        return;

    valueFlowForward(const_cast<Token*>(functionScope->bodyStart->next()), functionScope->bodyEnd, arg, varid2, argvalues, false, true, tokenlist, errorLogger, settings);
}

static void valueFlowSwitchVariable(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    for (const Scope &scope : symboldatabase->scopeList) {
        if (scope.type != Scope::ScopeType::eSwitch)
            continue;
        if (!match115(scope.classDef))
            continue;
        const Token *vartok = scope.classDef->tokAt(2);
        const Variable *var = vartok->variable();
        if (!var)
            continue;

        // bailout: global non-const variables
        if (!(var->isLocal() || var->isArgument()) && !var->isConst()) {
            if (settings->debugwarnings)
                bailout(tokenlist, errorLogger, vartok, "switch variable " + var->name() + " is global");
            continue;
        }

        for (Token *tok = scope.bodyStart->next(); tok != scope.bodyEnd; tok = tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("{")) {
                tok = tok->link();
                continue;
            }
            if (match116(tok)) {
                std::list<ValueFlow::Value> values;
                values.emplace_back(MathLib::toLongNumber(tok->next()->str()));
                values.back().condition = tok;
                const std::string info("case " + tok->next()->str() + ": " + vartok->str() + " is " + tok->next()->str() + " here.");
                values.back().errorPath.emplace_back(tok, info);
                bool known = false;
                if ((match59(tok->previous()) || match118(tok->tokAt(-2))) && !match117(tok->tokAt(3)))
                    known = true;
                while (match119(tok->tokAt(3))) {
                    known = false;
                    tok = tok->tokAt(3);
                    if (!tok->isName())
                        tok = tok->next();
                    values.emplace_back(MathLib::toLongNumber(tok->next()->str()));
                    values.back().condition = tok;
                    const std::string info2("case " + tok->next()->str() + ": " + vartok->str() + " is " + tok->next()->str() + " here.");
                    values.back().errorPath.emplace_back(tok, info2);
                }
                for (std::list<ValueFlow::Value>::const_iterator val = values.begin(); val != values.end(); ++val) {
                    valueFlowReverse(tokenlist,
                                     const_cast<Token*>(scope.classDef),
                                     vartok,
                                     *val,
                                     ValueFlow::Value(),
                                     errorLogger,
                                     settings);
                }
                if (vartok->variable()->scope()) {
                    if (known)
                        values.back().setKnown();
                    valueFlowForward(tok->tokAt(3), vartok->variable()->scope()->bodyEnd, vartok->variable(), vartok->varId(), values, values.back().isKnown(), false, tokenlist, errorLogger, settings);
                }
            }
        }
    }
}

static void setTokenValues(Token *tok, const std::list<ValueFlow::Value> &values, const Settings *settings)
{
    for (std::list<ValueFlow::Value>::const_iterator it = values.begin(); it != values.end(); ++it) {
        const ValueFlow::Value &value = *it;
        if (value.isIntValue())
            setTokenValue(tok, value, settings);
    }
}

static void valueFlowLibraryFunction(Token *tok, const std::string &returnValue, const Settings *settings)
{
    std::istringstream istr(returnValue);
    TokenList tokenList(settings);
    if (!tokenList.createTokens(istr))
        return;

    const Token *arg1 = tok->astOperand2();
    while (arg1 && arg1->str() == MatchCompiler::makeConstString(","))
        arg1 = arg1->astOperand1();
    if (findmatch120(tokenList.front())  && !arg1)
        return;

    if (match121(tokenList.front()) && arg1) {
        for (const ValueFlow::Value &value : arg1->values()) {
            if (value.isTokValue() && value.tokvalue->tokType() == Token::eString) {
                ValueFlow::Value retval(value); // copy all "inconclusive", "condition", etc attributes
                // set return value..
                retval.valueType = ValueFlow::Value::INT;
                retval.tokvalue = nullptr;
                retval.intvalue = Token::getStrLength(value.tokvalue);
                setTokenValue(tok, retval, settings);
            }
        }
        return;
    }

    // combine operators, set links, etc..
    for (Token *tok2 = tokenList.front(); tok2; tok2 = tok2->next()) {
        if (match122(tok2)) {
            tok2->str(tok2->str() + "=");
            tok2->deleteNext();
        }
    }

    // Evaluate expression
    tokenList.createAst();
    valueFlowNumber(&tokenList);
    for (Token *tok2 = tokenList.front(); tok2; tok2 = tok2->next()) {
        if (tok2->str() == MatchCompiler::makeConstString("arg1") && arg1) {
            setTokenValues(tok2, arg1->values(), settings);
        }
    }

    // Find result..
    for (const Token *tok2 = tokenList.front(); tok2; tok2 = tok2->next()) {
        if (!tok2->astParent() && !tok2->values().empty()) {
            setTokenValues(tok, tok2->values(), settings);
            return;
        }
    }
}

static void valueFlowSubFunction(TokenList *tokenlist, ErrorLogger *errorLogger, const Settings *settings)
{
    for (Token *tok = tokenlist->front(); tok; tok = tok->next()) {
        if (!match52(tok))
            continue;

        const Function * const calledFunction = tok->function();
        if (!calledFunction) {
            // library function?
            const std::string& returnValue(settings->library.returnValue(tok));
            if (!returnValue.empty())
                valueFlowLibraryFunction(tok->next(), returnValue, settings);
            continue;
        }

        const Scope * const calledFunctionScope = calledFunction->functionScope;
        if (!calledFunctionScope)
            continue;

        const std::vector<const Token *> &callArguments = getArguments(tok);
        for (unsigned int argnr = 0U; argnr < callArguments.size(); ++argnr) {
            const Token *argtok = callArguments[argnr];
            // Get function argument
            const Variable * const argvar = calledFunction->getArgumentVar(argnr);
            if (!argvar)
                break;

            // passing value(s) to function
            std::list<ValueFlow::Value> argvalues;
            if (match123(argtok) && !argtok->hasKnownIntValue()) {
                argvalues.emplace_back(0);
                argvalues.emplace_back(1);
            } else {
                argvalues = argtok->values();
            }

            if (argvalues.empty())
                continue;

            // Error path..
            for (std::list<ValueFlow::Value>::iterator it = argvalues.begin(); it != argvalues.end(); ++it) {
                const std::string nr = MathLib::toString(argnr + 1) + getOrdinalText(argnr + 1);

                it->errorPath.emplace_back(argtok,
                                           "Calling function '" +
                                           calledFunction->name() +
                                           "', " +
                                           nr +
                                           " argument '" +
                                           argvar->name() +
                                           "' value is " +
                                           it->infoString());
            }

            // passed values are not "known"..
            changeKnownToPossible(argvalues);

            valueFlowInjectParameter(tokenlist, errorLogger, settings, argvar, calledFunctionScope, argvalues);
        }
    }
}

static void valueFlowFunctionDefaultParameter(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    if (!tokenlist->isCPP())
        return;

    for (const Scope* scope : symboldatabase->functionScopes) {
        const Function* function = scope->function;
        if (!function)
            continue;
        for (std::size_t arg = function->minArgCount(); arg < function->argCount(); arg++) {
            const Variable* var = function->getArgumentVar(arg);
            if (var && var->hasDefault() && match124(var->nameToken())) {
                const std::list<ValueFlow::Value> &values = var->nameToken()->tokAt(2)->values();
                std::list<ValueFlow::Value> argvalues;
                for (const ValueFlow::Value &value : values) {
                    ValueFlow::Value v(value);
                    v.defaultArg = true;
                    v.changeKnownToPossible();
                    if (v.isPossible())
                        argvalues.push_back(v);
                }
                if (!argvalues.empty())
                    valueFlowInjectParameter(tokenlist, errorLogger, settings, var, scope, argvalues);
            }
        }
    }
}

static bool isKnown(const Token * tok)
{
    return tok && tok->hasKnownIntValue();
}

static void valueFlowFunctionReturn(TokenList *tokenlist, ErrorLogger *errorLogger)
{
    for (Token *tok = tokenlist->back(); tok; tok = tok->previous()) {
        if (tok->str() != MatchCompiler::makeConstString("(") || !tok->astOperand1() || !tok->astOperand1()->function())
            continue;

        // Arguments..
        std::vector<MathLib::bigint> parvalues;
        if (tok->astOperand2()) {
            const Token *partok = tok->astOperand2();
            while (partok && partok->str() == MatchCompiler::makeConstString(",") && isKnown(partok->astOperand2()))
                partok = partok->astOperand1();
            if (!isKnown(partok))
                continue;
            parvalues.push_back(partok->values().front().intvalue);
            partok = partok->astParent();
            while (partok && partok->str() == MatchCompiler::makeConstString(",")) {
                parvalues.push_back(partok->astOperand2()->values().front().intvalue);
                partok = partok->astParent();
            }
            if (partok != tok)
                continue;
        }

        // Get scope and args of function
        const Function * const function = tok->astOperand1()->function();
        const Scope * const functionScope = function->functionScope;
        if (!functionScope || !match125(functionScope->bodyStart)) {
            if (functionScope && tokenlist->getSettings()->debugwarnings && findmatch126(functionScope->bodyStart, functionScope->bodyEnd) )
                bailout(tokenlist, errorLogger, tok, "function return; nontrivial function body");
            continue;
        }

        ProgramMemory programMemory;
        for (std::size_t i = 0; i < parvalues.size(); ++i) {
            const Variable * const arg = function->getArgumentVar(i);
            if (!arg || !match127(arg->typeStartToken())) {
                if (tokenlist->getSettings()->debugwarnings)
                    bailout(tokenlist, errorLogger, tok, "function return; unhandled argument type");
                programMemory.clear();
                break;
            }
            programMemory.setIntValue(arg->declarationId(), parvalues[i]);
        }
        if (programMemory.empty() && !parvalues.empty())
            continue;

        // Determine return value of subfunction..
        MathLib::bigint result = 0;
        bool error = false;
        execute(functionScope->bodyStart->next()->astOperand1(),
                &programMemory,
                &result,
                &error);
        if (!error) {
            ValueFlow::Value v(result);
            if (function->isVirtual())
                v.setPossible();
            else
                v.setKnown();
            setTokenValue(tok, v, tokenlist->getSettings());
        }
    }
}

static void valueFlowUninit(TokenList *tokenlist, SymbolDatabase * /*symbolDatabase*/, ErrorLogger *errorLogger, const Settings *settings)
{
    for (Token *tok = tokenlist->front(); tok; tok = tok->next()) {
        if (!match128(tok))
            continue;
        if (!tok->scope()->isExecutable())
            continue;
        const Token *vardecl = tok->next();
        bool stdtype = false;
        bool pointer = false;
        while (match129(vardecl) && vardecl->varId() == 0) {
            stdtype |= vardecl->isStandardType();
            pointer |= vardecl->str() == MatchCompiler::makeConstString("*");
            vardecl = vardecl->next();
        }
        if (!stdtype && !pointer)
            continue;
        if (!match130(vardecl))
            continue;
        if (match131(vardecl, vardecl->varId()))
            continue;
        const Variable *var = vardecl->variable();
        if (!var || var->nameToken() != vardecl)
            continue;
        if ((!var->isPointer() && var->type() && var->type()->needInitialization != Type::True) ||
            !var->isLocal() || var->isStatic() || var->isExtern() || var->isReference() || var->isThrow())
            continue;

        ValueFlow::Value uninitValue;
        uninitValue.setKnown();
        uninitValue.valueType = ValueFlow::Value::UNINIT;
        std::list<ValueFlow::Value> values;
        values.push_back(uninitValue);

        const bool constValue = true;
        const bool subFunction = false;

        valueFlowForward(vardecl->next(), vardecl->scope()->bodyEnd, var, vardecl->varId(), values, constValue, subFunction, tokenlist, errorLogger, settings);
    }
}

static bool hasContainerSizeGuard(const Token *tok, unsigned int containerId)
{
    for (; tok && tok->astParent(); tok = tok->astParent()) {
        const Token *parent = tok->astParent();
        if (tok != parent->astOperand2())
            continue;
        if (!match132(parent))
            continue;
        // is container found in lhs?
        std::stack<const Token *> tokens;
        tokens.push(parent->astOperand1());
        while (!tokens.empty()) {
            const Token *t = tokens.top();
            tokens.pop();
            if (!t)
                continue;
            if (t->varId() == containerId)
                return true;
            tokens.push(t->astOperand1());
            tokens.push(t->astOperand2());
        }
    }
    return false;
}

static bool isContainerSizeChangedByFunction(const Token *tok)
{
    const Token *parent = tok->astParent();
    if (parent && parent->str() == MatchCompiler::makeConstString("&"))
        parent = parent->astParent();
    while (parent && parent->str() == MatchCompiler::makeConstString(","))
        parent = parent->astParent();
    if (!parent)
        return false;
    if (match52(parent->previous()))
        return true;
    // some unsimplified template function, assume it modifies the container.
    if (match92(parent->previous()) && parent->linkAt(-1))
        return true;
    return false;
}

static void valueFlowContainerReverse(const Token *tok, unsigned int containerId, const ValueFlow::Value &value, const Settings *settings)
{
    while (nullptr != (tok = tok->previous())) {
        if (match133(tok))
            break;
        if (match134(tok))
            break;
        if (tok->varId() != containerId)
            continue;
        if (match26(tok))
            break;
        if (isContainerSizeChangedByFunction(tok))
            break;
        if (!tok->valueType() || !tok->valueType()->container)
            break;
        if (match135(tok) && tok->valueType()->container->getAction(tok->strAt(2)) != Library::Container::Action::NO_ACTION)
            break;
        if (!hasContainerSizeGuard(tok, containerId))
            setTokenValue(const_cast<Token *>(tok), value, settings);
    }
}

static void valueFlowContainerForward(const Token *tok, unsigned int containerId, const ValueFlow::Value &value, const Settings *settings, bool cpp)
{
    while (nullptr != (tok = tok->next())) {
        if (match133(tok))
            break;
        if (tok->varId() != containerId)
            continue;
        if (match26(tok))
            break;
        if (isLikelyStreamRead(cpp, tok->astParent()))
            break;
        if (isContainerSizeChangedByFunction(tok))
            break;
        if (!tok->valueType() || !tok->valueType()->container)
            break;
        if (match135(tok) && tok->valueType()->container->getAction(tok->strAt(2)) != Library::Container::Action::NO_ACTION)
            break;
        if (!hasContainerSizeGuard(tok, containerId))
            setTokenValue(const_cast<Token *>(tok), value, settings);
    }
}

static bool isContainerSizeChanged(unsigned int varId, const Token *start, const Token *end)
{
    for (const Token *tok = start; tok != end; tok = tok->next()) {
        if (tok->varId() != varId)
            continue;
        if (!tok->valueType() || !tok->valueType()->container)
            return true;
        if (match26(tok))
            return true;
        if (match135(tok)) {
            Library::Container::Action action = tok->valueType()->container->getAction(tok->strAt(2));
            switch (action) {
            case Library::Container::Action::RESIZE:
            case Library::Container::Action::CLEAR:
            case Library::Container::Action::PUSH:
            case Library::Container::Action::POP:
            case Library::Container::Action::CHANGE:
            case Library::Container::Action::INSERT:
            case Library::Container::Action::ERASE:
            case Library::Container::Action::CHANGE_INTERNAL:
                return true;
            case Library::Container::Action::NO_ACTION: // might be unknown action
                return true;
            case Library::Container::Action::FIND:
            case Library::Container::Action::CHANGE_CONTENT:
                break;
            };
        }
    }
    return false;
}

static void valueFlowContainerSize(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger * /*errorLogger*/, const Settings *settings)
{
    // declaration
    for (const Variable *var : symboldatabase->variableList()) {
        if (!var || !var->isLocal() || var->isPointer() || var->isReference())
            continue;
        if (!var->valueType() || !var->valueType()->container)
            continue;
        if (!match136(var->nameToken()))
            continue;
        ValueFlow::Value value(0);
        if (var->valueType()->container->size_templateArgNo >= 0) {
            if (var->dimensions().size() == 1 && var->dimensions().front().known)
                value.intvalue = var->dimensions().front().num;
            else
                continue;
        }
        value.valueType = ValueFlow::Value::ValueType::CONTAINER_SIZE;
        value.setKnown();
        valueFlowContainerForward(var->nameToken()->next(), var->declarationId(), value, settings, tokenlist->isCPP());
    }

    // after assignment
    for (const Scope *functionScope : symboldatabase->functionScopes) {
        for (const Token *tok = functionScope->bodyStart; tok != functionScope->bodyEnd; tok = tok->next()) {
            if (match137(tok)) {
                const Token *containerTok = tok->next();
                if (containerTok && containerTok->valueType() && containerTok->valueType()->container && containerTok->valueType()->container->stdStringLike) {
                    ValueFlow::Value value(Token::getStrLength(containerTok->tokAt(2)));
                    value.valueType = ValueFlow::Value::ValueType::CONTAINER_SIZE;
                    value.setKnown();
                    valueFlowContainerForward(containerTok->next(), containerTok->varId(), value, settings, tokenlist->isCPP());
                }
            }
        }
    }

    // conditional conditionSize
    for (const Scope &scope : symboldatabase->scopeList) {
        if (scope.type != Scope::ScopeType::eIf) // TODO: while
            continue;
        for (const Token *tok = scope.classDef; tok && tok->str() != MatchCompiler::makeConstString("{"); tok = tok->next()) {
            if (!tok->isName() || !tok->valueType() || tok->valueType()->type != ValueType::CONTAINER || !tok->valueType()->container)
                continue;
            if (!match135(tok))
                continue;

            const Token *conditionToken;
            MathLib::bigint intval;

            if (tok->valueType()->container->getYield(tok->strAt(2)) == Library::Container::Yield::SIZE) {
                const Token *parent = tok->tokAt(3)->astParent();
                if (!parent || !parent->isComparisonOp() || !parent->astOperand2())
                    continue;
                if (parent->astOperand1()->hasKnownIntValue())
                    intval = parent->astOperand1()->values().front().intvalue;
                else if (parent->astOperand2()->hasKnownIntValue())
                    intval = parent->astOperand2()->values().front().intvalue;
                else
                    continue;
                conditionToken = parent;
            } else if (tok->valueType()->container->getYield(tok->strAt(2)) == Library::Container::Yield::EMPTY) {
                conditionToken = tok->tokAt(3);
                intval = 0;
            } else {
                continue;
            }

            ValueFlow::Value value(conditionToken, intval);
            value.valueType = ValueFlow::Value::ValueType::CONTAINER_SIZE;

            // possible value before condition
            valueFlowContainerReverse(scope.classDef, tok->varId(), value, settings);

            // possible value after condition
            if (!isEscapeScope(scope.bodyStart, tokenlist)) {
                const Token *after = scope.bodyEnd;
                if (match62(after))
                    after = isEscapeScope(after->tokAt(2), tokenlist) ? nullptr : after->linkAt(2);
                if (after && !isContainerSizeChanged(tok->varId(), scope.bodyStart, after))
                    valueFlowContainerForward(after, tok->varId(), value, settings, tokenlist->isCPP());
            }

            // known value in conditional code
            if (conditionToken->str() == MatchCompiler::makeConstString("==") || conditionToken->str() == MatchCompiler::makeConstString("(")) {
                const Token *parent = conditionToken->astParent();
                while (parent && !match138(parent))
                    parent = parent->astParent();
                if (!parent) {
                    value.setKnown();
                    valueFlowContainerForward(scope.bodyStart, tok->varId(), value, settings, tokenlist->isCPP());
                }
            }
        }
    }
}

ValueFlow::Value::Value(const Token *c, long long val)
    : valueType(INT),
      intvalue(val),
      tokvalue(nullptr),
      floatValue(0.0),
      moveKind(NonMovedVariable),
      varvalue(val),
      condition(c),
      varId(0U),
      conditional(false),
      defaultArg(false),
      valueKind(ValueKind::Possible)
{
    errorPath.emplace_back(c, "Assuming that condition '" + c->expressionString() + "' is not redundant");
}

std::string ValueFlow::Value::infoString() const
{
    switch (valueType) {
    case INT:
        return MathLib::toString(intvalue);
    case TOK:
        return tokvalue->str();
    case FLOAT:
        return MathLib::toString(floatValue);
    case MOVED:
        return "<Moved>";
    case UNINIT:
        return "<Uninit>";
    case CONTAINER_SIZE:
        return "size=" + MathLib::toString(intvalue);
    };
    throw InternalError(nullptr, "Invalid ValueFlow Value type");
}

const ValueFlow::Value *ValueFlow::valueFlowConstantFoldAST(const Token *expr, const Settings *settings)
{
    if (expr && expr->values().empty()) {
        valueFlowConstantFoldAST(expr->astOperand1(), settings);
        valueFlowConstantFoldAST(expr->astOperand2(), settings);
        valueFlowSetConstantValue(expr, settings, true /* TODO: this is a guess */);
    }
    return expr && expr->hasKnownValue() ? &expr->values().front() : nullptr;
}


void ValueFlow::setValues(TokenList *tokenlist, SymbolDatabase* symboldatabase, ErrorLogger *errorLogger, const Settings *settings)
{
    for (Token *tok = tokenlist->front(); tok; tok = tok->next())
        tok->clearValueFlow();

    valueFlowNumber(tokenlist);
    valueFlowString(tokenlist);
    valueFlowArray(tokenlist);
    valueFlowGlobalStaticVar(tokenlist, settings);
    valueFlowPointerAlias(tokenlist);
    valueFlowFunctionReturn(tokenlist, errorLogger);
    valueFlowBitAnd(tokenlist);
    valueFlowOppositeCondition(symboldatabase, settings);
    valueFlowBeforeCondition(tokenlist, symboldatabase, errorLogger, settings);
    valueFlowAfterMove(tokenlist, symboldatabase, errorLogger, settings);
    valueFlowAfterAssign(tokenlist, symboldatabase, errorLogger, settings);
    valueFlowAfterCondition(tokenlist, symboldatabase, errorLogger, settings);
    valueFlowSwitchVariable(tokenlist, symboldatabase, errorLogger, settings);
    valueFlowForLoop(tokenlist, symboldatabase, errorLogger, settings);
    valueFlowSubFunction(tokenlist, errorLogger, settings);
    valueFlowFunctionDefaultParameter(tokenlist, symboldatabase, errorLogger, settings);
    valueFlowUninit(tokenlist, symboldatabase, errorLogger, settings);
    if (tokenlist->isCPP())
        valueFlowContainerSize(tokenlist, symboldatabase, errorLogger, settings);
}


std::string ValueFlow::eitherTheConditionIsRedundant(const Token *condition)
{
    if (!condition)
        return "Either the condition is redundant";
    if (condition->str() == MatchCompiler::makeConstString("case")) {
        std::string expr;
        for (const Token *tok = condition; tok && tok->str() != MatchCompiler::makeConstString(":"); tok = tok->next()) {
            expr += tok->str();
            if (match139(tok))
                expr += ' ';
        }
        return "Either the switch case '" + expr + "' is redundant";
    }
    return "Either the condition '" + condition->expressionString() + "' is redundant";
}
