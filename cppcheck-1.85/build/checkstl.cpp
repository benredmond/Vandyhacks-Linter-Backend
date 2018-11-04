#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %name% . %name% (
static bool match1(const Token* tok) {
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
// pattern: %name% [
static bool match2(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: %name% .
static bool match3(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: iterator|const_iterator|reverse_iterator|const_reverse_iterator|auto
static bool match4(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("iterator")) || (tok->str()==MatchCompiler::makeConstString("const_iterator")) || (tok->str()==MatchCompiler::makeConstString("reverse_iterator")) || (tok->str()==MatchCompiler::makeConstString("const_reverse_iterator")) || (tok->str()==MatchCompiler::makeConstString("auto"))))
        return false;
    return true;
}
// pattern: [(=:{]
static bool match5(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(=:{", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% . end|rend|cend|crend ( )
static bool match6(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("crend"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %name% . insert|erase ( *| %varid% )|,
static bool match7(const Token* tok, const unsigned int varid) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("insert")) || (tok->str()==MatchCompiler::makeConstString("erase"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: std :: set
static bool match8(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("set")))
        return false;
    return true;
}
// pattern: %varid% = %name% .
static bool match9(const Token* tok, const unsigned int varid) {
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: erase (
static bool match10(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("erase")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: begin|rbegin|cbegin|crbegin|find (
static bool match11(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("begin")) || (tok->str()==MatchCompiler::makeConstString("rbegin")) || (tok->str()==MatchCompiler::makeConstString("cbegin")) || (tok->str()==MatchCompiler::makeConstString("crbegin")) || (tok->str()==MatchCompiler::makeConstString("find"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) .
static bool match12(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
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
// pattern: %varid% ,|)
static bool match14(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: * %varid%
static bool match15(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: %varid% . %name%
static bool match16(const Token* tok, const unsigned int varid) {
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
    return true;
}
// pattern: return|break|continue
static bool match17(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    return true;
}
// pattern: ;
template<class T> static T * findmatch18(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %var% . begin|end|rbegin|rend ( )
static bool match19(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("begin")) || (tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rbegin")) || (tok->str()==MatchCompiler::makeConstString("rend"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: std ::
static bool match20(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: begin|cbegin|rbegin|crbegin|end|cend|rend|crend (
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("begin")) || (tok->str()==MatchCompiler::makeConstString("cbegin")) || (tok->str()==MatchCompiler::makeConstString("rbegin")) || (tok->str()==MatchCompiler::makeConstString("crbegin")) || (tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("crend"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: . %name% ( ) !!.
static bool match22(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("."))
        return false;
    return true;
}
// pattern: %name% ( !!)
static bool match23(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(")"))
        return false;
    return true;
}
// pattern: .
static bool match24(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: %comp%|-
static bool match25(const Token* tok) {
    if (!tok || !(tok->isComparisonOp() || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: ==|!=
static bool match26(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    return true;
}
// pattern: %var% (
static bool match27(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% <= %var% . %name% ( ) ;|)|%oror%
static bool match28(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<=")))
        return false;
    tok = tok->next();
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
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    return true;
}
// pattern: . %name% ( )
static bool match29(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [ %varid% ]
static bool match30(const Token* tok, const unsigned int varid) {
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
// pattern: . %name% ( %varid% )
static bool match31(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %var% [
static bool match32(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: for (
static bool match33(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ; ++| %var% ++| ) {
static bool match34(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: while ( %var% !=
static bool match35(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!=")))
        return false;
    return true;
}
// pattern: . erase ( ++| %varid% )
static bool match36(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("erase")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
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
// pattern: =
static bool match37(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: } else {
static bool match38(const Token* tok) {
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
// pattern: break|return|goto
static bool match39(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("goto"))))
        return false;
    return true;
}
// pattern: %var% = & %var% [
static bool match40(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: %varid% . push_front|push_back|insert|reserve|resize|clear
static bool match41(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("push_front")) || (tok->str()==MatchCompiler::makeConstString("push_back")) || (tok->str()==MatchCompiler::makeConstString("insert")) || (tok->str()==MatchCompiler::makeConstString("reserve")) || (tok->str()==MatchCompiler::makeConstString("resize")) || (tok->str()==MatchCompiler::makeConstString("clear"))))
        return false;
    return true;
}
// pattern: iterator|const_iterator|reverse_iterator|const_reverse_iterator
static bool match42(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("iterator")) || (tok->str()==MatchCompiler::makeConstString("const_iterator")) || (tok->str()==MatchCompiler::makeConstString("reverse_iterator")) || (tok->str()==MatchCompiler::makeConstString("const_reverse_iterator"))))
        return false;
    return true;
}
// pattern: std| ::| vector <
static bool match43(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("std"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("vector")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: %varid% = %var% . begin|rbegin|cbegin|crbegin ( ) ; %varid% != %var% . end|rend|cend|crend ( ) ; ++| %varid% ++| ) {
static bool match44(const Token* tok, const unsigned int varid) {
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("begin")) || (tok->str()==MatchCompiler::makeConstString("rbegin")) || (tok->str()==MatchCompiler::makeConstString("cbegin")) || (tok->str()==MatchCompiler::makeConstString("crbegin"))))
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
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("crend"))))
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
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: %varid% . push_front|push_back|insert|reserve|resize|clear|erase (
static bool match45(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("push_front")) || (tok->str()==MatchCompiler::makeConstString("push_back")) || (tok->str()==MatchCompiler::makeConstString("insert")) || (tok->str()==MatchCompiler::makeConstString("reserve")) || (tok->str()==MatchCompiler::makeConstString("resize")) || (tok->str()==MatchCompiler::makeConstString("clear")) || (tok->str()==MatchCompiler::makeConstString("erase"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% . begin|end|rbegin|rend|cbegin|cend|crbegin|crend|insert|erase|find (
static bool match46(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("begin")) || (tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rbegin")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("cbegin")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("crbegin")) || (tok->str()==MatchCompiler::makeConstString("crend")) || (tok->str()==MatchCompiler::makeConstString("insert")) || (tok->str()==MatchCompiler::makeConstString("erase")) || (tok->str()==MatchCompiler::makeConstString("find"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: insert|erase ( *| %varid% )|,
static bool match47(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("insert")) || (tok->str()==MatchCompiler::makeConstString("erase"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: insert|erase ( *| %varid% ,|)
static bool match48(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("insert")) || (tok->str()==MatchCompiler::makeConstString("erase"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
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
// pattern: ++|--|*|+|-|(|,|=|!= %varid%
static bool match49(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: %varid% ++|--|+|-|.
static bool match50(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: !!* %varid% <
static bool match51(const Token* tok, const unsigned int varid) {
    if (tok && tok->str() == MatchCompiler::makeConstString("*"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > %varid% !!.
static bool match52(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("."))
        return false;
    return true;
}
// pattern: %name% (
static bool match53(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% . %name% (
static bool match54(const Token* tok) {
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
// pattern: . %name% (
static bool match55(const Token* tok) {
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
// pattern: <
template<class T> static T * findmatch56(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: std :: find|find_if (
static bool match57(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("find")) || (tok->str()==MatchCompiler::makeConstString("find_if"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% . size ( )
static bool match58(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("size")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %name% . %var% . size ( )
static bool match59(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("size")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ==|<=|!=|> 0
static bool match60(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    return true;
}
// pattern: 0 ==|>=|!=|<
static bool match61(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: >=|< 1
static bool match62(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    return true;
}
// pattern: 1 <=|>
static bool match63(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: if|while (
static bool match64(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: &&|)|,|;|%oror%
static bool match65(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    return true;
}
// pattern: %name% . find ( %any% ) != %name% . end|rend|cend|crend ( ) ) { %name% . remove|erase ( %any% ) ;
static bool match66(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("find")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("crend"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("remove")) || (tok->str()==MatchCompiler::makeConstString("erase"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %var% = %name% . begin|rbegin|cbegin|crbegin ( ) ; %name% != %name% . end|rend|cend|crend ( ) ; ++| %name% ++| ) {
static bool match67(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("begin")) || (tok->str()==MatchCompiler::makeConstString("rbegin")) || (tok->str()==MatchCompiler::makeConstString("cbegin")) || (tok->str()==MatchCompiler::makeConstString("crbegin"))))
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
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("crend"))))
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
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ++ %varid% )
static bool match68(const Token* tok, const unsigned int varid) {
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
    return true;
}
// pattern: %varid% ++ )
static bool match69(const Token* tok, const unsigned int varid) {
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
    return true;
}
// pattern: %varid% ++
static bool match70(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("++")))
        return false;
    return true;
}
// pattern: ++ %varid% !!.
static bool match71(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("++")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("."))
        return false;
    return true;
}
// pattern: %varid% !=|==
static bool match72(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString("=="))))
        return false;
    return true;
}
// pattern: %varid% = %name% . insert ( ++| %varid% ++| ,
static bool match73(const Token* tok, const unsigned int varid) {
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("insert")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: char|wchar_t *
static bool match74(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("wchar_t"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: const std :: string|wstring &
static bool match75(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("string")) || (tok->str()==MatchCompiler::makeConstString("wstring"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: std :: string|wstring !!&
static bool match76(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("string")) || (tok->str()==MatchCompiler::makeConstString("wstring"))))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("&"))
        return false;
    return true;
}
// pattern: throw %var% . c_str|data ( ) ;
static bool match77(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("throw")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("c_str")) || (tok->str()==MatchCompiler::makeConstString("data"))))
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
// pattern: [;{}] %name% = %var% . str ( ) . c_str|data ( ) ;
static bool match78(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("str")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("c_str")) || (tok->str()==MatchCompiler::makeConstString("data"))))
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
// pattern: [;{}] %var% = %name% (
static bool match79(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
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
// pattern: ) . c_str|data ( ) ;
static bool match80(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("c_str")) || (tok->str()==MatchCompiler::makeConstString("data"))))
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
// pattern: std :: string|wstring %name%
static bool match81(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("string")) || (tok->str()==MatchCompiler::makeConstString("wstring"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ::|.
static bool match82(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: . c_str|data ( )
static bool match83(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("c_str")) || (tok->str()==MatchCompiler::makeConstString("data"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %name% . str ( )
static bool match84(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("str")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: std :: string|wstring (
static bool match85(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("string")) || (tok->str()==MatchCompiler::makeConstString("wstring"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: (
static bool match86(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: + %var%
static bool match87(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: + std :: string|wstring (
static bool match88(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("string")) || (tok->str()==MatchCompiler::makeConstString("wstring"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% .
static bool match89(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: %var% = %var% .|;|[
static bool match90(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %var% .|::
static bool match91(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: c_str|data ( ) ;
static bool match92(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("c_str")) || (tok->str()==MatchCompiler::makeConstString("data"))))
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
// pattern: std :: string|wstring
static bool match93(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("string")) || (tok->str()==MatchCompiler::makeConstString("wstring"))))
        return false;
    return true;
}
// pattern: ] ;
static bool match94(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ] ) ;
static bool match95(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: auto_ptr <
static bool match96(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("auto_ptr")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > %name%
static bool match97(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ( new %type%
static bool match98(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: ( %name% (
static bool match99(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( %var%
static bool match100(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: [ ] )
static bool match101(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %varid% = new %type%
template<class T> static T * findmatch102(T * start_tok, unsigned int varid) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %name% = %var% ;
static bool match103(const Token* tok) {
    if (!tok || !tok->isName())
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
// pattern: %var% = new %type%
static bool match104(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %var% . reset ( new %type%
static bool match105(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("reset")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %var% = %name% (
static bool match106(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
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
// pattern: %var% . reset ( %name% (
static bool match107(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("reset")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% . compare|find|rfind|find_first_not_of|find_first_of|find_last_not_of|find_last_of ( %name% [,)]
static bool match108(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("compare")) || (tok->str()==MatchCompiler::makeConstString("find")) || (tok->str()==MatchCompiler::makeConstString("rfind")) || (tok->str()==MatchCompiler::makeConstString("find_first_not_of")) || (tok->str()==MatchCompiler::makeConstString("find_first_of")) || (tok->str()==MatchCompiler::makeConstString("find_last_not_of")) || (tok->str()==MatchCompiler::makeConstString("find_last_of"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %var% . swap ( %name% )
static bool match109(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("swap")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %var% . substr (
static bool match110(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("substr")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: 0| )
static bool match111(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("0"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: , 0 )
static bool match112(const Token* tok) {
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
// pattern: [{};] %var% . empty ( ) ;
static bool match113(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("empty")))
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
// pattern: [{};] std :: remove|remove_if|unique (
static bool match114(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("remove")) || (tok->str()==MatchCompiler::makeConstString("remove_if")) || (tok->str()==MatchCompiler::makeConstString("unique"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ;
template<class T> static T * findmatch115(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ||
template<class T> static T * findmatch116(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: &&
template<class T> static T * findmatch117(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: && %var% != %name% . end|rend|cend|crend ( )
template<class T> static T * findmatch118(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!=")))
        continue;
    tok = tok->next();
    if (!tok || !tok->isName())
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        continue;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("crend"))))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %oror% %var% == %name% . end|rend|cend|crend ( )
template<class T> static T * findmatch119(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        continue;
    tok = tok->next();
    if (!tok || !tok->isName())
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        continue;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("end")) || (tok->str()==MatchCompiler::makeConstString("rend")) || (tok->str()==MatchCompiler::makeConstString("cend")) || (tok->str()==MatchCompiler::makeConstString("crend"))))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: * %varid%
template<class T> static T * findmatch120(T * start_tok, const Token * end, unsigned int varid) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
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
// pattern: return|throw|break
template<class T> static T * findmatch121(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; }
template<class T> static T * findmatch122(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; }
static bool match123(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: %var% %assign%
static bool match124(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: %varid%
template<class T> static T * findmatch125(T * start_tok, const Token * end, unsigned int varid) {
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
// pattern: ) ; }
static bool match126(const Token* tok) {
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
// pattern: %var% . %name% ( %varid% )
static bool match127(const Token* tok, const unsigned int varid) {
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
// pattern: ++ %var% ; }
static bool match128(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("++")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: %var% ++ ; }
static bool match129(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("++")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: if (
static bool match130(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) {
static bool match131(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } }
static bool match132(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: += %any% ;
static bool match133(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+=")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: = %varid% + %any% ;
static bool match134(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: =|&= %bool% ;
static bool match135(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("&="))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isBoolean())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: = %varid% %oror%|%or%|&&|& %bool% ;
static bool match136(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") ) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isBoolean())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: &=
static bool match137(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&=")))
        return false;
    return true;
}
// pattern: = %varid% %oror%|%or%|&&|&
static bool match138(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") ) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: <|<=|>=|>
static bool match139(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: <|<=
static bool match140(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("<="))))
        return false;
    return true;
}
// pattern: :
static bool match141(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: %var%
static bool match142(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: %var%|%bool%|%num%|%char% ;
static bool match143(const Token* tok) {
    if (!tok || !((tok->varId() != 0) || tok->isBoolean() || tok->isNumber() || (tok->tokType()==Token::eChar)))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name% ( )
static bool match144(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: = %var% <|<=|>=|> %var% ? %var% : %var%
static bool match145(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("?")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
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

#include "checkstl.h"

#include "checknullpointer.h"
#include "errorlogger.h"
#include "settings.h"
#include "standards.h"
#include "symboldatabase.h"
#include "token.h"
#include "utils.h"
#include "astutils.h"

#include <cstddef>
#include <list>
#include <set>
#include <sstream>
#include <utility>

// Register this check class (by creating a static instance of it)
namespace {
    CheckStl instance;
}

// CWE IDs used:
static const struct CWE CWE398(398U);   // Indicator of Poor Code Quality
static const struct CWE CWE597(597U);   // Use of Wrong Operator in String Comparison
static const struct CWE CWE628(628U);   // Function Call with Incorrectly Specified Arguments
static const struct CWE CWE664(664U);   // Improper Control of a Resource Through its Lifetime
static const struct CWE CWE704(704U);   // Incorrect Type Conversion or Cast
static const struct CWE CWE762(762U);   // Mismatched Memory Management Routines
static const struct CWE CWE786(786U);   // Access of Memory Location Before Start of Buffer
static const struct CWE CWE788(788U);   // Access of Memory Location After End of Buffer
static const struct CWE CWE825(825U);   // Expired Pointer Dereference
static const struct CWE CWE834(834U);   // Excessive Iteration



void CheckStl::outOfBounds()
{
    for (const Scope *function : mTokenizer->getSymbolDatabase()->functionScopes) {
        for (const Token *tok = function->bodyStart; tok != function->bodyEnd; tok = tok->next()) {
            if (!tok->isName() || !tok->valueType())
                continue;
            const Library::Container *container = tok->valueType()->container;
            if (!container)
                continue;
            for (const ValueFlow::Value &value : tok->values()) {
                if (!value.isContainerSizeValue())
                    continue;
                if (value.isInconclusive() && !mSettings->inconclusive)
                    continue;
                if (!value.errorSeverity() && !mSettings->isEnabled(Settings::WARNING))
                    continue;
                if (value.intvalue == 0 && match1(tok) && container->getYield(tok->strAt(2)) == Library::Container::Yield::ITEM) {
                    outOfBoundsError(tok, &value, nullptr);
                    continue;
                }
                if (!container->arrayLike_indexOp && !container->stdStringLike)
                    continue;
                if (value.intvalue == 0 && match2(tok)) {
                    outOfBoundsError(tok, &value, nullptr);
                    continue;
                }
                if (container->arrayLike_indexOp && match2(tok)) {
                    const ValueFlow::Value *indexValue = tok->next()->astOperand2() ? tok->next()->astOperand2()->getMaxValue(false) : nullptr;
                    if (indexValue && indexValue->intvalue >= value.intvalue) {
                        outOfBoundsError(tok, &value, indexValue);
                        continue;
                    }
                    if (mSettings->isEnabled(Settings::WARNING)) {
                        indexValue = tok->next()->astOperand2() ? tok->next()->astOperand2()->getMaxValue(true) : nullptr;
                        if (indexValue && indexValue->intvalue >= value.intvalue) {
                            outOfBoundsError(tok, &value, indexValue);
                            continue;
                        }
                    }
                }
            }
        }
    }
}

void CheckStl::outOfBoundsError(const Token *tok, const ValueFlow::Value *containerSize, const ValueFlow::Value *index)
{
    // Do not warn if both the container size and index are possible
    if (containerSize && index && containerSize->isPossible() && index->isPossible())
        return;

    const std::string varname = tok ? tok->str() : std::string("var");

    std::string errmsg;
    if (!containerSize)
        errmsg = "Out of bounds access of item in container '$symbol'";
    else if (containerSize->intvalue == 0) {
        if (containerSize->condition)
            errmsg = "Accessing an item in container '$symbol'. " + ValueFlow::eitherTheConditionIsRedundant(containerSize->condition) + " or '$symbol' can be empty.";
        else
            errmsg = "Accessing an item in container '$symbol' that is empty.";
    } else if (index) {
        if (containerSize->condition || index->condition)
            errmsg = "Possible access out of bounds";
        else
            errmsg = "Access out of bounds";

        errmsg += " of container '$symbol'; size=" +
                  MathLib::toString(containerSize->intvalue) + ", index=" +
                  MathLib::toString(index->intvalue);
    } else {
        // should not happen
        return;
    }

    ErrorPath errorPath;
    if (!index)
        errorPath = getErrorPath(tok, containerSize, "Access out of bounds");
    else {
        ErrorPath errorPath1 = getErrorPath(tok, containerSize, "Access out of bounds");
        ErrorPath errorPath2 = getErrorPath(tok, index, "Access out of bounds");
        if (errorPath1.size() <= 1)
            errorPath = errorPath2;
        else if (errorPath2.size() <= 1)
            errorPath = errorPath1;
        else {
            errorPath = errorPath1;
            errorPath.splice(errorPath.end(), errorPath2);
        }
    }

    reportError(errorPath,
                (containerSize && !containerSize->errorSeverity()) || (index && !index->errorSeverity()) ? Severity::warning : Severity::error,
                "containerOutOfBounds",
                "$symbol:" + varname +"\n" + errmsg,
                CWE398,
                (containerSize && containerSize->isInconclusive()) || (index && index->isInconclusive()));
}


// Error message for bad iterator usage..
void CheckStl::invalidIteratorError(const Token *tok, const std::string &iteratorName)
{
    reportError(tok, Severity::error, "invalidIterator1", "$symbol:"+iteratorName+"\nInvalid iterator: $symbol", CWE664, false);
}

void CheckStl::iteratorsError(const Token *tok, const std::string &container1, const std::string &container2)
{
    reportError(tok, Severity::error, "iterators",
                "$symbol:" + container1 + "\n"
                "$symbol:" + container2 + "\n"
                "Same iterator is used with different containers '" + container1 + "' and '" + container2 + "'.", CWE664, false);
}

// Error message used when dereferencing an iterator that has been erased..
void CheckStl::dereferenceErasedError(const Token *erased, const Token* deref, const std::string &itername, bool inconclusive)
{
    if (erased) {
        std::list<const Token*> callstack = { deref, erased };
        reportError(callstack, Severity::error, "eraseDereference",
                    "$symbol:" + itername + "\n"
                    "Iterator '$symbol' used after element has been erased.\n"
                    "The iterator '$symbol' is invalid after the element it pointed to has been erased. "
                    "Dereferencing or comparing it with another iterator is invalid operation.", CWE664, inconclusive);
    } else {
        reportError(deref, Severity::error, "eraseDereference",
                    "$symbol:" + itername + "\n"
                    "Invalid iterator '$symbol' used.\n"
                    "The iterator '$symbol' is invalid before being assigned. "
                    "Dereferencing or comparing it with another iterator is invalid operation.", CWE664, inconclusive);
    }
}

static const Token *skipMembers(const Token *tok)
{
    while (match3(tok))
        tok = tok->tokAt(2);
    return tok;
}

static bool isIterator(const Variable *var, bool& inconclusiveType)
{
    // Check that its an iterator
    if (!var || !var->isLocal() || !match4(var->typeEndToken()))
        return false;

    inconclusiveType = false;
    if (var->typeEndToken()->str() == MatchCompiler::makeConstString("auto"))
        return (var->nameToken()->valueType() && var->nameToken()->valueType()->type == ValueType::Type::ITERATOR);

    if (var->type()) { // If it is defined, ensure that it is defined like an iterator
        // look for operator* and operator++
        const Function* end = var->type()->getFunction("operator*");
        const Function* incOperator = var->type()->getFunction("operator++");
        if (!end || end->argCount() > 0 || !incOperator) {
            return false;
        } else {
            inconclusiveType = true; // heuristics only
        }
    }

    return true;
}

static std::string getContainerName(const Token *containerToken)
{
    if (!containerToken)
        return std::string();
    std::string ret(containerToken->str());
    for (const Token *nametok = containerToken; nametok; nametok = nametok->tokAt(-2)) {
        if (!match3(nametok->tokAt(-2)))
            break;
        ret = nametok->strAt(-2) + '.' + ret;
    }
    return ret;
}

void CheckStl::iterators()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Variable* var : symbolDatabase->variableList()) {
        bool inconclusiveType=false;
        if (!isIterator(var, inconclusiveType))
            continue;
        if (inconclusiveType && !mSettings->inconclusive)
            continue;

        const unsigned int iteratorId = var->declarationId();

        // the validIterator flag says if the iterator has a valid value or not
        bool validIterator = match5(var->nameToken()->next());
        const Scope* invalidationScope = nullptr;

        // The container this iterator can be used with
        const Token* containerToken = nullptr;
        const Scope* containerAssignScope = nullptr;

        // When "validatingToken" is reached the validIterator is set to true
        const Token* validatingToken = nullptr;

        const Token* eraseToken = nullptr;

        // Scan through the rest of the code and see if the iterator is
        // used against other containers.
        for (const Token *tok2 = var->nameToken(); tok2 && tok2 != var->scope()->bodyEnd; tok2 = tok2->next()) {
            if (invalidationScope && tok2 == invalidationScope->bodyEnd)
                validIterator = true; // Assume that the iterator becomes valid again
            if (containerAssignScope && tok2 == containerAssignScope->bodyEnd)
                containerToken = nullptr; // We don't know which containers might be used with the iterator

            if (tok2 == validatingToken) {
                validIterator = true;
                eraseToken = nullptr;
                invalidationScope = nullptr;
            }

            // Is iterator compared against different container?
            if (tok2->isComparisonOp() && containerToken && tok2->astOperand1() && tok2->astOperand2()) {
                const Token *other = nullptr;
                if (tok2->astOperand1()->varId() == iteratorId)
                    other = tok2->astOperand2()->tokAt(-3);
                else if (tok2->astOperand2()->varId() == iteratorId)
                    other = tok2->astOperand1()->tokAt(-3);
                if (match6(other) && other->varId() != containerToken->varId())
                    iteratorsError(tok2, getContainerName(containerToken), getContainerName(other));
            }

            // Is the iterator used in a insert/erase operation?
            else if (match7(tok2, iteratorId)) {
                const Token* itTok = tok2->tokAt(4);
                if (itTok->str() == MatchCompiler::makeConstString("*")) {
                    if (tok2->strAt(2) == MatchCompiler::makeConstString("insert"))
                        continue;

                    itTok = itTok->next();
                }
                // It is bad to insert/erase an invalid iterator
                if (!validIterator)
                    invalidIteratorError(tok2, itTok->str());

                // If insert/erase is used on different container then
                // report an error
                if (containerToken && tok2->varId() != containerToken->varId()) {
                    // skip error message if container is a set..
                    const Variable *variableInfo = tok2->variable();
                    const Token *decltok = variableInfo ? variableInfo->typeStartToken() : nullptr;

                    if (match8(decltok))
                        continue; // No warning

                    // skip error message if the iterator is erased/inserted by value
                    if (itTok->previous()->str() == MatchCompiler::makeConstString("*"))
                        continue;

                    // inserting iterator range..
                    if (tok2->strAt(2) == MatchCompiler::makeConstString("insert")) {
                        const Token *par2 = itTok->nextArgument();
                        if (!par2 || par2->nextArgument())
                            continue;
                        while (par2->str() != MatchCompiler::makeConstString(")")) {
                            if (par2->varId() == containerToken->varId())
                                break;
                            bool inconclusiveType2=false;
                            if (isIterator(par2->variable(), inconclusiveType2))
                                break;  // TODO: check if iterator points at same container
                            if (par2->str() == MatchCompiler::makeConstString("("))
                                par2 = par2->link();
                            par2 = par2->next();
                        }
                        if (par2->str() != MatchCompiler::makeConstString(")"))
                            continue;
                    }

                    // Show error message, mismatching iterator is used.
                    iteratorsError(tok2, getContainerName(containerToken), getContainerName(tok2));
                }

                // invalidate the iterator if it is erased
                else if (tok2->strAt(2) == MatchCompiler::makeConstString("erase") && (tok2->strAt(4) != MatchCompiler::makeConstString("*") || (containerToken && tok2->varId() == containerToken->varId()))) {
                    validIterator = false;
                    eraseToken = tok2;
                    invalidationScope = tok2->scope();
                }

                // skip the operation
                tok2 = itTok->next();
            }

            // it = foo.erase(..
            // taking the result of an erase is ok
            else if (match9(tok2, iteratorId) &&
                     match10(skipMembers(tok2->tokAt(2)))) {
                // the returned iterator is valid
                validatingToken = skipMembers(tok2->tokAt(2))->linkAt(1);
                tok2 = validatingToken->link();
            }

            // Reassign the iterator
            else if (match9(tok2, iteratorId) &&
                     match11(skipMembers(tok2->tokAt(2)))) {
                validatingToken = skipMembers(tok2->tokAt(2))->linkAt(1);
                containerToken = skipMembers(tok2->tokAt(2))->tokAt(-2);
                if (containerToken->varId() == 0 || match12(validatingToken))
                    containerToken = nullptr;
                containerAssignScope = tok2->scope();

                // skip ahead
                tok2 = validatingToken->link();
            }

            // Reassign the iterator
            else if (match13(tok2, iteratorId)) {
                break;
            }

            // Passing iterator to function. Iterator might be initialized
            else if (match14(tok2, iteratorId)) {
                validIterator = true;
            }

            // Dereferencing invalid iterator?
            else if (!validIterator && match15(tok2, iteratorId)) {
                dereferenceErasedError(eraseToken, tok2, tok2->strAt(1), inconclusiveType);
                tok2 = tok2->next();
            } else if (!validIterator && match16(tok2, iteratorId)) {
                dereferenceErasedError(eraseToken, tok2, tok2->str(), inconclusiveType);
                tok2 = tok2->tokAt(2);
            }

            // bailout handling. Assume that the iterator becomes valid if we see return/break.
            // TODO: better handling
            else if (tok2->scope() == invalidationScope && match17(tok2)) {
                validatingToken = findmatch18(tok2->next()) ;
            }

            // bailout handling. Assume that the iterator becomes valid if we see else.
            // TODO: better handling
            else if (tok2->str() == MatchCompiler::makeConstString("else")) {
                validIterator = true;
            }
        }
    }
}


// Error message for bad iterator usage..
void CheckStl::mismatchingContainersError(const Token *tok)
{
    reportError(tok, Severity::error, "mismatchingContainers", "Iterators of different containers are used together.", CWE664, false);
}

void CheckStl::mismatchingContainerExpressionError(const Token *tok1, const Token *tok2)
{
    const std::string expr1(tok1 ? tok1->expressionString() : std::string("v1"));
    const std::string expr2(tok2 ? tok2->expressionString() : std::string("v2"));
    reportError(tok1, Severity::warning, "mismatchingContainerExpression",
                "Iterators to containers from different expressions '" +
                expr1 + "' and '" + expr2 + "' are used together.", CWE664, false);
}

void CheckStl::sameIteratorExpressionError(const Token *tok)
{
    reportError(tok, Severity::style, "sameIteratorExpression", "Same iterators expression are used for algorithm.", CWE664, false);
}

static const std::set<std::string> algorithm2 = { // func(begin1, end1
    "binary_search", "copy", "copy_if", "equal_range"
    , "generate", "is_heap", "is_heap_until", "is_partitioned"
    , "is_permutation", "is_sorted", "is_sorted_until", "lower_bound", "make_heap", "max_element", "minmax_element"
    , "min_element", "mismatch", "move", "move_backward", "next_permutation", "partition", "partition_copy"
    , "partition_point", "pop_heap", "prev_permutation", "push_heap", "random_shuffle", "remove", "remove_copy"
    , "remove_copy_if", "remove_if", "replace", "replace_copy", "replace_copy_if", "replace_if", "reverse", "reverse_copy"
    , "shuffle", "sort", "sort_heap", "stable_partition", "stable_sort", "swap_ranges", "transform", "unique"
    , "unique_copy", "upper_bound", "string", "wstring", "u16string", "u32string"
};
static const std::set<std::string> algorithm22 = { // func(begin1, end1, begin2, end2
    "includes", "lexicographical_compare", "merge", "partial_sort_copy"
    , "set_difference", "set_intersection", "set_symmetric_difference", "set_union"
};
static const std::set<std::string> algorithm1x1 = {  // func(begin1, x, end1
    "nth_element", "partial_sort", "rotate", "rotate_copy"
};

static const std::string iteratorBeginFuncPattern = "begin|cbegin|rbegin|crbegin";
static const std::string iteratorEndFuncPattern = "end|cend|rend|crend";

static const std::string pattern1x1_1 = "%name% . " + iteratorBeginFuncPattern + " ( ) , ";
static const std::string pattern1x1_2 = "%name% . " + iteratorEndFuncPattern + " ( ) ,|)";
static const std::string pattern2 = pattern1x1_1 + pattern1x1_2;

static const Variable *getContainer(const Token *argtok)
{
    while (argtok && argtok->astOperand1())
        argtok = argtok->astOperand1();
    if (!match19(argtok)) // TODO: use Library yield
        return nullptr;
    const Variable *var = argtok->variable();
    if (var && match20(var->typeStartToken()))
        return var;
    return nullptr;
}

static const Token * getIteratorExpression(const Token * tok)
{
    if (!tok)
        return nullptr;
    if (tok->isUnaryOp("*"))
        return nullptr;
    if (!tok->isName()) {
        const Token *iter1 = getIteratorExpression(tok->astOperand1());
        if (iter1)
            return iter1;
        if (tok->str() == MatchCompiler::makeConstString("("))
            return nullptr;
        const Token *iter2 = getIteratorExpression(tok->astOperand2());
        if (iter2)
            return iter2;
    } else if (match21(tok)) {
        if (match22(tok->previous()))
            return tok->previous()->astOperand1();
        if (!match24(tok->previous()) && match23(tok) && !match12(tok->linkAt(1)))
            return tok->next()->astOperand2();
    }
    return nullptr;
}

void CheckStl::mismatchingContainers()
{
    // Check if different containers are used in various calls of standard functions
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match25(tok)) {
                const Token * iter1 = getIteratorExpression(tok->astOperand1());
                const Token * iter2 = getIteratorExpression(tok->astOperand2());
                if (iter1 && iter2 && !isSameExpression(true, false, iter1, iter2, mSettings->library, false, false)) {
                    mismatchingContainerExpressionError(iter1, iter2);
                    continue;
                }
            }
            if (!match23(tok))
                continue;
            const Token * const ftok = tok;
            const Token * firstArg = nullptr;

            const std::vector<const Token *> args = getArguments(ftok);
            if (args.size() < 2)
                continue;

            std::map<const Variable *, unsigned int> containerNr;
            for (unsigned int argnr = 1; argnr <= args.size(); ++argnr) {
                const Library::ArgumentChecks::IteratorInfo *i = mSettings->library.getArgIteratorInfo(ftok, argnr);
                if (!i)
                    continue;
                const Token * const argTok = args[argnr - 1];
                if (i->first) {
                    firstArg = argTok;
                }
                if (i->last && firstArg && argTok && isSameExpression(true, false, firstArg, argTok, mSettings->library, false, false)) {
                    sameIteratorExpressionError(firstArg);
                }
                const Variable *c = getContainer(argTok);
                if (c) {
                    std::map<const Variable *, unsigned int>::const_iterator it = containerNr.find(c);
                    if (it == containerNr.end()) {
                        for (it = containerNr.begin(); it != containerNr.end(); ++it) {
                            if (it->second == i->container) {
                                mismatchingContainersError(argTok);
                                break;
                            }
                        }
                        containerNr[c] = i->container;
                    } else if (it->second != i->container) {
                        mismatchingContainersError(argTok);
                    }
                } else {
                    if (i->last && firstArg && argTok) {
                        const Token * iter1 = getIteratorExpression(firstArg);
                        const Token * iter2 = getIteratorExpression(argTok);
                        if (iter1 && iter2 && !isSameExpression(true, false, iter1, iter2, mSettings->library, false, false)) {
                            mismatchingContainerExpressionError(iter1, iter2);
                        }
                    }
                }
            }
            const int ret = mSettings->library.returnValueContainer(ftok);
            if (ret != -1 && match26(ftok->next()->astParent())) {
                const Token *parent = ftok->next()->astParent();
                const Token *other = (parent->astOperand1() == ftok->next()) ? parent->astOperand2() : parent->astOperand1();
                const Variable *c = getContainer(other);
                if (c) {
                    const std::map<const Variable *, unsigned int>::const_iterator it = containerNr.find(c);
                    if (it == containerNr.end() || it->second != ret)
                        mismatchingContainersError(other);
                }
            }
        }
    }
    for (const Variable *var : symbolDatabase->variableList()) {
        if (var && var->isStlStringType() && match27(var->nameToken()) && Token::Match(var->nameToken()->tokAt(2), pattern2.c_str())) {
            if (var->nameToken()->strAt(2) != var->nameToken()->strAt(8)) {
                mismatchingContainersError(var->nameToken());
            }
        }
    }
}


void CheckStl::stlOutOfBounds()
{
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    // Scan through all scopes..
    for (const Scope &scope : symbolDatabase->scopeList) {
        const Token* tok = scope.classDef;
        // only interested in conditions
        if ((scope.type != Scope::eFor && scope.type != Scope::eWhile && scope.type != Scope::eIf && scope.type != Scope::eDo) || !tok)
            continue;

        if (scope.type == Scope::eFor)
            tok = findmatch18(tok->tokAt(2)) ;
        else if (scope.type == Scope::eDo) {
            tok = tok->linkAt(1)->tokAt(2);
        } else
            tok = tok->next();

        if (!tok)
            continue;
        tok = tok->next();

        // check if the for loop condition is wrong
        if (!match28(tok))
            continue;
        // Is it a vector?
        const Variable *var = tok->tokAt(2)->variable();
        if (!var)
            continue;

        const Library::Container* container = mSettings->library.detectContainer(var->typeStartToken());
        if (!container)
            continue;

        if (container->getYield(tok->strAt(4)) != Library::Container::SIZE)
            continue;

        // variable id for loop variable.
        const unsigned int numId = tok->varId();

        // variable id for the container variable
        const unsigned int declarationId = var->declarationId();

        for (const Token *tok3 = scope.bodyStart; tok3 && tok3 != scope.bodyEnd; tok3 = tok3->next()) {
            if (tok3->varId() == declarationId) {
                tok3 = tok3->next();
                if (match29(tok3)) {
                    if (container->getYield(tok3->strAt(1)) == Library::Container::SIZE)
                        break;
                } else if (container->arrayLike_indexOp && match30(tok3, numId))
                    stlOutOfBoundsError(tok3, tok3->strAt(1), var->name(), false);
                else if (match31(tok3, numId)) {
                    const Library::Container::Yield yield = container->getYield(tok3->strAt(1));
                    if (yield == Library::Container::AT_INDEX)
                        stlOutOfBoundsError(tok3, tok3->strAt(3), var->name(), true);
                }
            }
        }
    }
}

void CheckStl::stlOutOfBoundsError(const Token *tok, const std::string &num, const std::string &var, bool at)
{
    if (at)
        reportError(tok, Severity::error, "stlOutOfBounds", "$symbol:" + var + "\nWhen " + num + "==$symbol.size(), $symbol.at(" + num + ") is out of bounds.", CWE788, false);
    else
        reportError(tok, Severity::error, "stlOutOfBounds", "$symbol:" + var + "\nWhen " + num + "==$symbol.size(), $symbol[" + num + "] is out of bounds.", CWE788, false);
}

void CheckStl::negativeIndex()
{
    // Negative index is out of bounds..
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (!match32(tok) || WRONG_DATA(!tok->next()->astOperand2(), tok))
                continue;
            const Variable * const var = tok->variable();
            if (!var || tok == var->nameToken())
                continue;
            const Library::Container * const container = mSettings->library.detectContainer(var->typeStartToken());
            if (!container || !container->arrayLike_indexOp)
                continue;
            const ValueFlow::Value *index = tok->next()->astOperand2()->getValueLE(-1, mSettings);
            if (!index)
                continue;
            negativeIndexError(tok, *index);
        }
    }
}

void CheckStl::negativeIndexError(const Token *tok, const ValueFlow::Value &index)
{
    const ErrorPath errorPath = getErrorPath(tok, &index, "Negative array index");
    std::ostringstream errmsg;
    if (index.condition)
        errmsg << ValueFlow::eitherTheConditionIsRedundant(index.condition)
               << ", otherwise there is negative array index " << index.intvalue << ".";
    else
        errmsg << "Array index " << index.intvalue << " is out of bounds.";
    reportError(errorPath, index.errorSeverity() ? Severity::error : Severity::warning, "negativeContainerIndex", errmsg.str(), CWE786, index.isInconclusive());
}

void CheckStl::erase()
{
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type == Scope::eFor && match33(scope.classDef)) {
            const Token *tok = scope.classDef->linkAt(1);
            if (!match34(tok->tokAt(-3)))
                continue;
            tok = tok->previous();
            if (!tok->isName())
                tok = tok->previous();
            eraseCheckLoopVar(scope, tok->variable());
        } else if (scope.type == Scope::eWhile && match35(scope.classDef)) {
            eraseCheckLoopVar(scope, scope.classDef->tokAt(2)->variable());
        }
    }
}

void CheckStl::eraseCheckLoopVar(const Scope &scope, const Variable *var)
{
    bool inconclusiveType=false;
    if (!isIterator(var, inconclusiveType))
        return;
    for (const Token *tok = scope.bodyStart; tok != scope.bodyEnd; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("("))
            continue;
        if (!match36(tok->tokAt(-2), var->declarationId()))
            continue;
        if (match37(tok->astParent()))
            continue;
        // Iterator is invalid..
        unsigned int indentlevel = 0U;
        const Token *tok2 = tok->link();
        for (; tok2 != scope.bodyEnd; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("{")) {
                ++indentlevel;
                continue;
            }
            if (tok2->str() == MatchCompiler::makeConstString("}")) {
                if (indentlevel > 0U)
                    --indentlevel;
                else if (match38(tok2))
                    tok2 = tok2->linkAt(2);
                continue;
            }
            if (tok2->varId() == var->declarationId()) {
                if (match37(tok2->next()))
                    break;
                dereferenceErasedError(tok, tok2, tok2->str(), inconclusiveType);
                break;
            }
            if (indentlevel == 0U && match39(tok2))
                break;
        }
        if (tok2 == scope.bodyEnd)
            dereferenceErasedError(tok, scope.classDef, var->nameToken()->str(), inconclusiveType);
    }
}

void CheckStl::pushback()
{
    // Pointer can become invalid after push_back, push_front, reserve or resize..
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match40(tok)) {
                // Skip it directly if it is a pointer or an array
                const Token* containerTok = tok->tokAt(3);
                if (containerTok->variable() && containerTok->variable()->isArrayOrPointer())
                    continue;

                // Variable id for pointer
                const unsigned int pointerId(tok->varId());

                bool invalidPointer = false;
                const Token* function = nullptr;
                const Token* end2 = tok->scope()->bodyEnd;
                for (const Token *tok2 = tok; tok2 != end2; tok2 = tok2->next()) {
                    // push_back on vector..
                    if (match41(tok2, containerTok->varId())) {
                        invalidPointer = true;
                        function = tok2->tokAt(2);
                    }

                    // Using invalid pointer..
                    if (invalidPointer && tok2->varId() == pointerId) {
                        bool unknown = false;
                        if (CheckNullPointer::isPointerDeRef(tok2, unknown))
                            invalidPointerError(tok2, function->str(), tok2->str());
                        break;
                    }
                }
            }
        }
    }

    // Iterator becomes invalid after reserve, resize, insert, push_back or push_front..
    for (const Variable* var : symbolDatabase->variableList()) {
        // Check that its an iterator
        if (!var || !var->isLocal() || !match42(var->typeEndToken()))
            continue;

        const unsigned int iteratorId = var->declarationId();

        // ... on std::vector
        if (!match43(var->typeStartToken()))
            continue;

        // the variable id for the vector
        unsigned int vectorid = 0;

        const Token* validatingToken = nullptr;

        std::string invalidIterator;
        const Token* end2 = var->scope()->bodyEnd;
        for (const Token *tok2 = var->nameToken(); tok2 != end2; tok2 = tok2->next()) {

            if (validatingToken == tok2) {
                invalidIterator.clear();
                validatingToken = nullptr;
            }

            // Using push_back or push_front inside a loop..
            if (match33(tok2)) {
                tok2 = tok2->tokAt(2);
            }

            if (match44(tok2, iteratorId)) {
                // variable id for the loop iterator
                const unsigned int varId(tok2->tokAt(2)->varId());

                const Token *pushbackTok = nullptr;

                // Count { and } for tok3
                const Token *tok3 = tok2->tokAt(20);
                for (const Token* const end3 = tok3->linkAt(-1); tok3 != end3; tok3 = tok3->next()) {
                    if (tok3->str() == MatchCompiler::makeConstString("break") || tok3->str() == MatchCompiler::makeConstString("return")) {
                        pushbackTok = nullptr;
                        break;
                    } else if (match45(tok3, varId) && !tok3->previous()->isAssignmentOp()) {
                        if (tok3->strAt(2) != MatchCompiler::makeConstString("erase") || (tok3->tokAt(4)->varId() != iteratorId && tok3->tokAt(5)->varId() != iteratorId)) // This case is handled in: CheckStl::iterators()
                            pushbackTok = tok3->tokAt(2);
                    }
                }

                if (pushbackTok)
                    invalidIteratorError(pushbackTok, pushbackTok->str(), tok2->str());
            }

            // Assigning iterator..
            if (match13(tok2, iteratorId)) {
                if (match46(tok2->tokAt(2))) {
                    if (!invalidIterator.empty() && match47(tok2->tokAt(4), iteratorId)) {
                        invalidIteratorError(tok2, invalidIterator, var->name());
                        break;
                    }
                    vectorid = tok2->tokAt(2)->varId();
                    tok2 = tok2->linkAt(5);
                } else {
                    vectorid = 0;
                }
                invalidIterator.clear();
            }

            // push_back on vector..
            if (vectorid > 0 && match45(tok2, vectorid)) {
                if (!invalidIterator.empty() && match48(tok2->tokAt(2), iteratorId)) {
                    invalidIteratorError(tok2, invalidIterator, var->name());
                    break;
                }

                if (tok2->strAt(2) != MatchCompiler::makeConstString("erase") || (tok2->tokAt(4)->varId() != iteratorId && tok2->tokAt(5)->varId() != iteratorId)) // This case is handled in: CheckStl::iterators()
                    invalidIterator = tok2->strAt(2);
                tok2 = tok2->linkAt(3);
            }

            else if (tok2->str() == MatchCompiler::makeConstString("return") || tok2->str() == MatchCompiler::makeConstString("throw"))
                validatingToken = findmatch18(tok2->next()) ;

            // TODO: instead of bail out for 'else' try to check all execution paths.
            else if (tok2->str() == MatchCompiler::makeConstString("break") || tok2->str() == MatchCompiler::makeConstString("else"))
                invalidIterator.clear();

            // Using invalid iterator..
            if (!invalidIterator.empty()) {
                if (match49(tok2, iteratorId))
                    invalidIteratorError(tok2, invalidIterator, tok2->strAt(1));
                if (match50(tok2, iteratorId))
                    invalidIteratorError(tok2, invalidIterator, tok2->str());
            }
        }
    }
}


// Error message for bad iterator usage..
void CheckStl::invalidIteratorError(const Token *tok, const std::string &func, const std::string &iterator_name)
{
    reportError(tok, Severity::error, "invalidIterator2",
                "$symbol:" + func + "\n"
                "$symbol:" + iterator_name + "\n"
                "After " + func + "(), the iterator '" + iterator_name + "' may be invalid.", CWE664, false);
}


// Error message for bad iterator usage..
void CheckStl::invalidPointerError(const Token *tok, const std::string &func, const std::string &pointer_name)
{
    reportError(tok, Severity::error, "invalidPointer",
                "$symbol:" + func + "\n"
                "$symbol:" + pointer_name + "\n"
                "Invalid pointer '" + pointer_name + "' after " + func + "().", CWE664, false);
}


void CheckStl::stlBoundaries()
{
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Variable* var : symbolDatabase->variableList()) {
        if (!var || !var->scope() || !var->scope()->isExecutable())
            continue;

        const Library::Container* container = mSettings->library.detectContainer(var->typeStartToken(), true);
        if (!container || container->opLessAllowed)
            continue;

        const Token* const end = var->scope()->bodyEnd;
        for (const Token *tok = var->nameToken(); tok != end; tok = tok->next()) {
            if (match51(tok, var->declarationId())) {
                stlBoundariesError(tok);
            } else if (match52(tok, var->declarationId())) {
                stlBoundariesError(tok);
            }
        }
    }
}

// Error message for bad boundary usage..
void CheckStl::stlBoundariesError(const Token *tok)
{
    reportError(tok, Severity::error, "stlBoundaries",
                "Dangerous comparison using operator< on iterator.\n"
                "Iterator compared with operator<. This is dangerous since the order of items in the "
                "container is not guaranteed. One should use operator!= instead to compare iterators.", CWE664, false);
}

static bool if_findCompare(const Token * const tokBack)
{
    const Token *tok = tokBack->astParent();
    if (!tok)
        return true;
    if (tok->isComparisonOp())
        return (!tok->astOperand1()->isNumber() && !tok->astOperand2()->isNumber());
    if (tok->isArithmeticalOp()) // result is used in some calculation
        return true;  // TODO: check if there is a comparison of the result somewhere
    if (tok->str() == MatchCompiler::makeConstString("."))
        return true; // Dereferencing is OK, the programmer might know that the element exists - TODO: An inconclusive warning might be appropriate
    if (tok->isAssignmentOp())
        return if_findCompare(tok); // Go one step upwards in the AST
    return false;
}

void CheckStl::if_find()
{
    const bool printWarning = mSettings->isEnabled(Settings::WARNING);
    const bool printPerformance = mSettings->isEnabled(Settings::PERFORMANCE);
    if (!printWarning && !printPerformance)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if ((scope.type != Scope::eIf && scope.type != Scope::eWhile) || !scope.classDef)
            continue;

        for (const Token *tok = scope.classDef->next(); tok->str() != MatchCompiler::makeConstString("{"); tok = tok->next()) {
            const Token* funcTok = nullptr;
            const Library::Container* container = nullptr;

            if (match53(tok))
                tok = tok->linkAt(1);

            else if (tok->variable() && match54(tok)) {
                container = mSettings->library.detectContainer(tok->variable()->typeStartToken());
                funcTok = tok->tokAt(2);
            }

            // check also for vector-like or pointer containers
            else if (tok->variable() && tok->astParent() && (tok->astParent()->str() == MatchCompiler::makeConstString("*") || tok->astParent()->str() == MatchCompiler::makeConstString("["))) {
                const Token *tok2 = tok->astParent();

                if (!match55(tok2->astParent()))
                    continue;

                funcTok = tok2->astParent()->next();

                if (tok->variable()->isArrayOrPointer())
                    container = mSettings->library.detectContainer(tok->variable()->typeStartToken());
                else { // Container of container - find the inner container
                    container = mSettings->library.detectContainer(tok->variable()->typeStartToken()); // outer container
                    tok2 = findmatch56(tok->variable()->typeStartToken(), tok->variable()->typeEndToken()) ;
                    if (container && container->type_templateArgNo >= 0 && tok2) {
                        tok2 = tok2->next();
                        for (int j = 0; j < container->type_templateArgNo; j++)
                            tok2 = tok2->nextTemplateArgument();

                        container = mSettings->library.detectContainer(tok2); // innner container
                    } else
                        container = nullptr;
                }
            }

            if (container && container->getAction(funcTok->str()) == Library::Container::FIND) {
                if (if_findCompare(funcTok->next()))
                    continue;

                if (printWarning && container->getYield(funcTok->str()) == Library::Container::ITERATOR)
                    if_findError(tok, false);
                else if (printPerformance && container->stdStringLike && funcTok->str() == MatchCompiler::makeConstString("find"))
                    if_findError(tok, true);
            } else if (printWarning && match57(tok)) {
                // check that result is checked properly
                if (!if_findCompare(tok->tokAt(3))) {
                    if_findError(tok, false);
                }
            }
        }
    }
}


void CheckStl::if_findError(const Token *tok, bool str)
{
    if (str)
        reportError(tok, Severity::performance, "stlIfStrFind",
                    "Inefficient usage of string::find() in condition; string::compare() would be faster.\n"
                    "Either inefficient or wrong usage of string::find(). string::compare() will be faster if "
                    "string::find's result is compared with 0, because it will not scan the whole "
                    "string. If your intention is to check that there are no findings in the string, "
                    "you should compare with std::string::npos.", CWE597, false);
    else
        reportError(tok, Severity::warning, "stlIfFind", "Suspicious condition. The result of find() is an iterator, but it is not properly checked.", CWE398, false);
}


/**
 * Is container.size() slow?
 */
static bool isCpp03ContainerSizeSlow(const Token *tok)
{
    if (!tok)
        return false;
    const Variable* var = tok->variable();
    return var && var->isStlType("list");
}

void CheckStl::size()
{
    if (!mSettings->isEnabled(Settings::PERFORMANCE))
        return;

    if (mSettings->standards.cpp == Standards::CPP11)
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match58(tok) ||
                match59(tok)) {
                // get the variable
                const Token *varTok = tok;
                if (tok->strAt(2) != MatchCompiler::makeConstString("size"))
                    varTok = varTok->tokAt(2);

                const Token* const end = varTok->tokAt(5);

                // check for comparison to zero
                if ((tok->previous() && !tok->previous()->isArithmeticalOp() && match60(end)) ||
                    (end->next() && !end->next()->isArithmeticalOp() && match61(tok->tokAt(-2)))) {
                    if (isCpp03ContainerSizeSlow(varTok)) {
                        sizeError(varTok);
                        continue;
                    }
                }

                // check for comparison to one
                if ((tok->previous() && !tok->previous()->isArithmeticalOp() && match62(end) && !end->tokAt(2)->isArithmeticalOp()) ||
                    (end->next() && !end->next()->isArithmeticalOp() && match63(tok->tokAt(-2)) && !tok->tokAt(-3)->isArithmeticalOp())) {
                    if (isCpp03ContainerSizeSlow(varTok))
                        sizeError(varTok);
                }

                // check for using as boolean expression
                else if ((match64(tok->tokAt(-2)) && end->str() == MatchCompiler::makeConstString(")")) ||
                         (tok->previous()->tokType() == Token::eLogicalOp && match65(end))) {
                    if (isCpp03ContainerSizeSlow(varTok))
                        sizeError(varTok);
                }
            }
        }
    }
}

void CheckStl::sizeError(const Token *tok)
{
    const std::string varname = tok ? tok->str() : std::string("list");
    reportError(tok, Severity::performance, "stlSize",
                "$symbol:" + varname + "\n"
                "Possible inefficient checking for '$symbol' emptiness.\n"
                "Checking for '$symbol' emptiness might be inefficient. "
                "Using $symbol.empty() instead of $symbol.size() can be faster. "
                "$symbol.size() can take linear time but $symbol.empty() is "
                "guaranteed to take constant time.", CWE398, false);
}

void CheckStl::redundantCondition()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eIf)
            continue;

        const Token* tok = scope.classDef->tokAt(2);
        if (!match66(tok))
            continue;

        // Get tokens for the fields %name% and %any%
        const Token *var1 = tok;
        const Token *any1 = var1->tokAt(4);
        const Token *var2 = any1->tokAt(3);
        const Token *var3 = var2->tokAt(7);
        const Token *any2 = var3->tokAt(4);

        // Check if all the "%name%" fields are the same and if all the "%any%" are the same..
        if (var1->str() == var2->str() &&
            var2->str() == var3->str() &&
            any1->str() == any2->str()) {
            redundantIfRemoveError(tok);
        }
    }
}

void CheckStl::redundantIfRemoveError(const Token *tok)
{
    reportError(tok, Severity::style, "redundantIfRemove",
                "Redundant checking of STL container element existence before removing it.\n"
                "Redundant checking of STL container element existence before removing it. "
                "It is safe to call the remove method on a non-existing element.", CWE398, false);
}

void CheckStl::missingComparison()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eFor || !scope.classDef)
            continue;

        for (const Token *tok2 = scope.classDef->tokAt(2); tok2 != scope.bodyStart; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString(";"))
                break;

            if (!match67(tok2))
                continue;

            // same container
            if (tok2->strAt(2) != tok2->strAt(10))
                break;

            const unsigned int iteratorId(tok2->varId());

            // same iterator
            if (iteratorId == tok2->tokAt(10)->varId())
                break;

            // increment iterator
            if (!match68(tok2->tokAt(16), iteratorId) &&
                !match69(tok2->tokAt(16), iteratorId)) {
                break;
            }

            const Token *incrementToken = nullptr;

            // Parse loop..
            for (const Token *tok3 = scope.bodyStart; tok3 != scope.bodyEnd; tok3 = tok3->next()) {
                if (match70(tok3, iteratorId))
                    incrementToken = tok3;
                else if (match71(tok3->previous(), iteratorId))
                    incrementToken = tok3;
                else if (match72(tok3, iteratorId))
                    incrementToken = nullptr;
                else if (tok3->str() == MatchCompiler::makeConstString("break") || tok3->str() == MatchCompiler::makeConstString("return"))
                    incrementToken = nullptr;
                else if (match73(tok3, iteratorId)) {
                    // skip insertion..
                    tok3 = tok3->linkAt(6);
                    if (!tok3)
                        break;
                }
            }
            if (incrementToken)
                missingComparisonError(incrementToken, tok2->tokAt(16));
        }
    }
}

void CheckStl::missingComparisonError(const Token *incrementToken1, const Token *incrementToken2)
{
    std::list<const Token*> callstack = { incrementToken1,incrementToken2 };

    std::ostringstream errmsg;
    errmsg << "Missing bounds check for extra iterator increment in loop.\n"
           << "The iterator incrementing is suspicious - it is incremented at line ";
    if (incrementToken1)
        errmsg << incrementToken1->linenr();
    errmsg << " and then at line ";
    if (incrementToken2)
        errmsg << incrementToken2->linenr();
    errmsg << ". The loop might unintentionally skip an element in the container. "
           << "There is no comparison between these increments to prevent that the iterator is "
           << "incremented beyond the end.";

    reportError(callstack, Severity::warning, "StlMissingComparison", errmsg.str(), CWE834, false);
}


static bool isLocal(const Token *tok)
{
    const Variable *var = tok->variable();
    return var && !var->isStatic() && var->isLocal();
}

namespace {
    const std::set<std::string> stl_string_stream = {
        "istringstream", "ostringstream", "stringstream", "wstringstream"
    };
}

void CheckStl::string_c_str()
{
    const bool printInconclusive = mSettings->inconclusive;
    const bool printPerformance = mSettings->isEnabled(Settings::PERFORMANCE);

    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();

    // Find all functions that take std::string as argument
    std::multimap<std::string, unsigned int> c_strFuncParam;
    if (printPerformance) {
        for (const Scope &scope : symbolDatabase->scopeList) {
            for (const Function &func : scope.functionList) {
                if (c_strFuncParam.erase(func.tokenDef->str()) != 0) { // Check if function with this name was already found
                    c_strFuncParam.insert(std::make_pair(func.tokenDef->str(), 0)); // Disable, because there are overloads. TODO: Handle overloads
                    continue;
                }

                unsigned int numpar = 0;
                c_strFuncParam.insert(std::make_pair(func.tokenDef->str(), numpar)); // Insert function as dummy, to indicate that there is at least one function with that name
                for (const Variable &var : func.argumentList) {
                    numpar++;
                    if (var.isStlStringType() && (!var.isReference() || var.isConst()))
                        c_strFuncParam.insert(std::make_pair(func.tokenDef->str(), numpar));
                }
            }
        }
    }

    // Try to detect common problems when using string::c_str()
    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eFunction || !scope.function)
            continue;

        enum {charPtr, stdString, stdStringConstRef, Other} returnType = Other;
        if (match74(scope.function->tokenDef->tokAt(-2)))
            returnType = charPtr;
        else if (match75(scope.function->tokenDef->tokAt(-5)))
            returnType = stdStringConstRef;
        else if (match76(scope.function->tokenDef->tokAt(-3)))
            returnType = stdString;

        for (const Token *tok = scope.bodyStart; tok && tok != scope.bodyEnd; tok = tok->next()) {
            // Invalid usage..
            if (match77(tok) && isLocal(tok->next()) &&
                tok->next()->variable() && tok->next()->variable()->isStlStringType()) {
                string_c_strThrowError(tok);
            } else if (match78(tok)) {
                const Variable* var = tok->next()->variable();
                const Variable* var2 = tok->tokAt(3)->variable();
                if (var && var->isPointer() && var2 && var2->isStlType(stl_string_stream))
                    string_c_strError(tok);
            } else if (match79(tok) &&
                       match80(tok->linkAt(4)) &&
                       tok->tokAt(3)->function() && match81(tok->tokAt(3)->function()->retDef)) {
                const Variable* var = tok->next()->variable();
                if (var && var->isPointer())
                    string_c_strError(tok);
            } else if (printPerformance && match23(tok) && c_strFuncParam.find(tok->str()) != c_strFuncParam.end() &&
                       !match82(tok->previous()) && tok->varId() == 0 && tok->str() != scope.className) { // calling function. TODO: Add support for member functions
                const std::pair<std::multimap<std::string, unsigned int>::const_iterator, std::multimap<std::string, unsigned int>::const_iterator> range = c_strFuncParam.equal_range(tok->str());
                for (std::multimap<std::string, unsigned int>::const_iterator i = range.first; i != range.second; ++i) {
                    if (i->second == 0)
                        continue;

                    const Token* tok2 = tok->tokAt(2);
                    unsigned int j;
                    for (j = 0; tok2 && j < i->second-1; j++)
                        tok2 = tok2->nextArgument();
                    if (tok2)
                        tok2 = tok2->nextArgument();
                    else
                        break;
                    if (!tok2 && j == i->second-1)
                        tok2 = tok->next()->link();
                    else if (tok2)
                        tok2 = tok2->previous();
                    else
                        break;
                    if (tok2 && match83(tok2->tokAt(-4))) {
                        const Variable* var = tok2->tokAt(-5)->variable();
                        if (var && var->isStlStringType()) {
                            string_c_strParam(tok, i->second);
                        } else if (match84(tok2->tokAt(-9))) { // Check ss.str().c_str() as parameter
                            const Variable* ssVar = tok2->tokAt(-9)->variable();
                            if (ssVar && ssVar->isStlType(stl_string_stream))
                                string_c_strParam(tok, i->second);
                        }

                    }
                }
            }

            // Using c_str() to get the return value is only dangerous if the function returns a char*
            if ((returnType == charPtr || (printPerformance && (returnType == stdString || returnType == stdStringConstRef))) && tok->str() == MatchCompiler::makeConstString("return")) {
                bool err = false;

                const Token* tok2 = tok->next();
                if (match85(tok2) &&
                    match80(tok2->linkAt(3))) {
                    err = true;
                } else if (match86(tok2) &&
                           match80(tok2->link())) {
                    // Check for "+ localvar" or "+ std::string(" inside the bracket
                    bool is_implicit_std_string = printInconclusive;
                    const Token *search_end = tok2->link();
                    for (const Token *search_tok = tok2->next(); search_tok != search_end; search_tok = search_tok->next()) {
                        if (match87(search_tok) && isLocal(search_tok->next()) &&
                            search_tok->next()->variable() && search_tok->next()->variable()->isStlStringType()) {
                            is_implicit_std_string = true;
                            break;
                        } else if (match88(search_tok)) {
                            is_implicit_std_string = true;
                            break;
                        }
                    }

                    if (is_implicit_std_string)
                        err = true;
                }

                bool local = false;
                bool ptrOrRef = false;
                const Variable* lastVar = nullptr;
                const Function* lastFunc = nullptr;
                bool funcStr = false;
                if (match89(tok2)) {
                    local = isLocal(tok2);
                    bool refToNonLocal = false;
                    if (tok2->variable() && tok2->variable()->isReference()) {
                        const Token *refTok = tok2->variable()->nameToken();
                        refToNonLocal = true; // safe assumption is default to avoid FPs
                        if (match90(refTok))
                            refToNonLocal = !isLocal(refTok->tokAt(2));
                    }
                    ptrOrRef = refToNonLocal || (tok2->variable() && tok2->variable()->isPointer());
                }
                while (tok2) {
                    if (match91(tok2)) {
                        if (ptrOrRef)
                            local = false;
                        lastVar = tok2->variable();
                        tok2 = tok2->tokAt(2);
                    } else if (match53(tok2) && match12(tok2->linkAt(1))) {
                        lastFunc = tok2->function();
                        local = false;
                        funcStr = tok2->str() == MatchCompiler::makeConstString("str");
                        tok2 = tok2->linkAt(1)->tokAt(2);
                    } else
                        break;
                }

                if (match92(tok2)) {
                    if ((local || returnType != charPtr) && lastVar && lastVar->isStlStringType())
                        err = true;
                    else if (funcStr && lastVar && lastVar->isStlType(stl_string_stream))
                        err = true;
                    else if (lastFunc && match93(lastFunc->tokenDef->tokAt(-3)))
                        err = true;
                }

                if (err) {
                    if (returnType == charPtr)
                        string_c_strError(tok);
                    else
                        string_c_strReturn(tok);
                }
            }
        }
    }
}

void CheckStl::string_c_strThrowError(const Token* tok)
{
    reportError(tok, Severity::error, "stlcstrthrow", "Dangerous usage of c_str(). The value returned by c_str() is invalid after throwing exception.\n"
                "Dangerous usage of c_str(). The string is destroyed after the c_str() call so the thrown pointer is invalid.");
}

void CheckStl::string_c_strError(const Token* tok)
{
    reportError(tok, Severity::error, "stlcstr", "Dangerous usage of c_str(). The value returned by c_str() is invalid after this call.\n"
                "Dangerous usage of c_str(). The c_str() return value is only valid until its string is deleted.", CWE664, false);
}

void CheckStl::string_c_strReturn(const Token* tok)
{
    reportError(tok, Severity::performance, "stlcstrReturn", "Returning the result of c_str() in a function that returns std::string is slow and redundant.\n"
                "The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly returning the string.", CWE704, false);
}

void CheckStl::string_c_strParam(const Token* tok, unsigned int number)
{
    std::ostringstream oss;
    oss << "Passing the result of c_str() to a function that takes std::string as argument no. " << number << " is slow and redundant.\n"
        "The conversion from const char* as returned by c_str() to std::string creates an unnecessary string copy. Solve that by directly passing the string.";
    reportError(tok, Severity::performance, "stlcstrParam", oss.str(), CWE704, false);
}

static bool hasArrayEnd(const Token *tok1)
{
    const Token *end = findmatch18(tok1) ;
    return (end && match94(end->previous()));
}

static bool hasArrayEndParen(const Token *tok1)
{
    const Token *end = findmatch18(tok1) ;
    return (end && end->previous() &&
            match95(end->tokAt(-2)));
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void CheckStl::checkAutoPointer()
{
    std::set<unsigned int> autoPtrVarId;
    std::map<unsigned int, const std::string> mallocVarId; // variables allocated by the malloc-like function
    const char STL_CONTAINER_LIST[] = "array|bitset|deque|list|forward_list|map|multimap|multiset|priority_queue|queue|set|stack|vector|hash_map|hash_multimap|hash_set|unordered_map|unordered_multimap|unordered_set|unordered_multiset|basic_string";
    const int malloc = mSettings->library.allocId("malloc"); // allocation function, which are not compatible with auto_ptr
    const bool printStyle = mSettings->isEnabled(Settings::STYLE);

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match96(tok)) {
            if ((tok->strAt(-1) == MatchCompiler::makeConstString("<") && Token::Match(tok->tokAt(-2), STL_CONTAINER_LIST)) ||
                (Token::simpleMatch(tok->tokAt(-3), "< std :: auto_ptr") && Token::Match(tok->tokAt(-4), STL_CONTAINER_LIST))) {
                autoPointerContainerError(tok);
            } else {
                const Token *tok2 = tok->linkAt(1);

                if (match97(tok2)) {
                    const Token *tok3 = tok2->tokAt(2);
                    if (match98(tok3) && hasArrayEndParen(tok3)) {
                        autoPointerArrayError(tok2->next());
                    }
                    if (match99(tok3) && malloc && mSettings->library.alloc(tok3->next(), -1) == malloc) {
                        // malloc-like function allocated memory passed to the auto_ptr constructor -> error
                        autoPointerMallocError(tok2->next(), tok3->next()->str());
                    }
                    if (match100(tok3)) {
                        const std::map<unsigned int, const std::string>::const_iterator it = mallocVarId.find(tok3->next()->varId());
                        if (it != mallocVarId.cend()) {
                            // pointer on the memory allocated by malloc used in the auto pointer constructor -> error
                            autoPointerMallocError(tok2->next(), it->second);
                        }
                    }
                    while (tok3 && tok3->str() != MatchCompiler::makeConstString(";")) {
                        tok3 = tok3->next();
                    }
                    if (tok3) {
                        tok3 = tok3->tokAt(-2);
                        if (match101(tok3->previous())) {
                            autoPointerArrayError(tok2->next());
                        } else if (tok3->varId()) {
                            const Token *decltok = findmatch102(mTokenizer->tokens(), tok3->varId()) ;
                            if (decltok && hasArrayEnd(decltok)) {
                                autoPointerArrayError(tok2->next());
                            }
                        }
                        if (tok2->next()->varId()) {
                            autoPtrVarId.insert(tok2->next()->varId());
                        }
                    }
                }
            }
        } else {
            if (match103(tok)) {
                if (printStyle) {
                    const std::set<unsigned int>::const_iterator iter = autoPtrVarId.find(tok->tokAt(2)->varId());
                    if (iter != autoPtrVarId.end()) {
                        autoPointerError(tok->tokAt(2));
                    }
                }
            } else if ((match104(tok) && hasArrayEnd(tok)) ||
                       (match105(tok) && hasArrayEndParen(tok))) {
                const std::set<unsigned int>::const_iterator iter = autoPtrVarId.find(tok->varId());
                if (iter != autoPtrVarId.end()) {
                    autoPointerArrayError(tok);
                }
            } else if (match106(tok) && malloc && mSettings->library.alloc(tok->tokAt(2), -1) == malloc) {
                // C library function like 'malloc' used together with auto pointer -> error
                const std::set<unsigned int>::const_iterator iter = autoPtrVarId.find(tok->varId());
                if (iter != autoPtrVarId.end()) {
                    autoPointerMallocError(tok, tok->strAt(2));
                } else if (tok->varId()) {
                    // it is not an auto pointer variable and it is allocated by malloc like function.
                    mallocVarId.insert(std::make_pair(tok->varId(), tok->strAt(2)));
                }
            } else if (match107(tok) && malloc && mSettings->library.alloc(tok->tokAt(4), -1) == malloc) {
                // C library function like 'malloc' used when resetting auto pointer -> error
                const std::set<unsigned int>::const_iterator iter = autoPtrVarId.find(tok->varId());
                if (iter != autoPtrVarId.end()) {
                    autoPointerMallocError(tok, tok->strAt(4));
                }
            }
        }
    }
}


void CheckStl::autoPointerError(const Token *tok)
{
    reportError(tok, Severity::style, "useAutoPointerCopy",
                "Copying 'auto_ptr' pointer to another does not create two equal objects since one has lost its ownership of the pointer.\n"
                "'std::auto_ptr' has semantics of strict ownership, meaning that the 'auto_ptr' instance is the sole entity responsible for the object's lifetime. If an 'auto_ptr' is copied, the source looses the reference.",
                CWE398, false);
}

void CheckStl::autoPointerContainerError(const Token *tok)
{
    reportError(tok, Severity::error, "useAutoPointerContainer",
                "You can randomly lose access to pointers if you store 'auto_ptr' pointers in an STL container.\n"
                "An element of container must be able to be copied but 'auto_ptr' does not fulfill this requirement. You should consider to use 'shared_ptr' or 'unique_ptr'. It is suitable for use in containers, because they no longer copy their values, they move them.", CWE664, false
               );
}

void CheckStl::autoPointerArrayError(const Token *tok)
{
    reportError(tok, Severity::error, "useAutoPointerArray",
                "Object pointed by an 'auto_ptr' is destroyed using operator 'delete'. You should not use 'auto_ptr' for pointers obtained with operator 'new[]'.\n"
                "Object pointed by an 'auto_ptr' is destroyed using operator 'delete'. This means that you should only use 'auto_ptr' for pointers obtained with operator 'new'. This excludes arrays, which are allocated by operator 'new[]' and must be deallocated by operator 'delete[]'.", CWE664, false
               );
}

void CheckStl::autoPointerMallocError(const Token *tok, const std::string& allocFunction)
{
    const std::string summary = "Object pointed by an 'auto_ptr' is destroyed using operator 'delete'. You should not use 'auto_ptr' for pointers obtained with function '$symbol'.";
    const std::string verbose = summary + " This means that you should only use 'auto_ptr' for pointers obtained with operator 'new'. This excludes use C library allocation functions (for example '$symbol'), which must be deallocated by the appropriate C library function.";
    reportError(tok, Severity::error, "useAutoPointerMalloc", "$symbol:" + allocFunction + '\n' + summary + '\n' + verbose, CWE762, false);
}

namespace {
    const std::set<std::string> stl_containers_with_empty_and_clear = {
        "deque",  "forward_list",  "list",
        "map",  "multimap",  "multiset",  "set",  "string",
        "unordered_map",  "unordered_multimap",  "unordered_multiset",
        "unordered_set",  "vector",  "wstring"
    };

}

void CheckStl::uselessCalls()
{
    const bool printPerformance = mSettings->isEnabled(Settings::PERFORMANCE);
    const bool printWarning = mSettings->isEnabled(Settings::WARNING);
    if (!printPerformance && !printWarning)
        return;

    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (printWarning && match108(tok) &&
                tok->varId() == tok->tokAt(4)->varId()) {
                const Variable* var = tok->variable();
                if (!var || !var->isStlType())
                    continue;
                uselessCallsReturnValueError(tok->tokAt(4), tok->str(), tok->strAt(2));
            } else if (printPerformance && match109(tok) &&
                       tok->varId() == tok->tokAt(4)->varId()) {
                const Variable* var = tok->variable();
                if (!var || !var->isStlType())
                    continue;
                uselessCallsSwapError(tok, tok->str());
            } else if (printPerformance && match110(tok) &&
                       tok->variable() && tok->variable()->isStlStringType()) {
                if (match111(tok->tokAt(4))) {
                    uselessCallsSubstrError(tok, false);
                } else if (tok->strAt(4) == MatchCompiler::makeConstString("0") && tok->linkAt(3)->strAt(-1) == MatchCompiler::makeConstString("npos")) {
                    if (!tok->linkAt(3)->previous()->variable()) // Make sure that its no variable
                        uselessCallsSubstrError(tok, false);
                } else if (match112(tok->linkAt(3)->tokAt(-2)))
                    uselessCallsSubstrError(tok, true);
            } else if (printWarning && match113(tok) &&
                       !tok->tokAt(4)->astParent() &&
                       tok->next()->variable() && tok->next()->variable()->isStlType(stl_containers_with_empty_and_clear))
                uselessCallsEmptyError(tok->next());
            else if (match114(tok) && tok->tokAt(5)->nextArgument())
                uselessCallsRemoveError(tok->next(), tok->strAt(3));
        }
    }
}


void CheckStl::uselessCallsReturnValueError(const Token *tok, const std::string &varname, const std::string &function)
{
    std::ostringstream errmsg;
    errmsg << "$symbol:" << varname << '\n';
    errmsg << "$symbol:" << function << '\n';
    errmsg << "It is inefficient to call '" << varname << "." << function << "(" << varname << ")' as it always returns 0.\n"
           << "'std::string::" << function << "()' returns zero when given itself as parameter "
           << "(" << varname << "." << function << "(" << varname << ")). As it is currently the "
           << "code is inefficient. It is possible either the string searched ('"
           << varname << "') or searched for ('" << varname << "') is wrong.";
    reportError(tok, Severity::warning, "uselessCallsCompare", errmsg.str(), CWE628, false);
}

void CheckStl::uselessCallsSwapError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::performance, "uselessCallsSwap",
                "$symbol:" + varname + "\n"
                "It is inefficient to swap a object with itself by calling '$symbol.swap($symbol)'\n"
                "The 'swap()' function has no logical effect when given itself as parameter "
                "($symbol.swap($symbol)). As it is currently the "
                "code is inefficient. Is the object or the parameter wrong here?", CWE628, false);
}

void CheckStl::uselessCallsSubstrError(const Token *tok, bool empty)
{
    if (empty)
        reportError(tok, Severity::performance, "uselessCallsSubstr", "Ineffective call of function 'substr' because it returns an empty string.", CWE398, false);
    else
        reportError(tok, Severity::performance, "uselessCallsSubstr", "Ineffective call of function 'substr' because it returns a copy of the object. Use operator= instead.", CWE398, false);
}

void CheckStl::uselessCallsEmptyError(const Token *tok)
{
    reportError(tok, Severity::warning, "uselessCallsEmpty", "Ineffective call of function 'empty()'. Did you intend to call 'clear()' instead?", CWE398, false);
}

void CheckStl::uselessCallsRemoveError(const Token *tok, const std::string& function)
{
    reportError(tok, Severity::warning, "uselessCallsRemove",
                "$symbol:" + function + "\n"
                "Return value of std::$symbol() ignored. Elements remain in container.\n"
                "The return value of std::$symbol() is ignored. This function returns an iterator to the end of the range containing those elements that should be kept. "
                "Elements past new end remain valid but with unspecified values. Use the erase method of the container to delete them.", CWE762, false);
}

// Check for iterators being dereferenced before being checked for validity.
// E.g.  if (*i && i != str.end()) { }
void CheckStl::checkDereferenceInvalidIterator()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    // Iterate over "if", "while", and "for" conditions where there may
    // be an iterator that is dereferenced before being checked for validity.
    for (const Scope &scope : mTokenizer->getSymbolDatabase()->scopeList) {
        if (!(scope.type == Scope::eIf || scope.type == Scope::eDo || scope.type == Scope::eWhile || scope.type == Scope::eFor))
            continue;

        const Token* const tok = scope.classDef;
        const Token* startOfCondition = tok->next();
        if (scope.type == Scope::eDo)
            startOfCondition = startOfCondition->link()->tokAt(2);
        if (!startOfCondition) // ticket #6626 invalid code
            continue;
        const Token* endOfCondition = startOfCondition->link();
        if (!endOfCondition)
            continue;

        // For "for" loops, only search between the two semicolons
        if (scope.type == Scope::eFor) {
            startOfCondition = findmatch115(tok->tokAt(2), endOfCondition) ;
            if (!startOfCondition)
                continue;
            endOfCondition = findmatch115(startOfCondition->next(), endOfCondition) ;
            if (!endOfCondition)
                continue;
        }

        // Only consider conditions composed of all "&&" terms and
        // conditions composed of all "||" terms
        const bool isOrExpression =
            findmatch116(startOfCondition, endOfCondition)  != nullptr;
        const bool isAndExpression =
            findmatch117(startOfCondition, endOfCondition)  != nullptr;

        // Look for a check of the validity of an iterator
        const Token* validityCheckTok = nullptr;
        if (!isOrExpression && isAndExpression) {
            validityCheckTok =
                findmatch118(startOfCondition, endOfCondition) ;
        } else if (isOrExpression && !isAndExpression) {
            validityCheckTok =
                findmatch119(startOfCondition, endOfCondition) ;
        }

        if (!validityCheckTok)
            continue;
        const unsigned int iteratorVarId = validityCheckTok->next()->varId();

        // If the iterator dereference is to the left of the check for
        // the iterator's validity, report an error.
        const Token* const dereferenceTok =
            findmatch120(startOfCondition, validityCheckTok, iteratorVarId) ;
        if (dereferenceTok)
            dereferenceInvalidIteratorError(dereferenceTok, dereferenceTok->strAt(1));
    }
}

void CheckStl::dereferenceInvalidIteratorError(const Token* deref, const std::string &iterName)
{
    reportError(deref, Severity::warning,
                "derefInvalidIterator",
                "$symbol:" + iterName + "\n"
                "Possible dereference of an invalid iterator: $symbol\n"
                "Possible dereference of an invalid iterator: $symbol. Make sure to check that the iterator is valid before dereferencing it - not after.", CWE825, false);
}


void CheckStl::readingEmptyStlContainer2()
{
    for (const Scope *function : mTokenizer->getSymbolDatabase()->functionScopes) {
        for (const Token *tok = function->bodyStart; tok != function->bodyEnd; tok = tok->next()) {
            if (!tok->isName() || !tok->valueType())
                continue;
            const Library::Container *container = tok->valueType()->container;
            if (!container)
                continue;
            const ValueFlow::Value *value = tok->getContainerSizeValue(0);
            if (!value)
                continue;
            if (value->isInconclusive() && !mSettings->inconclusive)
                continue;
            if (!value->errorSeverity() && !mSettings->isEnabled(Settings::WARNING))
                continue;
            if (match1(tok)) {
                if (container->getYield(tok->strAt(2)) == Library::Container::Yield::ITEM)
                    readingEmptyStlContainerError(tok,value);
            }
        }
    }
}

void CheckStl::readingEmptyStlContainerError(const Token *tok, const ValueFlow::Value *value)
{
    const std::string varname = tok ? tok->str() : std::string("var");

    std::string errmsg;
    if (value && value->condition)
        errmsg = "Reading from container '$symbol'. " + ValueFlow::eitherTheConditionIsRedundant(value->condition) + " or '$symbol' can be empty.";
    else
        errmsg = "Reading from empty STL container '$symbol'";

    const ErrorPath errorPath = getErrorPath(tok, value, "Reading from empty container");

    reportError(errorPath, value ? (value->errorSeverity() ? Severity::error : Severity::warning) : Severity::style, "reademptycontainer", "$symbol:" + varname +"\n" + errmsg, CWE398, !value);
}

void CheckStl::useStlAlgorithmError(const Token *tok, const std::string &algoName)
{
    reportError(tok, Severity::style, "useStlAlgorithm",
                "Consider using " + algoName + " algorithm instead of a raw loop.", CWE398, false);
}

static bool isEarlyExit(const Token *start)
{
    if (start->str() != MatchCompiler::makeConstString("{"))
        return false;
    const Token *endToken = start->link();
    const Token *tok = findmatch121(start, endToken) ;
    if (!tok)
        return false;
    const Token *endStatement = findmatch122(tok, endToken) ;
    if (!endStatement)
        return false;
    if (endStatement->next() != endToken)
        return false;
    return true;
}

static const Token *singleStatement(const Token *start)
{
    if (start->str() != MatchCompiler::makeConstString("{"))
        return nullptr;
    const Token *endToken = start->link();
    const Token *endStatement = findmatch18(start->next()) ;
    if (!match123(endStatement))
        return nullptr;
    if (endStatement->next() != endToken)
        return nullptr;
    return endStatement;
}

static const Token *singleAssignInScope(const Token *start, unsigned int varid, bool &input)
{
    const Token *endStatement = singleStatement(start);
    if (!endStatement)
        return nullptr;
    if (!match124(start->next()))
        return nullptr;
    const Token *assignTok = start->tokAt(2);
    if (isVariableChanged(assignTok->next(), endStatement, assignTok->astOperand1()->varId(), false, nullptr, true))
        return nullptr;
    if (isVariableChanged(assignTok->next(), endStatement, varid, false, nullptr, true))
        return nullptr;
    input = findmatch125(assignTok->next(), endStatement, varid) ;
    return assignTok;
}

static const Token *singleMemberCallInScope(const Token *start, unsigned int varid, bool &input)
{
    if (start->str() != MatchCompiler::makeConstString("{"))
        return nullptr;
    const Token *endToken = start->link();
    if (!match54(start->next()))
        return nullptr;
    if (!match126(start->linkAt(4)))
        return nullptr;
    const Token *endStatement = start->linkAt(4)->next();
    if (endStatement->next() != endToken)
        return nullptr;

    const Token *dotTok = start->tokAt(2);
    if (!findmatch125(dotTok->tokAt(2), endStatement, varid) )
        return nullptr;
    input = match127(start->next(), varid);
    if (isVariableChanged(dotTok->next(), endStatement, dotTok->astOperand1()->varId(), false, nullptr, true))
        return nullptr;
    return dotTok;
}

static const Token *singleIncrementInScope(const Token *start, unsigned int varid, bool &input)
{
    if (start->str() != MatchCompiler::makeConstString("{"))
        return nullptr;
    const Token *varTok = nullptr;
    if (match128(start->next()))
        varTok = start->tokAt(2);
    else if (match129(start->next()))
        varTok = start->tokAt(1);
    if (!varTok)
        return nullptr;
    input = varTok->varId() == varid;
    return varTok;
}

static const Token *singleConditionalInScope(const Token *start, unsigned int varid)
{
    if (start->str() != MatchCompiler::makeConstString("{"))
        return nullptr;
    const Token *endToken = start->link();
    if (!match130(start->next()))
        return nullptr;
    if (!match131(start->linkAt(2)))
        return nullptr;
    const Token *bodyTok = start->linkAt(2)->next();
    const Token *endBodyTok = bodyTok->link();
    if (!match132(endBodyTok))
        return nullptr;
    if (endBodyTok->next() != endToken)
        return nullptr;
    if (!findmatch125(start, bodyTok, varid) )
        return nullptr;
    if (isVariableChanged(start, bodyTok, varid, false, nullptr, true))
        return nullptr;
    return bodyTok;
}

static bool addByOne(const Token *tok, unsigned int varid)
{
    if (match133(tok) &&
        tok->tokAt(1)->hasKnownIntValue() &&
        tok->tokAt(1)->getValue(1)) {
        return true;
    }
    if (match134(tok, varid) &&
        tok->tokAt(3)->hasKnownIntValue() &&
        tok->tokAt(3)->getValue(1)) {
        return true;
    }
    return false;
}

static bool accumulateBoolLiteral(const Token *tok, unsigned int varid)
{
    // TODO: Missing %oreq%
    if (match135(tok) &&
        tok->tokAt(1)->hasKnownIntValue()) {
        return true;
    }
    if (match136(tok, varid) &&
        tok->tokAt(3)->hasKnownIntValue()) {
        return true;
    }
    return false;
}

static bool accumulateBool(const Token *tok, unsigned int varid)
{
    // TODO: Missing %oreq%
    if (match137(tok)) {
        return true;
    }
    if (match138(tok, varid)) {
        return true;
    }
    return false;
}

static bool hasVarIds(const Token *tok, unsigned int var1, unsigned int var2)
{
    if (tok->astOperand1()->varId() == tok->astOperand2()->varId())
        return false;
    if (tok->astOperand1()->varId() == var1 || tok->astOperand1()->varId() == var2) {
        if (tok->astOperand2()->varId() == var1 || tok->astOperand2()->varId() == var2) {
            return true;
        }
    }
    return false;
}

static std::string flipMinMax(const std::string &algo)
{
    if (algo == MatchCompiler::makeConstString("std::max_element"))
        return "std::min_element";
    if (algo == MatchCompiler::makeConstString("std::min_element"))
        return "std::max_element";
    return algo;
}

static std::string minmaxCompare(const Token *condTok, unsigned int loopVar, unsigned int assignVar, bool invert = false)
{
    if (!match139(condTok))
        return "std::accumulate";
    if (!hasVarIds(condTok, loopVar, assignVar))
        return "std::accumulate";
    std::string algo = "std::max_element";
    if (match140(condTok))
        algo = "std::min_element";
    if (condTok->astOperand1()->varId() == assignVar)
        algo = flipMinMax(algo);
    if (invert)
        algo = flipMinMax(algo);
    return algo;
}

void CheckStl::useStlAlgorithm()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;
    for (const Scope *function : mTokenizer->getSymbolDatabase()->functionScopes) {
        for (const Token *tok = function->bodyStart; tok != function->bodyEnd; tok = tok->next()) {
            // Parse range-based for loop
            if (!match33(tok))
                continue;
            if (!match131(tok->next()->link()))
                continue;
            const Token *bodyTok = tok->next()->link()->next();
            const Token *splitTok = tok->next()->astOperand2();
            if (!match141(splitTok))
                continue;
            const Token *loopVar = splitTok->previous();
            if (!match142(loopVar))
                continue;

            // Check for single assignment
            bool useLoopVarInAssign;
            const Token *assignTok = singleAssignInScope(bodyTok, loopVar->varId(), useLoopVarInAssign);
            if (assignTok) {
                unsigned int assignVarId = assignTok->astOperand1()->varId();
                std::string algo;
                if (assignVarId == loopVar->varId()) {
                    if (useLoopVarInAssign)
                        algo = "std::transform";
                    else if (match143(assignTok->next()))
                        algo = "std::fill";
                    else if (match144(assignTok->next()))
                        algo = "std::generate";
                    else
                        algo = "std::fill or std::generate";
                } else {
                    if (addByOne(assignTok, assignVarId))
                        algo = "std::distance";
                    else if (accumulateBool(assignTok, assignVarId))
                        algo = "std::any_of, std::all_of, std::none_of, or std::accumulate";
                    else if (match145(assignTok) && hasVarIds(assignTok->tokAt(6), loopVar->varId(), assignVarId))
                        algo = minmaxCompare(assignTok->tokAt(2), loopVar->varId(), assignVarId, assignTok->tokAt(5)->varId() == assignVarId);
                    else
                        algo = "std::accumulate";
                }
                useStlAlgorithmError(assignTok, algo);
                continue;
            }
            // Check for container calls
            bool useLoopVarInMemCall;
            const Token *memberAccessTok = singleMemberCallInScope(bodyTok, loopVar->varId(), useLoopVarInMemCall);
            if (memberAccessTok) {
                const Token *memberCallTok = memberAccessTok->astOperand2();
                const unsigned int contVarId = memberAccessTok->astOperand1()->varId();
                if (contVarId == loopVar->varId())
                    continue;
                if (memberCallTok->str() == MatchCompiler::makeConstString("push_back") ||
                    memberCallTok->str() == MatchCompiler::makeConstString("push_front") ||
                    memberCallTok->str() == MatchCompiler::makeConstString("emplace_back")) {
                    std::string algo;
                    if (useLoopVarInMemCall)
                        algo = "std::copy";
                    else
                        algo = "std::transform";
                    useStlAlgorithmError(memberCallTok, algo);
                }
                continue;
            }

            // Check for increment in loop
            bool useLoopVarInIncrement;
            const Token *incrementTok = singleIncrementInScope(bodyTok, loopVar->varId(), useLoopVarInIncrement);
            if (incrementTok) {
                std::string algo;
                if (useLoopVarInIncrement)
                    algo = "std::transform";
                else
                    algo = "std::distance";
                useStlAlgorithmError(incrementTok, algo);
                continue;
            }

            // Check for conditionals
            const Token *condBodyTok = singleConditionalInScope(bodyTok, loopVar->varId());
            if (condBodyTok) {
                // Check for single assign
                assignTok = singleAssignInScope(condBodyTok, loopVar->varId(), useLoopVarInAssign);
                if (assignTok) {
                    const unsigned int assignVarId = assignTok->astOperand1()->varId();
                    std::string algo;
                    if (assignVarId == loopVar->varId()) {
                        if (useLoopVarInAssign)
                            algo = "std::transform";
                        else
                            algo = "std::replace_if";
                    } else {
                        if (addByOne(assignTok, assignVarId))
                            algo = "std::count_if";
                        else if (accumulateBoolLiteral(assignTok, assignVarId))
                            algo = "std::any_of, std::all_of, std::none_of, or std::accumulate";
                        else
                            algo = "std::accumulate";
                    }
                    useStlAlgorithmError(assignTok, algo);
                    continue;
                }

                // Check for container call
                memberAccessTok = singleMemberCallInScope(condBodyTok, loopVar->varId(), useLoopVarInMemCall);
                if (memberAccessTok) {
                    const Token *memberCallTok = memberAccessTok->astOperand2();
                    const unsigned int contVarId = memberAccessTok->astOperand1()->varId();
                    if (contVarId == loopVar->varId())
                        continue;
                    if (memberCallTok->str() == MatchCompiler::makeConstString("push_back") ||
                        memberCallTok->str() == MatchCompiler::makeConstString("push_front") ||
                        memberCallTok->str() == MatchCompiler::makeConstString("emplace_back")) {
                        if (useLoopVarInMemCall)
                            useStlAlgorithmError(memberAccessTok, "std::copy_if");
                        // There is no transform_if to suggest
                    }
                    continue;
                }

                // Check for increment in loop
                incrementTok = singleIncrementInScope(condBodyTok, loopVar->varId(), useLoopVarInIncrement);
                if (incrementTok) {
                    std::string algo;
                    if (useLoopVarInIncrement)
                        algo = "std::transform";
                    else
                        algo = "std::count_if";
                    useStlAlgorithmError(incrementTok, algo);
                    continue;
                }

                // Check early return
                if (isEarlyExit(condBodyTok)) {
                    const Token *loopVar2 = findmatch125(condBodyTok, condBodyTok->link(), loopVar->varId()) ;
                    std::string algo;
                    if (loopVar2)
                        algo = "std::find_if";
                    else
                        algo = "std::any_of";
                    useStlAlgorithmError(condBodyTok, algo);
                    continue;
                }
            }
        }
    }
}
