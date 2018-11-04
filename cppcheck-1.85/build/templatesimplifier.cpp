#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: template < > %name%
static bool match1(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("template")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name%|::|<|>|,
static bool match2(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: %type% <
static bool match3(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: %type%|%num% ,
static bool match4(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || tok->isNumber()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    return true;
}
// pattern: %type%|%num% > (
static bool match5(const Token* tok) {
    if (!tok || !((tok->isName() && tok->varId()==0U && !tok->isKeyword()) || tok->isNumber()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( {
static bool match6(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: try {
static bool match7(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("try")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: } catch (
static bool match8(const Token* tok) {
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
// pattern: ) {
static bool match9(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: [;{}]
static bool match10(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: ;
static bool match11(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: typedef|typename
static bool match12(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("typedef")) || (tok->str()==MatchCompiler::makeConstString("typename"))))
        return false;
    return true;
}
// pattern: %type% ::
static bool match13(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: operator <
static bool match14(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("operator")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: < typename
static bool match15(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("typename")))
        return false;
    return true;
}
// pattern: <|, %type% <
static bool match16(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: < %type%
static bool match17(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    return true;
}
// pattern: const|volatile
static bool match18(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("volatile"))))
        return false;
    return true;
}
// pattern: struct|union
static bool match19(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    return true;
}
// pattern: & ::| %name%
static bool match20(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("::"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %type% . . .
static bool match21(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
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
    return true;
}
// pattern: =|?|:
static bool match22(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: %name% ::
static bool match23(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: *|&|&&|const
static bool match24(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("const"))))
        return false;
    return true;
}
// pattern: (|[
static bool match25(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: >|>>
static bool match26(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    return true;
}
// pattern: (
static bool match27(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: template <
static bool match28(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("template")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > %type% (
static bool match29(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: > class|struct|union %name% [,)]
static bool match30(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: template < >
static bool match31(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("template")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    return true;
}
// pattern: > (|{|:
static bool match32(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(":"))))
        return false;
    return true;
}
// pattern: <
template<class T> static T * findmatch33(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: namespace|class|struct|union %name% {|:|::
static bool match34(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("namespace")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: %name% :: %name%
static bool match35(const Token* tok) {
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
// pattern: }|namespace|class|struct|union
static bool match36(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("namespace")) || (tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    return true;
}
// pattern: %name%|.|,|=|>
static bool match37(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("=")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: {|;
template<class T> static T * findmatch38(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(";"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: [({};=] %name% ::|<
static bool match39(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("({};=", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: %type% %name% ::|<
static bool match40(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: [,:] private|protected|public %name% ::|<
static bool match41(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",:", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("private")) || (tok->str()==MatchCompiler::makeConstString("protected")) || (tok->str()==MatchCompiler::makeConstString("public"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: %name% <
static bool match42(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: , %name% <
static bool match43(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > class|struct|union %name%
static bool match44(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: = !!>
static bool match45(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(">"))
        return false;
    return true;
}
// pattern: ,|>
static bool match46(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: )|]
static bool match47(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("]"))))
        return false;
    return true;
}
// pattern: ;|)|}|]
static bool match48(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString("]"))))
        return false;
    return true;
}
// pattern: (|{|[
static bool match49(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: > using %name% = %name% ::|<
static bool match50(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("using")))
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
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: %name%|<|>|>>|,
static bool match51(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString(","))))
        return false;
    return true;
}
// pattern: [;{}] template <
static bool match52(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("template")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: [,>;{}]
static bool match53(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(",>;{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: (|<|[
static bool match54(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: <|,|(|:: %name% <
static bool match55(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: ;|{
static bool match56(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("{"))))
        return false;
    return true;
}
// pattern: :: ~| %name% (
static bool match57(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("~"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: > class|struct|union %type% {|:|<
static bool match58(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("{")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: > %type% *|&| %type% (
static bool match59(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: > %type% %type% *|&| %type% (
static bool match60(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: > %type% *|&| %type% :: %type% (
static bool match61(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
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
// pattern: > %type% %type% *|&| %type% :: %type% (
static bool match62(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
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
// pattern: *|&
static bool match63(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: . . .
static bool match64(const Token* tok) {
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
// pattern: <
static bool match65(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: &
static bool match66(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: }|;|(|[|]|)|,|?|:|%oror%|return|throw|case
static bool match67(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("}")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("?")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        return false;
    return true;
}
// pattern: %comp%|<<|>>
static bool match68(const Token* tok) {
    if (!tok || !(tok->isComparisonOp() || (tok->str()==MatchCompiler::makeConstString("<<")) || (tok->str()==MatchCompiler::makeConstString(">>"))))
        return false;
    return true;
}
// pattern: +|-
static bool match69(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("-"))))
        return false;
    return true;
}
// pattern: [*/%]
static bool match70(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("*/%", tok->str()[0]))
        return false;
    return true;
}
// pattern: * %num% /
static bool match71(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("/")))
        return false;
    return true;
}
// pattern: [/%]
static bool match72(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("/%", tok->str()[0]))
        return false;
    return true;
}
// pattern: [+-]
static bool match73(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("+-", tok->str()[0]))
        return false;
    return true;
}
// pattern: >>|<<
static bool match74(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString("<<"))))
        return false;
    return true;
}
// pattern: [/%] 0
static bool match75(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("/%", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    return true;
}
// pattern: >>|<<|&|^|%or%
static bool match76(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString("<<")) || (tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("^")) || (tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") )))
        return false;
    return true;
}
// pattern: %oror%|&&
static bool match77(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    return true;
}
// pattern: - %num% - %num%
static bool match78(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: - %num% + %num%
static bool match79(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("-")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("+")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: ( %name% ) ;|)|,|]
static bool match80(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("]"))))
        return false;
    return true;
}
// pattern: ( %name% ) %cop%
static bool match81(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isConstOp())
        return false;
    return true;
}
// pattern: [*&+-~]
static bool match82(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("*&+-~", tok->str()[0]))
        return false;
    return true;
}
// pattern: (|&&|%oror% %char% %comp% %num% &&|%oror%|)
static bool match83(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eChar))
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: [(=,] 0 &&
static bool match84(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(=,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    return true;
}
// pattern: [(=,] 1 %oror%
static bool match85(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(=,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    return true;
}
// pattern: [+-] 0 %cop%|;
static bool match86(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("+-", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: %or% 0 %cop%|;
static bool match87(const Token* tok) {
    if (!tok || !(tok->tokType() == Token::eBitOp && tok->str()==MatchCompiler::makeConstString("|") ))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString(";"))))
        return false;
    return true;
}
// pattern: [;{}] %name% = %name% [+-|] 0 ;
static bool match88(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
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
    if (!tok || tok->str().size()!=1U || !strchr("+-|", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [=([,] 0 [+|]
static bool match89(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=([,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("+|", tok->str()[0]))
        return false;
    return true;
}
// pattern: return|case 0 [+|]
static bool match90(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("+|", tok->str()[0]))
        return false;
    return true;
}
// pattern: [=[(,] 0 * %name%|%num% ,|]|)|;|=|%cop%
static bool match91(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=[(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || tok->isNumber()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: [=[(,] 0 * (
static bool match92(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=[(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: return|case 0 *|&& %name%|%num% ,|:|;|=|%cop%
static bool match93(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isName() || tok->isNumber()))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: return|case 0 *|&& (
static bool match94(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [=[(,] 0 && *|& %any% ,|]|)|;|=|%cop%
static bool match95(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=[(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: return|case 0 && *|& %any% ,|:|;|=|%cop%
static bool match96(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: [=[(,] 1 %oror% %any% ,|]|)|;|=|%cop%
static bool match97(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=[(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: return|case 1 %oror% %any% ,|:|;|=|%cop%
static bool match98(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: [=[(,] 1 %oror% *|& %any% ,|]|)|;|=|%cop%
static bool match99(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("=[(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: return|case 1 %oror% *|& %any% ,|:|;|=|%cop%
static bool match100(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("case"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(":")) || (tok->str()==MatchCompiler::makeConstString(";")) || (tok->str()==MatchCompiler::makeConstString("=")) || tok->isConstOp()))
        return false;
    return true;
}
// pattern: %any% * 1
static bool match101(const Token* tok) {
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    return true;
}
// pattern: %any% 1 *
static bool match102(const Token* tok) {
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("1")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: %op%|< ( %num% )
static bool match103(const Token* tok) {
    if (!tok || !(tok->isOp() || (tok->str()==MatchCompiler::makeConstString("<"))))
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
// pattern: ( 0 [|+]
static bool match104(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("|+", tok->str()[0]))
        return false;
    return true;
}
// pattern: [|+-] 0 )
static bool match105(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("|+-", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: [|+-]
static bool match106(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("|+-", tok->str()[0]))
        return false;
    return true;
}
// pattern: %num% %comp% %num%
static bool match107(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !tok->isComparisonOp())
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: (|&&|%oror%
static bool match108(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    return true;
}
// pattern: )|&&|%oror%|?
static bool match109(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("?"))))
        return false;
    return true;
}
// pattern: %name% ,|>
static bool match110(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: %name% !!<
static bool match111(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("<"))
        return false;
    return true;
}
// pattern: [;{}=]
static bool match112(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}=", tok->str()[0]))
        return false;
    return true;
}
// pattern: <|,|:: %name% <
static bool match113(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > [,>]
static bool match114(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",>", tok->str()[0]))
        return false;
    return true;
}
// pattern: [<,]
static bool match115(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("<,", tok->str()[0]))
        return false;
    return true;
}
// pattern: class|struct|union|enum
static bool match116(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("class")) || (tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union")) || (tok->str()==MatchCompiler::makeConstString("enum"))))
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

#include "templatesimplifier.h"

#include "errorlogger.h"
#include "mathlib.h"
#include "settings.h"
#include "token.h"
#include "tokenize.h"
#include "tokenlist.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stack>
#include <utility>

namespace {
    class FindToken {
    public:
        explicit FindToken(const Token *token) : mToken(token) {}
        bool operator()(const TemplateSimplifier::TokenAndName &tokenAndName) const {
            return tokenAndName.token == mToken;
        }
    private:
        const Token * const mToken;
    };

    class FindName {
    public:
        explicit FindName(const std::string &name) : mName(name) {}
        bool operator()(const TemplateSimplifier::TokenAndName &tokenAndName) const {
            return tokenAndName.name == mName;
        }
    private:
        const std::string mName;
    };
}

TemplateSimplifier::TokenAndName::TokenAndName(Token *tok, const std::string &s, const std::string &n) :
    token(tok), scope(s), name(n)
{
    token->hasTemplateSimplifierPointer(true);
}

TemplateSimplifier::TemplateSimplifier(TokenList &tokenlist, const Settings *settings, ErrorLogger *errorLogger)
    : mTokenList(tokenlist), mSettings(settings), mErrorLogger(errorLogger)
{
}

TemplateSimplifier::~TemplateSimplifier()
{
}

void TemplateSimplifier::cleanupAfterSimplify()
{
    bool goback = false;
    for (Token *tok = mTokenList.front(); tok; tok = tok->next()) {
        if (goback) {
            tok = tok->previous();
            goback = false;
        }
        if (tok->str() == MatchCompiler::makeConstString("("))
            tok = tok->link();

        else if (match1(tok)) {
            const Token *end = tok;
            while (end) {
                if (end->str() == MatchCompiler::makeConstString(";"))
                    break;
                if (end->str() == MatchCompiler::makeConstString("{")) {
                    end = end->link()->next();
                    break;
                }
                if (!match2(end)) {
                    end = nullptr;
                    break;
                }
                end = end->next();
            }
            if (end) {
                Token::eraseTokens(tok,end);
                tok->deleteThis();
            }
        }

        else if (match3(tok) &&
                 (!tok->previous() || tok->previous()->str() == MatchCompiler::makeConstString(";"))) {
            const Token *tok2 = tok->tokAt(2);
            std::string type;
            while (match4(tok2)) {
                type += tok2->str() + ",";
                tok2 = tok2->tokAt(2);
            }
            if (match5(tok2)) {
                type += tok2->str();
                tok->str(tok->str() + "<" + type + ">");
                Token::eraseTokens(tok, tok2->tokAt(2));
                if (tok == mTokenList.front())
                    goback = true;
            }
        }
    }
}


void TemplateSimplifier::checkComplicatedSyntaxErrorsInTemplates()
{
    // check for more complicated syntax errors when using templates..
    for (const Token *tok = mTokenList.front(); tok; tok = tok->next()) {
        // skip executing scopes (ticket #3183)..
        if (match6(tok)) {
            tok = tok->link();
            if (!tok)
                syntaxError(nullptr);
        }
        // skip executing scopes..
        const Token *start = Tokenizer::startOfExecutableScope(tok);
        if (start) {
            tok = start->link();
        }

        // skip executing scopes (ticket #1985)..
        else if (match7(tok)) {
            tok = tok->next()->link();
            while (match8(tok)) {
                tok = tok->linkAt(2);
                if (match9(tok))
                    tok = tok->next()->link();
            }
        }

        if (!tok)
            syntaxError(nullptr);
        // not start of statement?
        if (tok->previous() && !match10(tok))
            continue;

        // skip starting tokens.. ;;; typedef typename foo::bar::..
        while (match11(tok))
            tok = tok->next();
        while (match12(tok))
            tok = tok->next();
        while (match13(tok))
            tok = tok->tokAt(2);
        if (!tok)
            break;

        // template variable or type..
        if (match3(tok)) {
            // these are used types..
            std::set<std::string> usedtypes;

            // parse this statement and see if the '<' and '>' are matching
            unsigned int level = 0;
            for (const Token *tok2 = tok; tok2 && !match10(tok2); tok2 = tok2->next()) {
                if (tok2->str() == MatchCompiler::makeConstString("("))
                    tok2 = tok2->link();
                else if (tok2->str() == MatchCompiler::makeConstString("<")) {
                    bool inclevel = false;
                    if (match14(tok2->previous()))
                        ;
                    else if (level == 0)
                        inclevel = true;
                    else if (tok2->next() && tok2->next()->isStandardType())
                        inclevel = true;
                    else if (match15(tok2))
                        inclevel = true;
                    else if (match16(tok2->tokAt(-2)) && usedtypes.find(tok2->previous()->str()) != usedtypes.end())
                        inclevel = true;
                    else if (match17(tok2) && usedtypes.find(tok2->next()->str()) != usedtypes.end())
                        inclevel = true;
                    else if (match17(tok2)) {
                        // is the next token a type and not a variable/constant?
                        // assume it's a type if there comes another "<"
                        const Token *tok3 = tok2->next();
                        while (match13(tok3))
                            tok3 = tok3->tokAt(2);
                        if (match3(tok3))
                            inclevel = true;
                    }

                    if (inclevel) {
                        ++level;
                        if (match16(tok2->tokAt(-2)))
                            usedtypes.insert(tok2->previous()->str());
                    }
                } else if (tok2->str() == MatchCompiler::makeConstString(">")) {
                    if (level > 0)
                        --level;
                } else if (tok2->str() == MatchCompiler::makeConstString(">>")) {
                    if (level > 0)
                        --level;
                    if (level > 0)
                        --level;
                }
            }
            if (level > 0)
                syntaxError(tok);
        }
    }
}

unsigned int TemplateSimplifier::templateParameters(const Token *tok)
{
    unsigned int numberOfParameters = 1;

    if (!tok)
        return 0;
    if (tok->str() != MatchCompiler::makeConstString("<"))
        return 0;
    tok = tok->next();

    unsigned int level = 0;

    while (tok) {
        // skip const/volatile
        if (match18(tok))
            tok = tok->next();

        // skip struct/union
        if (match19(tok))
            tok = tok->next();

        // Skip '&'
        if (match20(tok))
            tok = tok->next();

        // Skip variadic types (Ticket #5774, #6059, #6172)
        if (match21(tok)) {
            tok = tok->tokAt(4);
            continue;
        }

        // Skip '=', '?', ':'
        if (match22(tok))
            tok = tok->next();
        if (!tok)
            return 0;

        // Skip casts
        if (tok->str() == MatchCompiler::makeConstString("(")) {
            tok = tok->link();
            if (tok)
                tok = tok->next();
        }

        // skip std::
        if (tok && tok->str() == MatchCompiler::makeConstString("::"))
            tok = tok->next();
        while (match23(tok)) {
            tok = tok->tokAt(2);
            if (tok && tok->str() == MatchCompiler::makeConstString("*")) // Ticket #5759: Class member pointer as a template argument; skip '*'
                tok = tok->next();
        }
        if (!tok)
            return 0;

        // num/type ..
        if (!tok->isNumber() && tok->tokType() != Token::eChar && !tok->isName() && !tok->isOp())
            return 0;
        tok = tok->next();
        if (!tok)
            return 0;

        // * / const
        while (match24(tok))
            tok = tok->next();

        if (!tok)
            return 0;

        // Function pointer or prototype..
        while (match25(tok)) {
            if (!tok->link())
                syntaxError(tok);

            tok = tok->link()->next();
            while (match18(tok)) // Ticket #5786: Skip function cv-qualifiers
                tok = tok->next();
        }
        if (!tok)
            return 0;

        // inner template
        if (tok->str() == MatchCompiler::makeConstString("<")) {
            ++level;
            tok = tok->next();
        }

        if (!tok)
            return 0;

        // ,/>
        while (match26(tok)) {
            if (level == 0)
                return ((tok->str() == MatchCompiler::makeConstString(">")) ? numberOfParameters : 0);
            --level;
            if (tok->str() == MatchCompiler::makeConstString(">>")) {
                if (level == 0)
                    return numberOfParameters;
                --level;
            }
            tok = tok->next();

            if (match27(tok))
                tok = tok->link()->next();

            if (!tok)
                return 0;
        }

        if (tok->str() != MatchCompiler::makeConstString(","))
            continue;
        if (level == 0)
            ++numberOfParameters;
        tok = tok->next();
    }
    return 0;
}

void TemplateSimplifier::eraseTokens(Token *begin, const Token *end)
{
    if (!begin || begin == end)
        return;

    while (begin->next() && begin->next() != end) {
        // check if token has a pointer to it
        if (begin->next()->hasTemplateSimplifierPointer()) {
            // check if the token is in a list
            const std::list<TokenAndName>::iterator it = std::find_if(mTemplateInstantiations.begin(),
                    mTemplateInstantiations.end(),
                    FindToken(begin->next()));
            if (it != mTemplateInstantiations.end()) {
                // remove the pointer flag and prevent the deleted pointer from being used
                it->token->hasTemplateSimplifierPointer(false);
                it->token = nullptr;
            }
        }
        begin->deleteNext();
    }
}

void TemplateSimplifier::deleteToken(Token *tok)
{
    if (tok->next())
        tok->next()->deletePrevious();
    else
        tok->deleteThis();
}

bool TemplateSimplifier::removeTemplate(Token *tok)
{
    if (!match28(tok))
        return false;

    int indentlevel = 0;
    unsigned int countgt = 0;   // Counter for ">"
    for (const Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {

        if (tok2->str() == MatchCompiler::makeConstString("(")) {
            tok2 = tok2->link();
        } else if (tok2->str() == MatchCompiler::makeConstString(")")) {  // garbage code! (#3504)
            Token::eraseTokens(tok,tok2);
            deleteToken(tok);
            return false;
        }

        else if (tok2->str() == MatchCompiler::makeConstString("{")) {
            tok2 = tok2->link()->next();
            Token::eraseTokens(tok, tok2);
            if (tok2 && tok2->str() == MatchCompiler::makeConstString(";") && tok2->next())
                tok->deleteNext();
            deleteToken(tok);
            return true;
        } else if (tok2->str() == MatchCompiler::makeConstString("}")) {  // garbage code! (#3449)
            Token::eraseTokens(tok,tok2);
            deleteToken(tok);
            return false;
        }

        // Count ">"
        if (tok2->str() == MatchCompiler::makeConstString(">"))
            countgt++;

        // don't remove constructor
        if (tok2->str() == MatchCompiler::makeConstString("explicit") ||
            (countgt == 1 && match29(tok2->previous()) &&
             Tokenizer::startOfExecutableScope(tok2->linkAt(1)))) {
            Token::eraseTokens(tok, tok2);
            deleteToken(tok);
            return true;
        }

        if (tok2->str() == MatchCompiler::makeConstString(";")) {
            tok2 = tok2->next();
            Token::eraseTokens(tok, tok2);
            deleteToken(tok);
            return true;
        }

        if (tok2->str() == MatchCompiler::makeConstString("<"))
            ++indentlevel;

        else if (indentlevel >= 2 && tok2->str() == MatchCompiler::makeConstString(">"))
            --indentlevel;

        else if (match30(tok2)) {
            tok2 = tok2->next();
            Token::eraseTokens(tok, tok2);
            deleteToken(tok);
            return true;
        }
    }

    return false;
}

std::set<std::string> TemplateSimplifier::expandSpecialized()
{
    std::set<std::string> expandedtemplates;

    // Locate specialized templates..
    for (Token *tok = mTokenList.front(); tok; tok = tok->next()) {
        if (!match31(tok))
            continue;

        // what kind of template is this?
        Token *tok2 = tok->tokAt(3);
        while (tok2 && (tok2->isName() || tok2->str() == MatchCompiler::makeConstString("*")))
            tok2 = tok2->next();

        if (!templateParameters(tok2))
            continue;

        // unknown template.. bail out
        if (!tok2->previous()->isName())
            continue;

        tok2 = tok2->previous();
        std::string s;
        {
            std::ostringstream ostr;
            const Token *tok3 = tok2;
            for (; tok3 && tok3->str() != MatchCompiler::makeConstString(">"); tok3 = tok3->next()) {
                if (tok3 != tok2)
                    ostr << " ";
                ostr << tok3->str();
            }
            if (!match32(tok3))
                continue;
            s = ostr.str();
        }

        // remove spaces to create new name
        const std::string name(s + " >");
        expandedtemplates.insert(name);

        // Rename template..
        Token::eraseTokens(tok2, findmatch33(tok2) ->findClosingBracket()->next());
        tok2->str(name);

        // delete the "template < >"
        tok->deleteNext(2);
        tok->deleteThis();

        // Use this special template in the code..
        while (nullptr != (tok2 = const_cast<Token *>(Token::findsimplematch(tok2, name.c_str())))) {
            Token::eraseTokens(tok2, findmatch33(tok2) ->findClosingBracket()->next());
            tok2->str(name);
        }
    }

    return expandedtemplates;
}

/// TODO: This is copy pasted from Tokenizer. We should reuse this code.
namespace {
    struct ScopeInfo2 {
        ScopeInfo2(const std::string &name_, const Token *bodyEnd_) : name(name_), bodyEnd(bodyEnd_) {}
        const std::string name;
        const Token * const bodyEnd;
    };
}
static std::string getScopeName(const std::list<ScopeInfo2> &scopeInfo)
{
    std::string ret;
    for (const ScopeInfo2 &i : scopeInfo)
        ret += (ret.empty() ? "" : " :: ") + i.name;
    return ret;
}
static std::string getFullName(const std::list<ScopeInfo2> &scopeInfo, const std::string &name)
{
    const std::string &scopeName = getScopeName(scopeInfo);
    return scopeName + (scopeName.empty() ? "" : " :: ") + name;
}

static void setScopeInfo(const Token *tok, std::list<ScopeInfo2> *scopeInfo)
{
    while (tok->str() == MatchCompiler::makeConstString("}") && !scopeInfo->empty() && tok == scopeInfo->back().bodyEnd)
        scopeInfo->pop_back();
    if (!match34(tok))
        return;
    tok = tok->next();
    std::string classname = tok->str();
    while (match35(tok)) {
        tok = tok->tokAt(2);
        classname += " :: " + tok->str();
    }
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(":")) {
        // ...
    }
    if (tok && tok->str() == MatchCompiler::makeConstString("{")) {
        scopeInfo->emplace_back(classname,tok->link());
    }
}

std::list<TemplateSimplifier::TokenAndName> TemplateSimplifier::getTemplateDeclarations(bool &codeWithTemplates)
{
    std::list<ScopeInfo2> scopeInfo;
    std::list<TokenAndName> declarations;
    for (Token *tok = mTokenList.front(); tok; tok = tok->next()) {
        if (match36(tok)) {
            setScopeInfo(tok, &scopeInfo);
            continue;
        }
        if (!match28(tok))
            continue;
        // Some syntax checks, see #6865
        if (!tok->tokAt(2))
            syntaxError(tok->next());
        if (tok->strAt(2)==MatchCompiler::makeConstString("typename") &&
            !match37(tok->tokAt(3)))
            syntaxError(tok->next());
        codeWithTemplates = true;
        const Token * const parmEnd = tok->next()->findClosingBracket();
        for (const Token *tok2 = parmEnd; tok2; tok2 = tok2->next()) {
            if (tok2->str() == MatchCompiler::makeConstString("("))
                tok2 = tok2->link();
            else if (tok2->str() == MatchCompiler::makeConstString(")"))
                break;
            // Just a declaration => ignore this
            else if (tok2->str() == MatchCompiler::makeConstString(";"))
                break;
            // Implementation => add to "templates"
            else if (tok2->str() == MatchCompiler::makeConstString("{")) {
                const int namepos = getTemplateNamePosition(parmEnd);
                if (namepos > 0)
                    declarations.emplace_back(tok, getScopeName(scopeInfo), getFullName(scopeInfo, parmEnd->strAt(namepos)));
                break;
            }
        }
    }
    return declarations;
}


void TemplateSimplifier::getTemplateInstantiations()
{
    std::list<ScopeInfo2> scopeList;

    for (Token *tok = mTokenList.front(); tok; tok = tok->next()) {
        if (match36(tok)) {
            setScopeInfo(tok, &scopeList);
            continue;
        }
        // template definition.. skip it
        if (match28(tok)) {
            tok = tok->next()->findClosingBracket();
            if (!tok)
                break;
            // #7914
            // Ignore template instantiations within template definitions: they will only be
            // handled if the definition is actually instantiated
            const Token *tok2 = findmatch38(tok) ;
            if (tok2 && tok2->str() == MatchCompiler::makeConstString("{"))
                tok = tok2->link();
        } else if (match39(tok->previous()) ||
                   match40(tok->previous()) ||
                   match41(tok->tokAt(-2))) {

            std::string scopeName = getScopeName(scopeList);
            while (match35(tok)) {
                scopeName += (scopeName.empty() ? "" : " :: ") + tok->str();
                tok = tok->tokAt(2);
            }
            if (!match42(tok))
                continue;

            // Add inner template instantiations first => go to the ">"
            // and then parse backwards, adding all seen instantiations
            const Token *tok2 = tok->next()->findClosingBracket();

            // parse backwards and add template instantiations
            // TODO
            for (; tok2 && tok2 != tok; tok2 = tok2->previous()) {
                if (match43(tok2) &&
                    templateParameters(tok2->tokAt(2))) {
                    mTemplateInstantiations.emplace_back(tok2->next(), getScopeName(scopeList), getFullName(scopeList, tok2->strAt(1)));
                }
            }

            // Add outer template..
            if (templateParameters(tok->next())) {
                const std::string scopeName1(scopeName);
                while (true) {
                    const std::string fullName = scopeName + (scopeName.empty()?"":" :: ") + tok->str();
                    const std::list<TokenAndName>::const_iterator it = std::find_if(mTemplateDeclarations.begin(), mTemplateDeclarations.end(), FindName(fullName));
                    if (it != mTemplateDeclarations.end()) {
                        mTemplateInstantiations.emplace_back(tok, getScopeName(scopeList), fullName);
                        break;
                    } else {
                        if (scopeName.empty()) {
                            mTemplateInstantiations.emplace_back(tok, getScopeName(scopeList), scopeName1 + (scopeName1.empty()?"":" :: ") + tok->str());
                            break;
                        }
                        const std::string::size_type pos = scopeName.rfind(" :: ");
                        scopeName = (pos == std::string::npos) ? std::string() : scopeName.substr(0,pos);
                    }
                }
            }
        }
    }
}


void TemplateSimplifier::useDefaultArgumentValues()
{
    for (const TokenAndName &template1 : mTemplateDeclarations) {
        // template parameters with default value has syntax such as:
        //     x = y
        // this list will contain all the '=' tokens for such arguments
        std::list<Token *> eq;
        // and this set the position of parameters with a default value
        std::set<std::size_t> defaultedArgPos;

        // parameter number. 1,2,3,..
        std::size_t templatepar = 1;

        // parameter depth
        std::size_t templateParmDepth = 0;

        // the template classname. This will be empty for template functions
        std::string classname;

        // Scan template declaration..
        for (Token *tok = template1.token; tok; tok = tok->next()) {
            if (match31(tok)) { // Ticket #5762: Skip specialization tokens
                tok = tok->tokAt(2);
                if (0 == templateParmDepth)
                    break;
                continue;
            }

            if (tok->str() == MatchCompiler::makeConstString("(")) { // Ticket #6835
                tok = tok->link();
                continue;
            }

            if (tok->str() == MatchCompiler::makeConstString("<") && templateParameters(tok))
                ++templateParmDepth;

            // end of template parameters?
            if (tok->str() == MatchCompiler::makeConstString(">")) {
                if (match44(tok))
                    classname = tok->strAt(2);
                if (templateParmDepth<2)
                    break;
                else
                    --templateParmDepth;
            }

            // next template parameter
            if (tok->str() == MatchCompiler::makeConstString(",") && (1 == templateParmDepth)) // Ticket #5823: Properly count parameters
                ++templatepar;

            // default parameter value?
            else if (match45(tok)) {
                if (defaultedArgPos.insert(templatepar).second) {
                    eq.push_back(tok);
                } else {
                    // Ticket #5605: Syntax error (two equal signs for the same parameter), bail out
                    eq.clear();
                    break;
                }
            }
        }
        if (eq.empty() || classname.empty())
            continue;

        // iterate through all template instantiations
        for (const TokenAndName &templateInst : mTemplateInstantiations) {
            Token *tok = templateInst.token;

            if (!Token::simpleMatch(tok, (classname + " <").c_str()))
                continue;

            // count the parameters..
            tok = tok->next();
            const unsigned int usedpar = templateParameters(tok);
            tok = tok->findClosingBracket();

            if (tok && tok->str() == MatchCompiler::makeConstString(">")) {
                tok = tok->previous();
                std::list<Token *>::const_iterator it = eq.begin();
                for (std::size_t i = (templatepar - eq.size()); it != eq.end() && i < usedpar; ++i)
                    ++it;
                while (it != eq.end()) {
                    int indentlevel = 0;
                    tok->insertToken(",");
                    tok = tok->next();
                    const Token *from = (*it)->next();
                    std::stack<Token *> links;
                    while (from && (!links.empty() || indentlevel || !match46(from))) {
                        if (from->str() == MatchCompiler::makeConstString("<"))
                            ++indentlevel;
                        else if (from->str() == MatchCompiler::makeConstString(">"))
                            --indentlevel;
                        tok->insertToken(from->str(), from->originalName());
                        tok = tok->next();
                        if (match25(tok))
                            links.push(tok);
                        else if (!links.empty() && match47(tok)) {
                            Token::createMutualLinks(links.top(), tok);
                            links.pop();
                        }
                        from = from->next();
                    }
                    ++it;
                }
            }
        }

        for (Token * const eqtok : eq) {
            Token *tok2;
            int indentlevel = 0;
            for (tok2 = eqtok->next(); tok2; tok2 = tok2->next()) {
                if (match48(tok2)) { // bail out #6607
                    tok2 = nullptr;
                    break;
                }
                if (match49(tok2))
                    tok2 = tok2->link();
                else if (match3(tok2) && templateParameters(tok2->next())) {
                    std::list<TokenAndName>::iterator ti = std::find_if(mTemplateInstantiations.begin(),
                                                           mTemplateInstantiations.end(),
                                                           FindToken(tok2));
                    if (ti != mTemplateInstantiations.end())
                        mTemplateInstantiations.erase(ti);
                    ++indentlevel;
                } else if (indentlevel > 0 && tok2->str() == MatchCompiler::makeConstString(">"))
                    --indentlevel;
                else if (indentlevel == 0 && match46(tok2))
                    break;
                if (indentlevel < 0)
                    break;
            }
            // something went wrong, don't call eraseTokens()
            // with a nullptr "end" parameter (=all remaining tokens).
            if (!tok2)
                continue;

            Token::eraseTokens(eqtok, tok2);
            eqtok->deleteThis();
        }
    }
}

void TemplateSimplifier::simplifyTemplateAliases()
{
    std::list<TokenAndName>::iterator it1, it2;
    for (it1 = mTemplateInstantiations.begin(); it1 != mTemplateInstantiations.end();) {
        TokenAndName &templateAlias = *it1;
        ++it1;
        Token *startToken = templateAlias.token;
        while (match35(startToken->tokAt(-2)))
            startToken = startToken->tokAt(-2);
        if (!match50(startToken->tokAt(-4)))
            continue;
        const std::string aliasName(startToken->strAt(-2));
        const Token * const aliasToken1 = startToken;

        // Get start token for alias
        startToken = startToken->tokAt(-5);
        while (match51(startToken))
            startToken = startToken->previous();
        if (!match52(startToken))
            continue;

        // alias parameters..
        std::vector<const Token *> aliasParameters;
        getTemplateParametersInDeclaration(startToken->tokAt(3), aliasParameters);
        std::map<std::string, unsigned int> aliasParameterNames;
        for (unsigned int argnr = 0; argnr < aliasParameters.size(); ++argnr)
            aliasParameterNames[aliasParameters[argnr]->str()] = argnr;

        // Look for alias usages..
        const Token *endToken = nullptr;
        for (it2 = it1; it2 != mTemplateInstantiations.end(); ++it2) {
            TokenAndName &aliasUsage = *it2;
            if (aliasUsage.name != aliasName)
                continue;
            std::vector<std::pair<Token *, Token *>> args;
            Token *tok2 = aliasUsage.token->tokAt(2);
            while (tok2) {
                Token * const start = tok2;
                while (tok2 && !match53(tok2)) {
                    if (tok2->link() && match54(tok2))
                        tok2 = tok2->link();
                    tok2 = tok2->next();
                }

                args.emplace_back(start, tok2);
                if (tok2 && tok2->str() == MatchCompiler::makeConstString(",")) {
                    tok2 = tok2->next();
                } else {
                    break;
                }
            }
            if (!tok2 || tok2->str() != MatchCompiler::makeConstString(">") || args.size() != aliasParameters.size())
                continue;

            // Replace template alias code..
            aliasUsage.name = templateAlias.name;
            if (aliasUsage.name.find(' ') == std::string::npos) {
                aliasUsage.token->str(templateAlias.token->str());
            } else {
                tok2 = TokenList::copyTokens(aliasUsage.token, aliasToken1, templateAlias.token, true);
                deleteToken(aliasUsage.token);
                aliasUsage.token = tok2;
            }
            tok2 = aliasUsage.token->next(); // the '<'
            const Token * const endToken1 = templateAlias.token->next()->findClosingBracket();
            const Token * const endToken2 = TokenList::copyTokens(tok2, templateAlias.token->tokAt(2), endToken1->previous(), false);
            for (const Token *tok1 = templateAlias.token->next(); tok2 != endToken2; tok1 = tok1->next(), tok2 = tok2->next()) {
                if (!tok2->isName())
                    continue;
                if (aliasParameterNames.find(tok2->str()) == aliasParameterNames.end()) {
                    // Create template instance..
                    if (match42(tok1)) {
                        const std::list<TokenAndName>::iterator it = std::find_if(mTemplateInstantiations.begin(),
                                mTemplateInstantiations.end(),
                                FindToken(tok1));
                        if (it != mTemplateInstantiations.end())
                            mTemplateInstantiations.emplace_back(tok2, it->scope, it->name);
                    }
                    continue;
                }
                const unsigned int argnr = aliasParameterNames[tok2->str()];
                const Token * const fromStart = args[argnr].first;
                const Token * const fromEnd   = args[argnr].second->previous();
                Token * const destToken = tok2;
                tok2 = TokenList::copyTokens(tok2, fromStart, fromEnd, true);
                if (tok2 == destToken->next())
                    tok2 = destToken;
                destToken->deleteThis();
            }

            endToken = endToken1->next();

            // Remove alias usage code (parameters)
            Token::eraseTokens(tok2, args.back().second);
        }
        if (endToken) {
            // Remove all template instantiations in template alias
            for (const Token *tok = startToken; tok != endToken; tok = tok->next()) {
                if (!match42(tok))
                    continue;
                std::list<TokenAndName>::iterator it = std::find_if(mTemplateInstantiations.begin(),
                                                       mTemplateInstantiations.end(),
                                                       FindToken(tok));
                if (it == mTemplateInstantiations.end())
                    continue;
                std::list<TokenAndName>::iterator next = it;
                ++next;
                if (it == it1)
                    it1 = next;
                mTemplateInstantiations.erase(it,next);
            }

            Token::eraseTokens(startToken, endToken);
        }
    }
}

bool TemplateSimplifier::instantiateMatch(const Token *instance, const std::size_t numberOfArguments, const char patternAfter[])
{
//    if (!Token::simpleMatch(instance, (name + " <").c_str()))
//        return false;

    if (numberOfArguments != templateParameters(instance->next()))
        return false;

    if (patternAfter) {
        const Token *tok = instance;
        unsigned int indentlevel = 0;
        for (tok = instance; tok && (tok->str() != MatchCompiler::makeConstString(">") || indentlevel > 0); tok = tok->next()) {
            if (match55(tok) && templateParameters(tok->tokAt(2)) > 0)
                ++indentlevel;
            if (indentlevel > 0 && tok->str() == MatchCompiler::makeConstString(">"))
                --indentlevel;
        }
        if (!tok || !Token::Match(tok->next(), patternAfter))
            return false;
    }

    // nothing mismatching was found..
    return true;
}

// Utility function for TemplateSimplifier::getTemplateNamePosition, that works on template member functions,
// hence this pattern: "> %type% [%type%] < ... > :: %type% ("
static bool getTemplateNamePositionTemplateMember(const Token *tok, int &namepos)
{
    namepos = 2;
    while (tok && tok->next()) {
        if (match56(tok->next()))
            return false;
        else if (match3(tok->next())) {
            const Token *closing = tok->tokAt(2)->findClosingBracket();
            if (closing && match57(closing->next())) {
                if (closing->strAt(1) == MatchCompiler::makeConstString("~"))
                    closing = closing->next();
                while (tok && tok->next() != closing->next()) {
                    tok = tok->next();
                    namepos++;
                }
                return true;
            }
        }
        tok = tok->next();
        namepos++;
    }
    return false;
}

int TemplateSimplifier::getTemplateNamePosition(const Token *tok)
{
    // get the position of the template name
    int namepos = 0, starAmpPossiblePosition = 0;
    if (match58(tok))
        namepos = 2;
    else if (match59(tok))
        namepos = 2;
    else if (match60(tok))
        namepos = 3;
    else if (getTemplateNamePositionTemplateMember(tok, namepos))
        ;
    else if (match61(tok)) {
        namepos = 4;
        starAmpPossiblePosition = 2;
    } else if (match62(tok)) {
        namepos = 5;
        starAmpPossiblePosition = 3;
    } else {
        // Name not found
        return -1;
    }
    if (match63(tok->tokAt(starAmpPossiblePosition ? starAmpPossiblePosition : namepos)))
        ++namepos;

    return namepos;
}


void TemplateSimplifier::expandTemplate(
    const Token *templateDeclarationToken,
    const std::string &fullName,
    const std::vector<const Token *> &typeParametersInDeclaration,
    const std::string &newName,
    const std::vector<const Token *> &typesUsedInTemplateInstantiation)
{
    std::list<ScopeInfo2> scopeInfo;
    bool inTemplateDefinition = false;
    const Token *startOfTemplateDeclaration = nullptr;
    const Token *endOfTemplateDefinition = nullptr;
    const Token * const templateDeclarationNameToken = templateDeclarationToken->tokAt(getTemplateNamePosition(templateDeclarationToken));
    for (const Token *tok3 = mTokenList.front(); tok3; tok3 = tok3 ? tok3->next() : nullptr) {
        if (match36(tok3)) {
            setScopeInfo(tok3, &scopeInfo);
            continue;
        }
        if (inTemplateDefinition) {
            if (!endOfTemplateDefinition && tok3->str() == MatchCompiler::makeConstString("{"))
                endOfTemplateDefinition = tok3->link();
            if (tok3 == endOfTemplateDefinition) {
                inTemplateDefinition = false;
                startOfTemplateDeclaration = nullptr;
            }
        }

        if (tok3->str()==MatchCompiler::makeConstString("template")) {
            if (tok3->next() && tok3->next()->str()==MatchCompiler::makeConstString("<")) {
                std::vector<const Token *> localTypeParametersInDeclaration;
                getTemplateParametersInDeclaration(tok3->tokAt(2), localTypeParametersInDeclaration);
                if (localTypeParametersInDeclaration.size() != typeParametersInDeclaration.size())
                    inTemplateDefinition = false; // Partial specialization
                else
                    inTemplateDefinition = true;
            } else {
                inTemplateDefinition = false; // Only template instantiation
            }
            startOfTemplateDeclaration = tok3;
        }
        if (match25(tok3))
            tok3 = tok3->link();

        // Start of template..
        if (tok3 == templateDeclarationToken) {
            tok3 = tok3->next();
        }

        // member function implemented outside class definition
        else if (inTemplateDefinition &&
                 match42(tok3) &&
                 fullName == getFullName(scopeInfo, tok3->str()) &&
                 instantiateMatch(tok3, typeParametersInDeclaration.size(), ":: ~| %name% (")) {
            // there must be template..
            bool istemplate = false;
            const Token * tok5 = nullptr; // start of function return type
            for (const Token *prev = tok3; prev && !match10(prev); prev = prev->previous()) {
                if (prev->str() == MatchCompiler::makeConstString("template")) {
                    istemplate = true;
                    tok5 = prev;
                    break;
                }
            }
            if (!istemplate)
                continue;

            const Token *tok4 = tok3->next()->findClosingBracket();
            while (tok4 && tok4->str() != MatchCompiler::makeConstString("("))
                tok4 = tok4->next();
            if (!Tokenizer::isFunctionHead(tok4, ":{", true))
                continue;
            // find function return type start
            tok5 = tok5->next()->findClosingBracket();
            if (tok5)
                tok5 = tok5->next();
            // copy return type
            while (tok5 && tok5 != tok3) {
                // replace name if found
                if (match42(tok5) && tok5->str() == fullName) {
                    mTokenList.addtoken(newName, tok5->linenr(), tok5->fileIndex());
                    tok5 = tok5->next()->findClosingBracket();
                } else
                    mTokenList.addtoken(tok5, tok5->linenr(), tok5->fileIndex());
                tok5 = tok5->next();
            }
            mTokenList.addtoken(newName, tok3->linenr(), tok3->fileIndex());
            while (tok3 && tok3->str() != MatchCompiler::makeConstString("::"))
                tok3 = tok3->next();

            std::list<TokenAndName>::iterator it = std::find_if(mTemplateDeclarations.begin(),
                                                   mTemplateDeclarations.end(),
                                                   FindToken(startOfTemplateDeclaration));
            if (it != mTemplateDeclarations.end())
                mMemberFunctionsToDelete.push_back(*it);
        }

        // not part of template.. go on to next token
        else
            continue;

        std::stack<Token *> brackets; // holds "(", "[" and "{" tokens

        // FIXME use full name matching somehow
        const std::string lastName = (fullName.find(' ') != std::string::npos) ? fullName.substr(fullName.rfind(' ')+1) : fullName;

        for (; tok3; tok3 = tok3->next()) {
            if (tok3->isName()) {
                // search for this token in the type vector
                unsigned int itype = 0;
                while (itype < typeParametersInDeclaration.size() && typeParametersInDeclaration[itype]->str() != tok3->str())
                    ++itype;

                // replace type with given type..
                if (itype < typeParametersInDeclaration.size()) {
                    unsigned int typeindentlevel = 0;
                    for (const Token *typetok = typesUsedInTemplateInstantiation[itype];
                         typetok && (typeindentlevel>0 || !match46(typetok));
                         typetok = typetok->next()) {
                        if (match64(typetok)) {
                            typetok = typetok->tokAt(2);
                            continue;
                        }
                        if (match42(typetok) && templateParameters(typetok->next()) > 0)
                            ++typeindentlevel;
                        else if (typeindentlevel > 0 && typetok->str() == MatchCompiler::makeConstString(">"))
                            --typeindentlevel;
                        mTokenList.addtoken(typetok, tok3->linenr(), tok3->fileIndex());
                        mTokenList.back()->isTemplateArg(true);
                    }
                    continue;
                }
            }

            // replace name..
            if (tok3->str() == lastName) {
                if (!match65(tok3->next())) {
                    mTokenList.addtoken(newName, tok3->linenr(), tok3->fileIndex());
                    continue;
                } else if (tok3 == templateDeclarationNameToken) {
                    mTokenList.addtoken(newName, tok3->linenr(), tok3->fileIndex());
                    tok3 = tok3->next()->findClosingBracket();
                    continue;
                }
            }

            // copy
            mTokenList.addtoken(tok3, tok3->linenr(), tok3->fileIndex());
            if (match3(tok3) && match26(tok3->next()->findClosingBracket())) {
                const Token *closingBracket = tok3->next()->findClosingBracket();
                if (match66(closingBracket->next())) {
                    int num = 0;
                    const Token *par = tok3->next();
                    while (num < typeParametersInDeclaration.size() && par != closingBracket) {
                        const std::string pattern("[<,] " + typeParametersInDeclaration[num]->str() + " [,>]");
                        if (!Token::Match(par, pattern.c_str()))
                            break;
                        ++num;
                        par = par->tokAt(2);
                    }
                    if (num < typeParametersInDeclaration.size() || par != closingBracket)
                        continue;
                }

                std::string name = tok3->str();
                for (const Token *prev = tok3->tokAt(-2); match23(prev); prev = prev->tokAt(-2))
                    name = prev->str() + " :: " + name;
                mTemplateInstantiations.emplace_back(mTokenList.back(), getScopeName(scopeInfo), getFullName(scopeInfo, name));
            }

            // link() newly tokens manually
            else if (tok3->str() == MatchCompiler::makeConstString("{")) {
                brackets.push(mTokenList.back());
            } else if (tok3->str() == MatchCompiler::makeConstString("(")) {
                brackets.push(mTokenList.back());
            } else if (tok3->str() == MatchCompiler::makeConstString("[")) {
                brackets.push(mTokenList.back());
            } else if (tok3->str() == MatchCompiler::makeConstString("}")) {
                assert(brackets.empty() == false && brackets.top()->str() == MatchCompiler::makeConstString("{"));
                Token::createMutualLinks(brackets.top(), mTokenList.back());
                if (tok3->strAt(1) == MatchCompiler::makeConstString(";")) {
                    const Token * tokSemicolon = tok3->next();
                    mTokenList.addtoken(tokSemicolon, tokSemicolon->linenr(), tokSemicolon->fileIndex());
                }
                brackets.pop();
                if (brackets.empty()) {
                    inTemplateDefinition = false;
                    break;
                }
            } else if (tok3->str() == MatchCompiler::makeConstString(")")) {
                assert(brackets.empty() == false && brackets.top()->str() == MatchCompiler::makeConstString("("));
                Token::createMutualLinks(brackets.top(), mTokenList.back());
                brackets.pop();
            } else if (tok3->str() == MatchCompiler::makeConstString("]")) {
                assert(brackets.empty() == false && brackets.top()->str() == MatchCompiler::makeConstString("["));
                Token::createMutualLinks(brackets.top(), mTokenList.back());
                brackets.pop();
            }
        }

        assert(brackets.empty());
    }
}

static bool isLowerThanLogicalAnd(const Token *lower)
{
    return lower->isAssignmentOp() || match67(lower);
}
static bool isLowerThanOr(const Token* lower)
{
    return isLowerThanLogicalAnd(lower) || lower->str() == MatchCompiler::makeConstString("&&");
}
static bool isLowerThanXor(const Token* lower)
{
    return isLowerThanOr(lower) || lower->str() == MatchCompiler::makeConstString("|");
}
static bool isLowerThanAnd(const Token* lower)
{
    return isLowerThanXor(lower) || lower->str() == MatchCompiler::makeConstString("^");
}
static bool isLowerThanShift(const Token* lower)
{
    return isLowerThanAnd(lower) || lower->str() == MatchCompiler::makeConstString("&");
}
static bool isLowerThanPlusMinus(const Token* lower)
{
    return isLowerThanShift(lower) || match68(lower);
}
static bool isLowerThanMulDiv(const Token* lower)
{
    return isLowerThanPlusMinus(lower) || match69(lower);
}
static bool isLowerEqualThanMulDiv(const Token* lower)
{
    return isLowerThanMulDiv(lower) || match70(lower);
}


bool TemplateSimplifier::simplifyNumericCalculations(Token *tok)
{
    bool ret = false;
    // (1-2)
    while (tok->tokAt(3) && tok->isNumber() && tok->tokAt(2)->isNumber()) { // %any% %num% %any% %num% %any%
        const Token *before = tok->previous();
        if (!before)
            break;
        const Token* op = tok->next();
        const Token* after = tok->tokAt(3);
        const std::string &num1 = op->previous()->str();
        const std::string &num2 = op->next()->str();
        if (match71(before) && (num2 != MatchCompiler::makeConstString("0")) && num1 == MathLib::multiply(num2, MathLib::divide(num1, num2))) {
            // Division where result is a whole number
        } else if (!((op->str() == MatchCompiler::makeConstString("*") && (isLowerThanMulDiv(before) || before->str() == MatchCompiler::makeConstString("*")) && isLowerEqualThanMulDiv(after)) || // associative
                     (match72(op) && isLowerThanMulDiv(before) && isLowerEqualThanMulDiv(after)) || // NOT associative
                     (match73(op) && isLowerThanMulDiv(before) && isLowerThanMulDiv(after)) || // Only partially (+) associative, but handled later
                     (match74(op) && isLowerThanShift(before) && isLowerThanPlusMinus(after)) || // NOT associative
                     (op->str() == MatchCompiler::makeConstString("&") && isLowerThanShift(before) && isLowerThanShift(after)) || // associative
                     (op->str() == MatchCompiler::makeConstString("^") && isLowerThanAnd(before) && isLowerThanAnd(after)) || // associative
                     (op->str() == MatchCompiler::makeConstString("|") && isLowerThanXor(before) && isLowerThanXor(after)) || // associative
                     (op->str() == MatchCompiler::makeConstString("&&") && isLowerThanOr(before) && isLowerThanOr(after)) ||
                     (op->str() == MatchCompiler::makeConstString("||") && isLowerThanLogicalAnd(before) && isLowerThanLogicalAnd(after))))
            break;

        // Don't simplify "%num% / 0"
        if (match75(op))
            break;

        // Integer operations
        if (match76(op)) {
            // Don't simplify if operand is negative, shifting with negative
            // operand is UB. Bitmasking with negative operand is implementation
            // defined behaviour.
            if (MathLib::isNegative(num1) || MathLib::isNegative(num2))
                break;

            const MathLib::value v1(num1);
            const MathLib::value v2(num2);

            if (!v1.isInt() || !v2.isInt())
                break;

            switch (op->str()[0]) {
            case '<':
                tok->str((v1 << v2).str());
                break;
            case '>':
                tok->str((v1 >> v2).str());
                break;
            case '&':
                tok->str((v1 & v2).str());
                break;
            case '|':
                tok->str((v1 | v2).str());
                break;
            case '^':
                tok->str((v1 ^ v2).str());
                break;
            };
        }

        // Logical operations
        else if (match77(op)) {
            const bool op1 = !MathLib::isNullValue(num1);
            const bool op2 = !MathLib::isNullValue(num2);
            const bool result = (op->str() == MatchCompiler::makeConstString("||")) ? (op1 || op2) : (op1 && op2);
            tok->str(result ? "1" : "0");
        }

        else if (match78(tok->previous()))
            tok->str(MathLib::add(num1, num2));
        else if (match79(tok->previous()))
            tok->str(MathLib::subtract(num1, num2));
        else {
            try {
                tok->str(MathLib::calculate(num1, num2, op->str()[0]));
            } catch (InternalError &e) {
                e.token = tok;
                throw;
            }
        }

        tok->deleteNext(2);

        ret = true;
    }

    return ret;
}

// TODO: This is not the correct class for simplifyCalculations(), so it
// should be moved away.
bool TemplateSimplifier::simplifyCalculations()
{
    bool ret = false;
    for (Token *tok = mTokenList.front(); tok; tok = tok->next()) {
        // Remove parentheses around variable..
        // keep parentheses here: dynamic_cast<Fred *>(p);
        // keep parentheses here: A operator * (int);
        // keep parentheses here: int ( * ( * f ) ( ... ) ) (int) ;
        // keep parentheses here: int ( * * ( * compilerHookVector ) (void) ) ( ) ;
        // keep parentheses here: operator new [] (size_t);
        // keep parentheses here: Functor()(a ... )
        // keep parentheses here: ) ( var ) ;
        if ((match80(tok->next()) ||
             (match81(tok->next()) && (tok->tokAt(2)->varId()>0 || !match82(tok->tokAt(4))))) &&
            !tok->isName() &&
            tok->str() != MatchCompiler::makeConstString(">") &&
            tok->str() != MatchCompiler::makeConstString(")") &&
            tok->str() != MatchCompiler::makeConstString("]")) {
            tok->deleteNext();
            tok = tok->next();
            tok->deleteNext();
            ret = true;
        }

        if (match83(tok->previous())) {
            tok->str(MathLib::toString(MathLib::toLongNumber(tok->str())));
        }

        if (tok->isNumber()) {
            if (simplifyNumericCalculations(tok)) {
                ret = true;
                Token *prev = tok->tokAt(-2);
                while (prev && simplifyNumericCalculations(prev)) {
                    tok = prev;
                    prev = prev->tokAt(-2);
                }
            }

            // Remove redundant conditions (0&&x) (1||x)
            if (match84(tok->previous()) ||
                match85(tok->previous())) {
                unsigned int par = 0;
                const Token *tok2 = tok;
                const bool andAnd = (tok->next()->str() == MatchCompiler::makeConstString("&&"));
                for (; tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString("(") || tok2->str() == MatchCompiler::makeConstString("["))
                        ++par;
                    else if (tok2->str() == MatchCompiler::makeConstString(")") || tok2->str() == MatchCompiler::makeConstString("]")) {
                        if (par == 0)
                            break;
                        --par;
                    } else if (par == 0 && isLowerThanLogicalAnd(tok2) && (andAnd || tok2->str() != MatchCompiler::makeConstString("||")))
                        break;
                }
                if (tok2) {
                    eraseTokens(tok, tok2);
                    ret = true;
                }
                continue;
            }

            if (tok->str() == MatchCompiler::makeConstString("0")) {
                if ((match86(tok->previous()) && isLowerThanMulDiv(tok->next())) ||
                    (match87(tok->previous()) && isLowerThanXor(tok->next()))) {
                    tok = tok->previous();
                    if (match88(tok->tokAt(-4)) &&
                        tok->strAt(-3) == tok->previous()->str()) {
                        tok = tok->tokAt(-4);
                        tok->deleteNext(5);
                    } else {
                        tok = tok->previous();
                        tok->deleteNext(2);
                    }
                    ret = true;
                } else if (match89(tok->previous()) ||
                           match90(tok->previous())) {
                    tok = tok->previous();
                    tok->deleteNext(2);
                    ret = true;
                } else if (match91(tok->previous()) ||
                           match92(tok->previous()) ||
                           match93(tok->previous()) ||
                           match94(tok->previous())) {
                    tok->deleteNext();
                    if (tok->next()->str() == MatchCompiler::makeConstString("("))
                        eraseTokens(tok, tok->next()->link());
                    tok->deleteNext();
                    ret = true;
                } else if (match95(tok->previous()) ||
                           match96(tok->previous())) {
                    tok->deleteNext();
                    tok->deleteNext();
                    if (tok->next()->str() == MatchCompiler::makeConstString("("))
                        eraseTokens(tok, tok->next()->link());
                    tok->deleteNext();
                    ret = true;
                }
            }

            if (tok->str() == MatchCompiler::makeConstString("1")) {
                if (match97(tok->previous()) ||
                    match98(tok->previous())) {
                    tok->deleteNext();
                    if (tok->next()->str() == MatchCompiler::makeConstString("("))
                        eraseTokens(tok, tok->next()->link());
                    tok->deleteNext();
                    ret = true;
                } else if (match99(tok->previous()) ||
                           match100(tok->previous())) {
                    tok->deleteNext();
                    tok->deleteNext();
                    if (tok->next()->str() == MatchCompiler::makeConstString("("))
                        eraseTokens(tok, tok->next()->link());
                    tok->deleteNext();
                    ret = true;
                }
            }

            if (match101(tok->tokAt(-2)) || match102(tok->previous())) {
                tok = tok->previous();
                if (tok->str() == MatchCompiler::makeConstString("*"))
                    tok = tok->previous();
                tok->deleteNext(2);
                ret = true;
            }

            // Remove parentheses around number..
            if (match103(tok->tokAt(-2)) && tok->strAt(-2) != MatchCompiler::makeConstString(">")) {
                tok = tok->previous();
                tok->deleteThis();
                tok->deleteNext();
                ret = true;
            }

            if (match104(tok->previous()) ||
                match105(tok->previous())) {
                tok = tok->previous();
                if (match106(tok))
                    tok = tok->previous();
                tok->deleteNext(2);
                ret = true;
            }

            if (match107(tok) &&
                MathLib::isInt(tok->str()) &&
                MathLib::isInt(tok->strAt(2))) {
                if (match108(tok->previous()) && match109(tok->tokAt(3))) {
                    const MathLib::bigint op1(MathLib::toLongNumber(tok->str()));
                    const std::string &cmp(tok->next()->str());
                    const MathLib::bigint op2(MathLib::toLongNumber(tok->strAt(2)));

                    std::string result;

                    if (cmp == MatchCompiler::makeConstString("=="))
                        result = (op1 == op2) ? "1" : "0";
                    else if (cmp == MatchCompiler::makeConstString("!="))
                        result = (op1 != op2) ? "1" : "0";
                    else if (cmp == MatchCompiler::makeConstString("<="))
                        result = (op1 <= op2) ? "1" : "0";
                    else if (cmp == MatchCompiler::makeConstString(">="))
                        result = (op1 >= op2) ? "1" : "0";
                    else if (cmp == MatchCompiler::makeConstString("<"))
                        result = (op1 < op2) ? "1" : "0";
                    else
                        result = (op1 > op2) ? "1" : "0";

                    tok->str(result);
                    tok->deleteNext(2);
                    ret = true;
                    tok = tok->previous();
                }
            }
        }
    }
    return ret;
}

const Token * TemplateSimplifier::getTemplateParametersInDeclaration(
    const Token * tok,
    std::vector<const Token *> & typeParametersInDeclaration)
{
    typeParametersInDeclaration.clear();
    for (; tok && tok->str() != MatchCompiler::makeConstString(">"); tok = tok->next()) {
        if (match110(tok))
            typeParametersInDeclaration.push_back(tok);
    }
    return tok;
}

bool TemplateSimplifier::matchSpecialization(
    const Token *templateDeclarationNameToken,
    const Token *templateInstantiationNameToken,
    const std::list<const Token *> & specializations)
{
    // Is there a matching specialization?
    for (std::list<const Token *>::const_iterator it = specializations.begin(); it != specializations.end(); ++it) {
        if (!match42(*it))
            continue;
        const Token *startToken = (*it);
        while (startToken->previous() && !match10(startToken->previous()))
            startToken = startToken->previous();
        if (!match28(startToken))
            continue;
        std::vector<const Token *> templateParameters;
        getTemplateParametersInDeclaration(startToken->tokAt(2), templateParameters);

        const Token *instToken = templateInstantiationNameToken->tokAt(2);
        const Token *declToken = (*it)->tokAt(2);
        const Token * const endToken = (*it)->next()->findClosingBracket();
        while (declToken != endToken) {
            if (declToken->str() != instToken->str()) {
                int nr = 0;
                while (nr < templateParameters.size() && templateParameters[nr]->str() != declToken->str())
                    ++nr;

                if (nr == templateParameters.size())
                    break;
            }
            declToken = declToken->next();
            instToken = instToken->next();
        }

        if (declToken == endToken && instToken->str() == MatchCompiler::makeConstString(">")) {
            // specialization matches.
            return templateDeclarationNameToken == *it;
        }
    }

    // No specialization matches. Return true if the declaration is not a specialization.
    return match111(templateDeclarationNameToken);
}

bool TemplateSimplifier::simplifyTemplateInstantiations(
    const TokenAndName &templateDeclaration,
    const std::list<const Token *> &specializations,
    const std::time_t maxtime,
    std::set<std::string> &expandedtemplates)
{
    // this variable is not used at the moment. The intention was to
    // allow continuous instantiations until all templates has been expanded
    //bool done = false;

    // Contains tokens such as "T"
    std::vector<const Token *> typeParametersInDeclaration;
    const Token * const tok = getTemplateParametersInDeclaration(templateDeclaration.token->tokAt(2), typeParametersInDeclaration);

    // bail out if the end of the file was reached
    if (!tok)
        return false;

    const bool printDebug = mSettings->debugwarnings;

    // get the position of the template name
    const int namepos = getTemplateNamePosition(tok);
    if (namepos == -1) {
        // debug message that we bail out..
        if (printDebug && mErrorLogger) {
            const std::list<const Token *> callstack(1, tok);
            mErrorLogger->reportErr(ErrorLogger::ErrorMessage(callstack, &mTokenList, Severity::debug, "debug", "simplifyTemplates: bailing out", false));
        }
        return false;
    }

    const bool isfunc(tok->strAt(namepos + 1) == MatchCompiler::makeConstString("("));

    // locate template usage..
    std::string::size_type numberOfTemplateInstantiations = mTemplateInstantiations.size();
    unsigned int recursiveCount = 0;

    bool instantiated = false;

    for (const TokenAndName &instantiation : mTemplateInstantiations) {
        if (numberOfTemplateInstantiations != mTemplateInstantiations.size()) {
            numberOfTemplateInstantiations = mTemplateInstantiations.size();
            simplifyCalculations();
            ++recursiveCount;
            if (recursiveCount > 100) {
                // bail out..
                break;
            }
        }

        // already simplified
        if (!match42(instantiation.token))
            continue;

        if (instantiation.name != templateDeclaration.name)
            continue;

        if (!matchSpecialization(tok->tokAt(namepos), instantiation.token, specializations))
            continue;

        Token * const tok2 = instantiation.token;
        if (mErrorLogger && !mTokenList.getFiles().empty())
            mErrorLogger->reportProgress(mTokenList.getFiles()[0], "TemplateSimplifier::simplifyTemplateInstantiations()", tok2->progressValue());
#ifdef MAXTIME
        if (std::time(0) > maxtime)
            return false;
#else
        (void)maxtime;
#endif
        assert(mTokenList.validateToken(tok2)); // that assertion fails on examples from #6021

        const Token *startToken = tok2;
        while (match35(startToken->tokAt(-2)))
            startToken = startToken->tokAt(-2);

        if (match112(startToken->previous()) &&
            !instantiateMatch(tok2, typeParametersInDeclaration.size(), isfunc ? "(" : "*| %name%"))
            continue;

        // New type..
        std::vector<const Token *> typesUsedInTemplateInstantiation;
        std::string typeForNewName;
        std::list<std::string> typeStringsUsedInTemplateInstantiation;
        unsigned int indentlevel = 0;
        for (const Token *tok3 = tok2->tokAt(2); tok3 && (indentlevel > 0 || tok3->str() != MatchCompiler::makeConstString(">")); tok3 = tok3->next()) {
            // #2648 - unhandled parentheses => bail out
            // #2721 - unhandled [ => bail out
            if (match25(tok3)) {
                typeForNewName.clear();
                break;
            }
            if (!tok3->next()) {
                typeForNewName.clear();
                break;
            }
            if (match113(tok3->tokAt(-2)) && templateParameters(tok3) > 0)
                ++indentlevel;
            else if (indentlevel > 0 && match114(tok3))
                --indentlevel;
            if (indentlevel == 0 && match115(tok3->previous()))
                typesUsedInTemplateInstantiation.push_back(tok3);
            const bool constconst = tok3->str() == MatchCompiler::makeConstString("const") && tok3->strAt(1) == MatchCompiler::makeConstString("const");
            if (!constconst) {
                typeStringsUsedInTemplateInstantiation.push_back(tok3->str());
            }
            // add additional type information
            if (!constconst && !match116(tok3)) {
                if (tok3->isUnsigned())
                    typeForNewName += "unsigned";
                else if (tok3->isSigned())
                    typeForNewName += "signed";
                if (tok3->isLong())
                    typeForNewName += "long";
                if (!typeForNewName.empty())
                    typeForNewName += ' ';
                typeForNewName += tok3->str();
            }
        }

        if (typeForNewName.empty() || typeParametersInDeclaration.size() != typesUsedInTemplateInstantiation.size()) {
            if (printDebug && mErrorLogger) {
                std::list<const Token *> callstack(1, tok2);
                mErrorLogger->reportErr(ErrorLogger::ErrorMessage(callstack, &mTokenList, Severity::debug, "debug",
                                        "Failed to instantiate template \"" + instantiation.name + "\". The checking continues anyway.", false));
            }
            if (typeForNewName.empty())
                continue;
            break;
        }

        // New classname/funcname..
        const std::string newName(templateDeclaration.name + " < " + typeForNewName + " >");

        if (expandedtemplates.find(newName) == expandedtemplates.end()) {
            expandedtemplates.insert(newName);
            expandTemplate(tok, instantiation.name, typeParametersInDeclaration, newName, typesUsedInTemplateInstantiation);
            instantiated = true;
        }

        // Replace all these template usages..
        replaceTemplateUsage(tok2, instantiation.name, typeStringsUsedInTemplateInstantiation, newName, typesUsedInTemplateInstantiation);
    }

    // Template has been instantiated .. then remove the template declaration
    return instantiated;
}

static bool matchTemplateParameters(const Token *nameTok, const std::list<std::string> &strings)
{
    std::list<std::string>::const_iterator it = strings.begin();
    const Token *tok = nameTok->tokAt(2);
    while (tok && it != strings.end() && *it == tok->str()) {
        tok = tok->next();
        ++it;
    }
    return it == strings.end() && tok && tok->str() == MatchCompiler::makeConstString(">");
}

void TemplateSimplifier::replaceTemplateUsage(Token * const instantiationToken,
        const std::string &templateName,
        const std::list<std::string> &typeStringsUsedInTemplateInstantiation,
        const std::string &newName,
        const std::vector<const Token *> &typesUsedInTemplateInstantiation)
{
    std::list<ScopeInfo2> scopeInfo;
    std::list< std::pair<Token *, Token *> > removeTokens;
    for (Token *nameTok = instantiationToken; nameTok; nameTok = nameTok->next()) {
        if (match36(nameTok)) {
            setScopeInfo(nameTok, &scopeInfo);
            continue;
        }
        if (!match42(nameTok))
            continue;
        if (!matchTemplateParameters(nameTok, typeStringsUsedInTemplateInstantiation))
            continue;

        // FIXME Proper name matching
        const std::string lastName(templateName.find(' ') == std::string::npos ? templateName : templateName.substr(templateName.rfind(' ') + 1));
        if (lastName != nameTok->str())
            continue;

        // match parameters
        Token * tok2 = nameTok->tokAt(2);
        unsigned int typeCountInInstantiation = 1U; // There is always at least one type
        const Token *typetok = (!typesUsedInTemplateInstantiation.empty()) ? typesUsedInTemplateInstantiation[0] : nullptr;
        unsigned int indentlevel2 = 0;  // indentlevel for tokgt
        while (tok2 && (indentlevel2 > 0 || tok2->str() != MatchCompiler::makeConstString(">"))) {
            if (tok2->str() == MatchCompiler::makeConstString("<") && templateParameters(tok2) > 0)
                ++indentlevel2;
            else if (indentlevel2 > 0 && match114(tok2))
                --indentlevel2;
            else if (indentlevel2 == 0) {
                if (tok2->str() != MatchCompiler::makeConstString(",")) {
                    if (!typetok ||
                        tok2->isUnsigned() != typetok->isUnsigned() ||
                        tok2->isSigned() != typetok->isSigned() ||
                        tok2->isLong() != typetok->isLong()) {
                        break;
                    }

                    typetok = typetok->next();
                } else {
                    if (typeCountInInstantiation < typesUsedInTemplateInstantiation.size())
                        typetok = typesUsedInTemplateInstantiation[typeCountInInstantiation++];
                    else
                        typetok = nullptr;
                }
            }
            tok2 = tok2->next();
        }

        if (!tok2)
            break;

        // matching template usage => replace tokens..
        // Foo < int >  =>  Foo<int>
        if (tok2->str() == MatchCompiler::makeConstString(">") && typeCountInInstantiation == typesUsedInTemplateInstantiation.size()) {
            const Token * const nameTok1 = nameTok;
            while (match35(nameTok->tokAt(-2)))
                nameTok = nameTok->tokAt(-2);
            nameTok->str(newName);
            for (std::list<TokenAndName>::iterator it = mTemplateInstantiations.begin(); it != mTemplateInstantiations.end(); ++it) {
                if (it->token == nameTok1)
                    it->token = nameTok;
            }
            for (Token *tok = nameTok1->next(); tok != tok2; tok = tok->next()) {
                if (tok->isName()) {
                    std::list<TokenAndName>::iterator ti;
                    for (ti = mTemplateInstantiations.begin(); ti != mTemplateInstantiations.end();) {
                        if (ti->token == tok)
                            mTemplateInstantiations.erase(ti++);
                        else
                            ++ti;
                    }
                }
            }
            removeTokens.emplace_back(nameTok, tok2->next());
        }

        nameTok = tok2;
    }
    while (!removeTokens.empty()) {
        eraseTokens(removeTokens.back().first, removeTokens.back().second);
        removeTokens.pop_back();
    }
}


void TemplateSimplifier::simplifyTemplates(
    const std::time_t maxtime,
    bool &codeWithTemplates
)
{
    std::set<std::string> expandedtemplates(expandSpecialized());

    if (getTemplateDeclarations(codeWithTemplates).empty())
        return;

    // There are templates..
    // Remove "typename" unless used in template arguments..
    for (Token *tok = mTokenList.front(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("typename"))
            tok->deleteThis();

        if (match28(tok)) {
            while (tok && tok->str() != MatchCompiler::makeConstString(">"))
                tok = tok->next();
            if (!tok)
                break;
        }
    }

    mTemplateDeclarations = getTemplateDeclarations(codeWithTemplates);

    // Locate possible instantiations of templates..
    getTemplateInstantiations();

    // No template instantiations? Then return.
    if (mTemplateInstantiations.empty())
        return;

    // Template arguments with default values
    useDefaultArgumentValues();

    simplifyTemplateAliases();

    // expand templates
    //bool done = false;
    //while (!done)
    {
        //done = true;
        for (std::list<TokenAndName>::reverse_iterator iter1 = mTemplateDeclarations.rbegin(); iter1 != mTemplateDeclarations.rend(); ++iter1) {
            // get specializations..
            std::list<const Token *> specializations;
            for (std::list<TokenAndName>::const_iterator iter2 = mTemplateDeclarations.begin(); iter2 != mTemplateDeclarations.end(); ++iter2) {
                if (iter1->name == iter2->name) {
                    const Token *tok = iter2->token->next()->findClosingBracket();
                    const int namepos = getTemplateNamePosition(tok);
                    if (namepos > 0)
                        specializations.push_back(tok->tokAt(namepos));
                }
            }

            const bool instantiated = simplifyTemplateInstantiations(
                                          *iter1,
                                          specializations,
                                          maxtime,
                                          expandedtemplates);
            if (instantiated)
                mInstantiatedTemplates.push_back(*iter1);
        }

        for (std::list<TokenAndName>::const_iterator it = mInstantiatedTemplates.begin(); it != mInstantiatedTemplates.end(); ++it) {
            std::list<TokenAndName>::iterator decl;
            for (decl = mTemplateDeclarations.begin(); decl != mTemplateDeclarations.end(); ++decl) {
                if (decl->token == it->token)
                    break;
            }
            if (decl != mTemplateDeclarations.end()) {
                mTemplateDeclarations.erase(decl);
                removeTemplate(it->token);
            }
        }

        // remove out of line member functions
        while (!mMemberFunctionsToDelete.empty()) {
            removeTemplate(mMemberFunctionsToDelete.begin()->token);
            mTemplateDeclarations.remove(mMemberFunctionsToDelete.front());
            mMemberFunctionsToDelete.erase(mMemberFunctionsToDelete.begin());
        }
    }
}

void TemplateSimplifier::syntaxError(const Token *tok)
{
    throw InternalError(tok, "syntax error", InternalError::SYNTAX);
}
