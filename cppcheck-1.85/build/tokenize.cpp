#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %name%|::|:
static bool match1(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: ) ;|{|[
static bool match2(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: const|noexcept|override|final|volatile|&|&& !!(
static bool match3(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("noexcept")) || (tok->str()==MatchCompiler::makeConstString("override")) || (tok->str()==MatchCompiler::makeConstString("final")) || (tok->str()==MatchCompiler::makeConstString("volatile")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("("))
        return false;
    return true;
}
// pattern: %name% !!(
static bool match4(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("("))
        return false;
    return true;
}
// pattern: throw|noexcept (
static bool match5(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("noexcept"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% (
static bool match6(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [;{]
static bool match7(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{", tok->str()[0]))
        return false;
    return true;
}
// pattern: <|[|(
static bool match8(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: = 0|default|delete ;
static bool match9(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("0")) || (tok->str()==MatchCompiler::makeConstString("default")) || (tok->str()==MatchCompiler::makeConstString("delete"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: class|struct|union|enum|%name%|>|>> {
static bool match10(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("enum")) || tok->isName() || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: class|struct|union|enum|{|}|;
static bool match11(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("enum")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: class|struct|union|enum
static bool match12(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    return true;
}
// pattern: ;|,|[|=|)|>|(|{
static bool match13(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: public|private|protected
static bool match14(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("private")) || (tok->str()==MatchCompiler::makeConstString("protected"))))
        return false;
    return true;
}
// pattern: ;|)|>
static bool match15(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: public:|private:|protected:
static bool match16(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("public:")) || (tok->str()==MatchCompiler::makeConstString("private:")) || (tok->str()==MatchCompiler::makeConstString("protected:"))))
        return false;
    return true;
}
// pattern: %type%
static bool match17(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: ,|<
static bool match18(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: ) {
static bool match19(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: return|new|const|struct
static bool match20(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("new")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    return true;
}
// pattern: return|new|const|volatile
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("new")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        return false;
    return true;
}
// pattern: typedef|}|>
static bool match22(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("typedef")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: return|new|const|friend|public|private|protected|throw|extern
static bool match23(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("new")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("friend")) || (tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("private")) || (tok->str()==MatchCompiler::makeConstString("protected")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("extern"))))
        return false;
    return true;
}
// pattern: friend class
static bool match24(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("friend")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("class")))
        return false;
    return true;
}
// pattern: ;|{
static bool match25(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: )|*
static bool match26(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: ) .|(|[
static bool match27(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: ( * %type% ) (
static bool match28(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: * ( * %type% ) (
static bool match29(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: * ( * %type% ) ;
static bool match30(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: * ( %type% [
static bool match31(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] ) ;|=
static bool match32(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: * ( * %type% (
static bool match33(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: * [
static bool match34(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] ;
static bool match35(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [|>|;
static bool match36(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: *|&
static bool match37(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: )|>
static bool match38(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: %name% ::
static bool match39(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: class|struct|namespace %any%
static bool match40(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("namespace"))))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    return true;
}
// pattern: const| struct|enum|union|class %type%| {
static bool match41(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("enum")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    tok = tok->next();
    if (tok && ((tok->isName() && tok->varId()==0U && !tok->isKeyword())))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: const| struct|class %type% :
static bool match42(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: enum %type% %type% ;
static bool match43(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("enum")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %type% ::
static bool match44(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: const|struct|enum %type%
static bool match45(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: unsigned|signed
static bool match46(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("unsigned")) || (tok->str()==MatchCompiler::makeConstString("signed"))))
        return false;
    return true;
}
// pattern: [|;|,|(
static bool match47(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: const (
static bool match48(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: :: %type%
static bool match49(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: const|volatile
static bool match50(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        return false;
    return true;
}
// pattern: *|&|&&|const
static bool match51(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("const"))))
        return false;
    return true;
}
// pattern: ;|,
static bool match52(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: ( *|%name%
static bool match53(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || tok->isName()))
        return false;
    return true;
}
// pattern: * %name% ) (
static bool match54(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
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
// pattern: __typeof__ (
static bool match55(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("__typeof__")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) %type% ;
static bool match56(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %type% ) (
static bool match57(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) const|volatile|;
static bool match58(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: ( (
static bool match59(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) const|volatile| ) ;|,
static bool match60(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: ( * (
static bool match61(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( %type% (
static bool match62(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( * ( * %type% ) (
static bool match63(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ) (
static bool match64(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ;|,
static bool match65(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: ( * %type% (
static bool match66(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( * ( %type% ) (
static bool match67(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( *|& %type% ) [
static bool match68(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ( %type% :: * %type% ) ;
static bool match69(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: )|] const| {
static bool match70(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]"))))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ) const| {
static bool match71(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: case|;|{|} %type% :
static bool match72(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: %type% *|&
static bool match73(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: public|protected|private %type% {|,
static bool match74(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("protected")) || (tok->str()==MatchCompiler::makeConstString("private"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: > (
static bool match75(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: <|,
static bool match76(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: &|*| &|*| >|,
static bool match77(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: sizeof ( %type% )
static bool match78(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: operator const
static bool match79(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("operator")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    return true;
}
// pattern: class|struct
static bool match80(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    return true;
}
// pattern: struct|class
static bool match81(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    return true;
}
// pattern: * (
static bool match82(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %type% (
static bool match83(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: , %name% ;|=|,
static bool match84(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: &|(
static bool match85(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: [;{}&(] *
static bool match86(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}&(", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: [;{}&(] * (
static bool match87(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}&(", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [;{}]
static bool match88(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}&(] * &
static bool match89(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}&(", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: [;{}&(] * ( &
static bool match90(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}&(", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: enum %name% {
static bool match91(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("enum")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: typedef
template<class T> static T * findmatch92(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("typedef")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: private|protected|public|__published : !!:
static bool match93(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("private")) || (tok->str()==MatchCompiler::makeConstString("protected")) || (tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("__published"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(":"))
        return false;
    return true;
}
// pattern: ( & %name% )
static bool match94(const Token* tok) {
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
    return true;
}
// pattern: [Lu] %char%|%str%
static bool match95(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("Lu", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType()==Token::eChar) || (tok->tokType()==Token::eString)))
        return false;
    return true;
}
// pattern: ?|:|,|(|[|{|return|case|sizeof|%op% +|-
static bool match96(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("sizeof")) || tok->isOp()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: - %num%
static bool match97(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: extern \"C\"
static bool match98(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("extern")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("\"C\"")))
        return false;
    return true;
}
// pattern: [;{}:] ( {
static bool match99(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}:", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } ) ;
static bool match100(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] %type% :
static bool match101(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: [;{}] %num%|%str% ;
static bool match102(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || (tok->tokType()==Token::eString)))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ( { %bool%|%char%|%num%|%str%|%name% ; } )
static bool match103(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isBoolean() || (tok->tokType()==Token::eChar) || tok->isNumber() || (tok->tokType()==Token::eString) || tok->isName()))
        return false;
    tok = tok->next();
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
// pattern: __CPPCHECK_EMBEDDED_SQL_EXEC__ SQL
static bool match104(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("__CPPCHECK_EMBEDDED_SQL_EXEC__")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("SQL")))
        return false;
    return true;
}
// pattern: %num% [ %name% ]
static bool match105(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: %name% ( void )
static bool match106(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("void")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: = {
static bool match107(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: { {
static bool match108(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } }
static bool match109(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: %var% [ ] =
static bool match110(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %var% [ ] = { %str% } ;
static bool match111(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %var% [ ] = %str% ;
static bool match112(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
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
// pattern: %var% [ ] = {
static bool match113(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
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
// pattern: {|(|[|<
static bool match114(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: [ %num% ]
static bool match115(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: [},]
static bool match116(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("},", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(<]
static bool match117(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(<", tok->str()[0]))
        return false;
    return true;
}
// pattern: [{};)]
static bool match118(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};)", tok->str()[0]))
        return false;
    return true;
}
// pattern: : %name% [({]
static bool match119(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("({", tok->str()[0]))
        return false;
    return true;
}
// pattern: [:,] %name% [({]
static bool match120(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(":,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("({", tok->str()[0]))
        return false;
    return true;
}
// pattern: (|[
static bool match121(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: [;{}:] case
static bool match122(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}:", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    return true;
}
// pattern: [:{};]
static bool match123(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(":{};", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}] %name% : !!;
static bool match124(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(";"))
        return false;
    return true;
}
// pattern: class|struct|enum
static bool match125(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    return true;
}
// pattern: case %num% . . . %num% :
static bool match126(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: case %char% . . . %char% :
static bool match127(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eChar))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eChar))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: %name% < sizeof ( %type% ) >
static bool match128(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    return true;
}
// pattern: namespace|public|private|protected
static bool match129(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("namespace")) || (tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("private")) || (tok->str()==MatchCompiler::makeConstString("protected"))))
        return false;
    return true;
}
// pattern: struct|union|enum
static bool match130(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    return true;
}
// pattern: class|typename
static bool match131(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("typename"))))
        return false;
    return true;
}
// pattern: void const| *|(
static bool match132(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("void")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: < >
static bool match133(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    return true;
}
// pattern: &|&&
static bool match134(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    return true;
}
// pattern: ( [*&]
static bool match135(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("*&", tok->str()[0]))
        return false;
    return true;
}
// pattern: ;|(|[|{
static bool match136(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: )|]
static bool match137(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]"))))
        return false;
    return true;
}
// pattern: ] (
static bool match138(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: (|=|{|:
static bool match139(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: = %num%
static bool match140(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: !!:: %type%
static bool match141(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("::"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %name% = { . %name% =
static bool match142(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: {|[|(
static bool match143(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: [,{] . %name% =
static bool match144(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",{", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: . %name% !!(
static bool match145(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("("))
        return false;
    return true;
}
// pattern: class|struct|enum %type% [:{]
static bool match146(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(":{", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name%|>|>> {|(
static bool match147(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: }|) ,|{
static bool match148(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: [,:] %name% [({]
static bool match149(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",:", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("({", tok->str()[0]))
        return false;
    return true;
}
// pattern: ::|.
static bool match150(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: ( * this ) .
static bool match151(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("this")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: %name% :: %name% ::
static bool match152(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: !!this .
static bool match153(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("this"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: {|}
static bool match154(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    return true;
}
// pattern: union|struct|enum {
static bool match155(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: %name%|>|>>
static bool match156(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    return true;
}
// pattern: } ,|{
static bool match157(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: ) %type%
static bool match158(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: sizeof (
static bool match159(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: const new
static bool match160(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    return true;
}
// pattern: %type% [;[=,)]
static bool match161(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";[=,)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %type% :
static bool match162(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: %type% ( !!)
static bool match163(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(")"))
        return false;
    return true;
}
// pattern: ) ;
static bool match164(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: struct|union|class %type%
static bool match165(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: [&*]
static bool match166(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("&*", tok->str()[0]))
        return false;
    return true;
}
// pattern: %type% {
static bool match167(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } ;
static bool match168(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: do|try|else
static bool match169(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("do")) || (tok->str()==MatchCompiler::makeConstString("try")) || (tok->str()==MatchCompiler::makeConstString("else"))))
        return false;
    return true;
}
// pattern: struct|class|:
static bool match170(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: %name%|::
static bool match171(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: struct|enum|union
static bool match172(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("enum")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    return true;
}
// pattern: %name% [,)]
static bool match173(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name%|*|&|,|(
static bool match174(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: [(,]
static bool match175(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] %type% %name%|*|&
static bool match176(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: %name%|*|&|,
static bool match177(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: [;{}] %type% %name%|*
static bool match178(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: ::|. %name%
static bool match179(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: class %type%
static bool match180(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("class")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %name% ;
static bool match181(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name% ::|<
static bool match182(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: > ::
static bool match183(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: ~| %name%
static bool match184(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ~| %name% (
static bool match185(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: using namespace %name% ::|;
static bool match186(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("using")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: namespace %name% {
static bool match187(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: !!:: %name% :: ~| %name%
static bool match188(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("::"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: !!:: %name% <
static bool match189(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("::"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > :: ~| %name%
static bool match190(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: :: ~| %name%
static bool match191(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% <
static bool match192(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: namespace|class|struct %name% {|:|::
static bool match193(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("namespace")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: :: %name%
static bool match194(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: :|::|,|%name%
static bool match195(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString(",")) || tok->isName()))
        return false;
    return true;
}
// pattern: %name% ,|{
static bool match196(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: {
static bool match197(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ) %name% (
static bool match198(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) : ::| %name%
static bool match199(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [)}] [,:]
static bool match200(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(")}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",:", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% :: %name%
static bool match201(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% (|{|<
static bool match202(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: )|} {
static bool match203(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: %name%|> %name% [:<]
static bool match204(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(":<", tok->str()[0]))
        return false;
    return true;
}
// pattern: }|]|)
static bool match205(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: %oror%|&&|;
static bool match206(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: && [,>]
static bool match207(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",>", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name%|%num%|%str%|%cop%|)|]
static bool match208(const Token* tok) {
    if (!tok || !(tok->isName() || tok->isNumber() || (tok->tokType()==Token::eString) || tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]"))))
        return false;
    return true;
}
// pattern: %name%|%num%|%str%|%cop%|(|[
static bool match209(const Token* tok) {
    if (!tok || !(tok->isName() || tok->isNumber() || (tok->tokType()==Token::eString) || tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %name%|>|&|&&|*|::|,|(|)|{|}|;|[|:
static bool match210(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: . . .
static bool match211(const Token* tok) {
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
// pattern: && %name% =
static bool match212(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: sizeof !!(
static bool match213(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("("))
        return false;
    return true;
}
// pattern: %name%|*|~|!
static bool match214(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("~")) || (tok->str()==MatchCompiler::makeConstString("!"))))
        return false;
    return true;
}
// pattern: * *
static bool match215(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: %name%|%num%|%str%|[|(|.|::|++|--|!|~
static bool match216(const Token* tok) {
    if (!tok || !(tok->isName() || tok->isNumber() || (tok->tokType()==Token::eString) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("!")) || (tok->str()==MatchCompiler::makeConstString("~"))))
        return false;
    return true;
}
// pattern: %type% * %op%|?|:|const|;|,
static bool match217(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isOp() || (tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: [;{}(,] %type% * %name% [;,)]
static bool match218(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}(,", tok->str()[0]))
        return false;
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
    if (!tok || tok->str().size()!=1U || !strchr(";,)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}(,] const %type% * %name% [;),]
static bool match219(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
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
    if (!tok || tok->str().size()!=1U || !strchr(";),", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}(,] %type% %name% [;),]
static bool match220(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";),", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}(,] const %type% %name% [;),]
static bool match221(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("const")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";),", tok->str()[0]))
        return false;
    return true;
}
// pattern: %type% %name% [ %num% ] [[;=]
static bool match222(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("[;=", tok->str()[0]))
        return false;
    return true;
}
// pattern: %type% * %name% [ %num% ] [[;=]
static bool match223(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("[;=", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;=]
static bool match224(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";=", tok->str()[0]))
        return false;
    return true;
}
// pattern: %type% %name% [ %num% ] [,)]
static bool match225(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %type% * %name% [ %num% ] [,)]
static bool match226(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: ( %char% )
static bool match227(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eChar))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ( %str% )
static bool match228(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ( %type% * )
static bool match229(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ( * )
static bool match230(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ( %var% )
static bool match231(const Token* tok) {
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
// pattern: ( %type% )
static bool match232(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ( %name% [
static bool match233(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ( *
static bool match234(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: %name% )
static bool match235(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %type%|* %name% [
static bool match236(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: %type% * %name%
static bool match237(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% [
static bool match238(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] [
static bool match239(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] [ %num% ]
static bool match240(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: if|for|while|BOOST_FOREACH %name% (
static bool match241(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("BOOST_FOREACH"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: for each
static bool match242(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("each")))
        return false;
    return true;
}
// pattern: if (
static bool match243(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) {|else
static bool match244(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("else"))))
        return false;
    return true;
}
// pattern: > struct {
static bool match245(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("struct")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: 0 [
static bool match246(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: %type% <
static bool match247(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: >|>> %name%|::|(
static bool match248(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: union|struct|class union|struct|class
static bool match249(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    return true;
}
// pattern: class %type% %type% [:{]
static bool match250(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("class")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(":{", tok->str()[0]))
        return false;
    return true;
}
// pattern: strlen ( %str% )
static bool match251(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strlen")))
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
    return true;
}
// pattern: return (
static bool match252(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) %type% {
static bool match253(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: const|namespace|class|struct|union|noexcept|override|final|volatile
static bool match254(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("namespace")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("noexcept")) || (tok->str()==MatchCompiler::makeConstString("override")) || (tok->str()==MatchCompiler::makeConstString("final")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        return false;
    return true;
}
// pattern: namespace|class|struct|union
static bool match255(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("namespace")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    return true;
}
// pattern: [;{}()]
static bool match256(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}()", tok->str()[0]))
        return false;
    return true;
}
// pattern: %type% :: %type%
static bool match257(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %type% :: %type% ::
static bool match258(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: %type% :: %type% (
static bool match259(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: class|struct %name% %name% {|:
static bool match260(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: [;{}] %name% (
static bool match261(const Token* tok) {
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
// pattern: const|static|struct|union|class
static bool match262(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("static")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    return true;
}
// pattern: class|struct %type% {|:
static bool match263(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: [;{}] %type% * %name% ;
static bool match264(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] %type% %name% ;
static bool match265(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] %name% = %char%|%num%|%name% ;
static bool match266(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType()==Token::eChar) || tok->isNumber() || tok->isName()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] %name% = %any% ;
static bool match267(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
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
// pattern: [;{}] %name% = realloc (
static bool match268(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("realloc")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( 0 ,
static bool match269(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: , 0 ) ;
static bool match270(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: (|[|{
static bool match271(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: ) %name% {
static bool match272(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ;|{|}|do {
static bool match273(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("do"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: continue|break ;
static bool match274(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: return|goto
static bool match275(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("goto"))))
        return false;
    return true;
}
// pattern: [{}]
static bool match276(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: if ( %bool% ) {
static bool match277(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isBoolean())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: else {
static bool match278(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: [;{}] for ( %name% = %num% ; %name% < %num% ; ++| %name% ++| ) {
static bool match279(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
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
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
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
// pattern: [;{}] for ( %type% %name% = %num% ; %name% < %num% ; ++| %name% ++| ) {
static bool match280(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
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
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
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
// pattern: ; ;
static bool match281(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ; { ; }
static bool match282(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: for|switch|BOOST_FOREACH
static bool match283(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("switch")) || (tok->str()==MatchCompiler::makeConstString("BOOST_FOREACH"))))
        return false;
    return true;
}
// pattern: }
static bool match284(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: )|}|,
static bool match285(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: try {
static bool match286(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("try")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } catch (
static bool match287(const Token* tok) {
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
// pattern: %name% : {
static bool match288(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ;|)|}
static bool match289(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    return true;
}
// pattern: [;{}] (| *| (| %name%
static bool match290(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: . %name%
static bool match291(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [|(
static bool match292(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: +=|-= 0 ;
static bool match293(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+=")) || (tok->str()==MatchCompiler::makeConstString("-="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: |= 0 ;
static bool match294(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("|=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: *=|/= 1 ;
static bool match295(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*=")) || (tok->str()==MatchCompiler::makeConstString("/="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;)]
static bool match296(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";)", tok->str()[0]))
        return false;
    return true;
}
// pattern: ]|)|}
static bool match297(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    return true;
}
// pattern: ! %bool%|%num%
static bool match298(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isBoolean() || tok->isNumber()))
        return false;
    return true;
}
// pattern: 0|false
static bool match299(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("0")) || (tok->str()==MatchCompiler::makeConstString("false"))))
        return false;
    return true;
}
// pattern: && true &&
static bool match300(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("true")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    return true;
}
// pattern: || false ||
static bool match301(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("false")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    return true;
}
// pattern: (|&& true && true &&|)
static bool match302(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("true")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("true")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: %oror%|( false %oror% false %oror%|)
static bool match303(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("false")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("false")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: ( true ||
static bool match304(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("true")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    return true;
}
// pattern: ( false &&
static bool match305(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("false")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    return true;
}
// pattern: || true )
static bool match306(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("true")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: && false )
static bool match307(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("false")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: && false &&
static bool match308(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("false")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    return true;
}
// pattern: || true ||
static bool match309(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("true")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    return true;
}
// pattern: if|while ( %num% )|%oror%|&&
static bool match310(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    return true;
}
// pattern: &&|%oror% %num% )|%oror%|&&
static bool match311(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    return true;
}
// pattern: &&|%oror%|(
static bool match312(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: %num% %any% %num%
static bool match313(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: %bool% %any% %bool%
static bool match314(const Token* tok) {
    if (!tok || !tok->isBoolean())
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !tok->isBoolean())
        return false;
    return true;
}
// pattern: &&|%oror%|)|?
static bool match315(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("?"))))
        return false;
    return true;
}
// pattern: <|=|,|(|[|{|}|;|case|return %bool%|%num%
static bool match316(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isBoolean() || tok->isNumber()))
        return false;
    return true;
}
// pattern: <|=|,|(|[|{|}|;|case|return ( %bool%|%num% )
static bool match317(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isBoolean() || tok->isNumber()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: false|0
static bool match318(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("false")) || (tok->str()==MatchCompiler::makeConstString("0"))))
        return false;
    return true;
}
// pattern: )|}|]|;|,|:|>
static bool match319(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: %name% [ ] ;|[
static bool match320(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: [ ] [;=[]
static bool match321(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";=[", tok->str()[0]))
        return false;
    return true;
}
// pattern: ) %name%|&
static bool match322(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: ) [
static bool match323(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ( %type% ) %num%
static bool match324(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: ( %type% *| *| *|&| ) *|&| %name%
static bool match325(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ( const| %type% * *| *|&| ) *|&| %name%
static bool match326(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ( const| %type% %type% *| *| *|&| ) *|&| %name%
static bool match327(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: ( %type% * *| *|&| ) (
static bool match328(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( const| %type% %type% * *| *|&| ) (
static bool match329(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% [|.
static bool match330(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: * &
static bool match331(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: ( %type% %type%| * *| ) 0
static bool match332(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->isName() && tok->varId()==0U && !tok->isKeyword())))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    return true;
}
// pattern: dynamic_cast|reinterpret_cast|const_cast|static_cast <
static bool match333(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("dynamic_cast")) || (tok->str()==MatchCompiler::makeConstString("reinterpret_cast")) || (tok->str()==MatchCompiler::makeConstString("const_cast")) || (tok->str()==MatchCompiler::makeConstString("static_cast"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: %name% ( %name% [,)]
static bool match334(const Token* tok) {
    if (!tok || !tok->isName())
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
// pattern: ; {
static bool match335(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: (|)
static bool match336(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: & %name% [ 0 ] !![
static bool match337(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
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
    if (tok && tok->str() == MatchCompiler::makeConstString("["))
        return false;
    return true;
}
// pattern: [,(=]
static bool match338(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",(=", tok->str()[0]))
        return false;
    return true;
}
// pattern: ) (
static bool match339(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( %type% %type%| *| *| ( * ) (
static bool match340(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->isName() && tok->varId()==0U && !tok->isKeyword())))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: static_cast < %type% %type%| *| *| ( * ) (
static bool match341(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("static_cast")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->isName() && tok->varId()==0U && !tok->isKeyword())))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) )|>
static bool match342(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: {|}|;|,|(|public:|protected:|private:
static bool match343(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("public:")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("private:"))))
        return false;
    return true;
}
// pattern: delete|else|return|throw|typedef
static bool match344(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("delete")) || (tok->str()==MatchCompiler::makeConstString("else")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("typedef"))))
        return false;
    return true;
}
// pattern: %type%|:: %type%|::
static bool match345(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: (|:: %type%
static bool match346(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: (|:: * *| %name%
static bool match347(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% ) (
static bool match348(const Token* tok) {
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
// pattern: %name% [ ] ) (
static bool match349(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) throw (
static bool match350(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("throw")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) const|volatile| const|volatile| ;|,|)|=|[|{
static bool match351(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: ( %type% ::
static bool match352(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: %name% ( ) { return %bool%|%char%|%num%|%str% ; }
static bool match353(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isBoolean() || (tok->tokType()==Token::eChar) || tok->isNumber() || (tok->tokType()==Token::eString)))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: (|[|=|return|%op% %name% ( ) ;|]|)|%cop%
static bool match354(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("return")) || tok->isOp()))
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
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: ) !!{
static bool match355(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("{"))
        return false;
    return true;
}
// pattern: [(,] [
static bool match356(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: {|}|;|)|public:|protected:|private:
static bool match357(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("public:")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("private:"))))
        return false;
    return true;
}
// pattern: template <
static bool match358(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("template")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: ::|extern| %type%
static bool match359(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("extern"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: else|return|public:|protected:|private:
static bool match360(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("else")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("public:")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("private:"))))
        return false;
    return true;
}
// pattern: ::|extern
static bool match361(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("extern"))))
        return false;
    return true;
}
// pattern: const|static
static bool match362(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("static"))))
        return false;
    return true;
}
// pattern: %type% const|static
static bool match363(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("static"))))
        return false;
    return true;
}
// pattern: & %name% ,
static bool match364(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: %type% *|&| %name% , %type% *|&| %name%
static bool match365(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: *
static bool match366(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: %type% %type%
static bool match367(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %name% ,|=
static bool match368(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: {|(|[
static bool match369(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: ] ,|=|[
static bool match370(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: =|,
static bool match371(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: %name% {
static bool match372(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ) ,
static bool match373(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: {|(
static bool match374(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: *|&|const
static bool match375(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("const"))))
        return false;
    return true;
}
// pattern: std| ::| %type%
static bool match376(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("std"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: std| ::| size_t|uintptr_t|uintmax_t
static bool match377(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("std"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("size_t")) || (tok->str()==MatchCompiler::makeConstString("uintptr_t")) || (tok->str()==MatchCompiler::makeConstString("uintmax_t"))))
        return false;
    return true;
}
// pattern: std| ::| ssize_t|ptrdiff_t|intptr_t|intmax_t
static bool match378(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("std"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("ssize_t")) || (tok->str()==MatchCompiler::makeConstString("ptrdiff_t")) || (tok->str()==MatchCompiler::makeConstString("intptr_t")) || (tok->str()==MatchCompiler::makeConstString("intmax_t"))))
        return false;
    return true;
}
// pattern: %type%|struct|::
static bool match379(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: private:|protected:|public:|operator
static bool match380(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("private:")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("public:")) || (tok->str()==MatchCompiler::makeConstString("operator"))))
        return false;
    return true;
}
// pattern: ;|{|}|(|,|private:|protected:|public:
static bool match381(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("private:")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("public:"))))
        return false;
    return true;
}
// pattern: if|while (
static bool match382(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: !| (| %name% =
static bool match383(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("!"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: !| (| %name% . %name% =
static bool match384(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("!"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: 0 == (| %name% =
static bool match385(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: 0 == (| %name% . %name% =
static bool match386(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: do
static bool match387(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("do")))
        return false;
    return true;
}
// pattern: !|0
static bool match388(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("!")) || (tok->str()==MatchCompiler::makeConstString("0"))))
        return false;
    return true;
}
// pattern: }|)|]|>
static bool match389(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: %name% = %name% = %num%|%name% ;
static bool match390(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || tok->isName()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name%
static bool match391(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name%|%num%|%char%|)|]|> %name% %name%|%num%|%char%|%op%|(
static bool match392(const Token* tok) {
    if (!tok || !(tok->isName() || tok->isNumber() || (tok->tokType()==Token::eChar) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || tok->isNumber() || (tok->tokType()==Token::eChar) || tok->isOp() || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: %assign%|%or%|%oror%|&&|*|/|%|^
static bool match393(const Token* tok) {
    if (!tok || !(tok->isAssignmentOp() || (tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") ) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("/")) || (tok->str()==MatchCompiler::makeConstString("%")) || (tok->str()==MatchCompiler::makeConstString("^"))))
        return false;
    return true;
}
// pattern: not|compl
static bool match394(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("not")) || (tok->str()==MatchCompiler::makeConstString("compl"))))
        return false;
    return true;
}
// pattern: %name%|(
static bool match395(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: class|struct|union| %type% *| %name% ( &| %any% ) ;
static bool match396(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
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
// pattern: %type% *| %name% ( %type% (
static bool match397(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: class|struct|union| %type% *| %name% ( &| %any% ) ,
static bool match398(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: class|struct|union
static bool match399(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    return true;
}
// pattern: %type%|* & %name% = %name% ;
static bool match400(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
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
// pattern: %type%|*|&
static bool match401(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: static| const| static| %type% const| %name% = %any% ;
static bool match402(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("static"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("static"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !tok->isName())
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
// pattern: static| const| static| %type% const| %name% ( %any% ) ;
static bool match403(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("static"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("static"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !tok->isName())
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
// pattern: ;|{|}|private:|protected:|public:
static bool match404(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("private:")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("public:"))))
        return false;
    return true;
}
// pattern: %bool%|%char%|%num%|%str% )| ;
static bool match405(const Token* tok) {
    if (!tok || !(tok->isBoolean() || (tok->tokType()==Token::eChar) || tok->isNumber() || (tok->tokType()==Token::eString)))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString(")"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: (|[|,|{|return|%op% & %varid%
static bool match406(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("return")) || tok->isOp()))
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
// pattern: %name%|* %name%|*
static bool match407(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: float|double
static bool match408(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("float")) || (tok->str()==MatchCompiler::makeConstString("double"))))
        return false;
    return true;
}
// pattern: %var% ;|[
static bool match409(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %var% ;
static bool match410(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %var% [
static bool match411(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: for (
static bool match412(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: * --|++
static bool match413(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("++"))))
        return false;
    return true;
}
// pattern: %name% = %bool%|%char%|%num%|%str%|%name% ;
static bool match414(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isBoolean() || (tok->tokType()==Token::eChar) || tok->isNumber() || (tok->tokType()==Token::eString) || tok->isName()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name% [ %num%| ] = %str% ;
static bool match415(const Token* tok) {
    if (!tok || !tok->isName())
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
// pattern: %name% = & %name% ;
static bool match416(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name% = & %name% [ 0 ] ;
static bool match417(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: for ( %type%
static bool match418(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %varid% = &| %varid%
static bool match419(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: [;{}] %name% .
static bool match420(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: strcpy|sprintf ( %name% , %str% ) ;
static bool match421(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("strcpy")) || (tok->str()==MatchCompiler::makeConstString("sprintf"))))
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
// pattern: %varid% = %num% ; %varid% <|<= %num% ; ++| %varid% ++| ) {
static bool match422(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
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
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("<="))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
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
// pattern: = &
static bool match423(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: & %name% [
static bool match424(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: %type% * %name% ; %name% = & %name% ;
static bool match425(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ; %type% : ;
static bool match426(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: break|continue
static bool match427(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue"))))
        return false;
    return true;
}
// pattern: ; }
static bool match428(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: ;
template<class T> static T * findmatch429(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: do|for|while
static bool match430(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("do")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    return true;
}
// pattern: if|for|while|switch|BOOST_FOREACH
static bool match431(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("switch")) || (tok->str()==MatchCompiler::makeConstString("BOOST_FOREACH"))))
        return false;
    return true;
}
// pattern: for|while|do
static bool match432(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("do"))))
        return false;
    return true;
}
// pattern: ++|-- %varid%
static bool match433(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: %varid% ++|--|=
static bool match434(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: %name% = realloc ( %name% ,
static bool match435(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("realloc")))
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
// pattern: ( %name% )
static bool match436(const Token* tok) {
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
// pattern: &&|(|%oror% %varid% &&|%oror%|)|;
static bool match437(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: [(,] %name% [,)]
static bool match438(const Token* tok) {
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
// pattern: . %name% (
static bool match439(const Token* tok) {
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
// pattern: return|= & %name% ;
static bool match440(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name% ( %name% ,|)
static bool match441(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: , %name% ,|)
static bool match442(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: ) { ++|--
static bool match443(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: ) { %name% ++|--
static bool match444(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: strlen|free ( %varid% )
static bool match445(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("strlen")) || (tok->str()==MatchCompiler::makeConstString("free"))))
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
// pattern: & %name% ;
static bool match446(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %varid% ;
static bool match447(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [ ] %varid%
static bool match448(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: %name%|::|*
static bool match449(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: ;|{|}|>>
static bool match450(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    return true;
}
// pattern: {|, %varid% ,|}
static bool match451(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    return true;
}
// pattern: ; %name% <|<=|!= %name% ; ++| %name% ++| )
static bool match452(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
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
    return true;
}
// pattern: %varid% ++|--
static bool match453(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: [{};] %any% %any% ;
static bool match454(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [.[]
static bool match455(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(".[", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}] %any% %any% ;
static bool match456(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: return %varid% %any%
static bool match457(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    return true;
}
// pattern: return * %varid% ;
static bool match458(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: else if
static bool match459(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("else")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    return true;
}
// pattern: (|{|[
static bool match460(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: }|;
static bool match461(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: ( {
static bool match462(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: ) %num%
static bool match463(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: [;{}=(] new (
static bool match464(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}=(", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("new")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) [;,{}[]
static bool match465(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";,{}[", tok->str()[0]))
        return false;
    return true;
}
// pattern: ! ( %name% )
static bool match466(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
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
// pattern: [(,;{}] ( %name% ) .
static bool match467(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,;{}", tok->str()[0]))
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
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: [(,;{}] ( %name% (
static bool match468(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,;{}", tok->str()[0]))
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
// pattern: [,;{}] ( delete [| ]| %name% ) ;
static bool match469(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",;{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("["))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("]"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: operator delete
static bool match470(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("operator")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    return true;
}
// pattern: delete|; (
static bool match471(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("delete")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [(!*;{}] ( %name% )
static bool match472(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(!*;{}", tok->str()[0]))
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
// pattern: [+-/=]
static bool match473(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("+-/=", tok->str()[0]))
        return false;
    return true;
}
// pattern: [;{}[(,!*] ( %name% .
static bool match474(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}[(,!*", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: ? (
static bool match475(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("?")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) :
static bool match476(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: %bool%|%num%|%name%
static bool match477(const Token* tok) {
    if (!tok || !(tok->isBoolean() || tok->isNumber() || tok->isName()))
        return false;
    return true;
}
// pattern: [{([,] ( !!{
static bool match478(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{([,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("{"))
        return false;
    return true;
}
// pattern: ) [;,])]
static bool match479(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(";,])", tok->str()[0]))
        return false;
    return true;
}
// pattern: operator ,
static bool match480(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("operator")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: ,
template<class T> static T * findmatch481(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: , (
static bool match482(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) =
static bool match483(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: ( %bool%|%num% ) %cop%|;|,|)
static bool match484(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isBoolean() || tok->isNumber()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: %name%|)
static bool match485(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: >|>>
static bool match486(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    return true;
}
// pattern: *|& ( %name% )
static bool match487(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
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
// pattern: %type%|static|const|extern
static bool match488(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("static")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("extern"))))
        return false;
    return true;
}
// pattern: [;,{]
static bool match489(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";,{", tok->str()[0]))
        return false;
    return true;
}
// pattern: %str% [ %num% ]
static bool match490(const Token* tok) {
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: [;{}] %type% & %name% (|= %name% )| ;
static bool match491(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
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
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString(")"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ]|)|++|--
static bool match492(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: * ( %name% +|- %num%|%name% )
static bool match493(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || tok->isName()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %num%|%name%|]|)
static bool match494(const Token* tok) {
    if (!tok || !(tok->isNumber() || tok->isName() || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: & %name% [ %num%|%name% ] !![
static bool match495(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || tok->isName()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("["))
        return false;
    return true;
}
// pattern: [;{}] strcat ( strcat (
static bool match496(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strcat")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strcat")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: strcat ( strcat
static bool match497(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strcat")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strcat")))
        return false;
    return true;
}
// pattern: ;|,|[|=|)|>
static bool match498(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: (|,
static bool match499(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: :|,
static bool match500(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: %type% &|*
static bool match501(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: enum|,
static bool match502(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("enum")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: [(,;{}]
static bool match503(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,;{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: return|throw
static bool match504(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: [(,{};] std :: %name% (
static bool match505(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ; }
static bool match506(const Token* tok) {
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
// pattern: [;{}=] %name% (
static bool match507(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}=", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: goto %name% ;
static bool match508(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("goto")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [{};] switch (
static bool match509(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("switch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [{};:] case
static bool match510(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};:", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("case")))
        return false;
    return true;
}
// pattern: : ;
template<class T> static T * findmatch511(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: [{};] default : ;
static bool match512(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("default")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [{};] %name% : ;
static bool match513(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [{};] do|while|for|BOOST_FOREACH
static bool match514(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("do")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("BOOST_FOREACH"))))
        return false;
    return true;
}
// pattern: atol ( %str% )
static bool match515(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("atol")))
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
    return true;
}
// pattern: std ::
static bool match516(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: sqrt|sqrtf|sqrtl|cbrt|cbrtf|cbrtl ( %num% )
static bool match517(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sqrt")) || (tok->str()==MatchCompiler::makeConstString("sqrtf")) || (tok->str()==MatchCompiler::makeConstString("sqrtl")) || (tok->str()==MatchCompiler::makeConstString("cbrt")) || (tok->str()==MatchCompiler::makeConstString("cbrtf")) || (tok->str()==MatchCompiler::makeConstString("cbrtl"))))
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
// pattern: exp|expf|expl|exp2|exp2f|exp2l|cos|cosf|cosl|cosh|coshf|coshl|erfc|erfcf|erfcl ( %num% )
static bool match518(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("exp")) || (tok->str()==MatchCompiler::makeConstString("expf")) || (tok->str()==MatchCompiler::makeConstString("expl")) || (tok->str()==MatchCompiler::makeConstString("exp2")) || (tok->str()==MatchCompiler::makeConstString("exp2f")) || (tok->str()==MatchCompiler::makeConstString("exp2l")) || (tok->str()==MatchCompiler::makeConstString("cos")) || (tok->str()==MatchCompiler::makeConstString("cosf")) || (tok->str()==MatchCompiler::makeConstString("cosl")) || (tok->str()==MatchCompiler::makeConstString("cosh")) || (tok->str()==MatchCompiler::makeConstString("coshf")) || (tok->str()==MatchCompiler::makeConstString("coshl")) || (tok->str()==MatchCompiler::makeConstString("erfc")) || (tok->str()==MatchCompiler::makeConstString("erfcf")) || (tok->str()==MatchCompiler::makeConstString("erfcl"))))
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
// pattern: log1p|log1pf|log1pl|sin|sinf|sinl|sinh|sinhf|sinhl|erf|erff|erfl|asin|asinf|asinl|asinh|asinhf|asinhl|tan|tanf|tanl|tanh|tanhf|tanhl|atan|atanf|atanl|atanh|atanhf|atanhl|expm1|expm1f|expm1l ( %num% )
static bool match519(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("log1p")) || (tok->str()==MatchCompiler::makeConstString("log1pf")) || (tok->str()==MatchCompiler::makeConstString("log1pl")) || (tok->str()==MatchCompiler::makeConstString("sin")) || (tok->str()==MatchCompiler::makeConstString("sinf")) || (tok->str()==MatchCompiler::makeConstString("sinl")) || (tok->str()==MatchCompiler::makeConstString("sinh")) || (tok->str()==MatchCompiler::makeConstString("sinhf")) || (tok->str()==MatchCompiler::makeConstString("sinhl")) || (tok->str()==MatchCompiler::makeConstString("erf")) || (tok->str()==MatchCompiler::makeConstString("erff")) || (tok->str()==MatchCompiler::makeConstString("erfl")) || (tok->str()==MatchCompiler::makeConstString("asin")) || (tok->str()==MatchCompiler::makeConstString("asinf")) || (tok->str()==MatchCompiler::makeConstString("asinl")) || (tok->str()==MatchCompiler::makeConstString("asinh")) || (tok->str()==MatchCompiler::makeConstString("asinhf")) || (tok->str()==MatchCompiler::makeConstString("asinhl")) || (tok->str()==MatchCompiler::makeConstString("tan")) || (tok->str()==MatchCompiler::makeConstString("tanf")) || (tok->str()==MatchCompiler::makeConstString("tanl")) || (tok->str()==MatchCompiler::makeConstString("tanh")) || (tok->str()==MatchCompiler::makeConstString("tanhf")) || (tok->str()==MatchCompiler::makeConstString("tanhl")) || (tok->str()==MatchCompiler::makeConstString("atan")) || (tok->str()==MatchCompiler::makeConstString("atanf")) || (tok->str()==MatchCompiler::makeConstString("atanl")) || (tok->str()==MatchCompiler::makeConstString("atanh")) || (tok->str()==MatchCompiler::makeConstString("atanhf")) || (tok->str()==MatchCompiler::makeConstString("atanhl")) || (tok->str()==MatchCompiler::makeConstString("expm1")) || (tok->str()==MatchCompiler::makeConstString("expm1f")) || (tok->str()==MatchCompiler::makeConstString("expm1l"))))
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
// pattern: log2|log2f|log2l|log|logf|logl|log10|log10f|log10l|logb|logbf|logbl|acosh|acoshf|acoshl|acos|acosf|acosl|ilogb|ilogbf|ilogbl ( %num% )
static bool match520(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("log2")) || (tok->str()==MatchCompiler::makeConstString("log2f")) || (tok->str()==MatchCompiler::makeConstString("log2l")) || (tok->str()==MatchCompiler::makeConstString("log")) || (tok->str()==MatchCompiler::makeConstString("logf")) || (tok->str()==MatchCompiler::makeConstString("logl")) || (tok->str()==MatchCompiler::makeConstString("log10")) || (tok->str()==MatchCompiler::makeConstString("log10f")) || (tok->str()==MatchCompiler::makeConstString("log10l")) || (tok->str()==MatchCompiler::makeConstString("logb")) || (tok->str()==MatchCompiler::makeConstString("logbf")) || (tok->str()==MatchCompiler::makeConstString("logbl")) || (tok->str()==MatchCompiler::makeConstString("acosh")) || (tok->str()==MatchCompiler::makeConstString("acoshf")) || (tok->str()==MatchCompiler::makeConstString("acoshl")) || (tok->str()==MatchCompiler::makeConstString("acos")) || (tok->str()==MatchCompiler::makeConstString("acosf")) || (tok->str()==MatchCompiler::makeConstString("acosl")) || (tok->str()==MatchCompiler::makeConstString("ilogb")) || (tok->str()==MatchCompiler::makeConstString("ilogbf")) || (tok->str()==MatchCompiler::makeConstString("ilogbl"))))
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
// pattern: fmin|fminl|fminf ( %num% , %num% )
static bool match521(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("fmin")) || (tok->str()==MatchCompiler::makeConstString("fminl")) || (tok->str()==MatchCompiler::makeConstString("fminf"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
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
// pattern: fmax|fmaxl|fmaxf ( %num% , %num% )
static bool match522(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("fmax")) || (tok->str()==MatchCompiler::makeConstString("fmaxl")) || (tok->str()==MatchCompiler::makeConstString("fmaxf"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
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
// pattern: pow|powf|powl (
static bool match523(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("pow")) || (tok->str()==MatchCompiler::makeConstString("powf")) || (tok->str()==MatchCompiler::makeConstString("powl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %num% , %num% )
static bool match524(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
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
// pattern: %any% , %num% )
static bool match525(const Token* tok) {
    if (!tok || false)
        return false;
    tok = tok->next();
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
// pattern: enum class|struct| %name%| :|{
static bool match526(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("enum")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        tok = tok->next();
    if (tok && (tok->isName()))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: %name%|= {
static bool match527(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: = (
static bool match528(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [;{}?:]
static bool match529(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}?:", tok->str()[0]))
        return false;
    return true;
}
// pattern: delete %name% , %name% ;
static bool match530(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: delete %name%
static bool match531(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: delete [ ] %name%
static bool match532(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [?:;,{}()]
static bool match533(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("?:;,{}()", tok->str()[0]))
        return false;
    return true;
}
// pattern: (|{
static bool match534(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: const_cast|dynamic_cast|reinterpret_cast|static_cast <
static bool match535(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const_cast")) || (tok->str()==MatchCompiler::makeConstString("dynamic_cast")) || (tok->str()==MatchCompiler::makeConstString("reinterpret_cast")) || (tok->str()==MatchCompiler::makeConstString("static_cast"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: %name% < %name% > (
static bool match536(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [()]
static bool match537(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("()", tok->str()[0]))
        return false;
    return true;
}
// pattern: using namespace %name% ;
static bool match538(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("using")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: template < class|typename %name% [,>]
static bool match539(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("template")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("typename"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",>", tok->str()[0]))
        return false;
    return true;
}
// pattern: class|namespace %name% [:{]
static bool match540(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("namespace"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(":{", tok->str()[0]))
        return false;
    return true;
}
// pattern: [{([]
static bool match541(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{([", tok->str()[0]))
        return false;
    return true;
}
// pattern: [})]]
static bool match542(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("})]", tok->str()[0]))
        return false;
    return true;
}
// pattern: if|while|for|switch
static bool match543(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("switch"))))
        return false;
    return true;
}
// pattern: %name%|:|;|{|}|(|)|,
static bool match544(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: ( !!)
static bool match545(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(")"))
        return false;
    return true;
}
// pattern: switch (
static bool match546(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("switch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ;|}
static bool match547(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    return true;
}
// pattern: > %cop%
static bool match548(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isConstOp())
        return false;
    return true;
}
// pattern: %assign% typename|class %assign%
static bool match549(const Token* tok) {
    if (!tok || !tok->isAssignmentOp())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("typename")) || (tok->str()==MatchCompiler::makeConstString("class"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: %cop%|=|,|[ %or%|%oror%|/|%
static bool match550(const Token* tok) {
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") ) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("/")) || (tok->str()==MatchCompiler::makeConstString("%"))))
        return false;
    return true;
}
// pattern: ;|(|[ %comp%
static bool match551(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    return true;
}
// pattern: %cop%|= ]
static bool match552(const Token* tok) {
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: [|, &|= ]
static bool match553(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: %cop%
static bool match554(const Token* tok) {
    if (!tok || !tok->isConstOp())
        return false;
    return true;
}
// pattern: %name%|;|}|)
static bool match555(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: void|char|short|int|long|float|double|const|volatile|static|inline|struct|class|enum|union|template|sizeof|case|break|continue|typedef
static bool match556(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("void")) || (tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("short")) || (tok->str()==MatchCompiler::makeConstString("int")) || (tok->str()==MatchCompiler::makeConstString("long")) || (tok->str()==MatchCompiler::makeConstString("float")) || (tok->str()==MatchCompiler::makeConstString("double")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile")) || (tok->str()==MatchCompiler::makeConstString("static")) || (tok->str()==MatchCompiler::makeConstString("inline")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("enum")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("template")) || (tok->str()==MatchCompiler::makeConstString("sizeof")) || (tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("typedef"))))
        return false;
    return true;
}
// pattern: [:;{})>]
static bool match557(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(":;{})>", tok->str()[0]))
        return false;
    return true;
}
// pattern: >|>> ::| %name%
static bool match558(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: >|>> [ [ %name%
static bool match559(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [;{}] [ %name% %name% ] ;
static bool match560(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [{};] while ( 0|false )
static bool match561(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("0")) || (tok->str()==MatchCompiler::makeConstString("false"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [{};] for ( %name% = %num% ; %name% < %num% ;
static bool match562(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
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
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [{};] for ( %type% %name% = %num% ; %name% < %num% ;
static bool match563(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
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
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: continue|break
template<class T> static T * findmatch564(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: for ( %name% =
static bool match565(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: && errno == EINTR ) { ;| }
static bool match566(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("errno")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("EINTR")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString(";"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: && ( errno == EINTR ) ) { ;| }
static bool match567(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("errno")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("==")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("EINTR")))
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
    if (tok && ((tok->str()==MatchCompiler::makeConstString(";"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    return true;
}
// pattern: while (
static bool match568(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: while ( %name% ( %name% ) ) {
static bool match569(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("while")))
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
    tok = tok->next();
    if (!tok || !tok->isName())
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
    return true;
}
// pattern: struct|union {
static bool match570(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } *|&| %type% ,|;|[|(|{
static bool match571(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: class|struct :
static bool match572(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: {
template<class T> static T * findmatch573(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: } %type%| ,|;|[|(|{
static bool match574(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (tok && ((tok->isName() && tok->varId()==0U && !tok->isKeyword())))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: enum {
static bool match575(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("enum")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: enum : %type% {
static bool match576(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("enum")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: const|)
static bool match577(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: class|struct|union|enum %type% :|{
static bool match578(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: *|&| %type% ,|;|[|=|(|{
static bool match579(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: struct|class|union|enum
static bool match580(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
        return false;
    return true;
}
// pattern: %type% %name% ;
static bool match581(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: __cdecl|__stdcall|__fastcall|__thiscall|__clrcall|__syscall|__pascal|__fortran|__far|__near
static bool match582(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("__cdecl")) || (tok->str()==MatchCompiler::makeConstString("__stdcall")) || (tok->str()==MatchCompiler::makeConstString("__fastcall")) || (tok->str()==MatchCompiler::makeConstString("__thiscall")) || (tok->str()==MatchCompiler::makeConstString("__clrcall")) || (tok->str()==MatchCompiler::makeConstString("__syscall")) || (tok->str()==MatchCompiler::makeConstString("__pascal")) || (tok->str()==MatchCompiler::makeConstString("__fortran")) || (tok->str()==MatchCompiler::makeConstString("__far")) || (tok->str()==MatchCompiler::makeConstString("__near"))))
        return false;
    return true;
}
// pattern: WINAPI|APIENTRY|CALLBACK
static bool match583(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("WINAPI")) || (tok->str()==MatchCompiler::makeConstString("APIENTRY")) || (tok->str()==MatchCompiler::makeConstString("CALLBACK"))))
        return false;
    return true;
}
// pattern: __declspec|_declspec (
static bool match584(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("__declspec")) || (tok->str()==MatchCompiler::makeConstString("_declspec"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: noreturn|nothrow
static bool match585(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("noreturn")) || (tok->str()==MatchCompiler::makeConstString("nothrow"))))
        return false;
    return true;
}
// pattern: __attribute__|__attribute (
static bool match586(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("__attribute__")) || (tok->str()==MatchCompiler::makeConstString("__attribute"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( constructor|__constructor__
static bool match587(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("constructor")) || (tok->str()==MatchCompiler::makeConstString("__constructor__"))))
        return false;
    return true;
}
// pattern: ( destructor|__destructor__
static bool match588(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("destructor")) || (tok->str()==MatchCompiler::makeConstString("__destructor__"))))
        return false;
    return true;
}
// pattern: ( unused|__unused__|used|__used__ )
static bool match589(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("unused")) || (tok->str()==MatchCompiler::makeConstString("__unused__")) || (tok->str()==MatchCompiler::makeConstString("used")) || (tok->str()==MatchCompiler::makeConstString("__used__"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ;|=
static bool match590(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("="))))
        return false;
    return true;
}
// pattern: ( pure|__pure__|const|__const__|noreturn|__noreturn__|nothrow|__nothrow__|warn_unused_result )
static bool match591(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("pure")) || (tok->str()==MatchCompiler::makeConstString("__pure__")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("__const__")) || (tok->str()==MatchCompiler::makeConstString("noreturn")) || (tok->str()==MatchCompiler::makeConstString("__noreturn__")) || (tok->str()==MatchCompiler::makeConstString("nothrow")) || (tok->str()==MatchCompiler::makeConstString("__nothrow__")) || (tok->str()==MatchCompiler::makeConstString("warn_unused_result"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ) __attribute__|__attribute (
static bool match592(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("__attribute__")) || (tok->str()==MatchCompiler::makeConstString("__attribute"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: } __attribute__ ( ( packed )
static bool match593(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("__attribute__")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("packed")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [ [ %name%
static bool match594(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: noreturn|nodiscard
static bool match595(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("noreturn")) || (tok->str()==MatchCompiler::makeConstString("nodiscard"))))
        return false;
    return true;
}
// pattern: %name%|::|*|&
static bool match596(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: .
static bool match597(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: [ static %num%
static bool match598(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("static")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: %type% final [:{]
static bool match599(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("final")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(":{", tok->str()[0]))
        return false;
    return true;
}
// pattern: ) noexcept :|{|;|const|override|final
static bool match600(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("noexcept")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("override")) || (tok->str()==MatchCompiler::makeConstString("final"))))
        return false;
    return true;
}
// pattern: [;{}] %name% ( %name% =
static bool match601(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: assert|while
static bool match602(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("assert")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    return true;
}
// pattern: )|,
static bool match603(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: [;{}] %name% = ( {
static bool match604(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: )|}
static bool match605(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("}"))))
        return false;
    return true;
}
// pattern: %type%|*
static bool match606(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || (tok->str()==MatchCompiler::makeConstString("*"))))
        return false;
    return true;
}
// pattern: } )
static bool match607(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [;{}] %num%|%name% ;
static bool match608(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || tok->isName()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: __asm|_asm|asm {
static bool match609(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("__asm")) || (tok->str()==MatchCompiler::makeConstString("_asm")) || (tok->str()==MatchCompiler::makeConstString("asm"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: asm|__asm|__asm__ volatile|__volatile|__volatile__| (
static bool match610(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("asm")) || (tok->str()==MatchCompiler::makeConstString("__asm")) || (tok->str()==MatchCompiler::makeConstString("__asm__"))))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("volatile")) || (tok->str()==MatchCompiler::makeConstString("__volatile")) || (tok->str()==MatchCompiler::makeConstString("__volatile__"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: _asm|__asm
static bool match611(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("_asm")) || (tok->str()==MatchCompiler::makeConstString("__asm"))))
        return false;
    return true;
}
// pattern: %num%|%name%|,|:|;
static bool match612(const Token* tok) {
    if (!tok || !(tok->isNumber() || tok->isName() || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: _asm|__asm|__endasm
static bool match613(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("_asm")) || (tok->str()==MatchCompiler::makeConstString("__asm")) || (tok->str()==MatchCompiler::makeConstString("__endasm"))))
        return false;
    return true;
}
// pattern: __endasm
static bool match614(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("__endasm")))
        return false;
    return true;
}
// pattern: ;
static bool match615(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: ^ {
static bool match616(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("^")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: [,(;{}=]
static bool match617(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",(;{}=", tok->str()[0]))
        return false;
    return true;
}
// pattern: )|]|>
static bool match618(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: %cop%|,|;|{|}|)
static bool match619(const Token* tok) {
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: ;|{|}|public:|protected:|private:
static bool match620(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("public:")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("private:"))))
        return false;
    return true;
}
// pattern: const| %type% %name% :
static bool match621(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: case|public|protected|private|class|struct
static bool match622(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("protected")) || (tok->str()==MatchCompiler::makeConstString("private")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    return true;
}
// pattern: default :
static bool match623(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("default")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: %name% : %num% ;
static bool match624(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %bool%|%num%
static bool match625(const Token* tok) {
    if (!tok || !(tok->isBoolean() || tok->isNumber()))
        return false;
    return true;
}
// pattern: public|protected|private| %type% ::|<|,|{|;
static bool match626(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("protected")) || (tok->str()==MatchCompiler::makeConstString("private"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: [;,)]{}]
static bool match627(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";,)]{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: [([]
static bool match628(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("([", tok->str()[0]))
        return false;
    return true;
}
// pattern: } %name%| : %num% ;
static bool match629(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("}")))
        return false;
    tok = tok->next();
    if (tok && (tok->isName()))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: const| %type% : %num%|%bool% ;
static bool match630(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || tok->isBoolean()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [{};()]
static bool match631(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};()", tok->str()[0]))
        return false;
    return true;
}
// pattern: using namespace std ;
template<class T> static T * findmatch632(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("using")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: .|::
static bool match633(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: (|<
static bool match634(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: !!:: tr1 ::
static bool match635(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("::"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("tr1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: std :: tr1 ::
static bool match636(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("tr1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: using namespace std ;
static bool match637(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("using")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: CopyMemory|RtlCopyMemory|RtlCopyBytes
static bool match638(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("CopyMemory")) || (tok->str()==MatchCompiler::makeConstString("RtlCopyMemory")) || (tok->str()==MatchCompiler::makeConstString("RtlCopyBytes"))))
        return false;
    return true;
}
// pattern: MoveMemory|RtlMoveMemory
static bool match639(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("MoveMemory")) || (tok->str()==MatchCompiler::makeConstString("RtlMoveMemory"))))
        return false;
    return true;
}
// pattern: FillMemory|RtlFillMemory|RtlFillBytes
static bool match640(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("FillMemory")) || (tok->str()==MatchCompiler::makeConstString("RtlFillMemory")) || (tok->str()==MatchCompiler::makeConstString("RtlFillBytes"))))
        return false;
    return true;
}
// pattern: ZeroMemory|RtlZeroMemory|RtlZeroBytes|RtlSecureZeroMemory
static bool match641(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("ZeroMemory")) || (tok->str()==MatchCompiler::makeConstString("RtlZeroMemory")) || (tok->str()==MatchCompiler::makeConstString("RtlZeroBytes")) || (tok->str()==MatchCompiler::makeConstString("RtlSecureZeroMemory"))))
        return false;
    return true;
}
// pattern: RtlCompareMemory
static bool match642(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("RtlCompareMemory")))
        return false;
    return true;
}
// pattern: _T|_TEXT|TEXT ( %char%|%str% )
static bool match643(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("_T")) || (tok->str()==MatchCompiler::makeConstString("_TEXT")) || (tok->str()==MatchCompiler::makeConstString("TEXT"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType()==Token::eChar) || (tok->tokType()==Token::eString)))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ( __closure * %name% )
static bool match644(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("__closure")))
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
// pattern: namespace %type%
static bool match645(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: class %name% :|{
static bool match646(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("class")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: ;|{|}|protected:|public:|__published:
static bool match647(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("protected:")) || (tok->str()==MatchCompiler::makeConstString("public:")) || (tok->str()==MatchCompiler::makeConstString("__published:"))))
        return false;
    return true;
}
// pattern: {|;
static bool match648(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: emit|Q_EMIT %name% (
static bool match649(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("emit")) || (tok->str()==MatchCompiler::makeConstString("Q_EMIT"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: class %name% :
static bool match650(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("class")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: public|protected|private slots|Q_SLOTS :
static bool match651(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("public")) || (tok->str()==MatchCompiler::makeConstString("protected")) || (tok->str()==MatchCompiler::makeConstString("private"))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("slots")) || (tok->str()==MatchCompiler::makeConstString("Q_SLOTS"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: signals|Q_SIGNALS :
static bool match652(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("signals")) || (tok->str()==MatchCompiler::makeConstString("Q_SIGNALS"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(":")))
        return false;
    return true;
}
// pattern: [=;{),]
static bool match653(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=;{),", tok->str()[0]))
        return false;
    return true;
}
// pattern: using|:: operator %op% ;
static bool match654(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("using")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("operator")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: :: %name%|%op%|.
static bool match655(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || tok->isOp() || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: .|%op%|,
static bool match656(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || tok->isOp() || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: [ ]
static bool match657(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: ( *| )
static bool match658(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: operator
template<class T> static T * findmatch659(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("operator")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: class|struct|namespace %type% :|{
static bool match660(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("namespace"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: %type% :: ~| %type% (
static bool match661(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %type% :: operator
static bool match662(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("operator")))
        return false;
    return true;
}
// pattern: ) const| {|;|:
static bool match663(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: %type% :: ~| %type%
static bool match664(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: %type% * *|&
static bool match665(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: return strncat (
static bool match666(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strncat")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: sin|sinf|sinl (
static bool match667(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sin")) || (tok->str()==MatchCompiler::makeConstString("sinf")) || (tok->str()==MatchCompiler::makeConstString("sinl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) , %num% ) + pow|powf|powl ( cos|cosf|cosl (
static bool match668(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("pow")) || (tok->str()==MatchCompiler::makeConstString("powf")) || (tok->str()==MatchCompiler::makeConstString("powl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("cos")) || (tok->str()==MatchCompiler::makeConstString("cosf")) || (tok->str()==MatchCompiler::makeConstString("cosl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) , %num% )
static bool match669(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
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
// pattern: cos|cosf|cosl (
static bool match670(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("cos")) || (tok->str()==MatchCompiler::makeConstString("cosf")) || (tok->str()==MatchCompiler::makeConstString("cosl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) , %num% ) + pow|powf|powl ( sin|sinf|sinl (
static bool match671(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("pow")) || (tok->str()==MatchCompiler::makeConstString("powf")) || (tok->str()==MatchCompiler::makeConstString("powl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sin")) || (tok->str()==MatchCompiler::makeConstString("sinf")) || (tok->str()==MatchCompiler::makeConstString("sinl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: sinh|sinhf|sinhl (
static bool match672(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sinh")) || (tok->str()==MatchCompiler::makeConstString("sinhf")) || (tok->str()==MatchCompiler::makeConstString("sinhl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) , %num% ) - pow|powf|powl ( cosh|coshf|coshl (
static bool match673(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("pow")) || (tok->str()==MatchCompiler::makeConstString("powf")) || (tok->str()==MatchCompiler::makeConstString("powl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("cosh")) || (tok->str()==MatchCompiler::makeConstString("coshf")) || (tok->str()==MatchCompiler::makeConstString("coshl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: cosh|coshf|coshl (
static bool match674(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("cosh")) || (tok->str()==MatchCompiler::makeConstString("coshf")) || (tok->str()==MatchCompiler::makeConstString("coshl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) , %num% ) - pow|powf|powl ( sinh|sinhf|sinhl (
static bool match675(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("pow")) || (tok->str()==MatchCompiler::makeConstString("powf")) || (tok->str()==MatchCompiler::makeConstString("powl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sinh")) || (tok->str()==MatchCompiler::makeConstString("sinhf")) || (tok->str()==MatchCompiler::makeConstString("sinhl"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [|(|<
static bool match676(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: ;|{|}|,|(
static bool match677(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: && [,)]
static bool match678(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: END - __CPPCHECK_EMBEDDED_SQL_EXEC__ ;
static bool match679(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("END")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("__CPPCHECK_EMBEDDED_SQL_EXEC__")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: {|}|==|&&|!|^|<<|>>|++|+=|-=|/=|*=|>>=|<<=|~
static bool match680(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("!")) || (tok->str()==MatchCompiler::makeConstString("^")) || (tok->str()==MatchCompiler::makeConstString("<<")) || (tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("+=")) || (tok->str()==MatchCompiler::makeConstString("-=")) || (tok->str()==MatchCompiler::makeConstString("/=")) || (tok->str()==MatchCompiler::makeConstString("*=")) || (tok->str()==MatchCompiler::makeConstString(">>=")) || (tok->str()==MatchCompiler::makeConstString("<<=")) || (tok->str()==MatchCompiler::makeConstString("~"))))
        return false;
    return true;
}
// pattern: namespace %name% ::
static bool match681(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: namespace %name% =
static bool match682(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("namespace")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
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
#include "tokenize.h"

#include "check.h"
#include "library.h"
#include "mathlib.h"
#include "path.h"
#include "platform.h"
#include "settings.h"
#include "standards.h"
#include "symboldatabase.h"
#include "templatesimplifier.h"
#include "timer.h"
#include "token.h"
#include "utils.h"
#include "valueflow.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>
//---------------------------------------------------------------------------

namespace {
    // local struct used in setVarId
    // in order to store information about the scope
    struct VarIdScopeInfo {
        VarIdScopeInfo()
            :isExecutable(false), isStructInit(false), isEnum(false), startVarid(0) {
        }
        VarIdScopeInfo(bool _isExecutable, bool _isStructInit, bool _isEnum, unsigned int _startVarid)
            :isExecutable(_isExecutable), isStructInit(_isStructInit), isEnum(_isEnum), startVarid(_startVarid) {
        }

        const bool isExecutable;
        const bool isStructInit;
        const bool isEnum;
        const unsigned int startVarid;
    };
}

/** Return whether tok is the "{" that starts an enumerator list */
static bool isEnumStart(const Token* tok)
{
    if (!tok || tok->str() != MatchCompiler::makeConstString("{"))
        return false;
    return (tok->strAt(-1) == MatchCompiler::makeConstString("enum")) || (tok->strAt(-2) == MatchCompiler::makeConstString("enum"));
}

template<typename T>
static void skipEnumBody(T **tok)
{
    T *defStart = *tok;
    while (match1(defStart))
        defStart = defStart->next();
    if (defStart && defStart->str() == MatchCompiler::makeConstString("{"))
        *tok = defStart->link()->next();
}

const Token * Tokenizer::isFunctionHead(const Token *tok, const std::string &endsWith) const
{
    return Tokenizer::isFunctionHead(tok, endsWith, isCPP());
}

const Token * Tokenizer::isFunctionHead(const Token *tok, const std::string &endsWith, bool cpp)
{
    if (!tok)
        return nullptr;
    if (tok->str() == MatchCompiler::makeConstString("("))
        tok = tok->link();
    if (match2(tok)) {
        tok = tok->next();
        while (tok && tok->str() == MatchCompiler::makeConstString("[") && tok->link())
            tok = tok->link()->next();
        return (tok && endsWith.find(tok->str()) != std::string::npos) ? tok : nullptr;
    }
    if (cpp && tok->str() == MatchCompiler::makeConstString(")")) {
        tok = tok->next();
        while (match3(tok) ||
               (match4(tok) && tok->isUpperCaseName()))
            tok = tok->next();
        if (tok && tok->str() == MatchCompiler::makeConstString(")"))
            tok = tok->next();
        while (tok && tok->str() == MatchCompiler::makeConstString("["))
            tok = tok->link()->next();
        if (match5(tok))
            tok = tok->linkAt(1)->next();
        if (match6(tok) && tok->isUpperCaseName())
            tok = tok->linkAt(1)->next();
        if (tok && tok->str() == MatchCompiler::makeConstString(".")) { // trailing return type
            for (tok = tok->next(); tok && !match7(tok); tok = tok->next())
                if (tok->link() && match8(tok))
                    tok = tok->link();
        }
        if (match9(tok))
            tok = tok->tokAt(2);
        return (tok && endsWith.find(tok->str()) != std::string::npos) ? tok : nullptr;
    }
    return nullptr;
}

/**
 * is tok the start brace { of a class, struct, union, or enum
 */
static bool isClassStructUnionEnumStart(const Token * tok)
{
    if (!match10(tok->previous()))
        return false;
    const Token * tok2 = tok->previous();
    while (tok2 && !match11(tok2))
        tok2 = tok2->previous();
    return match12(tok2);
}

//---------------------------------------------------------------------------

Tokenizer::Tokenizer() :
    list(nullptr),
    mSettings(nullptr),
    mErrorLogger(nullptr),
    mSymbolDatabase(nullptr),
    mTemplateSimplifier(nullptr),
    mVarId(0),
    mUnnamedCount(0),
    mCodeWithTemplates(false), //is there any templates?
    mTimerResults(nullptr)
#ifdef MAXTIME
    ,mMaxTime(std::time(0) + MAXTIME)
#endif
{
}

Tokenizer::Tokenizer(const Settings *settings, ErrorLogger *errorLogger) :
    list(settings),
    mSettings(settings),
    mErrorLogger(errorLogger),
    mSymbolDatabase(nullptr),
    mTemplateSimplifier(nullptr),
    mVarId(0),
    mUnnamedCount(0),
    mCodeWithTemplates(false), //is there any templates?
    mTimerResults(nullptr)
#ifdef MAXTIME
    ,mMaxTime(std::time(0) + MAXTIME)
#endif
{
    // make sure settings are specified
    assert(mSettings);

    mTemplateSimplifier = new TemplateSimplifier(list, settings, errorLogger);
}

Tokenizer::~Tokenizer()
{
    delete mSymbolDatabase;
    delete mTemplateSimplifier;
}


//---------------------------------------------------------------------------
// SizeOfType - gives the size of a type
//---------------------------------------------------------------------------

unsigned int Tokenizer::sizeOfType(const Token *type) const
{
    if (!type || type->str().empty())
        return 0;

    if (type->tokType() == Token::eString)
        return Token::getStrLength(type) + 1U;

    const std::map<std::string, unsigned int>::const_iterator it = mTypeSize.find(type->str());
    if (it == mTypeSize.end()) {
        const Library::PodType* podtype = mSettings->library.podtype(type->str());
        if (!podtype)
            return 0;

        return podtype->size;
    } else if (type->isLong()) {
        if (type->str() == MatchCompiler::makeConstString("double"))
            return mSettings->sizeof_long_double;
        else if (type->str() == MatchCompiler::makeConstString("long"))
            return mSettings->sizeof_long_long;
    }

    return it->second;
}
//---------------------------------------------------------------------------

// check if this statement is a duplicate definition
bool Tokenizer::duplicateTypedef(Token **tokPtr, const Token *name, const Token *typeDef) const
{
    // check for an end of definition
    const Token * tok = *tokPtr;
    if (tok && match13(tok->next())) {
        const Token * end = tok->next();

        if (end->str() == MatchCompiler::makeConstString("[")) {
            if (!end->link())
                syntaxError(end); // invalid code
            end = end->link()->next();
        } else if (end->str() == MatchCompiler::makeConstString(",")) {
            // check for derived class
            if (match14(tok->previous()))
                return false;

            // find end of definition
            while (end && end->next() && !match15(end->next())) {
                if (end->next()->str() == MatchCompiler::makeConstString("("))
                    end = end->linkAt(1);

                end = (end)?end->next():nullptr;
            }
            if (end)
                end = end->next();
        } else if (end->str() == MatchCompiler::makeConstString("(")) {
            if (tok->previous()->str().find("operator")  == 0) {
                // conversion operator
                return false;
            } else if (tok->previous()->str() == MatchCompiler::makeConstString("typedef")) {
                // typedef of function returning this type
                return false;
            } else if (match16(tok->previous())) {
                return false;
            } else if (tok->previous()->str() == MatchCompiler::makeConstString(">")) {
                if (!match17(tok->tokAt(-2)))
                    return false;

                if (!match18(tok->tokAt(-3)))
                    return false;

                *tokPtr = end->link();
                return true;
            }
        }

        if (end) {
            if (match19(end)) { // function parameter ?
                // look backwards
                if (match17(tok->previous()) &&
                    !match20(tok->previous())) {
                    // duplicate definition so skip entire function
                    *tokPtr = end->next()->link();
                    return true;
                }
            } else if (end->str() == MatchCompiler::makeConstString(">")) { // template parameter ?
                // look backwards
                if (match17(tok->previous()) &&
                    !match21(tok->previous())) {
                    // duplicate definition so skip entire template
                    while (end && end->str() != MatchCompiler::makeConstString("{"))
                        end = end->next();
                    if (end) {
                        *tokPtr = end->link();
                        return true;
                    }
                }
            } else {
                // look backwards
                if (match22(tok->previous()) ||
                    (end->str() == MatchCompiler::makeConstString(";") && tok->previous()->str() == MatchCompiler::makeConstString(",")) ||
                    (tok->previous()->str() == MatchCompiler::makeConstString("*") && tok->next()->str() != MatchCompiler::makeConstString("(")) ||
                    (match17(tok->previous()) &&
                     (!match23(tok->previous()) &&
                      !match24(tok->tokAt(-2))))) {
                    // scan backwards for the end of the previous statement
                    while (tok && tok->previous() && !match25(tok->previous())) {
                        if (tok->previous()->str() == MatchCompiler::makeConstString("}")) {
                            tok = tok->previous()->link();
                        } else if (tok->previous()->str() == MatchCompiler::makeConstString("typedef")) {
                            return true;
                        } else if (tok->previous()->str() == MatchCompiler::makeConstString("enum")) {
                            return true;
                        } else if (tok->previous()->str() == MatchCompiler::makeConstString("struct")) {
                            if (tok->strAt(-2) == MatchCompiler::makeConstString("typedef") &&
                                tok->next()->str() == MatchCompiler::makeConstString("{") &&
                                typeDef->strAt(3) != MatchCompiler::makeConstString("{")) {
                                // declaration after forward declaration
                                return true;
                            } else if (tok->next()->str() == MatchCompiler::makeConstString("{")) {
                                return true;
                            } else if (match26(tok->next())) {
                                return true;
                            } else if (tok->next()->str() == name->str()) {
                                return true;
                            } else if (tok->next()->str() != MatchCompiler::makeConstString(";")) {
                                return true;
                            } else {
                                return false;
                            }
                        } else if (tok->previous()->str() == MatchCompiler::makeConstString("union")) {
                            if (tok->next()->str() != MatchCompiler::makeConstString(";")) {
                                return true;
                            } else {
                                return false;
                            }
                        } else if (isCPP() && tok->previous()->str() == MatchCompiler::makeConstString("class")) {
                            if (tok->next()->str() != MatchCompiler::makeConstString(";")) {
                                return true;
                            } else {
                                return false;
                            }
                        }
                        if (tok)
                            tok = tok->previous();
                    }

                    if ((*tokPtr)->strAt(1) != MatchCompiler::makeConstString("(") || !match27((*tokPtr)->linkAt(1)))
                        return true;
                }
            }
        }
    }

    return false;
}

void Tokenizer::unsupportedTypedef(const Token *tok) const
{
    if (!mSettings->debugwarnings)
        return;

    std::ostringstream str;
    const Token *tok1 = tok;
    unsigned int level = 0;
    while (tok) {
        if (level == 0 && tok->str() == MatchCompiler::makeConstString(";"))
            break;
        else if (tok->str() == MatchCompiler::makeConstString("{"))
            ++level;
        else if (tok->str() == MatchCompiler::makeConstString("}")) {
            if (level == 0)
                break;
            --level;
        }

        if (tok != tok1)
            str << " ";
        str << tok->str();
        tok = tok->next();
    }
    if (tok)
        str << " ;";

    reportError(tok1, Severity::debug, "debug",
                "Failed to parse \'" + str.str() + "\'. The checking continues anyway.");
}

Token * Tokenizer::deleteInvalidTypedef(Token *typeDef)
{
    Token *tok = nullptr;

    // remove typedef but leave ;
    while (typeDef->next()) {
        if (typeDef->next()->str() == MatchCompiler::makeConstString(";")) {
            typeDef->deleteNext();
            break;
        } else if (typeDef->next()->str() == MatchCompiler::makeConstString("{"))
            Token::eraseTokens(typeDef, typeDef->linkAt(1));
        else if (typeDef->next()->str() == MatchCompiler::makeConstString("}"))
            break;
        typeDef->deleteNext();
    }

    if (typeDef != list.front()) {
        tok = typeDef->previous();
        tok->deleteNext();
    } else {
        list.front()->deleteThis();
        tok = list.front();
    }

    return tok;
}

namespace {
    struct Space {
        Space() : bodyEnd(nullptr), isNamespace(false) { }
        std::string className;
        const Token * bodyEnd;
        bool isNamespace;
    };
}

static Token *splitDefinitionFromTypedef(Token *tok, unsigned int *unnamedCount)
{
    Token *tok1;
    std::string name;
    bool isConst = false;

    if (tok->next()->str() == MatchCompiler::makeConstString("const")) {
        tok->deleteNext();
        isConst = true;
    }

    if (tok->strAt(2) == MatchCompiler::makeConstString("{")) { // unnamed
        tok1 = tok->linkAt(2);

        if (tok1 && tok1->next()) {
            // use typedef name if available
            if (match17(tok1->next()))
                name = tok1->next()->str();
            else // create a unique name
                name = "Unnamed" + MathLib::toString((*unnamedCount)++);
            tok->next()->insertToken(name);
        } else
            return nullptr;
    } else if (tok->strAt(3) == MatchCompiler::makeConstString(":")) {
        tok1 = tok->tokAt(4);
        while (tok1 && tok1->str() != MatchCompiler::makeConstString("{"))
            tok1 = tok1->next();
        if (!tok1)
            return nullptr;

        tok1 = tok1->link();

        name = tok->strAt(2);
    } else { // has a name
        tok1 = tok->linkAt(3);

        if (!tok1)
            return nullptr;

        name = tok->strAt(2);
    }

    tok1->insertToken(";");
    tok1 = tok1->next();

    if (tok1->next() && tok1->next()->str() == MatchCompiler::makeConstString(";") && tok1->previous()->str() == MatchCompiler::makeConstString("}")) {
        tok->deleteThis();
        tok1->deleteThis();
        return nullptr;
    } else {
        tok1->insertToken("typedef");
        tok1 = tok1->next();
        Token * tok3 = tok1;
        if (isConst) {
            tok1->insertToken("const");
            tok1 = tok1->next();
        }
        tok1->insertToken(tok->next()->str()); // struct, union or enum
        tok1 = tok1->next();
        tok1->insertToken(name);
        tok->deleteThis();
        tok = tok3;
    }

    return tok;
}

/* This function is called when processing function related typedefs.
 * If simplifyTypedef generates an "Internal Error" message and the
 * code that generated it deals in some way with functions, then this
 * function will probably need to be extended to handle a new function
 * related pattern */
Token *Tokenizer::processFunc(Token *tok2, bool inOperator) const
{
    if (tok2->next() && tok2->next()->str() != MatchCompiler::makeConstString(")") &&
        tok2->next()->str() != MatchCompiler::makeConstString(",")) {
        // skip over tokens for some types of canonicalization
        if (match28(tok2->next()))
            tok2 = tok2->linkAt(5);
        else if (match29(tok2->next()))
            tok2 = tok2->linkAt(6);
        else if (match30(tok2->next()))
            tok2 = tok2->tokAt(5);
        else if (match31(tok2->next()) &&
                 match32(tok2->linkAt(4)))
            tok2 = tok2->linkAt(4)->next();
        else if (match33(tok2->next()))
            tok2 = tok2->linkAt(5)->next();
        else if (match34(tok2->next()) &&
                 match35(tok2->linkAt(2)))
            tok2 = tok2->next();
        else {
            if (tok2->next()->str() == MatchCompiler::makeConstString("("))
                tok2 = tok2->next()->link();
            else if (!inOperator && !match36(tok2->next())) {
                tok2 = tok2->next();

                while (match37(tok2) &&
                       !match38(tok2->next()))
                    tok2 = tok2->next();

                // skip over namespace
                while (match39(tok2))
                    tok2 = tok2->tokAt(2);

                if (!tok2)
                    return nullptr;

                if (tok2->str() == MatchCompiler::makeConstString("(") &&
                    tok2->link()->next() &&
                    tok2->link()->next()->str() == MatchCompiler::makeConstString("(")) {
                    tok2 = tok2->link();

                    if (tok2->next()->str() == MatchCompiler::makeConstString("("))
                        tok2 = tok2->next()->link();
                }

                // skip over typedef parameter
                if (tok2->next() && tok2->next()->str() == MatchCompiler::makeConstString("(")) {
                    tok2 = tok2->next()->link();
                    if (!tok2->next())
                        syntaxError(tok2);

                    if (tok2->next()->str() == MatchCompiler::makeConstString("("))
                        tok2 = tok2->next()->link();
                }
            }
        }
    }
    return tok2;
}

void Tokenizer::simplifyTypedef()
{
    std::vector<Space> spaceInfo;
    bool isNamespace = false;
    std::string className;
    bool hasClass = false;
    bool goback = false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (mErrorLogger && !list.getFiles().empty())
            mErrorLogger->reportProgress(list.getFiles()[0], "Tokenize (typedef)", tok->progressValue());

        if (mSettings->terminated())
            return;

        if (isMaxTime())
            return;

        if (goback) {
            //jump back once, see the comment at the end of the function
            goback = false;
            tok = tok->previous();
        }

        if (tok->str() != MatchCompiler::makeConstString("typedef")) {
            if (tok->str() == MatchCompiler::makeConstString("(") && tok->strAt(1) == MatchCompiler::makeConstString("typedef")) {
                // Skip typedefs inside parentheses (#2453 and #4002)
                tok = tok->next();
            } else if (match40(tok) &&
                       (!tok->previous() || tok->previous()->str() != MatchCompiler::makeConstString("enum"))) {
                isNamespace = (tok->str() == MatchCompiler::makeConstString("namespace"));
                hasClass = true;
                className = tok->next()->str();
            } else if (hasClass && tok->str() == MatchCompiler::makeConstString(";")) {
                hasClass = false;
            } else if (hasClass && tok->str() == MatchCompiler::makeConstString("{")) {
                Space info;
                info.isNamespace = isNamespace;
                info.className = className;
                info.bodyEnd = tok->link();
                spaceInfo.push_back(info);

                hasClass = false;
            } else if (!spaceInfo.empty() && tok->str() == MatchCompiler::makeConstString("}") && spaceInfo.back().bodyEnd == tok) {
                spaceInfo.pop_back();
            }
            continue;
        }

        // pull struct, union, enum or class definition out of typedef
        // use typedef name for unnamed struct, union, enum or class
        if (match41(tok->next())) {
            Token *tok1 = splitDefinitionFromTypedef(tok, &mUnnamedCount);
            if (!tok1)
                continue;
            tok = tok1;
        } else if (match42(tok->next())) {
            Token *tok1 = tok;
            while (tok1 && tok1->str() != MatchCompiler::makeConstString(";") && tok1->str() != MatchCompiler::makeConstString("{"))
                tok1 = tok1->next();
            if (tok1 && tok1->str() == MatchCompiler::makeConstString("{")) {
                tok1 = splitDefinitionFromTypedef(tok, &mUnnamedCount);
                if (!tok1)
                    continue;
                tok = tok1;
            }
        }

        /** @todo add support for union */
        if (match43(tok->next()) && tok->strAt(2) == tok->strAt(3)) {
            tok->deleteNext(3);
            tok->deleteThis();
            if (tok->next())
                tok->deleteThis();
            //now the next token to process is 'tok', not 'tok->next()';
            goback = true;
            continue;
        }

        Token *typeName;
        Token *typeStart = nullptr;
        Token *typeEnd = nullptr;
        Token *argStart = nullptr;
        Token *argEnd = nullptr;
        Token *arrayStart = nullptr;
        Token *arrayEnd = nullptr;
        Token *specStart = nullptr;
        Token *specEnd = nullptr;
        Token *typeDef = tok;
        Token *argFuncRetStart = nullptr;
        Token *argFuncRetEnd = nullptr;
        Token *funcStart = nullptr;
        Token *funcEnd = nullptr;
        Token *tokOffset = tok->next();
        bool function = false;
        bool functionPtr = false;
        bool functionRetFuncPtr = false;
        bool functionPtrRetFuncPtr = false;
        bool ptrToArray = false;
        bool refToArray = false;
        bool ptrMember = false;
        bool typeOf = false;
        Token *namespaceStart = nullptr;
        Token *namespaceEnd = nullptr;

        // check for invalid input
        if (!tokOffset)
            syntaxError(tok);


        if (tokOffset->str() == MatchCompiler::makeConstString("::")) {
            typeStart = tokOffset;
            tokOffset = tokOffset->next();

            while (match44(tokOffset))
                tokOffset = tokOffset->tokAt(2);

            typeEnd = tokOffset;

            if (match17(tokOffset))
                tokOffset = tokOffset->next();
        } else if (match44(tokOffset)) {
            typeStart = tokOffset;

            do {
                tokOffset = tokOffset->tokAt(2);
            } while (match44(tokOffset));

            typeEnd = tokOffset;

            if (match17(tokOffset))
                tokOffset = tokOffset->next();
        } else if (match17(tokOffset)) {
            typeStart = tokOffset;

            while (match45(tokOffset) ||
                   (tokOffset->next() && tokOffset->next()->isStandardType()))
                tokOffset = tokOffset->next();

            typeEnd = tokOffset;
            tokOffset = tokOffset->next();

            while (match17(tokOffset) &&
                   (tokOffset->isStandardType() || match46(tokOffset))) {
                typeEnd = tokOffset;
                tokOffset = tokOffset->next();
            }

            bool atEnd = false;
            while (!atEnd) {
                if (tokOffset && tokOffset->str() == MatchCompiler::makeConstString("::")) {
                    typeEnd = tokOffset;
                    tokOffset = tokOffset->next();
                }

                if (match17(tokOffset) &&
                    tokOffset->next() && !match47(tokOffset->next())) {
                    typeEnd = tokOffset;
                    tokOffset = tokOffset->next();
                } else if (match48(tokOffset)) {
                    typeEnd = tokOffset;
                    tokOffset = tokOffset->next();
                    atEnd = true;
                } else
                    atEnd = true;
            }
        } else
            continue; // invalid input

        // check for invalid input
        if (!tokOffset)
            syntaxError(tok);

        // check for template
        if (!isC() && tokOffset->str() == MatchCompiler::makeConstString("<")) {
            typeEnd = tokOffset->findClosingBracket();

            while (typeEnd && match49(typeEnd->next()))
                typeEnd = typeEnd->tokAt(2);

            if (!typeEnd) {
                // internal error
                return;
            }

            while (match50(typeEnd->next()))
                typeEnd = typeEnd->next();

            tok = typeEnd;
            tokOffset = tok->next();
        }

        std::list<std::string> pointers;
        // check for pointers and references
        while (match51(tokOffset)) {
            pointers.push_back(tokOffset->str());
            tokOffset = tokOffset->next();
        }

        // check for invalid input
        if (!tokOffset)
            syntaxError(tok);

        if (match17(tokOffset)) {
            // found the type name
            typeName = tokOffset;
            tokOffset = tokOffset->next();

            // check for array
            if (tokOffset && tokOffset->str() == MatchCompiler::makeConstString("[")) {
                arrayStart = tokOffset;

                bool atEnd = false;
                while (!atEnd) {
                    while (tokOffset->next() && !match52(tokOffset->next())) {
                        tokOffset = tokOffset->next();
                    }

                    if (!tokOffset->next())
                        return; // invalid input
                    else if (tokOffset->next()->str() == MatchCompiler::makeConstString(";"))
                        atEnd = true;
                    else if (tokOffset->str() == MatchCompiler::makeConstString("]"))
                        atEnd = true;
                    else
                        tokOffset = tokOffset->next();
                }

                arrayEnd = tokOffset;
                tokOffset = tokOffset->next();
            }

            // check for end or another
            if (match52(tokOffset))
                tok = tokOffset;

            // or a function typedef
            else if (tokOffset && tokOffset->str() == MatchCompiler::makeConstString("(")) {
                Token *tokOffset2 = nullptr;
                if (match53(tokOffset)) {
                    tokOffset2 = tokOffset->next();
                    if (tokOffset2->str() == MatchCompiler::makeConstString("typename"))
                        tokOffset2 = tokOffset2->next();
                    while (match44(tokOffset2))
                        tokOffset2 = tokOffset2->tokAt(2);
                }

                // unhandled typedef, skip it and continue
                if (typeName->str() == MatchCompiler::makeConstString("void")) {
                    unsupportedTypedef(typeDef);
                    tok = deleteInvalidTypedef(typeDef);
                    if (tok == list.front())
                        //now the next token to process is 'tok', not 'tok->next()';
                        goback = true;
                    continue;
                }

                // function pointer
                else if (match54(tokOffset2)) {
                    // name token wasn't a name, it was part of the type
                    typeEnd = typeEnd->next();
                    functionPtr = true;
                    funcStart = funcEnd = tokOffset2; // *
                    tokOffset = tokOffset2->tokAt(3); // (
                    typeName = tokOffset->tokAt(-2);
                    argStart = tokOffset;
                    argEnd = tokOffset->link();
                    tok = argEnd->next();
                }

                // function
                else if (isFunctionHead(tokOffset->link(), ";,")) {
                    function = true;
                    if (tokOffset->link()->next()->str() == MatchCompiler::makeConstString("const")) {
                        specStart = tokOffset->link()->next();
                        specEnd = specStart;
                    }
                    argStart = tokOffset;
                    argEnd = tokOffset->link();
                    tok = argEnd->next();
                    if (specStart)
                        tok = tok->next();
                }

                // syntax error
                else
                    syntaxError(tok);
            }

            // unhandled typedef, skip it and continue
            else {
                unsupportedTypedef(typeDef);
                tok = deleteInvalidTypedef(typeDef);
                if (tok == list.front())
                    //now the next token to process is 'tok', not 'tok->next()';
                    goback = true;
                continue;
            }
        }

        // typeof: typedef __typeof__ ( ... ) type;
        else if (match55(tokOffset->previous()) &&
                 match56(tokOffset->link())) {
            argStart = tokOffset;
            argEnd = tokOffset->link();
            typeName = tokOffset->link()->next();
            tok = typeName->next();
            typeOf = true;
        }

        // function: typedef ... ( ... type )( ... );
        //           typedef ... (( ... type )( ... ));
        //           typedef ... ( * ( ... type )( ... ));
        else if (tokOffset->str() == MatchCompiler::makeConstString("(") && (
                     (tokOffset->link() && match57(tokOffset->link()->previous()) &&
                      match58(tokOffset->link()->next()->link())) ||
                     (match59(tokOffset) &&
                      tokOffset->next() && match57(tokOffset->next()->link()->previous()) &&
                      match60(tokOffset->next()->link()->next()->link())) ||
                     (match61(tokOffset) &&
                      tokOffset->linkAt(2) && match57(tokOffset->linkAt(2)->previous()) &&
                      match60(tokOffset->linkAt(2)->next()->link())))) {
            if (tokOffset->next()->str() == MatchCompiler::makeConstString("("))
                tokOffset = tokOffset->next();
            else if (match61(tokOffset)) {
                pointers.push_back("*");
                tokOffset = tokOffset->tokAt(2);
            }

            if (tokOffset->link()->strAt(-2) == MatchCompiler::makeConstString("*"))
                functionPtr = true;
            else
                function = true;
            funcStart = tokOffset->next();
            tokOffset = tokOffset->link();
            funcEnd = tokOffset->tokAt(-2);
            typeName = tokOffset->previous();
            argStart = tokOffset->next();
            argEnd = tokOffset->next()->link();
            if (!argEnd)
                syntaxError(argStart);

            tok = argEnd->next();
            Token *spec = tok;
            if (match50(spec)) {
                specStart = spec;
                specEnd = spec;
                while (match50(spec->next())) {
                    specEnd = spec->next();
                    spec = specEnd;
                }
                tok = specEnd->next();
            }
            if (!tok)
                syntaxError(specEnd);

            if (tok->str() == MatchCompiler::makeConstString(")"))
                tok = tok->next();
        }

        else if (match62(tokOffset)) {
            function = true;
            if (tokOffset->link()->next()) {
                tok = tokOffset->link()->next();
                tokOffset = tokOffset->tokAt(2);
                typeName = tokOffset->previous();
                argStart = tokOffset;
                argEnd = tokOffset->link();
            } else {
                // internal error
                continue;
            }
        }

        // pointer to function returning pointer to function
        else if (match63(tokOffset) &&
                 match64(tokOffset->linkAt(6)) &&
                 match65(tokOffset->linkAt(6)->linkAt(2))) {
            functionPtrRetFuncPtr = true;

            tokOffset = tokOffset->tokAt(6);
            typeName = tokOffset->tokAt(-2);
            argStart = tokOffset;
            argEnd = tokOffset->link();
            if (!argEnd)
                syntaxError(arrayStart);

            argFuncRetStart = argEnd->tokAt(2);
            argFuncRetEnd = argFuncRetStart->link();
            if (!argFuncRetEnd)
                syntaxError(argFuncRetStart);

            tok = argFuncRetEnd->next();
        }

        // function returning pointer to function
        else if (match66(tokOffset) &&
                 match64(tokOffset->linkAt(3)) &&
                 match65(tokOffset->linkAt(3)->linkAt(2))) {
            functionRetFuncPtr = true;

            tokOffset = tokOffset->tokAt(3);
            typeName = tokOffset->previous();
            argStart = tokOffset;
            argEnd = tokOffset->link();

            argFuncRetStart = argEnd->tokAt(2);
            if (!argFuncRetStart)
                syntaxError(tokOffset);

            argFuncRetEnd = argFuncRetStart->link();
            if (!argFuncRetEnd)
                syntaxError(tokOffset);

            tok = argFuncRetEnd->next();
        } else if (match67(tokOffset)) {
            functionRetFuncPtr = true;

            tokOffset = tokOffset->tokAt(5);
            typeName = tokOffset->tokAt(-2);
            argStart = tokOffset;
            argEnd = tokOffset->link();
            if (!argEnd)
                syntaxError(arrayStart);

            argFuncRetStart = argEnd->tokAt(2);
            if (!argFuncRetStart)
                syntaxError(tokOffset);

            argFuncRetEnd = argFuncRetStart->link();
            if (!argFuncRetEnd)
                syntaxError(tokOffset);

            tok = argFuncRetEnd->next();
        }

        // pointer/reference to array
        else if (match68(tokOffset)) {
            ptrToArray = (tokOffset->next()->str() == MatchCompiler::makeConstString("*"));
            refToArray = !ptrToArray;
            tokOffset = tokOffset->tokAt(2);
            typeName = tokOffset;
            arrayStart = tokOffset->tokAt(2);
            arrayEnd = arrayStart->link();
            if (!arrayEnd)
                syntaxError(arrayStart);

            tok = arrayEnd->next();
        }

        // pointer to class member
        else if (match69(tokOffset)) {
            tokOffset = tokOffset->tokAt(2);
            namespaceStart = tokOffset->previous();
            namespaceEnd = tokOffset;
            ptrMember = true;
            tokOffset = tokOffset->tokAt(2);
            typeName = tokOffset;
            tok = tokOffset->tokAt(2);
        }

        // unhandled typedef, skip it and continue
        else {
            unsupportedTypedef(typeDef);
            tok = deleteInvalidTypedef(typeDef);
            if (tok == list.front())
                //now the next token to process is 'tok', not 'tok->next()';
                goback = true;
            continue;
        }

        bool done = false;
        bool ok = true;

        while (!done) {
            std::string pattern = typeName->str();
            unsigned int scope = 0;
            bool simplifyType = false;
            bool inMemberFunc = false;
            int memberScope = 0;
            bool globalScope = false;
            std::size_t classLevel = spaceInfo.size();

            for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
                if (mSettings->terminated())
                    return;

                if (tok2->link()) { // Pre-check for performance
                    // check for end of scope
                    if (tok2->str() == MatchCompiler::makeConstString("}")) {
                        // check for end of member function
                        if (inMemberFunc) {
                            --memberScope;
                            if (memberScope == 0)
                                inMemberFunc = false;
                        }

                        if (classLevel > 0 && tok2 == spaceInfo[classLevel - 1].bodyEnd) {
                            --classLevel;
                            pattern.clear();

                            for (std::size_t i = classLevel; i < spaceInfo.size(); ++i)
                                pattern += (spaceInfo[i].className + " :: ");

                            pattern += typeName->str();
                        } else {
                            if (scope == 0)
                                break;
                            --scope;
                        }
                    }

                    // check for member functions
                    else if (isCPP() && match70(tok2)) {
                        const Token *temp = tok2;
                        while (temp && temp->str() == MatchCompiler::makeConstString("]") && temp->link() && temp->link()->previous())
                            temp = temp->link()->previous();
                        if (!temp || !temp->link() || !temp->link()->previous())
                            continue;
                        const Token *func = temp->link()->previous();
                        if (temp->str() != MatchCompiler::makeConstString(")"))
                            continue;
                        if (!func || !func->previous()) // Ticket #4239
                            continue;

                        /** @todo add support for multi-token operators */
                        if (func->previous()->str() == MatchCompiler::makeConstString("operator"))
                            func = func->previous();

                        if (!func->previous())
                            syntaxError(func);

                        // check for qualifier
                        if (func->previous()->str() == MatchCompiler::makeConstString("::")) {
                            // check for available and matching class name
                            if (!spaceInfo.empty() && classLevel < spaceInfo.size() &&
                                func->strAt(-2) == spaceInfo[classLevel].className) {
                                memberScope = 0;
                                inMemberFunc = true;
                            }
                        }
                    }

                    // check for entering a new scope
                    else if (tok2->str() == MatchCompiler::makeConstString("{")) {
                        // check for entering a new namespace
                        if (isCPP() && tok2->strAt(-2) == MatchCompiler::makeConstString("namespace")) {
                            if (classLevel < spaceInfo.size() &&
                                spaceInfo[classLevel].isNamespace &&
                                spaceInfo[classLevel].className == tok2->previous()->str()) {
                                spaceInfo[classLevel].bodyEnd = tok2->link();
                                ++classLevel;
                                pattern.clear();
                                for (std::size_t i = classLevel; i < spaceInfo.size(); ++i)
                                    pattern += (spaceInfo[i].className + " :: ");

                                pattern += typeName->str();
                            }
                            ++scope;
                        }

                        // keep track of scopes within member function
                        if (inMemberFunc)
                            ++memberScope;

                        ++scope;
                    }
                }

                // check for operator typedef
                /** @todo add support for multi-token operators */
                else if (isCPP() &&
                         tok2->str() == MatchCompiler::makeConstString("operator") &&
                         tok2->next() &&
                         tok2->next()->str() == typeName->str() &&
                         tok2->linkAt(2) &&
                         tok2->strAt(2) == MatchCompiler::makeConstString("(") &&
                         match71(tok2->linkAt(2))) {
                    // check for qualifier
                    if (tok2->previous()->str() == MatchCompiler::makeConstString("::")) {
                        // check for available and matching class name
                        if (!spaceInfo.empty() && classLevel < spaceInfo.size() &&
                            tok2->strAt(-2) == spaceInfo[classLevel].className) {
                            tok2 = tok2->next();
                            simplifyType = true;
                        }
                    }
                }

                // check for typedef that can be substituted
                else if (Token::simpleMatch(tok2, pattern.c_str()) ||
                         (inMemberFunc && tok2->str() == typeName->str())) {
                    // member function class variables don't need qualification
                    if (!(inMemberFunc && tok2->str() == typeName->str()) && pattern.find("::") != std::string::npos) { // has a "something ::"
                        Token *start = tok2;
                        std::size_t count = 0;
                        int back = int(classLevel) - 1;
                        bool good = true;
                        // check for extra qualification
                        while (back >= 0) {
                            Token *qualificationTok = start->tokAt(-2);
                            if (!match44(qualificationTok))
                                break;
                            if (qualificationTok->str() == spaceInfo[back].className) {
                                start = qualificationTok;
                                back--;
                                count++;
                            } else {
                                good = false;
                                break;
                            }
                        }
                        // check global namespace
                        if (good && back == 0 && start->strAt(-1) == MatchCompiler::makeConstString("::"))
                            good = false;

                        if (good) {
                            // remove any extra qualification if present
                            while (count) {
                                tok2->tokAt(-3)->deleteNext(2);
                                --count;
                            }

                            // remove global namespace if present
                            if (tok2->strAt(-1) == MatchCompiler::makeConstString("::")) {
                                tok2->tokAt(-2)->deleteNext();
                                globalScope = true;
                            }

                            // remove qualification if present
                            for (std::size_t i = classLevel; i < spaceInfo.size(); ++i) {
                                tok2->deleteNext(2);
                            }
                            simplifyType = true;
                        }
                    } else {
                        if (tok2->strAt(-1) == MatchCompiler::makeConstString("::")) {
                            std::size_t relativeSpaceInfoSize = spaceInfo.size();
                            Token * tokBeforeType = tok2->previous();
                            while (relativeSpaceInfoSize != 0 &&
                                   tokBeforeType && tokBeforeType->str() == MatchCompiler::makeConstString("::") &&
                                   tokBeforeType->strAt(-1) == spaceInfo[relativeSpaceInfoSize-1].className) {
                                tokBeforeType = tokBeforeType->tokAt(-2);
                                --relativeSpaceInfoSize;
                            }
                            if (tokBeforeType && tokBeforeType->str() != MatchCompiler::makeConstString("::")) {
                                Token::eraseTokens(tokBeforeType, tok2);
                                simplifyType = true;
                            }
                        } else if (match72(tok2->previous())) {
                            tok2 = tok2->next();
                        } else if (duplicateTypedef(&tok2, typeName, typeDef)) {
                            // skip to end of scope if not already there
                            if (tok2->str() != MatchCompiler::makeConstString("}")) {
                                while (tok2->next()) {
                                    if (tok2->next()->str() == MatchCompiler::makeConstString("{"))
                                        tok2 = tok2->linkAt(1)->previous();
                                    else if (tok2->next()->str() == MatchCompiler::makeConstString("}"))
                                        break;

                                    tok2 = tok2->next();
                                }
                            }
                        } else if (match73(tok2->tokAt(-2))) {
                            // Ticket #5868: Don't substitute variable names
                        } else if (tok2->previous()->str() != MatchCompiler::makeConstString(".")) {
                            simplifyType = true;
                        }
                    }
                }

                if (simplifyType) {
                    // can't simplify 'operator functionPtr ()' and 'functionPtr operator ... ()'
                    if (functionPtr && (tok2->previous()->str() == MatchCompiler::makeConstString("operator") ||
                                        (tok2->next() && tok2->next()->str() == MatchCompiler::makeConstString("operator")))) {
                        simplifyType = false;
                        tok2 = tok2->next();
                        continue;
                    }

                    // There are 2 categories of typedef substitutions:
                    // 1. variable declarations that preserve the variable name like
                    //    global, local, and function parameters
                    // 2. not variable declarations that have no name like derived
                    //    classes, casts, operators, and template parameters

                    // try to determine which category this substitution is
                    bool inCast = false;
                    bool inTemplate = false;
                    bool inOperator = false;
                    bool inSizeof = false;

                    const bool sameStartEnd = (typeStart == typeEnd);

                    // check for derived class: class A : some_typedef {
                    const bool isDerived = match74(tok2->previous());

                    // check for cast: (some_typedef) A or static_cast<some_typedef>(A)
                    // todo: check for more complicated casts like: (const some_typedef *)A
                    if ((tok2->previous()->str() == MatchCompiler::makeConstString("(") && tok2->next()->str() == MatchCompiler::makeConstString(")") && tok2->strAt(-2) != MatchCompiler::makeConstString("sizeof")) ||
                        (tok2->previous()->str() == MatchCompiler::makeConstString("<") && match75(tok2->next())))
                        inCast = true;

                    // check for template parameters: t<some_typedef> t1
                    else if (match76(tok2->previous()) &&
                             match77(tok2->next()))
                        inTemplate = true;

                    else if (match78(tok2->tokAt(-2)))
                        inSizeof = true;

                    // check for operator
                    if (tok2->strAt(-1) == MatchCompiler::makeConstString("operator") ||
                        match79(tok2->tokAt(-2)))
                        inOperator = true;

                    if (typeStart->str() == MatchCompiler::makeConstString("typename") && tok2->strAt(-1)==MatchCompiler::makeConstString("typename")) {
                        // Remove one typename if it is already contained in the goal
                        typeStart = typeStart->next();
                    }

                    // skip over class or struct in derived class declaration
                    bool structRemoved = false;
                    if (isDerived && match80(typeStart)) {
                        if (typeStart->str() == MatchCompiler::makeConstString("struct"))
                            structRemoved = true;
                        typeStart = typeStart->next();
                    }
                    if (match81(typeStart) && match39(tok2))
                        typeStart = typeStart->next();

                    if (sameStartEnd)
                        typeEnd = typeStart;

                    // start substituting at the typedef name by replacing it with the type
                    tok2->str(typeStart->str());

                    // restore qualification if it was removed
                    if (typeStart->str() == MatchCompiler::makeConstString("struct") || structRemoved) {
                        if (structRemoved)
                            tok2 = tok2->previous();

                        if (globalScope) {
                            tok2->insertToken("::");
                            tok2 = tok2->next();
                        }

                        for (std::size_t i = classLevel; i < spaceInfo.size(); ++i) {
                            tok2->insertToken(spaceInfo[i].className);
                            tok2 = tok2->next();
                            tok2->insertToken("::");
                            tok2 = tok2->next();
                        }
                    }

                    // add remainder of type
                    tok2 = TokenList::copyTokens(tok2, typeStart->next(), typeEnd);

                    if (!pointers.empty()) {
                        for (const std::string &p : pointers) {
                            tok2->insertToken(p);
                            tok2 = tok2->next();
                        }
                    }

                    if (funcStart && funcEnd) {
                        tok2->insertToken("(");
                        tok2 = tok2->next();
                        Token *tok3 = tok2;
                        tok2 = TokenList::copyTokens(tok2, funcStart, funcEnd);

                        if (!inCast)
                            tok2 = processFunc(tok2, inOperator);

                        if (!tok2)
                            break;

                        tok2->insertToken(")");
                        tok2 = tok2->next();
                        Token::createMutualLinks(tok2, tok3);

                        tok2 = TokenList::copyTokens(tok2, argStart, argEnd);

                        if (specStart) {
                            Token *spec = specStart;
                            tok2->insertToken(spec->str());
                            tok2 = tok2->next();
                            while (spec != specEnd) {
                                spec = spec->next();
                                tok2->insertToken(spec->str());
                                tok2 = tok2->next();
                            }
                        }
                    }

                    else if (functionPtr || function) {
                        // don't add parentheses around function names because it
                        // confuses other simplifications
                        bool needParen = true;
                        if (!inTemplate && function && tok2->next() && tok2->next()->str() != MatchCompiler::makeConstString("*"))
                            needParen = false;
                        if (needParen) {
                            tok2->insertToken("(");
                            tok2 = tok2->next();
                        }
                        Token *tok3 = tok2;
                        if (namespaceStart) {
                            const Token *tok4 = namespaceStart;

                            while (tok4 != namespaceEnd) {
                                tok2->insertToken(tok4->str());
                                tok2 = tok2->next();
                                tok4 = tok4->next();
                            }
                            tok2->insertToken(namespaceEnd->str());
                            tok2 = tok2->next();
                        }
                        if (functionPtr) {
                            tok2->insertToken("*");
                            tok2 = tok2->next();
                        }

                        if (!inCast)
                            tok2 = processFunc(tok2, inOperator);

                        if (needParen) {
                            if (!tok2)
                                syntaxError(nullptr);

                            tok2->insertToken(")");
                            tok2 = tok2->next();
                            Token::createMutualLinks(tok2, tok3);
                        }
                        if (!tok2)
                            syntaxError(nullptr);

                        tok2 = TokenList::copyTokens(tok2, argStart, argEnd);
                        if (inTemplate) {
                            if (!tok2)
                                syntaxError(nullptr);

                            tok2 = tok2->next();
                        }

                        if (specStart) {
                            Token *spec = specStart;
                            tok2->insertToken(spec->str());
                            tok2 = tok2->next();
                            while (spec != specEnd) {
                                spec = spec->next();
                                tok2->insertToken(spec->str());
                                tok2 = tok2->next();
                            }
                        }
                    } else if (functionRetFuncPtr || functionPtrRetFuncPtr) {
                        tok2->insertToken("(");
                        tok2 = tok2->next();
                        Token *tok3 = tok2;
                        tok2->insertToken("*");
                        tok2 = tok2->next();

                        Token * tok4 = nullptr;
                        if (functionPtrRetFuncPtr) {
                            tok2->insertToken("(");
                            tok2 = tok2->next();
                            tok4 = tok2;
                            tok2->insertToken("*");
                            tok2 = tok2->next();
                        }

                        // skip over variable name if there
                        if (!inCast) {
                            if (!tok2 || !tok2->next())
                                syntaxError(nullptr);

                            if (tok2->next()->str() != MatchCompiler::makeConstString(")"))
                                tok2 = tok2->next();
                        }

                        if (tok4 && functionPtrRetFuncPtr) {
                            tok2->insertToken(")");
                            tok2 = tok2->next();
                            Token::createMutualLinks(tok2, tok4);
                        }

                        tok2 = TokenList::copyTokens(tok2, argStart, argEnd);

                        tok2->insertToken(")");
                        tok2 = tok2->next();
                        Token::createMutualLinks(tok2, tok3);

                        tok2 = TokenList::copyTokens(tok2, argFuncRetStart, argFuncRetEnd);
                    } else if (ptrToArray || refToArray) {
                        tok2->insertToken("(");
                        tok2 = tok2->next();
                        Token *tok3 = tok2;

                        if (ptrToArray)
                            tok2->insertToken("*");
                        else
                            tok2->insertToken("&");
                        tok2 = tok2->next();

                        // skip over name
                        if (tok2->next() && tok2->next()->str() != MatchCompiler::makeConstString(")")) {
                            if (tok2->next()->str() != MatchCompiler::makeConstString("("))
                                tok2 = tok2->next();

                            // check for function and skip over args
                            if (tok2 && tok2->next() && tok2->next()->str() == MatchCompiler::makeConstString("("))
                                tok2 = tok2->next()->link();

                            // check for array
                            if (tok2 && tok2->next() && tok2->next()->str() == MatchCompiler::makeConstString("["))
                                tok2 = tok2->next()->link();
                        } else {
                            // syntax error
                        }

                        tok2->insertToken(")");
                        Token::createMutualLinks(tok2->next(), tok3);
                    } else if (ptrMember) {
                        if (match82(tok2)) {
                            tok2->insertToken("*");
                            tok2 = tok2->next();
                        } else {
                            // This is the case of casting operator.
                            // Name is not available, and () should not be
                            // inserted
                            const bool castOperator = inOperator && match83(tok2);
                            Token *openParenthesis = nullptr;

                            if (!castOperator) {
                                tok2->insertToken("(");
                                tok2 = tok2->next();

                                openParenthesis = tok2;
                            }

                            const Token *tok4 = namespaceStart;

                            while (tok4 != namespaceEnd) {
                                tok2->insertToken(tok4->str());
                                tok2 = tok2->next();
                                tok4 = tok4->next();
                            }
                            tok2->insertToken(namespaceEnd->str());
                            tok2 = tok2->next();

                            tok2->insertToken("*");
                            tok2 = tok2->next();

                            if (openParenthesis) {
                                // skip over name
                                tok2 = tok2->next();

                                tok2->insertToken(")");
                                tok2 = tok2->next();

                                Token::createMutualLinks(tok2, openParenthesis);
                            }
                        }
                    } else if (typeOf) {
                        tok2 = TokenList::copyTokens(tok2, argStart, argEnd);
                    } else if (tok2->strAt(2) == MatchCompiler::makeConstString("[")) {
                        do {
                            if (!tok2->linkAt(2))
                                syntaxError(tok2);

                            tok2 = tok2->linkAt(2)->previous();
                        } while (tok2->strAt(2) == MatchCompiler::makeConstString("["));
                    }

                    if (arrayStart && arrayEnd) {
                        do {
                            if (!tok2->next())
                                syntaxError(tok2); // can't recover so quit

                            if (!inCast && !inSizeof && !inTemplate)
                                tok2 = tok2->next();

                            if (tok2->str() == MatchCompiler::makeConstString("const"))
                                tok2 = tok2->next();

                            // reference to array?
                            if (tok2->str() == MatchCompiler::makeConstString("&")) {
                                tok2 = tok2->previous();
                                tok2->insertToken("(");
                                Token *tok3 = tok2->next();

                                // handle missing variable name
                                if (tok2->strAt(3) == MatchCompiler::makeConstString(")") || tok2->strAt(3) == MatchCompiler::makeConstString(","))
                                    tok2 = tok2->tokAt(2);
                                else
                                    tok2 = tok2->tokAt(3);
                                if (!tok2)
                                    syntaxError(nullptr);

                                while (tok2->strAt(1) == MatchCompiler::makeConstString("::"))
                                    tok2 = tok2->tokAt(2);

                                // skip over function parameters
                                if (tok2->strAt(1) == MatchCompiler::makeConstString("(")) {
                                    tok2 = tok2->linkAt(1);

                                    if (tok2->strAt(1) == MatchCompiler::makeConstString("const"))
                                        tok2 = tok2->next();
                                }

                                tok2->insertToken(")");
                                tok2 = tok2->next();
                                Token::createMutualLinks(tok2, tok3);
                            }

                            if (!tok2->next())
                                syntaxError(tok2); // can't recover so quit

                            // skip over array dimensions
                            while (tok2->next()->str() == MatchCompiler::makeConstString("["))
                                tok2 = tok2->linkAt(1);

                            tok2 = TokenList::copyTokens(tok2, arrayStart, arrayEnd);
                            if (!tok2->next())
                                syntaxError(tok2);

                            if (tok2->str() == MatchCompiler::makeConstString("=")) {
                                if (!tok2->next())
                                    syntaxError(tok2);
                                if (tok2->next()->str() == MatchCompiler::makeConstString("{"))
                                    tok2 = tok2->next()->link()->next();
                                else if (tok2->next()->str().at(0) == '\"')
                                    tok2 = tok2->tokAt(2);
                            }
                        } while (match84(tok2));
                    }

                    simplifyType = false;
                }
                if (!tok2)
                    break;
            }

            if (!tok)
                syntaxError(nullptr);

            if (tok->str() == MatchCompiler::makeConstString(";"))
                done = true;
            else if (tok->str() == MatchCompiler::makeConstString(",")) {
                arrayStart = nullptr;
                arrayEnd = nullptr;
                tokOffset = tok->next();
                pointers.clear();

                while (match37(tokOffset)) {
                    pointers.push_back(tokOffset->str());
                    tokOffset = tokOffset->next();
                }

                if (match17(tokOffset)) {
                    typeName = tokOffset;
                    tokOffset = tokOffset->next();

                    if (tokOffset && tokOffset->str() == MatchCompiler::makeConstString("[")) {
                        arrayStart = tokOffset;

                        for (;;) {
                            while (tokOffset->next() && !match52(tokOffset->next()))
                                tokOffset = tokOffset->next();

                            if (!tokOffset->next())
                                return; // invalid input
                            else if (tokOffset->next()->str() == MatchCompiler::makeConstString(";"))
                                break;
                            else if (tokOffset->str() == MatchCompiler::makeConstString("]"))
                                break;
                            else
                                tokOffset = tokOffset->next();
                        }

                        arrayEnd = tokOffset;
                        tokOffset = tokOffset->next();
                    }

                    if (match52(tokOffset))
                        tok = tokOffset;
                    else {
                        // we encountered a typedef we don't support yet so just continue
                        done = true;
                        ok = false;
                    }
                } else {
                    // we encountered a typedef we don't support yet so just continue
                    done = true;
                    ok = false;
                }
            } else {
                // something is really wrong (internal error)
                done = true;
                ok = false;
            }
        }

        if (ok) {
            // remove typedef
            Token::eraseTokens(typeDef, tok);

            if (typeDef != list.front()) {
                tok = typeDef->previous();
                tok->deleteNext();
                //no need to remove last token in the list
                if (tok->tokAt(2))
                    tok->deleteNext();
            } else {
                list.front()->deleteThis();
                //no need to remove last token in the list
                if (list.front()->next())
                    list.front()->deleteThis();
                tok = list.front();
                //now the next token to process is 'tok', not 'tok->next()';
                goback = true;
            }
        }
    }
}

void Tokenizer::simplifyMulAndParens()
{
    if (!list.front())
        return;
    for (Token *tok = list.front()->tokAt(3); tok; tok = tok->next()) {
        if (!tok->isName())
            continue;
        //fix ticket #2784 - improved by ticket #3184
        unsigned int closedPars = 0;
        Token *tokend = tok->next();
        Token *tokbegin = tok->previous();
        while (tokend && tokend->str() == MatchCompiler::makeConstString(")")) {
            ++closedPars;
            tokend = tokend->next();
        }
        if (!tokend || !(tokend->isAssignmentOp()))
            continue;
        while (match85(tokbegin)) {
            if (tokbegin->str() == MatchCompiler::makeConstString("&")) {
                if (match86(tokbegin->tokAt(-2))) {
                    //remove '* &'
                    tokbegin = tokbegin->tokAt(-2);
                    tokbegin->deleteNext(2);
                } else if (match87(tokbegin->tokAt(-3))) {
                    if (closedPars == 0)
                        break;
                    --closedPars;
                    //remove ')'
                    tok->deleteNext();
                    //remove '* ( &'
                    tokbegin = tokbegin->tokAt(-3);
                    tokbegin->deleteNext(3);
                } else
                    break;
            } else if (tokbegin->str() == MatchCompiler::makeConstString("(")) {
                if (closedPars == 0)
                    break;

                //find consecutive opening parentheses
                unsigned int openPars = 0;
                while (tokbegin && tokbegin->str() == MatchCompiler::makeConstString("(") && openPars <= closedPars) {
                    ++openPars;
                    tokbegin = tokbegin->previous();
                }
                if (!tokbegin || openPars > closedPars)
                    break;

                if ((openPars == closedPars && match88(tokbegin)) ||
                    match89(tokbegin->tokAt(-2)) ||
                    match90(tokbegin->tokAt(-3))) {
                    //remove the excessive parentheses around the variable
                    while (openPars > 0) {
                        tok->deleteNext();
                        tokbegin->deleteNext();
                        --closedPars;
                        --openPars;
                    }
                } else
                    break;
            }
        }
    }
}

bool Tokenizer::createTokens(std::istream &code,
                             const std::string& FileName)
{
    // make sure settings specified
    assert(mSettings);

    return list.createTokens(code, Path::getRelativePath(Path::simplifyPath(FileName), mSettings->basePaths));
}

void Tokenizer::createTokens(const simplecpp::TokenList *tokenList)
{
    // make sure settings specified
    assert(mSettings);
    list.createTokens(tokenList);
}

bool Tokenizer::simplifyTokens1(const std::string &configuration)
{
    // Fill the map mTypeSize..
    fillTypeSizes();

    mConfiguration = configuration;

    if (!simplifyTokenList1(list.getFiles().front().c_str()))
        return false;

    list.createAst();
    list.validateAst();

    createSymbolDatabase();

    // Use symbol database to identify rvalue references. Split && to & &. This is safe, since it doesn't delete any tokens (which might be referenced by symbol database)
    for (const Variable* var : mSymbolDatabase->variableList()) {
        if (var && var->isRValueReference()) {
            Token* endTok = const_cast<Token*>(var->typeEndToken());
            endTok->str("&");
            endTok->astOperand1(nullptr);
            endTok->astOperand2(nullptr);
            endTok->insertToken("&");
            endTok->next()->scope(endTok->scope());
        }
    }

    mSymbolDatabase->setValueTypeInTokenList();
    ValueFlow::setValues(&list, mSymbolDatabase, mErrorLogger, mSettings);

    printDebugOutput(1);

    return true;
}

bool Tokenizer::tokenize(std::istream &code,
                         const char FileName[],
                         const std::string &configuration)
{
    if (!createTokens(code, FileName))
        return false;

    return simplifyTokens1(configuration);
}
//---------------------------------------------------------------------------

void Tokenizer::findComplicatedSyntaxErrorsInTemplates()
{
    validate();
    mTemplateSimplifier->checkComplicatedSyntaxErrorsInTemplates();
}

void Tokenizer::checkForEnumsWithTypedef()
{
    for (const Token *tok = list.front(); tok; tok = tok->next()) {
        if (match91(tok)) {
            tok = tok->tokAt(2);
            const Token *tok2 = findmatch92(tok, tok->link()) ;
            if (tok2)
                syntaxError(tok2);
            tok = tok->link();
        }
    }
}

void Tokenizer::fillTypeSizes()
{
    mTypeSize.clear();
    mTypeSize["char"] = 1;
    mTypeSize["_Bool"] = mSettings->sizeof_bool;
    mTypeSize["bool"] = mSettings->sizeof_bool;
    mTypeSize["short"] = mSettings->sizeof_short;
    mTypeSize["int"] = mSettings->sizeof_int;
    mTypeSize["long"] = mSettings->sizeof_long;
    mTypeSize["float"] = mSettings->sizeof_float;
    mTypeSize["double"] = mSettings->sizeof_double;
    mTypeSize["wchar_t"] = mSettings->sizeof_wchar_t;
    mTypeSize["size_t"] = mSettings->sizeof_size_t;
    mTypeSize["*"] = mSettings->sizeof_pointer;
}

void Tokenizer::combineOperators()
{
    const bool cpp = isCPP();

    // Combine tokens..
    for (Token *tok = list.front(); tok && tok->next(); tok = tok->next()) {
        const char c1 = tok->str()[0];

        if (tok->str().length() == 1 && tok->next()->str().length() == 1) {
            const char c2 = tok->next()->str()[0];

            // combine +-*/ and =
            if (c2 == '=' && (std::strchr("+-*/%|^=!<>", c1))) {
                tok->str(tok->str() + c2);
                tok->deleteNext();
                continue;
            }
        } else if (tok->next()->str() == MatchCompiler::makeConstString("=")) {
            if (tok->str() == MatchCompiler::makeConstString(">>")) {
                tok->str(">>=");
                tok->deleteNext();
            } else if (tok->str() == MatchCompiler::makeConstString("<<")) {
                tok->str("<<=");
                tok->deleteNext();
            }
        } else if (cpp && (c1 == 'p' || c1 == '_') &&
                   match93(tok)) {
            bool simplify = false;
            unsigned int par = 0U;
            for (const Token *prev = tok->previous(); prev; prev = prev->previous()) {
                if (prev->str() == MatchCompiler::makeConstString(")")) {
                    ++par;
                } else if (prev->str() == MatchCompiler::makeConstString("(")) {
                    if (par == 0U)
                        break;
                    --par;
                }
                if (par != 0U || prev->str() == MatchCompiler::makeConstString("("))
                    continue;
                if (match88(prev)) {
                    simplify = true;
                    break;
                }
                if (prev->isName() && prev->isUpperCaseName())
                    continue;
                if (prev->isName() && endsWith(prev->str(), ':'))
                    simplify = true;
                break;
            }
            if (simplify) {
                tok->str(tok->str() + ":");
                tok->deleteNext();
            }
        } else if (tok->str() == MatchCompiler::makeConstString("->")) {
            // If the preceding sequence is "( & %name% )", replace it by "%name%"
            Token *t = tok->tokAt(-4);
            if (match94(t)) {
                t->deleteThis();
                t->deleteThis();
                t->deleteNext();
            }

            tok->str(".");
            tok->originalName("->");
        }
    }
}

void Tokenizer::combineStringAndCharLiterals()
{
    // Combine wide strings and wide characters
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match95(tok)) {
            // Combine 'L "string"' and 'L 'c''
            tok->str(tok->next()->str());
            tok->deleteNext();
            tok->isLong(true);
        }
    }

    // Combine strings
    for (Token *tok = list.front();
         tok;
         tok = tok->next()) {
        if (tok->str()[0] != '"')
            continue;

        tok->str(simplifyString(tok->str()));
        while (tok->next() && tok->next()->tokType() == Token::eString) {
            // Two strings after each other, combine them
            tok->concatStr(simplifyString(tok->next()->str()));
            tok->deleteNext();
        }
    }
}

void Tokenizer::concatenateNegativeNumberAndAnyPositive()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match96(tok) || tok->tokType() == Token::eIncDecOp)
            continue;

        while (tok->next() && tok->next()->str() == MatchCompiler::makeConstString("+"))
            tok->deleteNext();

        if (match97(tok->next())) {
            tok->deleteNext();
            tok->next()->str("-" + tok->next()->str());
        }
    }
}

void Tokenizer::simplifyExternC()
{
    if (isC())
        return;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match98(tok)) {
            if (tok->strAt(2) == MatchCompiler::makeConstString("{")) {
                tok->linkAt(2)->deleteThis();
                tok->deleteNext(2);
            } else
                tok->deleteNext();
            tok->deleteThis();
        }
    }
}

void Tokenizer::simplifyRoundCurlyParentheses()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        while (match99(tok) &&
               match100(tok->linkAt(2))) {
            if (tok->str() == MatchCompiler::makeConstString(":") && !match101(tok->tokAt(-2)))
                break;
            Token *end = tok->linkAt(2)->tokAt(-3);
            if (match102(end))
                end->deleteNext(2);
            tok->linkAt(2)->previous()->deleteNext(3);
            tok->deleteNext(2);
        }
        if (match103(tok)) {
            tok->deleteNext();
            tok->deleteThis();
            tok->deleteNext(3);
        }
    }
}

void Tokenizer::simplifySQL()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match104(tok))
            continue;

        const Token *end = findSQLBlockEnd(tok);
        if (end == nullptr)
            syntaxError(nullptr);

        const std::string instruction = tok->stringifyList(end);
        // delete all tokens until the embedded SQL block end
        Token::eraseTokens(tok, end);

        // insert "asm ( "instruction" ) ;"
        tok->str("asm");
        // it can happen that 'end' is NULL when wrong code is inserted
        if (!tok->next())
            tok->insertToken(";");
        tok->insertToken(")");
        tok->insertToken("\"" + instruction + "\"");
        tok->insertToken("(");
        // jump to ';' and continue
        tok = tok->tokAt(3);
    }
}

void Tokenizer::simplifyArrayAccessSyntax()
{
    // 0[a] -> a[0]
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->isNumber() && match105(tok)) {
            const std::string number(tok->str());
            Token* indexTok = tok->tokAt(2);
            tok->str(indexTok->str());
            tok->varId(indexTok->varId());
            indexTok->str(number);
        }
    }
}

void Tokenizer::simplifyParameterVoid()
{
    for (Token* tok = list.front(); tok; tok = tok->next()) {
        if (match106(tok))
            tok->next()->deleteNext();
    }
}

void Tokenizer::simplifyRedundantConsecutiveBraces()
{
    // Remove redundant consecutive braces, i.e. '.. { { .. } } ..' -> '.. { .. } ..'.
    for (Token *tok = list.front(); tok;) {
        if (match107(tok)) {
            tok = tok->linkAt(1);
        } else if (match108(tok) && match109(tok->next()->link())) {
            //remove internal parentheses
            tok->next()->link()->deleteThis();
            tok->deleteNext();
        } else
            tok = tok->next();
    }
}

void Tokenizer::simplifyDoublePlusAndDoubleMinus()
{
    // Convert - - into + and + - into -
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        while (tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("+")) {
                if (tok->next()->str()[0] == '-') {
                    tok = tok->next();
                    if (tok->str().size() == 1) {
                        tok = tok->previous();
                        tok->str("-");
                        tok->deleteNext();
                    } else if (tok->isNumber()) {
                        tok->str(tok->str().substr(1));
                        tok = tok->previous();
                        tok->str("-");
                    }
                    continue;
                }
            } else if (tok->str() == MatchCompiler::makeConstString("-")) {
                if (tok->next()->str()[0] == '-') {
                    tok = tok->next();
                    if (tok->str().size() == 1) {
                        tok = tok->previous();
                        tok->str("+");
                        tok->deleteNext();
                    } else if (tok->isNumber()) {
                        tok->str(tok->str().substr(1));
                        tok = tok->previous();
                        tok->str("+");
                    }
                    continue;
                }
            }

            break;
        }
    }
}

/** Specify array size if it hasn't been given */

void Tokenizer::arraySize()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!tok->isName() || !match110(tok))
            continue;
        bool addlength = false;
        if (match111(tok)) {
            Token *t = tok->tokAt(3);
            t->deleteNext();
            t->next()->deleteNext();
            addlength = true;
        }

        if (addlength || match112(tok)) {
            tok = tok->next();
            const std::size_t sz = Token::getStrSize(tok->tokAt(3));
            tok->insertToken(MathLib::toString(sz));
            tok = tok->tokAt(5);
        }

        else if (match113(tok)) {
            MathLib::biguint sz = 1;
            tok = tok->next();
            Token *end = tok->linkAt(3);
            for (Token *tok2 = tok->tokAt(4); tok2 && tok2 != end; tok2 = tok2->next()) {
                if (tok2->link() && match114(tok2)) {
                    if (tok2->str() == MatchCompiler::makeConstString("[") && tok2->link()->strAt(1) == MatchCompiler::makeConstString("=")) { // designated initializer
                        if (match115(tok2))
                            sz = std::max(sz, MathLib::toULongNumber(tok2->strAt(1)) + 1U);
                        else {
                            sz = 0;
                            break;
                        }
                    }
                    tok2 = tok2->link();
                } else if (tok2->str() == MatchCompiler::makeConstString(",")) {
                    if (!match116(tok2->next()))
                        ++sz;
                    else {
                        tok2 = tok2->previous();
                        tok2->deleteNext();
                    }
                }
            }

            if (sz != 0)
                tok->insertToken(MathLib::toString(sz));

            tok = end->next() ? end->next() : end;
        }
    }
}

static Token *skipTernaryOp(Token *tok)
{
    unsigned int colonLevel = 1;
    while (nullptr != (tok = tok->next())) {
        if (tok->str() == MatchCompiler::makeConstString("?")) {
            ++colonLevel;
        } else if (tok->str() == MatchCompiler::makeConstString(":")) {
            --colonLevel;
            if (colonLevel == 0) {
                tok = tok->next();
                break;
            }
        }
        if (tok->link() && match117(tok))
            tok = tok->link();
        else if (match118(tok->next()))
            break;
    }
    if (colonLevel > 0) // Ticket #5214: Make sure the ':' matches the proper '?'
        return nullptr;
    return tok;
}

const Token * Tokenizer::startOfExecutableScope(const Token * tok)
{
    if (tok->str() != MatchCompiler::makeConstString(")"))
        return nullptr;

    tok = isFunctionHead(tok, ":{", true);

    if (match119(tok)) {
        while (match120(tok))
            tok = tok->linkAt(2)->next();
    }

    return (tok && tok->str() == MatchCompiler::makeConstString("{")) ? tok : nullptr;
}


/** simplify labels and case|default in the code: add a ";" if not already in.*/

void Tokenizer::simplifyLabelsCaseDefault()
{
    const bool cpp = isCPP();
    bool executablescope = false;
    unsigned int indentLevel = 0;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // Simplify labels in the executable scope..
        Token *start = const_cast<Token *>(startOfExecutableScope(tok));
        if (start) {
            tok = start;
            executablescope = true;
        }

        if (!executablescope)
            continue;

        if (tok->str() == MatchCompiler::makeConstString("{")) {
            if (tok->previous()->str() == MatchCompiler::makeConstString("="))
                tok = tok->link();
            else
                ++indentLevel;
        } else if (tok->str() == MatchCompiler::makeConstString("}")) {
            --indentLevel;
            if (indentLevel == 0) {
                executablescope = false;
                continue;
            }
        } else if (match121(tok))
            tok = tok->link();

        if (match122(tok)) {
            while (nullptr != (tok = tok->next())) {
                if (match121(tok)) {
                    tok = tok->link();
                } else if (tok->str() == MatchCompiler::makeConstString("?")) {
                    Token *tok1 = skipTernaryOp(tok);
                    if (!tok1) {
                        syntaxError(tok);
                    }
                    tok = tok1;
                }
                if (match123(tok->next()))
                    break;
            }
            if (!tok)
                break;
            if (tok->str() != MatchCompiler::makeConstString("case") && tok->next() && tok->next()->str() == MatchCompiler::makeConstString(":")) {
                tok = tok->next();
                if (!tok->next())
                    syntaxError(tok);
                if (tok->next()->str() != MatchCompiler::makeConstString(";") && tok->next()->str() != MatchCompiler::makeConstString("case"))
                    tok->insertToken(";");
                else
                    tok = tok->previous();
            } else {
                syntaxError(tok);
            }
        } else if (match124(tok)) {
            if (!cpp || !match125(tok->next())) {
                tok = tok->tokAt(2);
                tok->insertToken(";");
            }
        }
    }
}


void Tokenizer::simplifyCaseRange()
{
    for (Token* tok = list.front(); tok; tok = tok->next()) {
        if (match126(tok)) {
            const MathLib::bigint start = MathLib::toLongNumber(tok->strAt(1));
            MathLib::bigint end = MathLib::toLongNumber(tok->strAt(5));
            end = std::min(start + 50, end); // Simplify it 50 times at maximum
            if (start < end) {
                tok = tok->tokAt(2);
                tok->str(":");
                tok->deleteNext();
                tok->next()->str("case");
                for (MathLib::bigint i = end-1; i > start; i--) {
                    tok->insertToken(":");
                    tok->insertToken(MathLib::toString(i));
                    tok->insertToken("case");
                }
            }
        } else if (match127(tok)) {
            const char start = tok->strAt(1)[1];
            const char end = tok->strAt(5)[1];
            if (start < end) {
                tok = tok->tokAt(2);
                tok->str(":");
                tok->deleteNext();
                tok->next()->str("case");
                for (char i = end - 1; i > start; i--) {
                    tok->insertToken(":");
                    tok->insertToken(std::string(1, '\'') + i + '\'');
                    tok->insertToken("case");
                }
            }
        }
    }
}



void Tokenizer::simplifyTemplates()
{
    if (isC())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // simple fix for sizeof used as template parameter
        // TODO: this is a bit hardcoded. make a bit more generic
        if (match128(tok) && tok->tokAt(4)->isStandardType()) {
            Token * const tok3 = tok->next();
            const unsigned int sizeOfResult = sizeOfType(tok3->tokAt(3));
            tok3->deleteNext(4);
            tok3->insertToken(MathLib::toString(sizeOfResult));
        }
        // Ticket #6181: normalize C++11 template parameter list closing syntax
        if (tok->str() == MatchCompiler::makeConstString("<") && mTemplateSimplifier->templateParameters(tok)) {
            Token *endTok = tok->findClosingBracket();
            if (endTok && endTok->str() == MatchCompiler::makeConstString(">>")) {
                endTok->str(">");
                endTok->insertToken(">");
            }
        }
    }

    mTemplateSimplifier->simplifyTemplates(
#ifdef MAXTIME
        mMaxTime,
#else
        0, // ignored
#endif
        mCodeWithTemplates);
}
//---------------------------------------------------------------------------


static bool setVarIdParseDeclaration(const Token **tok, const std::map<std::string,unsigned int> &variableId, bool executableScope, bool cpp, bool c)
{
    const Token *tok2 = *tok;
    if (!tok2->isName())
        return false;

    unsigned int typeCount = 0;
    unsigned int singleNameCount = 0;
    bool hasstruct = false;   // Is there a "struct" or "class"?
    bool bracket = false;
    bool ref = false;
    while (tok2) {
        if (tok2->isName()) {
            if (cpp && match129(tok2))
                return false;
            if (match130(tok2) || (!c && match131(tok2))) {
                hasstruct = true;
                typeCount = 0;
                singleNameCount = 0;
            } else if (tok2->str() == MatchCompiler::makeConstString("const")) {
                ;  // just skip "const"
            } else if (!hasstruct && variableId.find(tok2->str()) != variableId.end() && tok2->previous()->str() != MatchCompiler::makeConstString("::")) {
                ++typeCount;
                tok2 = tok2->next();
                if (!tok2 || tok2->str() != MatchCompiler::makeConstString("::"))
                    break;
            } else {
                if (tok2->str() != MatchCompiler::makeConstString("void") || match132(tok2)) // just "void" cannot be a variable type
                    ++typeCount;
                ++singleNameCount;
            }
        } else if (!c && ((TemplateSimplifier::templateParameters(tok2) > 0) ||
                          match133(tok2) /* Ticket #4764 */)) {
            const Token * tok3 = tok2->findClosingBracket();
            if (tok3 == nullptr) { /* Ticket #8151 */
                throw tok2;
            }
            tok2 = tok3;
            if (tok2->str() != MatchCompiler::makeConstString(">"))
                break;
            singleNameCount = 1;
        } else if (match134(tok2)) {
            ref = !bracket;
        } else if (singleNameCount == 1 && match135(tok2) && match121(tok2->link()->next())) {
            bracket = true; // Skip: Seems to be valid pointer to array or function pointer
        } else if (tok2->str() == MatchCompiler::makeConstString("::")) {
            singleNameCount = 0;
        } else if (tok2->str() != MatchCompiler::makeConstString("*") && tok2->str() != MatchCompiler::makeConstString("::")) {
            break;
        }
        tok2 = tok2->next();
    }

    if (tok2) {
        bool isLambdaArg = false;
        if (cpp) {
            const Token *tok3 = (*tok)->previous();
            if (tok3 && tok3->str() == MatchCompiler::makeConstString(",")) {
                while (tok3 && !match136(tok3)) {
                    if (match137(tok3))
                        tok3 = tok3->link();
                    tok3 = tok3->previous();
                }
            }
            if (tok3 && match138(tok3->previous()) && match19(tok3->link()))
                isLambdaArg = true;
        }

        *tok = tok2;

        // In executable scopes, references must be assigned
        // Catching by reference is an exception
        if (executableScope && ref && !isLambdaArg) {
            if (match139(tok2))
                ;   // reference is assigned => ok
            else if (tok2->str() != MatchCompiler::makeConstString(")") || tok2->link()->strAt(-1) != MatchCompiler::makeConstString("catch"))
                return false;   // not catching by reference => not declaration
        }
    }

    // Check if array declaration is valid (#2638)
    // invalid declaration: AAA a[4] = 0;
    if (typeCount >= 2 && executableScope && tok2 && tok2->str() == MatchCompiler::makeConstString("[")) {
        const Token *tok3 = tok2->link()->next();
        while (tok3 && tok3->str() == MatchCompiler::makeConstString("[")) {
            tok3 = tok3->link()->next();
        }
        if (match140(tok3))
            return false;
    }

    return (typeCount >= 2 && tok2 && match141(tok2->tokAt(-2)));
}


static void setVarIdStructMembers(Token **tok1,
                                  std::map<unsigned int, std::map<std::string, unsigned int> >& structMembers,
                                  unsigned int *varId)
{
    Token *tok = *tok1;

    if (match142(tok)) {
        const unsigned int struct_varid = tok->varId();
        if (struct_varid == 0)
            return;

        std::map<std::string, unsigned int>& members = structMembers[struct_varid];

        tok = tok->tokAt(3);
        while (tok->str() != MatchCompiler::makeConstString("}")) {
            if (match143(tok))
                tok = tok->link();
            if (match144(tok->previous())) {
                tok = tok->next();
                const std::map<std::string, unsigned int>::iterator it = members.find(tok->str());
                if (it == members.end()) {
                    members[tok->str()] = ++(*varId);
                    tok->varId(*varId);
                } else {
                    tok->varId(it->second);
                }
            }
            tok = tok->next();
        }

        return;
    }

    while (match145(tok->next())) {
        const unsigned int struct_varid = tok->varId();
        tok = tok->tokAt(2);
        if (struct_varid == 0)
            continue;

        // Don't set varid for template function
        if (TemplateSimplifier::templateParameters(tok->next()) > 0)
            break;

        std::map<std::string, unsigned int>& members = structMembers[struct_varid];
        const std::map<std::string, unsigned int>::iterator it = members.find(tok->str());
        if (it == members.end()) {
            members[tok->str()] = ++(*varId);
            tok->varId(*varId);
        } else {
            tok->varId(it->second);
        }
    }
    // tok can't be null
    *tok1 = tok;
}


void Tokenizer::setVarIdClassDeclaration(const Token * const startToken,
        const VariableMap &variableMap,
        const unsigned int scopeStartVarId,
        std::map<unsigned int, std::map<std::string,unsigned int> >& structMembers)
{
    // end of scope
    const Token * const endToken = startToken->link();

    // determine class name
    std::string className;
    for (const Token *tok = startToken->previous(); tok; tok = tok->previous()) {
        if (!tok->isName() && tok->str() != MatchCompiler::makeConstString(":"))
            break;
        if (match146(tok)) {
            className = tok->next()->str();
            break;
        }
    }

    // replace varids..
    unsigned int indentlevel = 0;
    bool initList = false;
    bool inEnum = false;
    const Token *initListArgLastToken = nullptr;
    for (Token *tok = startToken->next(); tok != endToken; tok = tok->next()) {
        if (!tok)
            syntaxError(nullptr);
        if (initList) {
            if (tok == initListArgLastToken)
                initListArgLastToken = nullptr;
            else if (!initListArgLastToken &&
                     match147(tok->previous()) &&
                     match148(tok->link()))
                initListArgLastToken = tok->link();
        }
        if (tok->str() == MatchCompiler::makeConstString("{")) {
            inEnum = isEnumStart(tok);
            if (initList && !initListArgLastToken)
                initList = false;
            ++indentlevel;
        } else if (tok->str() == MatchCompiler::makeConstString("}")) {
            --indentlevel;
            inEnum = false;
        } else if (initList && indentlevel == 0 && match149(tok->previous())) {
            const std::map<std::string, unsigned int>::const_iterator it = variableMap.find(tok->str());
            if (it != variableMap.end()) {
                tok->varId(it->second);
            }
        } else if (tok->isName() && tok->varId() <= scopeStartVarId) {
            if (indentlevel > 0 || initList) {
                if (match150(tok->previous()) && tok->strAt(-2) != MatchCompiler::makeConstString("this") && !match151(tok->tokAt(-5)))
                    continue;
                if (!tok->next())
                    syntaxError(nullptr);
                if (tok->next()->str() == MatchCompiler::makeConstString("::")) {
                    if (tok->str() == className)
                        tok = tok->tokAt(2);
                    else
                        continue;
                }

                if (!inEnum) {
                    const std::map<std::string, unsigned int>::const_iterator it = variableMap.find(tok->str());
                    if (it != variableMap.end()) {
                        tok->varId(it->second);
                        setVarIdStructMembers(&tok, structMembers, variableMap.getVarId());
                    }
                }
            }
        } else if (indentlevel == 0 && tok->str() == MatchCompiler::makeConstString(":") && !initListArgLastToken)
            initList = true;
    }
}



// Update the variable ids..
// Parse each function..
static void setVarIdClassFunction(const std::string &classname,
                                  Token * const startToken,
                                  const Token * const endToken,
                                  const std::map<std::string, unsigned int> &varlist,
                                  std::map<unsigned int, std::map<std::string, unsigned int> >& structMembers,
                                  unsigned int *varId_)
{
    for (Token *tok2 = startToken; tok2 && tok2 != endToken; tok2 = tok2->next()) {
        if (tok2->varId() != 0 || !tok2->isName())
            continue;
        if (Token::Match(tok2->tokAt(-2), ("!!" + classname + " ::").c_str()))
            continue;
        if (match152(tok2->tokAt(-4))) // Currently unsupported
            continue;
        if (match153(tok2->tokAt(-2)) && !match151(tok2->tokAt(-5)))
            continue;

        const std::map<std::string,unsigned int>::const_iterator it = varlist.find(tok2->str());
        if (it != varlist.end()) {
            tok2->varId(it->second);
            setVarIdStructMembers(&tok2, structMembers, varId_);
        }
    }
}



void Tokenizer::setVarId()
{
    // Clear all variable ids
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->isName())
            tok->varId(0);
    }

    setPodTypes();

    setVarIdPass1();

    setVarIdPass2();
}


// Variable declarations can't start with "return" etc.
#define NOTSTART_C "case", "default", "goto", "NOT", "return", "sizeof", "typedef"
static const std::set<std::string> notstart_c = { NOTSTART_C };
static const std::set<std::string> notstart_cpp = { NOTSTART_C,
                                                    "delete", "friend", "new", "throw", "using", "virtual", "explicit", "const_cast", "dynamic_cast", "reinterpret_cast", "static_cast", "template"
                                                  };

void Tokenizer::setVarIdPass1()
{
    // Variable declarations can't start with "return" etc.
    const std::set<std::string>& notstart = (isC()) ? notstart_c : notstart_cpp;

    VariableMap variableMap;
    std::map<unsigned int, std::map<std::string, unsigned int> > structMembers;

    std::stack<VarIdScopeInfo> scopeStack;

    scopeStack.push(VarIdScopeInfo());
    std::stack<const Token *> functionDeclEndStack;
    const Token *functionDeclEndToken = nullptr;
    bool initlist = false;
    bool inlineFunction = false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->isOp())
            continue;
        if (tok == functionDeclEndToken) {
            functionDeclEndStack.pop();
            functionDeclEndToken = functionDeclEndStack.empty() ? nullptr : functionDeclEndStack.top();
            if (tok->str() == MatchCompiler::makeConstString(":"))
                initlist = true;
            else if (tok->str() == MatchCompiler::makeConstString(";")) {
                if (!variableMap.leaveScope())
                    cppcheckError(tok);
            } else if (tok->str() == MatchCompiler::makeConstString("{"))
                scopeStack.push(VarIdScopeInfo(true, scopeStack.top().isStructInit || tok->strAt(-1) == MatchCompiler::makeConstString("="), /*isEnum=*/false, *variableMap.getVarId()));
        } else if (!initlist && tok->str()==MatchCompiler::makeConstString("(")) {
            const Token * newFunctionDeclEnd = nullptr;
            if (!scopeStack.top().isExecutable)
                newFunctionDeclEnd = isFunctionHead(tok, "{:;");
            else {
                Token const * const tokenLinkNext = tok->link()->next();
                if (tokenLinkNext && tokenLinkNext->str() == MatchCompiler::makeConstString("{")) // might be for- or while-loop or if-statement
                    newFunctionDeclEnd = tokenLinkNext;
            }
            if (newFunctionDeclEnd && newFunctionDeclEnd != functionDeclEndToken) {
                functionDeclEndStack.push(newFunctionDeclEnd);
                functionDeclEndToken = newFunctionDeclEnd;
                variableMap.enterScope();
            }
        } else if (match154(tok)) {
            inlineFunction = false;

            const Token * const startToken = (tok->str() == MatchCompiler::makeConstString("{")) ? tok : tok->link();

            // parse anonymous unions as part of the current scope
            if (!match155(startToken->previous()) &&
                !(initlist && match156(startToken->previous()) && match157(startToken->link()))) {

                if (tok->str() == MatchCompiler::makeConstString("{")) {
                    bool isExecutable;
                    if (tok->strAt(-1) == MatchCompiler::makeConstString(")") || match158(tok->tokAt(-2)) ||
                        (initlist && tok->strAt(-1) == MatchCompiler::makeConstString("}"))) {
                        isExecutable = true;
                    } else {
                        isExecutable = ((scopeStack.top().isExecutable || initlist || tok->strAt(-1) == MatchCompiler::makeConstString("else")) &&
                                        !isClassStructUnionEnumStart(tok));
                        if (!(scopeStack.top().isStructInit || tok->strAt(-1) == MatchCompiler::makeConstString("=")))
                            variableMap.enterScope();
                    }
                    initlist = false;
                    scopeStack.push(VarIdScopeInfo(isExecutable, scopeStack.top().isStructInit || tok->strAt(-1) == MatchCompiler::makeConstString("="), isEnumStart(tok), *variableMap.getVarId()));
                } else { /* if (tok->str() == MatchCompiler::makeConstString("}")) */
                    bool isNamespace = false;
                    for (const Token *tok1 = tok->link()->previous(); tok1 && tok1->isName(); tok1 = tok1->previous()) {
                        if (tok1->str() == MatchCompiler::makeConstString("namespace")) {
                            isNamespace = true;
                            break;
                        }
                    }
                    // Set variable ids in class declaration..
                    if (!initlist && !isC() && !scopeStack.top().isExecutable && tok->link() && !isNamespace) {
                        setVarIdClassDeclaration(tok->link(),
                                                 variableMap,
                                                 scopeStack.top().startVarid,
                                                 structMembers);
                    }

                    if (!scopeStack.top().isStructInit) {
                        variableMap.leaveScope();
                    }

                    scopeStack.pop();
                    if (scopeStack.empty()) {  // should be impossible
                        scopeStack.push(VarIdScopeInfo());
                    }
                }
            }
        }

        if (!scopeStack.top().isStructInit &&
            (tok == list.front() ||
             match88(tok) ||
             (tok->str() == MatchCompiler::makeConstString("(") && isFunctionHead(tok,"{")) ||
             (tok->str() == MatchCompiler::makeConstString("(") && !scopeStack.top().isExecutable && isFunctionHead(tok,";:")) ||
             (tok->str() == MatchCompiler::makeConstString(",") && (!scopeStack.top().isExecutable || inlineFunction)) ||
             (tok->isName() && endsWith(tok->str(), ':')))) {

            // No variable declarations in sizeof
            if (match159(tok->previous())) {
                continue;
            }

            if (mSettings->terminated())
                return;

            // locate the variable name..
            const Token *tok2 = (tok->isName()) ? tok : tok->next();

            // private: protected: public: etc
            while (tok2 && endsWith(tok2->str(), ':')) {
                tok2 = tok2->next();
            }
            if (!tok2)
                break;

            // Variable declaration can't start with "return", etc
            if (notstart.find(tok2->str()) != notstart.end())
                continue;

            if (!isC() && match160(tok2))
                continue;

            bool decl;
            try { /* Ticket #8151 */
                decl = setVarIdParseDeclaration(&tok2, variableMap.map(), scopeStack.top().isExecutable, isCPP(), isC());
            } catch (const Token * errTok) {
                syntaxError(errTok);
            }
            if (decl) {
                if (tok->str() == MatchCompiler::makeConstString("(") && isFunctionHead(tok,"{") && scopeStack.top().isExecutable)
                    inlineFunction = true;

                const Token* prev2 = tok2->previous();
                if (match161(prev2) && tok2->previous()->str() != MatchCompiler::makeConstString("const"))
                    ;
                else if (match162(prev2) && tok->strAt(-1) == MatchCompiler::makeConstString("for"))
                    ;
                else if (match163(prev2) && match164(tok2->link())) {
                    // In C++ , a variable can't be called operator+ or something like that.
                    if (isCPP() &&
                        prev2->isOperatorKeyword())
                        continue;

                    const Token *tok3 = tok2->next();
                    if (!tok3->isStandardType() && tok3->str() != MatchCompiler::makeConstString("void") && !match165(tok3) && tok3->str() != MatchCompiler::makeConstString(".") && !match166(tok2->link()->previous())) {
                        if (!scopeStack.top().isExecutable) {
                            // Detecting initializations with () in non-executable scope is hard and often impossible to be done safely. Thus, only treat code as a variable that definitely is one.
                            decl = false;
                            bool rhs = false;
                            for (; tok3; tok3 = tok3->nextArgumentBeforeCreateLinks2()) {
                                if (tok3->str() == MatchCompiler::makeConstString("=")) {
                                    rhs = true;
                                    continue;
                                }

                                if (tok3->str() == MatchCompiler::makeConstString(",")) {
                                    rhs = false;
                                    continue;
                                }

                                if (rhs)
                                    continue;

                                if (tok3->isLiteral() ||
                                    (tok3->isName() && variableMap.hasVariable(tok3->str())) ||
                                    tok3->isOp() ||
                                    tok3->str() == MatchCompiler::makeConstString("(") ||
                                    notstart.find(tok3->str()) != notstart.end()) {
                                    decl = true;
                                    break;
                                }
                            }
                        }
                    } else
                        decl = false;
                } else if (isCPP() && match167(prev2) && match168(tok2->link())) { // C++11 initialization style
                    if (match169(prev2) || match170(prev2->tokAt(-2)))
                        continue;
                } else
                    decl = false;

                if (decl) {
                    variableMap.addVariable(prev2->str());

                    // set varid for template parameters..
                    tok = tok->next();
                    while (match171(tok))
                        tok = tok->next();
                    if (tok && tok->str() == MatchCompiler::makeConstString("<")) {
                        const Token *end = tok->findClosingBracket();
                        while (tok != end) {
                            if (tok->isName()) {
                                const std::map<std::string, unsigned int>::const_iterator it = variableMap.find(tok->str());
                                if (it != variableMap.end())
                                    tok->varId(it->second);
                            }
                            tok = tok->next();
                        }
                    }

                    tok = tok2->previous();
                }
            }
        }

        if (tok->isName()) {
            // don't set variable id after a struct|enum|union
            if (match172(tok->previous()) || (isCPP() && tok->strAt(-1) == MatchCompiler::makeConstString("class")))
                continue;

            if (!isC()) {
                if (tok->previous() && tok->previous()->str() == MatchCompiler::makeConstString("::"))
                    continue;
                if (tok->next() && tok->next()->str() == MatchCompiler::makeConstString("::"))
                    continue;
            }

            // function declaration inside executable scope? Function declaration is of form: type name "(" args ")"
            if (scopeStack.top().isExecutable && match173(tok)) {
                bool par = false;
                const Token *start, *end;

                // search begin of function declaration
                for (start = tok; match174(start); start = start->previous()) {
                    if (start->str() == MatchCompiler::makeConstString("(")) {
                        if (par)
                            break;
                        par = true;
                    }
                    if (match175(start)) {
                        if (!match176(start))
                            break;
                    }
                    if (start->varId() > 0U)
                        break;
                }

                // search end of function declaration
                for (end = tok->next(); match177(end); end = end->next()) {}

                // there are tokens which can't appear at the begin of a function declaration such as "return"
                const bool isNotstartKeyword = start->next() && notstart.find(start->next()->str()) != notstart.end();

                // now check if it is a function declaration
                if (match178(start) && par && match164(end) && !isNotstartKeyword)
                    // function declaration => don't set varid
                    continue;
            }

            if (!scopeStack.top().isEnum) {
                const std::map<std::string, unsigned int>::const_iterator it = variableMap.find(tok->str());
                if (it != variableMap.end()) {
                    tok->varId(it->second);
                    setVarIdStructMembers(&tok, structMembers, variableMap.getVarId());
                }
            }
        } else if (match179(tok)) {
            // Don't set varid after a :: or . token
            tok = tok->next();
        } else if (tok->str() == MatchCompiler::makeConstString(":") && match180(tok->tokAt(-2))) {
            do {
                tok = tok->next();
            } while (tok && (tok->isName() || tok->str() == MatchCompiler::makeConstString(",")));
            if (!tok)
                break;
            tok = tok->previous();
        }
    }

    mVarId = *variableMap.getVarId();
}

namespace {
    struct Member {
        Member(const std::list<std::string> &s, const std::list<const Token *> &ns, Token *t) : usingnamespaces(ns), scope(s), tok(t) {}
        std::list<const Token *> usingnamespaces;
        std::list<std::string> scope;
        Token *tok;
    };

    struct ScopeInfo2 {
        ScopeInfo2(const std::string &name_, const Token *bodyEnd_) : name(name_), bodyEnd(bodyEnd_) {}
        const std::string name;
        const Token * const bodyEnd;
    };
}

static std::string getScopeName(const std::list<ScopeInfo2> &scopeInfo)
{
    std::string ret;
    for (const ScopeInfo2 &si : scopeInfo)
        ret += (ret.empty() ? "" : " :: ") + (si.name);
    return ret;
}

static Token * matchMemberName(const std::list<std::string> &scope, const Token *nsToken, Token *memberToken, const std::list<ScopeInfo2> &scopeInfo)
{
    std::list<ScopeInfo2>::const_iterator scopeIt = scopeInfo.begin();

    // Current scope..
    for (std::list<std::string>::const_iterator it = scope.begin(); it != scope.end(); ++it) {
        if (scopeIt == scopeInfo.end() || scopeIt->name != *it)
            return nullptr;
        ++scopeIt;
    }

    // using namespace..
    if (nsToken) {
        while (match39(nsToken)) {
            if (scopeIt != scopeInfo.end() && nsToken->str() == scopeIt->name) {
                nsToken = nsToken->tokAt(2);
                ++scopeIt;
            } else {
                return nullptr;
            }
        }
        if (!match181(nsToken))
            return nullptr;
        if (scopeIt == scopeInfo.end() || nsToken->str() != scopeIt->name)
            return nullptr;
        ++scopeIt;
    }

    // Parse member tokens..
    while (scopeIt != scopeInfo.end()) {
        if (!match182(memberToken))
            return nullptr;
        if (memberToken->str() != scopeIt->name)
            return nullptr;
        if (memberToken->next()->str() == MatchCompiler::makeConstString("<")) {
            memberToken = memberToken->next()->findClosingBracket();
            if (!match183(memberToken))
                return nullptr;
        }
        memberToken = memberToken->tokAt(2);
        ++scopeIt;
    }

    return match184(memberToken) ? memberToken : nullptr;
}

static Token * matchMemberName(const Member &member, const std::list<ScopeInfo2> &scopeInfo)
{
    if (scopeInfo.empty())
        return nullptr;

    // Does this member match without "using namespace"..
    Token *ret = matchMemberName(member.scope, nullptr, member.tok, scopeInfo);
    if (ret)
        return ret;

    // Try to match member using the "using namespace ..." namespaces..
    for (const Token *ns : member.usingnamespaces) {
        ret = matchMemberName(member.scope, ns, member.tok, scopeInfo);
        if (ret)
            return ret;
    }

    return nullptr;
}

static Token * matchMemberVarName(const Member &var, const std::list<ScopeInfo2> &scopeInfo)
{
    Token *tok = matchMemberName(var, scopeInfo);
    return match4(tok) ? tok : nullptr;
}

static Token * matchMemberFunctionName(const Member &func, const std::list<ScopeInfo2> &scopeInfo)
{
    Token *tok = matchMemberName(func, scopeInfo);
    return match185(tok) ? tok : nullptr;
}

void Tokenizer::setVarIdPass2()
{
    std::map<unsigned int, std::map<std::string, unsigned int> > structMembers;

    // Member functions and variables in this source
    std::list<Member> allMemberFunctions;
    std::list<Member> allMemberVars;
    if (!isC()) {
        std::map<const Token *, std::string> endOfScope;
        std::list<std::string> scope;
        std::list<const Token *> usingnamespaces;
        for (Token *tok = list.front(); tok; tok = tok->next()) {
            if (!tok->previous() || match88(tok->previous())) {
                if (match186(tok)) {
                    const Token *endtok = tok->tokAt(2);
                    while (match39(endtok))
                        endtok = endtok->tokAt(2);
                    if (match181(endtok))
                        usingnamespaces.push_back(tok->tokAt(2));
                } else if (match187(tok)) {
                    scope.push_back(tok->strAt(1));
                    endOfScope[tok->linkAt(2)] = tok->strAt(1);
                }
            }

            if (tok->str() == MatchCompiler::makeConstString("}")) {
                const std::map<const Token *, std::string>::iterator it = endOfScope.find(tok);
                if (it != endOfScope.end())
                    scope.remove(it->second);
            }

            Token* const tok1 = tok;
            if (match188(tok->previous()))
                tok = tok->next();
            else if (match189(tok->previous()) && match190(tok->next()->findClosingBracket()))
                tok = tok->next()->findClosingBracket()->next();
            else
                continue;

            while (match191(tok)) {
                tok = tok->next();
                if (tok->str() == MatchCompiler::makeConstString("~"))
                    tok = tok->next();
                else if (match192(tok) && match190(tok->next()->findClosingBracket()))
                    tok = tok->next()->findClosingBracket()->next();
                else if (match39(tok))
                    tok = tok->next();
                else
                    break;
            }
            if (!tok->next())
                syntaxError(tok);
            if (match6(tok))
                allMemberFunctions.emplace_back(scope, usingnamespaces, tok1);
            else
                allMemberVars.emplace_back(scope, usingnamespaces, tok1);
        }
    }

    std::list<ScopeInfo2> scopeInfo;

    // class members..
    std::map<std::string, std::map<std::string, unsigned int> > varsByClass;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        while (tok->str() == MatchCompiler::makeConstString("}") && !scopeInfo.empty() && tok == scopeInfo.back().bodyEnd)
            scopeInfo.pop_back();

        if (!match193(tok))
            continue;

        const std::string &scopeName(getScopeName(scopeInfo));
        const std::string scopeName2(scopeName.empty() ? std::string() : (scopeName + " :: "));

        std::list<const Token *> classnameTokens;
        classnameTokens.push_back(tok->next());
        const Token* tokStart = tok->tokAt(2);
        while (match194(tokStart)) {
            classnameTokens.push_back(tokStart->next());
            tokStart = tokStart->tokAt(2);
        }

        std::string classname;
        for (const Token *it : classnameTokens)
            classname += (classname.empty() ? "" : " :: ") + it->str();

        std::map<std::string, unsigned int> &thisClassVars = varsByClass[scopeName2 + classname];
        while (match195(tokStart)) {
            if (match192(tokStart)) {
                tokStart = tokStart->next()->findClosingBracket();
                if (tokStart)
                    tokStart = tokStart->next();
                continue;
            }
            if (match196(tokStart)) {
                const std::map<std::string, unsigned int>& baseClassVars = varsByClass[tokStart->str()];
                thisClassVars.insert(baseClassVars.begin(), baseClassVars.end());
            }
            tokStart = tokStart->next();
        }
        if (!match197(tokStart))
            continue;

        // What member variables are there in this class?
        for (const Token *it : classnameTokens)
            scopeInfo.emplace_back(it->str(), tokStart->link());

        for (Token *tok2 = tokStart->next(); tok2 && tok2 != tokStart->link(); tok2 = tok2->next()) {
            // skip parentheses..
            if (tok2->link()) {
                if (tok2->str() == MatchCompiler::makeConstString("{")) {
                    if (tok2->strAt(-1) == MatchCompiler::makeConstString(")") || tok2->strAt(-2) == MatchCompiler::makeConstString(")"))
                        setVarIdClassFunction(scopeName2 + classname, tok2, tok2->link(), thisClassVars, structMembers, &mVarId);
                    tok2 = tok2->link();
                } else if (tok2->str() == MatchCompiler::makeConstString("(") && tok2->link()->strAt(1) != MatchCompiler::makeConstString("("))
                    tok2 = tok2->link();
            }

            // Found a member variable..
            else if (tok2->varId() > 0)
                thisClassVars[tok2->str()] = tok2->varId();
        }

        // Are there any member variables in this class?
        if (thisClassVars.empty())
            continue;

        // Member variables
        for (const Member &var : allMemberVars) {
            Token *tok2 = matchMemberVarName(var, scopeInfo);
            if (!tok2)
                continue;
            tok2->varId(thisClassVars[tok2->str()]);
        }

        if (isC() || tok->str() == MatchCompiler::makeConstString("namespace"))
            continue;

        // Set variable ids in member functions for this class..
        for (const Member &func : allMemberFunctions) {
            Token *tok2 = matchMemberFunctionName(func, scopeInfo);
            if (!tok2)
                continue;

            if (tok2->str() == MatchCompiler::makeConstString("~"))
                tok2 = tok2->linkAt(2);
            else
                tok2 = tok2->linkAt(1);

            // If this is a function implementation.. add it to funclist
            Token * start = const_cast<Token *>(isFunctionHead(tok2, "{"));
            if (start) {
                setVarIdClassFunction(classname, start, start->link(), thisClassVars, structMembers, &mVarId);
            }

            if (match198(tok2))
                tok2 = tok2->linkAt(2);

            // constructor with initializer list
            if (!match199(tok2))
                continue;

            Token *tok3 = tok2;
            while (match200(tok3)) {
                tok3 = tok3->tokAt(2);
                if (match194(tok3))
                    tok3 = tok3->next();
                while (match201(tok3))
                    tok3 = tok3->tokAt(2);
                if (!match202(tok3))
                    break;

                // set varid
                const std::map<std::string, unsigned int>::const_iterator varpos = thisClassVars.find(tok3->str());
                if (varpos != thisClassVars.end())
                    tok3->varId(varpos->second);

                // goto end of var
                if (tok3->strAt(1) == MatchCompiler::makeConstString("<")) {
                    tok3 = tok3->next()->findClosingBracket();
                    if (tok3 && tok3->next() && tok3->next()->link())
                        tok3 = tok3->next()->link();
                } else
                    tok3 = tok3->linkAt(1);
            }
            if (match203(tok3)) {
                setVarIdClassFunction(classname, tok2, tok3->next()->link(), thisClassVars, structMembers, &mVarId);
            }
        }
    }
}

static void linkBrackets(const Tokenizer * const tokenizer, std::stack<const Token*>& type, std::stack<Token*>& links, Token * const token, const char open, const char close)
{
    if (token->str()[0] == open) {
        links.push(token);
        type.push(token);
    } else if (token->str()[0] == close) {
        if (links.empty()) {
            // Error, { and } don't match.
            tokenizer->syntaxError(token, open);
        }
        if (type.top()->str()[0] != open) {
            tokenizer->syntaxError(type.top(), type.top()->str()[0]);
        }
        type.pop();

        Token::createMutualLinks(links.top(), token);
        links.pop();
    }
}

void Tokenizer::createLinks()
{
    std::stack<const Token*> type;
    std::stack<Token*> links1;
    std::stack<Token*> links2;
    std::stack<Token*> links3;
    for (Token *token = list.front(); token; token = token->next()) {
        if (token->link()) {
            token->link(nullptr);
        }

        linkBrackets(this, type, links1, token, '{', '}');

        linkBrackets(this, type, links2, token, '(', ')');

        linkBrackets(this, type, links3, token, '[', ']');
    }

    if (!links1.empty()) {
        // Error, { and } don't match.
        syntaxError(links1.top(), '{');
    }

    if (!links2.empty()) {
        // Error, ( and ) don't match.
        syntaxError(links2.top(), '(');
    }

    if (!links3.empty()) {
        // Error, [ and ] don't match.
        syntaxError(links3.top(), '[');
    }
}

void Tokenizer::createLinks2()
{
    if (isC())
        return;

    const Token * templateToken = nullptr;
    bool isStruct = false;

    std::stack<Token*> type;
    for (Token *token = list.front(); token; token = token->next()) {
        if (match204(token))
            isStruct = true;
        else if (match88(token))
            isStruct = false;

        if (token->link()) {
            if (match143(token))
                type.push(token);
            else if (!type.empty() && match205(token)) {
                while (type.top()->str() == MatchCompiler::makeConstString("<"))
                    type.pop();
                type.pop();
                templateToken = nullptr;
            } else
                token->link(nullptr);
        } else if (!templateToken && !isStruct && match206(token)) {
            if (match207(token))
                continue;
            // If there is some such code:  A<B||C>..
            // Then this is probably a template instantiation if either "B" or "C" has comparisons
            if (token->tokType() == Token::eLogicalOp && !type.empty() && type.top()->str() == MatchCompiler::makeConstString("<")) {
                const Token *prev = token->previous();
                bool foundComparison = false;
                while (match208(prev) && prev != type.top()) {
                    if (prev->str() == MatchCompiler::makeConstString(")") || prev->str() == MatchCompiler::makeConstString("]"))
                        prev = prev->link();
                    else if (prev->tokType() == Token::eLogicalOp)
                        break;
                    else if (prev->isComparisonOp())
                        foundComparison = true;
                    prev = prev->previous();
                }
                if (prev == type.top() && foundComparison)
                    continue;
                const Token *next = token->next();
                foundComparison = false;
                while (match209(next) && next->str() != MatchCompiler::makeConstString(">")) {
                    if (next->str() == MatchCompiler::makeConstString("(") || next->str() == MatchCompiler::makeConstString("["))
                        next = next->link();
                    else if (next->tokType() == Token::eLogicalOp)
                        break;
                    else if (next->isComparisonOp())
                        foundComparison = true;
                    next = next->next();
                }
                if (next && next->str() == MatchCompiler::makeConstString(">") && foundComparison)
                    continue;
            }

            while (!type.empty() && type.top()->str() == MatchCompiler::makeConstString("<"))
                type.pop();
        } else if (token->str() == MatchCompiler::makeConstString("<") && token->previous() && token->previous()->isName() && !token->previous()->varId()) {
            type.push(token);
            if (!templateToken && (token->previous()->str() == MatchCompiler::makeConstString("template")))
                templateToken = token;
        } else if (token->str() == MatchCompiler::makeConstString(">") || token->str() == MatchCompiler::makeConstString(">>")) {
            if (type.empty() || type.top()->str() != MatchCompiler::makeConstString("<")) // < and > don't match.
                continue;
            Token * const top1 = type.top();
            type.pop();
            Token * const top2 = type.empty() ? nullptr : type.top();
            type.push(top1);
            if (!top2 || top2->str() != MatchCompiler::makeConstString("<")) {
                if (token->str() == MatchCompiler::makeConstString(">>"))
                    continue;
                if (token->next() &&
                    !match210(token->next()) &&
                    !match211(token->next()) &&
                    !match212(token->next()))
                    continue;
            }
            // if > is followed by [ .. "new a<b>[" is expected
            if (token->strAt(1) == MatchCompiler::makeConstString("[")) {
                Token *prev = type.top()->previous();
                while (prev && match194(prev->previous()))
                    prev = prev->tokAt(-2);
                if (prev && prev->str() != MatchCompiler::makeConstString("new"))
                    prev = prev->previous();
                if (!prev || prev->str() != MatchCompiler::makeConstString("new"))
                    continue;
            }

            if (token->str() == MatchCompiler::makeConstString(">>")) {
                type.pop();
                type.pop();
                Token::createMutualLinks(top2, token);
                if (top1 == templateToken || top2 == templateToken)
                    templateToken = nullptr;
            } else {
                type.pop();
                Token::createMutualLinks(top1, token);
                if (top1 == templateToken)
                    templateToken = nullptr;
            }
        }
    }
}

void Tokenizer::sizeofAddParentheses()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match213(tok))
            continue;
        if (tok->next()->isLiteral() || match214(tok->next())) {
            Token *endToken = tok->next();
            while (match215(endToken))
                endToken = endToken->next();
            while (match216(endToken->next()) || (match217(endToken))) {
                if (match121(endToken->next()))
                    endToken = endToken->linkAt(1);
                else
                    endToken = endToken->next();
            }

            // Add ( after sizeof and ) behind endToken
            tok->insertToken("(");
            endToken->insertToken(")");
            Token::createMutualLinks(tok->next(), endToken->next());
        }
    }
}

bool Tokenizer::simplifySizeof()
{
    // Locate variable declarations and calculate the size
    std::map<unsigned int, unsigned int> sizeOfVar;
    std::map<unsigned int, const Token *> declTokOfVar;
    for (const Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->varId() != 0 && sizeOfVar.find(tok->varId()) == sizeOfVar.end()) {
            const unsigned int varId = tok->varId();
            if (match218(tok->tokAt(-3)) ||
                match219(tok->tokAt(-4)) ||
                match220(tok->tokAt(-2)) ||
                match221(tok->tokAt(-3))) {
                const unsigned int size = sizeOfType(tok->previous());
                if (size == 0) {
                    continue;
                }

                sizeOfVar[varId] = size;
                declTokOfVar[varId] = tok;
            }

            else if (match222(tok->previous()) ||
                     match223(tok->tokAt(-2))) {
                unsigned int size = sizeOfType(tok->previous());
                if (size == 0)
                    continue;

                const Token* tok2 = tok->next();
                do {
                    const MathLib::bigint num = MathLib::toLongNumber(tok2->strAt(1));
                    if (num<0)
                        break;
                    size *= (unsigned)num;
                    tok2 = tok2->tokAt(3);
                } while (match115(tok2));
                if (match224(tok2)) {
                    sizeOfVar[varId] = size;
                    declTokOfVar[varId] = tok;
                }
                if (!tok2) {
                    syntaxError(tok);
                }
                tok = tok2;
            }

            else if (match225(tok->previous()) ||
                     match226(tok->tokAt(-2))) {
                Token tempTok;
                tempTok.str("*");
                sizeOfVar[varId] = sizeOfType(&tempTok);
                declTokOfVar[varId] = tok;
            }
        }
    }

    bool ret = false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("sizeof"))
            continue;

        if (match211(tok->next())) {
            tok->deleteNext(3);
        }

        // sizeof('x')
        if (match227(tok->next())) {
            tok->deleteNext();
            tok->deleteThis();
            tok->deleteNext();
            std::ostringstream sz;
            sz << ((isC()) ? mSettings->sizeof_int : 1);
            tok->str(sz.str());
            ret = true;
            continue;
        }

        // sizeof ("text")
        if (match228(tok->next())) {
            tok->deleteNext();
            tok->deleteThis();
            tok->deleteNext();
            std::ostringstream ostr;
            ostr << (Token::getStrLength(tok) + 1);
            tok->str(ostr.str());
            ret = true;
            continue;
        }

        // sizeof(type *) => sizeof(*)
        if (match229(tok->next())) {
            tok->next()->deleteNext();
        }

        if (match230(tok->next())) {
            tok->str(MathLib::toString(sizeOfType(tok->tokAt(2))));
            tok->deleteNext(3);
            ret = true;
        }

        // sizeof( a )
        else if (match231(tok->next())) {
            const std::map<unsigned int, unsigned int>::const_iterator sizeOfVarPos = sizeOfVar.find(tok->tokAt(2)->varId());
            if (sizeOfVarPos != sizeOfVar.end()) {
                tok->deleteNext();
                tok->deleteThis();
                tok->deleteNext();
                tok->str(MathLib::toString(sizeOfVarPos->second));
                ret = true;
            } else {
                // don't try to replace size of variable if variable has
                // similar name with type (#329)
            }
        }

        else if (match232(tok->next())) {
            const unsigned int size = sizeOfType(tok->tokAt(2));
            if (size > 0) {
                tok->str(MathLib::toString(size));
                tok->deleteNext(3);
                ret = true;
            }
        }

        else if (match234(tok->next()) || match233(tok->next())) {
            unsigned int derefs = 0;

            const Token* nametok = tok->tokAt(2);
            if (nametok->str() == MatchCompiler::makeConstString("*")) {
                do {
                    nametok = nametok->next();
                    derefs++;
                } while (nametok && nametok->str() == MatchCompiler::makeConstString("*"));

                if (!match235(nametok))
                    continue;
            } else {
                const Token* tok2 = nametok->next();
                do {
                    tok2 = tok2->link()->next();
                    derefs++;
                } while (tok2 && tok2->str() == MatchCompiler::makeConstString("["));

                if (!tok2 || tok2->str() != MatchCompiler::makeConstString(")"))
                    continue;
            }

            // Some default value
            MathLib::biguint size = 0;

            const unsigned int varid = nametok->varId();
            if (derefs != 0 && varid != 0 && declTokOfVar.find(varid) != declTokOfVar.end()) {
                // Try to locate variable declaration..
                const Token *decltok = declTokOfVar[varid];
                if (match236(decltok->previous())) {
                    size = sizeOfType(decltok->previous());
                } else if (match237(decltok->tokAt(-2))) {
                    size = sizeOfType(decltok->tokAt(-2));
                }
                // Multi-dimensional array..
                if (match238(decltok) && match239(decltok->linkAt(1))) {
                    const Token *tok2 = decltok;
                    for (unsigned int i = 0; i < derefs; i++)
                        tok2 = tok2->linkAt(1); // Skip all dimensions that are dereferenced before the sizeof call
                    while (match240(tok2)) {
                        size *= MathLib::toULongNumber(tok2->strAt(2));
                        tok2 = tok2->linkAt(1);
                    }
                    if (match239(tok2))
                        continue;
                }
            } else if (nametok->strAt(1) == MatchCompiler::makeConstString("[") && nametok->isStandardType()) {
                size = sizeOfType(nametok);
                if (size == 0)
                    continue;
                const Token *tok2 = nametok->next();
                while (match115(tok2)) {
                    size *= MathLib::toULongNumber(tok2->strAt(1));
                    tok2 = tok2->link()->next();
                }
                if (!tok2 || tok2->str() != MatchCompiler::makeConstString(")"))
                    continue;
            }

            if (size > 0) {
                tok->str(MathLib::toString(size));
                Token::eraseTokens(tok, tok->next()->link()->next());
                ret = true;
            }
        }
    }
    return ret;
}

bool Tokenizer::simplifyTokenList1(const char FileName[])
{
    if (mSettings->terminated())
        return false;

    // if MACRO
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match241(tok)) {
            if (match242(tok)) {
                // 'for each ( )' -> 'asm ( )'
                tok->str("asm");
                tok->deleteNext();
            } else if (tok->strAt(1) == MatchCompiler::makeConstString("constexpr")) {
                tok->deleteNext();
            } else {
                syntaxError(tok);
            }
        }
    }

    // Is there C++ code in C file?
    validateC();

    // remove MACRO in variable declaration: MACRO int x;
    removeMacroInVarDecl();

    // Combine strings and character literals, e.g. L"string", L'c', "string1" "string2"
    combineStringAndCharLiterals();

    // replace inline SQL with "asm()" (Oracle PRO*C). Ticket: #1959
    simplifySQL();

    createLinks();

    // Remove __asm..
    simplifyAsm();

    // Bail out if code is garbage
    if (mTimerResults) {
        Timer t("Tokenizer::tokenize::findGarbageCode", mSettings->showtime, mTimerResults);
        findGarbageCode();
    } else {
        findGarbageCode();
    }

    checkConfiguration();

    // if (x) MACRO() ..
    for (const Token *tok = list.front(); tok; tok = tok->next()) {
        if (match243(tok)) {
            tok = tok->next()->link();
            if (match198(tok) &&
                tok->next()->isUpperCaseName() &&
                match244(tok->linkAt(2))) {
                syntaxError(tok->next());
            }
        }
    }

    if (mSettings->terminated())
        return false;

    // convert C++17 style nested namespaces to old style namespaces
    simplifyNestedNamespace();

    // simplify namespace aliases
    simplifyNamespaceAliases();

    // Remove [[attribute]]
    simplifyCPPAttribute();

    // remove __attribute__((?))
    simplifyAttribute();

    // Combine tokens..
    combineOperators();

    // Simplify the C alternative tokens (and, or, etc.)
    simplifyCAlternativeTokens();

    // replace 'sin(0)' to '0' and other similar math expressions
    simplifyMathExpressions();

    // combine "- %num%"
    concatenateNegativeNumberAndAnyPositive();

    // remove extern "C" and extern "C" {}
    if (isCPP())
        simplifyExternC();

    // simplify weird but legal code: "[;{}] ( { code; } ) ;"->"[;{}] code;"
    simplifyRoundCurlyParentheses();

    // check for simple syntax errors..
    for (const Token *tok = list.front(); tok; tok = tok->next()) {
        if (match245(tok) &&
            match168(tok->linkAt(2))) {
            syntaxError(tok);
        }
    }

    if (!simplifyAddBraces())
        return false;

    sizeofAddParentheses();

    // Simplify: 0[foo] -> *(foo)
    for (Token* tok = list.front(); tok; tok = tok->next()) {
        if (match246(tok) && tok->linkAt(1)) {
            tok->str("*");
            tok->next()->str("(");
            tok->linkAt(1)->str(")");
        }
    }

    if (mSettings->terminated())
        return false;

    // Remove "inline", "register", and "restrict"
    simplifyKeyword();

    // simplify simple calculations inside <..>
    if (isCPP()) {
        Token *lt = nullptr;
        for (Token *tok = list.front(); tok; tok = tok->next()) {
            if (match88(tok))
                lt = nullptr;
            else if (match247(tok))
                lt = tok->next();
            else if (lt && match248(tok)) {
                const Token * const end = tok;
                for (tok = lt; tok != end; tok = tok->next()) {
                    if (tok->isNumber())
                        mTemplateSimplifier->simplifyNumericCalculations(tok);
                }
                lt = tok->next();
            }
        }
    }

    // Convert K&R function declarations to modern C
    simplifyVarDecl(true);
    simplifyFunctionParameters();

    // simplify case ranges (gcc extension)
    simplifyCaseRange();

    // simplify labels and 'case|default'-like syntaxes
    simplifyLabelsCaseDefault();

    // simplify '[;{}] * & ( %any% ) =' to '%any% ='
    simplifyMulAndParens();

    if (!isC() && !mSettings->library.markupFile(FileName)) {
        findComplicatedSyntaxErrorsInTemplates();
    }

    if (mSettings->terminated())
        return false;

    // remove calling conventions __cdecl, __stdcall..
    simplifyCallingConvention();

    // Remove __declspec()
    simplifyDeclspec();
    validate();
    // remove some unhandled macros in global scope
    removeMacrosInGlobalScope();

    // remove undefined macro in class definition:
    // class DLLEXPORT Fred { };
    // class Fred FINAL : Base { };
    removeMacroInClassDef();

    // That call here fixes #7190
    validate();

    // remove unnecessary member qualification..
    removeUnnecessaryQualification();

    // convert Microsoft memory functions
    simplifyMicrosoftMemoryFunctions();

    // convert Microsoft string functions
    simplifyMicrosoftStringFunctions();

    if (mSettings->terminated())
        return false;

    // Remove Qt signals and slots
    simplifyQtSignalsSlots();

    // remove Borland stuff..
    simplifyBorland();

    // syntax error: enum with typedef in it
    checkForEnumsWithTypedef();

    // Add parentheses to ternary operator where necessary
    prepareTernaryOpForAST();

    // Change initialisation of variable to assignment
    simplifyInitVar();

    // Split up variable declarations.
    simplifyVarDecl(false);

    // typedef..
    if (mTimerResults) {
        Timer t("Tokenizer::tokenize::simplifyTypedef", mSettings->showtime, mTimerResults);
        simplifyTypedef();
    } else {
        simplifyTypedef();
    }

    // Add parentheses to ternary operator where necessary
    // TODO: this is only necessary if one typedef simplification had a comma and was used within ?:
    // If typedef handling is refactored and moved to symboldatabase someday we can remove this
    prepareTernaryOpForAST();

    for (Token* tok = list.front(); tok;) {
        if (match249(tok))
            tok->deleteNext();
        else
            tok = tok->next();
    }

    // class x y {
    if (isCPP() && mSettings->isEnabled(Settings::INFORMATION)) {
        for (const Token *tok = list.front(); tok; tok = tok->next()) {
            if (match250(tok)) {
                unhandled_macro_class_x_y(tok);
            }
        }
    }

    // catch bad typedef canonicalization
    //
    // to reproduce bad typedef, download upx-ucl from:
    // http://packages.debian.org/sid/upx-ucl
    // analyse the file src/stub/src/i386-linux.elf.interp-main.c
    validate();

    // The simplify enum have inner loops
    if (mSettings->terminated())
        return false;

    // Put ^{} statements in asm()
    simplifyAsm2();

    // Order keywords "static" and "const"
    simplifyStaticConst();

    // convert platform dependent types to standard types
    // 32 bits: size_t -> unsigned long
    // 64 bits: size_t -> unsigned long long
    simplifyPlatformTypes();

    // collapse compound standard types into a single token
    // unsigned long long int => long (with _isUnsigned=true,_isLong=true)
    list.simplifyStdType();

    if (mSettings->terminated())
        return false;

    // simplify bit fields..
    simplifyBitfields();

    if (mSettings->terminated())
        return false;

    // struct simplification "struct S {} s; => struct S { } ; S s ;
    simplifyStructDecl();

    if (mSettings->terminated())
        return false;

    // x = ({ 123; });  =>   { x = 123; }
    simplifyAssignmentBlock();

    if (mSettings->terminated())
        return false;

    simplifyVariableMultipleAssign();

    // Collapse operator name tokens into single token
    // operator = => operator=
    simplifyOperatorName();

    // Remove redundant parentheses
    simplifyRedundantParentheses();

    if (!isC()) {
        // TODO: Only simplify template parameters
        for (Token *tok = list.front(); tok; tok = tok->next())
            while (mTemplateSimplifier->simplifyNumericCalculations(tok))
                ;

        // Handle templates..
        simplifyTemplates();

        // The simplifyTemplates have inner loops
        if (mSettings->terminated())
            return false;

        // sometimes the "simplifyTemplates" fail and then unsimplified
        // function calls etc remain. These have the "wrong" syntax. So
        // this function will just fix so that the syntax is corrected.
        validate(); // #6847 - invalid code
        mTemplateSimplifier->cleanupAfterSimplify();
    }

    // Simplify pointer to standard types (C only)
    simplifyPointerToStandardType();

    // simplify function pointers
    simplifyFunctionPointers();

    // Change initialisation of variable to assignment
    simplifyInitVar();

    // Split up variable declarations.
    simplifyVarDecl(false);

    validate(); // #6772 "segmentation fault (invalid code) in Tokenizer::setVarId"

    if (mTimerResults) {
        Timer t("Tokenizer::tokenize::setVarId", mSettings->showtime, mTimerResults);
        setVarId();
    } else {
        setVarId();
    }

    // Link < with >
    createLinks2();

    // specify array size
    arraySize();

    // The simplify enum might have inner loops
    if (mSettings->terminated())
        return false;

    // Add std:: in front of std classes, when using namespace std; was given
    simplifyNamespaceStd();

    // Change initialisation of variable to assignment
    simplifyInitVar();

    // Convert e.g. atol("0") into 0
    simplifyMathFunctions();

    simplifyDoublePlusAndDoubleMinus();

    simplifyArrayAccessSyntax();

    Token::assignProgressValues(list.front());

    removeRedundantSemicolons();

    simplifyParameterVoid();

    simplifyRedundantConsecutiveBraces();

    simplifyEmptyNamespaces();

    elseif();

    SimplifyNamelessRValueReferences();


    validate();
    return true;
}

bool Tokenizer::simplifyTokenList2()
{
    // clear the _functionList so it can't contain dead pointers
    deleteSymbolDatabase();

    // Clear AST,ValueFlow. These will be created again at the end of this function.
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        tok->clearAst();
        tok->clearValueFlow();
    }

    // f(x=g())   =>   x=g(); f(x)
    simplifyAssignmentInFunctionCall();

    // ";a+=b;" => ";a=a+b;"
    simplifyCompoundAssignment();

    simplifyCharAt();

    // simplify references
    simplifyReference();

    simplifyStd();

    if (mSettings->terminated())
        return false;

    simplifySizeof();

    simplifyUndefinedSizeArray();

    simplifyCasts();

    // Simplify simple calculations before replace constants, this allows the replacement of constants that are calculated
    // e.g. const static int value = sizeof(X)/sizeof(Y);
    simplifyCalculations();

    if (mSettings->terminated())
        return false;

    // Replace "*(ptr + num)" => "ptr[num]"
    simplifyOffsetPointerDereference();

    // Replace "&str[num]" => "(str + num)"
    simplifyOffsetPointerReference();

    removeRedundantAssignment();

    simplifyRealloc();

    // Change initialisation of variable to assignment
    simplifyInitVar();

    // Simplify variable declarations
    simplifyVarDecl(false);

    simplifyErrNoInWhile();
    simplifyIfAndWhileAssign();
    simplifyRedundantParentheses();
    simplifyNestedStrcat();
    simplifyFuncInWhile();

    simplifyIfAndWhileAssign();

    // replace strlen(str)
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match251(tok)) {
            tok->str(MathLib::toString(Token::getStrLength(tok->tokAt(2))));
            tok->deleteNext(3);
        }
    }

    bool modified = true;
    while (modified) {
        if (mSettings->terminated())
            return false;

        modified = false;
        modified |= simplifyConditions();
        modified |= simplifyFunctionReturn();
        modified |= simplifyKnownVariables();
        modified |= simplifyStrlen();

        modified |= removeRedundantConditions();
        modified |= simplifyRedundantParentheses();
        modified |= simplifyConstTernaryOp();
        modified |= simplifyCalculations();
        validate();
    }

    // simplify redundant loops
    simplifyWhile0();
    removeRedundantFor();

    // Remove redundant parentheses in return..
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        while (match252(tok)) {
            Token *tok2 = tok->next()->link();
            if (match164(tok2)) {
                tok->deleteNext();
                tok2->deleteThis();
            } else {
                break;
            }
        }
    }

    simplifyReturnStrncat();

    removeRedundantAssignment();

    simplifyComma();

    removeRedundantSemicolons();

    simplifyFlowControl();

    simplifyRedundantConsecutiveBraces();

    simplifyEmptyNamespaces();

    simplifyMathFunctions();

    validate();

    Token::assignProgressValues(list.front());

    list.createAst();
    // needed for #7208 (garbage code) and #7724 (ast max depth limit)
    list.validateAst();

    // Create symbol database and then remove const keywords
    createSymbolDatabase();
    mSymbolDatabase->setValueTypeInTokenList();

    ValueFlow::setValues(&list, mSymbolDatabase, mErrorLogger, mSettings);

    if (mSettings->terminated())
        return false;

    printDebugOutput(2);

    return true;
}
//---------------------------------------------------------------------------

void Tokenizer::printDebugOutput(unsigned int simplification) const
{
    const bool debug = (simplification != 1U && mSettings->debugSimplified) ||
                       (simplification != 2U && mSettings->debugnormal);

    if (debug && list.front()) {
        list.front()->printOut(nullptr, list.getFiles());

        if (mSettings->xml)
            std::cout << "<debug>" << std::endl;

        if (mSymbolDatabase) {
            if (mSettings->xml)
                mSymbolDatabase->printXml(std::cout);
            else if (mSettings->verbose) {
                mSymbolDatabase->printOut("Symbol database");
            }
        }

        if (mSettings->verbose)
            list.front()->printAst(mSettings->verbose, mSettings->xml, std::cout);

        list.front()->printValueFlow(mSettings->xml, std::cout);

        if (mSettings->xml)
            std::cout << "</debug>" << std::endl;
    }

    if (mSymbolDatabase && simplification == 2U && mSettings->debugwarnings) {
        printUnknownTypes();

        // the typeStartToken() should come before typeEndToken()
        for (const Variable *var : mSymbolDatabase->variableList()) {
            if (!var)
                continue;

            const Token * typetok = var->typeStartToken();
            while (typetok && typetok != var->typeEndToken())
                typetok = typetok->next();

            if (typetok != var->typeEndToken()) {
                reportError(var->typeStartToken(),
                            Severity::debug,
                            "debug",
                            "Variable::typeStartToken() of variable '" + var->name() + "' is not located before Variable::typeEndToken(). The location of the typeStartToken() is '" + var->typeStartToken()->str() + "' at line " + MathLib::toString(var->typeStartToken()->linenr()));
            }
        }
    }
}

void Tokenizer::dump(std::ostream &out) const
{
    // Create a xml data dump.
    // The idea is not that this will be readable for humans. It's a
    // data dump that 3rd party tools could load and get useful info from.

    // tokens..
    out << "  <tokenlist>" << std::endl;
    for (const Token *tok = list.front(); tok; tok = tok->next()) {
        out << "    <token id=\"" << tok << "\" file=\"" << ErrorLogger::toxml(list.file(tok)) << "\" linenr=\"" << tok->linenr() << '\"';
        out << " str=\"" << ErrorLogger::toxml(tok->str()) << '\"';
        out << " scope=\"" << tok->scope() << '\"';
        if (tok->isName()) {
            out << " type=\"name\"";
            if (tok->isUnsigned())
                out << " isUnsigned=\"true\"";
            else if (tok->isSigned())
                out << " isSigned=\"true\"";
        } else if (tok->isNumber()) {
            out << " type=\"number\"";
            if (MathLib::isInt(tok->str()))
                out << " isInt=\"True\"";
            if (MathLib::isFloat(tok->str()))
                out << " isFloat=\"True\"";
        } else if (tok->tokType() == Token::eString)
            out << " type=\"string\" strlen=\"" << Token::getStrLength(tok) << '\"';
        else if (tok->tokType() == Token::eChar)
            out << " type=\"char\"";
        else if (tok->isBoolean())
            out << " type=\"boolean\"";
        else if (tok->isOp()) {
            out << " type=\"op\"";
            if (tok->isArithmeticalOp())
                out << " isArithmeticalOp=\"True\"";
            else if (tok->isAssignmentOp())
                out << " isAssignmentOp=\"True\"";
            else if (tok->isComparisonOp())
                out << " isComparisonOp=\"True\"";
            else if (tok->tokType() == Token::eLogicalOp)
                out << " isLogicalOp=\"True\"";
        }
        if (tok->link())
            out << " link=\"" << tok->link() << '\"';
        if (tok->varId() > 0U)
            out << " varId=\"" << MathLib::toString(tok->varId()) << '\"';
        if (tok->variable())
            out << " variable=\"" << tok->variable() << '\"';
        if (tok->function())
            out << " function=\"" << tok->function() << '\"';
        if (!tok->values().empty())
            out << " values=\"" << &tok->values() << '\"';
        if (tok->type())
            out << " type-scope=\"" << tok->type()->classScope << '\"';
        if (tok->astParent())
            out << " astParent=\"" << tok->astParent() << '\"';
        if (tok->astOperand1())
            out << " astOperand1=\"" << tok->astOperand1() << '\"';
        if (tok->astOperand2())
            out << " astOperand2=\"" << tok->astOperand2() << '\"';
        if (!tok->originalName().empty())
            out << " originalName=\"" << tok->originalName() << '\"';
        if (tok->valueType()) {
            const std::string vt = tok->valueType()->dump();
            if (!vt.empty())
                out << ' ' << vt;
        }
        out << "/>" << std::endl;
    }
    out << "  </tokenlist>" << std::endl;

    mSymbolDatabase->printXml(out);
    if (list.front())
        list.front()->printValueFlow(true, out);
}

void Tokenizer::removeMacrosInGlobalScope()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("(")) {
            tok = tok->link();
            if (match253(tok) &&
                !match254(tok->next()))
                tok->deleteNext();
        }

        if ((!tok->previous() || match88(tok->previous())) &&
            match17(tok) && tok->isUpperCaseName()) {
            const Token *tok2 = tok->next();
            if (tok2 && tok2->str() == MatchCompiler::makeConstString("("))
                tok2 = tok2->link()->next();

            // remove unknown macros before namespace|class|struct|union
            if (match255(tok2)) {
                // is there a "{" for?
                const Token *tok3 = tok2;
                while (tok3 && !match256(tok3))
                    tok3 = tok3->next();
                if (tok3 && tok3->str() == MatchCompiler::makeConstString("{")) {
                    Token::eraseTokens(tok, tok2);
                    tok->deleteThis();
                }
                continue;
            }

            // replace unknown macros before foo(
            if (match83(tok2) && isFunctionHead(tok2->next(), "{")) {
                std::string typeName;
                for (const Token* tok3 = tok; tok3 != tok2; tok3 = tok3->next())
                    typeName += tok3->str();
                Token::eraseTokens(tok, tok2);
                tok->str(typeName);
            }

            // remove unknown macros before foo::foo(
            if (match257(tok2)) {
                const Token *tok3 = tok2;
                while (match258(tok3))
                    tok3 = tok3->tokAt(2);
                if (match259(tok3) && tok3->str() == tok3->strAt(2)) {
                    Token::eraseTokens(tok, tok2);
                    tok->deleteThis();
                }
                continue;
            }
        }

        // Skip executable scopes
        if (tok->str() == MatchCompiler::makeConstString("{")) {
            const Token *prev = tok->previous();
            while (prev && prev->isName())
                prev = prev->previous();
            if (prev && prev->str() == MatchCompiler::makeConstString(")"))
                tok = tok->link();
        }
    }
}

//---------------------------------------------------------------------------

void Tokenizer::removeMacroInClassDef()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match260(tok))
            continue;

        const bool nextIsUppercase = tok->next()->isUpperCaseName();
        const bool afterNextIsUppercase = tok->tokAt(2)->isUpperCaseName();
        if (nextIsUppercase && !afterNextIsUppercase)
            tok->deleteNext();
        else if (!nextIsUppercase && afterNextIsUppercase)
            tok->next()->deleteNext();
    }
}

//---------------------------------------------------------------------------

void Tokenizer::removeMacroInVarDecl()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match261(tok) && tok->next()->isUpperCaseName()) {
            // goto ')' parentheses
            const Token *tok2 = tok;
            int parlevel = 0;
            while (tok2) {
                if (tok2->str() == MatchCompiler::makeConstString("("))
                    ++parlevel;
                else if (tok2->str() == MatchCompiler::makeConstString(")")) {
                    if (--parlevel <= 0)
                        break;
                }
                tok2 = tok2->next();
            }
            tok2 = tok2 ? tok2->next() : nullptr;

            // check if this is a variable declaration..
            const Token *tok3 = tok2;
            while (tok3 && tok3->isUpperCaseName())
                tok3 = tok3->next();
            if (tok3 && (tok3->isStandardType() || match262(tok3)))
                Token::eraseTokens(tok,tok2);
        }
    }
}
//---------------------------------------------------------------------------

void Tokenizer::removeRedundantAssignment()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("{"))
            tok = tok->link();

        const Token * const start = const_cast<Token *>(startOfExecutableScope(tok));
        if (start) {
            tok = start->previous();
            // parse in this function..
            std::set<unsigned int> localvars;
            const Token * const end = tok->next()->link();
            for (Token * tok2 = tok->next(); tok2 && tok2 != end; tok2 = tok2->next()) {
                // skip local class or struct
                if (match263(tok2)) {
                    // skip to '{'
                    tok2 = tok2->tokAt(2);
                    while (tok2 && tok2->str() != MatchCompiler::makeConstString("{"))
                        tok2 = tok2->next();

                    if (tok2)
                        tok2 = tok2->link(); // skip local class or struct
                    else
                        return;
                } else if (match264(tok2) && tok2->next()->str() != MatchCompiler::makeConstString("return")) {
                    tok2 = tok2->tokAt(3);
                    localvars.insert(tok2->varId());
                } else if (match265(tok2) && tok2->next()->isStandardType()) {
                    tok2 = tok2->tokAt(2);
                    localvars.insert(tok2->varId());
                } else if (tok2->varId() &&
                           !match266(tok2->previous())) {
                    localvars.erase(tok2->varId());
                }
            }
            localvars.erase(0);
            if (!localvars.empty()) {
                for (Token *tok2 = tok->next(); tok2 && tok2 != end;) {
                    if (match265(tok2) && localvars.find(tok2->tokAt(2)->varId()) != localvars.end()) {
                        tok2->deleteNext(3);
                    } else if ((match264(tok2) &&
                                localvars.find(tok2->tokAt(3)->varId()) != localvars.end()) ||
                               (match267(tok2) &&
                                localvars.find(tok2->next()->varId()) != localvars.end())) {
                        tok2->deleteNext(4);
                    } else
                        tok2 = tok2->next();
                }
            }
        }
    }
}

void Tokenizer::simplifyRealloc()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match121(tok) ||
            (tok->str() == MatchCompiler::makeConstString("{") && tok->previous() && tok->previous()->str() == MatchCompiler::makeConstString("=")))
            tok = tok->link();
        else if (match268(tok)) {
            tok = tok->tokAt(3);
            if (match269(tok->next())) {
                //no "x = realloc(0,);"
                if (!match164(tok->next()->link()) || tok->next()->link()->previous() == tok->tokAt(3))
                    continue;

                // delete "0 ,"
                tok->next()->deleteNext(2);

                // Change function name "realloc" to "malloc"
                tok->str("malloc");
                tok = tok->next()->link();
            } else {
                Token *tok2 = tok->next()->link()->tokAt(-2);
                //no "x = realloc(,0);"
                if (!match270(tok2) || tok2 == tok->tokAt(2))
                    continue;

                //remove ", 0"
                tok2 = tok2->previous();
                tok2->deleteNext(2);
                //change "realloc" to "free"
                tok->str("free");
                //insert "0" after "var ="
                tok = tok->previous();
                tok->insertToken("0");
                //move "var = 0" between "free(...)" and ";"
                tok2 = tok2->next();
                Token::move(tok->previous(), tok->next(), tok2);
                //add missing ";" after "free(...)"
                tok2->insertToken(";");
                //goto before last ";" and continue
                tok = tok->next();
            }
        }
    }
}

void Tokenizer::simplifyEmptyNamespaces()
{
    if (isC())
        return;

    bool goback = false;
    for (Token *tok = list.front(); tok; tok = tok ? tok->next() : nullptr) {
        if (goback) {
            tok = tok->previous();
            goback = false;
        }
        if (match271(tok)) {
            tok = tok->link();
            continue;
        }
        if (!match187(tok))
            continue;
        if (tok->strAt(3) == MatchCompiler::makeConstString("}")) {
            tok->deleteNext(3);             // remove '%name% { }'
            if (!tok->previous()) {
                // remove 'namespace' or replace it with ';' if isolated
                tok->deleteThis();
                goback = true;
            } else {                    // '%any% namespace %any%'
                tok = tok->previous();  // goto previous token
                tok->deleteNext();      // remove next token: 'namespace'
            }
        } else {
            tok = tok->tokAt(2);
        }
    }
}

void Tokenizer::simplifyFlowControl()
{
    for (Token *begin = list.front(); begin; begin = begin->next()) {

        if (match121(begin) ||
            (begin->str() == MatchCompiler::makeConstString("{") && begin->previous() && begin->strAt(-1) == MatchCompiler::makeConstString("=")))
            begin = begin->link();

        //function scope
        if (!match19(begin) && !match272(begin))
            continue;

        Token* end = begin->linkAt(1+(begin->next()->str() == MatchCompiler::makeConstString("{") ? 0 : 1));
        unsigned int indentLevel = 0;
        bool stilldead = false;

        for (Token *tok = begin; tok && tok != end; tok = tok->next()) {
            if (match121(tok)) {
                tok = tok->link();
                continue;
            }

            if (tok->str() == MatchCompiler::makeConstString("{")) {
                if (tok->previous() && tok->previous()->str() == MatchCompiler::makeConstString("=")) {
                    tok = tok->link();
                    continue;
                }
                ++indentLevel;
            } else if (tok->str() == MatchCompiler::makeConstString("}")) {
                if (indentLevel == 0)
                    break;
                --indentLevel;
                if (stilldead) {
                    eraseDeadCode(tok, nullptr);
                    if (indentLevel == 1 || tok->next()->str() != MatchCompiler::makeConstString("}") || !match273(tok->next()->link()->previous()))
                        stilldead = false;
                    continue;
                }
            }

            if (indentLevel == 0)
                continue;

            if (match274(tok)) {
                tok = tok->next();
                eraseDeadCode(tok, nullptr);

            } else if (match275(tok) ||
                       (match261(tok->previous()) &&
                        mSettings->library.isnoreturn(tok)) ||
                       (isCPP() && tok->str() == MatchCompiler::makeConstString("throw"))) {
                if (tok->next()->str() == MatchCompiler::makeConstString("}"))
                    syntaxError(tok->next()); // invalid code like in #6731
                //TODO: ensure that we exclude user-defined 'exit|abort|throw', except for 'noreturn'
                //catch the first ';'
                for (Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                    if (match121(tok2)) {
                        tok2 = tok2->link();
                    } else if (tok2->str() == MatchCompiler::makeConstString(";")) {
                        tok = tok2;
                        eraseDeadCode(tok, nullptr);
                        break;
                    } else if (match276(tok2))
                        break;
                }
                //if everything is removed, then remove also the code after an inferior scope
                //only if the actual scope is not special
                if (indentLevel > 1 && tok->next()->str() == MatchCompiler::makeConstString("}") && match273(tok->next()->link()->previous()))
                    stilldead = true;
            }
        }
        begin = end;
    }
}


bool Tokenizer::removeRedundantConditions()
{
    // Return value for function. Set to true if there are any simplifications
    bool ret = false;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match277(tok))
            continue;

        // Find matching else
        Token *elseTag = tok->linkAt(4)->next();

        const bool boolValue = (tok->strAt(2) == MatchCompiler::makeConstString("true"));

        // Handle if with else
        if (match278(elseTag)) {
            // Handle else
            if (!boolValue) {
                // Convert "if( false ) {aaa;} else {bbb;}" => "{bbb;}"

                //remove '(false)'
                tok->deleteNext(3);
                //delete dead code inside scope
                eraseDeadCode(tok, elseTag);
                //remove 'else'
                elseTag->deleteThis();
                //remove 'if'
                tok->deleteThis();
            } else {
                // Convert "if( true ) {aaa;} else {bbb;}" => "{aaa;}"
                const Token *end = elseTag->next()->link()->next();

                // Remove "else { bbb; }"
                elseTag = elseTag->previous();
                eraseDeadCode(elseTag, end);

                // Remove "if( true )"
                tok->deleteNext(3);
                tok->deleteThis();
            }

            ret = true;
        }

        // Handle if without else
        else {
            if (!boolValue) {
                //remove '(false)'
                tok->deleteNext(3);
                //delete dead code inside scope
                eraseDeadCode(tok, elseTag);
                //remove 'if'
                tok->deleteThis();
            } else {
                // convert "if( true ) {aaa;}" => "{aaa;}"
                tok->deleteNext(3);
                tok->deleteThis();
            }

            ret = true;
        }
    }

    return ret;
}

void Tokenizer::removeRedundantFor()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match279(tok) ||
            match280(tok)) {
            // Same variable name..
            const Token* varTok = tok->tokAt(3);
            const bool type = varTok->next()->isName();
            if (type)
                varTok = varTok->next();
            const std::string varname(varTok->str());
            const unsigned int varid(varTok->varId());
            if (varname != varTok->strAt(4))
                continue;
            const Token *vartok2 = tok->linkAt(2)->previous();
            if (vartok2->str() == MatchCompiler::makeConstString("++"))
                vartok2 = vartok2->previous();
            else if (vartok2->strAt(-1) != MatchCompiler::makeConstString("++"))
                continue;
            if (varname != vartok2->str())
                continue;

            // Check that the difference of the numeric values is 1
            const MathLib::bigint num1(MathLib::toLongNumber(varTok->strAt(2)));
            const MathLib::bigint num2(MathLib::toLongNumber(varTok->strAt(6)));
            if (num1 + 1 != num2)
                continue;

            // check how loop variable is used in loop..
            bool read = false;
            bool write = false;
            const Token* end = tok->linkAt(2)->next()->link();
            for (const Token *tok2 = tok->linkAt(2); tok2 != end; tok2 = tok2->next()) {
                if (tok2->str() == varname) {
                    if (tok2->previous()->isArithmeticalOp() &&
                        tok2->next() &&
                        (tok2->next()->isArithmeticalOp() || tok2->next()->str() == MatchCompiler::makeConstString(";"))) {
                        read = true;
                    } else {
                        read = write = true;
                        break;
                    }
                }
            }

            // Simplify loop if loop variable isn't written
            if (!write) {
                Token* bodyBegin = tok->linkAt(2)->next();
                // remove "for ("
                tok->deleteNext(2);

                // If loop variable is read then keep assignment before
                // loop body..
                if (type) {
                    tok->insertToken("{");
                    Token::createMutualLinks(tok->next(), bodyBegin->link());
                    bodyBegin->deleteThis();
                    tok = tok->tokAt(6);
                } else if (read) {
                    // goto ";"
                    tok = tok->tokAt(4);
                } else {
                    // remove "x = 0 ;"
                    tok->deleteNext(4);
                }

                // remove "x < 1 ; x ++ )"
                tok->deleteNext(7);

                if (!type) {
                    // Add assignment after the loop body so the loop variable
                    // get the correct end value
                    Token *tok2 = tok->next()->link();
                    tok2->insertToken(";");
                    tok2->insertToken(MathLib::toString(num2));
                    tok2->insertToken("=");
                    tok2->insertToken(varname);
                    tok2->next()->varId(varid);
                }
            }
        }
    }
}


void Tokenizer::removeRedundantSemicolons()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->link() && tok->str() == MatchCompiler::makeConstString("(")) {
            tok = tok->link();
            continue;
        }
        for (;;) {
            if (match281(tok)) {
                tok->deleteNext();
            } else if (match282(tok)) {
                tok->deleteNext(3);
            } else {
                break;
            }
        }
    }
}


bool Tokenizer::simplifyAddBraces()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        Token const * tokRet=simplifyAddBracesToCommand(tok);
        if (!tokRet)
            return false;
    }
    return true;
}

Token *Tokenizer::simplifyAddBracesToCommand(Token *tok)
{
    Token * tokEnd=tok;
    if (match283(tok)) {
        tokEnd=simplifyAddBracesPair(tok,true);
    } else if (tok->str()==MatchCompiler::makeConstString("while")) {
        Token *tokPossibleDo=tok->previous();
        if (match197(tok->previous()))
            tokPossibleDo = nullptr;
        else if (match284(tokPossibleDo))
            tokPossibleDo = tokPossibleDo->link();
        if (!tokPossibleDo || tokPossibleDo->strAt(-1) != MatchCompiler::makeConstString("do"))
            tokEnd=simplifyAddBracesPair(tok,true);
    } else if (tok->str()==MatchCompiler::makeConstString("do")) {
        tokEnd=simplifyAddBracesPair(tok,false);
        if (tokEnd!=tok) {
            // walk on to next token, i.e. "while"
            // such that simplifyAddBracesPair does not close other braces
            // before the "while"
            if (tokEnd) {
                tokEnd=tokEnd->next();
                if (!tokEnd || tokEnd->str()!=MatchCompiler::makeConstString("while")) // no while
                    syntaxError(tok);
            }
        }
    } else if (tok->str()==MatchCompiler::makeConstString("if")) {
        tokEnd=simplifyAddBracesPair(tok,true);
        if (!tokEnd)
            return nullptr;
        if (tokEnd->strAt(1) == MatchCompiler::makeConstString("else")) {
            Token * tokEndNextNext= tokEnd->tokAt(2);
            if (!tokEndNextNext || tokEndNextNext->str() == MatchCompiler::makeConstString("}"))
                syntaxError(tokEndNextNext);
            if (tokEndNextNext->str() == MatchCompiler::makeConstString("if"))
                // do not change "else if ..." to "else { if ... }"
                tokEnd=simplifyAddBracesToCommand(tokEndNextNext);
            else
                tokEnd=simplifyAddBracesPair(tokEnd->next(),false);
        }
    }

    return tokEnd;
}

Token *Tokenizer::simplifyAddBracesPair(Token *tok, bool commandWithCondition)
{
    Token * tokCondition=tok->next();
    if (!tokCondition) // Missing condition
        return tok;

    Token *tokAfterCondition=tokCondition;
    if (commandWithCondition) {
        if (tokCondition->str()==MatchCompiler::makeConstString("("))
            tokAfterCondition=tokCondition->link();
        else
            syntaxError(tok); // Bad condition

        if (!tokAfterCondition || tokAfterCondition->strAt(1) == MatchCompiler::makeConstString("]"))
            syntaxError(tok); // Bad condition

        tokAfterCondition=tokAfterCondition->next();
        if (!tokAfterCondition || match285(tokAfterCondition)) {
            // No tokens left where to add braces around
            return tok;
        }
    }
    Token * tokBracesEnd=nullptr;
    if (tokAfterCondition->str()==MatchCompiler::makeConstString("{")) {
        // already surrounded by braces
        tokBracesEnd=tokAfterCondition->link();
    } else if (match286(tokAfterCondition) &&
               match287(tokAfterCondition->linkAt(1))) {
        tokAfterCondition->previous()->insertToken("{");
        Token * tokOpenBrace = tokAfterCondition->previous();
        Token * tokEnd = tokAfterCondition->linkAt(1)->linkAt(2)->linkAt(1);
        if (!tokEnd) {
            syntaxError(tokAfterCondition);
        }
        tokEnd->insertToken("}");
        Token * tokCloseBrace = tokEnd->next();

        Token::createMutualLinks(tokOpenBrace, tokCloseBrace);
        tokBracesEnd = tokCloseBrace;
    } else if (match288(tokAfterCondition)) {
        tokAfterCondition->previous()->insertToken("{");
        tokAfterCondition->linkAt(2)->insertToken("}");
        tokBracesEnd = tokAfterCondition->linkAt(2)->next();
        Token::createMutualLinks(tokAfterCondition->previous(), tokBracesEnd);
    } else {
        Token * tokEnd = simplifyAddBracesToCommand(tokAfterCondition);
        if (!tokEnd) // Ticket #4887
            return tok;
        if (tokEnd->str()!=MatchCompiler::makeConstString("}")) {
            // Token does not end with brace
            // Look for ; to add own closing brace after it
            while (tokEnd && !match289(tokEnd)) {
                if (tokEnd->tokType()==Token::eBracket || tokEnd->str() == MatchCompiler::makeConstString("(")) {
                    tokEnd = tokEnd->link();
                    if (!tokEnd) {
                        // Inner bracket does not close
                        return tok;
                    }
                }
                tokEnd=tokEnd->next();
            }
            if (!tokEnd || tokEnd->str() != MatchCompiler::makeConstString(";")) {
                // No trailing ;
                return tok;
            }
        }

        tokAfterCondition->previous()->insertToken("{");
        Token * tokOpenBrace=tokAfterCondition->previous();

        tokEnd->insertToken("}");
        Token * TokCloseBrace=tokEnd->next();

        Token::createMutualLinks(tokOpenBrace,TokCloseBrace);
        tokBracesEnd=TokCloseBrace;
    }

    return tokBracesEnd;
}

void Tokenizer::simplifyCompoundAssignment()
{
    // Simplify compound assignments:
    // "a+=b" => "a = a + b"
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match290(tok))
            continue;
        // backup current token..
        Token * const tok1 = tok;

        if (tok->next()->str() == MatchCompiler::makeConstString("*"))
            tok = tok->next();

        if (tok->next() && tok->next()->str() == MatchCompiler::makeConstString("(")) {
            tok = tok->next()->link()->next();
        } else {
            // variable..
            tok = tok->tokAt(2);
            while (match291(tok) ||
                   match292(tok)) {
                if (tok->str() == MatchCompiler::makeConstString("."))
                    tok = tok->tokAt(2);
                else {
                    // goto "]" or ")"
                    tok = tok->link();

                    // goto next token..
                    tok = tok ? tok->next() : nullptr;
                }
            }
        }
        if (!tok)
            break;

        // Is current token at a compound assignment: +=|-=|.. ?
        const std::string &str = tok->str();
        std::string op;  // operator used in assignment
        if (tok->isAssignmentOp() && str.size() == 2)
            op = str.substr(0, 1);
        else if (tok->isAssignmentOp() && str.size() == 3)
            op = str.substr(0, 2);
        else {
            tok = tok1;
            continue;
        }

        // Remove the whole statement if it says: "+=0;", "-=0;", "*=1;" or "/=1;"
        if (match293(tok) ||
            match294(tok) ||
            match295(tok)) {
            tok = tok1;
            while (tok->next()->str() != MatchCompiler::makeConstString(";"))
                tok->deleteNext();
        } else {
            // Enclose the rhs in parentheses..
            if (!match296(tok->tokAt(2))) {
                // Only enclose rhs in parentheses if there is some operator
                bool someOperator = false;
                for (Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                    if (tok2->link() && match143(tok2))
                        tok2 = tok2->link();

                    if (match296(tok2->next())) {
                        if (someOperator) {
                            tok->insertToken("(");
                            tok2->insertToken(")");
                            Token::createMutualLinks(tok->next(), tok2->next());
                        }
                        break;
                    }

                    someOperator |= (tok2->isOp() || tok2->str() == MatchCompiler::makeConstString("?"));
                }
            }

            // simplify the compound assignment..
            tok->str("=");
            tok->insertToken(op);

            std::stack<Token *> tokend;
            for (Token *tok2 = tok->previous(); tok2 && tok2 != tok1; tok2 = tok2->previous()) {
                // Don't duplicate ++ and --. Put preincrement in lhs. Put
                // postincrement in rhs.
                if (tok2->tokType() == Token::eIncDecOp) {
                    // pre increment/decrement => don't copy
                    if (tok2->next()->isName()) {
                        continue;
                    }

                    // post increment/decrement => move from lhs to rhs
                    tok->insertToken(tok2->str());
                    tok2->deleteThis();
                    continue;
                }

                // Copy token from lhs to rhs
                tok->insertToken(tok2->str());
                tok->next()->varId(tok2->varId());
                if (match297(tok->next()))
                    tokend.push(tok->next());
                else if (match271(tok->next())) {
                    Token::createMutualLinks(tok->next(), tokend.top());
                    tokend.pop();
                }
            }
        }
    }
}

bool Tokenizer::simplifyConditions()
{
    bool ret = false;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match298(tok)) {
            tok->deleteThis();
            if (match299(tok))
                tok->str("true");
            else
                tok->str("false");

            ret = true;
        }

        if (match300(tok)) {
            tok->deleteNext(2);
            ret = true;
        }

        else if (match301(tok)) {
            tok->deleteNext(2);
            ret = true;
        }

        else if (match302(tok)) {
            tok->deleteNext(2);
            ret = true;
        }

        else if (match303(tok)) {
            tok->deleteNext(2);
            ret = true;
        }

        else if (match304(tok) ||
                 match305(tok)) {
            Token::eraseTokens(tok->next(), tok->link());
            ret = true;
        }

        else if (match306(tok) ||
                 match307(tok)) {
            tok = tok->next();
            Token::eraseTokens(tok->next()->link(), tok);
            ret = true;
        }

        else if (match308(tok) ||
                 match309(tok)) {
            //goto '('
            Token *tok2 = tok;
            while (tok2->previous()) {
                if (tok2->previous()->str() == MatchCompiler::makeConstString(")"))
                    tok2 = tok2->previous()->link();
                else {
                    tok2 = tok2->previous();
                    if (tok2->str() == MatchCompiler::makeConstString("("))
                        break;
                }
            }
            if (!tok2)
                continue;
            //move tok to 'true|false' position
            tok = tok->next();
            //remove everything before 'true|false'
            Token::eraseTokens(tok2, tok);
            //remove everything after 'true|false'
            Token::eraseTokens(tok, tok2->link());
            ret = true;
        }

        // Change numeric constant in condition to "true" or "false"
        if (match310(tok)) {
            tok->tokAt(2)->str((tok->strAt(2) != MatchCompiler::makeConstString("0")) ? "true" : "false");
            ret = true;
        }
        if (match311(tok)) {
            tok->next()->str((tok->next()->str() != MatchCompiler::makeConstString("0")) ? "true" : "false");
            ret = true;
        }

        // Reduce "(%num% == %num%)" => "(true)"/"(false)"
        if (match312(tok) &&
            (match313(tok->next()) ||
             match314(tok->next())) &&
            match315(tok->tokAt(4))) {
            std::string cmp = tok->strAt(2);
            bool result = false;
            if (tok->next()->isNumber()) {
                // Compare numbers

                if (cmp == MatchCompiler::makeConstString("==") || cmp == MatchCompiler::makeConstString("!=")) {
                    const std::string& op1(tok->next()->str());
                    const std::string& op2(tok->strAt(3));

                    bool eq = false;
                    if (MathLib::isInt(op1) && MathLib::isInt(op2))
                        eq = (MathLib::toLongNumber(op1) == MathLib::toLongNumber(op2));
                    else {
                        eq = (op1 == op2);

                        // It is inconclusive whether two unequal float representations are numerically equal
                        if (!eq && MathLib::isFloat(op1))
                            cmp.clear();
                    }

                    if (cmp == MatchCompiler::makeConstString("=="))
                        result = eq;
                    else
                        result = !eq;
                } else {
                    const double op1 = MathLib::toDoubleNumber(tok->next()->str());
                    const double op2 = MathLib::toDoubleNumber(tok->strAt(3));
                    if (cmp == MatchCompiler::makeConstString(">="))
                        result = (op1 >= op2);
                    else if (cmp == MatchCompiler::makeConstString(">"))
                        result = (op1 > op2);
                    else if (cmp == MatchCompiler::makeConstString("<="))
                        result = (op1 <= op2);
                    else if (cmp == MatchCompiler::makeConstString("<"))
                        result = (op1 < op2);
                    else
                        cmp.clear();
                }
            } else {
                // Compare boolean
                const bool op1 = (tok->next()->str() == std::string("true"));
                const bool op2 = (tok->strAt(3) == std::string("true"));

                if (cmp == MatchCompiler::makeConstString("=="))
                    result = (op1 == op2);
                else if (cmp == MatchCompiler::makeConstString("!="))
                    result = (op1 != op2);
                else if (cmp == MatchCompiler::makeConstString(">="))
                    result = (op1 >= op2);
                else if (cmp == MatchCompiler::makeConstString(">"))
                    result = (op1 > op2);
                else if (cmp == MatchCompiler::makeConstString("<="))
                    result = (op1 <= op2);
                else if (cmp == MatchCompiler::makeConstString("<"))
                    result = (op1 < op2);
                else
                    cmp.clear();
            }

            if (! cmp.empty()) {
                tok = tok->next();
                tok->deleteNext(2);

                tok->str(result ? "true" : "false");
                ret = true;
            }
        }
    }

    return ret;
}

bool Tokenizer::simplifyConstTernaryOp()
{
    bool ret = false;
    const Token *templateParameterEnd = nullptr; // The end of the current template parameter list, if any
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("<") && mTemplateSimplifier->templateParameters(tok))
            templateParameterEnd = tok->findClosingBracket();
        if (tok == templateParameterEnd)
            templateParameterEnd = nullptr; // End of the current template parameter list
        if (tok->str() != MatchCompiler::makeConstString("?"))
            continue;

        if (!match316(tok->tokAt(-2)) &&
            !match317(tok->tokAt(-4)))
            continue;

        const int offset = (tok->previous()->str() == MatchCompiler::makeConstString(")")) ? 2 : 1;

        if (tok->strAt(-2*offset) == MatchCompiler::makeConstString("<")) {
            if (isC() || !mTemplateSimplifier->templateParameters(tok->tokAt(-2*offset)))
                continue; // '<' is less than; the condition is not a constant
        }

        // Find the token ":" then go to the next token
        Token *colon = skipTernaryOp(tok);
        if (!colon || colon->previous()->str() != MatchCompiler::makeConstString(":") || !colon->next())
            continue;

        //handle the GNU extension: "x ? : y" <-> "x ? x : y"
        if (colon->previous() == tok->next())
            tok->insertToken(tok->strAt(-offset));

        // go back before the condition, if possible
        tok = tok->tokAt(-2);
        if (offset == 2) {
            // go further back before the "("
            tok = tok->tokAt(-2);
            //simplify the parentheses
            tok->deleteNext();
            tok->next()->deleteNext();
        }

        if (match318(tok->next())) {
            // Use code after colon, remove code before it.
            Token::eraseTokens(tok, colon);

            tok = tok->next();
            ret = true;
        }

        // The condition is true. Delete the operator after the ":"..
        else {
            // delete the condition token and the "?"
            tok->deleteNext(2);

            unsigned int ternaryOplevel = 0;
            for (const Token *endTok = colon; endTok; endTok = endTok->next()) {
                if (match271(endTok)) {
                    endTok = endTok->link();
                }

                else if (endTok->str() == MatchCompiler::makeConstString("?"))
                    ++ternaryOplevel;
                else if (match319(endTok)) {
                    if (endTok->str() == MatchCompiler::makeConstString(":") && ternaryOplevel)
                        --ternaryOplevel;
                    else if (endTok->str() == MatchCompiler::makeConstString(">") && !templateParameterEnd)
                        ;
                    else {
                        Token::eraseTokens(colon->tokAt(-2), endTok);
                        ret = true;
                        break;
                    }
                }
            }
        }
    }

    return ret;
}

void Tokenizer::simplifyUndefinedSizeArray()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match17(tok)) {
            Token *tok2 = tok->next();
            while (tok2 && tok2->str() == MatchCompiler::makeConstString("*"))
                tok2 = tok2->next();
            if (!match320(tok2))
                continue;

            tok = tok2->previous();
            Token *end = tok2->next();
            unsigned int count = 0;
            do {
                end = end->tokAt(2);
                ++count;
            } while (match321(end));
            if (match224(end)) {
                do {
                    tok2->deleteNext(2);
                    tok->insertToken("*");
                } while (--count);
                tok = end;
            } else
                tok = tok->tokAt(3);
        }
    }
}

void Tokenizer::simplifyCasts()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // Don't remove cast in such cases:
        // *((char *)a + 1) = 0;
        // Remove cast when casting a function pointer:
        // (*(void (*)(char *))fp)(x);
        if (!tok->isName() &&
            match82(tok->next()) &&
            !match322(tok->linkAt(2))) {
            tok = tok->linkAt(2);
            continue;
        }
        // #3935 : don't remove cast in such cases:
        // ((char *)a)[1] = 0;
        if (tok->str() == MatchCompiler::makeConstString("(") && match323(tok->link())) {
            tok = tok->link();
            continue;
        }
        // #4164 : ((unsigned char)1) => (1)
        if (match324(tok->next()) && tok->next()->link()->previous()->isStandardType()) {
            const MathLib::bigint value = MathLib::toLongNumber(tok->next()->link()->next()->str());
            unsigned int bits = mSettings->char_bit * mTypeSize[tok->next()->link()->previous()->str()];
            if (!tok->tokAt(2)->isUnsigned() && bits > 0)
                bits--;
            if (bits < 31 && value >= 0 && value < (1LL << bits)) {
                tok->linkAt(1)->next()->isCast(true);
                Token::eraseTokens(tok, tok->next()->link()->next());
            }
            continue;
        }

        while ((match325(tok->next()) && (tok->str() != MatchCompiler::makeConstString(")") || tok->tokAt(2)->isStandardType())) ||
               match326(tok->next()) ||
               match327(tok->next()) ||
               (!tok->isName() && (match328(tok->next()) ||
                                   match329(tok->next())))) {
            if (tok->isName() && tok->str() != MatchCompiler::makeConstString("return"))
                break;

            if (isCPP() && tok->strAt(-1) == MatchCompiler::makeConstString("operator"))
                break;

            // Remove cast..
            Token::eraseTokens(tok, tok->next()->link()->next());

            // Set isCasted flag.
            Token *tok2 = tok->next();
            if (!match330(tok2))
                tok2->isCast(true);
            else {
                // TODO: handle more complex expressions
                tok2->next()->isCast(true);
            }

            // Remove '* &'
            if (match331(tok)) {
                tok->deleteNext();
                tok->deleteThis();
            }

            if (tok->str() == MatchCompiler::makeConstString(")") && tok->link()->previous()) {
                // If there was another cast before this, go back
                // there to check it also. e.g. "(int)(char)x"
                tok = tok->link()->previous();
            }
        }

        // Replace pointer casts of 0.. "(char *)0" => "0"
        while (match332(tok->next())) {
            tok->linkAt(1)->next()->isCast(true);
            Token::eraseTokens(tok, tok->next()->link()->next());
            if (tok->str() == MatchCompiler::makeConstString(")") && tok->link()->previous()) {
                // If there was another cast before this, go back
                // there to check it also. e.g. "(char*)(char*)0"
                tok = tok->link()->previous();
            }
        }

        if (match333(tok->next())) {
            Token *tok2 = tok->linkAt(2);
            if (!match75(tok2))
                break;

            tok2->tokAt(2)->isCast(true);
            Token::eraseTokens(tok, tok2->next());
        }
    }
}


void Tokenizer::simplifyFunctionParameters()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->link() && match143(tok)) {
            tok = tok->link();
        }

        // Find the function e.g. foo( x ) or foo( x, y )
        else if (match334(tok) &&
                 !(tok->strAt(-1) == MatchCompiler::makeConstString(":") || tok->strAt(-1) == MatchCompiler::makeConstString(","))) {
            // We have found old style function, now we need to change it

            // First step: Get list of argument names in parentheses
            std::map<std::string, Token *> argumentNames;
            bool bailOut = false;
            Token * tokparam = nullptr;

            //take count of the function name..
            const std::string& funcName(tok->str());

            //floating token used to check for parameters
            Token *tok1 = tok;

            while (nullptr != (tok1 = tok1->tokAt(2))) {
                if (!match173(tok1)) {
                    bailOut = true;
                    break;
                }

                //same parameters: take note of the parameter
                if (argumentNames.find(tok1->str()) != argumentNames.end())
                    tokparam = tok1;
                else if (tok1->str() != funcName)
                    argumentNames[tok1->str()] = tok1;
                else {
                    if (tok1->next()->str() == MatchCompiler::makeConstString(")")) {
                        if (tok1->previous()->str() == MatchCompiler::makeConstString(",")) {
                            tok1 = tok1->tokAt(-2);
                            tok1->deleteNext(2);
                        } else {
                            tok1 = tok1->previous();
                            tok1->deleteNext();
                            bailOut = true;
                            break;
                        }
                    } else {
                        tok1 = tok1->tokAt(-2);
                        tok1->next()->deleteNext(2);
                    }
                }

                if (tok1->next()->str() == MatchCompiler::makeConstString(")")) {
                    tok1 = tok1->tokAt(2);
                    //expect at least a type name after round brace..
                    if (!tok1 || !tok1->isName())
                        bailOut = true;
                    break;
                }
            }

            //goto '('
            tok = tok->next();

            if (bailOut) {
                tok = tok->link();
                continue;
            }

            tok1 = tok->link()->next();

            // there should be the sequence '; {' after the round parentheses
            for (const Token* tok2 = tok1; tok2; tok2 = tok2->next()) {
                if (match335(tok2))
                    break;
                else if (tok2->str() == MatchCompiler::makeConstString("{")) {
                    bailOut = true;
                    break;
                }
            }

            if (bailOut) {
                tok = tok->link();
                continue;
            }

            // Last step: check out if the declarations between ')' and '{' match the parameters list
            std::map<std::string, Token *> argumentNames2;

            while (tok1 && tok1->str() != MatchCompiler::makeConstString("{")) {
                if (match336(tok1)) {
                    bailOut = true;
                    break;
                }
                if (tok1->str() == MatchCompiler::makeConstString(";")) {
                    if (tokparam) {
                        syntaxError(tokparam);
                    }
                    Token *tok2 = tok1->previous();
                    while (tok2->str() == MatchCompiler::makeConstString("]"))
                        tok2 = tok2->link()->previous();

                    //it should be a name..
                    if (!tok2->isName()) {
                        bailOut = true;
                        break;
                    }

                    if (argumentNames2.find(tok2->str()) != argumentNames2.end()) {
                        //same parameter names...
                        syntaxError(tok1);
                    } else
                        argumentNames2[tok2->str()] = tok2;

                    if (argumentNames.find(tok2->str()) == argumentNames.end()) {
                        //non-matching parameter... bailout
                        bailOut = true;
                        break;
                    }
                }
                tok1 = tok1->next();
            }

            if (bailOut || !tok1) {
                tok = tok->link();
                continue;
            }

            //the two containers may not hold the same size...
            //in that case, the missing parameters are defined as 'int'
            if (argumentNames.size() != argumentNames2.size()) {
                //move back 'tok1' to the last ';'
                tok1 = tok1->previous();
                for (std::map<std::string, Token *>::iterator it = argumentNames.begin(); it != argumentNames.end(); ++it) {
                    if (argumentNames2.find(it->first) == argumentNames2.end()) {
                        //add the missing parameter argument declaration
                        tok1->insertToken(";");
                        tok1->insertToken(it->first);
                        //register the change inside argumentNames2
                        argumentNames2[it->first] = tok1->next();
                        tok1->insertToken("int");
                    }
                }
            }

            while (tok->str() != MatchCompiler::makeConstString(")")) {
                //initialize start and end tokens to be moved
                Token *declStart = argumentNames2[tok->next()->str()];
                Token *declEnd = declStart;
                while (declStart->previous()->str() != MatchCompiler::makeConstString(";") && declStart->previous()->str() != MatchCompiler::makeConstString(")"))
                    declStart = declStart->previous();
                while (declEnd->next()->str() != MatchCompiler::makeConstString(";") && declEnd->next()->str() != MatchCompiler::makeConstString("{"))
                    declEnd = declEnd->next();

                //remove ';' after declaration
                declEnd->deleteNext();

                //replace the parameter name in the parentheses with all the declaration
                Token::replace(tok->next(), declStart, declEnd);

                //since there are changes to tokens, put tok where tok1 is
                tok = declEnd->next();
            }
            //goto forward and continue
            tok = tok->next()->link();
        }
    }
}

void Tokenizer::simplifyPointerToStandardType()
{
    if (!isC())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match337(tok))
            continue;

        if (!match338(tok->previous()))
            continue;

        // Remove '[ 0 ]' suffix
        tok->next()->eraseTokens(tok->next(), tok->tokAt(5));
        // Remove '&' prefix
        tok = tok->previous();
        if (!tok)
            break;
        tok->deleteNext();
    }
}

void Tokenizer::simplifyFunctionPointers()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // #2873 - do not simplify function pointer usage here:
        // (void)(xy(*p)(0));
        if (match339(tok)) {
            tok = tok->next()->link();
            continue;
        }

        // check for function pointer cast
        if (match340(tok) ||
            match341(tok)) {
            Token *tok1 = tok;

            if (isCPP() && tok1->str() == MatchCompiler::makeConstString("static_cast"))
                tok1 = tok1->next();

            tok1 = tok1->next();

            if (match17(tok1->next()))
                tok1 = tok1->next();

            while (tok1->next()->str() == MatchCompiler::makeConstString("*"))
                tok1 = tok1->next();

            // check that the cast ends
            if (!match342(tok1->linkAt(4)))
                continue;

            // ok simplify this function pointer cast to an ordinary pointer cast
            tok1->deleteNext();
            tok1->next()->deleteNext();
            Token::eraseTokens(tok1->next(), tok1->linkAt(2)->next());
            continue;
        }

        // check for start of statement
        else if (tok->previous() && !match343(tok->previous()))
            continue;

        if (match344(tok))
            continue;

        while (match345(tok))
            tok = tok->next();

        Token *tok2 = (tok && tok->isName()) ? tok->next() : nullptr;
        while (match37(tok2))
            tok2 = tok2->next();
        if (!tok2 || tok2->str() != MatchCompiler::makeConstString("("))
            continue;
        while (match346(tok2))
            tok2 = tok2->tokAt(2);
        if (!match347(tok2))
            continue;
        tok2 = tok2->tokAt(2);
        if (tok2->str() == MatchCompiler::makeConstString("*"))
            tok2 = tok2->next();
        while (match345(tok2))
            tok2 = tok2->next();

        if (!match348(tok2) &&
            !match349(tok2) &&
            !(match6(tok2) && match64(tok2->linkAt(1))))
            continue;

        while (tok->str() != MatchCompiler::makeConstString("("))
            tok = tok->next();

        // check that the declaration ends
        if (!tok || !tok->link() || !tok->link()->next()) {
            syntaxError(nullptr);
        }
        Token *endTok = tok->link()->next()->link();
        if (match350(endTok))
            endTok = endTok->linkAt(2);
        if (!match351(endTok))
            continue;

        while (match50(endTok->next()))
            endTok->deleteNext();

        // ok simplify this function pointer to an ordinary pointer
        Token::eraseTokens(tok->link(), endTok->next());
        tok->link()->deleteThis();
        while (match352(tok))
            tok->deleteNext(2);
        tok->deleteThis();
    }
}


bool Tokenizer::simplifyFunctionReturn()
{
    std::map<std::string, const Token*> functions;

    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("{"))
            tok = tok->link();

        else if (match353(tok) && tok->strAt(-1) != MatchCompiler::makeConstString("::")) {
            const Token* const any = tok->tokAt(5);
            functions[tok->str()] = any;
            tok = any;
        }
    }

    if (functions.empty())
        return false;

    bool ret = false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match354(tok)) {
            tok = tok->next();
            auto it = functions.find(tok->str());
            if (it != functions.cend()) {
                tok->str(it->second->str());
                tok->deleteNext(2);
                ret = true;
            }
        }
    }

    return ret;
}

void Tokenizer::simplifyVarDecl(const bool only_k_r_fpar)
{
    simplifyVarDecl(list.front(), nullptr, only_k_r_fpar);
}

void Tokenizer::simplifyVarDecl(Token * tokBegin, const Token * const tokEnd, const bool only_k_r_fpar)
{
    const bool isCPP11  = mSettings->standards.cpp >= Standards::CPP11;

    // Split up variable declarations..
    // "int a=4;" => "int a; a=4;"
    bool finishedwithkr = true;
    for (Token *tok = tokBegin; tok != tokEnd; tok = tok->next()) {
        if (match107(tok)) {
            tok = tok->next()->link();
        }
        if (!tok) {
            syntaxError(tokBegin);
        }
        if (only_k_r_fpar && finishedwithkr) {
            if (match271(tok)) {
                tok = tok->link();
                if (tok->next() && match355(tok))
                    tok = tok->next();
                else
                    continue;
            } else
                continue;
        } else if (tok->str() == MatchCompiler::makeConstString("(")) {
            if (isCPP()) {
                for (Token * tok2 = tok; tok2 && tok2 != tok->link(); tok2 = tok2->next()) {
                    if (match356(tok2)) {
                        // lambda function at tok2->next()
                        // find start of lambda body
                        Token * lambdaBody = tok2;
                        while (lambdaBody && lambdaBody != tok2->link() && lambdaBody->str() != MatchCompiler::makeConstString("{"))
                            lambdaBody = lambdaBody->next();
                        if (lambdaBody && lambdaBody != tok2->link() && lambdaBody->link())
                            simplifyVarDecl(lambdaBody, lambdaBody->link()->next(), only_k_r_fpar);
                    }
                }
            }
            tok = tok->link();
        }

        if (!tok)
            syntaxError(nullptr); // #7043 invalid code
        if (tok->previous() && !match357(tok->previous()))
            continue;
        if (match358(tok))
            continue;

        Token *type0 = tok;
        if (!match359(type0))
            continue;
        if (match360(type0))
            continue;
        if (isCPP11 && type0->str() == MatchCompiler::makeConstString("using"))
            continue;
        if (isCPP() && type0->str() == MatchCompiler::makeConstString("namespace"))
            continue;

        bool isconst = false;
        bool isstatic = false;
        Token *tok2 = type0;
        unsigned int typelen = 1;

        if (match361(tok2)) {
            tok2 = tok2->next();
            typelen++;
        }

        //check if variable is declared 'const' or 'static' or both
        while (tok2) {
            if (!match362(tok2) && match363(tok2)) {
                tok2 = tok2->next();
                ++typelen;
            }

            if (tok2->str() == MatchCompiler::makeConstString("const"))
                isconst = true;

            else if (tok2->str() == MatchCompiler::makeConstString("static"))
                isstatic = true;

            else if (match257(tok2)) {
                tok2 = tok2->next();
                ++typelen;
            }

            else
                break;

            if (tok2->strAt(1) == MatchCompiler::makeConstString("*"))
                break;

            if (match364(tok2->next()))
                break;

            tok2 = tok2->next();
            ++typelen;
        }

        // strange looking variable declaration => don't split up.
        if (match365(tok2))
            continue;

        if (match165(tok2)) {
            tok2 = tok2->next();
            ++typelen;
        }

        // check for qualification..
        if (match49(tok2)) {
            ++typelen;
            tok2 = tok2->next();
        }

        //skip combinations of templates and namespaces
        while (!isC() && (match247(tok2) || match44(tok2))) {
            if (tok2->next()->str() == MatchCompiler::makeConstString("<") && !mTemplateSimplifier->templateParameters(tok2->next())) {
                tok2 = nullptr;
                break;
            }
            typelen += 2;
            tok2 = tok2->tokAt(2);
            if (tok2 && tok2->previous()->str() == MatchCompiler::makeConstString("::"))
                continue;
            unsigned int indentlevel = 0;
            unsigned int parens = 0;

            for (Token *tok3 = tok2; tok3; tok3 = tok3->next()) {
                ++typelen;

                if (!parens && tok3->str() == MatchCompiler::makeConstString("<")) {
                    ++indentlevel;
                } else if (!parens && tok3->str() == MatchCompiler::makeConstString(">")) {
                    if (indentlevel == 0) {
                        tok2 = tok3->next();
                        break;
                    }
                    --indentlevel;
                } else if (!parens && tok3->str() == MatchCompiler::makeConstString(">>")) {
                    if (indentlevel <= 1U) {
                        tok2 = tok3->next();
                        break;
                    }
                    indentlevel -= 2;
                } else if (tok3->str() == MatchCompiler::makeConstString("(")) {
                    ++parens;
                } else if (tok3->str() == MatchCompiler::makeConstString(")")) {
                    if (!parens) {
                        tok2 = nullptr;
                        break;
                    }
                    --parens;
                } else if (tok3->str() == MatchCompiler::makeConstString(";")) {
                    break;
                }
            }

            if (match49(tok2)) {
                ++typelen;
                tok2 = tok2->next();
            }
        }

        //pattern: "%type% *| ... *| const| %name% ,|="
        if (match17(tok2) ||
            (tok2 && tok2->previous() && tok2->previous()->str() == MatchCompiler::makeConstString(">"))) {
            Token *varName = tok2;
            if (!tok2->previous() || tok2->previous()->str() != MatchCompiler::makeConstString(">"))
                varName = varName->next();
            else
                --typelen;
            //skip all the pointer part
            bool isPointerOrRef = false;
            while (match366(varName) || match364(varName)) {
                isPointerOrRef = true;
                varName = varName->next();
            }

            while (match367(varName)) {
                if (varName->str() != MatchCompiler::makeConstString("const")) {
                    ++typelen;
                }
                varName = varName->next();
            }
            //non-VLA case
            if (match368(varName)) {
                if (varName->str() != MatchCompiler::makeConstString("operator")) {
                    tok2 = varName->next(); // The ',' or '=' token

                    if (tok2->str() == MatchCompiler::makeConstString("=") && (isstatic || (isconst && !isPointerOrRef))) {
                        //do not split const non-pointer variables..
                        while (tok2 && tok2->str() != MatchCompiler::makeConstString(",") && tok2->str() != MatchCompiler::makeConstString(";")) {
                            if (match369(tok2))
                                tok2 = tok2->link();
                            if (!isC() && tok2->str() == MatchCompiler::makeConstString("<") && mTemplateSimplifier->templateParameters(tok2) > 0) {
                                tok2 = tok2->findClosingBracket();
                            }
                            if (!tok2)
                                syntaxError(nullptr); // #6881 invalid code
                            tok2 = tok2->next();
                        }
                        if (tok2 && tok2->str() == MatchCompiler::makeConstString(";"))
                            tok2 = nullptr;
                    }
                } else
                    tok2 = nullptr;
            }

            //VLA case
            else if (match238(varName)) {
                tok2 = varName->next();

                while (match370(tok2->link()))
                    tok2 = tok2->link()->next();
                if (!match371(tok2))
                    tok2 = nullptr;
                if (tok2 && tok2->str() == MatchCompiler::makeConstString("=")) {
                    while (tok2 && tok2->str() != MatchCompiler::makeConstString(",") && tok2->str() != MatchCompiler::makeConstString(";")) {
                        if (match369(tok2))
                            tok2 = tok2->link();
                        tok2 = tok2->next();
                    }
                    if (tok2 && tok2->str() == MatchCompiler::makeConstString(";"))
                        tok2 = nullptr;
                }
            }

            // brace initialization
            else if (match372(varName)) {
                tok2 = varName->next();
                tok2 = tok2->link();
                if (tok2)
                    tok2 = tok2->next();
                if (tok2 && tok2->str() != MatchCompiler::makeConstString(","))
                    tok2 = nullptr;
            }

            // parenthesis, functions can't be declared like:
            // int f1(a,b), f2(c,d);
            // so if there is a comma assume this is a variable declaration
            else if (match6(varName) && match373(varName->linkAt(1))) {
                tok2 = varName->linkAt(1)->next();
            }

            else
                tok2 = nullptr;
        } else {
            tok2 = nullptr;
        }

        if (!tok2) {
            if (only_k_r_fpar)
                finishedwithkr = false;
            continue;
        }

        if (tok2->str() == MatchCompiler::makeConstString(",")) {
            tok2->str(";");
            //TODO: should we have to add also template '<>' links?
            list.insertTokens(tok2, type0, typelen);
        }

        else {
            Token *eq = tok2;

            while (tok2) {
                if (match374(tok2))
                    tok2 = tok2->link();

                else if (!isC() && tok2->str() == MatchCompiler::makeConstString("<") && tok2->previous()->isName() && !tok2->previous()->varId())
                    tok2 = tok2->findClosingBracket();

                else if (std::strchr(";,", tok2->str()[0])) {
                    // "type var ="   =>   "type var; var ="
                    const Token *varTok = type0->tokAt((int)typelen);
                    while (match375(varTok))
                        varTok = varTok->next();
                    if (!varTok)
                        syntaxError(tok2); // invalid code
                    list.insertTokens(eq, varTok, 2);
                    eq->str(";");

                    // "= x, "   =>   "= x; type "
                    if (tok2->str() == MatchCompiler::makeConstString(",")) {
                        tok2->str(";");
                        list.insertTokens(tok2, type0, typelen);
                    }
                    break;
                }
                if (tok2)
                    tok2 = tok2->next();
            }
        }
        finishedwithkr = (only_k_r_fpar && tok2 && tok2->strAt(1) == MatchCompiler::makeConstString("{"));
    }
}

void Tokenizer::simplifyPlatformTypes()
{
    const bool isCPP11  = mSettings->standards.cpp >= Standards::CPP11;

    enum { isLongLong, isLong, isInt } type;

    /** @todo This assumes a flat address space. Not true for segmented address space (FAR *). */

    if (mSettings->sizeof_size_t == mSettings->sizeof_long)
        type = isLong;
    else if (mSettings->sizeof_size_t == mSettings->sizeof_long_long)
        type = isLongLong;
    else if (mSettings->sizeof_size_t == mSettings->sizeof_int)
        type = isInt;
    else
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // pre-check to reduce unneeded match calls
        if (!match376(tok))
            continue;
        bool isUnsigned;
        if (match377(tok)) {
            if (isCPP11 && tok->strAt(-1) == MatchCompiler::makeConstString("using") && tok->strAt(1) == MatchCompiler::makeConstString("="))
                continue;
            isUnsigned = true;
        } else if (match378(tok)) {
            if (isCPP11 && tok->strAt(-1) == MatchCompiler::makeConstString("using") && tok->strAt(1) == MatchCompiler::makeConstString("="))
                continue;
            isUnsigned = false;
        } else
            continue;

        bool inStd = false;
        if (tok->str() == MatchCompiler::makeConstString("::")) {
            tok->deleteThis();
        } else if (tok->str() == MatchCompiler::makeConstString("std")) {
            if (tok->next()->str() != MatchCompiler::makeConstString("::"))
                continue;
            inStd = true;
            tok->deleteNext();
            tok->deleteThis();
        }

        if (inStd)
            tok->originalName("std::" + tok->str());
        else
            tok->originalName(tok->str());
        if (isUnsigned)
            tok->isUnsigned(true);

        switch (type) {
        case isLongLong:
            tok->isLong(true);
            tok->str("long");
            break;
        case isLong:
            tok->str("long");
            break;
        case isInt:
            tok->str("int");
            break;
        }
    }

    const std::string platform_type(mSettings->platformString());

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->tokType() != Token::eType && tok->tokType() != Token::eName)
            continue;

        const Library::PlatformType * const platformtype = mSettings->library.platform_type(tok->str(), platform_type);

        if (platformtype) {
            // check for namespace
            if (tok->strAt(-1) == MatchCompiler::makeConstString("::")) {
                const Token * tok1 = tok->tokAt(-2);
                // skip when non-global namespace defined
                if (tok1 && tok1->tokType() == Token::eName)
                    continue;
                tok = tok->previous();
                tok->deleteThis();
            }
            Token *typeToken;
            if (platformtype->_const_ptr) {
                tok->str("const");
                tok->insertToken("*");
                tok->insertToken(platformtype->mType);
                typeToken = tok;
            } else if (platformtype->_pointer) {
                tok->str(platformtype->mType);
                typeToken = tok;
                tok->insertToken("*");
            } else if (platformtype->_ptr_ptr) {
                tok->str(platformtype->mType);
                typeToken = tok;
                tok->insertToken("*");
                tok->insertToken("*");
            } else {
                tok->originalName(tok->str());
                tok->str(platformtype->mType);
                typeToken = tok;
            }
            if (platformtype->_signed)
                typeToken->isSigned(true);
            if (platformtype->_unsigned)
                typeToken->isUnsigned(true);
            if (platformtype->_long)
                typeToken->isLong(true);
        }
    }
}

void Tokenizer::simplifyStaticConst()
{
    // This function will simplify the token list so that the qualifiers "extern", "static"
    // and "const" appear in the same order as in the array below.
    const std::string qualifiers[] = {"extern", "static", "const"};

    // Move 'const' before all other qualifiers and types and then
    // move 'static' before all other qualifiers and types, ...
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        bool continue2 = false;
        for (size_t i = 0; i < sizeof(qualifiers)/sizeof(qualifiers[0]); i++) {

            // Keep searching for a qualifier
            if (!tok->next() || tok->next()->str() != qualifiers[i])
                continue;

            // Look backwards to find the beginning of the declaration
            Token* leftTok = tok;
            bool behindOther = false;
            for (; leftTok; leftTok = leftTok->previous()) {
                for (size_t j = 0; j <= i; j++) {
                    if (leftTok->str() == qualifiers[j]) {
                        behindOther = true;
                        break;
                    }
                }
                if (behindOther)
                    break;
                if (!match379(leftTok) ||
                    (isCPP() && match380(leftTok))) {
                    break;
                }
            }

            // The token preceding the declaration should indicate the start of a declaration
            if (leftTok == tok)
                continue;

            if (leftTok && !behindOther && !match381(leftTok)) {
                continue2 = true;
                break;
            }

            // Move the qualifier to the left-most position in the declaration
            tok->deleteNext();
            if (!leftTok) {
                list.front()->insertToken(qualifiers[i], emptyString, false);
                list.front()->swapWithNext();
                tok = list.front();
            } else if (leftTok->next()) {
                leftTok->next()->insertToken(qualifiers[i], emptyString, true);
                tok = leftTok->next();
            } else {
                leftTok->insertToken(qualifiers[i]);
                tok = leftTok;
            }
        }
        if (continue2)
            continue;
    }
}

void Tokenizer::simplifyIfAndWhileAssign()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match382(tok->next()))
            continue;

        const Token* tokAt3 = tok->tokAt(3);
        if (!match383(tokAt3) &&
            !match384(tokAt3) &&
            !match385(tokAt3) &&
            !match386(tokAt3))
            continue;

        // simplifying a "while(cond) { }" condition ?
        const bool iswhile(tok->next()->str() == MatchCompiler::makeConstString("while"));

        // simplifying a "do { } while(cond);" condition ?
        const bool isDoWhile = iswhile && match284(tok) && match387(tok->link()->previous());
        Token* openBraceTok = tok->link();

        // delete the "if|while"
        tok->deleteNext();

        // Remember if there is a "!" or not. And delete it if there are.
        const bool isNot(match388(tok->tokAt(2)));
        if (isNot)
            tok->next()->deleteNext((tok->strAt(2) == MatchCompiler::makeConstString("0")) ? 2 : 1);

        // Delete parentheses.. and remember how many there are with
        // their links.
        std::stack<Token *> braces;
        while (tok->next()->str() == MatchCompiler::makeConstString("(")) {
            braces.push(tok->next()->link());
            tok->deleteNext();
        }

        // Skip the "%name% = ..."
        Token *tok2;
        for (tok2 = tok->next(); tok2; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("("))
                tok2 = tok2->link();
            else if (tok2->str() == MatchCompiler::makeConstString(")"))
                break;
        }

        // Insert "; if|while ( .."
        tok2 = tok2->previous();
        if (tok->strAt(2) == MatchCompiler::makeConstString(".")) {
            tok2->insertToken(tok->strAt(3));
            tok2->next()->varId(tok->tokAt(3)->varId());
            tok2->insertToken(".");
        }
        tok2->insertToken(tok->next()->str());
        tok2->next()->varId(tok->next()->varId());

        while (! braces.empty()) {
            tok2->insertToken("(");
            Token::createMutualLinks(tok2->next(), braces.top());
            braces.pop();
        }

        if (isNot)
            tok2->next()->insertToken("!");
        tok2->insertToken(iswhile ? "while" : "if");
        if (isDoWhile) {
            tok2->insertToken("}");
            Token::createMutualLinks(openBraceTok, tok2->next());
        }

        tok2->insertToken(";");

        // delete the extra "}"
        if (isDoWhile)
            tok->deleteThis();

        // If it's a while loop, insert the assignment in the loop
        if (iswhile && !isDoWhile) {
            unsigned int indentlevel = 0;
            Token *tok3 = tok2;

            for (; tok3; tok3 = tok3->next()) {
                if (tok3->str() == MatchCompiler::makeConstString("{"))
                    ++indentlevel;
                else if (tok3->str() == MatchCompiler::makeConstString("}")) {
                    if (indentlevel <= 1)
                        break;
                    --indentlevel;
                }
            }

            if (tok3 && indentlevel == 1) {
                tok3 = tok3->previous();
                std::stack<Token *> braces2;

                for (tok2 = tok2->next(); tok2 && tok2 != tok; tok2 = tok2->previous()) {
                    tok3->insertToken(tok2->str());
                    Token *newTok = tok3->next();

                    newTok->varId(tok2->varId());
                    newTok->fileIndex(tok2->fileIndex());
                    newTok->linenr(tok2->linenr());

                    // link() new tokens manually
                    if (tok2->link()) {
                        if (match389(newTok)) {
                            braces2.push(newTok);
                        } else {
                            Token::createMutualLinks(newTok, braces2.top());
                            braces2.pop();
                        }
                    }
                }
            }
        }
    }
}

void Tokenizer::simplifyVariableMultipleAssign()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match390(tok)) {
            // skip intermediate assignments
            Token *tok2 = tok->previous();
            while (tok2 &&
                   tok2->str() == MatchCompiler::makeConstString("=") &&
                   match391(tok2->previous())) {
                tok2 = tok2->tokAt(-2);
            }

            if (!tok2 || tok2->str() != MatchCompiler::makeConstString(";")) {
                continue;
            }

            Token *stopAt = tok->tokAt(2);
            const Token *valueTok = stopAt->tokAt(2);
            const std::string& value(valueTok->str());
            tok2 = tok2->next();

            while (tok2 != stopAt) {
                tok2->next()->insertToken(";");
                tok2->next()->insertToken(value);
                tok2 = tok2->tokAt(4);
            }
        }
    }
}

// Binary operators simplification map
static const std::map<std::string, std::string> cAlternativeTokens = {
    std::make_pair("and", "&&")
    , std::make_pair("and_eq", "&=")
    , std::make_pair("bitand", "&")
    , std::make_pair("bitor", "|")
    , std::make_pair("not_eq", "!=")
    , std::make_pair("or", "||")
    , std::make_pair("or_eq", "|=")
    , std::make_pair("xor", "^")
    , std::make_pair("xor_eq", "^=")
};

// Simplify the C alternative tokens:
//  and      =>     &&
//  and_eq   =>     &=
//  bitand   =>     &
//  bitor    =>     |
//  compl    =>     ~
//  not      =>     !
//  not_eq   =>     !=
//  or       =>     ||
//  or_eq    =>     |=
//  xor      =>     ^
//  xor_eq   =>     ^=
bool Tokenizer::simplifyCAlternativeTokens()
{
    /* executable scope level */
    unsigned int executableScopeLevel = 0;

    bool ret = false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("{")) {
            if (executableScopeLevel > 0 || match19(tok->previous()))
                ++executableScopeLevel;
            continue;
        }

        if (tok->str() == MatchCompiler::makeConstString("}")) {
            if (executableScopeLevel > 0)
                --executableScopeLevel;
            continue;
        }

        if (!tok->isName())
            continue;

        const std::map<std::string, std::string>::const_iterator cOpIt = cAlternativeTokens.find(tok->str());
        if (cOpIt != cAlternativeTokens.end()) {
            if (!match392(tok->previous()))
                continue;
            if (match393(tok->next()))
                continue;
            tok->str(cOpIt->second);
            ret = true;
        } else if (match394(tok)) {
            // Don't simplify 'not p;' (in case 'not' is a type)
            if (!match395(tok->next()) ||
                match88(tok->previous()) ||
                (executableScopeLevel == 0U && tok->strAt(-1) == MatchCompiler::makeConstString("(")))
                continue;
            tok->str((tok->str() == MatchCompiler::makeConstString("not")) ? "!" : "~");
            ret = true;
        }
    }
    return ret;
}

// int i(0); => int i; i = 0;
// int i(0), j; => int i; i = 0; int j;
void Tokenizer::simplifyInitVar()
{
    if (isC())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!tok->isName() || (tok->previous() && !match88(tok->previous())))
            continue;

        if (tok->str() == MatchCompiler::makeConstString("return"))
            continue;

        if (match396(tok)) {
            tok = initVar(tok);
        } else if (match397(tok)) {
            const Token* tok2 = tok->tokAt(2);
            if (!tok2->link())
                tok2 = tok2->next();
            if (!tok2->link() || (tok2->link()->strAt(1) == MatchCompiler::makeConstString(";") && !match339(tok2->linkAt(2))))
                tok = initVar(tok);
        } else if (match398(tok)) {
            Token *tok1 = tok->tokAt(5);
            while (tok1->str() != MatchCompiler::makeConstString(","))
                tok1 = tok1->next();
            tok1->str(";");

            const unsigned int numTokens = (match399(tok)) ? 2U : 1U;
            list.insertTokens(tok1, tok, numTokens);
            tok = initVar(tok);
        }
    }
}

Token * Tokenizer::initVar(Token * tok)
{
    // call constructor of class => no simplification
    if (match399(tok)) {
        if (tok->strAt(2) != MatchCompiler::makeConstString("*"))
            return tok;

        tok = tok->next();
    } else if (!tok->isStandardType() && tok->next()->str() != MatchCompiler::makeConstString("*"))
        return tok;

    // goto variable name..
    tok = tok->next();
    if (tok->str() == MatchCompiler::makeConstString("*"))
        tok = tok->next();

    // sizeof is not a variable name..
    if (tok->str() == MatchCompiler::makeConstString("sizeof"))
        return tok;

    // check initializer..
    if (tok->tokAt(2)->isStandardType() || tok->strAt(2) == MatchCompiler::makeConstString("void"))
        return tok;
    else if (!tok->tokAt(2)->isNumber() && !match83(tok->tokAt(2)) && tok->strAt(2) != MatchCompiler::makeConstString("&") && tok->tokAt(2)->varId() == 0)
        return tok;

    // insert '; var ='
    tok->insertToken(";");
    tok->next()->insertToken(tok->str());
    tok->tokAt(2)->varId(tok->varId());
    tok = tok->tokAt(2);
    tok->insertToken("=");

    // goto '('..
    tok = tok->tokAt(2);

    // delete ')'
    tok->link()->deleteThis();

    // delete this
    tok->deleteThis();

    return tok;
}


bool Tokenizer::simplifyKnownVariables()
{
    // return value for function. Set to true if any simplifications are made
    bool ret = false;

    // constants..
    {
        std::unordered_map<unsigned int, std::string> constantValues;
        std::map<unsigned int, Token*> constantVars;
        std::unordered_map<unsigned int, std::list<Token*>> constantValueUsages;
        bool goback = false;
        for (Token *tok = list.front(); tok; tok = tok->next()) {
            if (goback) {
                tok = tok->previous();
                goback = false;
            }
            // Reference to variable
            if (match400(tok)) {
                Token *start = tok->previous();
                while (match401(start))
                    start = start->previous();
                if (!match88(start))
                    continue;
                const Token *reftok = tok->tokAt(2);
                const Token *vartok = reftok->tokAt(2);
                int level = 0;
                for (Token *tok2 = tok->tokAt(6); tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString("{")) {
                        ++level;
                    } else if (tok2->str() == MatchCompiler::makeConstString("}")) {
                        if (level <= 0)
                            break;
                        --level;
                    } else if (tok2->varId() == reftok->varId()) {
                        tok2->str(vartok->str());
                        tok2->varId(vartok->varId());
                    }
                }
                Token::eraseTokens(start, tok->tokAt(6));
                tok = start;
            }

            if (tok->isName() && (match402(tok) ||
                                  match403(tok))) {
                bool isconst = false;
                for (const Token *tok2 = tok; (tok2->str() != MatchCompiler::makeConstString("=")) && (tok2->str() != MatchCompiler::makeConstString("(")); tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString("const")) {
                        isconst = true;
                        break;
                    }
                }
                if (!isconst)
                    continue;

                Token *tok1 = tok;

                // start of statement
                if (tok != list.front() && !match404(tok->previous()))
                    continue;
                // skip "const" and "static"
                while (match362(tok))
                    tok = tok->next();
                // pod type
                if (!tok->isStandardType())
                    continue;

                Token * const vartok = (tok->next() && tok->next()->str() == MatchCompiler::makeConstString("const")) ? tok->tokAt(2) : tok->next();
                const Token * const valuetok = vartok->tokAt(2);
                if (match405(valuetok)) {
                    // record a constant value for this variable
                    constantValues[vartok->varId()] = valuetok->str();
                    constantVars[vartok->varId()] = tok1;
                }
            } else if (tok->varId()) {
                // find the entry for the known variable, if any.  Exclude the location where the variable is assigned with next == MatchCompiler::makeConstString("=")
                if (constantValues.find(tok->varId()) != constantValues.end() && tok->next()->str() != MatchCompiler::makeConstString("=")) {
                    constantValueUsages[tok->varId()].push_back(tok);
                }
            }
        }

        for (auto constantVar = constantVars.rbegin(); constantVar != constantVars.rend(); constantVar++) {
            bool referenceFound = false;
            std::list<Token*> usageList = constantValueUsages[constantVar->first];
            for (Token* usage : usageList) {
                // check if any usages of each known variable are a reference
                if (match406(usage->tokAt(-2), constantVar->first)) {
                    referenceFound = true;
                    break;
                }
            }

            if (!referenceFound) {
                // replace all usages of non-referenced known variables with their value
                for (Token* usage : usageList) {
                    usage->str(constantValues[constantVar->first]);
                }

                Token* startTok = constantVar->second;
                // remove variable assignment statement
                while (startTok->next()->str() != MatchCompiler::makeConstString(";"))
                    startTok->deleteNext();
                startTok->deleteNext();

                // #8579 if we can we want another token to delete startTok. if we can't it doesn't matter
                if (startTok->previous()) {
                    startTok->previous()->deleteNext();
                } else if (startTok->next()) {
                    startTok->next()->deletePrevious();
                } else {
                    startTok->deleteThis();
                }
                startTok = nullptr;

                constantVar->second = nullptr;
                ret = true;
            }
        }
    }

    // variable id for local, float/double, array variables
    std::set<unsigned int> localvars;
    std::set<unsigned int> floatvars;
    std::set<unsigned int> arrays;

    // auto variables..
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // Search for a block of code
        Token * const start = const_cast<Token *>(startOfExecutableScope(tok));
        if (!start)
            continue;

        for (const Token *tok2 = start->previous(); tok2 && !match88(tok2); tok2 = tok2->previous()) {
            if (tok2->varId() != 0)
                localvars.insert(tok2->varId());
        }

        tok = start;
        // parse the block of code..
        int indentlevel = 0;
        Token *tok2 = tok;
        for (; tok2; tok2 = tok2->next()) {
            if (match178(tok2)) {
                bool isfloat = false;
                bool ispointer = false;
                const Token *vartok = tok2->next();
                while (match407(vartok)) {
                    if (match408(vartok))
                        isfloat = true;
                    if (vartok->str() == MatchCompiler::makeConstString("*"))
                        ispointer = true;
                    vartok = vartok->next();
                }
                if (match409(vartok))
                    localvars.insert(vartok->varId());
                if (isfloat && !ispointer && match410(vartok))
                    floatvars.insert(vartok->varId());
                if (match411(vartok))
                    arrays.insert(vartok->varId());
            }

            if (tok2->str() == MatchCompiler::makeConstString("{"))
                ++indentlevel;

            else if (tok2->str() == MatchCompiler::makeConstString("}")) {
                --indentlevel;
                if (indentlevel <= 0)
                    break;
            }

            else if (match412(tok2))
                tok2 = tok2->next()->link();

            else if (tok2->previous()->str() != MatchCompiler::makeConstString("*") && !match413(tok2->tokAt(-2)) &&
                     (match414(tok2) ||
                      match415(tok2) ||
                      match416(tok2) ||
                      (match417(tok2) && arrays.find(tok2->tokAt(3)->varId()) != arrays.end()))) {
                const unsigned int varid = tok2->varId();
                if (varid == 0)
                    continue;

                if (match88(tok2->previous()) && localvars.find(varid) == localvars.end())
                    continue;

                // initialization of static variable => the value is not *known*
                {
                    bool isstatic = false;
                    const Token *decl = tok2->previous();
                    while (decl && (decl->isName() || decl->str() == MatchCompiler::makeConstString("*"))) {
                        if (decl->str() == MatchCompiler::makeConstString("static")) {
                            isstatic = true;
                            break;
                        }
                        decl = decl->previous();
                    }
                    if (isstatic)
                        continue;
                }

                // skip loop variable
                if (match346(tok2->tokAt(-2))) {
                    const Token *tok3 = tok2->previous();
                    do {
                        tok3 = tok3->tokAt(-2);
                    } while (match49(tok3->previous()));
                    if (match418(tok3->tokAt(-2)))
                        continue;
                }

                // struct name..
                if (match419(tok2, tok2->varId()))
                    continue;

                const std::string structname = match420(tok2->tokAt(-3)) ?
                                               std::string(tok2->strAt(-2) + " .") :
                                               std::string();

                const Token * const valueToken = tok2->tokAt(2);

                std::string value;
                unsigned int valueVarId = 0;

                Token *tok3 = nullptr;
                bool valueIsPointer = false;

                // there could be a hang here if tok2 is moved back by the function calls below for some reason
                if (mSettings->terminated())
                    return false;

                if (!simplifyKnownVariablesGetData(varid, &tok2, &tok3, value, valueVarId, valueIsPointer, floatvars.find(tok2->varId()) != floatvars.end()))
                    continue;

                if (valueVarId > 0 && arrays.find(valueVarId) != arrays.end())
                    continue;

                ret |= simplifyKnownVariablesSimplify(&tok2, tok3, varid, structname, value, valueVarId, valueIsPointer, valueToken, indentlevel);
            }

            else if (match421(tok2)) {
                const unsigned int varid(tok2->tokAt(2)->varId());
                if (varid == 0)
                    continue;

                const Token * const valueToken = tok2->tokAt(4);
                std::string value(valueToken->str());
                if (tok2->str() == MatchCompiler::makeConstString("sprintf")) {
                    std::string::size_type n = 0;
                    while ((n = value.find("%%", n)) != std::string::npos) {
                        // Replace "%%" with "%" - erase the first '%' and continue past the second '%'
                        value.erase(n, 1);
                        ++n;
                    }
                }
                const unsigned int valueVarId(0);
                const bool valueIsPointer(false);
                Token *tok3 = tok2->tokAt(6);
                ret |= simplifyKnownVariablesSimplify(&tok2, tok3, varid, emptyString, value, valueVarId, valueIsPointer, valueToken, indentlevel);

                // there could be a hang here if tok2 was moved back by the function call above for some reason
                if (mSettings->terminated())
                    return false;
            }
        }

        if (tok2)
            tok = tok2->previous();
    }

    return ret;
}

bool Tokenizer::simplifyKnownVariablesGetData(unsigned int varid, Token **_tok2, Token **_tok3, std::string &value, unsigned int &valueVarId, bool &valueIsPointer, bool floatvar)
{
    Token *tok2 = *_tok2;
    Token *tok3 = nullptr;

    if (match412(tok2->tokAt(-2))) {
        // only specific for loops is handled
        if (!match422(tok2, varid))
            return false;

        // is there a "break" in the for loop?
        bool hasbreak = false;
        const Token* end4 = tok2->linkAt(-1)->linkAt(1);
        for (const Token *tok4 = tok2->previous()->link(); tok4 != end4; tok4 = tok4->next()) {
            if (tok4->str() == MatchCompiler::makeConstString("break")) {
                hasbreak = true;
                break;
            }
        }
        if (hasbreak)
            return false;

        // no break => the value of the counter value is known after the for loop..
        const Token* compareTok = tok2->tokAt(5);
        if (compareTok->str() == MatchCompiler::makeConstString("<")) {
            value = compareTok->next()->str();
            valueVarId = compareTok->next()->varId();
        } else
            value = MathLib::toString(MathLib::toLongNumber(compareTok->next()->str()) + 1);

        // Skip for-body..
        tok3 = tok2->previous()->link()->next()->link()->next();
    } else {
        value = tok2->strAt(2);
        valueVarId = tok2->tokAt(2)->varId();
        if (tok2->strAt(1) == MatchCompiler::makeConstString("[")) {
            value = tok2->next()->link()->strAt(2);
            valueVarId = 0;
        } else if (value == MatchCompiler::makeConstString("&")) {
            value = tok2->strAt(3);
            valueVarId = tok2->tokAt(3)->varId();

            // *ptr = &var; *ptr = 5;
            // equals
            // var = 5; not *var = 5;
            if (tok2->strAt(4) == MatchCompiler::makeConstString(";"))
                valueIsPointer = true;
        }

        // Add a '.0' to a decimal value and therefore convert it to an floating point number.
        else if (MathLib::isDec(tok2->strAt(2)) && floatvar) {
            value += ".0";
        }

        // float variable: convert true/false to 1.0 / 0.0
        else if (tok2->tokAt(2)->isBoolean() && floatvar) {
            value = (value == MatchCompiler::makeConstString("true")) ? "1.0" : "0.0";
        }

        if (match423(tok2->next()))
            tok2 = tok2->tokAt(3);

        tok3 = tok2->next();
    }
    *_tok2 = tok2;
    *_tok3 = tok3;
    return true;
}

bool Tokenizer::simplifyKnownVariablesSimplify(Token **tok2, Token *tok3, unsigned int varid, const std::string &structname, std::string &value, unsigned int valueVarId, bool valueIsPointer, const Token * const valueToken, int indentlevel) const
{
    const bool pointeralias(valueToken->isName() || match424(valueToken));
    const bool varIsGlobal = (indentlevel == 0);
    const bool printDebug = mSettings->debugwarnings;

    if (mErrorLogger && !list.getFiles().empty())
        mErrorLogger->reportProgress(list.getFiles()[0], "Tokenize (simplifyKnownVariables)", tok3->progressValue());

    if (isMaxTime())
        return false;

    bool ret = false;

    Token* bailOutFromLoop = nullptr;
    int indentlevel3 = indentlevel;
    bool ret3 = false;
    for (; tok3; tok3 = tok3->next()) {
        if (tok3->str() == MatchCompiler::makeConstString("{")) {
            ++indentlevel3;
        } else if (tok3->str() == MatchCompiler::makeConstString("}")) {
            --indentlevel3;
            if (indentlevel3 < indentlevel) {
                if (match425((*tok2)->tokAt(-7)) &&
                    (*tok2)->strAt(-5) == (*tok2)->strAt(-3)) {
                    (*tok2) = (*tok2)->tokAt(-4);
                    Token::eraseTokens((*tok2), (*tok2)->tokAt(6));
                }
                break;
            }
        }

        // Stop if there is a pointer alias and a shadow variable is
        // declared in an inner scope (#3058)
        if (valueIsPointer && tok3->varId() > 0 &&
            tok3->previous() && (tok3->previous()->isName() || tok3->previous()->str() == MatchCompiler::makeConstString("*")) &&
            valueToken->str() == MatchCompiler::makeConstString("&") &&
            valueToken->next() &&
            valueToken->next()->isName() &&
            tok3->str() == valueToken->next()->str() &&
            tok3->varId() > valueToken->next()->varId()) {
            // more checking if this is a variable declaration
            bool decl = true;
            for (const Token *tok4 = tok3->previous(); tok4; tok4 = tok4->previous()) {
                if (match88(tok4))
                    break;

                else if (tok4->isName()) {
                    if (tok4->varId() > 0) {
                        decl = false;
                        break;
                    }
                }

                else if (!match166(tok4)) {
                    decl = false;
                    break;
                }
            }
            if (decl)
                break;
        }

        // Stop if label is found
        if (match426(tok3))
            break;

        // Stop if break/continue is found ..
        if (match427(tok3))
            break;
        if ((indentlevel3 > 1 || !match428(findmatch429(tok3) )) && tok3->str() == MatchCompiler::makeConstString("return"))
            ret3 = true;
        if (ret3 && tok3->str() == MatchCompiler::makeConstString(";"))
            break;

        if (pointeralias && Token::Match(tok3, ("!!= " + value).c_str()))
            break;

        // Stop if a loop is found
        if (pointeralias && match430(tok3))
            break;

        // Stop if unknown function call is seen and the variable is global: it might be
        // changed by the function call
        if (varIsGlobal && tok3->str() == MatchCompiler::makeConstString(")") && tok3->link() &&
            match261(tok3->link()->tokAt(-2)) &&
            !match431(tok3->link()->previous()))
            break;

        // Stop if something like 'while (--var)' is found
        if (match432(tok3)) {
            const Token *endpar = tok3->next()->link();
            if (match19(endpar))
                endpar = endpar->next()->link();
            bool bailout = false;
            for (const Token *tok4 = tok3; tok4 && tok4 != endpar; tok4 = tok4->next()) {
                if (match433(tok4, varid) ||
                    match434(tok4, varid)) {
                    bailout = true;
                    break;
                }
            }
            if (bailout)
                break;
        }

        if (bailOutFromLoop) {
            // This could be a loop, skip it, but only if it doesn't contain
            // the variable we are checking for. If it contains the variable
            // we will bail out.
            if (tok3->varId() == varid) {
                // Continue
                //tok2 = bailOutFromLoop;
                break;
            } else if (tok3 == bailOutFromLoop) {
                // We have skipped the loop
                bailOutFromLoop = nullptr;
                continue;
            }

            continue;
        } else if (tok3->str() == MatchCompiler::makeConstString("{") && tok3->previous()->str() == MatchCompiler::makeConstString(")")) {
            // There is a possible loop after the assignment. Try to skip it.
            if (tok3->previous()->link() &&
                tok3->previous()->link()->strAt(-1) != MatchCompiler::makeConstString("if"))
                bailOutFromLoop = tok3->link();
            continue;
        }

        // Variable used in realloc (see Ticket #1649)
        if (match435(tok3) &&
            tok3->varId() == varid &&
            tok3->tokAt(4)->varId() == varid) {
            tok3->tokAt(4)->str(value);
            ret = true;
        }

        // condition "(|&&|%OROR% %varid% )|&&|%OROR%|;
        if (!match436(tok3->previous()) &&
            match437(tok3->previous(), varid)) {
            tok3->str(value);
            tok3->varId(valueVarId);
            ret = true;
        }

        // parameter in function call..
        if (tok3->varId() == varid && match438(tok3->previous())) {
            // If the parameter is passed by value then simplify it
            if (isFunctionParameterPassedByValue(tok3)) {
                tok3->str(value);
                tok3->varId(valueVarId);
                ret = true;
            }
        }

        // Variable is used somehow in a non-defined pattern => bail out
        if (tok3->varId() == varid) {
            // This is a really generic bailout so let's try to avoid this.
            // There might be lots of false negatives.
            if (printDebug) {
                // FIXME: Fix all the debug warnings for values and then
                // remove this bailout
                if (pointeralias)
                    break;

                // suppress debug-warning when calling member function
                if (match439(tok3->next()))
                    break;

                // suppress debug-warning when assignment
                if (tok3->strAt(1) == MatchCompiler::makeConstString("="))
                    break;

                // taking address of variable..
                if (match440(tok3->tokAt(-2)))
                    break;

                // parameter in function call..
                if (match441(tok3->tokAt(-2)) ||
                    match442(tok3->previous()))
                    break;

                // conditional increment
                if (match443(tok3->tokAt(-3)) ||
                    match444(tok3->tokAt(-2)))
                    break;

                reportError(tok3, Severity::debug, "debug",
                            "simplifyKnownVariables: bailing out (variable="+tok3->str()+", value="+value+")");
            }

            break;
        }

        // Using the variable in condition..
        if (Token::Match(tok3->previous(), ("if ( " + structname + " %varid% %cop%|)").c_str(), varid) ||
            Token::Match(tok3, ("( " + structname + " %varid% %comp%").c_str(), varid) ||
            Token::Match(tok3, ("%comp%|!|= " + structname + " %varid% %cop%|)|;").c_str(), varid) ||
            match445(tok3->previous(), varid)) {
            if (value[0] == '\"' && tok3->previous()->str() != MatchCompiler::makeConstString("strlen")) {
                // bail out if value is a string unless if it's just given
                // as parameter to strlen
                break;
            }
            if (!structname.empty()) {
                tok3->deleteNext(2);
            }
            if (match446(valueToken)) {
                tok3->insertToken("&");
                tok3 = tok3->next();
            }
            tok3 = tok3->next();
            tok3->str(value);
            tok3->varId(valueVarId);
            ret = true;
        }

        // pointer alias used in condition..
        if (match446(valueToken) && Token::Match(tok3, ("( * " + structname + " %varid% %cop%").c_str(), varid)) {
            tok3->deleteNext();
            if (!structname.empty())
                tok3->deleteNext(2);
            tok3 = tok3->next();
            tok3->str(value);
            tok3->varId(valueVarId);
            ret = true;
        }

        // Delete pointer alias
        if (isCPP() && pointeralias && (tok3->str() == MatchCompiler::makeConstString("delete")) && tok3->next() &&
            (match447(tok3->next(), varid) ||
             match448(tok3->next(), varid))) {
            tok3 = (tok3->next()->str() == MatchCompiler::makeConstString("[")) ? tok3->tokAt(3) : tok3->next();
            tok3->str(value);
            tok3->varId(valueVarId);
            ret = true;
        }

        // Variable is used in function call..
        if (Token::Match(tok3, ("%name% ( " + structname + " %varid% ,").c_str(), varid)) {
            static const char * const functionName[] = {
                // always simplify
                "strcmp", "strdup",
                // don't simplify buffer value
                "memcmp","memcpy","memmove","memset","strcpy","strncmp","strncpy"
            };
            for (unsigned int i = 0; i < (sizeof(functionName) / sizeof(*functionName)); ++i) {
                if (valueVarId == 0U && i >= 2)
                    break;
                if (tok3->str() == functionName[i]) {
                    Token *par1 = tok3->tokAt(2);
                    if (!structname.empty()) {
                        par1->deleteNext();
                        par1->deleteThis();
                    }
                    par1->str(value);
                    par1->varId(valueVarId);
                    break;
                }
            }
        }

        // Variable is used as 2nd parameter in function call..
        if (Token::Match(tok3, ("%name% ( %any% , " + structname + " %varid% ,|)").c_str(), varid)) {
            static const char * const functionName[] = {
                // always simplify
                "strcmp","strcpy","strncmp","strncpy",
                // don't simplify buffer value
                "memcmp","memcpy","memmove"
            };
            for (unsigned int i = 0; i < (sizeof(functionName) / sizeof(*functionName)); ++i) {
                if (valueVarId == 0U && i >= 4)
                    break;
                if (tok3->str() == functionName[i]) {
                    Token *par = tok3->tokAt(4);
                    if (!structname.empty()) {
                        par->deleteNext();
                        par->deleteThis();
                    }
                    par->str(value);
                    par->varId(valueVarId);
                    break;
                }
            }
        }

        // array usage
        if (value[0] != '\"' && Token::Match(tok3, ("[(,] " + structname + " %varid% [|%cop%").c_str(), varid)) {
            if (!structname.empty()) {
                tok3->deleteNext(2);
            }
            tok3 = tok3->next();
            tok3->str(value);
            tok3->varId(valueVarId);
            ret = true;
        }

        // The >> operator is sometimes used to assign a variable in C++
        if (isCPP() && Token::Match(tok3, (">> " + structname + " %varid%").c_str(), varid)) {
            // bailout for such code:   ; std :: cin >> i ;
            const Token *prev = tok3->previous();
            while (prev && prev->str() != MatchCompiler::makeConstString("return") && match449(prev))
                prev = prev->previous();
            if (match450(prev))
                break;
        }

        // Variable is used in calculation..
        if (((tok3->previous()->varId() > 0) && Token::Match(tok3, ("& " + structname + " %varid%").c_str(), varid)) ||
            (Token::Match(tok3, ("[=+-*/%^|[] " + structname + " %varid% [=?+-*/%^|;])]").c_str(), varid) && !Token::Match(tok3, ("= " + structname + " %name% =").c_str())) ||
            Token::Match(tok3, ("[(=+-*/%^|[] " + structname + " %varid% <<|>>").c_str(), varid) ||
            Token::Match(tok3, ("<<|>> " + structname + " %varid% %cop%|;|]|)").c_str(), varid) ||
            Token::Match(tok3->previous(), ("[=+-*/%^|[] ( " + structname + " %varid% !!=").c_str(), varid)) {
            if (value[0] == '\"')
                break;
            if (!structname.empty()) {
                tok3->deleteNext(2);
                ret = true;
            }
            tok3 = tok3->next();
            if (tok3->str() != value)
                ret = true;
            tok3->str(value);
            tok3->varId(valueVarId);
            if (tok3->previous()->str() == MatchCompiler::makeConstString("*") && (valueIsPointer || match446(valueToken))) {
                tok3 = tok3->previous();
                tok3->deleteThis();
                ret = true;
            } else if (match446(valueToken))
                tok3->insertToken("&", emptyString, true);
        }

        if (match107(tok3)) {
            const Token* const end4 = tok3->linkAt(1);
            for (const Token *tok4 = tok3; tok4 != end4; tok4 = tok4->next()) {
                if (match451(tok4, varid)) {
                    tok4->next()->str(value);
                    tok4->next()->varId(valueVarId);
                    ret = true;
                }
            }
        }

        // Using the variable in for-condition..
        if (match412(tok3)) {
            for (Token *tok4 = tok3->tokAt(2); tok4; tok4 = tok4->next()) {
                if (match336(tok4))
                    break;

                // Replace variable used in condition..
                if (match452(tok4)) {
                    const Token *inctok = tok4->tokAt(5);
                    if (inctok->str() == MatchCompiler::makeConstString("++"))
                        inctok = inctok->next();
                    if (inctok->varId() == varid)
                        break;

                    if (tok4->next()->varId() == varid) {
                        tok4->next()->str(value);
                        tok4->next()->varId(valueVarId);
                        ret = true;
                    }
                    if (tok4->tokAt(3)->varId() == varid) {
                        tok4->tokAt(3)->str(value);
                        tok4->tokAt(3)->varId(valueVarId);
                        ret = true;
                    }
                }
            }
        }

        if (indentlevel == indentlevel3 && match453(tok3->next(), varid) && MathLib::isInt(value)) {
            const std::string op(tok3->strAt(2));
            if (match454(tok3)) {
                tok3->deleteNext(3);
            } else {
                tok3 = tok3->next();
                tok3->str(value);
                tok3->varId(valueVarId);
                tok3->deleteNext();
            }
            value = MathLib::incdec(value, op);
            if (!match412((*tok2)->tokAt(-2))) {
                (*tok2)->tokAt(2)->str(value);
                (*tok2)->tokAt(2)->varId(valueVarId);
            }
            ret = true;
        }

        if (indentlevel == indentlevel3 && match433(tok3->next(), varid) && MathLib::isInt(value) &&
            !match455(tok3->tokAt(3))) {
            value = MathLib::incdec(value, tok3->next()->str());
            (*tok2)->tokAt(2)->str(value);
            (*tok2)->tokAt(2)->varId(valueVarId);
            if (match456(tok3)) {
                tok3->deleteNext(3);
            } else {
                tok3->deleteNext();
                tok3->next()->str(value);
                tok3->next()->varId(valueVarId);
            }
            tok3 = tok3->next();
            ret = true;
        }

        // return variable..
        if (match457(tok3, varid) &&
            valueToken->str() != MatchCompiler::makeConstString("&") &&
            (tok3->tokAt(2)->isExtendedOp() || tok3->strAt(2) == MatchCompiler::makeConstString(";")) &&
            value[0] != '\"') {
            tok3->next()->str(value);
            tok3->next()->varId(valueVarId);
        }

        else if (pointeralias && match458(tok3, varid) && value[0] != '\"') {
            tok3->deleteNext();
            tok3->next()->str(value);
            tok3->next()->varId(valueVarId);
        }
    }
    return ret;
}


void Tokenizer::elseif()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match459(tok))
            continue;

        for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
            if (match460(tok2))
                tok2 = tok2->link();

            if (match461(tok2)) {
                if (tok2->next() && tok2->next()->str() != MatchCompiler::makeConstString("else")) {
                    tok->insertToken("{");
                    tok2->insertToken("}");
                    Token::createMutualLinks(tok->next(), tok2->next());
                    break;
                }
            }
        }
    }
}


bool Tokenizer::simplifyRedundantParentheses()
{
    bool ret = false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("("))
            continue;

        if (match462(tok))
            continue;

        if (match463(tok->link())) {
            tok = tok->link();
            continue;
        }

        // !!operator = ( x ) ;
        if (tok->strAt(-2) != MatchCompiler::makeConstString("operator") &&
            tok->previous() && tok->previous()->str() == MatchCompiler::makeConstString("=") &&
            tok->next() && tok->next()->str() != MatchCompiler::makeConstString("{") &&
            match164(tok->link())) {
            tok->link()->deleteThis();
            tok->deleteThis();
            continue;
        }

        while (match59(tok) &&
               tok->link() && tok->link()->previous() == tok->next()->link()) {
            // We have "(( *something* ))", remove the inner
            // parentheses
            tok->deleteNext();
            tok->link()->tokAt(-2)->deleteNext();
            ret = true;
        }

        if (isCPP() && match464(tok->tokAt(-2)) && match465(tok->link())) {
            // Remove the parentheses in "new (type)" constructs
            tok->link()->deleteThis();
            tok->deleteThis();
            ret = true;
        }

        if (match466(tok->previous())) {
            // Remove the parentheses
            tok->deleteThis();
            tok->deleteNext();
            ret = true;
        }

        if (match467(tok->previous())) {
            // Remove the parentheses
            tok->deleteThis();
            tok->deleteNext();
            ret = true;
        }

        if (match468(tok->previous()) &&
            tok->link()->previous() == tok->linkAt(2)) {
            // We have "( func ( *something* ))", remove the outer
            // parentheses
            tok->link()->deleteThis();
            tok->deleteThis();
            ret = true;
        }

        if (match469(tok->previous())) {
            // We have "( delete [| ]| var )", remove the outer
            // parentheses
            tok->link()->deleteThis();
            tok->deleteThis();
            ret = true;
        }

        if (!match470(tok->tokAt(-2)) &&
            match471(tok->previous()) &&
            (tok->previous()->str() != MatchCompiler::makeConstString("delete") || tok->next()->varId() > 0) &&
            match65(tok->link())) {
            tok->link()->deleteThis();
            tok->deleteThis();
            ret = true;
        }

        if (match472(tok->previous()) &&
            (tok->next()->varId() != 0 || match473(tok->tokAt(3))) && !tok->next()->isStandardType()) {
            // We have "( var )", remove the parentheses
            tok->deleteThis();
            tok->deleteNext();
            ret = true;
        }

        while (match474(tok->previous())) {
            Token *tok2 = tok->tokAt(2);
            while (match291(tok2)) {
                tok2 = tok2->tokAt(2);
            }
            if (tok2 != tok->link())
                break;
            // We have "( var . var . ... . var )", remove the parentheses
            tok = tok->previous();
            tok->deleteNext();
            tok2->deleteThis();
            ret = true;
            continue;
        }

        if (match475(tok->previous()) && match476(tok->link())) {
            const Token *tok2 = tok->next();
            while (tok2 && (match477(tok2) || tok2->isArithmeticalOp()))
                tok2 = tok2->next();
            if (tok2 && tok2->str() == MatchCompiler::makeConstString(")")) {
                tok->link()->deleteThis();
                tok->deleteThis();
                ret = true;
                continue;
            }
        }

        while (match478(tok->previous()) &&
               match479(tok->link()) &&
               !match480(tok->tokAt(-2)) && // Ticket #5709
               !findmatch481(tok, tok->link()) ) {
            // We have "( ... )", remove the parentheses
            tok->link()->deleteThis();
            tok->deleteThis();
            ret = true;
        }

        if (match482(tok->previous()) &&
            match483(tok->link())) {
            tok->link()->deleteThis();
            tok->deleteThis();
            ret = true;
        }

        // Simplify "!!operator !!%name%|)|>|>> ( %num%|%bool% ) %op%|;|,|)"
        if (match484(tok) &&
            tok->strAt(-2) != MatchCompiler::makeConstString("operator") &&
            tok->previous() &&
            !match485(tok->previous()) &&
            (!(isCPP() && match486(tok->previous())))) {
            tok->link()->deleteThis();
            tok->deleteThis();
            ret = true;
        }

        if (match487(tok->previous())) {
            // We may have a variable declaration looking like "type_name *(var_name)"
            Token *tok2 = tok->tokAt(-2);
            while (match488(tok2) && tok2->str() != MatchCompiler::makeConstString("operator")) {
                tok2 = tok2->previous();
            }
            if (tok2 && !match489(tok2)) {
                // Not a variable declaration
            } else {
                tok->deleteThis();
                tok->deleteNext();
            }
        }
    }
    return ret;
}

void Tokenizer::simplifyCharAt()
{
    // Replace "string"[0] with 's'
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match490(tok)) {
            const MathLib::bigint index = MathLib::toLongNumber(tok->strAt(2));
            // Check within range
            if (index >= 0 && index <= Token::getStrLength(tok)) {
                tok->str("'" + Token::getCharAt(tok, index) + "'");
                tok->deleteNext(3);
            }
        }
    }
}

void Tokenizer::simplifyReference()
{
    if (isC())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // starting executable scope..
        Token *start = const_cast<Token *>(startOfExecutableScope(tok));
        if (start) {
            tok = start;
            // replace references in this scope..
            Token * const end = tok->link();
            for (Token *tok2 = tok; tok2 && tok2 != end; tok2 = tok2->next()) {
                // found a reference..
                if (match491(tok2)) {
                    const unsigned int refId = tok2->tokAt(3)->varId();
                    if (!refId)
                        continue;

                    // replace reference in the code..
                    for (Token *tok3 = tok2->tokAt(7); tok3 && tok3 != end; tok3 = tok3->next()) {
                        if (tok3->varId() == refId) {
                            tok3->str(tok2->strAt(5));
                            tok3->varId(tok2->tokAt(5)->varId());
                        }
                    }

                    tok2->deleteNext(6+(tok2->strAt(6)==MatchCompiler::makeConstString(")") ? 1 : 0));
                }
            }
            tok = end;
        }
    }
}

bool Tokenizer::simplifyCalculations()
{
    return mTemplateSimplifier->simplifyCalculations();
}

void Tokenizer::simplifyOffsetPointerDereference()
{
    // Replace "*(str + num)" => "str[num]" and
    // Replace "*(str - num)" => "str[-num]"
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!tok->isName() && !tok->isLiteral()
            && !match492(tok)
            && match493(tok->next())) {

            // remove '* ('
            tok->deleteNext(2);

            // '+'->'['
            tok = tok->tokAt(2);
            Token* const openBraceTok = tok;
            const bool isNegativeIndex = (tok->str() == MatchCompiler::makeConstString("-"));
            tok->str("[");

            // Insert a "-" in front of the number or variable
            if (isNegativeIndex) {
                if (tok->next()->isName()) {
                    tok->insertToken("-");
                    tok = tok->next();
                } else
                    tok->next()->str(std::string("-") + tok->next()->str());
            }

            tok = tok->tokAt(2);
            tok->str("]");
            Token::createMutualLinks(openBraceTok, tok);
        }
    }
}

void Tokenizer::simplifyOffsetPointerReference()
{
    std::set<unsigned int> pod;
    for (const Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->isStandardType()) {
            tok = tok->next();
            while (tok && (tok->str() == MatchCompiler::makeConstString("*") || tok->isName())) {
                if (tok->varId() > 0) {
                    pod.insert(tok->varId());
                    break;
                }
                tok = tok->next();
            }
            if (!tok)
                break;
        }
    }

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match494(tok) &&
            (match495(tok->next()))) {
            tok = tok->next();

            if (tok->next()->varId()) {
                if (pod.find(tok->next()->varId()) == pod.end()) {
                    tok = tok->tokAt(5);
                    if (!tok)
                        syntaxError(tok);
                    continue;
                }
            }

            // '&' => '('
            tok->str("(");

            tok = tok->next();
            // '[' => '+'
            tok->deleteNext();
            tok->insertToken("+");

            tok = tok->tokAt(3);
            //remove ']'
            tok->str(")");
            Token::createMutualLinks(tok->tokAt(-4), tok);
        }
    }
}

void Tokenizer::simplifyNestedStrcat()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (! match496(tok)) {
            continue;
        }

        // find inner strcat call
        Token *tok2 = tok->tokAt(3);
        while (match497(tok2))
            tok2 = tok2->tokAt(2);

        if (tok2->strAt(3) != MatchCompiler::makeConstString(","))
            continue;

        // If we have this code:
        //   strcat(strcat(dst, foo), bar);
        // We move this part of code before all strcat() calls: strcat(dst, foo)
        // And place "dst" token where the code was.
        Token *prevTok = tok2->previous();

        // Move tokens to new place
        Token::move(tok2, tok2->next()->link(), tok);
        tok = tok2->next()->link();

        // Insert the "dst" token
        prevTok->insertToken(tok2->strAt(2));
        prevTok->next()->varId(tok2->tokAt(2)->varId());

        // Insert semicolon after the moved strcat()
        tok->insertToken(";");
    }
}

// Check if this statement is a duplicate definition.  A duplicate
// definition will hide the enumerator within it's scope so just
// skip the entire scope of the duplicate.
bool Tokenizer::duplicateDefinition(Token ** tokPtr)
{
    // check for an end of definition
    const Token * tok = *tokPtr;
    if (tok && match498(tok->next())) {
        const Token * end = tok->next();

        if (end->str() == MatchCompiler::makeConstString("[")) {
            end = end->link()->next();
        } else if (end->str() == MatchCompiler::makeConstString(",")) {
            // check for function argument
            if (match499(tok->previous()))
                return false;

            // find end of definition
            int level = 0;
            while (end->next() && (!match15(end->next()) ||
                                   (end->next()->str() == MatchCompiler::makeConstString(")") && level == 0))) {
                if (end->next()->str() == MatchCompiler::makeConstString("("))
                    ++level;
                else if (end->next()->str() == MatchCompiler::makeConstString(")"))
                    --level;

                end = end->next();
            }
        } else if (end->str() == MatchCompiler::makeConstString(")")) {
            // check for function argument
            if (tok->previous()->str() == MatchCompiler::makeConstString(","))
                return false;
        }

        if (end) {
            if (match19(end)) { // function parameter ?
                // make sure it's not a conditional
                if (match431(end->link()->previous()) || match500(end->link()->tokAt(-2)))
                    return false;

                // look backwards
                if (tok->previous()->str() == MatchCompiler::makeConstString("enum") ||
                    (match17(tok->previous()) &&
                     tok->previous()->str() != MatchCompiler::makeConstString("return")) ||
                    match501(tok->tokAt(-2))) {
                    // duplicate definition so skip entire function
                    *tokPtr = end->next()->link();
                    return true;
                }
            } else if (end->str() == MatchCompiler::makeConstString(">")) { // template parameter ?
                // look backwards
                if (tok->previous()->str() == MatchCompiler::makeConstString("enum") ||
                    (match17(tok->previous()) &&
                     tok->previous()->str() != MatchCompiler::makeConstString("return"))) {
                    // duplicate definition so skip entire template
                    while (end && end->str() != MatchCompiler::makeConstString("{"))
                        end = end->next();
                    if (end) {
                        *tokPtr = end->link();
                        return true;
                    }
                }
            } else {
                if (match502(tok->previous()))
                    return true;
                else if (match17(tok->previous())) {
                    // look backwards
                    const Token *back = tok;
                    while (back && back->isName())
                        back = back->previous();
                    if (!back || (match503(back) && !match504(back->next())))
                        return true;
                }
            }
        }
    }
    return false;
}

static const std::set<std::string> stdFunctionsPresentInC = {
    "strcat",
    "strcpy",
    "strncat",
    "strncpy",
    "free",
    "malloc",
    "strdup"
};

void Tokenizer::simplifyStd()
{
    if (isC())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("std"))
            continue;

        if (match505(tok->previous()) &&
            stdFunctionsPresentInC.find(tok->strAt(2)) != stdFunctionsPresentInC.end()) {
            tok->deleteNext();
            tok->deleteThis();
        }
    }
}

//---------------------------------------------------------------------------
// Helper functions for handling the tokens list
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

bool Tokenizer::IsScopeNoReturn(const Token *endScopeToken, bool *unknown) const
{
    std::string unknownFunc;
    const bool ret = mSettings->library.isScopeNoReturn(endScopeToken,&unknownFunc);
    if (unknown)
        *unknown = !unknownFunc.empty();
    if (!unknownFunc.empty() && mSettings->checkLibrary && mSettings->isEnabled(Settings::INFORMATION)) {
        // Is function global?
        bool globalFunction = true;
        if (match506(endScopeToken->tokAt(-2))) {
            const Token * const ftok = endScopeToken->linkAt(-2)->previous();
            if (ftok &&
                ftok->isName() &&
                ftok->function() &&
                ftok->function()->nestedIn &&
                ftok->function()->nestedIn->type != Scope::eGlobal) {
                globalFunction = false;
            }
        }

        // don't warn for nonglobal functions (class methods, functions hidden in namespaces) since they can't be configured yet
        // FIXME: when methods and namespaces can be configured properly, remove the "globalFunction" check
        if (globalFunction) {
            reportError(endScopeToken->previous(),
                        Severity::information,
                        "checkLibraryNoReturn",
                        "--check-library: Function " + unknownFunc + "() should have <noreturn> configuration");
        }
    }
    return ret;
}

//---------------------------------------------------------------------------

bool Tokenizer::isFunctionParameterPassedByValue(const Token *fpar) const
{
    // TODO: If symbol database is available, use it.
    const Token *ftok;

    // Look at function call, what parameter number is it?
    unsigned int parameter = 1;
    for (ftok = fpar->previous(); ftok; ftok = ftok->previous()) {
        if (ftok->str() == MatchCompiler::makeConstString("("))
            break;
        else if (ftok->str() == MatchCompiler::makeConstString(")"))
            ftok = ftok->link();
        else if (ftok->str() == MatchCompiler::makeConstString(","))
            ++parameter;
        else if (match88(ftok))
            break;
    }

    // Is this a function call?
    if (ftok && match507(ftok->tokAt(-2))) {
        const std::string& functionName(ftok->previous()->str());

        if (functionName == MatchCompiler::makeConstString("return"))
            return true;

        // Locate function declaration..
        for (const Token *tok = tokens(); tok; tok = tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("{"))
                tok = tok->link();
            else if (match83(tok) && tok->str() == functionName) {
                // Goto parameter
                tok = tok->tokAt(2);
                unsigned int par = 1;
                while (tok && par < parameter) {
                    if (tok->str() == MatchCompiler::makeConstString(")"))
                        break;
                    if (tok->str() == MatchCompiler::makeConstString(","))
                        ++par;
                    tok = tok->next();
                }
                if (!tok)
                    return false;

                // If parameter was found, determine if it's passed by value
                if (par == parameter) {
                    bool knowntype = false;
                    while (tok && tok->isName()) {
                        knowntype |= tok->isStandardType();
                        knowntype |= (tok->str() == MatchCompiler::makeConstString("struct"));
                        tok = tok->next();
                    }
                    if (!tok || !knowntype)
                        return false;
                    if (tok->str() != MatchCompiler::makeConstString(",") && tok->str() != MatchCompiler::makeConstString(")"))
                        return false;
                    return true;
                }
            }
        }
    }
    return false;
}

//---------------------------------------------------------------------------

void Tokenizer::eraseDeadCode(Token *begin, const Token *end)
{
    if (!begin)
        return;
    const bool isgoto = match508(begin->tokAt(-2));
    unsigned int indentlevel = 1,
                 indentcase = 0,
                 indentswitch = 0,
                 indentlabel = 0,
                 roundbraces = 0,
                 indentcheck = 0;
    std::vector<unsigned int> switchindents;
    bool checklabel = false;
    Token *tok = begin;
    Token *tokcheck = nullptr;
    while (tok->next() && tok->next() != end) {
        if (tok->next()->str() == MatchCompiler::makeConstString("(")) {
            ++roundbraces;
            tok->deleteNext();
            continue;
        } else if (tok->next()->str() == MatchCompiler::makeConstString(")")) {
            if (!roundbraces)
                break;  //too many ending round parentheses
            --roundbraces;
            tok->deleteNext();
            continue;
        }

        if (roundbraces) {
            tok->deleteNext();
            continue;
        }

        if (match509(tok)) {
            if (!checklabel) {
                if (!indentlabel) {
                    //remove 'switch ( ... )'
                    Token::eraseTokens(tok, tok->linkAt(2)->next());
                } else {
                    tok = tok->linkAt(2);
                }
                if (tok->next()->str() == MatchCompiler::makeConstString("{")) {
                    ++indentswitch;
                    indentcase = indentlevel + 1;
                    switchindents.push_back(indentcase);
                }
            } else {
                tok = tok->linkAt(2);
                if (match19(tok)) {
                    ++indentswitch;
                    indentcase = indentlevel + 1;
                    switchindents.push_back(indentcase);
                }
            }
        } else if (tok->next()->str() == MatchCompiler::makeConstString("{")) {
            ++indentlevel;
            if (!checklabel) {
                checklabel = true;
                tokcheck = tok;
                indentcheck = indentlevel;
                indentlabel = 0;
            }
            tok = tok->next();
        } else if (tok->next()->str() == MatchCompiler::makeConstString("}")) {
            --indentlevel;
            if (!indentlevel)
                break;

            if (!checklabel) {
                tok->deleteNext();
            } else {
                if (indentswitch && indentlevel == indentcase)
                    --indentlevel;
                if (indentlevel < indentcheck) {
                    const Token *end2 = tok->next();
                    tok = end2->link()->previous();  //return to initial '{'
                    if (indentswitch && match19(tok) && match509(tok->link()->tokAt(-2)))
                        tok = tok->link()->tokAt(-2);       //remove also 'switch ( ... )'
                    Token::eraseTokens(tok, end2->next());
                    checklabel = false;
                    tokcheck = nullptr;
                    indentcheck = 0;
                } else {
                    tok = tok->next();
                }
            }
            if (indentswitch && indentlevel <= indentcase) {
                --indentswitch;
                switchindents.pop_back();
                if (!indentswitch)
                    indentcase = 0;
                else
                    indentcase = switchindents[indentswitch-1];
            }
        } else if (match510(tok)) {
            const Token *tok2 = findmatch511(tok->next(), end) ;
            if (!tok2) {
                tok->deleteNext();
                continue;
            }
            if (indentlevel == 1)
                break;      //it seems like the function was called inside a case-default block.
            if (indentlevel == indentcase)
                ++indentlevel;
            tok2 = tok2->next();
            if (!checklabel || !indentswitch) {
                Token::eraseTokens(tok, tok2->next());
            } else {
                tok = const_cast<Token *>(tok2);
            }
        }  else if (match512(tok)) {
            if (indentlevel == 1)
                break;      //it seems like the function was called inside a case-default block.
            if (indentlevel == indentcase)
                ++indentlevel;
            if (!checklabel || !indentswitch) {
                tok->deleteNext(3);
            } else {
                tok = tok->tokAt(3);
            }
        } else if (match513(tok) && tok->next()->str() != MatchCompiler::makeConstString("default")) {
            if (checklabel) {
                indentlabel = indentlevel;
                tok = tokcheck->next();
                checklabel = false;
                indentlevel = indentcheck;
            } else {
                if (indentswitch) {
                    //Before stopping the function, since the 'switch()'
                    //instruction is removed, there's no sense to keep the
                    //case instructions. Remove them, if there are any.
                    Token *tok2 = tok->tokAt(3);
                    unsigned int indentlevel2 = indentlevel;
                    while (tok2->next() && tok2->next() != end) {
                        if (match143(tok2->next())) {
                            tok2 = tok2->next()->link();
                        } else if (match510(tok2)) {
                            const Token *tok3 = findmatch511(tok2->next(), end) ;
                            if (!tok3) {
                                tok2 = tok2->next();
                                continue;
                            }
                            Token::eraseTokens(tok2, tok3->next());
                        } else if (match512(tok2)) {
                            tok2->deleteNext(3);
                        } else if (tok2->next()->str() == MatchCompiler::makeConstString("}")) {
                            --indentlevel2;
                            if (indentlevel2 <= indentcase)
                                break;
                            tok2 = tok2->next();
                        } else {
                            tok2 = tok2->next();
                        }
                    }
                }
                break;  //stop removing tokens, we arrived to the label.
            }
        } else if (isgoto && match514(tok)) {
            //it's possible that code inside loop is not dead,
            //because of the possible presence of the label pointed by 'goto'
            const Token *start = tok->tokAt(2);
            if (start && start->str() == MatchCompiler::makeConstString("("))
                start = start->link()->next();
            if (start && start->str() == MatchCompiler::makeConstString("{")) {
                std::string labelpattern = "[{};] " + begin->previous()->str() + " : ;";
                bool simplify = true;
                for (Token *tok2 = start->next(); tok2 != start->link(); tok2 = tok2->next()) {
                    if (Token::Match(tok2, labelpattern.c_str())) {
                        simplify = false;
                        break;
                    }
                }
                //bailout for now
                if (!simplify)
                    break;
            }
            tok->deleteNext();
        } else {
            // no need to keep the other strings, remove them.
            if (tok->strAt(1) == MatchCompiler::makeConstString("while")) {
                if (tok->str() == MatchCompiler::makeConstString("}") && tok->link()->strAt(-1) == MatchCompiler::makeConstString("do"))
                    tok->link()->previous()->deleteThis();
            }
            tok->deleteNext();
        }
    }
}

//---------------------------------------------------------------------------

void Tokenizer::syntaxError(const Token *tok) const
{
    printDebugOutput(0);
    throw InternalError(tok, "syntax error", InternalError::SYNTAX);
}

void Tokenizer::syntaxError(const Token *tok, char c) const
{
    printDebugOutput(0);
    if (mConfiguration.empty())
        throw InternalError(tok,
                            std::string("Invalid number of character '") + c + "' when no macros are defined.",
                            InternalError::SYNTAX);
    else
        throw InternalError(tok,
                            std::string("Invalid number of character '") + c + "' when these macros are defined: '" + mConfiguration + "'.",
                            InternalError::SYNTAX);
}

void Tokenizer::syntaxErrorC(const Token *tok, const std::string &what) const
{
    printDebugOutput(0);
    throw InternalError(tok, "Code '"+what+"' is invalid C code. Use --std or --language to configure the language.", InternalError::SYNTAX);
}

void Tokenizer::unhandled_macro_class_x_y(const Token *tok) const
{
    reportError(tok,
                Severity::information,
                "class_X_Y",
                "The code '" +
                tok->str() + " " +
                tok->strAt(1) + " " +
                tok->strAt(2) + " " +
                tok->strAt(3) + "' is not handled. You can use -I or --include to add handling of this code.");
}

void Tokenizer::macroWithSemicolonError(const Token *tok, const std::string &macroName) const
{
    reportError(tok,
                Severity::information,
                "macroWithSemicolon",
                "Ensure that '" + macroName + "' is defined either using -I, --include or -D.");
}

void Tokenizer::cppcheckError(const Token *tok) const
{
    printDebugOutput(0);
    throw InternalError(tok, "Analysis failed. If the code is valid then please report this failure.", InternalError::INTERNAL);
}
/**
 * Helper function to check whether number is equal to integer constant X
 * or floating point pattern X.0
 * @param s the string to check
 * @param intConstant the integer constant to check against
 * @param floatConstant the string with stringified float constant to check against
 * @return true in case s is equal to X or X.0 and false otherwise.
 */
static bool isNumberOneOf(const std::string &s, const MathLib::bigint& intConstant, const char* floatConstant)
{
    if (MathLib::isInt(s)) {
        if (MathLib::toLongNumber(s) == intConstant)
            return true;
    } else if (MathLib::isFloat(s)) {
        if (MathLib::toString(MathLib::toDoubleNumber(s)) == floatConstant)
            return true;
    }
    return false;
}

// ------------------------------------------------------------------------
// Helper function to check whether number is zero (0 or 0.0 or 0E+0) or not?
// @param s the string to check
// @return true in case s is zero and false otherwise.
// ------------------------------------------------------------------------
bool Tokenizer::isZeroNumber(const std::string &s)
{
    return isNumberOneOf(s, 0L, "0.0");
}

// ------------------------------------------------------------------------
// Helper function to check whether number is one (1 or 0.1E+1 or 1E+0) or not?
// @param s the string to check
// @return true in case s is one and false otherwise.
// ------------------------------------------------------------------------
bool Tokenizer::isOneNumber(const std::string &s)
{
    if (!MathLib::isPositive(s))
        return false;
    return isNumberOneOf(s, 1L, "1.0");
}

// ------------------------------------------------------------------------
// Helper function to check whether number is two (2 or 0.2E+1 or 2E+0) or not?
// @param s the string to check
// @return true in case s is two and false otherwise.
// ------------------------------------------------------------------------
bool Tokenizer::isTwoNumber(const std::string &s)
{
    if (!MathLib::isPositive(s))
        return false;
    return isNumberOneOf(s, 2L, "2.0");
}

// ------------------------------------------------------
// Simplify math functions.
// It simplifies the following functions: atol(), fmin(),
// fminl(), fminf(), fmax(), fmaxl(), fmaxf(), pow(),
// powf(), powl(), cbrt(), cbrtl(), cbtrf(), sqrt(),
// sqrtf(), sqrtl(), exp(), expf(), expl(), exp2(),
// exp2f(), exp2l(), log2(), log2f(), log2l(), log1p(),
// log1pf(), log1pl(), log10(), log10l(), log10f(),
// log(), logf(), logl(), logb(), logbf(), logbl(), acosh()
// acoshf(), acoshl(), acos(), acosf(), acosl(), cosh()
// coshf(), coshf(), cos(), cosf(), cosl(), erfc(),
// erfcf(), erfcl(), ilogb(), ilogbf(), ilogbf(), erf(),
// erfl(), erff(), asin(), asinf(), asinf(), asinh(),
// asinhf(), asinhl(), tan(), tanf(), tanl(), tanh(),
// tanhf(), tanhl(), atan(), atanf(), atanl(), atanh(),
// atanhf(), atanhl(), expm1(), expm1l(), expm1f(), sin(),
// sinf(), sinl(), sinh(), sinhf(), sinhl()
// in the tokenlist.
//
// Reference:
// - http://www.cplusplus.com/reference/cmath/
// ------------------------------------------------------
void Tokenizer::simplifyMathFunctions()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->isName() && !tok->varId() && tok->strAt(1) == MatchCompiler::makeConstString("(")) { // precondition for function
            bool simplifcationMade = false;
            if (match515(tok)) { //@todo Add support for atoll()
                if (match516(tok->tokAt(-2))) {
                    tok = tok->tokAt(-2);// set token index two steps back
                    tok->deleteNext(2);  // delete "std ::"
                }
                const std::string& strNumber = tok->tokAt(2)->strValue(); // get number
                const bool isNotAnInteger = (!MathLib::isInt(strNumber));// check: is not an integer
                if (strNumber.empty() || isNotAnInteger) {
                    // Ignore strings which we can't convert
                    continue;
                }
                // Convert string into a number and insert into token list
                tok->str(MathLib::toString(MathLib::toLongNumber(strNumber)));
                // remove ( %num% )
                tok->deleteNext(3);
                simplifcationMade = true;
            } else if (match517(tok)) {
                // Simplify: sqrt(0) = 0 and cbrt(0) == 0
                //           sqrt(1) = 1 and cbrt(1) == 1
                // get number string
                const std::string& parameter(tok->strAt(2));
                // is parameter 0 ?
                if (isZeroNumber(parameter)) {
                    tok->deleteNext(3);  // delete tokens
                    tok->str("0"); // insert result into token list
                    simplifcationMade = true;
                } else if (isOneNumber(parameter)) {
                    tok->deleteNext(3);  // delete tokens
                    tok->str("1"); // insert result into token list
                    simplifcationMade = true;
                }
            } else if (match518(tok)) {
                // Simplify: exp[f|l](0)  = 1 and exp2[f|l](0) = 1
                //           cosh[f|l](0) = 1 and cos[f|l](0)  = 1
                //           erfc[f|l](0) = 1
                // get number string
                const std::string& parameter(tok->strAt(2));
                // is parameter 0 ?
                if (isZeroNumber(parameter)) {
                    tok->deleteNext(3);  // delete tokens
                    tok->str("1"); // insert result into token list
                    simplifcationMade = true;
                }
            } else if (match519(tok)) {
                // Simplify: log1p[f|l](0) = 0 and sin[f|l](0)  = 0
                //           sinh[f|l](0)  = 0 and erf[f|l](0)  = 0
                //           asin[f|l](0)  = 0 and sinh[f|l](0) = 0
                //           tan[f|l](0)   = 0 and tanh[f|l](0) = 0
                //           atan[f|l](0)  = 0 and atanh[f|l](0)= 0
                //           expm1[f|l](0) = 0
                // get number string
                const std::string& parameter(tok->strAt(2));
                // is parameter 0 ?
                if (isZeroNumber(parameter)) {
                    tok->deleteNext(3);  // delete tokens
                    tok->str("0"); // insert result into token list
                    simplifcationMade = true;
                }
            } else if (match520(tok)) {
                // Simplify: log2[f|l](1)  = 0 , log10[f|l](1)  = 0
                //           log[f|l](1)   = 0 , logb10[f|l](1) = 0
                //           acosh[f|l](1) = 0 , acos[f|l](1)   = 0
                //           ilogb[f|l](1) = 0
                // get number string
                const std::string& parameter(tok->strAt(2));
                // is parameter 1 ?
                if (isOneNumber(parameter)) {
                    tok->deleteNext(3);  // delete tokens
                    tok->str("0"); // insert result into token list
                    simplifcationMade = true;
                }
            } else if (match521(tok)) {
                // @todo if one of the parameters is NaN the other is returned
                // e.g. printf ("fmin (NaN, -1.0) = %f\n", fmin(NaN,-1.0));
                // e.g. printf ("fmin (-1.0, NaN) = %f\n", fmin(-1.0,NaN));
                const std::string& strLeftNumber(tok->strAt(2));
                const std::string& strRightNumber(tok->strAt(4));
                const bool isLessEqual =  MathLib::isLessEqual(strLeftNumber, strRightNumber);
                // case: left <= right ==> insert left
                if (isLessEqual) {
                    tok->str(strLeftNumber); // insert e.g. -1.0
                    tok->deleteNext(5);      // delete e.g. fmin ( -1.0, 1.0 )
                    simplifcationMade = true;
                } else { // case left > right ==> insert right
                    tok->str(strRightNumber); // insert e.g. 0.0
                    tok->deleteNext(5);       // delete e.g. fmin ( 1.0, 0.0 )
                    simplifcationMade = true;
                }
            } else if (match522(tok)) {
                // @todo if one of the parameters is NaN the other is returned
                // e.g. printf ("fmax (NaN, -1.0) = %f\n", fmax(NaN,-1.0));
                // e.g. printf ("fmax (-1.0, NaN) = %f\n", fmax(-1.0,NaN));
                const std::string& strLeftNumber(tok->strAt(2));
                const std::string& strRightNumber(tok->strAt(4));
                const bool isLessEqual =  MathLib::isLessEqual(strLeftNumber, strRightNumber);
                // case: left <= right ==> insert right
                if (isLessEqual) {
                    tok->str(strRightNumber);// insert e.g. 1.0
                    tok->deleteNext(5);      // delete e.g. fmax ( -1.0, 1.0 )
                    simplifcationMade = true;
                } else { // case left > right ==> insert left
                    tok->str(strLeftNumber);  // insert e.g. 1.0
                    tok->deleteNext(5);       // delete e.g. fmax ( 1.0, 0.0 )
                    simplifcationMade = true;
                }
            } else if (match523(tok)) {
                if (match524(tok->tokAt(2))) {
                    // In case of pow ( 0 , anyNumber > 0): It can be simplified to 0
                    // In case of pow ( 0 , 0 ): It simplified to 1
                    // In case of pow ( 1 , anyNumber ): It simplified to 1
                    const std::string& leftNumber(tok->strAt(2)); // get the left parameter
                    const std::string& rightNumber(tok->strAt(4)); // get the right parameter
                    const bool isLeftNumberZero = isZeroNumber(leftNumber);
                    const bool isLeftNumberOne = isOneNumber(leftNumber);
                    const bool isRightNumberZero = isZeroNumber(rightNumber);
                    if (isLeftNumberZero && !isRightNumberZero && MathLib::isPositive(rightNumber)) { // case: 0^(y) = 0 and y > 0
                        tok->deleteNext(5); // delete tokens
                        tok->str("0");  // insert simplified result
                        simplifcationMade = true;
                    } else if (isLeftNumberZero && isRightNumberZero) { // case: 0^0 = 1
                        tok->deleteNext(5); // delete tokens
                        tok->str("1");  // insert simplified result
                        simplifcationMade = true;
                    } else if (isLeftNumberOne) { // case 1^(y) = 1
                        tok->deleteNext(5); // delete tokens
                        tok->str("1");  // insert simplified result
                        simplifcationMade = true;
                    }
                }
                if (match525(tok->tokAt(2))) {
                    // In case of pow( x , 1 ): It can be simplified to x.
                    const std::string& leftParameter(tok->strAt(2)); // get the left parameter
                    const std::string& rightNumber(tok->strAt(4)); // get right number
                    if (isOneNumber(rightNumber)) { // case: x^(1) = x
                        tok->str(leftParameter);  // insert simplified result
                        tok->deleteNext(5); // delete tokens
                        simplifcationMade = true;
                    } else if (isZeroNumber(rightNumber)) { // case: x^(0) = 1
                        tok->deleteNext(5); // delete tokens
                        tok->str("1");  // insert simplified result
                        simplifcationMade = true;
                    }
                }
            }
            // Jump back to begin of statement if a simplification was performed
            if (simplifcationMade) {
                while (tok->previous() && tok->str() != MatchCompiler::makeConstString(";")) {
                    tok = tok->previous();
                }
            }
        }
    }
}

void Tokenizer::simplifyComma()
{
    bool inReturn = false;

    for (Token *tok = list.front(); tok; tok = tok->next()) {

        // skip enums
        if (match526(tok)) {
            skipEnumBody(&tok);
        }
        if (!tok)
            syntaxError(nullptr); // invalid code like in #4195

        if (match121(tok) || match527(tok->previous())) {
            tok = tok->link();
            continue;
        }

        if (match528(tok) && match19(tok->linkAt(1))) {
            tok = tok->linkAt(1)->linkAt(1);
            continue;
        }

        // Skip unhandled template specifiers..
        if (tok->link() && tok->str() == MatchCompiler::makeConstString("<"))
            tok = tok->link();

        if (tok->str() == MatchCompiler::makeConstString("return") && match88(tok->previous()))
            inReturn = true;

        if (inReturn && match529(tok))
            inReturn = false;

        if (!tok->next() || tok->str() != MatchCompiler::makeConstString(","))
            continue;

        // We must not accept just any keyword, e.g. accepting int
        // would cause function parameters to corrupt.
        if (isCPP() && tok->strAt(1) == MatchCompiler::makeConstString("delete")) {
            // Handle "delete a, delete b;"
            tok->str(";");
        }

        if (isCPP() && match530(tok->tokAt(-2)) &&
            tok->next()->varId() != 0) {
            // Handle "delete a, b;" - convert to delete a; b;
            tok->str(";");
        } else if (!inReturn && tok->tokAt(-2)) {
            bool replace = false;
            for (Token *tok2 = tok->previous(); tok2; tok2 = tok2->previous()) {
                if (tok2->str() == MatchCompiler::makeConstString("=")) {
                    // Handle "a = 0, b = 0;"
                    replace = true;
                } else if (isCPP() && (match531(tok2) ||
                                       match532(tok2))) {
                    // Handle "delete a, a = 0;"
                    replace = true;
                } else if (match533(tok2)) {
                    if (replace && match88(tok2))
                        tok->str(";");
                    break;
                }
            }
        }

        // find token where return ends and also count commas
        if (inReturn) {
            Token *startFrom = nullptr;    // "[;{}]" token before "return"
            Token *endAt = nullptr;        // first ";" token after "[;{}] return"

            // find "; return" pattern before comma
            for (Token *tok2 = tok->previous(); tok2; tok2 = tok2->previous()) {
                if (tok2->str() == MatchCompiler::makeConstString("return")) {
                    startFrom = tok2->previous();
                    break;
                }
            }
            if (!startFrom)
                // to be very sure...
                return;
            std::size_t commaCounter = 0;
            for (Token *tok2 = startFrom->next(); tok2; tok2 = tok2->next()) {
                if (tok2->str() == MatchCompiler::makeConstString(";")) {
                    endAt = tok2;
                    break;

                } else if (match121(tok2) ||
                           (tok2->str() == MatchCompiler::makeConstString("{") && tok2->previous() && tok2->previous()->str() == MatchCompiler::makeConstString("="))) {
                    tok2 = tok2->link();

                } else if (tok2->str() == MatchCompiler::makeConstString(",")) {
                    ++commaCounter;
                }
            }

            if (!endAt)
                //probably a syntax error
                return;

            if (commaCounter) {
                // change tokens:
                // "; return a ( ) , b ( ) , c ;"
                // to
                // "; a ( ) ; b ( ) ; return c ;"

                // remove "return"
                startFrom->deleteNext();
                for (Token *tok2 = startFrom->next(); tok2 != endAt; tok2 = tok2->next()) {
                    if (match121(tok2) ||
                        (tok2->str() == MatchCompiler::makeConstString("{") && tok2->previous() && tok2->previous()->str() == MatchCompiler::makeConstString("="))) {
                        tok2 = tok2->link();

                    } else if (tok2->str() == MatchCompiler::makeConstString(",")) {
                        tok2->str(";");
                        --commaCounter;
                        if (commaCounter == 0) {
                            tok2->insertToken("return");
                        }
                    }
                }
                tok = endAt;
            }
        }
    }
}

void Tokenizer::checkConfiguration() const
{
    if (!mSettings->checkConfiguration)
        return;
    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        if (!match6(tok))
            continue;
        if (tok->isControlFlowKeyword())
            continue;
        for (const Token *tok2 = tok->tokAt(2); tok2 && tok2->str() != MatchCompiler::makeConstString(")"); tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString(";")) {
                macroWithSemicolonError(tok, tok->str());
                break;
            }
            if (match534(tok2))
                tok2 = tok2->link();
        }
    }
}

void Tokenizer::validateC() const
{
    if (isCPP())
        return;
    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        // That might trigger false positives, but it's much faster to have this truncated pattern
        if (match535(tok))
            syntaxErrorC(tok, "C++ cast <...");
        // Template function..
        if (match536(tok)) {
            const Token *tok2 = tok->tokAt(5);
            while (tok2 && !match537(tok2))
                tok2 = tok2->next();
            if (match19(tok2))
                syntaxErrorC(tok, tok->str() + '<' + tok->strAt(2) + ">() {}");
        }
        if (tok->previous() && !match88(tok->previous()))
            continue;
        if (match538(tok))
            syntaxErrorC(tok, "using namespace " + tok->strAt(2));
        if (match539(tok))
            syntaxErrorC(tok, "template<...");
        if (match201(tok))
            syntaxErrorC(tok, tok->str() + tok->strAt(1) + tok->strAt(2));
        if (match540(tok))
            syntaxErrorC(tok, tok->str() + tok->strAt(1) + tok->strAt(2));
    }
}

void Tokenizer::validate() const
{
    std::stack<const Token *> linkTokens;
    const Token *lastTok = nullptr;
    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        lastTok = tok;
        if (match541(tok) || (tok->str() == MatchCompiler::makeConstString("<") && tok->link())) {
            if (tok->link() == nullptr)
                cppcheckError(tok);

            linkTokens.push(tok);
        }

        else if (match542(tok) || (match486(tok) && tok->link())) {
            if (tok->link() == nullptr)
                cppcheckError(tok);

            if (linkTokens.empty() == true)
                cppcheckError(tok);

            if (tok->link() != linkTokens.top())
                cppcheckError(tok);

            if (tok != tok->link()->link())
                cppcheckError(tok);

            linkTokens.pop();
        }

        else if (tok->link() != nullptr)
            cppcheckError(tok);
    }

    if (!linkTokens.empty())
        cppcheckError(linkTokens.top());

    // Validate that the Tokenizer::list.back() is updated correctly during simplifications
    if (lastTok != list.back())
        cppcheckError(lastTok);
}

static const Token *findUnmatchedTernaryOp(const Token * const begin, const Token * const end, unsigned depth = 0)
{
    std::stack<const Token *> ternaryOp;
    for (const Token *tok = begin; tok != end && tok->str() != MatchCompiler::makeConstString(";"); tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("?"))
            ternaryOp.push(tok);
        else if (!ternaryOp.empty() && tok->str() == MatchCompiler::makeConstString(":"))
            ternaryOp.pop();
        else if (depth < 100 && match121(tok)) {
            const Token *inner = findUnmatchedTernaryOp(tok->next(), tok->link(), depth+1);
            if (inner)
                return inner;
            tok = tok->link();
        }
    }
    return ternaryOp.empty() ? nullptr : ternaryOp.top();
}

void Tokenizer::findGarbageCode() const
{
    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        if (match543(tok)) { // if|while|for|switch (EXPR) { ... }
            if (tok->previous() && !match544(tok->previous()))
                syntaxError(tok);
            if (match175(tok->previous()))
                continue;
            if (!match545(tok->next()))
                syntaxError(tok);
            if (tok->str() != MatchCompiler::makeConstString("for")) {
                if (isGarbageExpr(tok->next(), tok->linkAt(1)))
                    syntaxError(tok);
            }
        }
    }

    // case keyword must be inside switch
    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        if (match546(tok)) {
            if (match19(tok->linkAt(1))) {
                tok = tok->linkAt(1)->linkAt(1);
                continue;
            }
            const Token *switchToken = tok;
            tok = tok->linkAt(1);
            if (!tok)
                syntaxError(switchToken);
            // Look for the end of the switch statement, i.e. the first semi-colon or '}'
            for (; tok ; tok = tok->next()) {
                if (tok->str() == MatchCompiler::makeConstString("{")) {
                    tok = tok->link();
                }
                if (match547(tok)) {
                    // We're at the end of the switch block
                    if (tok->str() == MatchCompiler::makeConstString("}") && tok->strAt(-1) == MatchCompiler::makeConstString(":")) // Invalid case
                        syntaxError(switchToken);
                    break;
                }
            }
            if (!tok)
                break;
        } else if (tok->str() == MatchCompiler::makeConstString("(")) {
            tok = tok->link();
        } else if (tok->str() == MatchCompiler::makeConstString("case")) {
            syntaxError(tok);
        }
    }

    for (const Token *tok = tokens(); tok ; tok = tok->next()) {
        if (!match412(tok)) // find for loops
            continue;
        // count number of semicolons
        unsigned int semicolons = 0;
        const Token* const startTok = tok;
        tok = tok->next()->link()->previous(); // find ")" of the for-loop
        // walk backwards until we find the beginning (startTok) of the for() again
        for (; tok != startTok; tok = tok->previous()) {
            if (tok->str() == MatchCompiler::makeConstString(";")) { // do the counting
                semicolons++;
            } else if (tok->str() == MatchCompiler::makeConstString(")")) { // skip pairs of ( )
                tok = tok->link();
            }
        }
        // if we have an invalid number of semicolons inside for( ), assume syntax error
        if ((semicolons == 1) || (semicolons > 2)) {
            syntaxError(tok);
        }
    }

    // Operators without operands..
    const Token *templateEndToken = nullptr;
    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        if (!templateEndToken) {
            if (tok->str() == MatchCompiler::makeConstString("<") && isCPP())
                templateEndToken = tok->findClosingBracket();
        } else {
            if (templateEndToken == tok)
                templateEndToken = nullptr;
            if (match548(tok))
                continue;
        }
        if (match549(tok))
            syntaxError(tok);
        if (match550(tok))
            syntaxError(tok);
        if (match551(tok))
            syntaxError(tok);
        if (match552(tok) && !(isCPP() && match553(tok->previous())))
            syntaxError(tok);
    }

    // ternary operator without :
    if (const Token *ternaryOp = findUnmatchedTernaryOp(tokens(), nullptr))
        syntaxError(ternaryOp);

    // Code must not start with an arithmetical operand
    if (match554(list.front()))
        syntaxError(list.front());

    // Code must end with } ; ) NAME
    if (!match555(list.back()))
        syntaxError(list.back());
    if (list.back()->str() == MatchCompiler::makeConstString(")") && !match6(list.back()->link()->previous()))
        syntaxError(list.back());
    if (match556(list.back()))
        syntaxError(list.back());
    if ((list.back()->str()==MatchCompiler::makeConstString(")") || list.back()->str()==MatchCompiler::makeConstString("}")) && list.back()->previous() && list.back()->previous()->isControlFlowKeyword())
        syntaxError(list.back()->previous());

    // Garbage templates..
    if (isCPP()) {
        for (const Token *tok = tokens(); tok; tok = tok->next()) {
            if (!match358(tok))
                continue;
            if (tok->previous() && !match557(tok->previous()))
                syntaxError(tok);
            const Token * const tok1 = tok;
            tok = tok->next()->findClosingBracket();
            if (!tok)
                syntaxError(tok1);
            if (!match558(tok) &&
                !match559(tok))
                syntaxError(tok->next() ? tok->next() : tok1);
        }
    }

    // Objective C/C++
    for (const Token *tok = tokens(); tok; tok = tok->next()) {
        if (match560(tok))
            syntaxError(tok->next());
    }
}


bool Tokenizer::isGarbageExpr(const Token *start, const Token *end)
{
    for (const Token *tok = start; tok != end; tok = tok->next()) {
        if (tok->isControlFlowKeyword())
            return true;
        if (tok->str() == MatchCompiler::makeConstString(";"))
            return true;
        if (tok->str() == MatchCompiler::makeConstString("{"))
            tok = tok->link();
    }
    return false;
}

std::string Tokenizer::simplifyString(const std::string &source)
{
    std::string str = source;

    for (std::string::size_type i = 0; i + 1U < str.size(); ++i) {
        if (str[i] != '\\')
            continue;

        int c = 'a';   // char
        unsigned int sz = 0;    // size of stringdata
        if (str[i+1] == 'x') {
            sz = 2;
            while (sz < 4 && std::isxdigit((unsigned char)str[i+sz]))
                sz++;
            if (sz > 2) {
                std::istringstream istr(str.substr(i+2, sz-2));
                istr >> std::hex >> c;
            }
        } else if (MathLib::isOctalDigit(str[i+1])) {
            sz = 2;
            while (sz < 4 && MathLib::isOctalDigit(str[i+sz]))
                sz++;
            std::istringstream istr(str.substr(i+1, sz-1));
            istr >> std::oct >> c;
            str = str.substr(0,i) + (char)c + str.substr(i+sz);
            continue;
        }

        if (sz <= 2)
            i++;
        else if (i+sz < str.size())
            str.replace(i, sz, std::string(1U, (char)c));
        else
            str.replace(i, str.size() - i - 1U, "a");
    }

    return str;
}

void Tokenizer::simplifyWhile0()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // while (0)
        const bool while0(match561(tok->previous()));

        // for (0) - not banal, ticket #3140
        const bool for0((match562(tok->previous()) &&
                         tok->strAt(2) == tok->strAt(6) && tok->strAt(4) == tok->strAt(8)) ||
                        (match563(tok->previous()) &&
                         tok->strAt(3) == tok->strAt(7) && tok->strAt(5) == tok->strAt(9)));

        if (!while0 && !for0)
            continue;

        if (while0 && tok->previous()->str() == MatchCompiler::makeConstString("}")) {
            // find "do"
            Token *tok2 = tok->previous()->link();
            tok2 = tok2->previous();
            if (tok2 && tok2->str() == MatchCompiler::makeConstString("do")) {
                const bool flowmatch = findmatch564(tok2, tok)  != nullptr;
                // delete "do ({)"
                tok2->deleteThis();
                if (!flowmatch)
                    tok2->deleteThis();

                // delete "(}) while ( 0 ) (;)"
                tok = tok->previous();
                tok->deleteNext(4);  // while ( 0 )
                if (tok->next() && tok->next()->str() == MatchCompiler::makeConstString(";"))
                    tok->deleteNext(); // ;
                if (!flowmatch)
                    tok->deleteThis(); // }

                continue;
            }
        }

        // remove "while (0) { .. }"
        if (match19(tok->next()->link())) {
            Token *end = tok->next()->link(), *old_prev = tok->previous();
            end = end->next()->link();
            if (match565(tok))
                old_prev = end->link();
            eraseDeadCode(old_prev, end->next());
            if (old_prev && old_prev->next())
                tok = old_prev->next();
            else
                break;
        }
    }
}

void Tokenizer::simplifyErrNoInWhile()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("errno"))
            continue;

        Token *endpar = nullptr;
        if (match566(tok->previous()))
            endpar = tok->tokAt(3);
        else if (match567(tok->tokAt(-2)))
            endpar = tok->tokAt(4);
        else
            continue;

        if (match568(endpar->link()->previous())) {
            Token *tok1 = tok->previous();
            if (tok1->str() == MatchCompiler::makeConstString("("))
                tok1 = tok1->previous();

            // erase "&& errno == EINTR"
            tok1 = tok1->previous();
            Token::eraseTokens(tok1, endpar);

            // tok is invalid.. move to endpar
            tok = endpar;
        }
    }
}


void Tokenizer::simplifyFuncInWhile()
{
    unsigned int count = 0;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!match569(tok))
            continue;

        Token *func = tok->tokAt(2);
        const Token * const var = tok->tokAt(4);
        Token * const end = tok->next()->link()->next()->link();

        const unsigned int varid = ++mVarId; // Create new variable
        const std::string varname("cppcheck:r" + MathLib::toString(++count));
        tok->str("int");
        tok->next()->insertToken(varname);
        tok->tokAt(2)->varId(varid);
        tok->insertToken("while");
        tok->insertToken(";");
        tok->insertToken(")");
        tok->insertToken(var->str());
        tok->next()->varId(var->varId());
        tok->insertToken("(");
        tok->insertToken(func->str());
        tok->insertToken("=");
        tok->insertToken(varname);
        tok->next()->varId(varid);
        Token::createMutualLinks(tok->tokAt(4), tok->tokAt(6));
        end->previous()->insertToken(varname);
        end->previous()->varId(varid);
        end->previous()->insertToken("=");
        Token::move(func, func->tokAt(3), end->previous());
        end->previous()->insertToken(";");

        tok = end;
    }
}

void Tokenizer::simplifyStructDecl()
{
    const bool cpp = isCPP();

    // A counter that is used when giving unique names for anonymous structs.
    unsigned int count = 0;

    // Skip simplification of unions in class definition
    std::stack<bool> skip; // true = in function, false = not in function
    skip.push(false);

    // Add names for anonymous structs
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!tok->isName())
            continue;
        // check for anonymous struct/union
        if (match570(tok)) {
            if (match571(tok->next()->link())) {
                tok->insertToken("Anonymous" + MathLib::toString(count++));
            }
        }
        // check for derived anonymous class/struct
        else if (cpp && match572(tok)) {
            const Token *tok1 = findmatch573(tok) ;
            if (tok1 && match571(tok1->link())) {
                tok->insertToken("Anonymous" + MathLib::toString(count++));
            }
        }
        // check for anonymous enum
        else if ((match575(tok) && match574(tok->next()->link())) ||
                 (match576(tok) && match574(tok->linkAt(3)))) {
            tok->insertToken("Anonymous" + MathLib::toString(count++));
        }
    }

    for (Token *tok = list.front(); tok; tok = tok->next()) {

        // check for start of scope and determine if it is in a function
        if (tok->str() == MatchCompiler::makeConstString("{"))
            skip.push(match577(tok->previous()));

        // end of scope
        else if (tok->str() == MatchCompiler::makeConstString("}") && !skip.empty())
            skip.pop();

        // check for named struct/union
        else if (match578(tok)) {
            Token *start = tok;
            while (match17(start->previous()))
                start = start->previous();
            const Token * const type = tok->next();
            Token *next = tok->tokAt(2);

            while (next && next->str() != MatchCompiler::makeConstString("{"))
                next = next->next();
            if (!next)
                continue;
            skip.push(false);
            tok = next->link();
            if (!tok)
                break; // see #4869 segmentation fault in Tokenizer::simplifyStructDecl (invalid code)
            Token *restart = next;

            // check for named type
            if (match579(tok->next())) {
                tok->insertToken(";");
                tok = tok->next();
                while (!match580(start)) {
                    tok->insertToken(start->str());
                    tok = tok->next();
                    start->deleteThis();
                }
                if (!tok)
                    break; // see #4869 segmentation fault in Tokenizer::simplifyStructDecl (invalid code)
                tok->insertToken(type->str());
                if (start->str() != MatchCompiler::makeConstString("class")) {
                    tok->insertToken(start->str());
                    tok = tok->next();
                }

                tok = tok->tokAt(2);

                // check for initialization
                if (tok && (tok->next()->str() == MatchCompiler::makeConstString("(") || tok->next()->str() == MatchCompiler::makeConstString("{"))) {
                    tok->insertToken("=");
                    tok = tok->next();

                    if (start->str() == MatchCompiler::makeConstString("enum")) {
                        if (tok->next()->str() == MatchCompiler::makeConstString("{")) {
                            tok->next()->str("(");
                            tok->linkAt(1)->str(")");
                        }
                    }
                }
            }

            tok = restart;
        }

        // check for anonymous struct/union
        else if (match570(tok)) {
            const bool inFunction = skip.top();
            skip.push(false);
            Token *tok1 = tok;

            Token *restart = tok->next();
            tok = tok->next()->link();

            // unnamed anonymous struct/union so possibly remove it
            if (tok && tok->next() && tok->next()->str() == MatchCompiler::makeConstString(";")) {
                if (inFunction && tok1->str() == MatchCompiler::makeConstString("union")) {
                    // Try to create references in the union..
                    Token *tok2 = tok1->tokAt(2);
                    while (tok2) {
                        if (match581(tok2))
                            tok2 = tok2->tokAt(3);
                        else
                            break;
                    }
                    if (!match168(tok2))
                        continue;
                    Token *vartok = nullptr;
                    tok2 = tok1->tokAt(2);
                    while (match581(tok2)) {
                        if (!vartok) {
                            vartok = tok2->next();
                            tok2 = tok2->tokAt(3);
                        } else {
                            tok2->insertToken("&");
                            tok2 = tok2->tokAt(2);
                            tok2->insertToken(vartok->str());
                            tok2->next()->varId(vartok->varId());
                            tok2->insertToken("=");
                            tok2 = tok2->tokAt(4);
                        }
                    }
                }

                // don't remove unnamed anonymous unions from a class, struct or union
                if (!(!inFunction && tok1->str() == MatchCompiler::makeConstString("union"))) {
                    skip.pop();
                    tok1->deleteThis();
                    if (tok1->next() == tok) {
                        tok1->deleteThis();
                        tok = tok1;
                    } else
                        tok1->deleteThis();
                    restart = tok1->previous();
                    tok->deleteThis();
                    if (tok->next())
                        tok->deleteThis();
                }
            }

            if (!restart) {
                simplifyStructDecl();
                return;
            } else if (!restart->next())
                return;

            tok = restart;
        }
    }
}

void Tokenizer::simplifyCallingConvention()
{
    const bool windows = mSettings->isWindowsPlatform();

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        while (match582(tok) || (windows && match583(tok))) {
            tok->deleteThis();
        }
    }
}

void Tokenizer::simplifyDeclspec()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        while (match584(tok) && tok->next()->link() && tok->next()->link()->next()) {
            if (match585(tok->tokAt(2))) {
                Token *tok1 = tok->next()->link()->next();
                while (tok1 && !match391(tok1)) {
                    tok1 = tok1->next();
                }
                if (tok1) {
                    if (tok->strAt(2) == MatchCompiler::makeConstString("noreturn"))
                        tok1->isAttributeNoreturn(true);
                    else
                        tok1->isAttributeNothrow(true);
                }
            } else if (tok->strAt(2) == MatchCompiler::makeConstString("property"))
                tok->next()->link()->insertToken("__property");

            Token::eraseTokens(tok, tok->next()->link()->next());
            tok->deleteThis();
        }
    }
}

void Tokenizer::simplifyAttribute()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match83(tok) && !mSettings->library.isNotLibraryFunction(tok)) {
            if (mSettings->library.isFunctionConst(tok->str(), true))
                tok->isAttributePure(true);
            if (mSettings->library.isFunctionConst(tok->str(), false))
                tok->isAttributeConst(true);
        }
        while (match586(tok) && tok->next()->link() && tok->next()->link()->next()) {
            if (match587(tok->tokAt(2))) {
                // prototype for constructor is: void func(void);
                if (!tok->next()->link()->next())
                    syntaxError(tok);

                if (tok->next()->link()->next()->str() == MatchCompiler::makeConstString("void")) { // __attribute__((constructor)) void func() {}
                    if (!tok->next()->link()->next()->next())
                        syntaxError(tok);

                    tok->next()->link()->next()->next()->isAttributeConstructor(true);
                } else if (tok->next()->link()->next()->str() == MatchCompiler::makeConstString(";") && tok->linkAt(-1) && tok->previous()->link()->previous()) // void func() __attribute__((constructor));
                    tok->previous()->link()->previous()->isAttributeConstructor(true);
                else // void __attribute__((constructor)) func() {}
                    tok->next()->link()->next()->isAttributeConstructor(true);
            }

            else if (match588(tok->tokAt(2))) {
                // prototype for destructor is: void func(void);
                if (!tok->next()->link()->next())
                    syntaxError(tok);

                if (tok->next()->link()->next()->str() == MatchCompiler::makeConstString("void")) // __attribute__((destructor)) void func() {}
                    tok->next()->link()->next()->next()->isAttributeDestructor(true);
                else if (tok->next()->link()->next()->str() == MatchCompiler::makeConstString(";") && tok->linkAt(-1) && tok->previous()->link()->previous()) // void func() __attribute__((destructor));
                    tok->previous()->link()->previous()->isAttributeDestructor(true);
                else // void __attribute__((destructor)) func() {}
                    tok->next()->link()->next()->isAttributeDestructor(true);
            }

            else if (match589(tok->tokAt(2))) {
                Token *vartok = nullptr;

                // check if after variable name
                if (match590(tok->next()->link()->next())) {
                    if (match17(tok->previous()))
                        vartok = tok->previous();
                }

                // check if before variable name
                else if (match17(tok->next()->link()->next()))
                    vartok = tok->next()->link()->next();

                if (vartok) {
                    const std::string &attribute(tok->strAt(3));
                    if (attribute.find("unused") != std::string::npos)
                        vartok->isAttributeUnused(true);
                    else
                        vartok->isAttributeUsed(true);
                }
            }

            else if (match591(tok->tokAt(2))) {
                Token *functok = nullptr;

                // type func(...) __attribute__((attribute));
                if (tok->previous() && tok->previous()->link() && match6(tok->previous()->link()->previous()))
                    functok = tok->previous()->link()->previous();

                // type __attribute__((attribute)) func() { }
                else {
                    Token *tok2 = tok->next()->link();
                    while (match592(tok2))
                        tok2 = tok2->linkAt(2);
                    if (match198(tok2))
                        functok = tok2->next();
                }

                if (functok) {
                    const std::string &attribute(tok->strAt(3));
                    if (attribute.find("pure") != std::string::npos)
                        functok->isAttributePure(true);
                    else if (attribute.find("const") != std::string::npos)
                        functok->isAttributeConst(true);
                    else if (attribute.find("noreturn") != std::string::npos)
                        functok->isAttributeNoreturn(true);
                    else if (attribute.find("nothrow") != std::string::npos)
                        functok->isAttributeNothrow(true);
                    else if (attribute.find("warn_unused_result") != std::string::npos)
                        functok->isAttributeNodiscard(true);
                }
            }

            else if (match593(tok->previous())) {
                tok->previous()->isAttributePacked(true);
            }

            Token::eraseTokens(tok, tok->next()->link()->next());
            tok->deleteThis();
        }
    }
}

void Tokenizer::simplifyCPPAttribute()
{
    if (mSettings->standards.cpp < Standards::CPP11 || isC())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!tok->link() || !match594(tok))
            continue;
        if (match595(tok->tokAt(2))) {
            const Token * head = tok->link()->next();
            while (match596(head))
                head = head->next();
            if (head && head->str() == MatchCompiler::makeConstString("(") && isFunctionHead(head, "{|;")) {
                if (tok->strAt(2) == MatchCompiler::makeConstString("noreturn"))
                    head->previous()->isAttributeNoreturn(true);
                else
                    head->previous()->isAttributeNodiscard(true);
            }
        }
        Token::eraseTokens(tok, tok->link()->next());
        tok->deleteThis();
    }
}

static const std::set<std::string> keywords = {
    "inline"
    , "_inline"
    , "__inline"
    , "__forceinline"
    , "register"
    , "__restrict"
    , "__restrict__"
    , "__thread"
};
// Remove "inline", "register", "restrict", "override", "final", "static" and "constexpr"
// "restrict" keyword
//   - New to 1999 ANSI/ISO C standard
//   - Not in C++ standard yet
void Tokenizer::simplifyKeyword()
{
    // FIXME: There is a risk that "keywords" are removed by mistake. This
    // code should be fixed so it doesn't remove variables etc. Nonstandard
    // keywords should be defined with a library instead. For instance the
    // linux kernel code at least uses "_inline" as struct member name at some
    // places.

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (keywords.find(tok->str()) != keywords.end()) {
            // Don't remove struct members
            if (!match597(tok->previous()))
                tok->deleteThis(); // Simplify..
        }

        if (isC() || mSettings->standards.cpp == Standards::CPP03) {
            if (tok->str() == MatchCompiler::makeConstString("auto"))
                tok->deleteThis();
        }


        if (mSettings->standards.c >= Standards::C99) {
            while (tok->str() == MatchCompiler::makeConstString("restrict")) {
                tok->deleteThis();
            }

            // simplify static keyword:
            // void foo( int [ static 5 ] ); ==> void foo( int [ 5 ] );
            if (match598(tok)) {
                tok->deleteNext();
            }
        }

        if (mSettings->standards.c >= Standards::C11) {
            while (tok->str() == MatchCompiler::makeConstString("_Atomic")) {
                tok->deleteThis();
            }
        }

        if (isCPP() && mSettings->standards.cpp >= Standards::CPP11) {
            while (tok->str() == MatchCompiler::makeConstString("constexpr")) {
                tok->deleteThis();
            }

            // final:
            // 1) struct name final { };   <- struct is final
            if (match599(tok)) {
                tok->deleteNext();
            }

            // noexcept -> noexcept(true)
            // 2) void f() noexcept; -> void f() noexcept(true);
            else if (match600(tok)) {
                // Insertion is done in inverse order
                // The brackets are linked together accordingly afterwards
                Token * tokNoExcept = tok->next();
                tokNoExcept->insertToken(")");
                Token * braceEnd = tokNoExcept->next();
                tokNoExcept->insertToken("true");
                tokNoExcept->insertToken("(");
                Token * braceStart = tokNoExcept->next();
                tok = tok->tokAt(3);
                Token::createMutualLinks(braceStart, braceEnd);
            }

            // 3) thread_local
            else if (tok->str() == MatchCompiler::makeConstString("thread_local")) {
                tok->deleteThis();
            }
        }
    }
}

void Tokenizer::simplifyAssignmentInFunctionCall()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("("))
            tok = tok->link();

        // Find 'foo(var='. Exclude 'assert(var=' to allow tests to check that assert(...) does not contain side-effects
        else if (match601(tok) &&
                 match164(tok->linkAt(2)) &&
                 !match602(tok->next())) {
            const std::string& funcname(tok->next()->str());
            Token* const vartok = tok->tokAt(3);

            // Goto ',' or ')'..
            for (Token *tok2 = vartok->tokAt(2); tok2; tok2 = tok2->next()) {
                if (tok2->link() && match271(tok2))
                    tok2 = tok2->link();
                else if (tok2->str() == MatchCompiler::makeConstString(";"))
                    break;
                else if (match603(tok2)) {
                    tok2 = tok2->previous();

                    tok2->insertToken(vartok->str());
                    tok2->next()->varId(vartok->varId());

                    tok2->insertToken("(");
                    Token::createMutualLinks(tok2->next(), tok->linkAt(2));

                    tok2->insertToken(funcname);
                    tok2->insertToken(";");

                    Token::eraseTokens(tok, vartok);
                    break;
                }
            }
        }
    }
}

void Tokenizer::simplifyAssignmentBlock()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match604(tok)) {
            const std::string &varname = tok->next()->str();

            // goto the "} )"
            unsigned int indentlevel = 0;
            Token *tok2 = tok;
            while (nullptr != (tok2 = tok2->next())) {
                if (match534(tok2))
                    ++indentlevel;
                else if (match605(tok2)) {
                    if (indentlevel <= 2)
                        break;
                    --indentlevel;
                } else if (indentlevel == 2 && tok2->str() == varname && match606(tok2->previous()))
                    // declaring variable in inner scope with same name as lhs variable
                    break;
            }
            if (indentlevel == 2 && match607(tok2)) {
                tok2 = tok2->tokAt(-3);
                if (match608(tok2)) {
                    tok2->insertToken("=");
                    tok2->insertToken(tok->next()->str());
                    tok2->next()->varId(tok->next()->varId());
                    tok->deleteNext(3);
                    tok2->tokAt(5)->deleteNext();
                }
            }
        }
    }
}

// Remove __asm..
void Tokenizer::simplifyAsm()
{
    std::string instruction;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match609(tok) &&
            tok->next()->link()->next()) {
            instruction = tok->tokAt(2)->stringifyList(tok->next()->link());
            Token::eraseTokens(tok, tok->next()->link()->next());
        }

        else if (match610(tok)) {
            // Goto "("
            Token *partok = tok->next();
            if (partok->str() != MatchCompiler::makeConstString("("))
                partok = partok->next();
            instruction = partok->next()->stringifyList(partok->link());
            Token::eraseTokens(tok, partok->link()->next());
        }

        else if (match611(tok)) {
            Token *endasm = tok->next();
            const Token *firstSemiColon = nullptr;
            unsigned int comment = 0;
            while (match612(endasm) || (endasm && endasm->linenr() == comment)) {
                if (match613(endasm))
                    break;
                if (endasm->str() == MatchCompiler::makeConstString(";")) {
                    comment = endasm->linenr();
                    if (!firstSemiColon)
                        firstSemiColon = endasm;
                }
                endasm = endasm->next();
            }
            if (match614(endasm)) {
                instruction = tok->next()->stringifyList(endasm);
                Token::eraseTokens(tok, endasm->next());
                if (!match615(tok->next()))
                    tok->insertToken(";");
            } else if (firstSemiColon) {
                instruction = tok->next()->stringifyList(firstSemiColon);
                Token::eraseTokens(tok, firstSemiColon);
            } else if (!endasm) {
                instruction = tok->next()->stringifyList(endasm);
                Token::eraseTokens(tok, endasm);
                tok->insertToken(";");
            } else
                continue;
        }

        else
            continue;

        // insert "asm ( "instruction" )"
        tok->str("asm");
        if (tok->strAt(1) != MatchCompiler::makeConstString(";") && tok->strAt(1) != MatchCompiler::makeConstString("{"))
            tok->insertToken(";");
        tok->insertToken(")");
        tok->insertToken("\"" + instruction + "\"");
        tok->insertToken("(");

        tok = tok->next();
        Token::createMutualLinks(tok, tok->tokAt(2));

        //move the new tokens in the same line as ";" if available
        tok = tok->tokAt(2);
        if (tok->next() && tok->next()->str() == MatchCompiler::makeConstString(";") &&
            tok->next()->linenr() != tok->linenr()) {
            const unsigned int endposition = tok->next()->linenr();
            tok = tok->tokAt(-3);
            for (int i = 0; i < 4; ++i) {
                tok = tok->next();
                tok->linenr(endposition);
            }
        }
    }
}

void Tokenizer::simplifyAsm2()
{
    // Block declarations: ^{}
    // A C extension used to create lambda like closures.

    // Put ^{} statements in asm()
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("^"))
            continue;

        if (match616(tok) || match19(tok->linkAt(1))) {
            Token * start = tok;
            while (start && !match617(start)) {
                if (start->link() && match618(start))
                    start = start->link();
                start = start->previous();
            }

            const Token *last = tok->next()->link();
            if (match19(last))
                last = last->linkAt(1);
            last = last->next();
            while (last && !match619(last)) {
                if (match121(last))
                    last = last->link();
                last = last->next();
            }

            if (start && last) {
                std::string asmcode;
                while (start->next() != last) {
                    asmcode += start->next()->str();
                    start->deleteNext();
                }
                if (last->str() == MatchCompiler::makeConstString("}"))
                    start->insertToken(";");
                start->insertToken(")");
                start->insertToken("\"" + asmcode + "\"");
                start->insertToken("(");
                start->insertToken("asm");
                start->tokAt(2)->link(start->tokAt(4));
                start->tokAt(4)->link(start->tokAt(2));
                tok = start->tokAt(4);
            }
        }
    }

    // When the assembly code has been cleaned up, no @ is allowed
    for (const Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("(")) {
            tok = tok->link();
            if (!tok)
                syntaxError(nullptr);
        } else if (tok->str()[0] == '@') {
            syntaxError(nullptr);
        }
    }
}

// Simplify bitfields
void Tokenizer::simplifyBitfields()
{
    bool goback = false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (goback) {
            goback = false;
            tok = tok->previous();
        }
        Token *last = nullptr;

        if (!match620(tok))
            continue;

        bool isEnum = false;
        if (tok->str() == MatchCompiler::makeConstString("}")) {
            const Token *type = tok->link()->previous();
            while (type && type->isName()) {
                if (type->str() == MatchCompiler::makeConstString("enum")) {
                    isEnum = true;
                    break;
                }
                type = type->previous();
            }
        }

        if (match621(tok->next()) &&
            !match622(tok->next()) &&
            !match623(tok->tokAt(2))) {
            Token *tok1 = (tok->next()->str() == MatchCompiler::makeConstString("const")) ? tok->tokAt(3) : tok->tokAt(2);
            if (match624(tok1))
                tok1->setBits(MathLib::toLongNumber(tok1->strAt(2)));
            if (tok1 && tok1->tokAt(2) &&
                (match625(tok1->tokAt(2)) ||
                 !match626(tok1->tokAt(2)))) {
                while (tok1->next() && !match627(tok1->next())) {
                    if (match628(tok1->next()))
                        Token::eraseTokens(tok1, tok1->next()->link());
                    tok1->deleteNext();
                }

                last = tok1->next();
            }
        } else if (isEnum && match629(tok)) {
            if (tok->next()->str() == MatchCompiler::makeConstString(":")) {
                tok->deleteNext(2);
                tok->insertToken("Anonymous");
            } else {
                tok->next()->deleteNext(2);
            }
        } else if (match630(tok->next()) &&
                   tok->next()->str() != MatchCompiler::makeConstString("default")) {
            const int offset = (tok->next()->str() == MatchCompiler::makeConstString("const")) ? 1 : 0;
            if (!match631(tok->tokAt(3 + offset))) {
                tok->deleteNext(4 + offset);
                goback = true;
            }
        }

        if (last && last->str() == MatchCompiler::makeConstString(",")) {
            Token * tok1 = last;
            tok1->str(";");

            const Token *const tok2 = tok->next();
            tok1->insertToken(tok2->str());
            tok1 = tok1->next();
            tok1->isSigned(tok2->isSigned());
            tok1->isUnsigned(tok2->isUnsigned());
            tok1->isLong(tok2->isLong());
        }
    }
}


// Types and objects in std namespace that are neither functions nor templates
static const std::set<std::string> stdTypes = {
    "string", "wstring", "u16string", "u32string",
    "iostream", "ostream", "ofstream", "ostringstream",
    "istream", "ifstream", "istringstream", "fstream", "stringstream",
    "wstringstream", "wistringstream", "wostringstream", "wstringbuf",
    "stringbuf", "streambuf", "ios", "filebuf", "ios_base",
    "exception", "bad_exception", "bad_alloc",
    "logic_error", "domain_error", "invalid_argument_", "length_error",
    "out_of_range", "runtime_error", "range_error", "overflow_error", "underflow_error",
    "locale",
    "cout", "cerr", "clog", "cin",
    "wcerr", "wcin", "wclog", "wcout",
    "endl", "ends", "flush",
    "boolalpha", "noboolalpha", "showbase", "noshowbase",
    "showpoint", "noshowpoint", "showpos", "noshowpos",
    "skipws", "noskipws", "unitbuf", "nounitbuf", "uppercase", "nouppercase",
    "dec", "hex", "oct",
    "fixed", "scientific",
    "internal", "left", "right",
    "fpos", "streamoff", "streampos", "streamsize"
};

static const std::set<std::string> stdTemplates = {
    "array", "basic_string", "bitset", "deque", "list", "map", "multimap",
    "priority_queue", "queue", "set", "multiset", "stack", "vector", "pair",
    "iterator", "iterator_traits",
    "unordered_map", "unordered_multimap", "unordered_set", "unordered_multiset",
    "tuple", "function"
};
static const std::set<std::string> stdFunctions = {
    "getline",
    "for_each", "find", "find_if", "find_end", "find_first_of",
    "adjacent_find", "count", "count_if", "mismatch", "equal", "search", "search_n",
    "copy", "copy_backward", "swap", "swap_ranges", "iter_swap", "transform", "replace",
    "replace_if", "replace_copy", "replace_copy_if", "fill", "fill_n", "generate", "generate_n", "remove",
    "remove_if", "remove_copy", "remove_copy_if",
    "unique", "unique_copy", "reverse", "reverse_copy",
    "rotate", "rotate_copy", "random_shuffle", "partition", "stable_partition",
    "sort", "stable_sort", "partial_sort", "partial_sort_copy", "nth_element",
    "lower_bound", "upper_bound", "equal_range", "binary_search", "merge", "inplace_merge", "includes",
    "set_union", "set_intersection", "set_difference",
    "set_symmetric_difference", "push_heap", "pop_heap", "make_heap", "sort_heap",
    "min", "max", "min_element", "max_element", "lexicographical_compare", "next_permutation", "prev_permutation",
    "advance", "back_inserter", "distance", "front_inserter", "inserter",
    "make_pair", "make_shared", "make_tuple"
};


// Add std:: in front of std classes, when using namespace std; was given
void Tokenizer::simplifyNamespaceStd()
{
    if (!isCPP())
        return;

    const bool isCPP11  = mSettings->standards.cpp == Standards::CPP11;

    std::set<std::string> userFunctions;

    for (const Token* tok = findmatch632(list.front()) ; tok; tok = tok->next()) {
        bool insert = false;
        if (match526(tok)) { // Don't replace within enum definitions
            skipEnumBody(&tok);
        }
        if (!match633(tok->previous())) {
            if (match6(tok)) {
                if (isFunctionHead(tok->next(), "{"))
                    userFunctions.insert(tok->str());
                else if (isFunctionHead(tok->next(), ";")) {
                    const Token *start = tok;
                    while (match401(start->previous()))
                        start = start->previous();
                    if (start != tok && start->isName() && (!start->previous() || match88(start->previous())))
                        userFunctions.insert(tok->str());
                }
                if (userFunctions.find(tok->str()) == userFunctions.end() && stdFunctions.find(tok->str()) != stdFunctions.end())
                    insert = true;
            } else if (match192(tok) && stdTemplates.find(tok->str()) != stdTemplates.end())
                insert = true;
            else if (tok->isName() && !tok->varId() && !match634(tok->next()) && stdTypes.find(tok->str()) != stdTypes.end())
                insert = true;
        }

        if (insert) {
            tok->previous()->insertToken("std");
            tok->previous()->linenr(tok->linenr()); // For stylistic reasons we put the std:: in the same line as the following token
            tok->previous()->fileIndex(tok->fileIndex());
            tok->previous()->insertToken("::");
        } else if (isCPP11 && match635(tok))
            tok->next()->str("std");
    }

    for (Token* tok = list.front(); tok; tok = tok->next()) {
        if (isCPP11 && match636(tok))
            Token::eraseTokens(tok, tok->tokAt(3));

        else if (match637(tok)) {
            Token::eraseTokens(tok, tok->tokAt(4));
            tok->deleteThis();
        }
    }
}


void Tokenizer::simplifyMicrosoftMemoryFunctions()
{
    // skip if not Windows
    if (!mSettings->isWindowsPlatform())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->strAt(1) != MatchCompiler::makeConstString("("))
            continue;

        if (match638(tok)) {
            tok->str("memcpy");
        } else if (match639(tok)) {
            tok->str("memmove");
        } else if (match640(tok)) {
            // FillMemory(dst, len, val) -> memset(dst, val, len)
            tok->str("memset");

            Token *tok1 = tok->tokAt(2);
            if (tok1)
                tok1 = tok1->nextArgument(); // Second argument
            if (tok1) {
                Token *tok2 = tok1->nextArgument(); // Third argument

                if (tok2)
                    Token::move(tok1->previous(), tok2->tokAt(-2), tok->next()->link()->previous()); // Swap third with second argument
            }
        } else if (match641(tok)) {
            // ZeroMemory(dst, len) -> memset(dst, 0, len)
            tok->str("memset");

            Token *tok1 = tok->tokAt(2);
            if (tok1)
                tok1 = tok1->nextArgument(); // Second argument

            if (tok1) {
                tok1 = tok1->previous();
                tok1->insertToken("0");
                tok1 = tok1->next();
                tok1->insertToken(",");
            }
        } else if (match642(tok)) {
            // RtlCompareMemory(src1, src2, len) -> memcmp(src1, src2, len)
            tok->str("memcmp");
            // For the record, when memcmp returns 0, both strings are equal.
            // When RtlCompareMemory returns len, both strings are equal.
            // It might be needed to improve this replacement by something
            // like ((len - memcmp(src1, src2, len)) % (len + 1)) to
            // respect execution path (if required)
        }
    }
}

namespace {
    struct triplet {
        triplet(const char* m, const char* u) :  mbcs(m), unicode(u) {}
        std::string mbcs, unicode;
    };

    const std::map<std::string, triplet> apis = {
        std::make_pair("_topen", triplet("open", "_wopen")),
        std::make_pair("_tsopen_s", triplet("_sopen_s", "_wsopen_s")),
        std::make_pair("_tfopen", triplet("fopen", "_wfopen")),
        std::make_pair("_tfopen_s", triplet("fopen_s", "_wfopen_s")),
        std::make_pair("_tfreopen", triplet("freopen", "_wfreopen")),
        std::make_pair("_tfreopen_s", triplet("freopen_s", "_wfreopen_s")),
        std::make_pair("_tcscat", triplet("strcat", "wcscat")),
        std::make_pair("_tcschr", triplet("strchr", "wcschr")),
        std::make_pair("_tcscmp", triplet("strcmp", "wcscmp")),
        std::make_pair("_tcsdup", triplet("strdup", "wcsdup")),
        std::make_pair("_tcscpy", triplet("strcpy", "wcscpy")),
        std::make_pair("_tcslen", triplet("strlen", "wcslen")),
        std::make_pair("_tcsncat", triplet("strncat", "wcsncat")),
        std::make_pair("_tcsncpy", triplet("strncpy", "wcsncpy")),
        std::make_pair("_tcsnlen", triplet("strnlen", "wcsnlen")),
        std::make_pair("_tcsrchr", triplet("strrchr", "wcsrchr")),
        std::make_pair("_tcsstr", triplet("strstr", "wcsstr")),
        std::make_pair("_tcstok", triplet("strtok", "wcstok")),
        std::make_pair("_ftprintf", triplet("fprintf", "fwprintf")),
        std::make_pair("_tprintf", triplet("printf", "wprintf")),
        std::make_pair("_stprintf", triplet("sprintf", "swprintf")),
        std::make_pair("_sntprintf", triplet("_snprintf", "_snwprintf")),
        std::make_pair("_ftscanf", triplet("fscanf", "fwscanf")),
        std::make_pair("_tscanf", triplet("scanf", "wscanf")),
        std::make_pair("_stscanf", triplet("sscanf", "swscanf")),
        std::make_pair("_ftprintf_s", triplet("fprintf_s", "fwprintf_s")),
        std::make_pair("_tprintf_s", triplet("printf_s", "wprintf_s")),
        std::make_pair("_stprintf_s", triplet("sprintf_s", "swprintf_s")),
        std::make_pair("_sntprintf_s", triplet("_snprintf_s", "_snwprintf_s")),
        std::make_pair("_ftscanf_s", triplet("fscanf_s", "fwscanf_s")),
        std::make_pair("_tscanf_s", triplet("scanf_s", "wscanf_s")),
        std::make_pair("_stscanf_s", triplet("sscanf_s", "swscanf_s"))
    };
}

void Tokenizer::simplifyMicrosoftStringFunctions()
{
    // skip if not Windows
    if (!mSettings->isWindowsPlatform())
        return;

    const bool ansi = mSettings->platformType == Settings::Win32A;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->strAt(1) != MatchCompiler::makeConstString("("))
            continue;

        const std::map<std::string, triplet>::const_iterator match = apis.find(tok->str());
        if (match!=apis.end()) {
            tok->str(ansi ? match->second.mbcs : match->second.unicode);
            tok->originalName(match->first);
        } else if (match643(tok)) {
            tok->deleteNext();
            tok->deleteThis();
            tok->deleteNext();
            if (!ansi)
                tok->isLong(true);
            while (match643(tok->next())) {
                tok->next()->deleteNext();
                tok->next()->deleteThis();
                tok->next()->deleteNext();
                tok->concatStr(tok->next()->str());
                tok->deleteNext();
            }
        }
    }
}

// Remove Borland code
void Tokenizer::simplifyBorland()
{
    // skip if not Windows
    if (!mSettings->isWindowsPlatform())
        return;
    if (isC())
        return;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match644(tok)) {
            tok->deleteNext();
        }
    }

    // I think that these classes are always declared at the outer scope
    // I save some time by ignoring inner classes.
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("{") && !match645(tok->tokAt(-2))) {
            tok = tok->link();
            if (!tok)
                break;
        } else if (match646(tok)) {
            while (tok && tok->str() != MatchCompiler::makeConstString("{") && tok->str() != MatchCompiler::makeConstString(";"))
                tok = tok->next();
            if (!tok)
                break;
            if (tok->str() == MatchCompiler::makeConstString(";"))
                continue;

            const Token* end = tok->link()->next();
            for (Token *tok2 = tok->next(); tok2 != end; tok2 = tok2->next()) {
                if (tok2->str() == MatchCompiler::makeConstString("__property") &&
                    match647(tok2->previous())) {
                    while (tok2->next() && !match648(tok2->next()))
                        tok2->deleteNext();
                    tok2->deleteThis();
                    if (tok2->str() == MatchCompiler::makeConstString("{")) {
                        Token::eraseTokens(tok2, tok2->link());
                        tok2->deleteNext();
                        tok2->deleteThis();

                        // insert "; __property ;"
                        tok2->previous()->insertToken(";");
                        tok2->previous()->insertToken("__property");
                        tok2->previous()->insertToken(";");
                    }
                }
            }
        }
    }
}

// Remove Qt signals and slots
void Tokenizer::simplifyQtSignalsSlots()
{
    if (isC())
        return;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        // check for emit which can be outside of class
        if (match649(tok) &&
            match164(tok->linkAt(2))) {
            tok->deleteThis();
        } else if (!match650(tok))
            continue;

        if (tok->previous() && tok->previous()->str() == MatchCompiler::makeConstString("enum")) {
            tok = tok->tokAt(2);
            continue;
        }

        // count { and } for tok2
        unsigned int indentlevel = 0;
        for (Token *tok2 = tok; tok2; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("{")) {
                ++indentlevel;
                if (indentlevel == 1)
                    tok = tok2;
                else
                    tok2 = tok2->link();
            } else if (tok2->str() == MatchCompiler::makeConstString("}")) {
                if (indentlevel<2)
                    break;
                else
                    --indentlevel;
            }
            if (tok2->strAt(1) == MatchCompiler::makeConstString("Q_OBJECT"))
                tok2->deleteNext();

            if (match651(tok2->next())) {
                tok2 = tok2->next();
                tok2->str(tok2->str() + ":");
                tok2->deleteNext(2);
                tok2 = tok2->previous();
            } else if (match652(tok2->next())) {
                tok2 = tok2->next();
                tok2->str("protected:");
                tok2->deleteNext();
            } else if (match649(tok2->next()) &&
                       match164(tok2->linkAt(3))) {
                tok2->deleteNext();
            }
        }
    }
}

void Tokenizer::createSymbolDatabase()
{
    if (!mSymbolDatabase)
        mSymbolDatabase = new SymbolDatabase(this, mSettings, mErrorLogger);
    mSymbolDatabase->validate();
}

void Tokenizer::deleteSymbolDatabase()
{
    delete mSymbolDatabase;
    mSymbolDatabase = nullptr;
}

static bool operatorEnd(const Token * tok)
{
    if (tok && tok->str() == MatchCompiler::makeConstString(")")) {
        tok = tok->next();
        while (tok && !match653(tok)) {
            if (match50(tok)) {
                tok = tok->next();
            } else if (tok->str() == MatchCompiler::makeConstString("noexcept")) {
                tok = tok->next();
                if (tok && tok->str() == MatchCompiler::makeConstString("(")) {
                    tok = tok->link()->next();
                }
            } else if (tok->str() == MatchCompiler::makeConstString("throw") && tok->next() && tok->next()->str() == MatchCompiler::makeConstString("(")) {
                tok = tok->next()->link()->next();
            }
            // unknown macros ") MACRO {" and ") MACRO(...) {"
            else if (tok->isUpperCaseName()) {
                tok = tok->next();
                if (tok && tok->str() == MatchCompiler::makeConstString("(")) {
                    tok = tok->link()->next();
                }
            } else
                return false;
        }

        return true;
    }

    return false;
}

void Tokenizer::simplifyOperatorName()
{
    if (isC())
        return;

    bool isUsingStmt = false;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString(";")) {
            if (isUsingStmt && match654(tok->tokAt(-3))) {
                tok->previous()->previous()->str("operator" + tok->previous()->str());
                tok->deletePrevious();
            }
            isUsingStmt = false;
            continue;
        }
        if (tok->str() == MatchCompiler::makeConstString("using")) {
            isUsingStmt = true;
            continue;
        }

        if (tok->str() != MatchCompiler::makeConstString("operator"))
            continue;
        // operator op
        std::string op;
        Token *par = tok->next();
        bool done = false;
        while (!done && par) {
            done = true;
            if (par->isName()) {
                op += par->str();
                par = par->next();
                // merge namespaces eg. 'operator std :: string () const {'
                if (match655(par)) {
                    op += par->str();
                    par = par->next();
                }
                done = false;
            } else if (match656(par)) {
                op += par->str();
                par = par->next();
                done = false;
            } else if (match657(par)) {
                op += "[]";
                par = par->tokAt(2);
                done = false;
            } else if (match658(par)) {
                // break out and simplify..
                if (operatorEnd(par->next()))
                    break;

                while (par->str() != MatchCompiler::makeConstString(")")) {
                    op += par->str();
                    par = par->next();
                }
                op += ")";
                par = par->next();
                done = false;
            }
        }

        if (par && operatorEnd(par->link())) {
            tok->str("operator" + op);
            Token::eraseTokens(tok, par);
        }

        if (!op.empty())
            tok->isOperatorKeyword(true);
    }

    if (mSettings->debugwarnings) {
        const Token *tok = list.front();

        while ((tok = findmatch659(tok) ) != nullptr) {
            reportError(tok, Severity::debug, "debug",
                        "simplifyOperatorName: found unsimplified operator name");
            tok = tok->next();
        }
    }
}

// remove unnecessary member qualification..
void Tokenizer::removeUnnecessaryQualification()
{
    if (isC())
        return;

    std::vector<Space> classInfo;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match660(tok) &&
            (!tok->previous() || tok->previous()->str() != MatchCompiler::makeConstString("enum"))) {
            Space info;
            info.isNamespace = tok->str() == MatchCompiler::makeConstString("namespace");
            tok = tok->next();
            info.className = tok->str();
            tok = tok->next();
            while (tok && tok->str() != MatchCompiler::makeConstString("{"))
                tok = tok->next();
            if (!tok)
                return;
            info.bodyEnd = tok->link();
            classInfo.push_back(info);
        } else if (!classInfo.empty()) {
            if (tok == classInfo.back().bodyEnd)
                classInfo.pop_back();
            else if (tok->str() == classInfo.back().className &&
                     !classInfo.back().isNamespace && tok->previous()->str() != MatchCompiler::makeConstString(":") &&
                     (match661(tok) ||
                      match662(tok))) {
                const Token *tok1 = tok->tokAt(3);
                if (tok->strAt(2) == MatchCompiler::makeConstString("operator")) {
                    // check for operator ()
                    if (tok1->str() == MatchCompiler::makeConstString("("))
                        tok1 = tok1->next();

                    while (tok1 && tok1->str() != MatchCompiler::makeConstString("(")) {
                        if (tok1->str() == MatchCompiler::makeConstString(";"))
                            break;
                        tok1 = tok1->next();
                    }
                    if (!tok1 || tok1->str() != MatchCompiler::makeConstString("("))
                        continue;
                } else if (tok->strAt(2) == MatchCompiler::makeConstString("~"))
                    tok1 = tok1->next();

                if (!tok1 || !match663(tok1->link())) {
                    continue;
                }

                const bool isConstructorOrDestructor =
                    match664(tok) && (tok->strAt(2) == tok->str() || (tok->strAt(2) == MatchCompiler::makeConstString("~") && tok->strAt(3) == tok->str()));
                if (!isConstructorOrDestructor) {
                    bool isPrependedByType = match17(tok->previous());
                    if (!isPrependedByType) {
                        const Token* tok2 = tok->tokAt(-2);
                        isPrependedByType = match73(tok2);
                    }
                    if (!isPrependedByType) {
                        const Token* tok3 = tok->tokAt(-3);
                        isPrependedByType = match665(tok3);
                    }
                    if (!isPrependedByType) {
                        // It's not a constructor declaration and it's not a function declaration so
                        // this is a function call which can have all the qualifiers just fine - skip.
                        continue;
                    }
                }
            }
        }
    }
}

void Tokenizer::simplifyReturnStrncat()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match666(tok) &&
            match164(tok->linkAt(2)) &&
            tok->strAt(3) != MatchCompiler::makeConstString(")") && tok->strAt(3) != MatchCompiler::makeConstString(",")) {

            //first argument
            Token *tok2 = tok->tokAt(3);

            //check if there are at least three arguments
            for (unsigned char i = 0; i < 2; ++i) {
                tok2 = tok2->nextArgument();
                if (!tok2) {
                    tok = tok->linkAt(2)->next();
                    break;
                }
            }
            if (!tok2)
                continue;

            tok2 = tok2->nextArgument();
            //we want only three arguments
            if (tok2) {
                tok = tok->linkAt(2)->next();
                continue;
            }

            // Remove 'return'
            tok->deleteThis();

            // Add 'return arg1 ;' after 'strncat(arg1, arg2, arg3);'
            tok = tok->next();

            tok2 = tok->link()->next();
            tok2->insertToken(";");

            //the last token of the first argument before ','
            const Token * const end = tok->next()->nextArgument()->tokAt(-2);

            //all the first argument is copied
            TokenList::copyTokens(tok2, tok->next(), end);
            tok2->insertToken("return");
        }
    }
}

void Tokenizer::printUnknownTypes() const
{
    if (!mSymbolDatabase)
        return;

    std::multimap<std::string, const Token *> unknowns;

    for (unsigned int i = 1; i <= mVarId; ++i) {
        const Variable *var = mSymbolDatabase->getVariableFromVarId(i);
        if (!var)
            continue;
        // is unknown type?
        if (var->type() || var->typeStartToken()->isStandardType())
            continue;

        std::string name;
        const Token * nameTok;

        // single token type?
        if (var->typeStartToken() == var->typeEndToken()) {
            nameTok = var->typeStartToken();
            name = nameTok->str();
        }

        // complicated type
        else {
            const Token *tok = var->typeStartToken();
            int level = 0;

            nameTok =  tok;

            while (tok) {
                // skip pointer and reference part of type
                if (level == 0 && match37(tok))
                    break;

                name += tok->str();

                if (match130(tok))
                    name += " ";

                // pointers and references are OK in template
                else if (tok->str() == MatchCompiler::makeConstString("<"))
                    ++level;
                else if (tok->str() == MatchCompiler::makeConstString(">"))
                    --level;

                if (tok == var->typeEndToken())
                    break;

                tok = tok->next();
            }
        }

        unknowns.insert(std::pair<std::string, const Token *>(name, nameTok));
    }

    if (!unknowns.empty()) {
        std::string last;
        size_t count = 0;

        for (std::multimap<std::string, const Token *>::const_iterator it = unknowns.begin(); it != unknowns.end(); ++it) {
            // skip types is std namespace because they are not interesting
            if (it->first.find("std::") != 0) {
                if (it->first != last) {
                    last = it->first;
                    count = 1;
                    reportError(it->second, Severity::debug, "debug", "Unknown type \'" + it->first + "\'.");
                } else {
                    if (count < 3) // limit same type to 3
                        reportError(it->second, Severity::debug, "debug", "Unknown type \'" + it->first + "\'.");
                    count++;
                }
            }
        }
    }
}

void Tokenizer::simplifyMathExpressions()
{
    for (Token *tok = list.front(); tok; tok = tok->next()) {

        //simplify Pythagorean trigonometric identity: pow(sin(x),2)+pow(cos(x),2) = 1
        //                                             pow(cos(x),2)+pow(sin(x),2) = 1
        // @todo: sin(x) * sin(x) + cos(x) * cos(x) = 1
        //        cos(x) * cos(x) + sin(x) * sin(x) = 1
        //simplify Hyperbolic identity: pow(sinh(x),2)-pow(cosh(x),2) = -1
        //                              pow(cosh(x),2)-pow(sinh(x),2) = -1
        // @todo: sinh(x) * sinh(x) - cosh(x) * cosh(x) = -1
        //        cosh(x) * cosh(x) - sinh(x) * sinh(x) = -1
        if (match523(tok)) {
            if (match667(tok->tokAt(2))) {
                Token * const tok2 = tok->linkAt(3);
                if (!match668(tok2))
                    continue;
                const std::string& leftExponent = tok2->strAt(2);
                if (!isTwoNumber(leftExponent))
                    continue; // left exponent is not 2
                const Token * const tok3 = tok2->tokAt(8);
                Token * const tok4 = tok3->link();
                if (!match669(tok4))
                    continue;
                const std::string& rightExponent = tok4->strAt(2);
                if (!isTwoNumber(rightExponent))
                    continue; // right exponent is not 2
                if (tok->tokAt(3)->stringifyList(tok2->next()) == tok3->stringifyList(tok4->next())) {
                    Token::eraseTokens(tok, tok4->tokAt(4));
                    tok->str("1");
                }
            } else if (match670(tok->tokAt(2))) {
                Token * const tok2 = tok->linkAt(3);
                if (!match671(tok2))
                    continue;
                const std::string& leftExponent = tok2->strAt(2);
                if (!isTwoNumber(leftExponent))
                    continue; // left exponent is not 2
                const Token * const tok3 = tok2->tokAt(8);
                Token * const tok4 = tok3->link();
                if (!match669(tok4))
                    continue;
                const std::string& rightExponent = tok4->strAt(2);
                if (!isTwoNumber(rightExponent))
                    continue; // right exponent is not 2
                if (tok->tokAt(3)->stringifyList(tok2->next()) == tok3->stringifyList(tok4->next())) {
                    Token::eraseTokens(tok, tok4->tokAt(4));
                    tok->str("1");
                }
            } else if (match672(tok->tokAt(2))) {
                Token * const tok2 = tok->linkAt(3);
                if (!match673(tok2))
                    continue;
                const std::string& leftExponent = tok2->strAt(2);
                if (!isTwoNumber(leftExponent))
                    continue; // left exponent is not 2
                const Token * const tok3 = tok2->tokAt(8);
                Token * const tok4 = tok3->link();
                if (!match669(tok4))
                    continue;
                const std::string& rightExponent = tok4->strAt(2);
                if (!isTwoNumber(rightExponent))
                    continue; // right exponent is not 2
                if (tok->tokAt(3)->stringifyList(tok2->next()) == tok3->stringifyList(tok4->next())) {
                    Token::eraseTokens(tok, tok4->tokAt(4));
                    tok->str("-1");
                }
            } else if (match674(tok->tokAt(2))) {
                Token * const tok2 = tok->linkAt(3);
                if (!match675(tok2))
                    continue;
                const std::string& leftExponent = tok2->strAt(2);
                if (!isTwoNumber(leftExponent))
                    continue; // left exponent is not 2
                const Token * const tok3 = tok2->tokAt(8);
                Token * const tok4 = tok3->link();
                if (!match669(tok4))
                    continue;
                const std::string& rightExponent = tok4->strAt(2);
                if (!isTwoNumber(rightExponent))
                    continue; // right exponent is not 2
                if (tok->tokAt(3)->stringifyList(tok2->next()) == tok3->stringifyList(tok4->next())) {
                    Token::eraseTokens(tok, tok4->tokAt(4));
                    tok->str("-1");
                }
            }
        }
    }
}

bool Tokenizer::simplifyStrlen()
{
    // replace strlen(str)
    bool modified=false;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match251(tok)) {
            tok->str(MathLib::toString(Token::getStrLength(tok->tokAt(2))));
            tok->deleteNext(3);
            modified=true;
        }
    }
    return modified;
}

void Tokenizer::prepareTernaryOpForAST()
{
    // http://en.cppreference.com/w/cpp/language/operator_precedence says about ternary operator:
    //       "The expression in the middle of the conditional operator (between ? and :) is parsed as if parenthesized: its precedence relative to ?: is ignored."
    // The AST parser relies on this function to add such parentheses where necessary.
    for (Token* tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("?")) {
            bool parenthesesNeeded = false;
            unsigned int depth = 0;
            Token* tok2 = tok->next();
            for (; tok2; tok2 = tok2->next()) {
                if (tok2->link() && match676(tok2))
                    tok2 = tok2->link();
                else if (tok2->str() == MatchCompiler::makeConstString(":")) {
                    if (depth == 0)
                        break;
                    depth--;
                } else if (tok2->str() == MatchCompiler::makeConstString(";") || (tok2->link() && tok2->str() != MatchCompiler::makeConstString("{") && tok2->str() != MatchCompiler::makeConstString("}")))
                    break;
                else if (tok2->str() == MatchCompiler::makeConstString(","))
                    parenthesesNeeded = true;
                else if (tok2->str() == MatchCompiler::makeConstString("<"))
                    parenthesesNeeded = true;
                else if (tok2->str() == MatchCompiler::makeConstString("?")) {
                    depth++;
                    parenthesesNeeded = true;
                }
            }
            if (parenthesesNeeded && tok2 && tok2->str() == MatchCompiler::makeConstString(":")) {
                tok->insertToken("(");
                tok2->insertToken(")", emptyString, true);
                Token::createMutualLinks(tok->next(), tok2->previous());
            }
        }
    }
}

void Tokenizer::reportError(const Token* tok, const Severity::SeverityType severity, const std::string& id, const std::string& msg, bool inconclusive) const
{
    const std::list<const Token*> callstack(1, tok);
    reportError(callstack, severity, id, msg, inconclusive);
}

void Tokenizer::reportError(const std::list<const Token*>& callstack, Severity::SeverityType severity, const std::string& id, const std::string& msg, bool inconclusive) const
{
    const ErrorLogger::ErrorMessage errmsg(callstack, &list, severity, id, msg, inconclusive);
    if (mErrorLogger)
        mErrorLogger->reportErr(errmsg);
    else
        Check::reportError(errmsg);
}

void Tokenizer::setPodTypes()
{
    if (!mSettings)
        return;
    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (!tok->isName())
            continue;

        // pod type
        const struct Library::PodType *podType = mSettings->library.podtype(tok->str());
        if (podType) {
            const Token *prev = tok->previous();
            while (prev && prev->isName())
                prev = prev->previous();
            if (prev && !match677(prev))
                continue;
            tok->isStandardType(true);
        }
    }
}

void Tokenizer::SimplifyNamelessRValueReferences()
{
    // Simplify nameless rValue references - named ones are simplified later
    for (Token* tok = list.front(); tok; tok = tok->next()) {
        if (match678(tok)) {
            tok->str("&");
            tok->insertToken("&");
        }
    }
}

const Token *Tokenizer::findSQLBlockEnd(const Token *tokSQLStart)
{
    const Token *tokLastEnd = nullptr;
    for (const Token *tok = tokSQLStart->tokAt(2); tok != nullptr; tok = tok->next()) {
        if (tokLastEnd == nullptr && tok->str() == MatchCompiler::makeConstString(";"))
            tokLastEnd = tok;
        else if (tok->str() == MatchCompiler::makeConstString("__CPPCHECK_EMBEDDED_SQL_EXEC__")) {
            if (match679(tok->tokAt(-2)))
                return tok->next();
            return tokLastEnd;
        } else if (match680(tok))
            break; // We are obviously outside the SQL block
    }

    return tokLastEnd;
}

void Tokenizer::simplifyNestedNamespace()
{
    if (!isCPP())
        return;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (match681(tok) && tok->strAt(-1) != MatchCompiler::makeConstString("using")) {
            Token * tok2 = tok->tokAt(2);

            // validate syntax
            while (match194(tok2))
                tok2 = tok2->tokAt(2);

            if (!tok2 || tok2->str() != MatchCompiler::makeConstString("{"))
                return; // syntax error

            std::stack<Token *> links;
            tok2 = tok->tokAt(2);

            while (tok2->str() == MatchCompiler::makeConstString("::")) {
                links.push(tok2);
                tok2->str("{");
                tok2->insertToken("namespace");
                tok2 = tok2->tokAt(3);
            }

            tok = tok2;

            if (!links.empty() && tok2->str() == MatchCompiler::makeConstString("{")) {
                tok2 = tok2->link();
                while (!links.empty()) {
                    tok2->insertToken("}");
                    tok2 = tok2->next();
                    Token::createMutualLinks(links.top(), tok2);
                    links.pop();
                }
            }
        }
    }
}

static bool sameTokens(const Token *first, const Token *last, const Token *other)
{
    while (other && first->str() == other->str()) {
        if (first == last)
            return true;
        first = first->next();
        other = other->next();
    }

    return false;
}

static Token * deleteAlias(Token * tok)
{
    Token::eraseTokens(tok, findmatch429(tok) );

    // delete first token
    tok->deleteThis();

    // delete ';' if not last token
    tok->deleteThis();

    return tok;
}

void Tokenizer::simplifyNamespaceAliases()
{
    if (!isCPP())
        return;

    int scope = 0;

    for (Token *tok = list.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("{"))
            scope++;
        else if (tok->str() == MatchCompiler::makeConstString("}"))
            scope--;
        else if (match682(tok)) {
            const std::string name(tok->next()->str());
            Token * tokNameStart = tok->tokAt(3);
            Token * tokNameEnd = tokNameStart;

            while (tokNameEnd && tokNameEnd->next() && tokNameEnd->next()->str() != MatchCompiler::makeConstString(";"))
                tokNameEnd = tokNameEnd->next();

            if (!tokNameEnd)
                return; // syntax error

            int endScope = scope;
            Token * tokLast = tokNameEnd->next();
            Token * tokNext = tokLast->next();
            Token * tok2 = tokNext;

            while (tok2 && endScope >= scope) {
                if (match197(tok2))
                    endScope++;
                else if (match284(tok2))
                    endScope--;
                else if (tok2->str() == name) {
                    if (match682(tok2->previous())) {
                        // check for possible duplicate aliases
                        if (sameTokens(tokNameStart, tokNameEnd, tok2->tokAt(2))) {
                            // delete duplicate
                            tok2 = deleteAlias(tok2->previous());
                            continue;
                        } else {
                            // conflicting declaration (syntax error)
                            if (endScope == scope) {
                                // delete conflicting declaration
                                tok2 = deleteAlias(tok2->previous());
                            }

                            // new declaration
                            else {
                                // TODO: use the new alias in this scope
                                tok2 = deleteAlias(tok2->previous());
                            }
                            continue;
                        }
                    }

                    tok2->str(tokNameStart->str());
                    Token * tok3 = tokNameStart;
                    while (tok3 != tokNameEnd) {
                        tok2->insertToken(tok3->next()->str());
                        tok2 = tok2->next();
                        tok3 = tok3->next();
                    }
                }
                tok2 = tok2->next();
            }

            if (tok->previous() && tokNext) {
                Token::eraseTokens(tok->previous(), tokNext);
                tok = tokNext->previous();
            } else if (tok->previous()) {
                Token::eraseTokens(tok->previous(), tokLast);
                tok = tokLast;
            } else if (tokNext) {
                Token::eraseTokens(tok, tokNext);
                tok->deleteThis();
            } else {
                Token::eraseTokens(tok, tokLast);
                tok->deleteThis();
            }
        }
    }
}


Tokenizer::VariableMap::VariableMap() : mVarId(0) {}

void Tokenizer::VariableMap::enterScope()
{
    mScopeInfo.push(std::list<std::pair<std::string, unsigned int>>());
}

bool Tokenizer::VariableMap::leaveScope()
{
    if (mScopeInfo.empty())
        return false;

    for (const std::pair<std::string, unsigned int> &outerVariable : mScopeInfo.top()) {
        if (outerVariable.second != 0)
            mVariableId[outerVariable.first] = outerVariable.second;
        else
            mVariableId.erase(outerVariable.first);
    }
    mScopeInfo.pop();
    return true;
}

void Tokenizer::VariableMap::addVariable(const std::string &varname)
{
    if (mScopeInfo.empty()) {
        mVariableId[varname] = ++mVarId;
        return;
    }
    std::map<std::string, unsigned int>::iterator it = mVariableId.find(varname);
    if (it == mVariableId.end()) {
        mScopeInfo.top().push_back(std::pair<std::string, unsigned int>(varname, 0));
        mVariableId[varname] = ++mVarId;
        return;
    }
    mScopeInfo.top().push_back(std::pair<std::string, unsigned int>(varname, it->second));
    it->second = ++mVarId;
}

bool Tokenizer::VariableMap::hasVariable(const std::string &varname) const
{
    return mVariableId.find(varname) != mVariableId.end();
}
