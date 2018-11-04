#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: = 0 ;
static bool match1(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %var% = %var% !!;
static bool match2(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString(";"))
        return false;
    return true;
}
// pattern: %var% %assign%
static bool match3(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: ( const| struct|union| %type% * ) ( (
static bool match4(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
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
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ( [(<] const| struct|union| %type% *| [>)]
static bool match5(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("(<", tok->str()[0]))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(">)", tok->str()[0]))
        return false;
    return true;
}
// pattern: (| &| %name%
static bool match6(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: < const| struct|union| %type% *| > ( &| %name%
static bool match7(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %var% .
static bool match8(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: struct|union
static bool match9(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("struct")) || (tok->str()==MatchCompiler::makeConstString("union"))))
        return false;
    return true;
}
// pattern: %cop% %var%
static bool match10(const Token* tok) {
    if (!tok || !tok->isConstOp())
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: %name% ?
static bool match11(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("?")))
        return false;
    return true;
}
// pattern: %name% .
static bool match12(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: %name% = %name% ;
static bool match13(const Token* tok) {
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
// pattern: . %name%
static bool match14(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: [+:]
static bool match15(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("+:", tok->str()[0]))
        return false;
    return true;
}
// pattern: lock_guard|unique_lock|shared_ptr|unique_ptr|auto_ptr|shared_lock
static bool match16(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("lock_guard")) || (tok->str()==MatchCompiler::makeConstString("unique_lock")) || (tok->str()==MatchCompiler::makeConstString("shared_ptr")) || (tok->str()==MatchCompiler::makeConstString("unique_ptr")) || (tok->str()==MatchCompiler::makeConstString("auto_ptr")) || (tok->str()==MatchCompiler::makeConstString("shared_lock"))))
        return false;
    return true;
}
// pattern: * %var% ) (
static bool match17(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% [ %var% ]
static bool match18(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: = {
static bool match19(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: asm ( %str% )
static bool match20(const Token* tok) {
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
    return true;
}
// pattern: goto|break
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("goto")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        return false;
    return true;
}
// pattern: %name% (
static bool match22(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: if|for|while|switch
static bool match23(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("switch"))))
        return false;
    return true;
}
// pattern: ) {
static bool match24(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: for_each (
static bool match25(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("for_each")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ;
static bool match26(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: =
static bool match27(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: [;{}]
static bool match28(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    return true;
}
// pattern: ( %name% )
static bool match29(const Token* tok) {
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
// pattern: = %var% ;
static bool match30(const Token* tok) {
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
// pattern: [;({=]
static bool match31(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";({=", tok->str()[0]))
        return false;
    return true;
}
// pattern: free|g_free|kfree|vfree ( %var% )
static bool match32(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("free")) || (tok->str()==MatchCompiler::makeConstString("g_free")) || (tok->str()==MatchCompiler::makeConstString("kfree")) || (tok->str()==MatchCompiler::makeConstString("vfree"))))
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
// pattern: delete %var% ;
static bool match33(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: delete [ ] %var% ;
static bool match34(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: return|throw
static bool match35(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: *| ++|--| %name% ++|--| %assign%
static bool match36(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: *| ( const| %type% *| ) %name% %assign%
static bool match37(const Token* tok) {
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
        return false;
    return true;
}
// pattern: ( const| %type% *| ) %name% %assign%
static bool match38(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("const"))))
        tok = tok->next();
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !tok->isAssignmentOp())
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
// pattern: %assign% %name%
static bool match40(const Token* tok) {
    if (!tok || !tok->isAssignmentOp())
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% = new|malloc|calloc|kmalloc|kzalloc|kcalloc|strdup|strndup|vmalloc|g_new0|g_try_new|g_new|g_malloc|g_malloc0|g_try_malloc|g_try_malloc0|g_strdup|g_strndup|g_strdup_printf
static bool match41(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("new")) || (tok->str()==MatchCompiler::makeConstString("malloc")) || (tok->str()==MatchCompiler::makeConstString("calloc")) || (tok->str()==MatchCompiler::makeConstString("kmalloc")) || (tok->str()==MatchCompiler::makeConstString("kzalloc")) || (tok->str()==MatchCompiler::makeConstString("kcalloc")) || (tok->str()==MatchCompiler::makeConstString("strdup")) || (tok->str()==MatchCompiler::makeConstString("strndup")) || (tok->str()==MatchCompiler::makeConstString("vmalloc")) || (tok->str()==MatchCompiler::makeConstString("g_new0")) || (tok->str()==MatchCompiler::makeConstString("g_try_new")) || (tok->str()==MatchCompiler::makeConstString("g_new")) || (tok->str()==MatchCompiler::makeConstString("g_malloc")) || (tok->str()==MatchCompiler::makeConstString("g_malloc0")) || (tok->str()==MatchCompiler::makeConstString("g_try_malloc")) || (tok->str()==MatchCompiler::makeConstString("g_try_malloc0")) || (tok->str()==MatchCompiler::makeConstString("g_strdup")) || (tok->str()==MatchCompiler::makeConstString("g_strndup")) || (tok->str()==MatchCompiler::makeConstString("g_strdup_printf"))))
        return false;
    return true;
}
// pattern: ( nothrow )
static bool match42(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("nothrow")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: ( std :: nothrow )
static bool match43(const Token* tok) {
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
    return true;
}
// pattern: %varid% .
static bool match44(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: %name% ;
static bool match45(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %name% .|[
static bool match46(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %name% [
static bool match47(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: * (
static bool match48(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) =
static bool match49(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: = new|malloc|calloc|kmalloc|kzalloc|kcalloc|strdup|strndup|vmalloc|g_new0|g_try_new|g_new|g_malloc|g_malloc0|g_try_malloc|g_try_malloc0|g_strdup|g_strndup|g_strdup_printf
static bool match50(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("new")) || (tok->str()==MatchCompiler::makeConstString("malloc")) || (tok->str()==MatchCompiler::makeConstString("calloc")) || (tok->str()==MatchCompiler::makeConstString("kmalloc")) || (tok->str()==MatchCompiler::makeConstString("kzalloc")) || (tok->str()==MatchCompiler::makeConstString("kcalloc")) || (tok->str()==MatchCompiler::makeConstString("strdup")) || (tok->str()==MatchCompiler::makeConstString("strndup")) || (tok->str()==MatchCompiler::makeConstString("vmalloc")) || (tok->str()==MatchCompiler::makeConstString("g_new0")) || (tok->str()==MatchCompiler::makeConstString("g_try_new")) || (tok->str()==MatchCompiler::makeConstString("g_new")) || (tok->str()==MatchCompiler::makeConstString("g_malloc")) || (tok->str()==MatchCompiler::makeConstString("g_malloc0")) || (tok->str()==MatchCompiler::makeConstString("g_try_malloc")) || (tok->str()==MatchCompiler::makeConstString("g_try_malloc0")) || (tok->str()==MatchCompiler::makeConstString("g_strdup")) || (tok->str()==MatchCompiler::makeConstString("g_strndup")) || (tok->str()==MatchCompiler::makeConstString("g_strdup_printf"))))
        return false;
    return true;
}
// pattern: [;{}] %var% <<
static bool match51(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<<")))
        return false;
    return true;
}
// pattern: & %var%
static bool match52(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: >>|>>= %name%
static bool match53(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString(">>="))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %var% >>|&
static bool match54(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">>")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: [{};:]
static bool match55(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{};:", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] %var% [
static bool match56(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: [(,] %var% [,)]
static bool match57(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] & %var% [,)]
static bool match58(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] (
static bool match59(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) %var% [,)]
static bool match60(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: [(,] *| %var% =
static bool match61(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("*"))))
        tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: std :: ref ( %var% )
static bool match62(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("ref")))
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
// pattern: [{,] %var% [,}]
static bool match63(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("{,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",}", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% ( %var% [,)]
static bool match64(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %var% ;
static bool match65(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}:]
static bool match66(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}:", tok->str()[0]))
        return false;
    return true;
}
// pattern: sizeof (
template<class T> static T * findmatch67(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        continue;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        continue;
    return start_tok;
    }
    return NULL;
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
#include "checkunusedvar.h"

#include "astutils.h"
#include "errorlogger.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "valueflow.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <list>
#include <set>
#include <utility>
#include <vector>
//---------------------------------------------------------------------------

// Register this check class (by creating a static instance of it)
namespace {
    CheckUnusedVar instance;
}

static const struct CWE CWE563(563U);   // Assignment to Variable without Use ('Unused Variable')
static const struct CWE CWE665(665U);   // Improper Initialization


/**
 * @brief This class is used create a list of variables within a function.
 */
class Variables {
public:
    enum VariableType { standard, array, pointer, reference, pointerArray, referenceArray, pointerPointer, none };

    /** Store information about variable usage */
    class VariableUsage {
    public:
        explicit VariableUsage(const Variable *var = nullptr,
                               VariableType type = standard,
                               bool read = false,
                               bool write = false,
                               bool modified = false,
                               bool allocateMemory = false) :
            _var(var),
            _lastAccess(var ? var->nameToken() : nullptr),
            mType(type),
            _read(read),
            _write(write),
            _modified(modified),
            _allocateMemory(allocateMemory) {
        }

        /** variable is used.. set both read+write */
        void use(std::list<std::set<unsigned int> > & varReadInScope) {
            varReadInScope.back().insert(_var->declarationId());
            _read = true;
            _write = true;
        }

        /** is variable unused? */
        bool unused() const {
            return (!_read && !_write);
        }

        std::set<unsigned int> _aliases;
        std::set<const Scope*> _assignments;

        const Variable* _var;
        const Token* _lastAccess;
        VariableType mType;
        bool _read;
        bool _write;
        bool _modified; // read/modify/write
        bool _allocateMemory;
    };

    class ScopeGuard {
    public:
        ScopeGuard(Variables & guarded,
                   bool insideLoop)
            :mGuarded(guarded),
             mInsideLoop(insideLoop) {
            mGuarded.enterScope();
        }

        ~ScopeGuard() {
            mGuarded.leaveScope(mInsideLoop);
        }

    private:
        /** No implementation */
        ScopeGuard();
        ScopeGuard& operator=(const ScopeGuard &);

        Variables & mGuarded;
        bool mInsideLoop;
    };

    void clear() {
        mVarUsage.clear();
    }
    const std::map<unsigned int, VariableUsage> &varUsage() const {
        return mVarUsage;
    }
    void addVar(const Variable *var, VariableType type, bool write_);
    void allocateMemory(unsigned int varid, const Token* tok);
    void read(unsigned int varid, const Token* tok);
    void readAliases(unsigned int varid, const Token* tok);
    void readAll(unsigned int varid, const Token* tok);
    void write(unsigned int varid, const Token* tok);
    void writeAliases(unsigned int varid, const Token* tok);
    void writeAll(unsigned int varid, const Token* tok);
    void use(unsigned int varid, const Token* tok);
    void modified(unsigned int varid, const Token* tok);
    VariableUsage *find(unsigned int varid);
    void alias(unsigned int varid1, unsigned int varid2, bool replace);
    void erase(unsigned int varid) {
        mVarUsage.erase(varid);
    }
    void eraseAliases(unsigned int varid);
    void eraseAll(unsigned int varid);
    void clearAliases(unsigned int varid);

    ScopeGuard newScope(bool insideLoop) {
        return ScopeGuard(*this, insideLoop);
    }

private:
    void enterScope();
    void leaveScope(bool insideLoop);

    std::map<unsigned int, VariableUsage> mVarUsage;
    std::list<std::set<unsigned int> > mVarAddedInScope;
    std::list<std::set<unsigned int> > mVarReadInScope;
};


/**
 * Alias the 2 given variables. Either replace the existing aliases if
 * they exist or merge them.  You would replace an existing alias when this
 * assignment is in the same scope as the previous assignment.  You might
 * merge the aliases when this assignment is in a different scope from the
 * previous assignment depending on the relationship of the 2 scopes.
 */
void Variables::alias(unsigned int varid1, unsigned int varid2, bool replace)
{
    VariableUsage *var1 = find(varid1);
    VariableUsage *var2 = find(varid2);

    if (!var1 || !var2)
        return;

    // alias to self
    if (varid1 == varid2) {
        var1->use(mVarReadInScope);
        return;
    }

    if (replace) {
        // remove var1 from all aliases
        for (std::set<unsigned int>::const_iterator i = var1->_aliases.begin(); i != var1->_aliases.end(); ++i) {
            VariableUsage *temp = find(*i);

            if (temp)
                temp->_aliases.erase(var1->_var->declarationId());
        }

        // remove all aliases from var1
        var1->_aliases.clear();
    }

    // var1 gets all var2s aliases
    for (std::set<unsigned int>::const_iterator i = var2->_aliases.begin(); i != var2->_aliases.end(); ++i) {
        if (*i != varid1)
            var1->_aliases.insert(*i);
    }

    // var2 is an alias of var1
    var2->_aliases.insert(varid1);
    var1->_aliases.insert(varid2);

    if (var2->mType == Variables::pointer) {
        mVarReadInScope.back().insert(varid2);
        var2->_read = true;
    }
}

void Variables::clearAliases(unsigned int varid)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        // remove usage from all aliases
        std::set<unsigned int>::const_iterator i;

        for (i = usage->_aliases.begin(); i != usage->_aliases.end(); ++i) {
            VariableUsage *temp = find(*i);

            if (temp)
                temp->_aliases.erase(usage->_var->declarationId());
        }

        // remove all aliases from usage
        usage->_aliases.clear();
    }
}

void Variables::eraseAliases(unsigned int varid)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        for (std::set<unsigned int>::const_iterator aliases = usage->_aliases.begin(); aliases != usage->_aliases.end(); ++aliases)
            erase(*aliases);
    }
}

void Variables::eraseAll(unsigned int varid)
{
    eraseAliases(varid);
    erase(varid);
}

void Variables::addVar(const Variable *var,
                       VariableType type,
                       bool write_)
{
    if (var->declarationId() > 0) {
        mVarAddedInScope.back().insert(var->declarationId());
        mVarUsage.insert(std::make_pair(var->declarationId(), VariableUsage(var, type, false, write_, false)));
    }
}

void Variables::allocateMemory(unsigned int varid, const Token* tok)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        usage->_allocateMemory = true;
        usage->_lastAccess = tok;
    }
}

void Variables::read(unsigned int varid, const Token* tok)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        mVarReadInScope.back().insert(varid);
        usage->_read = true;
        if (tok)
            usage->_lastAccess = tok;
    }
}

void Variables::readAliases(unsigned int varid, const Token* tok)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        for (std::set<unsigned int>::iterator aliases = usage->_aliases.begin(); aliases != usage->_aliases.end(); ++aliases) {
            VariableUsage *aliased = find(*aliases);

            if (aliased) {
                mVarReadInScope.back().insert(*aliases);
                aliased->_read = true;
                aliased->_lastAccess = tok;
            }
        }
    }
}

void Variables::readAll(unsigned int varid, const Token* tok)
{
    read(varid, tok);
    readAliases(varid, tok);
}

void Variables::write(unsigned int varid, const Token* tok)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        usage->_write = true;
        if (!usage->_var->isStatic() && !match1(tok->next()))
            usage->_read = false;
        usage->_lastAccess = tok;
    }
}

void Variables::writeAliases(unsigned int varid, const Token* tok)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        for (std::set<unsigned int>::const_iterator aliases = usage->_aliases.begin(); aliases != usage->_aliases.end(); ++aliases) {
            VariableUsage *aliased = find(*aliases);

            if (aliased) {
                aliased->_write = true;
                aliased->_lastAccess = tok;
            }
        }
    }
}

void Variables::writeAll(unsigned int varid, const Token* tok)
{
    write(varid, tok);
    writeAliases(varid, tok);
}

void Variables::use(unsigned int varid, const Token* tok)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        usage->use(mVarReadInScope);
        usage->_lastAccess = tok;

        for (std::set<unsigned int>::const_iterator aliases = usage->_aliases.begin(); aliases != usage->_aliases.end(); ++aliases) {
            VariableUsage *aliased = find(*aliases);

            if (aliased) {
                aliased->use(mVarReadInScope);
                aliased->_lastAccess = tok;
            }
        }
    }
}

void Variables::modified(unsigned int varid, const Token* tok)
{
    VariableUsage *usage = find(varid);

    if (usage) {
        if (!usage->_var->isStatic())
            usage->_read = false;
        usage->_modified = true;
        usage->_lastAccess = tok;

        for (std::set<unsigned int>::const_iterator aliases = usage->_aliases.begin(); aliases != usage->_aliases.end(); ++aliases) {
            VariableUsage *aliased = find(*aliases);

            if (aliased) {
                aliased->_modified = true;
                aliased->_lastAccess = tok;
            }
        }
    }
}

Variables::VariableUsage *Variables::find(unsigned int varid)
{
    if (varid) {
        std::map<unsigned int, VariableUsage>::iterator i = mVarUsage.find(varid);
        if (i != mVarUsage.end())
            return &i->second;
    }
    return nullptr;
}

void Variables::enterScope()
{
    mVarAddedInScope.emplace_back();
    mVarReadInScope.emplace_back();
}

void Variables::leaveScope(bool insideLoop)
{
    if (insideLoop) {
        // read variables are read again in subsequent run through loop
        std::set<unsigned int> const & currentVarReadInScope = mVarReadInScope.back();
        for (std::set<unsigned int>::const_iterator readIter = currentVarReadInScope.begin();
             readIter != currentVarReadInScope.end();
             ++readIter) {
            read(*readIter, nullptr);
        }
    }

    std::list<std::set<unsigned int> >::reverse_iterator reverseReadIter = mVarReadInScope.rbegin();
    ++reverseReadIter;
    if (reverseReadIter != mVarReadInScope.rend()) {
        // Transfer read variables into previous scope

        std::set<unsigned int> const & currentVarAddedInScope = mVarAddedInScope.back();
        std::set<unsigned int>  & currentVarReadInScope = mVarReadInScope.back();
        for (std::set<unsigned int>::const_iterator addedIter = currentVarAddedInScope.begin();
             addedIter != currentVarAddedInScope.end();
             ++addedIter) {
            currentVarReadInScope.erase(*addedIter);
        }
        std::set<unsigned int> & previousVarReadInScope = *reverseReadIter;
        previousVarReadInScope.insert(currentVarReadInScope.begin(),
                                      currentVarReadInScope.end());
    }
    mVarReadInScope.pop_back();
    mVarAddedInScope.pop_back();
}

static const Token* doAssignment(Variables &variables, const Token *tok, bool dereference, const Scope *scope)
{
    // a = a + b;
    if (match2(tok)) {
        const Token* rhsVarTok = tok->tokAt(2);
        if (tok->varId() == rhsVarTok->varId()) {
            return rhsVarTok;
        }
    }

    if (match3(tok) && tok->strAt(1) != MatchCompiler::makeConstString("="))
        return tok->next();

    const Token* const tokOld = tok;

    // check for aliased variable
    const unsigned int varid1 = tok->varId();
    Variables::VariableUsage *var1 = variables.find(varid1);

    if (var1) {
        // jump behind '='
        tok = tok->next();
        while (!tok->isAssignmentOp()) {
            if (tok->varId())
                variables.read(tok->varId(), tok);
            tok = tok->next();
        }
        tok = tok->next();

        if (match4(tok))
            tok = tok->link()->next();

        if (match5(tok))
            tok = tok->next();

        if (match6(tok) ||
            (match7(tok->next()))) {
            bool addressOf = false;

            if (match8(tok))
                variables.use(tok->varId(), tok);   // use = read + write

            // check for C style cast
            if (tok->str() == MatchCompiler::makeConstString("(")) {
                tok = tok->next();
                if (tok->str() == MatchCompiler::makeConstString("const"))
                    tok = tok->next();

                if (match9(tok))
                    tok = tok->next();

                while ((tok->isName() && tok->varId() == 0) || (tok->str() == MatchCompiler::makeConstString("*")) || (tok->str() == MatchCompiler::makeConstString(")")))
                    tok = tok->next();

                if (tok->str() == MatchCompiler::makeConstString("&")) {
                    addressOf = true;
                    tok = tok->next();
                } else if (tok->str() == MatchCompiler::makeConstString("(")) {
                    tok = tok->next();
                    if (tok->str() == MatchCompiler::makeConstString("&")) {
                        addressOf = true;
                        tok = tok->next();
                    }
                } else if (match10(tok)) {
                    variables.read(tok->next()->varId(), tok);
                }
            }

            // check for C++ style cast
            else if (tok->str().find("cast") != std::string::npos &&
                     tok->strAt(1) == MatchCompiler::makeConstString("<")) {
                tok = tok->tokAt(2);
                if (tok->str() == MatchCompiler::makeConstString("const"))
                    tok = tok->next();

                if (match9(tok))
                    tok = tok->next();

                tok = tok->next();
                if (tok->str() == MatchCompiler::makeConstString("*"))
                    tok = tok->next();

                tok = tok->tokAt(2);
                if (!tok)
                    return tokOld;
                if (tok->str() == MatchCompiler::makeConstString("&")) {
                    addressOf = true;
                    tok = tok->next();
                }
            }

            // no cast, no ?
            else if (!match11(tok)) {
                if (tok->str() == MatchCompiler::makeConstString("&")) {
                    addressOf = true;
                    tok = tok->next();
                } else if (tok->str() == MatchCompiler::makeConstString("new"))
                    return tokOld;
            }

            // check if variable is local
            const unsigned int varid2 = tok->varId();
            const Variables::VariableUsage* var2 = variables.find(varid2);

            if (var2) { // local variable (alias or read it)
                if (var1->mType == Variables::pointer || var1->mType == Variables::pointerArray) {
                    if (dereference)
                        variables.read(varid2, tok);
                    else {
                        if (addressOf ||
                            var2->mType == Variables::array ||
                            var2->mType == Variables::pointer) {
                            bool replace = true;

                            // pointerArray => don't replace
                            if (var1->mType == Variables::pointerArray)
                                replace = false;

                            // check if variable declared in same scope
                            else if (scope == var1->_var->scope())
                                replace = true;

                            // not in same scope as declaration
                            else {
                                // no other assignment in this scope
                                if (var1->_assignments.find(scope) == var1->_assignments.end() ||
                                    scope->type == Scope::eSwitch) {
                                    // nothing to replace
                                    if (var1->_assignments.empty())
                                        replace = false;

                                    // this variable has previous assignments
                                    else {
                                        /**
                                         * @todo determine if existing aliases should be replaced or merged
                                         */

                                        replace = false;
                                    }
                                }

                                // assignment in this scope
                                else {
                                    // replace when only one other assignment, merge them otherwise
                                    replace = (var1->_assignments.size() == 1);
                                }
                            }

                            variables.alias(varid1, varid2, replace);
                        } else if (tok->strAt(1) == MatchCompiler::makeConstString("?")) {
                            if (var2->mType == Variables::reference)
                                variables.readAliases(varid2, tok);
                            else
                                variables.read(varid2, tok);
                        } else {
                            variables.readAll(varid2, tok);
                        }
                    }
                } else if (var1->mType == Variables::reference) {
                    variables.alias(varid1, varid2, true);
                } else {
                    if ((var2->mType == Variables::pointer || var2->mType == Variables::pointerArray) && tok->strAt(1) == MatchCompiler::makeConstString("["))
                        variables.readAliases(varid2, tok);

                    variables.read(varid2, tok);
                }
            } else { // not a local variable (or an unsupported local variable)
                if (var1->mType == Variables::pointer && !dereference) {
                    // check if variable declaration is in this scope
                    if (var1->_var->scope() == scope) {
                        // If variable is used in RHS then "use" variable
                        for (const Token *rhs = tok; rhs && rhs->str() != MatchCompiler::makeConstString(";"); rhs = rhs->next()) {
                            if (rhs->varId() == varid1) {
                                variables.use(varid1, tok);
                                break;
                            }
                        }
                        variables.clearAliases(varid1);
                    } else {
                        // no other assignment in this scope
                        if (var1->_assignments.find(scope) == var1->_assignments.end()) {
                            /**
                             * @todo determine if existing aliases should be discarded
                             */
                        }

                        // this assignment replaces the last assignment in this scope
                        else {
                            // aliased variables in a larger scope are not supported
                            // remove all aliases
                            variables.clearAliases(varid1);
                        }
                    }
                }
            }
        } else
            tok = tokOld;

        var1->_assignments.insert(scope);
    }

    // check for alias to struct member
    // char c[10]; a.b = c;
    else if (match12(tok->tokAt(-2))) {
        const Token *rhsVarTok = tok->tokAt(2);
        if (rhsVarTok && rhsVarTok->varId()) {
            const unsigned int varid2 = rhsVarTok->varId();
            const Variables::VariableUsage *var2 = variables.find(varid2);

            // struct member aliased to local variable
            if (var2 && (var2->mType == Variables::array ||
                         var2->mType == Variables::pointer)) {
                // erase aliased variable and all variables that alias it
                // to prevent false positives
                variables.eraseAll(varid2);
            }
        }
    }

    // Possible pointer alias
    else if (match13(tok)) {
        const unsigned int varid2 = tok->tokAt(2)->varId();
        const Variables::VariableUsage *var2 = variables.find(varid2);
        if (var2 && (var2->mType == Variables::array ||
                     var2->mType == Variables::pointer)) {
            variables.use(varid2,tok);
        }
    }

    return tok;
}

static bool isPartOfClassStructUnion(const Token* tok)
{
    for (; tok; tok = tok->previous()) {
        if (tok->str() == MatchCompiler::makeConstString("}") || tok->str() == MatchCompiler::makeConstString(")"))
            tok = tok->link();
        else if (tok->str() == MatchCompiler::makeConstString("("))
            return (false);
        else if (tok->str() == MatchCompiler::makeConstString("{")) {
            return (tok->strAt(-1) == MatchCompiler::makeConstString("struct") || tok->strAt(-2) == MatchCompiler::makeConstString("struct") || tok->strAt(-1) == MatchCompiler::makeConstString("class") || tok->strAt(-2) == MatchCompiler::makeConstString("class") || tok->strAt(-1) == MatchCompiler::makeConstString("union") || tok->strAt(-2) == MatchCompiler::makeConstString("union"));
        }
    }
    return false;
}

// Skip [ .. ]
static const Token * skipBrackets(const Token *tok)
{
    while (tok && tok->str() == MatchCompiler::makeConstString("["))
        tok = tok->link()->next();
    return tok;
}


// Skip [ .. ] . x
static const Token * skipBracketsAndMembers(const Token *tok)
{
    while (tok) {
        if (tok->str() == MatchCompiler::makeConstString("["))
            tok = tok->link()->next();
        else if (match14(tok))
            tok = tok->tokAt(2);
        else
            break;
    }
    return tok;
}

static void useFunctionArgs(const Token *tok, Variables& variables)
{
    // TODO: Match function args to see if they are const or not. Assume that const data is not written.
    if (!tok)
        return;
    if (tok->str() == MatchCompiler::makeConstString(",")) {
        useFunctionArgs(tok->astOperand1(), variables);
        useFunctionArgs(tok->astOperand2(), variables);
    } else if (match15(tok) && (!tok->valueType() || tok->valueType()->pointer)) {
        useFunctionArgs(tok->astOperand1(), variables);
        useFunctionArgs(tok->astOperand2(), variables);
    } else if (tok->variable() && tok->variable()->isArray()) {
        variables.use(tok->varId(), tok);
    }
}

//---------------------------------------------------------------------------
// Usage of function variables
//---------------------------------------------------------------------------
void CheckUnusedVar::checkFunctionVariableUsage_iterateScopes(const Scope* const scope, Variables& variables, bool insideLoop)
{
    Variables::ScopeGuard scopeGuard=variables.newScope(insideLoop);

    // Find declarations if the scope is executable..
    if (scope->isExecutable()) {
        // Find declarations
        for (std::list<Variable>::const_iterator i = scope->varlist.begin(); i != scope->varlist.end(); ++i) {
            if (i->isThrow() || i->isExtern())
                continue;
            Variables::VariableType type = Variables::none;
            if (i->isArray() && (i->nameToken()->previous()->str() == MatchCompiler::makeConstString("*") || i->nameToken()->strAt(-2) == MatchCompiler::makeConstString("*")))
                type = Variables::pointerArray;
            else if (i->isArray() && i->nameToken()->previous()->str() == MatchCompiler::makeConstString("&"))
                type = Variables::referenceArray;
            else if (i->isArray())
                type = (i->dimensions().size() == 1U) ? Variables::array : Variables::pointerArray;
            else if (i->isReference())
                type = Variables::reference;
            else if (i->nameToken()->previous()->str() == MatchCompiler::makeConstString("*") && i->nameToken()->strAt(-2) == MatchCompiler::makeConstString("*"))
                type = Variables::pointerPointer;
            else if (i->isPointerToArray())
                type = Variables::pointerPointer;
            else if (i->isPointer())
                type = Variables::pointer;
            else if (mTokenizer->isC() ||
                     i->typeEndToken()->isStandardType() ||
                     isRecordTypeWithoutSideEffects(i->type()) ||
                     (i->isStlType() &&
                      !match16(i->typeStartToken()->tokAt(2))))
                type = Variables::standard;
            if (type == Variables::none || isPartOfClassStructUnion(i->typeStartToken()))
                continue;
            const Token* defValTok = i->nameToken()->next();
            if (match17(i->nameToken()->previous())) // function pointer. Jump behind parameter list.
                defValTok = defValTok->linkAt(1)->next();
            for (; defValTok; defValTok = defValTok->next()) {
                if (defValTok->str() == MatchCompiler::makeConstString("["))
                    defValTok = defValTok->link();
                else if (defValTok->str() == MatchCompiler::makeConstString("(") || defValTok->str() == MatchCompiler::makeConstString("{") || defValTok->str() == MatchCompiler::makeConstString("=") || defValTok->str() == MatchCompiler::makeConstString(":")) {
                    variables.addVar(&*i, type, true);
                    break;
                } else if (defValTok->str() == MatchCompiler::makeConstString(";") || defValTok->str() == MatchCompiler::makeConstString(",") || defValTok->str() == MatchCompiler::makeConstString(")")) {
                    variables.addVar(&*i, type, i->isStatic());
                    break;
                }
            }
            if (i->isArray() && i->isClass()) // Array of class/struct members. Initialized by ctor.
                variables.write(i->declarationId(), i->nameToken());
            if (i->isArray() && match18(i->nameToken())) // Array index variable read.
                variables.read(i->nameToken()->tokAt(2)->varId(), i->nameToken());

            if (defValTok && defValTok->next()) {
                // simple assignment "var = 123"
                if (defValTok->str() == MatchCompiler::makeConstString("=") && defValTok->next()->str() != MatchCompiler::makeConstString("{")) {
                    doAssignment(variables, i->nameToken(), false, scope);
                } else {
                    // could be "var = {...}" OR "var{...}" (since C++11)
                    const Token* tokBraceStart = nullptr;
                    if (match19(defValTok)) {
                        // "var = {...}"
                        tokBraceStart = defValTok->next();
                    } else if (defValTok->str() == MatchCompiler::makeConstString("{")) {
                        // "var{...}"
                        tokBraceStart = defValTok;
                    }
                    if (tokBraceStart) {
                        for (const Token* tok = tokBraceStart->next(); tok && tok != tokBraceStart->link(); tok = tok->next()) {
                            if (tok->varId()) {
                                // Variables used to initialize the array read.
                                variables.read(tok->varId(), i->nameToken());
                            }
                        }
                    }
                }
            }
        }
    }

    // Check variable usage
    const Token *tok;
    if (scope->type == Scope::eFunction)
        tok = scope->bodyStart->next();
    else
        tok = scope->classDef->next();
    for (; tok && tok != scope->bodyEnd; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("for") || tok->str() == MatchCompiler::makeConstString("while") || tok->str() == MatchCompiler::makeConstString("do")) {
            for (std::list<Scope*>::const_iterator i = scope->nestedList.begin(); i != scope->nestedList.end(); ++i) {
                if ((*i)->classDef == tok) { // Find associated scope
                    checkFunctionVariableUsage_iterateScopes(*i, variables, true); // Scan child scope
                    tok = (*i)->bodyStart->link();
                    break;
                }
            }
            if (!tok)
                break;
        }
        if (tok->str() == MatchCompiler::makeConstString("{") && tok != scope->bodyStart && !tok->previous()->varId()) {
            for (std::list<Scope*>::const_iterator i = scope->nestedList.begin(); i != scope->nestedList.end(); ++i) {
                if ((*i)->bodyStart == tok) { // Find associated scope
                    checkFunctionVariableUsage_iterateScopes(*i, variables, false); // Scan child scope
                    tok = tok->link();
                    break;
                }
            }
            if (!tok)
                break;
        }

        if (match20(tok)) {
            variables.clear();
            break;
        }
        if (match21(tok)) { // #4447
            variables.clear();
            break;
        }

        // templates
        if (tok->isName() && endsWith(tok->str(), '>')) {
            // TODO: This is a quick fix to handle when constants are used
            // as template parameters. Try to handle this better, perhaps
            // only remove constants.
            variables.clear();
        }

        // bailout when for_each is used
        if (match22(tok) && match24(tok->linkAt(1)) && !match23(tok)) {
            // does the name contain "for_each" or "foreach"?
            std::string nameTok;
            nameTok.resize(tok->str().size());
            std::transform(tok->str().begin(), tok->str().end(), nameTok.begin(), ::tolower);
            if (nameTok.find("foreach") != std::string::npos || nameTok.find("for_each") != std::string::npos) {
                // bailout all variables in the body that are used more than once.
                // TODO: there is no need to bailout if variable is only read or only written
                std::set<unsigned int> varid;
                const Token * const endTok = tok->linkAt(1)->linkAt(1);
                for (const Token *tok2 = endTok->link(); tok2 && tok2 != endTok; tok2 = tok2->next()) {
                    if (tok2->varId()) {
                        if (varid.find(tok2->varId()) == varid.end())
                            varid.insert(tok2->varId());
                        else
                            variables.erase(tok2->varId());
                    }
                }
            }
        }

        // C++11 std::for_each
        // No warning should be written if a variable is first read and
        // then written in the body.
        else if (mTokenizer->isCPP() && match25(tok) && match26(tok->linkAt(1))) {
            const Token *end = tok->linkAt(1);
            if (end->previous()->str() == MatchCompiler::makeConstString("}")) {
                std::set<unsigned int> readvar;
                for (const Token *body = end->linkAt(-1); body != end; body = body->next()) {
                    if (body->varId() == 0U)
                        continue;
                    if (!match27(body->next()))
                        readvar.insert(body->varId());
                    else if (readvar.find(body->varId()) != readvar.end())
                        variables.erase(body->varId());
                }
            }
        }

        else if (match28(tok->previous())) {
            for (const Token* tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                if (tok2->varId()) {
                    // Is this a variable declaration?
                    const Variable *var = tok2->variable();
                    if (!var || var->nameToken() != tok2)
                        continue;

                    // Mark template parameters used in declaration as use..
                    if (tok2->strAt(-1) == MatchCompiler::makeConstString(">")) {
                        for (const Token *tok3 = tok; tok3 != tok2; tok3 = tok3->next()) {
                            if (tok3->varId() > 0U)
                                variables.use(tok3->varId(), tok3);
                        }
                    }

                    // Skip variable declaration..
                    tok = tok2->next();
                    if (match29(tok)) // Simple initialization through copy ctor
                        tok = tok->next();
                    else if (match30(tok)) { // Simple initialization
                        tok = tok->next();
                        if (!var->isReference())
                            variables.read(tok->varId(), tok);
                    } else if (tok->str() == MatchCompiler::makeConstString("[") && match19(skipBrackets(tok))) {
                        const Token * const rhs1 = skipBrackets(tok)->next();
                        for (const Token *rhs = rhs1->link(); rhs != rhs1; rhs = rhs->previous()) {
                            if (rhs->varId())
                                variables.readAll(rhs->varId(), rhs);
                        }
                    } else if (var->typeEndToken()->str() == MatchCompiler::makeConstString(">")) // Be careful with types like std::vector
                        tok = tok->previous();
                    break;
                } else if (match31(tok2))
                    break;
            }
        }
        // Freeing memory (not considered "using" the pointer if it was also allocated in this function)
        if (match32(tok) ||
            (mTokenizer->isCPP() && (match33(tok) || match34(tok)))) {
            unsigned int varid = 0;
            if (tok->str() != MatchCompiler::makeConstString("delete")) {
                const Token *varTok = tok->tokAt(2);
                varid = varTok->varId();
                tok = varTok->next();
            } else if (tok->strAt(1) == MatchCompiler::makeConstString("[")) {
                const Token *varTok = tok->tokAt(3);
                varid = varTok->varId();
                tok = varTok;
            } else {
                varid = tok->next()->varId();
                tok = tok->next();
            }

            const Variables::VariableUsage *const var = variables.find(varid);
            if (var && !var->_allocateMemory) {
                variables.readAll(varid, tok);
            }
        }

        else if (match35(tok)) {
            for (const Token *tok2 = tok->next(); tok2; tok2 = tok2->next()) {
                if (tok2->varId())
                    variables.readAll(tok2->varId(), tok);
                else if (tok2->str() == MatchCompiler::makeConstString(";"))
                    break;
            }
        }

        // assignment
        else if (match36(tok) ||
                 match37(tok)) {
            bool dereference = false;
            bool pre = false;
            bool post = false;

            if (tok->str() == MatchCompiler::makeConstString("*")) {
                dereference = true;
                tok = tok->next();
            }

            if (match38(tok))
                tok = tok->link()->next();

            else if (tok->str() == MatchCompiler::makeConstString("("))
                tok = tok->next();

            if (tok->tokType() == Token::eIncDecOp) {
                pre = true;
                tok = tok->next();
            }

            if (tok->next()->tokType() == Token::eIncDecOp)
                post = true;

            const unsigned int varid1 = tok->varId();
            const Token * const start = tok;

            // assignment in while head..
            bool inwhile = false;
            {
                const Token *parent = tok->astParent();
                while (parent) {
                    if (match39(parent->previous())) {
                        inwhile = true;
                        break;
                    }
                    parent = parent->astParent();
                }
            }

            tok = doAssignment(variables, tok, dereference, scope);

            if (tok && tok->isAssignmentOp() && tok->str() != MatchCompiler::makeConstString("=")) {
                variables.use(varid1, tok);
                if (match40(tok)) {
                    tok = tok->next();
                    variables.read(tok->varId(), tok);
                }
            }

            if (pre || post)
                variables.use(varid1, tok);

            if (dereference) {
                const Variables::VariableUsage *const var = variables.find(varid1);
                if (var && var->mType == Variables::array)
                    variables.write(varid1, tok);
                variables.writeAliases(varid1, tok);
                variables.read(varid1, tok);
            } else {
                const Variables::VariableUsage *const var = variables.find(varid1);
                if (var && (inwhile || start->strAt(-1) == MatchCompiler::makeConstString(","))) {
                    variables.use(varid1, tok);
                } else if (var && var->mType == Variables::reference) {
                    variables.writeAliases(varid1, tok);
                    variables.read(varid1, tok);
                }
                // Consider allocating memory separately because allocating/freeing alone does not constitute using the variable
                else if (var && var->mType == Variables::pointer &&
                         match41(start)) {
                    bool allocate = true;

                    if (start->strAt(2) == MatchCompiler::makeConstString("new")) {
                        const Token *type = start->tokAt(3);

                        // skip nothrow
                        if (mTokenizer->isCPP() && (match42(type) ||
                                                    match43(type)))
                            type = type->link()->next();

                        // is it a user defined type?
                        if (!type->isStandardType()) {
                            const Variable *variable = start->variable();
                            if (!variable || !isRecordTypeWithoutSideEffects(variable->type()))
                                allocate = false;
                        }
                    }

                    if (allocate)
                        variables.allocateMemory(varid1, tok);
                    else
                        variables.write(varid1, tok);
                } else if (varid1 && match44(tok, varid1)) {
                    variables.read(varid1, tok);
                    variables.write(varid1, start);
                } else if (var &&
                           var->mType == Variables::pointer &&
                           match45(tok) &&
                           tok->varId() == 0 &&
                           tok->hasKnownIntValue() &&
                           tok->values().front().intvalue == 0) {
                    variables.use(varid1, tok);
                } else {
                    variables.write(varid1, tok);
                }
            }

            const Variables::VariableUsage * const var2 = variables.find(tok->varId());
            if (var2) {
                if (var2->mType == Variables::reference) {
                    variables.writeAliases(tok->varId(), tok);
                    variables.read(tok->varId(), tok);
                } else if (tok->varId() != varid1 && match46(tok))
                    variables.read(tok->varId(), tok);
                else if (tok->varId() != varid1 &&
                         var2->mType == Variables::standard &&
                         tok->strAt(-1) != MatchCompiler::makeConstString("&"))
                    variables.use(tok->varId(), tok);
            }

            const Token * const equal = skipBracketsAndMembers(tok->next());

            // checked for chained assignments
            if (tok != start && equal && equal->str() == MatchCompiler::makeConstString("=")) {
                const unsigned int varId = tok->varId();
                const Variables::VariableUsage * const var = variables.find(varId);

                if (var && var->mType != Variables::reference) {
                    variables.read(varId,tok);
                }

                tok = tok->previous();
            }
        }

        // assignment
        else if ((match47(tok) && match27(skipBracketsAndMembers(tok->next()))) ||
                 (match48(tok) && match49(tok->next()->link()))) {
            const Token *eq = tok;
            while (eq && !eq->isAssignmentOp())
                eq = eq->astParent();

            const bool deref = eq && eq->astOperand1() && eq->astOperand1()->valueType() && eq->astOperand1()->valueType()->pointer == 0U;

            if (tok->str() == MatchCompiler::makeConstString("*")) {
                tok = tok->tokAt(2);
                if (tok->str() == MatchCompiler::makeConstString("("))
                    tok = tok->link()->next();
            }

            const unsigned int varid = tok->varId();
            const Variables::VariableUsage *var = variables.find(varid);

            if (var) {
                // Consider allocating memory separately because allocating/freeing alone does not constitute using the variable
                if (var->mType == Variables::pointer &&
                    match50(skipBrackets(tok->next()))) {
                    variables.allocateMemory(varid, tok);
                } else if (var->mType == Variables::pointer || var->mType == Variables::reference) {
                    variables.read(varid, tok);
                    variables.writeAliases(varid, tok);
                } else if (var->mType == Variables::pointerArray) {
                    tok = doAssignment(variables, tok, deref, scope);
                } else
                    variables.writeAll(varid, tok);
            }
        }

        else if (mTokenizer->isCPP() && match51(tok)) {
            variables.erase(tok->next()->varId());
        }

        else if (match52(tok)) {
            if (tok->astOperand2()) { // bitop
                variables.read(tok->next()->varId(), tok);
            } else // addressof
                variables.use(tok->next()->varId(), tok); // use = read + write
        } else if (match53(tok)) {
            if (isLikelyStreamRead(mTokenizer->isCPP(), tok))
                variables.use(tok->next()->varId(), tok); // use = read + write
            else
                variables.read(tok->next()->varId(), tok);
        } else if (match54(tok) && match55(tok->previous())) {
            variables.read(tok->varId(), tok);
        } else if (isLikelyStreamRead(mTokenizer->isCPP(),tok->previous())) {
            variables.use(tok->varId(), tok);
        }

        // function parameter
        else if (match56(tok)) {
            variables.use(tok->next()->varId(), tok);   // use = read + write
        } else if (match57(tok) && tok->previous()->str() != MatchCompiler::makeConstString("*")) {
            variables.use(tok->next()->varId(), tok);   // use = read + write
        } else if (match58(tok)) {
            variables.eraseAll(tok->tokAt(2)->varId());
        } else if (match59(tok) &&
                   match60(tok->next()->link())) {
            variables.use(tok->next()->link()->next()->varId(), tok);   // use = read + write
        } else if (match61(tok)) {
            tok = tok->next();
            if (tok->str() == MatchCompiler::makeConstString("*"))
                tok = tok->next();
            variables.use(tok->varId(), tok);
        }

        // function
        else if (match22(tok)) {
            variables.read(tok->varId(), tok);
            useFunctionArgs(tok->next()->astOperand2(), variables);
        } else if (match62(tok)) {
            variables.eraseAll(tok->tokAt(4)->varId());
        }

        else if (match63(tok->previous())) {
            variables.read(tok->varId(), tok);
        }

        else if (tok->varId() && match8(tok)) {
            variables.use(tok->varId(), tok);   // use = read + write
        }

        else if (tok->str() == MatchCompiler::makeConstString(":") && (!tok->valueType() || tok->valueType()->pointer)) {
            if (tok->astOperand1())
                variables.use(tok->astOperand1()->varId(), tok->astOperand1());
            if (tok->astOperand2())
                variables.use(tok->astOperand2()->varId(), tok->astOperand2());
        }

        else if (tok->isExtendedOp() && tok->next() && tok->next()->varId() && tok->strAt(2) != MatchCompiler::makeConstString("=")) {
            variables.readAll(tok->next()->varId(), tok);
        }

        else if (tok->varId() && tok->next() && (tok->next()->str() == MatchCompiler::makeConstString(")") || tok->next()->isExtendedOp())) {
            if (match64(tok->tokAt(-2)) &&
                !(tok->tokAt(-2)->variable() && tok->tokAt(-2)->variable()->isReference()))
                variables.use(tok->varId(), tok);
            else
                variables.readAll(tok->varId(), tok);
        }

        else if (match65(tok) && match66(tok->previous())) {
            variables.readAll(tok->varId(), tok);
        }

        // ++|--
        else if (tok->next() && tok->next()->tokType() == Token::eIncDecOp && tok->next()->astOperand1() && tok->next()->astOperand1()->varId()) {
            if (tok->next()->astParent())
                variables.use(tok->next()->astOperand1()->varId(), tok);
            else
                variables.modified(tok->next()->astOperand1()->varId(), tok);
        }

        else if (tok->isAssignmentOp()) {
            for (const Token *tok2 = tok->next(); tok2 && tok2->str() != MatchCompiler::makeConstString(";"); tok2 = tok2->next()) {
                if (tok2->varId()) {
                    if (tok2->strAt(1) == MatchCompiler::makeConstString("="))
                        variables.write(tok2->varId(), tok);
                    else if (tok2->next() && tok2->next()->isAssignmentOp())
                        variables.use(tok2->varId(), tok);
                    else
                        variables.read(tok2->varId(), tok);
                }
            }
        }
    }
}

void CheckUnusedVar::checkFunctionVariableUsage()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    // Parse all executing scopes..
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    // only check functions
    const std::size_t functions = symbolDatabase->functionScopes.size();
    for (std::size_t i = 0; i < functions; ++i) {
        const Scope * scope = symbolDatabase->functionScopes[i];

        // Bailout when there are lambdas or inline functions
        // TODO: Handle lambdas and inline functions properly
        if (scope->hasInlineOrLambdaFunction())
            continue;

        // varId, usage {read, write, modified}
        Variables variables;

        checkFunctionVariableUsage_iterateScopes(scope, variables, false);


        // Check usage of all variables in the current scope..
        for (std::map<unsigned int, Variables::VariableUsage>::const_iterator it = variables.varUsage().begin();
             it != variables.varUsage().end();
             ++it) {
            const Variables::VariableUsage &usage = it->second;

            // variable has been marked as unused so ignore it
            if (usage._var->nameToken()->isAttributeUnused() || usage._var->nameToken()->isAttributeUsed())
                continue;

            // skip things that are only partially implemented to prevent false positives
            if (usage.mType == Variables::pointerPointer ||
                usage.mType == Variables::pointerArray ||
                usage.mType == Variables::referenceArray)
                continue;

            const std::string &varname = usage._var->name();
            const Variable* var = symbolDatabase->getVariableFromVarId(it->first);

            // variable has had memory allocated for it, but hasn't done
            // anything with that memory other than, perhaps, freeing it
            if (usage.unused() && !usage._modified && usage._allocateMemory)
                allocatedButUnusedVariableError(usage._lastAccess, varname);

            // variable has not been written, read, or modified
            else if (usage.unused() && !usage._modified)
                unusedVariableError(usage._var->nameToken(), varname);

            // variable has not been written but has been modified
            else if (usage._modified && !usage._write && !usage._allocateMemory && var && !var->isStlType())
                unassignedVariableError(usage._var->nameToken(), varname);

            // variable has been written but not read
            else if (!usage._read)
                unreadVariableError(usage._lastAccess, varname, usage._modified);

            // variable has been read but not written
            else if (!usage._write && !usage._allocateMemory && var && !var->isStlType() && !isEmptyType(var->type()))
                unassignedVariableError(usage._var->nameToken(), varname);
        }
    }
}

void CheckUnusedVar::unusedVariableError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::style, "unusedVariable", "$symbol:" + varname + "\nUnused variable: $symbol", CWE563, false);
}

void CheckUnusedVar::allocatedButUnusedVariableError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::style, "unusedAllocatedMemory", "$symbol:" + varname + "\nVariable '$symbol' is allocated memory that is never used.", CWE563, false);
}

void CheckUnusedVar::unreadVariableError(const Token *tok, const std::string &varname, bool modified)
{
    if (modified)
        reportError(tok, Severity::style, "unreadVariable", "$symbol:" + varname + "\nVariable '$symbol' is modified but its new value is never used.", CWE563, false);
    else
        reportError(tok, Severity::style, "unreadVariable", "$symbol:" + varname + "\nVariable '$symbol' is assigned a value that is never used.", CWE563, false);
}

void CheckUnusedVar::unassignedVariableError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::style, "unassignedVariable", "$symbol:" + varname + "\nVariable '$symbol' is not assigned a value.", CWE665, false);
}

//---------------------------------------------------------------------------
// Check that all struct members are used
//---------------------------------------------------------------------------
void CheckUnusedVar::checkStructMemberUsage()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (std::list<Scope>::const_iterator scope = symbolDatabase->scopeList.cbegin(); scope != symbolDatabase->scopeList.cend(); ++scope) {
        if (scope->type != Scope::eStruct && scope->type != Scope::eUnion)
            continue;

        if (scope->bodyStart->fileIndex() != 0 || scope->className.empty())
            continue;

        // Packed struct => possibly used by lowlevel code. Struct members might be required by hardware.
        if (scope->bodyEnd->isAttributePacked())
            continue;

        // Bail out if struct/union contains any functions
        if (!scope->functionList.empty())
            continue;

        // bail out if struct is inherited
        bool bailout = false;
        for (std::list<Scope>::const_iterator i = symbolDatabase->scopeList.cbegin(); i != symbolDatabase->scopeList.cend(); ++i) {
            if (i->definedType) {
                for (size_t j = 0; j < i->definedType->derivedFrom.size(); j++) {
                    if (i->definedType->derivedFrom[j].type == scope->definedType) {
                        bailout = true;
                        break;
                    }
                }
            }
        }
        if (bailout)
            continue;

        // bail out for extern/global struct
        for (const Variable* var : symbolDatabase->variableList()) {
            if (var && (var->isExtern() || (var->isGlobal() && !var->isStatic())) && var->typeEndToken()->str() == scope->className) {
                bailout = true;
                break;
            }
        }
        if (bailout)
            continue;

        // Bail out if some data is casted to struct..
        const std::string castPattern("( struct| " + scope->className + " * ) & %name% [");
        if (Token::findmatch(scope->bodyEnd, castPattern.c_str()))
            continue;

        // (struct S){..}
        const std::string initPattern("( struct| " + scope->className + " ) {");
        if (Token::findmatch(scope->bodyEnd, initPattern.c_str()))
            continue;

        // Bail out if struct is used in sizeof..
        for (const Token *tok = scope->bodyEnd; nullptr != (tok = findmatch67(tok) );) {
            tok = tok->tokAt(2);
            if (Token::Match(tok, ("struct| " + scope->className).c_str())) {
                bailout = true;
                break;
            }
        }
        if (bailout)
            continue;

        // Try to prevent false positives when struct members are not used directly.
        if (Token::findmatch(scope->bodyEnd, (scope->className + " %type%| *").c_str()))
            continue;

        for (std::list<Variable>::const_iterator var = scope->varlist.cbegin(); var != scope->varlist.cend(); ++var) {
            // declaring a POD member variable?
            if (!var->typeStartToken()->isStandardType() && !var->isPointer())
                continue;

            // Check if the struct member variable is used anywhere in the file
            if (Token::findsimplematch(mTokenizer->tokens(), (". " + var->name()).c_str()))
                continue;

            unusedStructMemberError(var->nameToken(), scope->className, var->name(), scope->type == Scope::eUnion);
        }
    }
}

void CheckUnusedVar::unusedStructMemberError(const Token *tok, const std::string &structname, const std::string &varname, bool isUnion)
{
    const std::string prefix = isUnion ? "union member " : "struct member ";
    reportError(tok, Severity::style, "unusedStructMember", "$symbol:" + structname + "::" + varname + '\n' + prefix + "'$symbol' is never used.", CWE563, false);
}

bool CheckUnusedVar::isRecordTypeWithoutSideEffects(const Type* type)
{
    // a type that has no side effects (no constructors and no members with constructors)
    /** @todo false negative: check constructors for side effects */

    const std::pair<std::map<const Type *,bool>::iterator,bool> found=mIsRecordTypeWithoutSideEffectsMap.insert(
                std::pair<const Type *,bool>(type,false)); //Initialize with side effects for possible recursions
    bool & withoutSideEffects=found.first->second;
    if (!found.second)
        return withoutSideEffects;

    if (type && type->classScope && type->classScope->numConstructors == 0 &&
        (type->classScope->varlist.empty() || type->needInitialization == Type::True)) {
        for (std::vector<Type::BaseInfo>::const_iterator i = type->derivedFrom.begin(); i != type->derivedFrom.end(); ++i) {
            if (!isRecordTypeWithoutSideEffects(i->type)) {
                withoutSideEffects=false;
                return withoutSideEffects;
            }
        }
        withoutSideEffects=true;
        return withoutSideEffects;
    }

    withoutSideEffects=false;   // unknown types are assumed to have side effects
    return withoutSideEffects;
}

bool CheckUnusedVar::isEmptyType(const Type* type)
{
    // a type that has no variables and no constructor

    const std::pair<std::map<const Type *,bool>::iterator,bool> found=mIsEmptyTypeMap.insert(
                std::pair<const Type *,bool>(type,false));
    bool & emptyType=found.first->second;
    if (!found.second)
        return emptyType;

    if (type && type->classScope && type->classScope->numConstructors == 0 &&
        (type->classScope->varlist.empty())) {
        for (std::vector<Type::BaseInfo>::const_iterator i = type->derivedFrom.begin(); i != type->derivedFrom.end(); ++i) {
            if (!isEmptyType(i->type)) {
                emptyType=false;
                return emptyType;
            }
        }
        emptyType=true;
        return emptyType;
    }

    emptyType=false;   // unknown types are assumed to be nonempty
    return emptyType;
}
