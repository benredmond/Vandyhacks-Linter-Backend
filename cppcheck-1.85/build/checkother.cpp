#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %var%|EOF %comp%|=
static bool match1(const Token* tok) {
    if (!tok || !((tok->varId() != 0) || (tok->str()==MatchCompiler::makeConstString("EOF"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isComparisonOp() || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: %var% = fclose|fflush|fputc|fputs|fscanf|getchar|getc|fgetc|putchar|putc|puts|scanf|sscanf|ungetc (
static bool match2(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("fclose")) || (tok->str()==MatchCompiler::makeConstString("fflush")) || (tok->str()==MatchCompiler::makeConstString("fputc")) || (tok->str()==MatchCompiler::makeConstString("fputs")) || (tok->str()==MatchCompiler::makeConstString("fscanf")) || (tok->str()==MatchCompiler::makeConstString("getchar")) || (tok->str()==MatchCompiler::makeConstString("getc")) || (tok->str()==MatchCompiler::makeConstString("fgetc")) || (tok->str()==MatchCompiler::makeConstString("putchar")) || (tok->str()==MatchCompiler::makeConstString("putc")) || (tok->str()==MatchCompiler::makeConstString("puts")) || (tok->str()==MatchCompiler::makeConstString("scanf")) || (tok->str()==MatchCompiler::makeConstString("sscanf")) || (tok->str()==MatchCompiler::makeConstString("ungetc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: EOF %comp% ( %var% = fclose|fflush|fputc|fputs|fscanf|getchar|getc|fgetc|putchar|putc|puts|scanf|sscanf|ungetc (
static bool match3(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("EOF")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("fclose")) || (tok->str()==MatchCompiler::makeConstString("fflush")) || (tok->str()==MatchCompiler::makeConstString("fputc")) || (tok->str()==MatchCompiler::makeConstString("fputs")) || (tok->str()==MatchCompiler::makeConstString("fscanf")) || (tok->str()==MatchCompiler::makeConstString("getchar")) || (tok->str()==MatchCompiler::makeConstString("getc")) || (tok->str()==MatchCompiler::makeConstString("fgetc")) || (tok->str()==MatchCompiler::makeConstString("putchar")) || (tok->str()==MatchCompiler::makeConstString("putc")) || (tok->str()==MatchCompiler::makeConstString("puts")) || (tok->str()==MatchCompiler::makeConstString("scanf")) || (tok->str()==MatchCompiler::makeConstString("sscanf")) || (tok->str()==MatchCompiler::makeConstString("ungetc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: EOF %comp% ( %var% = std :: cin . get (
static bool match4(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("EOF")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("cin")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("get")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: EOF %comp% ( %var% = cin . get (
static bool match5(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("EOF")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("cin")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("get")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% = std :: cin . get (
static bool match6(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("cin")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("get")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% = cin . get (
static bool match7(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("cin")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("get")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% %comp% EOF
static bool match8(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("EOF")))
        return false;
    return true;
}
// pattern: EOF %comp% %var%
static bool match9(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("EOF")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: * %name%
static bool match10(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [{};]
static bool match11(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    return true;
}
// pattern: ++|-- [;,]
static bool match12(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";,", tok->str()[0]))
        return false;
    return true;
}
// pattern: { ; } {
static bool match13(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ( const|volatile| const|volatile| %type% * const| ) (| %name%|%num%|%bool%|%char%|%str%
static bool match14(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !(tok->isName() || tok->isNumber() || tok->isBoolean() || (tok->tokType()==Token::eChar) || (tok->tokType()==Token::eString)))
        return false;
    return true;
}
// pattern: const|volatile
static bool match15(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        return false;
    return true;
}
// pattern: ( const|volatile| const|volatile| %type% %type%| const| * )
static bool match16(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->isName() && tok->varId()==0U && !tok->isKeyword())))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: reinterpret_cast <
static bool match17(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("reinterpret_cast")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: pipe ( %var% )
static bool match18(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("pipe")))
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
// pattern: pipe2 ( %var% ,
static bool match19(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("pipe2")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: } catch (
static bool match20(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("catch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %varid%
template<class T> static T * findmatch21(T * start_tok, const Token * end, unsigned int varid) {
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
// pattern: ;|}
template<class T> static T * findmatch22(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: for|if|while (
static bool match23(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: break|return|continue|throw|goto|asm
static bool match24(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("goto")) || (tok->str()==MatchCompiler::makeConstString("asm"))))
        return false;
    return true;
}
// pattern: %var% = [ & ] (
static bool match25(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) {
static bool match26(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: %name% (
static bool match27(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [([]
static bool match28(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("([", tok->str()[0]))
        return false;
    return true;
}
// pattern: [)];,]
static bool match29(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(")];,", tok->str()[0]))
        return false;
    return true;
}
// pattern: %var% [
static bool match30(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: %name%|::|.
static bool match31(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: %name% . %var%
static bool match32(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: [;{}]
static bool match33(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: ++|--|=
static bool match34(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: %name%|.|[|*|(
static bool match35(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: ++|--
static bool match36(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: default|case
template<class T> static T * findmatch37(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("default")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: 0 ;
static bool match38(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: sizeof (
static bool match39(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: memcpy|memmove|memset|strcpy|strncpy|sprintf|snprintf|strcat|strncat|wcscpy|wcsncpy|swprintf|wcscat|wcsncat
static bool match40(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("memcpy")) || (tok->str()==MatchCompiler::makeConstString("memmove")) || (tok->str()==MatchCompiler::makeConstString("memset")) || (tok->str()==MatchCompiler::makeConstString("strcpy")) || (tok->str()==MatchCompiler::makeConstString("strncpy")) || (tok->str()==MatchCompiler::makeConstString("sprintf")) || (tok->str()==MatchCompiler::makeConstString("snprintf")) || (tok->str()==MatchCompiler::makeConstString("strcat")) || (tok->str()==MatchCompiler::makeConstString("strncat")) || (tok->str()==MatchCompiler::makeConstString("wcscpy")) || (tok->str()==MatchCompiler::makeConstString("wcsncpy")) || (tok->str()==MatchCompiler::makeConstString("swprintf")) || (tok->str()==MatchCompiler::makeConstString("wcscat")) || (tok->str()==MatchCompiler::makeConstString("wcsncat"))))
        return false;
    return true;
}
// pattern: strcat|strncat|wcscat|wcsncat
static bool match41(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("strcat")) || (tok->str()==MatchCompiler::makeConstString("strncat")) || (tok->str()==MatchCompiler::makeConstString("wcscat")) || (tok->str()==MatchCompiler::makeConstString("wcsncat"))))
        return false;
    return true;
}
// pattern: break|continue|return|exit|goto|throw
static bool match42(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("exit")) || (tok->str()==MatchCompiler::makeConstString("goto")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: )|else {
static bool match43(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("else"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ;|{|}|: %var% = %any% ;
static bool match44(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ;|{|}|: %var% %assign% %num% ;
static bool match45(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %num%
static bool match46(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: ;|{|}|: %var% = %name% %or%|& %num% ;
static bool match47(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(":"))))
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
    if (!tok || !((tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") ) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;}{]
static bool match48(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";}{", tok->str()[0]))
        return false;
    return true;
}
// pattern: &&|%oror%
static bool match49(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    return true;
}
// pattern: for (
static bool match50(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% ==
static bool match51(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    return true;
}
// pattern: [;{}] *| %name% == %any% ;
static bool match52(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: } )
static bool match53(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: (|[|<
static bool match54(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: break|continue ;
static bool match55(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}:] return|throw
static bool match56(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}:", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: goto %any% ;
static bool match57(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("goto")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ?|:
static bool match58(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: continue|goto|throw
static bool match59(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("goto")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: [}:]
template<class T> static T * findmatch60(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || tok->str().size()!=1U || !strchr("}:", tok->str()[0]))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: return|}|case|default
static bool match61(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("default"))))
        return false;
    return true;
}
// pattern: while|do|for
static bool match62(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("do")) || (tok->str()==MatchCompiler::makeConstString("for"))))
        return false;
    return true;
}
// pattern: {
template<class T> static T * findmatch63(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: [;{}] %any% :
static bool match64(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: ( void ) %name% ;
static bool match65(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("void")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ; %varid% = %any% ;
static bool match66(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
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
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [(=]
static bool match67(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(=", tok->str()[0]))
        return false;
    return true;
}
// pattern: (
static bool match68(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: else { if (
static bool match69(const Token* tok) {
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
// pattern: } }
static bool match70(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: } else {
static bool match71(const Token* tok) {
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
// pattern: %varid% =
static bool match72(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %varid% !!=
static bool match73(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("="))
        return false;
    return true;
}
// pattern: & %varid%
static bool match74(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: * %varid%
static bool match75(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: = %varid%
static bool match76(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: [([{<]
static bool match77(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("([{<", tok->str()[0]))
        return false;
    return true;
}
// pattern: move (
static bool match78(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("move")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: )|]|}|>
static bool match79(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: %var% . %name% (
static bool match80(const Token* tok) {
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
// pattern: [&|^]
static bool match81(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("&|^", tok->str()[0]))
        return false;
    return true;
}
// pattern: =
static bool match82(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: (|[
static bool match83(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %name%|] {
static bool match84(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("]"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ;
template<class T> static T * findmatch85(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: [;{}] %str%|%num%
static bool match86(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType()==Token::eString) || tok->isNumber()))
        return false;
    return true;
}
// pattern: %num% [,.]
static bool match87(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",.", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}] 0 ;
static bool match88(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ; } )
static bool match89(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: inf.0 +|-
static bool match90(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("inf.0")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: +|- inf.0
static bool match91(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("inf.0")))
        return false;
    return true;
}
// pattern: +|- %num% / 0.0
static bool match92(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("/")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0.0")))
        return false;
    return true;
}
// pattern: [;{}] %name% (
static bool match93(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ; !!}
static bool match94(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("}"))
        return false;
    return true;
}
// pattern: %var% = malloc|g_malloc|new
static bool match95(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("malloc")) || (tok->str()==MatchCompiler::makeConstString("g_malloc")) || (tok->str()==MatchCompiler::makeConstString("new"))))
        return false;
    return true;
}
// pattern: %var% = %name% +|-
static bool match96(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: %var% =
static bool match97(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: free|g_free|delete ( %any% +|-
static bool match98(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("free")) || (tok->str()==MatchCompiler::makeConstString("g_free")) || (tok->str()==MatchCompiler::makeConstString("delete"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: delete [ ] ( %any% +|-
static bool match99(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: delete %any% +|- %any%
static bool match100(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    return true;
}
// pattern: %var%
template<class T> static T * findmatch101(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->varId() != 0))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %var%
static bool match102(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: ;
template<class T> static T * findmatch103(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; %type% %var% ;
static bool match104(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
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
// pattern: %var% %assign%
static bool match105(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: ;|{|} %var%
static bool match106(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: %varid% = %var%
static bool match107(const Token* tok, const unsigned int varid) {
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
    return true;
}
// pattern: %var% = %varid%
static bool match108(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: +|*|<<|>>|+=|*=|<<=|>>=
static bool match109(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("<<")) || (tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString("+=")) || (tok->str()==MatchCompiler::makeConstString("*=")) || (tok->str()==MatchCompiler::makeConstString("<<=")) || (tok->str()==MatchCompiler::makeConstString(">>="))))
        return false;
    return true;
}
// pattern: ==|!=|-
static bool match110(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: =|-|-=|/|/=
static bool match111(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("-")) || (tok->str()==MatchCompiler::makeConstString("-=")) || (tok->str()==MatchCompiler::makeConstString("/")) || (tok->str()==MatchCompiler::makeConstString("/="))))
        return false;
    return true;
}
// pattern: [-/%]
static bool match112(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("-/%", tok->str()[0]))
        return false;
    return true;
}
// pattern: ==|>=|<=
static bool match113(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("<="))))
        return false;
    return true;
}
// pattern: !=|>|<
static bool match114(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("!=")) || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: %num%|NULL|nullptr
static bool match115(const Token* tok) {
    if (!tok || !(tok->isNumber() || (tok->str()==MatchCompiler::makeConstString("NULL")) || (tok->str()==MatchCompiler::makeConstString("nullptr"))))
        return false;
    return true;
}
// pattern: isgreater|isless|islessgreater|isgreaterequal|islessequal ( %var% , %var% )
static bool match116(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("isgreater")) || (tok->str()==MatchCompiler::makeConstString("isless")) || (tok->str()==MatchCompiler::makeConstString("islessgreater")) || (tok->str()==MatchCompiler::makeConstString("isgreaterequal")) || (tok->str()==MatchCompiler::makeConstString("islessequal"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: <|<= 0
static bool match117(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("<="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    return true;
}
// pattern: 0 >|>=
static bool match118(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">="))))
        return false;
    return true;
}
// pattern: >= 0
static bool match119(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    return true;
}
// pattern: 0 <=
static bool match120(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<=")))
        return false;
    return true;
}
// pattern: ) )| ;
static bool match121(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString(")"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: <<|>>|<<=|>>=
static bool match122(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<<")) || (tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString("<<=")) || (tok->str()==MatchCompiler::makeConstString(">>="))))
        return false;
    return true;
}
// pattern: memset|memcpy|memmove ( %var% ,
static bool match123(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("memset")) || (tok->str()==MatchCompiler::makeConstString("memcpy")) || (tok->str()==MatchCompiler::makeConstString("memmove"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: , %num% )
static bool match124(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [(,] NULL [,)]
static bool match125(const Token* tok) {
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
// pattern: . . .
static bool match126(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: InterlockedDecrement ( & %name% ) ; if ( %name%|!|0
static bool match127(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("InterlockedDecrement")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("!")) || (tok->str()==MatchCompiler::makeConstString("0"))))
        return false;
    return true;
}
// pattern: 0 %comp% %name% )
static bool match128(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ! %name% )
static bool match129(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %name% )
static bool match130(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %name% %comp% 0 )
static bool match131(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: if ( ::| InterlockedDecrement ( & %name%
static bool match132(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("InterlockedDecrement")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: } return %name%
static bool match133(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: } else { return %name%
static bool match134(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: {|}|; %name% :
static bool match135(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: %oror%|&&|?|:|;
static bool match136(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: ,
static bool match137(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: sizeof
static bool match138(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    return true;
}
// pattern: std :: move (
static bool match139(const Token* tok) {
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
    return true;
}
// pattern: > (
static bool match140(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: std :: forward <
static bool match141(const Token* tok) {
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
// pattern: ,|)|;
static bool match142(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(";"))))
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
#include "checkother.h"

#include "astutils.h"
#include "errorlogger.h"
#include "library.h"
#include "mathlib.h"
#include "settings.h"
#include "standards.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "utils.h"

#include <algorithm> // find_if()
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <stack>
#include <utility>
//---------------------------------------------------------------------------

// Register this check class (by creating a static instance of it)
namespace {
    CheckOther instance;
}

static const struct CWE CWE128(128U);   // Wrap-around Error
static const struct CWE CWE131(131U);   // Incorrect Calculation of Buffer Size
static const struct CWE CWE197(197U);   // Numeric Truncation Error
static const struct CWE CWE362(362U);   // Concurrent Execution using Shared Resource with Improper Synchronization ('Race Condition')
static const struct CWE CWE369(369U);   // Divide By Zero
static const struct CWE CWE398(398U);   // Indicator of Poor Code Quality
static const struct CWE CWE475(475U);   // Undefined Behavior for Input to API
static const struct CWE CWE482(482U);   // Comparing instead of Assigning
static const struct CWE CWE561(561U);   // Dead Code
static const struct CWE CWE563(563U);   // Assignment to Variable without Use ('Unused Variable')
static const struct CWE CWE570(570U);   // Expression is Always False
static const struct CWE CWE571(571U);   // Expression is Always True
static const struct CWE CWE672(672U);   // Operation on a Resource after Expiration or Release
static const struct CWE CWE628(628U);   // Function Call with Incorrectly Specified Arguments
static const struct CWE CWE683(683U);   // Function Call With Incorrect Order of Arguments
static const struct CWE CWE686(686U);   // Function Call With Incorrect Argument Type
static const struct CWE CWE704(704U);   // Incorrect Type Conversion or Cast
static const struct CWE CWE758(758U);   // Reliance on Undefined, Unspecified, or Implementation-Defined Behavior
static const struct CWE CWE768(768U);   // Incorrect Short Circuit Evaluation
static const struct CWE CWE783(783U);   // Operator Precedence Logic Error

//----------------------------------------------------------------------------------
// The return value of fgetc(), getc(), ungetc(), getchar() etc. is an integer value.
// If this return value is stored in a character variable and then compared
// to EOF, which is an integer, the comparison maybe be false.
//
// Reference:
// - Ticket #160
// - http://www.cplusplus.com/reference/cstdio/fgetc/
// - http://www.cplusplus.com/reference/cstdio/getc/
// - http://www.cplusplus.com/reference/cstdio/getchar/
// - http://www.cplusplus.com/reference/cstdio/ungetc/ ...
//----------------------------------------------------------------------------------
void CheckOther::checkCastIntToCharAndBack()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        std::map<unsigned int, std::string> vars;
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            // Quick check to see if any of the matches below have any chances
            if (!match1(tok))
                continue;
            if (match2(tok)) {
                const Variable *var = tok->variable();
                if (var && var->typeEndToken()->str() == MatchCompiler::makeConstString("char") && !var->typeEndToken()->isSigned()) {
                    vars[tok->varId()] = tok->strAt(2);
                }
            } else if (match3(tok)) {
                tok = tok->tokAt(3);
                const Variable *var = tok->variable();
                if (var && var->typeEndToken()->str() == MatchCompiler::makeConstString("char") && !var->typeEndToken()->isSigned()) {
                    checkCastIntToCharAndBackError(tok, tok->strAt(2));
                }
            } else if (mTokenizer->isCPP() && (match4(tok) || match5(tok))) {
                tok = tok->tokAt(3);
                const Variable *var = tok->variable();
                if (var && var->typeEndToken()->str() == MatchCompiler::makeConstString("char") && !var->typeEndToken()->isSigned()) {
                    checkCastIntToCharAndBackError(tok, "cin.get");
                }
            } else if (mTokenizer->isCPP() && (match6(tok) || match7(tok))) {
                const Variable *var = tok->variable();
                if (var && var->typeEndToken()->str() == MatchCompiler::makeConstString("char") && !var->typeEndToken()->isSigned()) {
                    vars[tok->varId()] = "cin.get";
                }
            } else if (match8(tok)) {
                if (vars.find(tok->varId()) != vars.end()) {
                    checkCastIntToCharAndBackError(tok, vars[tok->varId()]);
                }
            } else if (match9(tok)) {
                tok = tok->tokAt(2);
                if (vars.find(tok->varId()) != vars.end()) {
                    checkCastIntToCharAndBackError(tok, vars[tok->varId()]);
                }
            }
        }
    }
}

void CheckOther::checkCastIntToCharAndBackError(const Token *tok, const std::string &strFunctionName)
{
    reportError(
        tok,
        Severity::warning,
        "checkCastIntToCharAndBack",
        "$symbol:" + strFunctionName + "\n"
        "Storing $symbol() return value in char variable and then comparing with EOF.\n"
        "When saving $symbol() return value in char variable there is loss of precision. "
        " When $symbol() returns EOF this value is truncated. Comparing the char "
        "variable with EOF can have unexpected results. For instance a loop \"while (EOF != (c = $symbol());\" "
        "loops forever on some compilers/platforms and on other compilers/platforms it will stop "
        "when the file contains a matching character.", CWE197, false
    );
}


//---------------------------------------------------------------------------
// Clarify calculation precedence for ternary operators.
//---------------------------------------------------------------------------
void CheckOther::clarifyCalculation()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            // ? operator where lhs is arithmetical expression
            if (tok->str() != MatchCompiler::makeConstString("?") || !tok->astOperand1() || !tok->astOperand1()->isCalculation())
                continue;
            if (!tok->astOperand1()->isArithmeticalOp() && tok->astOperand1()->tokType() != Token::eBitOp)
                continue;

            // Is code clarified by parentheses already?
            const Token *tok2 = tok->astOperand1();
            for (; tok2; tok2 = tok2->next()) {
                if (tok2->str() == MatchCompiler::makeConstString("("))
                    tok2 = tok2->link();
                else if (tok2->str() == MatchCompiler::makeConstString(")"))
                    break;
                else if (tok2->str() == MatchCompiler::makeConstString("?")) {
                    clarifyCalculationError(tok, tok->astOperand1()->str());
                    break;
                }
            }
        }
    }
}

void CheckOther::clarifyCalculationError(const Token *tok, const std::string &op)
{
    // suspicious calculation
    const std::string calc("'a" + op + "b?c:d'");

    // recommended calculation #1
    const std::string s1("'(a" + op + "b)?c:d'");

    // recommended calculation #2
    const std::string s2("'a" + op + "(b?c:d)'");

    reportError(tok,
                Severity::style,
                "clarifyCalculation",
                "Clarify calculation precedence for '" + op + "' and '?'.\n"
                "Suspicious calculation. Please use parentheses to clarify the code. "
                "The code '" + calc + "' should be written as either '" + s1 + "' or '" + s2 + "'.", CWE783, false);
}

//---------------------------------------------------------------------------
// Clarify (meaningless) statements like *foo++; with parentheses.
//---------------------------------------------------------------------------
void CheckOther::clarifyStatement()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok && tok != scope->bodyEnd; tok = tok->next()) {
            if (match10(tok) && tok->astOperand1()) {
                const Token *tok2 = tok->previous();

                while (tok2 && tok2->str() == MatchCompiler::makeConstString("*"))
                    tok2 = tok2->previous();

                if (tok2 && !tok2->astParent() && match11(tok2)) {
                    tok2 = tok->astOperand1();
                    if (match12(tok2))
                        clarifyStatementError(tok2);
                }
            }
        }
    }
}

void CheckOther::clarifyStatementError(const Token *tok)
{
    reportError(tok, Severity::warning, "clarifyStatement", "Ineffective statement similar to '*A++;'. Did you intend to write '(*A)++;'?\n"
                "A statement like '*A++;' might not do what you intended. Postfix 'operator++' is executed before 'operator*'. "
                "Thus, the dereference is meaningless. Did you intend to write '(*A)++;'?", CWE783, false);
}

//---------------------------------------------------------------------------
// Check for suspicious occurrences of 'if(); {}'.
//---------------------------------------------------------------------------
void CheckOther::checkSuspiciousSemicolon()
{
    if (!mSettings->inconclusive || !mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    // Look for "if(); {}", "for(); {}" or "while(); {}"
    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type == Scope::eIf || scope.type == Scope::eElse || scope.type == Scope::eWhile || scope.type == Scope::eFor) {
            // Ensure the semicolon is at the same line number as the if/for/while statement
            // and the {..} block follows it without an extra empty line.
            if (match13(scope.bodyStart) &&
                scope.bodyStart->previous()->linenr() == scope.bodyStart->tokAt(2)->linenr()
                && scope.bodyStart->linenr()+1 >= scope.bodyStart->tokAt(3)->linenr()) {
                SuspiciousSemicolonError(scope.classDef);
            }
        }
    }
}

void CheckOther::SuspiciousSemicolonError(const Token* tok)
{
    reportError(tok, Severity::warning, "suspiciousSemicolon",
                "Suspicious use of ; at the end of '" + (tok ? tok->str() : std::string()) + "' statement.", CWE398, true);
}


//---------------------------------------------------------------------------
// For C++ code, warn if C-style casts are used on pointer types
//---------------------------------------------------------------------------
void CheckOther::warningOldStylePointerCast()
{
    // Only valid on C++ code
    if (!mSettings->isEnabled(Settings::STYLE) || !mTokenizer->isCPP())
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        const Token* tok;
        if (scope->function && scope->function->isConstructor())
            tok = scope->classDef;
        else
            tok = scope->bodyStart;
        for (; tok && tok != scope->bodyEnd; tok = tok->next()) {
            // Old style pointer casting..
            if (!match14(tok))
                continue;

            // skip first "const" in "const Type* const"
            while (match15(tok->next()))
                tok = tok->next();
            const Token* typeTok = tok->next();
            // skip second "const" in "const Type* const"
            if (tok->strAt(3) == MatchCompiler::makeConstString("const"))
                tok = tok->next();

            const Token *p = tok->tokAt(4);
            if (p->hasKnownIntValue() && p->values().front().intvalue==0) // Casting nullpointers is safe
                continue;

            // Is "type" a class?
            if (typeTok->type())
                cstyleCastError(tok);
        }
    }
}

void CheckOther::cstyleCastError(const Token *tok)
{
    reportError(tok, Severity::style, "cstyleCast",
                "C-style pointer casting\n"
                "C-style pointer casting detected. C++ offers four different kinds of casts as replacements: "
                "static_cast, const_cast, dynamic_cast and reinterpret_cast. A C-style cast could evaluate to "
                "any of those automatically, thus it is considered safer if the programmer explicitly states "
                "which kind of cast is expected. See also: https://www.securecoding.cert.org/confluence/display/cplusplus/EXP05-CPP.+Do+not+use+C-style+casts.", CWE398, false);
}

//---------------------------------------------------------------------------
// float* f; double* d = (double*)f; <-- Pointer cast to a type with an incompatible binary data representation
//---------------------------------------------------------------------------

void CheckOther::invalidPointerCast()
{
    if (!mSettings->isEnabled(Settings::PORTABILITY))
        return;

    const bool printInconclusive = mSettings->inconclusive;
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            const Token* toTok = nullptr;
            const Token* fromTok = nullptr;
            // Find cast
            if (match16(tok)) {
                toTok = tok;
                fromTok = tok->astOperand1();
            } else if (match17(tok) && tok->linkAt(1)) {
                toTok = tok->linkAt(1)->next();
                fromTok = toTok->astOperand2();
            }
            if (!fromTok)
                continue;

            const ValueType* fromType = fromTok->valueType();
            const ValueType* toType = toTok->valueType();
            if (!fromType || !toType || !fromType->pointer || !toType->pointer)
                continue;

            if (fromType->type != toType->type && fromType->type >= ValueType::Type::BOOL && toType->type >= ValueType::Type::BOOL && (toType->type != ValueType::Type::CHAR || printInconclusive)) {
                if (toType->isIntegral() && fromType->isIntegral())
                    continue;
                std::string toStr = toType->isIntegral() ? "integer *" : toType->str();
                toStr.erase(toStr.size()-2);
                std::string fromStr = fromType->isIntegral() ? "integer *" : fromType->str();
                fromStr.erase(fromStr.size() - 2);

                invalidPointerCastError(tok, fromStr, toStr, toType->type == ValueType::Type::CHAR);
            }
        }
    }
}

void CheckOther::invalidPointerCastError(const Token* tok, const std::string& from, const std::string& to, bool inconclusive)
{
    if (to == MatchCompiler::makeConstString("integer")) { // If we cast something to int*, this can be useful to play with its binary data representation
        if (!inconclusive)
            reportError(tok, Severity::portability, "invalidPointerCast", "Casting from " + from + "* to integer* is not portable due to different binary data representations on different platforms.", CWE704, false);
        else
            reportError(tok, Severity::portability, "invalidPointerCast", "Casting from " + from + "* to char* is not portable due to different binary data representations on different platforms.", CWE704, true);
    } else
        reportError(tok, Severity::portability, "invalidPointerCast", "Casting between " + from + "* and " + to + "* which have an incompatible binary data representation.", CWE704, false);
}

//---------------------------------------------------------------------------
// This check detects errors on POSIX systems, when a pipe command called
// with a wrong dimensioned file descriptor array. The pipe command requires
// exactly an integer array of dimension two as parameter.
//
// References:
//  - http://linux.die.net/man/2/pipe
//  - ticket #3521
//---------------------------------------------------------------------------
void CheckOther::checkPipeParameterSize()
{
    if (!mSettings->standards.posix)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match18(tok) ||
                match19(tok)) {
                const Token * const varTok = tok->tokAt(2);

                const Variable *var = varTok->variable();
                MathLib::bigint dim;
                if (var && var->isArray() && !var->isArgument() && ((dim=var->dimension(0U)) < 2)) {
                    const std::string strDim = MathLib::toString(dim);
                    checkPipeParameterSizeError(varTok,varTok->str(), strDim);
                }
            }
        }
    }
}

void CheckOther::checkPipeParameterSizeError(const Token *tok, const std::string &strVarName, const std::string &strDim)
{
    reportError(tok, Severity::error,
                "wrongPipeParameterSize",
                "$symbol:" + strVarName + "\n"
                "Buffer '$symbol' must have size of 2 integers if used as parameter of pipe().\n"
                "The pipe()/pipe2() system command takes an argument, which is an array of exactly two integers.\n"
                "The variable '$symbol' is an array of size " + strDim + ", which does not match.", CWE686, false);
}

//---------------------------------------------------------------------------
// Detect redundant assignments: x = 0; x = 4;
//---------------------------------------------------------------------------

static bool nonLocal(const Variable* var)
{
    return !var || (!var->isLocal() && !var->isArgument()) || var->isStatic() || var->isReference();
}

static bool nonLocalVolatile(const Variable* var)
{
    if (var && var->isVolatile())
        return false;
    return nonLocal(var);
}

static void eraseNotLocalArg(std::map<unsigned int, const Token*>& container, const SymbolDatabase* symbolDatabase)
{
    for (std::map<unsigned int, const Token*>::iterator i = container.begin(); i != container.end();) {
        const Variable* var = symbolDatabase->getVariableFromVarId(i->first);
        if (!var || nonLocal(var)) {
            container.erase(i++);
        } else
            ++i;
    }
}

static void eraseMemberAssignments(const unsigned int varId, const std::map<unsigned int, std::set<unsigned int> > &membervars, std::map<unsigned int, const Token*> &varAssignments)
{
    const std::map<unsigned int, std::set<unsigned int> >::const_iterator it = membervars.find(varId);
    if (it != membervars.end()) {
        const std::set<unsigned int>& vars = it->second;
        for (unsigned int var : vars) {
            varAssignments.erase(var);
            if (var != varId)
                eraseMemberAssignments(var, membervars, varAssignments);
        }
    }
}

static bool checkExceptionHandling(const Token* tok)
{
    const Variable* var = tok->variable();
    const Scope* upperScope = tok->scope();
    if (var && upperScope == var->scope())
        return true;
    while (upperScope && upperScope->type != Scope::eTry && upperScope->type != Scope::eLambda && (!var || upperScope->nestedIn != var->scope()) && upperScope->isExecutable()) {
        upperScope = upperScope->nestedIn;
    }
    if (var && upperScope && upperScope->type == Scope::eTry) {
        // Check all exception han
        const Token* tok2 = upperScope->bodyEnd;
        while (match20(tok2)) {
            tok2 = tok2->linkAt(2)->next();
            if (findmatch21(tok2, tok2->link(), var->declarationId()) )
                return false;
            tok2 = tok2->link();
        }
    }
    return true;
}

void CheckOther::checkRedundantAssignment()
{
    const bool printPerformance = mSettings->isEnabled(Settings::PERFORMANCE);
    const bool printStyle = mSettings->isEnabled(Settings::STYLE);
    const bool printWarning = mSettings->isEnabled(Settings::WARNING);
    if (!printWarning && !printPerformance && !printStyle)
        return;

    const bool printInconclusive = mSettings->inconclusive;
    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if (!scope.isExecutable())
            continue;

        std::map<unsigned int, std::set<unsigned int>> usedByLambda; // map key: lambda function varId. set of varIds used by lambda.
        std::map<unsigned int, const Token*> varAssignments;
        std::map<unsigned int, const Token*> memAssignments;
        std::map<unsigned int, std::set<unsigned int> > membervars;
        std::set<unsigned int> initialized;
        const Token* writtenArgumentsEnd = nullptr;

        for (const Token* tok = scope.bodyStart->next(); tok && tok != scope.bodyEnd; tok = tok->next()) {
            if (tok == writtenArgumentsEnd)
                writtenArgumentsEnd = nullptr;

            if (tok->str() == MatchCompiler::makeConstString("?") && tok->astOperand2()) {
                tok = findmatch22(tok->astOperand2()) ;
                if (!tok)
                    break;
                varAssignments.clear();
                memAssignments.clear();
            } else if (tok->str() == MatchCompiler::makeConstString("{") && tok->strAt(-1) != MatchCompiler::makeConstString("{") && tok->strAt(-1) != MatchCompiler::makeConstString("=") && tok->strAt(-4) != MatchCompiler::makeConstString("case") && tok->strAt(-3) != MatchCompiler::makeConstString("default")) { // conditional or non-executable inner scope: Skip it and reset status
                tok = tok->link();
                varAssignments.clear();
                memAssignments.clear();
            } else if (match23(tok)) {
                tok = tok->linkAt(1);
            } else if (match24(tok)) {
                varAssignments.clear();
                memAssignments.clear();
            } else if (match25(tok)) {
                const unsigned int lambdaId = tok->varId();
                const Token *lambdaParams = tok->tokAt(5);
                if (match26(lambdaParams->link())) {
                    const Token *lambdaBodyStart = lambdaParams->link()->next();
                    const Token * const lambdaBodyEnd = lambdaBodyStart->link();
                    for (const Token *tok2 = lambdaBodyStart; tok2 != lambdaBodyEnd; tok2 = tok2->next()) {
                        if (tok2->varId())
                            usedByLambda[lambdaId].insert(tok2->varId());
                    }
                }
            } else if (tok->tokType() == Token::eVariable && !match27(tok)) {
                const Token *eq = nullptr;
                for (const Token *tok2 = tok; tok2; tok2 = tok2->next()) {
                    if (match28(tok2)) {
                        // bail out if there is a variable in rhs - we only track 1 variable
                        bool bailout = false;
                        for (const Token *tok3 = tok2->link(); tok3 != tok2; tok3 = tok3->previous()) {
                            if (tok3->varId()) {
                                const Variable *var = tok3->variable();
                                if (!var || !var->isConst() || var->isReference() || var->isPointer()) {
                                    bailout = true;
                                    break;
                                }
                            }
                        }
                        if (bailout)
                            break;
                        tok2 = tok2->link();
                    } else if (match29(tok2))
                        break;
                    else if (tok2->str() == MatchCompiler::makeConstString("=")) {
                        eq = tok2;
                        break;
                    }
                }

                // Set initialization flag
                if (!match30(tok))
                    initialized.insert(tok->varId());
                else {
                    const Token *tok2 = tok->next();
                    while (tok2 && tok2->str() == MatchCompiler::makeConstString("["))
                        tok2 = tok2->link()->next();
                    if (tok2 && tok2->str() != MatchCompiler::makeConstString(";"))
                        initialized.insert(tok->varId());
                }

                const Token *startToken = tok;
                while (match31(startToken)) {
                    startToken = startToken->previous();
                    if (match32(startToken))
                        membervars[startToken->varId()].insert(startToken->tokAt(2)->varId());
                }

                const std::map<unsigned int, const Token*>::iterator it = varAssignments.find(tok->varId());
                if (eq && match33(startToken)) { // Assignment
                    if (it != varAssignments.end()) {
                        const Token *oldeq = nullptr;
                        for (const Token *tok2 = it->second; tok2; tok2 = tok2->next()) {
                            if (match28(tok2))
                                tok2 = tok2->link();
                            else if (match29(tok2))
                                break;
                            else if (match34(tok2)) {
                                oldeq = tok2;
                                break;
                            }
                        }
                        if (!oldeq) {
                            const Token *tok2 = it->second;
                            while (match35(tok2))
                                tok2 = tok2->astParent();
                            if (match36(tok2))
                                oldeq = tok2;
                        }

                        // Ensure that LHS in assignments are the same
                        bool error = oldeq && eq->astOperand1() && isSameExpression(mTokenizer->isCPP(), true, eq->astOperand1(), oldeq->astOperand1(), mSettings->library, true, false);

                        // Ensure that variable is not used on right side
                        std::stack<const Token *> tokens;
                        tokens.push(eq->astOperand2());
                        while (!tokens.empty()) {
                            const Token *rhs = tokens.top();
                            tokens.pop();
                            if (!rhs)
                                continue;
                            tokens.push(rhs->astOperand1());
                            tokens.push(rhs->astOperand2());
                            if (rhs->varId() == tok->varId()) {
                                error = false;
                                break;
                            }
                            if (match27(rhs->previous()) && nonLocalVolatile(tok->variable())) { // Called function might use the variable
                                const Function* const func = rhs->function();
                                const Variable* const var = tok->variable();
                                if (!var || var->isGlobal() || var->isReference() || ((!func || func->nestedIn) && rhs->strAt(-1) != MatchCompiler::makeConstString("."))) {// Global variable, or member function
                                    error = false;
                                    break;
                                }
                            }
                        }

                        if (error) {
                            if (printWarning && scope.type == Scope::eSwitch && findmatch37(it->second, tok) )
                                redundantAssignmentInSwitchError(it->second, tok, eq->astOperand1()->expressionString());
                            else if (printStyle) {
                                // c++, unknown type => assignment might have additional side effects
                                const bool possibleSideEffects(mTokenizer->isCPP() && !tok->valueType());

                                // TODO nonlocal variables are not tracked entirely.
                                const bool nonlocal = it->second->variable() && nonLocalVolatile(it->second->variable());

                                // Warnings are inconclusive if there are possible side effects or if variable is not
                                // tracked perfectly.
                                const bool inconclusive = possibleSideEffects | nonlocal;

                                if (printInconclusive || !inconclusive)
                                    if (mTokenizer->isC() || checkExceptionHandling(tok)) // see #6555 to see how exception handling might have an impact
                                        redundantAssignmentError(it->second, tok, eq->astOperand1()->expressionString(), inconclusive);
                            }
                        }
                        it->second = tok;
                    }
                    if (!match38(tok->tokAt(2)) || (tok->variable() && tok->variable()->nameToken() != tok->tokAt(-2)))
                        varAssignments[tok->varId()] = tok;
                    memAssignments.erase(tok->varId());
                    eraseMemberAssignments(tok->varId(), membervars, varAssignments);
                } else if ((tok->next() && tok->next()->tokType() == Token::eIncDecOp) || (tok->previous()->tokType() == Token::eIncDecOp && tok->strAt(1) == MatchCompiler::makeConstString(";"))) { // Variable incremented/decremented; Prefix-Increment is only suspicious, if its return value is unused
                    varAssignments[tok->varId()] = tok;
                    memAssignments.erase(tok->varId());
                    eraseMemberAssignments(tok->varId(), membervars, varAssignments);
                } else if (!match39(tok->tokAt(-2))) { // Other usage of variable
                    if (it != varAssignments.end())
                        varAssignments.erase(it);
                    if (!writtenArgumentsEnd) // Indicates that we are in the first argument of strcpy/memcpy/... function
                        memAssignments.erase(tok->varId());
                }
            } else if (match27(tok) && !mSettings->library.isFunctionConst(tok->str(), true)) { // Function call. Global variables might be used. Reset their status
                const bool memfunc = match40(tok);
                if (tok->varId()) {
                    // operator(), function pointer
                    varAssignments.erase(tok->varId());

                    // lambda..
                    std::map<unsigned int, std::set<unsigned int>>::const_iterator lambda = usedByLambda.find(tok->varId());
                    if (lambda != usedByLambda.end()) {
                        for (unsigned int varId : lambda->second) {
                            varAssignments.erase(varId);
                        }
                    }
                }

                if (memfunc && tok->strAt(-1) != MatchCompiler::makeConstString("(") && tok->strAt(-1) != MatchCompiler::makeConstString("=")) {
                    const Token* param1 = tok->tokAt(2);
                    writtenArgumentsEnd = param1->next();
                    if (param1->varId() && param1->strAt(1) == MatchCompiler::makeConstString(",") && !match41(tok) && param1->variable() && param1->variable()->isLocal() && param1->variable()->isArray()) {
                        if (tok->str() == MatchCompiler::makeConstString("memset") && initialized.find(param1->varId()) == initialized.end())
                            initialized.insert(param1->varId());
                        else {
                            const std::map<unsigned int, const Token*>::const_iterator it = memAssignments.find(param1->varId());
                            if (it == memAssignments.end())
                                memAssignments[param1->varId()] = tok;
                            else {
                                bool read = false;
                                for (const Token *tok2 = tok->linkAt(1); tok2 != writtenArgumentsEnd; tok2 = tok2->previous()) {
                                    if (tok2->varId() == param1->varId()) {
                                        // TODO: is this a read? maybe it's a write
                                        read = true;
                                        break;
                                    }
                                }
                                if (read) {
                                    memAssignments[param1->varId()] = tok;
                                    continue;
                                }

                                if (printWarning && scope.type == Scope::eSwitch && findmatch37(it->second, tok) )
                                    redundantCopyInSwitchError(it->second, tok, param1->str());
                                else if (printPerformance)
                                    redundantCopyError(it->second, tok, param1->str());
                            }
                        }
                    }
                } else if (scope.type == Scope::eSwitch) { // Avoid false positives if noreturn function is called in switch
                    const Function* const func = tok->function();
                    if (!func || !func->hasBody()) {
                        varAssignments.clear();
                        memAssignments.clear();
                        continue;
                    }
                    const Token* funcEnd = func->functionScope->bodyEnd;
                    bool noreturn;
                    if (!mTokenizer->IsScopeNoReturn(funcEnd, &noreturn) && !noreturn) {
                        eraseNotLocalArg(varAssignments, symbolDatabase);
                        eraseNotLocalArg(memAssignments, symbolDatabase);
                    } else {
                        varAssignments.clear();
                        memAssignments.clear();
                    }
                } else { // Noreturn functions outside switch don't cause problems
                    eraseNotLocalArg(varAssignments, symbolDatabase);
                    eraseNotLocalArg(memAssignments, symbolDatabase);
                }
            }
        }
    }
}

void CheckOther::redundantCopyError(const Token *tok1, const Token* tok2, const std::string& var)
{
    const std::list<const Token *> callstack = { tok1, tok2 };
    reportError(callstack, Severity::performance, "redundantCopy",
                "$symbol:" + var + "\n"
                "Buffer '$symbol' is being written before its old content has been used.", CWE563, false);
}

void CheckOther::redundantCopyInSwitchError(const Token *tok1, const Token* tok2, const std::string &var)
{
    const std::list<const Token *> callstack = { tok1, tok2 };
    reportError(callstack, Severity::warning, "redundantCopyInSwitch",
                "$symbol:" + var + "\n"
                "Buffer '$symbol' is being written before its old content has been used. 'break;' missing?", CWE563, false);
}

void CheckOther::redundantAssignmentError(const Token *tok1, const Token* tok2, const std::string& var, bool inconclusive)
{
    const std::list<const Token *> callstack = { tok1, tok2 };
    if (inconclusive)
        reportError(callstack, Severity::style, "redundantAssignment",
                    "$symbol:" + var + "\n"
                    "Variable '$symbol' is reassigned a value before the old one has been used if variable is no semaphore variable.\n"
                    "Variable '$symbol' is reassigned a value before the old one has been used. Make sure that this variable is not used like a semaphore in a threading environment before simplifying this code.", CWE563, true);
    else
        reportError(callstack, Severity::style, "redundantAssignment",
                    "$symbol:" + var + "\n"
                    "Variable '$symbol' is reassigned a value before the old one has been used.", CWE563, false);
}

void CheckOther::redundantAssignmentInSwitchError(const Token *tok1, const Token* tok2, const std::string &var)
{
    const std::list<const Token *> callstack = { tok1, tok2 };
    reportError(callstack, Severity::warning, "redundantAssignInSwitch",
                "$symbol:" + var + "\n"
                "Variable '$symbol' is reassigned a value before the old one has been used. 'break;' missing?", CWE563, false);
}


//---------------------------------------------------------------------------
//    switch (x)
//    {
//        case 2:
//            y = a;        // <- this assignment is redundant
//        case 3:
//            y = b;        // <- case 2 falls through and sets y twice
//    }
//---------------------------------------------------------------------------
static inline bool isFunctionOrBreakPattern(const Token *tok)
{
    if (match27(tok) || match42(tok))
        return true;

    return false;
}

void CheckOther::checkRedundantAssignmentInSwitch()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    // Find the beginning of a switch. E.g.:
    //   switch (var) { ...
    for (const Scope &switchScope : symbolDatabase->scopeList) {
        if (switchScope.type != Scope::eSwitch || !switchScope.bodyStart)
            continue;

        // Check the contents of the switch statement
        std::map<unsigned int, const Token*> varsWithBitsSet;
        std::map<unsigned int, std::string> bitOperations;

        for (const Token *tok2 = switchScope.bodyStart->next(); tok2 != switchScope.bodyEnd; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("{")) {
                // Inside a conditional or loop. Don't mark variable accesses as being redundant. E.g.:
                //   case 3: b = 1;
                //   case 4: if (a) { b = 2; }    // Doesn't make the b=1 redundant because it's conditional
                if (match43(tok2->previous()) && tok2->link()) {
                    const Token* endOfConditional = tok2->link();
                    for (const Token* tok3 = tok2; tok3 != endOfConditional; tok3 = tok3->next()) {
                        if (tok3->varId() != 0) {
                            varsWithBitsSet.erase(tok3->varId());
                            bitOperations.erase(tok3->varId());
                        } else if (isFunctionOrBreakPattern(tok3)) {
                            varsWithBitsSet.clear();
                            bitOperations.clear();
                        }
                    }
                    tok2 = endOfConditional;
                }
            }

            // Variable assignment. Report an error if it's assigned to twice before a break. E.g.:
            //    case 3: b = 1;    // <== redundant
            //    case 4: b = 2;

            if (match44(tok2->previous())) {
                varsWithBitsSet.erase(tok2->varId());
                bitOperations.erase(tok2->varId());
            }

            // Bitwise operation. Report an error if it's performed twice before a break. E.g.:
            //    case 3: b |= 1;    // <== redundant
            //    case 4: b |= 1;
            else if (match45(tok2->previous()) &&
                     (tok2->strAt(1) == MatchCompiler::makeConstString("|=") || tok2->strAt(1) == MatchCompiler::makeConstString("&=")) &&
                     match46(tok2->next()->astOperand2())) {
                const std::string bitOp = tok2->strAt(1)[0] + tok2->strAt(2);
                const std::map<unsigned int, const Token*>::const_iterator i2 = varsWithBitsSet.find(tok2->varId());

                // This variable has not had a bit operation performed on it yet, so just make a note of it
                if (i2 == varsWithBitsSet.end()) {
                    varsWithBitsSet[tok2->varId()] = tok2;
                    bitOperations[tok2->varId()] = bitOp;
                }

                // The same bit operation has been performed on the same variable twice, so report an error
                else if (bitOperations[tok2->varId()] == bitOp)
                    redundantBitwiseOperationInSwitchError(i2->second, i2->second->str());

                // A different bit operation was performed on the variable, so clear it
                else {
                    varsWithBitsSet.erase(tok2->varId());
                    bitOperations.erase(tok2->varId());
                }
            }

            // Bitwise operation. Report an error if it's performed twice before a break. E.g.:
            //    case 3: b = b | 1;    // <== redundant
            //    case 4: b = b | 1;
            else if (match47(tok2->previous()) &&
                     tok2->varId() == tok2->tokAt(2)->varId()) {
                const std::string bitOp = tok2->strAt(3) + tok2->strAt(4);
                const std::map<unsigned int, const Token*>::const_iterator i2 = varsWithBitsSet.find(tok2->varId());

                // This variable has not had a bit operation performed on it yet, so just make a note of it
                if (i2 == varsWithBitsSet.end()) {
                    varsWithBitsSet[tok2->varId()] = tok2;
                    bitOperations[tok2->varId()] = bitOp;
                }

                // The same bit operation has been performed on the same variable twice, so report an error
                else if (bitOperations[tok2->varId()] == bitOp)
                    redundantBitwiseOperationInSwitchError(i2->second, i2->second->str());

                // A different bit operation was performed on the variable, so clear it
                else {
                    varsWithBitsSet.erase(tok2->varId());
                    bitOperations.erase(tok2->varId());
                }
            }

            // Not a simple assignment so there may be good reason if this variable is assigned to twice. E.g.:
            //    case 3: b = 1;
            //    case 4: b++;
            else if (tok2->varId() != 0 && tok2->strAt(1) != MatchCompiler::makeConstString("|") && tok2->strAt(1) != MatchCompiler::makeConstString("&")) {
                varsWithBitsSet.erase(tok2->varId());
                bitOperations.erase(tok2->varId());
            }

            // Reset our record of assignments if there is a break or function call. E.g.:
            //    case 3: b = 1; break;
            if (isFunctionOrBreakPattern(tok2)) {
                varsWithBitsSet.clear();
                bitOperations.clear();
            }
        }
    }
}

void CheckOther::redundantBitwiseOperationInSwitchError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::warning,
                "redundantBitwiseOperationInSwitch",
                "$symbol:" + varname + "\n"
                "Redundant bitwise operation on '$symbol' in 'switch' statement. 'break;' missing?");
}


//---------------------------------------------------------------------------
// Check for statements like case A||B: in switch()
//---------------------------------------------------------------------------
void CheckOther::checkSuspiciousCaseInSwitch()
{
    if (!mSettings->inconclusive || !mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope & scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eSwitch)
            continue;

        for (const Token* tok = scope.bodyStart->next(); tok != scope.bodyEnd; tok = tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("case")) {
                const Token* finding = nullptr;
                for (const Token* tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString(":"))
                        break;
                    if (match48(tok2))
                        break;

                    if (tok2->str() == MatchCompiler::makeConstString("?"))
                        finding = nullptr;
                    else if (match49(tok2))
                        finding = tok2;
                }
                if (finding)
                    suspiciousCaseInSwitchError(finding, finding->str());
            }
        }
    }
}

void CheckOther::suspiciousCaseInSwitchError(const Token* tok, const std::string& operatorString)
{
    reportError(tok, Severity::warning, "suspiciousCase",
                "Found suspicious case label in switch(). Operator '" + operatorString + "' probably doesn't work as intended.\n"
                "Using an operator like '" + operatorString + "' in a case label is suspicious. Did you intend to use a bitwise operator, multiple case labels or if/else instead?", CWE398, true);
}

//---------------------------------------------------------------------------
//    if (x == 1)
//        x == 0;       // <- suspicious equality comparison.
//---------------------------------------------------------------------------
void CheckOther::checkSuspiciousEqualityComparison()
{
    if (!mSettings->isEnabled(Settings::WARNING) || !mSettings->inconclusive)
        return;

    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (match50(tok)) {
                const Token* const openParen = tok->next();
                const Token* const closeParen = tok->linkAt(1);

                // Search for any suspicious equality comparison in the initialization
                // or increment-decrement parts of the for() loop.
                // For example:
                //    for (i == 2; i < 10; i++)
                // or
                //    for (i = 0; i < 10; i == a)
                if (match51(openParen->next()))
                    suspiciousEqualityComparisonError(openParen->tokAt(2));
                if (closeParen->strAt(-2) == MatchCompiler::makeConstString("=="))
                    suspiciousEqualityComparisonError(closeParen->tokAt(-2));

                // Skip over for() loop conditions because "for (;running==1;)"
                // is a bit strange, but not necessarily incorrect.
                tok = closeParen;
            } else if (match52(tok)) {

                // Exclude compound statements surrounded by parentheses, such as
                //    printf("%i\n", ({x==0;}));
                // because they may appear as an expression in GNU C/C++.
                // See http://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
                const Token* afterStatement = tok->strAt(1) == MatchCompiler::makeConstString("*") ? tok->tokAt(6) : tok->tokAt(5);
                if (!match53(afterStatement))
                    suspiciousEqualityComparisonError(tok->next());
            }
        }
    }
}

void CheckOther::suspiciousEqualityComparisonError(const Token* tok)
{
    reportError(tok, Severity::warning, "suspiciousEqualityComparison",
                "Found suspicious equality comparison. Did you intend to assign a value instead?", CWE482, true);
}


//---------------------------------------------------------------------------
//    Find consecutive return, break, continue, goto or throw statements. e.g.:
//        break; break;
//    Detect dead code, that follows such a statement. e.g.:
//        return(0); foo();
//---------------------------------------------------------------------------
void CheckOther::checkUnreachableCode()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;
    const bool printInconclusive = mSettings->inconclusive;
    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok && tok != scope->bodyEnd; tok = tok->next()) {
            const Token* secondBreak = nullptr;
            const Token* labelName = nullptr;
            if (tok->link() && match54(tok))
                tok = tok->link();
            else if (match55(tok))
                secondBreak = tok->tokAt(2);
            else if (match56(tok)) {
                tok = tok->next(); // tok should point to return or throw
                for (const Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString("(") || tok2->str() == MatchCompiler::makeConstString("{"))
                        tok2 = tok2->link();
                    if (tok2->str() == MatchCompiler::makeConstString(";")) {
                        secondBreak = tok2->next();
                        break;
                    }
                }
            } else if (match57(tok)) {
                secondBreak = tok->tokAt(3);
                labelName = tok->next();
            } else if (match27(tok) && mSettings->library.isnoreturn(tok) && !match58(tok->next()->astParent())) {
                if ((!tok->function() || (tok->function()->token != tok && tok->function()->tokenDef != tok)) && tok->linkAt(1)->strAt(1) != MatchCompiler::makeConstString("{"))
                    secondBreak = tok->linkAt(1)->tokAt(2);
            }

            // Statements follow directly, no line between them. (#3383)
            // TODO: Try to find a better way to avoid false positives due to preprocessor configurations.
            const bool inconclusive = secondBreak && (secondBreak->linenr() - 1 > secondBreak->previous()->linenr());

            if (secondBreak && (printInconclusive || !inconclusive)) {
                if (match59(secondBreak) ||
                    (secondBreak->str() == MatchCompiler::makeConstString("return") && (tok->str() == MatchCompiler::makeConstString("return") || secondBreak->strAt(1) == MatchCompiler::makeConstString(";")))) { // return with value after statements like throw can be necessary to make a function compile
                    duplicateBreakError(secondBreak, inconclusive);
                    tok = findmatch60(secondBreak) ;
                } else if (secondBreak->str() == MatchCompiler::makeConstString("break")) { // break inside switch as second break statement should not issue a warning
                    if (tok->str() == MatchCompiler::makeConstString("break")) // If the previous was a break, too: Issue warning
                        duplicateBreakError(secondBreak, inconclusive);
                    else {
                        if (tok->scope()->type != Scope::eSwitch) // Check, if the enclosing scope is a switch
                            duplicateBreakError(secondBreak, inconclusive);
                    }
                    tok = findmatch60(secondBreak) ;
                } else if (!match61(secondBreak) && secondBreak->strAt(1) != MatchCompiler::makeConstString(":")) { // TODO: No bailout for unconditional scopes
                    // If the goto label is followed by a loop construct in which the label is defined it's quite likely
                    // that the goto jump was intended to skip some code on the first loop iteration.
                    bool labelInFollowingLoop = false;
                    if (labelName && match62(secondBreak)) {
                        const Token *scope2 = findmatch63(secondBreak) ;
                        if (scope2) {
                            for (const Token *tokIter = scope2; tokIter != scope2->link() && tokIter; tokIter = tokIter->next()) {
                                if (match64(tokIter) && labelName->str() == tokIter->strAt(1)) {
                                    labelInFollowingLoop = true;
                                    break;
                                }
                            }
                        }
                    }

                    // hide FP for statements that just hide compiler warnings about unused function arguments
                    bool silencedCompilerWarningOnly = false;
                    const Token *silencedWarning = secondBreak;
                    for (;;) {
                        if (match65(silencedWarning)) {
                            silencedWarning = silencedWarning->tokAt(5);
                            continue;
                        } else if (silencedWarning && silencedWarning == scope->bodyEnd)
                            silencedCompilerWarningOnly = true;

                        break;
                    }
                    if (silencedWarning)
                        secondBreak = silencedWarning;

                    if (!labelInFollowingLoop && !silencedCompilerWarningOnly)
                        unreachableCodeError(secondBreak, inconclusive);
                    tok = findmatch60(secondBreak) ;
                } else
                    tok = secondBreak;

                if (!tok)
                    break;
                tok = tok->previous(); // Will be advanced again by for loop
            }
        }
    }
}

void CheckOther::duplicateBreakError(const Token *tok, bool inconclusive)
{
    reportError(tok, Severity::style, "duplicateBreak",
                "Consecutive return, break, continue, goto or throw statements are unnecessary.\n"
                "Consecutive return, break, continue, goto or throw statements are unnecessary. "
                "The second statement can never be executed, and so should be removed.", CWE561, inconclusive);
}

void CheckOther::unreachableCodeError(const Token *tok, bool inconclusive)
{
    reportError(tok, Severity::style, "unreachableCode",
                "Statements following return, break, continue, goto or throw will never be executed.", CWE561, inconclusive);
}

//---------------------------------------------------------------------------
// Check scope of variables..
//---------------------------------------------------------------------------
void CheckOther::checkVariableScope()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Variable* var : symbolDatabase->variableList()) {
        if (!var || !var->isLocal() || (!var->isPointer() && !var->isReference() && !var->typeStartToken()->isStandardType()))
            continue;

        if (var->isConst())
            continue;

        bool forHead = false; // Don't check variables declared in header of a for loop
        for (const Token* tok = var->typeStartToken(); tok; tok = tok->previous()) {
            if (tok->str() == MatchCompiler::makeConstString("(")) {
                forHead = true;
                break;
            } else if (match33(tok))
                break;
        }
        if (forHead)
            continue;

        const Token* tok = var->nameToken()->next();
        if (match66(tok, var->declarationId())) {
            tok = tok->tokAt(3);
            if (!tok->isNumber() && tok->tokType() != Token::eString && tok->tokType() != Token::eChar && !tok->isBoolean())
                continue;
        }
        // bailout if initialized with function call that has possible side effects
        if (match67(tok) && match68(tok->astOperand2()))
            continue;
        bool reduce = true;
        bool used = false; // Don't warn about unused variables
        for (; tok && tok != var->scope()->bodyEnd; tok = tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("{") && tok->scope() != tok->previous()->scope() && !tok->isExpandedMacro() && tok->scope()->type != Scope::eLambda) {
                if (used) {
                    bool used2 = false;
                    if (!checkInnerScope(tok, var, used2) || used2) {
                        reduce = false;
                        break;
                    }
                } else if (!checkInnerScope(tok, var, used)) {
                    reduce = false;
                    break;
                }

                tok = tok->link();

                // parse else if blocks..
            } else if (match69(tok) && match26(tok->linkAt(3))) {
                const Token *endif = tok->linkAt(3)->linkAt(1);
                bool elseif = false;
                if (match70(endif))
                    elseif = true;
                else if (match71(endif) && match70(endif->linkAt(2)))
                    elseif = true;
                if (elseif && findmatch21(tok->next(), tok->linkAt(1), var->declarationId()) ) {
                    reduce = false;
                    break;
                }
            } else if (tok->varId() == var->declarationId() || tok->str() == MatchCompiler::makeConstString("goto")) {
                reduce = false;
                break;
            }
        }

        if (reduce && used)
            variableScopeError(var->nameToken(), var->name());
    }
}

bool CheckOther::checkInnerScope(const Token *tok, const Variable* var, bool& used)
{
    const Scope* scope = tok->next()->scope();
    bool loopVariable = scope->type == Scope::eFor || scope->type == Scope::eWhile || scope->type == Scope::eDo;
    bool noContinue = true;
    const Token* forHeadEnd = nullptr;
    const Token* end = tok->link();
    if (scope->type == Scope::eUnconditional && (tok->strAt(-1) == MatchCompiler::makeConstString(")") || tok->previous()->isName())) // Might be an unknown macro like BOOST_FOREACH
        loopVariable = true;

    if (scope->type == Scope::eDo) {
        end = end->linkAt(2);
    } else if (loopVariable && tok->strAt(-1) == MatchCompiler::makeConstString(")")) {
        tok = tok->linkAt(-1); // Jump to opening ( of for/while statement
    } else if (scope->type == Scope::eSwitch) {
        for (const Scope* innerScope : scope->nestedList) {
            if (used) {
                bool used2 = false;
                if (!checkInnerScope(innerScope->bodyStart, var, used2) || used2) {
                    return false;
                }
            } else if (!checkInnerScope(innerScope->bodyStart, var, used)) {
                return false;
            }
        }
    }

    bool bFirstAssignment=false;
    for (; tok && tok != end; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("goto"))
            return false;
        if (tok->str() == MatchCompiler::makeConstString("continue"))
            noContinue = false;

        if (match50(tok))
            forHeadEnd = tok->linkAt(1);
        if (tok == forHeadEnd)
            forHeadEnd = nullptr;

        if (loopVariable && noContinue && tok->scope() == scope && !forHeadEnd && scope->type != Scope::eSwitch && match72(tok, var->declarationId())) { // Assigned in outer scope.
            loopVariable = false;
            unsigned int indent = 0;
            for (const Token* tok2 = tok->tokAt(2); tok2; tok2 = tok2->next()) { // Ensure that variable isn't used on right side of =, too
                if (tok2->str() == MatchCompiler::makeConstString("("))
                    indent++;
                else if (tok2->str() == MatchCompiler::makeConstString(")")) {
                    if (indent == 0)
                        break;
                    indent--;
                } else if (tok2->str() == MatchCompiler::makeConstString(";"))
                    break;
                else if (tok2->varId() == var->declarationId()) {
                    loopVariable = true;
                    break;
                }
            }
        }

        if (loopVariable && match73(tok, var->declarationId())) // Variable used in loop
            return false;

        if (match74(tok, var->declarationId())) // Taking address of variable
            return false;

        if (match72(tok, var->declarationId()))
            bFirstAssignment = true;

        if (!bFirstAssignment && match75(tok, var->declarationId())) // dereferencing means access to previous content
            return false;

        if (match76(tok, var->declarationId()) && (var->isArray() || var->isPointer())) // Create a copy of array/pointer. Bailout, because the memory it points to might be necessary in outer scope
            return false;

        if (tok->varId() == var->declarationId()) {
            used = true;
            if (scope->type == Scope::eSwitch && scope == tok->scope())
                return false; // Used in outer switch scope - unsafe or impossible to reduce scope
        }
    }

    return true;
}

void CheckOther::variableScopeError(const Token *tok, const std::string &varname)
{
    reportError(tok,
                Severity::style,
                "variableScope",
                "$symbol:" + varname + "\n"
                "The scope of the variable '$symbol' can be reduced.\n"
                "The scope of the variable '$symbol' can be reduced. Warning: Be careful "
                "when fixing this message, especially when there are inner loops. Here is an "
                "example where cppcheck will write that the scope for 'i' can be reduced:\n"
                "void f(int x)\n"
                "{\n"
                "    int i = 0;\n"
                "    if (x) {\n"
                "        // it's safe to move 'int i = 0;' here\n"
                "        for (int n = 0; n < 10; ++n) {\n"
                "            // it is possible but not safe to move 'int i = 0;' here\n"
                "            do_something(&i);\n"
                "        }\n"
                "    }\n"
                "}\n"
                "When you see this message it is always safe to reduce the variable scope 1 level.", CWE398, false);
}

//---------------------------------------------------------------------------
// Comma in return statement: return a+1, b++;. (experimental)
//---------------------------------------------------------------------------
void CheckOther::checkCommaSeparatedReturn()
{
    // This is experimental for now. See #5076
    if (!mSettings->experimental)
        return;

    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("return")) {
            tok = tok->next();
            while (tok && tok->str() != MatchCompiler::makeConstString(";")) {
                if (tok->link() && match77(tok))
                    tok = tok->link();

                if (!tok->isExpandedMacro() && tok->str() == MatchCompiler::makeConstString(",") && tok->linenr() != tok->next()->linenr())
                    commaSeparatedReturnError(tok);

                tok = tok->next();
            }
            // bailout: missing semicolon (invalid code / bad tokenizer)
            if (!tok)
                break;
        }
    }
}

void CheckOther::commaSeparatedReturnError(const Token *tok)
{
    reportError(tok,
                Severity::style,
                "commaSeparatedReturn",
                "Comma is used in return statement. The comma can easily be misread as a ';'.\n"
                "Comma is used in return statement. When comma is used in a return statement it can "
                "easily be misread as a semicolon. For example in the code below the value "
                "of 'b' is returned if the condition is true, but it is easy to think that 'a+1' is "
                "returned:\n"
                "    if (x)\n"
                "        return a + 1,\n"
                "    b++;\n"
                "However it can be useful to use comma in macros. Cppcheck does not warn when such a "
                "macro is then used in a return statement, it is less likely such code is misunderstood.", CWE398, false);
}

//---------------------------------------------------------------------------
// Check for function parameters that should be passed by const reference
//---------------------------------------------------------------------------
static std::size_t estimateSize(const Type* type, const Settings* settings, const SymbolDatabase* symbolDatabase, std::size_t recursionDepth = 0)
{
    if (recursionDepth > 20)
        return 0;

    std::size_t cumulatedSize = 0;
    for (const Variable&var : type->classScope->varlist) {
        std::size_t size = 0;
        if (var.isStatic())
            continue;
        if (var.isPointer() || var.isReference())
            size = settings->sizeof_pointer;
        else if (var.type() && var.type()->classScope)
            size = estimateSize(var.type(), settings, symbolDatabase, recursionDepth+1);
        else if (var.valueType()->type == ValueType::Type::CONTAINER)
            size = 3 * settings->sizeof_pointer; // Just guess
        else
            size = symbolDatabase->sizeOfType(var.typeStartToken());

        if (var.isArray())
            cumulatedSize += size * var.dimension(0);
        else
            cumulatedSize += size;
    }
    for (const Type::BaseInfo &baseInfo : type->derivedFrom) {
        if (baseInfo.type && baseInfo.type->classScope)
            cumulatedSize += estimateSize(baseInfo.type, settings, symbolDatabase, recursionDepth+1);
    }
    return cumulatedSize;
}

static bool canBeConst(const Variable *var)
{
    {
        // check initializer list. If variable is moved from it can't be const.
        const Function* func_scope = var->scope()->function;
        if (func_scope->type == Function::Type::eConstructor) {
            //could be initialized in initializer list
            if (func_scope->arg->link()->next()->str() == MatchCompiler::makeConstString(":")) {
                for (const Token* tok2 = func_scope->arg->link()->next()->next(); tok2 != var->scope()->bodyStart; tok2 = tok2->next()) {
                    if (tok2->varId() != var->declarationId())
                        continue;
                    const Token* parent = tok2->astParent();
                    if (parent && match78(parent->previous()))
                        return false;
                }
            }
        }
    }
    for (const Token* tok2 = var->scope()->bodyStart; tok2 != var->scope()->bodyEnd; tok2 = tok2->next()) {
        if (tok2->varId() != var->declarationId())
            continue;

        const Token* parent = tok2->astParent();
        if (!parent)
            continue;
        if (parent->str() == MatchCompiler::makeConstString("<<") || isLikelyStreamRead(true, parent)) {
            if (parent->str() == MatchCompiler::makeConstString("<<") && parent->astOperand1() == tok2)
                return false;
            if (parent->str() == MatchCompiler::makeConstString(">>") && parent->astOperand2() == tok2)
                return false;
        } else if (parent->str() == MatchCompiler::makeConstString(",") || parent->str() == MatchCompiler::makeConstString("(")) { // function argument
            const Token* tok3 = tok2->previous();
            unsigned int argNr = 0;
            while (tok3 && tok3->str() != MatchCompiler::makeConstString("(")) {
                if (tok3->link() && match79(tok3))
                    tok3 = tok3->link();
                else if (tok3->link())
                    break;
                else if (tok3->str() == MatchCompiler::makeConstString(";"))
                    break;
                else if (tok3->str() == MatchCompiler::makeConstString(","))
                    argNr++;
                tok3 = tok3->previous();
            }
            if (!tok3 || tok3->str() != MatchCompiler::makeConstString("(") || !tok3->astOperand1() || !tok3->astOperand1()->function())
                return false;
            else {
                const Variable* argVar = tok3->astOperand1()->function()->getArgumentVar(argNr);
                if (!argVar|| (!argVar->isConst() && argVar->isReference()))
                    return false;
            }
        } else if (parent->isUnaryOp("&")) {
            // TODO: check how pointer is used
            return false;
        } else if (parent->isConstOp())
            continue;
        else if (parent->isAssignmentOp()) {
            if (parent->astOperand1() == tok2)
                return false;
            else if (parent->astOperand1()->str() == MatchCompiler::makeConstString("&")) {
                const Variable* assignedVar = parent->previous()->variable();
                if (!assignedVar || !assignedVar->isConst())
                    return false;
            }
        } else if (match80(tok2)) {
            const Function* func = tok2->tokAt(2)->function();
            if (func && (func->isConst() || func->isStatic()))
                continue;
            else
                return false;
        } else
            return false;
    }

    return true;
}

void CheckOther::checkPassByReference()
{
    if (!mSettings->isEnabled(Settings::PERFORMANCE) || mTokenizer->isC())
        return;

    const SymbolDatabase * const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Variable* var : symbolDatabase->variableList()) {
        if (!var || !var->isArgument() || !var->isClass() || var->isPointer() || var->isArray() || var->isReference() || var->isEnumType())
            continue;

        if (var->scope() && var->scope()->function->arg->link()->strAt(-1) == MatchCompiler::makeConstString("."))
            continue; // references could not be used as va_start parameters (#5824)

        bool inconclusive = false;

        if (var->valueType()->type == ValueType::Type::CONTAINER) {
        } else if (var->type() && !var->type()->isEnumType()) { // Check if type is a struct or class.
            // Ensure that it is a large object.
            if (!var->type()->classScope)
                inconclusive = true;
            else if (estimateSize(var->type(), mSettings, symbolDatabase) <= 2 * mSettings->sizeof_pointer)
                continue;
        } else
            continue;

        if (inconclusive && !mSettings->inconclusive)
            continue;

        const bool isConst = var->isConst();
        if (isConst) {
            passedByValueError(var->nameToken(), var->name(), inconclusive);
            continue;
        }

        // Check if variable could be const
        if (!var->scope() || var->scope()->function->isVirtual())
            continue;

        if (canBeConst(var)) {
            passedByValueError(var->nameToken(), var->name(), inconclusive);
        }
    }
}

void CheckOther::passedByValueError(const Token *tok, const std::string &parname, bool inconclusive)
{
    reportError(tok, Severity::performance, "passedByValue",
                "$symbol:" + parname + "\n"
                "Function parameter '$symbol' should be passed by const reference.\n"
                "Parameter '$symbol' is passed by value. It could be passed "
                "as a const reference which is usually faster and recommended in C++.", CWE398, inconclusive);
}

//---------------------------------------------------------------------------
// Check usage of char variables..
//---------------------------------------------------------------------------

void CheckOther::checkCharVariable()
{
    const bool warning = mSettings->isEnabled(Settings::WARNING);
    const bool portability = mSettings->isEnabled(Settings::PORTABILITY);
    if (!warning && !portability)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (match30(tok)) {
                if (!tok->variable())
                    continue;
                if (!tok->variable()->isArray() && !tok->variable()->isPointer())
                    continue;
                const Token *index = tok->next()->astOperand2();
                if (warning && tok->variable()->isArray() && astIsSignedChar(index) && index->getValueGE(0x80, mSettings))
                    signedCharArrayIndexError(tok);
                if (portability && astIsUnknownSignChar(index) && index->getValueGE(0x80, mSettings))
                    unknownSignCharArrayIndexError(tok);
            } else if (warning && match81(tok) && tok->isBinaryOp()) {
                bool warn = false;
                if (astIsSignedChar(tok->astOperand1())) {
                    const ValueFlow::Value *v1 = tok->astOperand1()->getValueLE(-1, mSettings);
                    const ValueFlow::Value *v2 = tok->astOperand2()->getMaxValue(false);
                    if (!v1)
                        v1 = tok->astOperand1()->getValueGE(0x80, mSettings);
                    if (v1 && !(tok->str() == MatchCompiler::makeConstString("&") && v2 && v2->isKnown() && v2->intvalue >= 0 && v2->intvalue < 0x100))
                        warn = true;
                } else if (astIsSignedChar(tok->astOperand2())) {
                    const ValueFlow::Value *v1 = tok->astOperand2()->getValueLE(-1, mSettings);
                    const ValueFlow::Value *v2 = tok->astOperand1()->getMaxValue(false);
                    if (!v1)
                        v1 = tok->astOperand2()->getValueGE(0x80, mSettings);
                    if (v1 && !(tok->str() == MatchCompiler::makeConstString("&") && v2 && v2->isKnown() && v2->intvalue >= 0 && v2->intvalue < 0x100))
                        warn = true;
                }

                // is the result stored in a short|int|long?
                if (warn && match82(tok->astParent())) {
                    const Token *lhs = tok->astParent()->astOperand1();
                    if (lhs && lhs->valueType() && lhs->valueType()->type >= ValueType::Type::SHORT)
                        charBitOpError(tok); // This is an error..
                }
            }
        }
    }
}

void CheckOther::signedCharArrayIndexError(const Token *tok)
{
    reportError(tok,
                Severity::warning,
                "signedCharArrayIndex",
                "Signed 'char' type used as array index.\n"
                "Signed 'char' type used as array index. If the value "
                "can be greater than 127 there will be a buffer underflow "
                "because of sign extension.", CWE128, false);
}

void CheckOther::unknownSignCharArrayIndexError(const Token *tok)
{
    reportError(tok,
                Severity::portability,
                "unknownSignCharArrayIndex",
                "'char' type used as array index.\n"
                "'char' type used as array index. Values greater that 127 will be "
                "treated depending on whether 'char' is signed or unsigned on target platform.", CWE758, false);
}

void CheckOther::charBitOpError(const Token *tok)
{
    reportError(tok,
                Severity::warning,
                "charBitOp",
                "When using 'char' variables in bit operations, sign extension can generate unexpected results.\n"
                "When using 'char' variables in bit operations, sign extension can generate unexpected results. For example:\n"
                "    char c = 0x80;\n"
                "    int i = 0 | c;\n"
                "    if (i & 0x8000)\n"
                "        printf(\"not expected\");\n"
                "The \"not expected\" will be printed on the screen.", CWE398, false);
}

//---------------------------------------------------------------------------
// Incomplete statement..
//---------------------------------------------------------------------------
void CheckOther::checkIncompleteStatement()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match83(tok))
            tok = tok->link();

        else if (tok->str() == MatchCompiler::makeConstString("{") && tok->astParent())
            tok = tok->link();

        // C++11 struct/array/etc initialization in initializer list
        else if (match84(tok->previous()) && !findmatch85(tok,tok->link()) )
            tok = tok->link();


        if (!match86(tok))
            continue;

        // No warning if numeric constant is followed by a "." or ","
        if (match87(tok->next()))
            continue;

        // No warning for [;{}] (void *) 0 ;
        if (match88(tok) && (tok->next()->isCast() || tok->next()->isExpandedMacro()))
            continue;

        // bailout if there is a "? :" in this statement
        bool bailout = false;
        for (const Token *tok2 = tok->tokAt(2); tok2; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("?")) {
                bailout = true;
                break;
            } else if (tok2->str() == MatchCompiler::makeConstString(";"))
                break;
        }
        if (bailout)
            continue;

        // no warning if this is the last statement in a ({})
        for (const Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("("))
                tok2 = tok2->link();
            else if (match33(tok2)) {
                bailout = match89(tok2);
                break;
            }
        }
        if (bailout)
            continue;

        constStatementError(tok->next(), tok->next()->isNumber() ? "numeric" : "string");
    }
}

void CheckOther::constStatementError(const Token *tok, const std::string &type)
{
    reportError(tok, Severity::warning, "constStatement", "Redundant code: Found a statement that begins with " + type + " constant.", CWE398, false);
}

//---------------------------------------------------------------------------
// Detect division by zero.
//---------------------------------------------------------------------------
void CheckOther::checkZeroDivision()
{
    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (!tok->astOperand2() || !tok->astOperand1())
            continue;
        if (tok->str() != MatchCompiler::makeConstString("%") && tok->str() != MatchCompiler::makeConstString("/") && tok->str() != MatchCompiler::makeConstString("%=") && tok->str() != MatchCompiler::makeConstString("/="))
            continue;
        if (!tok->valueType() || !tok->valueType()->isIntegral())
            continue;
        if (tok->astOperand1()->isNumber()) {
            if (MathLib::isFloat(tok->astOperand1()->str()))
                continue;
        } else if (tok->astOperand1()->isName()) {
            if (!tok->astOperand1()->valueType()->isIntegral())
                continue;
        } else if (!tok->astOperand1()->isArithmeticalOp())
            continue;

        // Value flow..
        const ValueFlow::Value *value = tok->astOperand2()->getValue(0LL);
        if (value && mSettings->isEnabled(value, false))
            zerodivError(tok, value);
    }
}

void CheckOther::zerodivError(const Token *tok, const ValueFlow::Value *value)
{
    if (!tok && !value) {
        reportError(tok, Severity::error, "zerodiv", "Division by zero.", CWE369, false);
        reportError(tok, Severity::error, "zerodivcond", ValueFlow::eitherTheConditionIsRedundant(nullptr) + " or there is division by zero.", CWE369, false);
        return;
    }

    const ErrorPath errorPath = getErrorPath(tok, value, "Division by zero");

    std::ostringstream errmsg;
    if (value->condition) {
        const unsigned int line = tok ? tok->linenr() : 0;
        errmsg << ValueFlow::eitherTheConditionIsRedundant(value->condition)
               << " or there is division by zero at line " << line << ".";
    } else
        errmsg << "Division by zero.";

    reportError(errorPath,
                value->errorSeverity() ? Severity::error : Severity::warning,
                value->condition ? "zerodivcond" : "zerodiv",
                errmsg.str(), CWE369, value->isInconclusive());
}

//---------------------------------------------------------------------------
// Check for NaN (not-a-number) in an arithmetic expression, e.g.
// double d = 1.0 / 0.0 + 100.0;
//---------------------------------------------------------------------------

void CheckOther::checkNanInArithmeticExpression()
{
    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match90(tok) ||
            match91(tok) ||
            match92(tok)) {
            nanInArithmeticExpressionError(tok);
        }
    }
}

void CheckOther::nanInArithmeticExpressionError(const Token *tok)
{
    reportError(tok, Severity::style, "nanInArithmeticExpression",
                "Using NaN/Inf in a computation.\n"
                "Using NaN/Inf in a computation. "
                "Although nothing bad really happens, it is suspicious.", CWE369, false);
}

//---------------------------------------------------------------------------
// Creating instance of classes which are destroyed immediately
//---------------------------------------------------------------------------
void CheckOther::checkMisusedScopedObject()
{
    // Skip this check for .c files
    if (mTokenizer->isC())
        return;

    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase * const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart; tok && tok != scope->bodyEnd; tok = tok->next()) {
            if ((tok->next()->type() || (tok->next()->function() && tok->next()->function()->isConstructor())) // TODO: The rhs of || should be removed; It is a workaround for a symboldatabase bug
                && match93(tok)
                && match94(tok->linkAt(2))
                && (!tok->next()->function() || // is not a function on this scope
                    tok->next()->function()->isConstructor())) { // or is function in this scope and it's a ctor
                tok = tok->next();
                misusedScopeObjectError(tok, tok->str());
                tok = tok->next();
            }
        }
    }
}

void CheckOther::misusedScopeObjectError(const Token *tok, const std::string& varname)
{
    reportError(tok, Severity::style,
                "unusedScopedObject",
                "$symbol:" + varname + "\n"
                "Instance of '$symbol' object is destroyed immediately.", CWE563, false);
}

//-----------------------------------------------------------------------------
// check for duplicate code in if and else branches
// if (a) { b = true; } else { b = true; }
//-----------------------------------------------------------------------------
void CheckOther::checkDuplicateBranch()
{
    // This is inconclusive since in practice most warnings are noise:
    // * There can be unfixed low-priority todos. The code is fine as it
    //   is but it could be possible to enhance it. Writing a warning
    //   here is noise since the code is fine (see cppcheck, abiword, ..)
    // * There can be overspecified code so some conditions can't be true
    //   and their conditional code is a duplicate of the condition that
    //   is always true just in case it would be false. See for instance
    //   abiword.
    if (!mSettings->isEnabled(Settings::STYLE) || !mSettings->inconclusive)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope & scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eIf)
            continue;

        // check all the code in the function for if (..) else
        if (match71(scope.bodyEnd)) {
            // Make sure there are no macros (different macros might be expanded
            // to the same code)
            bool macro = false;
            for (const Token *tok = scope.bodyStart; tok != scope.bodyEnd->linkAt(2); tok = tok->next()) {
                if (tok->isExpandedMacro()) {
                    macro = true;
                    break;
                }
            }
            if (macro)
                continue;

            // save if branch code
            const std::string branch1 = scope.bodyStart->next()->stringifyList(scope.bodyEnd);

            if (branch1.empty())
                continue;

            // save else branch code
            const std::string branch2 = scope.bodyEnd->tokAt(3)->stringifyList(scope.bodyEnd->linkAt(2));

            // check for duplicates
            if (branch1 == branch2)
                duplicateBranchError(scope.classDef, scope.bodyEnd->next());
        }
    }
}

void CheckOther::duplicateBranchError(const Token *tok1, const Token *tok2)
{
    const std::list<const Token *> toks = { tok2, tok1 };

    reportError(toks, Severity::style, "duplicateBranch", "Found duplicate branches for 'if' and 'else'.\n"
                "Finding the same code in an 'if' and related 'else' branch is suspicious and "
                "might indicate a cut and paste or logic error. Please examine this code "
                "carefully to determine if it is correct.", CWE398, true);
}


//-----------------------------------------------------------------------------
// Check for a free() of an invalid address
// char* p = malloc(100);
// free(p + 10);
//-----------------------------------------------------------------------------
void CheckOther::checkInvalidFree()
{
    std::map<unsigned int, bool> allocatedVariables;

    const bool printInconclusive = mSettings->inconclusive;
    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {

            // Keep track of which variables were assigned addresses to newly-allocated memory
            if (match95(tok)) {
                allocatedVariables.insert(std::make_pair(tok->varId(), false));
            }

            // If a previously-allocated pointer is incremented or decremented, any subsequent
            // free involving pointer arithmetic may or may not be invalid, so we should only
            // report an inconclusive result.
            else if (match96(tok) &&
                     tok->varId() == tok->tokAt(2)->varId() &&
                     allocatedVariables.find(tok->varId()) != allocatedVariables.end()) {
                if (printInconclusive)
                    allocatedVariables[tok->varId()] = true;
                else
                    allocatedVariables.erase(tok->varId());
            }

            // If a previously-allocated pointer is assigned a completely new value,
            // we can't know if any subsequent free() on that pointer is valid or not.
            else if (match97(tok)) {
                allocatedVariables.erase(tok->varId());
            }

            // If a variable that was previously assigned a newly-allocated memory location is
            // added or subtracted from when used to free the memory, report an error.
            else if (match98(tok) ||
                     match99(tok) ||
                     match100(tok)) {

                const int varIndex = tok->strAt(1) == MatchCompiler::makeConstString("(") ? 2 :
                                     tok->strAt(3) == MatchCompiler::makeConstString("(") ? 4 : 1;
                const unsigned int var1 = tok->tokAt(varIndex)->varId();
                const unsigned int var2 = tok->tokAt(varIndex + 2)->varId();
                const std::map<unsigned int, bool>::const_iterator alloc1 = allocatedVariables.find(var1);
                const std::map<unsigned int, bool>::const_iterator alloc2 = allocatedVariables.find(var2);
                if (alloc1 != allocatedVariables.end()) {
                    invalidFreeError(tok, alloc1->second);
                } else if (alloc2 != allocatedVariables.end()) {
                    invalidFreeError(tok, alloc2->second);
                }
            }

            // If the previously-allocated variable is passed in to another function
            // as a parameter, it might be modified, so we shouldn't report an error
            // if it is later used to free memory
            else if (match27(tok) && !mSettings->library.isFunctionConst(tok->str(), true)) {
                const Token* tok2 = findmatch101(tok->next(), tok->linkAt(1)) ;
                while (tok2 != nullptr) {
                    allocatedVariables.erase(tok2->varId());
                    tok2 = findmatch101(tok2->next(), tok->linkAt(1)) ;
                }
            }
        }
    }
}

void CheckOther::invalidFreeError(const Token *tok, bool inconclusive)
{
    reportError(tok, Severity::error, "invalidFree", "Invalid memory address freed.", CWE(0U), inconclusive);
}


//---------------------------------------------------------------------------
// check for the same expression on both sides of an operator
// (x == x), (x && x), (x || x)
// (x.y == x.y), (x.y && x.y), (x.y || x.y)
//---------------------------------------------------------------------------

namespace {
    bool notconst(const Function* func)
    {
        return !func->isConst();
    }

    void getConstFunctions(const SymbolDatabase *symbolDatabase, std::list<const Function*> &constFunctions)
    {
        for (const Scope &scope : symbolDatabase->scopeList) {
            // only add const functions that do not have a non-const overloaded version
            // since it is pretty much impossible to tell which is being called.
            typedef std::map<std::string, std::list<const Function*> > StringFunctionMap;
            StringFunctionMap functionsByName;
            for (const Function &func : scope.functionList) {
                functionsByName[func.tokenDef->str()].push_back(&func);
            }
            for (StringFunctionMap::iterator it = functionsByName.begin();
                 it != functionsByName.end(); ++it) {
                const std::list<const Function*>::const_iterator nc = std::find_if(it->second.begin(), it->second.end(), notconst);
                if (nc == it->second.end()) {
                    // ok to add all of them
                    constFunctions.splice(constFunctions.end(), it->second);
                }
            }
        }
    }
}

void CheckOther::checkDuplicateExpression()
{
    const bool styleEnabled = mSettings->isEnabled(Settings::STYLE);
    const bool warningEnabled = mSettings->isEnabled(Settings::WARNING);
    if (!styleEnabled && !warningEnabled)
        return;

    // Parse all executing scopes..
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    std::list<const Function*> constFunctions;
    getConstFunctions(symbolDatabase, constFunctions);

    for (const Scope &scope : symbolDatabase->scopeList) {
        // only check functions
        if (scope.type != Scope::eFunction)
            continue;

        for (const Token *tok = scope.bodyStart; tok && tok != scope.bodyEnd; tok = tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("=") && match102(tok->astOperand1())) {
                const Token * endStatement = findmatch103(tok) ;
                if (match104(endStatement)) {
                    endStatement = endStatement->tokAt(4);
                }
                if (match105(endStatement)) {
                    const Token * nextAssign = endStatement->tokAt(1);
                    const Token * var1 = tok->astOperand1();
                    const Token * var2 = nextAssign->astOperand1();
                    if (var1 && var2 &&
                        match106(var1->previous()) &&
                        match106(var2->previous()) &&
                        var2->valueType() && var1->valueType() &&
                        var2->valueType()->originalTypeName == var1->valueType()->originalTypeName &&
                        var2->valueType()->pointer == var1->valueType()->pointer &&
                        var2->valueType()->constness == var1->valueType()->constness &&
                        var2->varId() != var1->varId() && (
                            tok->astOperand2()->isArithmeticalOp() ||
                            tok->astOperand2()->str() == MatchCompiler::makeConstString(".") ||
                            match27(tok->astOperand2()->previous())
                        ) &&
                        tok->next()->tokType() != Token::eType &&
                        tok->next()->tokType() != Token::eName &&
                        isSameExpression(mTokenizer->isCPP(), true, tok->next(), nextAssign->next(), mSettings->library, true, false) &&
                        isSameExpression(mTokenizer->isCPP(), true, tok->astOperand2(), nextAssign->astOperand2(), mSettings->library, true, false) &&
                        tok->astOperand2()->expressionString() == nextAssign->astOperand2()->expressionString() &&
                        !isUniqueExpression(tok->astOperand2())) {
                        bool assigned = false;
                        const Scope * varScope = var1->scope() ? var1->scope() : &scope;
                        for (const Token *assignTok = findmatch103(var2) ; assignTok && assignTok != varScope->bodyEnd; assignTok = assignTok->next()) {
                            if (match107(assignTok, var1->varId()) && match108(assignTok, var2->varId())) {
                                assigned = true;
                            }
                            if (match107(assignTok, var2->varId()) && match108(assignTok, var1->varId())) {
                                assigned = true;
                            }
                        }
                        if (!assigned)
                            duplicateAssignExpressionError(var1, var2);
                    }
                }
            }
            ErrorPath errorPath;
            if (tok->isOp() && tok->astOperand1() && !match109(tok)) {
                if (match110(tok) && astIsFloat(tok->astOperand1(), true))
                    continue;
                if (isSameExpression(mTokenizer->isCPP(), true, tok->astOperand1(), tok->astOperand2(), mSettings->library, true, true, &errorPath)) {
                    if (isWithoutSideEffects(mTokenizer->isCPP(), tok->astOperand1())) {
                        const bool assignment = tok->str() == MatchCompiler::makeConstString("=");
                        if (assignment && warningEnabled)
                            selfAssignmentError(tok, tok->astOperand1()->expressionString());
                        else if (styleEnabled) {
                            if (mTokenizer->isCPP() && mSettings->standards.cpp==Standards::CPP11 && tok->str() == MatchCompiler::makeConstString("==")) {
                                const Token* parent = tok->astParent();
                                while (parent && parent->astParent()) {
                                    parent = parent->astParent();
                                }
                                if (parent && parent->previous() && parent->previous()->str() == MatchCompiler::makeConstString("static_assert")) {
                                    continue;
                                }
                            }
                            duplicateExpressionError(tok->astOperand1(), tok->astOperand2(), tok, errorPath);
                        }
                    }
                } else if (styleEnabled &&
                           isOppositeExpression(mTokenizer->isCPP(), tok->astOperand1(), tok->astOperand2(), mSettings->library, false, true, &errorPath) &&
                           !match111(tok) &&
                           isWithoutSideEffects(mTokenizer->isCPP(), tok->astOperand1())) {
                    oppositeExpressionError(tok, errorPath);
                } else if (!match112(tok)) { // These operators are not associative
                    if (styleEnabled && tok->astOperand2() && tok->str() == tok->astOperand1()->str() && isSameExpression(mTokenizer->isCPP(), true, tok->astOperand2(), tok->astOperand1()->astOperand2(), mSettings->library, true, false, &errorPath) && isWithoutSideEffects(mTokenizer->isCPP(), tok->astOperand2()))
                        duplicateExpressionError(tok->astOperand2(), tok->astOperand1()->astOperand2(), tok, errorPath);
                    else if (tok->astOperand2()) {
                        const Token *ast1 = tok->astOperand1();
                        while (ast1 && tok->str() == ast1->str()) {
                            if (isSameExpression(mTokenizer->isCPP(), true, ast1->astOperand1(), tok->astOperand2(), mSettings->library, true, false, &errorPath) && isWithoutSideEffects(mTokenizer->isCPP(), ast1->astOperand1()))
                                // TODO: warn if variables are unchanged. See #5683
                                // Probably the message should be changed to 'duplicate expressions X in condition or something like that'.
                                ;//duplicateExpressionError(ast1->astOperand1(), tok->astOperand2(), tok, errorPath);
                            else if (styleEnabled && isSameExpression(mTokenizer->isCPP(), true, ast1->astOperand2(), tok->astOperand2(), mSettings->library, true, false, &errorPath) && isWithoutSideEffects(mTokenizer->isCPP(), ast1->astOperand2()))
                                duplicateExpressionError(ast1->astOperand2(), tok->astOperand2(), tok, errorPath);
                            if (!isConstExpression(ast1->astOperand2(), mSettings->library, true))
                                break;
                            ast1 = ast1->astOperand1();
                        }
                    }
                }
            } else if (styleEnabled && tok->astOperand1() && tok->astOperand2() && tok->str() == MatchCompiler::makeConstString(":") && tok->astParent() && tok->astParent()->str() == MatchCompiler::makeConstString("?")) {
                if (!tok->astOperand1()->values().empty() && !tok->astOperand2()->values().empty() && isEqualKnownValue(tok->astOperand1(), tok->astOperand2()))
                    duplicateValueTernaryError(tok);
                else if (isSameExpression(mTokenizer->isCPP(), true, tok->astOperand1(), tok->astOperand2(), mSettings->library, false, true, &errorPath))
                    duplicateExpressionTernaryError(tok, errorPath);
            }
        }
    }
}

void CheckOther::oppositeExpressionError(const Token *opTok, ErrorPath errors)
{
    errors.emplace_back(opTok, "");

    const std::string& op = opTok ? opTok->str() : "&&";

    reportError(errors, Severity::style, "oppositeExpression", "Opposite expression on both sides of \'" + op + "\'.\n"
                "Finding the opposite expression on both sides of an operator is suspicious and might "
                "indicate a cut and paste or logic error. Please examine this code carefully to "
                "determine if it is correct.", CWE398, false);
}

void CheckOther::duplicateExpressionError(const Token *tok1, const Token *tok2, const Token *opTok, ErrorPath errors)
{
    errors.emplace_back(opTok, "");

    const std::string& expr1 = tok1 ? tok1->expressionString() : "x";
    const std::string& expr2 = tok2 ? tok2->expressionString() : "x";

    const std::string& op = opTok ? opTok->str() : "&&";
    std::string msg = "Same expression on both sides of \'" + op + "\'";
    std::string id = "duplicateExpression";
    if (expr1 != expr2) {
        id = "knownConditionTrueFalse";
        std::string exprMsg = "The expression \'" + expr1 + " " + op +  " " + expr2 + "\' is always ";
        if (match113(opTok))
            msg = exprMsg + "true";
        else if (match114(opTok))
            msg = exprMsg + "false";
        if (!match115(tok1) && !match115(tok2))
            msg += " because '" + expr1 + "' and '" + expr2 + "' represent the same value";
    }

    reportError(errors, Severity::style, id.c_str(), msg + ".\n"
                "Finding the same expression on both sides of an operator is suspicious and might "
                "indicate a cut and paste or logic error. Please examine this code carefully to "
                "determine if it is correct.", CWE398, false);
}

void CheckOther::duplicateAssignExpressionError(const Token *tok1, const Token *tok2)
{
    const std::list<const Token *> toks = { tok2, tok1 };

    reportError(toks, Severity::style, "duplicateAssignExpression",
                "Same expression used in consecutive assignments of '" + tok1->str() + "' and '" + tok2->str() + "'.\n"
                "Finding variables '" + tok1->str() + "' and '" + tok2->str() + "' that are assigned the same expression "
                "is suspicious and might indicate a cut and paste or logic error. Please examine this code carefully to "
                "determine if it is correct.", CWE398, false);
}

void CheckOther::duplicateExpressionTernaryError(const Token *tok, ErrorPath errors)
{
    errors.emplace_back(tok, "");
    reportError(errors, Severity::style, "duplicateExpressionTernary", "Same expression in both branches of ternary operator.\n"
                "Finding the same expression in both branches of ternary operator is suspicious as "
                "the same code is executed regardless of the condition.", CWE398, false);
}

void CheckOther::duplicateValueTernaryError(const Token *tok)
{
    reportError(tok, Severity::style, "duplicateValueTernary", "Same value in both branches of ternary operator.\n"
                "Finding the same value in both branches of ternary operator is suspicious as "
                "the same code is executed regardless of the condition.", CWE398, false);
}

void CheckOther::selfAssignmentError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::warning,
                "selfAssignment",
                "$symbol:" + varname + "\n"
                "Redundant assignment of '$symbol' to itself.", CWE398, false);
}

//-----------------------------------------------------------------------------
// Check is a comparison of two variables leads to condition, which is
// always true or false.
// For instance: int a = 1; if(isless(a,a)){...}
// In this case isless(a,a) always evaluates to false.
//
// Reference:
// - http://www.cplusplus.com/reference/cmath/
//-----------------------------------------------------------------------------
void CheckOther::checkComparisonFunctionIsAlwaysTrueOrFalse()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (tok->isName() && match116(tok)) {
                const unsigned int varidLeft = tok->tokAt(2)->varId();// get the left varid
                const unsigned int varidRight = tok->tokAt(4)->varId();// get the right varid
                // compare varids: if they are not zero but equal
                // --> the comparison function is called with the same variables
                if (varidLeft == varidRight) {
                    const std::string& functionName = tok->str(); // store function name
                    const std::string& varNameLeft = tok->strAt(2); // get the left variable name
                    if (functionName == MatchCompiler::makeConstString("isgreater") || functionName == MatchCompiler::makeConstString("isless") || functionName == MatchCompiler::makeConstString("islessgreater")) {
                        // e.g.: isgreater(x,x) --> (x)>(x) --> false
                        checkComparisonFunctionIsAlwaysTrueOrFalseError(tok, functionName, varNameLeft, false);
                    } else { // functionName == MatchCompiler::makeConstString("isgreaterequal") || functionName == MatchCompiler::makeConstString("islessequal")
                        // e.g.: isgreaterequal(x,x) --> (x)>=(x) --> true
                        checkComparisonFunctionIsAlwaysTrueOrFalseError(tok, functionName, varNameLeft, true);
                    }
                }
            }
        }
    }
}
void CheckOther::checkComparisonFunctionIsAlwaysTrueOrFalseError(const Token* tok, const std::string &functionName, const std::string &varName, const bool result)
{
    const std::string strResult = result ? "true" : "false";
    const struct CWE cweResult = result ? CWE571 : CWE570;

    reportError(tok, Severity::warning, "comparisonFunctionIsAlwaysTrueOrFalse",
                "$symbol:" + functionName + "\n"
                "Comparison of two identical variables with $symbol(" + varName + "," + varName + ") always evaluates to " + strResult + ".\n"
                "The function $symbol is designed to compare two variables. Calling this function with one variable (" + varName + ") "
                "for both parameters leads to a statement which is always " + strResult + ".", cweResult, false);
}

//---------------------------------------------------------------------------
// Check testing sign of unsigned variables and pointers.
//---------------------------------------------------------------------------
void CheckOther::checkSignOfUnsignedVariable()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const bool inconclusive = mTokenizer->codeWithTemplates();
    if (inconclusive && !mSettings->inconclusive)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope * scope : symbolDatabase->functionScopes) {
        // check all the code in the function
        for (const Token *tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (!tok->isComparisonOp() || !tok->astOperand1() || !tok->astOperand2())
                continue;

            if (match117(tok) && tok->next() == tok->astOperand2()) {
                const ValueType* vt = tok->astOperand1()->valueType();
                if (vt && vt->pointer)
                    pointerLessThanZeroError(tok, inconclusive);
                if (vt && vt->sign == ValueType::UNSIGNED)
                    unsignedLessThanZeroError(tok, tok->astOperand1()->expressionString(), inconclusive);
            } else if (match118(tok->previous()) && tok->previous() == tok->astOperand1()) {
                const ValueType* vt = tok->astOperand2()->valueType();
                if (vt && vt->pointer)
                    pointerLessThanZeroError(tok, inconclusive);
                if (vt && vt->sign == ValueType::UNSIGNED)
                    unsignedLessThanZeroError(tok, tok->astOperand2()->expressionString(), inconclusive);
            } else if (match119(tok) && tok->next() == tok->astOperand2()) {
                const ValueType* vt = tok->astOperand1()->valueType();
                if (vt && vt->pointer)
                    pointerPositiveError(tok, inconclusive);
                if (vt && vt->sign == ValueType::UNSIGNED)
                    unsignedPositiveError(tok, tok->astOperand1()->str(), inconclusive);
            } else if (match120(tok->previous()) && tok->previous() == tok->astOperand1()) {
                const ValueType* vt = tok->astOperand2()->valueType();
                if (vt && vt->pointer)
                    pointerPositiveError(tok, inconclusive);
                if (vt && vt->sign == ValueType::UNSIGNED)
                    unsignedPositiveError(tok, tok->astOperand2()->str(), inconclusive);
            }
        }
    }
}

void CheckOther::unsignedLessThanZeroError(const Token *tok, const std::string &varname, bool inconclusive)
{
    if (inconclusive) {
        reportError(tok, Severity::style, "unsignedLessThanZero",
                    "$symbol:" + varname + "\n"
                    "Checking if unsigned variable '$symbol' is less than zero. This might be a false warning.\n"
                    "Checking if unsigned variable '$symbol' is less than zero. An unsigned "
                    "variable will never be negative so it is either pointless or an error to check if it is. "
                    "It's not known if the used constant is a template parameter or not and therefore "
                    "this message might be a false warning.", CWE570, true);
    } else {
        reportError(tok, Severity::style, "unsignedLessThanZero",
                    "$symbol:" + varname + "\n"
                    "Checking if unsigned variable '$symbol' is less than zero.\n"
                    "The unsigned variable '$symbol' will never be negative so it "
                    "is either pointless or an error to check if it is.", CWE570, false);
    }
}

void CheckOther::pointerLessThanZeroError(const Token *tok, bool inconclusive)
{
    reportError(tok, Severity::style, "pointerLessThanZero",
                "A pointer can not be negative so it is either pointless or an error to check if it is.", CWE570, inconclusive);
}

void CheckOther::unsignedPositiveError(const Token *tok, const std::string &varname, bool inconclusive)
{
    if (inconclusive) {
        reportError(tok, Severity::style, "unsignedPositive",
                    "$symbol:" + varname + "\n"
                    "Unsigned variable '$symbol' can't be negative so it is unnecessary to test it.\n"
                    "The unsigned variable '$symbol' can't be negative so it is unnecessary to test it. "
                    "It's not known if the used constant is a "
                    "template parameter or not and therefore this message might be a false warning", CWE570, true);
    } else {
        reportError(tok, Severity::style, "unsignedPositive",
                    "$symbol:" + varname + "\n"
                    "Unsigned variable '$symbol' can't be negative so it is unnecessary to test it.", CWE570, false);
    }
}

void CheckOther::pointerPositiveError(const Token *tok, bool inconclusive)
{
    reportError(tok, Severity::style, "pointerPositive",
                "A pointer can not be negative so it is either pointless or an error to check if it is not.", CWE570, inconclusive);
}

/* check if a constructor in given class scope takes a reference */
static bool constructorTakesReference(const Scope * const classScope)
{
    for (const Function &constructor : classScope->functionList) {
        if (constructor.isConstructor()) {
            for (std::size_t argnr = 0U; argnr < constructor.argCount(); argnr++) {
                const Variable * const argVar = constructor.getArgumentVar(argnr);
                if (argVar && argVar->isReference()) {
                    return true;
                }
            }
        }
    }
    return false;
}

//---------------------------------------------------------------------------
// This check rule works for checking the "const A a = getA()" usage when getA() returns "const A &" or "A &".
// In most scenarios, "const A & a = getA()" will be more efficient.
//---------------------------------------------------------------------------
void CheckOther::checkRedundantCopy()
{
    if (!mSettings->isEnabled(Settings::PERFORMANCE) || mTokenizer->isC() || !mSettings->inconclusive)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Variable* var : symbolDatabase->variableList()) {
        if (!var || var->isReference() || !var->isConst() || var->isPointer() || (!var->type() && !var->isStlType())) // bailout if var is of standard type, if it is a pointer or non-const
            continue;

        const Token* startTok = var->nameToken();
        if (startTok->strAt(1) == MatchCompiler::makeConstString("=")) // %type% %name% = ... ;
            ;
        else if (startTok->strAt(1) == MatchCompiler::makeConstString("(") && var->isClass() && var->typeScope()) {
            // Object is instantiated. Warn if constructor takes arguments by value.
            if (constructorTakesReference(var->typeScope()))
                continue;
        } else
            continue;

        const Token* tok = startTok->next()->astOperand2();
        if (!tok)
            continue;
        if (!match27(tok->previous()))
            continue;
        if (!match121(tok->link())) // bailout for usage like "const A a = getA()+3"
            continue;

        const Function* func = tok->previous()->function();
        if (func && func->tokenDef->strAt(-1) == MatchCompiler::makeConstString("&")) {
            redundantCopyError(startTok, startTok->str());
        }
    }
}
void CheckOther::redundantCopyError(const Token *tok,const std::string& varname)
{
    reportError(tok, Severity::performance, "redundantCopyLocalConst",
                "$symbol:" + varname + "\n"
                "Use const reference for '$symbol' to avoid unnecessary data copying.\n"
                "The const variable '$symbol' is assigned a copy of the data. You can avoid "
                "the unnecessary data copying by converting '$symbol' to const reference.",
                CWE398,
                true); // since #5618 that check became inconclusive
}

//---------------------------------------------------------------------------
// Checking for shift by negative values
//---------------------------------------------------------------------------

static bool isNegative(const Token *tok, const Settings *settings)
{
    return tok->valueType() && tok->valueType()->sign == ValueType::SIGNED && tok->getValueLE(-1LL, settings);
}

void CheckOther::checkNegativeBitwiseShift()
{
    const bool portability = mSettings->isEnabled(Settings::PORTABILITY);

    for (const Token* tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (!tok->astOperand1() || !tok->astOperand2())
            continue;

        if (!match122(tok))
            continue;

        // don't warn if lhs is a class. this is an overloaded operator then
        if (mTokenizer->isCPP()) {
            const ValueType * lhsType = tok->astOperand1()->valueType();
            if (!lhsType || !lhsType->isIntegral())
                continue;
        }

        // bailout if operation is protected by ?:
        bool ternary = false;
        for (const Token *parent = tok; parent; parent = parent->astParent()) {
            if (match58(parent)) {
                ternary = true;
                break;
            }
        }
        if (ternary)
            continue;

        // Get negative rhs value. preferably a value which doesn't have 'condition'.
        if (portability && isNegative(tok->astOperand1(), mSettings))
            negativeBitwiseShiftError(tok, 1);
        else if (isNegative(tok->astOperand2(), mSettings))
            negativeBitwiseShiftError(tok, 2);
    }
}


void CheckOther::negativeBitwiseShiftError(const Token *tok, int op)
{
    if (op == 1)
        // LHS - this is used by intention in various software, if it
        // is used often in a project and works as expected then this is
        // a portability issue
        reportError(tok, Severity::portability, "shiftNegativeLHS", "Shifting a negative value is technically undefined behaviour", CWE758, false);
    else // RHS
        reportError(tok, Severity::error, "shiftNegative", "Shifting by a negative value is undefined behaviour", CWE758, false);
}

//---------------------------------------------------------------------------
// Check for incompletely filled buffers.
//---------------------------------------------------------------------------
void CheckOther::checkIncompleteArrayFill()
{
    if (!mSettings->inconclusive)
        return;
    const bool printWarning = mSettings->isEnabled(Settings::WARNING);
    const bool printPortability = mSettings->isEnabled(Settings::PORTABILITY);
    if (!printPortability && !printWarning)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match123(tok) && match124(tok->linkAt(1)->tokAt(-2))) {
                const Variable *var = tok->tokAt(2)->variable();
                if (!var || !var->isArray() || var->dimensions().empty() || !var->dimension(0))
                    continue;

                if (MathLib::toLongNumber(tok->linkAt(1)->strAt(-1)) == var->dimension(0)) {
                    unsigned int size = mTokenizer->sizeOfType(var->typeStartToken());
                    if (size == 0 && var->valueType()->pointer)
                        size = mSettings->sizeof_pointer;
                    if ((size != 1 && size != 100 && size != 0) || var->isPointer()) {
                        if (printWarning)
                            incompleteArrayFillError(tok, var->name(), tok->str(), false);
                    } else if (var->valueType()->type == ValueType::Type::BOOL && printPortability) // sizeof(bool) is not 1 on all platforms
                        incompleteArrayFillError(tok, var->name(), tok->str(), true);
                }
            }
        }
    }
}

void CheckOther::incompleteArrayFillError(const Token* tok, const std::string& buffer, const std::string& function, bool boolean)
{
    if (boolean)
        reportError(tok, Severity::portability, "incompleteArrayFill",
                    "$symbol:" + buffer + "\n"
                    "$symbol:" + function + "\n"
                    "Array '" + buffer + "' might be filled incompletely. Did you forget to multiply the size given to '" + function + "()' with 'sizeof(*" + buffer + ")'?\n"
                    "The array '" + buffer + "' is filled incompletely. The function '" + function + "()' needs the size given in bytes, but the type 'bool' is larger than 1 on some platforms. Did you forget to multiply the size with 'sizeof(*" + buffer + ")'?", CWE131, true);
    else
        reportError(tok, Severity::warning, "incompleteArrayFill",
                    "$symbol:" + buffer + "\n"
                    "$symbol:" + function + "\n"
                    "Array '" + buffer + "' is filled incompletely. Did you forget to multiply the size given to '" + function + "()' with 'sizeof(*" + buffer + ")'?\n"
                    "The array '" + buffer + "' is filled incompletely. The function '" + function + "()' needs the size given in bytes, but an element of the given array is larger than one byte. Did you forget to multiply the size with 'sizeof(*" + buffer + ")'?", CWE131, true);
}

//---------------------------------------------------------------------------
// Detect NULL being passed to variadic function.
//---------------------------------------------------------------------------

void CheckOther::checkVarFuncNullUB()
{
    if (!mSettings->isEnabled(Settings::PORTABILITY))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            // Is NULL passed to a function?
            if (match125(tok)) {
                // Locate function name in this function call.
                const Token *ftok = tok;
                std::size_t argnr = 1;
                while (ftok && ftok->str() != MatchCompiler::makeConstString("(")) {
                    if (ftok->str() == MatchCompiler::makeConstString(")"))
                        ftok = ftok->link();
                    else if (ftok->str() == MatchCompiler::makeConstString(","))
                        ++argnr;
                    ftok = ftok->previous();
                }
                ftok = ftok ? ftok->previous() : nullptr;
                if (ftok && ftok->isName()) {
                    // If this is a variadic function then report error
                    const Function *f = ftok->function();
                    if (f && f->argCount() <= argnr) {
                        const Token *tok2 = f->argDef;
                        tok2 = tok2 ? tok2->link() : nullptr; // goto ')'
                        if (tok2 && match126(tok2->tokAt(-3)))
                            varFuncNullUBError(tok);
                    }
                }
            }
        }
    }
}

void CheckOther::varFuncNullUBError(const Token *tok)
{
    reportError(tok,
                Severity::portability,
                "varFuncNullUB",
                "Passing NULL after the last typed argument to a variadic function leads to undefined behaviour.\n"
                "Passing NULL after the last typed argument to a variadic function leads to undefined behaviour.\n"
                "The C99 standard, in section 7.15.1.1, states that if the type used by va_arg() is not compatible with the type of the actual next argument (as promoted according to the default argument promotions), the behavior is undefined.\n"
                "The value of the NULL macro is an implementation-defined null pointer constant (7.17), which can be any integer constant expression with the value 0, or such an expression casted to (void*) (6.3.2.3). This includes values like 0, 0L, or even 0LL.\n"
                "In practice on common architectures, this will cause real crashes if sizeof(int) != sizeof(void*), and NULL is defined to 0 or any other null pointer constant that promotes to int.\n"
                "To reproduce you might be able to use this little code example on 64bit platforms. If the output includes \"ERROR\", the sentinel had only 4 out of 8 bytes initialized to zero and was not detected as the final argument to stop argument processing via va_arg(). Changing the 0 to (void*)0 or 0L will make the \"ERROR\" output go away.\n"
                "#include <stdarg.h>\n"
                "#include <stdio.h>\n"
                "\n"
                "void f(char *s, ...) {\n"
                "    va_list ap;\n"
                "    va_start(ap,s);\n"
                "    for (;;) {\n"
                "        char *p = va_arg(ap,char*);\n"
                "        printf(\"%018p, %s\\n\", p, (long)p & 255 ? p : \"\");\n"
                "        if(!p) break;\n"
                "    }\n"
                "    va_end(ap);\n"
                "}\n"
                "\n"
                "void g() {\n"
                "    char *s2 = \"x\";\n"
                "    char *s3 = \"ERROR\";\n"
                "\n"
                "    // changing 0 to 0L for the 7th argument (which is intended to act as sentinel) makes the error go away on x86_64\n"
                "    f(\"first\", s2, s2, s2, s2, s2, 0, s3, (char*)0);\n"
                "}\n"
                "\n"
                "void h() {\n"
                "    int i;\n"
                "    volatile unsigned char a[1000];\n"
                "    for (i = 0; i<sizeof(a); i++)\n"
                "        a[i] = -1;\n"
                "}\n"
                "\n"
                "int main() {\n"
                "    h();\n"
                "    g();\n"
                "    return 0;\n"
                "}", CWE475, false);
}

void CheckOther::checkRedundantPointerOp()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (!tok->isUnaryOp("&") || !tok->astOperand1()->isUnaryOp("*"))
            continue;

        // variable
        const Token *varTok = tok->astOperand1()->astOperand1();
        if (!varTok || varTok->isExpandedMacro())
            continue;

        const Variable *var = varTok->variable();
        if (!var || !var->isPointer())
            continue;

        redundantPointerOpError(tok, var->name(), false);
    }
}

void CheckOther::redundantPointerOpError(const Token* tok, const std::string &varname, bool inconclusive)
{
    reportError(tok, Severity::style, "redundantPointerOp",
                "$symbol:" + varname + "\n"
                "Redundant pointer operation on '$symbol' - it's already a pointer.", CWE398, inconclusive);
}

void CheckOther::checkInterlockedDecrement()
{
    if (!mSettings->isWindowsPlatform()) {
        return;
    }

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (tok->isName() && match127(tok)) {
            const Token* interlockedVarTok = tok->tokAt(3);
            const Token* checkStartTok =  interlockedVarTok->tokAt(5);
            if ((match128(checkStartTok) && checkStartTok->strAt(2) == interlockedVarTok->str()) ||
                (match129(checkStartTok) && checkStartTok->strAt(1) == interlockedVarTok->str()) ||
                (match130(checkStartTok) && checkStartTok->str() == interlockedVarTok->str()) ||
                (match131(checkStartTok) && checkStartTok->str() == interlockedVarTok->str())) {
                raceAfterInterlockedDecrementError(checkStartTok);
            }
        } else if (match132(tok)) {
            const Token* condEnd = tok->next()->link();
            const Token* funcTok = tok->tokAt(2);
            const Token* firstAccessTok = funcTok->str() == MatchCompiler::makeConstString("::") ? funcTok->tokAt(4) : funcTok->tokAt(3);
            if (condEnd && condEnd->next() && condEnd->next()->link()) {
                const Token* ifEndTok = condEnd->next()->link();
                if (match133(ifEndTok)) {
                    const Token* secondAccessTok = ifEndTok->tokAt(2);
                    if (secondAccessTok->str() == firstAccessTok->str()) {
                        raceAfterInterlockedDecrementError(secondAccessTok);
                    }
                } else if (match134(ifEndTok)) {
                    const Token* secondAccessTok = ifEndTok->tokAt(4);
                    if (secondAccessTok->str() == firstAccessTok->str()) {
                        raceAfterInterlockedDecrementError(secondAccessTok);
                    }
                }
            }
        }
    }
}

void CheckOther::raceAfterInterlockedDecrementError(const Token* tok)
{
    reportError(tok, Severity::error, "raceAfterInterlockedDecrement",
                "Race condition: non-interlocked access after InterlockedDecrement(). Use InterlockedDecrement() return value instead.", CWE362, false);
}

void CheckOther::checkUnusedLabel()
{
    if (!mSettings->isEnabled(Settings::STYLE) && !mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (!tok->scope()->isExecutable())
                tok = tok->scope()->bodyEnd;

            if (match135(tok) && tok->strAt(1) != MatchCompiler::makeConstString("default")) {
                if (!Token::findsimplematch(scope->bodyStart->next(), ("goto " + tok->strAt(1)).c_str(), scope->bodyEnd->previous()))
                    unusedLabelError(tok->next(), tok->next()->scope()->type == Scope::eSwitch);
            }
        }
    }
}

void CheckOther::unusedLabelError(const Token* tok, bool inSwitch)
{
    if (inSwitch) {
        if (!tok || mSettings->isEnabled(Settings::WARNING))
            reportError(tok, Severity::warning, "unusedLabelSwitch",
                        "$symbol:" + (tok ? tok->str() : emptyString) + "\n"
                        "Label '$symbol' is not used. Should this be a 'case' of the enclosing switch()?", CWE398, false);
    } else {
        if (!tok || mSettings->isEnabled(Settings::STYLE))
            reportError(tok, Severity::style, "unusedLabel",
                        "$symbol:" + (tok ? tok->str() : emptyString) + "\n"
                        "Label '$symbol' is not used.", CWE398, false);
    }
}


void CheckOther::checkEvaluationOrder()
{
    // This checker is not written according to C++11 sequencing rules
    if (mTokenizer->isCPP() && mSettings->standards.cpp >= Standards::CPP11)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * functionScope : symbolDatabase->functionScopes) {
        for (const Token* tok = functionScope->bodyStart; tok != functionScope->bodyEnd; tok = tok->next()) {
            if (!match36(tok) && !tok->isAssignmentOp())
                continue;
            if (!tok->astOperand1())
                continue;
            for (const Token *tok2 = tok;; tok2 = tok2->astParent()) {
                // If ast parent is a sequence point then break
                const Token * const parent = tok2->astParent();
                if (!parent)
                    break;
                if (match136(parent))
                    break;
                if (parent->str() == MatchCompiler::makeConstString(",")) {
                    const Token *par = parent;
                    while (match137(par))
                        par = par->astParent();
                    // not function or in a while clause => break
                    if (!(par && par->str() == MatchCompiler::makeConstString("(") && par->astOperand2() && par->strAt(-1) != MatchCompiler::makeConstString("while")))
                        break;
                    // control flow (if|while|etc) => break
                    if (match26(par->link()))
                        break;
                    // sequence point in function argument: dostuff((1,2),3) => break
                    par = par->next();
                    while (par && (par->previous() != parent))
                        par = par->nextArgument();
                    if (!par)
                        break;
                }
                if (parent->str() == MatchCompiler::makeConstString("(") && parent->astOperand2())
                    break;

                // self assignment..
                if (tok2 == tok &&
                    tok->str() == MatchCompiler::makeConstString("=") &&
                    parent->str() == MatchCompiler::makeConstString("=") &&
                    isSameExpression(mTokenizer->isCPP(), false, tok->astOperand1(), parent->astOperand1(), mSettings->library, true, false)) {
                    if (mSettings->isEnabled(Settings::WARNING) &&
                        isSameExpression(mTokenizer->isCPP(), true, tok->astOperand1(), parent->astOperand1(), mSettings->library, true, false))
                        selfAssignmentError(parent, tok->astOperand1()->expressionString());
                    break;
                }

                // Is expression used?
                bool foundError = false;
                std::stack<const Token *> tokens;
                tokens.push((parent->astOperand1() != tok2) ? parent->astOperand1() : parent->astOperand2());
                while (!tokens.empty() && !foundError) {
                    const Token * const tok3 = tokens.top();
                    tokens.pop();
                    if (!tok3)
                        continue;
                    if (tok3->str() == MatchCompiler::makeConstString("&") && !tok3->astOperand2())
                        continue; // don't handle address-of for now
                    if (tok3->str() == MatchCompiler::makeConstString("(") && match138(tok3->previous()))
                        continue; // don't care about sizeof usage
                    tokens.push(tok3->astOperand1());
                    tokens.push(tok3->astOperand2());
                    if (isSameExpression(mTokenizer->isCPP(), false, tok->astOperand1(), tok3, mSettings->library, true, false)) {
                        foundError = true;
                    }
                }

                if (foundError) {
                    unknownEvaluationOrder(parent);
                    break;
                }
            }
        }
    }
}

void CheckOther::unknownEvaluationOrder(const Token* tok)
{
    reportError(tok, Severity::error, "unknownEvaluationOrder",
                "Expression '" + (tok ? tok->expressionString() : std::string("x = x++;")) + "' depends on order of evaluation of side effects", CWE768, false);
}

void CheckOther::checkAccessOfMovedVariable()
{
    if (!mTokenizer->isCPP() || mSettings->standards.cpp < Standards::CPP11 || !mSettings->isEnabled(Settings::WARNING))
        return;
    const bool reportInconclusive = mSettings->inconclusive;
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        const Token * scopeStart = scope->bodyStart;
        if (scope->function) {
            const Token * memberInitializationStart = scope->function->constructorMemberInitialization();
            if (memberInitializationStart)
                scopeStart = memberInitializationStart;
        }
        for (const Token* tok = scopeStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            const ValueFlow::Value * movedValue = tok->getMovedValue();
            if (!movedValue || movedValue->moveKind == ValueFlow::Value::NonMovedVariable)
                continue;
            if (movedValue->isInconclusive() && !reportInconclusive)
                continue;

            bool inconclusive = false;
            bool accessOfMoved = false;
            if (tok->strAt(1) == MatchCompiler::makeConstString(".")) {
                if (tok->next()->originalName() == MatchCompiler::makeConstString("->"))
                    accessOfMoved = true;
                else
                    inconclusive = true;
            } else {
                const bool isVariableChanged = isVariableChangedByFunctionCall(tok, mSettings, &inconclusive);
                accessOfMoved = !isVariableChanged;
                if (inconclusive) {
                    accessOfMoved = !isMovedParameterAllowedForInconclusiveFunction(tok);
                    if (accessOfMoved)
                        inconclusive = false;
                }
            }
            if (accessOfMoved || (inconclusive && reportInconclusive))
                accessMovedError(tok, tok->str(), movedValue, inconclusive || movedValue->isInconclusive());
        }
    }
}

bool CheckOther::isMovedParameterAllowedForInconclusiveFunction(const Token * tok)
{
    if (match139(tok->tokAt(-4)))
        return false;
    const Token * tokAtM2 = tok->tokAt(-2);
    if (match140(tokAtM2) && tokAtM2->link()) {
        const Token * leftAngle = tokAtM2->link();
        if (match141(leftAngle->tokAt(-3)))
            return false;
    }
    return true;
}

void CheckOther::accessMovedError(const Token *tok, const std::string &varname, const ValueFlow::Value *value, bool inconclusive)
{
    if (!tok) {
        reportError(tok, Severity::warning, "accessMoved", "Access of moved variable 'v'.", CWE672, false);
        reportError(tok, Severity::warning, "accessForwarded", "Access of forwarded variable 'v'.", CWE672, false);
        return;
    }

    const char * errorId = nullptr;
    std::string kindString;
    switch (value->moveKind) {
    case ValueFlow::Value::MovedVariable:
        errorId = "accessMoved";
        kindString = "moved";
        break;
    case ValueFlow::Value::ForwardedVariable:
        errorId = "accessForwarded";
        kindString = "forwarded";
        break;
    default:
        return;
    }
    const std::string errmsg("$symbol:" + varname + "\nAccess of " + kindString + " variable '$symbol'.");
    const ErrorPath errorPath = getErrorPath(tok, value, errmsg);
    reportError(errorPath, Severity::warning, errorId, errmsg, CWE672, inconclusive);
}



void CheckOther::checkFuncArgNamesDifferent()
{
    const bool style = mSettings->isEnabled(Settings::STYLE);
    const bool inconclusive = mSettings->inconclusive;
    const bool warning = mSettings->isEnabled(Settings::WARNING);

    if (!(warning || (style && inconclusive)))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    // check every function
    for (const Scope *scope : symbolDatabase->functionScopes) {
        const Function * function = scope->function;
        // only check functions with arguments
        if (!function || function->argCount() == 0)
            continue;

        // only check functions with separate declarations and definitions
        if (function->argDef == function->arg)
            continue;

        // get the function argument name tokens
        std::vector<const Token *>  declarations(function->argCount());
        std::vector<const Token *>  definitions(function->argCount());
        const Token * decl = function->argDef->next();
        for (std::size_t j = 0; j < function->argCount(); ++j) {
            declarations[j] = nullptr;
            definitions[j] = nullptr;
            // get the definition
            const Variable * variable = function->getArgumentVar(j);
            if (variable) {
                definitions[j] = variable->nameToken();
            }
            // get the declaration (search for first token with varId)
            while (decl && !match142(decl)) {
                // skip everything after the assignment because
                // it could also have a varId or be the first
                // token with a varId if there is no name token
                if (decl->str() == MatchCompiler::makeConstString("=")) {
                    decl = decl->nextArgument();
                    break;
                }
                // skip over template
                if (decl->link())
                    decl = decl->link();
                else if (decl->varId())
                    declarations[j] = decl;
                decl = decl->next();
            }
            if (match137(decl))
                decl = decl->next();
        }
        // check for different argument order
        if (warning) {
            bool order_different = false;
            for (std::size_t j = 0; j < function->argCount(); ++j) {
                if (!declarations[j] || !definitions[j] || declarations[j]->str() == definitions[j]->str())
                    continue;

                for (std::size_t k = 0; k < function->argCount(); ++k) {
                    if (j != k && definitions[k] && declarations[j]->str() == definitions[k]->str()) {
                        order_different = true;
                        break;
                    }
                }
            }
            if (order_different) {
                funcArgOrderDifferent(function->name(), function->argDef->next(), function->arg->next(), declarations, definitions);
                continue;
            }
        }
        // check for different argument names
        if (style && inconclusive) {
            for (std::size_t j = 0; j < function->argCount(); ++j) {
                if (declarations[j] && definitions[j] && declarations[j]->str() != definitions[j]->str())
                    funcArgNamesDifferent(function->name(), j, declarations[j], definitions[j]);
            }
        }
    }
}

void CheckOther::funcArgNamesDifferent(const std::string & functionName, size_t index,
                                       const Token* declaration, const Token* definition)
{
    std::list<const Token *> tokens = { declaration,definition };
    reportError(tokens, Severity::style, "funcArgNamesDifferent",
                "$symbol:" + functionName + "\n"
                "Function '$symbol' argument " + MathLib::toString(index + 1) + " names different: declaration '" +
                (declaration ? declaration->str() : std::string("A")) + "' definition '" +
                (definition ? definition->str() : std::string("B")) + "'.", CWE628, true);
}

void CheckOther::funcArgOrderDifferent(const std::string & functionName,
                                       const Token* declaration, const Token* definition,
                                       const std::vector<const Token *> & declarations,
                                       const std::vector<const Token *> & definitions)
{
    std::list<const Token *> tokens = {
        declarations.size() ? declarations[0] ? declarations[0] : declaration : nullptr,
        definitions.size() ? definitions[0] ? definitions[0] : definition : nullptr
    };
    std::string msg = "$symbol:" + functionName + "\nFunction '$symbol' argument order different: declaration '";
    for (std::size_t i = 0; i < declarations.size(); ++i) {
        if (i != 0)
            msg += ", ";
        if (declarations[i])
            msg += declarations[i]->str();
    }
    msg += "' definition '";
    for (std::size_t i = 0; i < definitions.size(); ++i) {
        if (i != 0)
            msg += ", ";
        if (definitions[i])
            msg += definitions[i]->str();
    }
    msg += "'";
    reportError(tokens, Severity::warning, "funcArgOrderDifferent", msg, CWE683, false);
}

