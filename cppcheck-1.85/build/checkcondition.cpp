#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: = & %var% ;
static bool match1(const Token* tok) {
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
// pattern: %num% [&|]
static bool match3(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("&|", tok->str()[0]))
        return false;
    return true;
}
// pattern: ;
template<class T> static T * findmatch4(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: * ) & %any% ;
static bool match5(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [&|] %num% ;
static bool match6(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("&|", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [(,] &
static bool match7(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("(,", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: %varid% %cop% %num% ;
static bool match8(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !tok->isConstOp())
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %varid% =
static bool match9(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: ++|-- %varid%
static bool match10(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    return true;
}
// pattern: %varid% ++|--
static bool match11(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: [(,] &| %varid% [,)]
static bool match12(const Token* tok, const unsigned int varid) {
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
// pattern: break|continue|return
static bool match13(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("return"))))
        return false;
    return true;
}
// pattern: %name% (
static bool match14(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) {
static bool match15(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: if|while (
static bool match16(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: &&|%oror%|( %varid% ==|!= %num% &&|%oror%|)
static bool match17(const Token* tok, const unsigned int varid) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    tok = tok->next();
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: %varid% %op%
static bool match18(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !tok->isOp())
        return false;
    return true;
}
// pattern: } else {
static bool match19(const Token* tok) {
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
// pattern: static|const
static bool match20(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("static")) || (tok->str()==MatchCompiler::makeConstString("const"))))
        return false;
    return true;
}
// pattern: bool|_Bool
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("_Bool"))))
        return false;
    return true;
}
// pattern: &&|%oror%
static bool match22(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    return true;
}
// pattern: if|while
static bool match23(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    return true;
}
// pattern: [&|]
static bool match24(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr("&|", tok->str()[0]))
        return false;
    return true;
}
// pattern: ==|!=
static bool match25(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    return true;
}
// pattern: >=|<=
static bool match26(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("<="))))
        return false;
    return true;
}
// pattern: >=|<
static bool match27(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">=")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: <=|>
static bool match28(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<=")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: &|==
static bool match29(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&")) || (tok->str()==MatchCompiler::makeConstString("=="))))
        return false;
    return true;
}
// pattern: } else { if (
static bool match30(const Token* tok) {
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
// pattern: %oror%|&&|!
static bool match31(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("!"))))
        return false;
    return true;
}
// pattern: %cop%|(|[
static bool match32(const Token* tok) {
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %name% .
static bool match33(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: { return|throw|continue|break
static bool match34(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("break"))))
        return false;
    return true;
}
// pattern: if (
static bool match35(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ++|--
template<class T> static T * findmatch36(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %type% (
static bool match37(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: case|break|continue|return|throw
static bool match38(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("case")) || (tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("continue")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: [;{}] %name% :
static bool match39(const Token* tok) {
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
// pattern: for|while|do
static bool match40(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("do"))))
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
// pattern: } while (
static bool match42(const Token* tok) {
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
// pattern: %name% %assign%|++|--
static bool match43(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->isAssignmentOp() || (tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: *|.|[
static bool match44(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("*")) || (tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: .|[
static bool match45(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("["))))
        return false;
    return true;
}
// pattern: %assign%|++|--
static bool match46(const Token* tok) {
    if (!tok || !(tok->isAssignmentOp() || (tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: %name% <<
static bool match47(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<<")))
        return false;
    return true;
}
// pattern: %name% [
static bool match48(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] [
static bool match49(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] %assign%|++|--
static bool match50(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isAssignmentOp() || (tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: ++|--|& %name%
static bool match51(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: %name% . %name% (
static bool match52(const Token* tok) {
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
// pattern: [(,] %name% [,)]
static bool match53(const Token* tok) {
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
// pattern: %cop%|&&|%oror%
static bool match54(const Token* tok) {
    if (!tok || !(tok->isConstOp() || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||"))))
        return false;
    return true;
}
// pattern: %oror%|&&
static bool match55(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&"))))
        return false;
    return true;
}
// pattern: %num%
static bool match56(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: %
static bool match57(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("%")))
        return false;
    return true;
}
// pattern: ( %name% [=&|^]
static bool match58(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("=&|^", tok->str()[0]))
        return false;
    return true;
}
// pattern: ?
static bool match59(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("?")))
        return false;
    return true;
}
// pattern: %name%|(|[|::|.
static bool match60(const Token* tok) {
    if (!tok || !(tok->isName() || (tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: %num%|%bool%|%char%
static bool match61(const Token* tok) {
    if (!tok || !(tok->isNumber() || tok->isBoolean() || (tok->tokType()==Token::eChar)))
        return false;
    return true;
}
// pattern: ! %num%|%bool%|%char%
static bool match62(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->isNumber() || tok->isBoolean() || (tok->tokType()==Token::eChar)))
        return false;
    return true;
}
// pattern: %comp%
static bool match63(const Token* tok) {
    if (!tok || !tok->isComparisonOp())
        return false;
    return true;
}
// pattern: %oror%|&&|?
static bool match64(const Token* tok) {
    if (!tok || !((tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("&&")) || (tok->str()==MatchCompiler::makeConstString("?"))))
        return false;
    return true;
}
// pattern: %comp%|!
static bool match65(const Token* tok) {
    if (!tok || !(tok->isComparisonOp() || (tok->str()==MatchCompiler::makeConstString("!"))))
        return false;
    return true;
}
// pattern: sizeof (
static bool match66(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: <|>=
static bool match67(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("<")) || (tok->str()==MatchCompiler::makeConstString(">="))))
        return false;
    return true;
}
// pattern: >|<=
static bool match68(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(">")) || (tok->str()==MatchCompiler::makeConstString("<="))))
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
// Check for condition mismatches
//---------------------------------------------------------------------------

#include "checkcondition.h"

#include "astutils.h"
#include "errorlogger.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "valueflow.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <list>
#include <ostream>
#include <set>
#include <stack>
#include <utility>

// CWE ids used
static const struct CWE CWE398(398U);   // Indicator of Poor Code Quality
static const struct CWE CWE570(570U);   // Expression is Always False
static const struct CWE CWE571(571U);   // Expression is Always True

//---------------------------------------------------------------------------

// Register this check class (by creating a static instance of it)
namespace {
    CheckCondition instance;
}

bool CheckCondition::isAliased(const std::set<unsigned int> &vars) const
{
    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match1(tok) && vars.find(tok->tokAt(2)->varId()) != vars.end())
            return true;
    }
    return false;
}

void CheckCondition::assignIf()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (tok->str() != MatchCompiler::makeConstString("="))
            continue;

        if (match2(tok->tokAt(-2))) {
            const Variable *var = tok->previous()->variable();
            if (var == nullptr)
                continue;

            char bitop = '\0';
            MathLib::bigint num = 0;

            if (match3(tok->next())) {
                bitop = tok->strAt(2).at(0);
                num = MathLib::toLongNumber(tok->next()->str());
            } else {
                const Token *endToken = findmatch4(tok) ;

                // Casting address
                if (endToken && match5(endToken->tokAt(-4)))
                    endToken = nullptr;

                if (endToken && match6(endToken->tokAt(-2))) {
                    bitop = endToken->strAt(-2).at(0);
                    num = MathLib::toLongNumber(endToken->previous()->str());
                }
            }

            if (bitop == '\0')
                continue;

            if (num < 0 && bitop == '|')
                continue;

            assignIfParseScope(tok, tok->tokAt(4), var->declarationId(), var->isLocal(), bitop, num);
        }
    }
}

static bool isParameterChanged(const Token *partok)
{
    bool addressOf = match7(partok);
    unsigned int argumentNumber = 0;
    const Token *ftok;
    for (ftok = partok; ftok && ftok->str() != MatchCompiler::makeConstString("("); ftok = ftok->previous()) {
        if (ftok->str() == MatchCompiler::makeConstString(")"))
            ftok = ftok->link();
        else if (argumentNumber == 0U && ftok->str() == MatchCompiler::makeConstString("&"))
            addressOf = true;
        else if (ftok->str() == MatchCompiler::makeConstString(","))
            argumentNumber++;
    }
    ftok = ftok ? ftok->previous() : nullptr;
    if (!(ftok && ftok->function()))
        return true;
    const Variable *par = ftok->function()->getArgumentVar(argumentNumber);
    if (!par)
        return true;
    if (par->isConst())
        return false;
    if (addressOf || par->isReference() || par->isPointer())
        return true;
    return false;
}

/** parse scopes recursively */
bool CheckCondition::assignIfParseScope(const Token * const assignTok,
                                        const Token * const startTok,
                                        const unsigned int varid,
                                        const bool islocal,
                                        const char bitop,
                                        const MathLib::bigint num)
{
    bool ret = false;

    for (const Token *tok2 = startTok; tok2; tok2 = tok2->next()) {
        if ((bitop == '&') && match8(tok2->tokAt(2), varid) && tok2->strAt(3) == std::string(1U, bitop)) {
            const MathLib::bigint num2 = MathLib::toLongNumber(tok2->strAt(4));
            if (0 == (num & num2))
                mismatchingBitAndError(assignTok, num, tok2, num2);
        }
        if (match9(tok2, varid)) {
            return true;
        }
        if (match10(tok2, varid) || match11(tok2, varid))
            return true;
        if (match12(tok2, varid) && isParameterChanged(tok2))
            return true;
        if (tok2->str() == MatchCompiler::makeConstString("}"))
            return false;
        if (match13(tok2))
            ret = true;
        if (ret && tok2->str() == MatchCompiler::makeConstString(";"))
            return false;
        if (!islocal && match14(tok2) && !match15(tok2->next()->link()))
            return true;
        if (match16(tok2)) {
            if (!islocal && tok2->str() == MatchCompiler::makeConstString("while"))
                continue;
            if (tok2->str() == MatchCompiler::makeConstString("while")) {
                // is variable changed in loop?
                const Token *bodyStart = tok2->linkAt(1)->next();
                const Token *bodyEnd   = bodyStart ? bodyStart->link() : nullptr;
                if (!bodyEnd || bodyEnd->str() != MatchCompiler::makeConstString("}") || isVariableChanged(bodyStart, bodyEnd, varid, !islocal, mSettings, mTokenizer->isCPP()))
                    continue;
            }

            // parse condition
            const Token * const end = tok2->next()->link();
            for (; tok2 != end; tok2 = tok2->next()) {
                if (match12(tok2, varid)) {
                    return true;
                }
                if (match17(tok2, varid)) {
                    const Token *vartok = tok2->next();
                    const MathLib::bigint num2 = MathLib::toLongNumber(vartok->strAt(2));
                    if ((num & num2) != ((bitop=='&') ? num2 : num)) {
                        const std::string& op(vartok->strAt(1));
                        const bool alwaysTrue = op == MatchCompiler::makeConstString("!=");
                        const std::string condition(vartok->str() + op + vartok->strAt(2));
                        assignIfError(assignTok, tok2, condition, alwaysTrue);
                    }
                }
                if (match18(tok2, varid) && tok2->next()->isAssignmentOp()) {
                    return true;
                }
            }

            const bool ret1 = assignIfParseScope(assignTok, end->tokAt(2), varid, islocal, bitop, num);
            bool ret2 = false;
            if (match19(end->next()->link()))
                ret2 = assignIfParseScope(assignTok, end->next()->link()->tokAt(3), varid, islocal, bitop, num);
            if (ret1 || ret2)
                return true;
        }
    }
    return false;
}

void CheckCondition::assignIfError(const Token *tok1, const Token *tok2, const std::string &condition, bool result)
{
    std::list<const Token *> locations = { tok1, tok2 };
    reportError(locations,
                Severity::style,
                "assignIfError",
                "Mismatching assignment and comparison, comparison '" + condition + "' is always " + std::string(result ? "true" : "false") + ".", CWE398, false);
}


void CheckCondition::mismatchingBitAndError(const Token *tok1, const MathLib::bigint num1, const Token *tok2, const MathLib::bigint num2)
{
    std::list<const Token *> locations = { tok1, tok2 };

    std::ostringstream msg;
    msg << "Mismatching bitmasks. Result is always 0 ("
        << "X = Y & 0x" << std::hex << num1 << "; Z = X & 0x" << std::hex << num2 << "; => Z=0).";

    reportError(locations,
                Severity::style,
                "mismatchingBitAnd",
                msg.str(), CWE398, false);
}


static void getnumchildren(const Token *tok, std::list<MathLib::bigint> &numchildren)
{
    if (tok->astOperand1() && tok->astOperand1()->isNumber())
        numchildren.push_back(MathLib::toLongNumber(tok->astOperand1()->str()));
    else if (tok->astOperand1() && tok->str() == tok->astOperand1()->str())
        getnumchildren(tok->astOperand1(), numchildren);
    if (tok->astOperand2() && tok->astOperand2()->isNumber())
        numchildren.push_back(MathLib::toLongNumber(tok->astOperand2()->str()));
    else if (tok->astOperand2() && tok->str() == tok->astOperand2()->str())
        getnumchildren(tok->astOperand2(), numchildren);
}

/* Return whether tok is in the body for a function returning a boolean. */
static bool inBooleanFunction(const Token *tok)
{
    const Scope *scope = tok ? tok->scope() : nullptr;
    while (scope && scope->isLocal())
        scope = scope->nestedIn;
    if (scope && scope->type == Scope::eFunction) {
        const Function *func = scope->function;
        if (func) {
            const Token *ret = func->retDef;
            while (match20(ret))
                ret = ret->next();
            return match21(ret);
        }
    }
    return false;
}

void CheckCondition::checkBadBitmaskCheck()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("|") && tok->astOperand1() && tok->astOperand2() && tok->astParent()) {
            const Token* parent = tok->astParent();
            const bool isBoolean = match22(parent) ||
                                   (parent->str() == MatchCompiler::makeConstString("?") && parent->astOperand1() == tok) ||
                                   (parent->str() == MatchCompiler::makeConstString("=") && parent->astOperand2() == tok && parent->astOperand1() && parent->astOperand1()->variable() && match21(parent->astOperand1()->variable()->typeStartToken())) ||
                                   (parent->str() == MatchCompiler::makeConstString("(") && match23(parent->astOperand1())) ||
                                   (parent->str() == MatchCompiler::makeConstString("return") && parent->astOperand1() == tok && inBooleanFunction(tok));

            const bool isTrue = (tok->astOperand1()->values().size() == 1 && tok->astOperand1()->values().front().intvalue != 0 && tok->astOperand1()->values().front().isKnown()) ||
                                (tok->astOperand2()->values().size() == 1 && tok->astOperand2()->values().front().intvalue != 0 && tok->astOperand2()->values().front().isKnown());

            if (isBoolean && isTrue)
                badBitmaskCheckError(tok);
        }
    }
}

void CheckCondition::badBitmaskCheckError(const Token *tok)
{
    reportError(tok, Severity::warning, "badBitmaskCheck", "Result of operator '|' is always true if one operand is non-zero. Did you intend to use '&'?", CWE571, false);
}

void CheckCondition::comparison()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (!tok->isComparisonOp())
            continue;

        const Token *expr1 = tok->astOperand1();
        const Token *expr2 = tok->astOperand2();
        if (!expr1 || !expr2)
            continue;
        if (expr1->isNumber())
            std::swap(expr1,expr2);
        if (!expr2->isNumber())
            continue;
        const MathLib::bigint num2 = MathLib::toLongNumber(expr2->str());
        if (num2 < 0)
            continue;
        if (!match24(expr1))
            continue;
        std::list<MathLib::bigint> numbers;
        getnumchildren(expr1, numbers);
        for (const MathLib::bigint num1 : numbers) {
            if (num1 < 0)
                continue;
            if (match25(tok)) {
                if ((expr1->str() == MatchCompiler::makeConstString("&") && (num1 & num2) != num2) ||
                    (expr1->str() == MatchCompiler::makeConstString("|") && (num1 | num2) != num2)) {
                    const std::string& op(tok->str());
                    comparisonError(expr1, expr1->str(), num1, op, num2, op==MatchCompiler::makeConstString("==") ? false : true);
                }
            } else if (expr1->str() == MatchCompiler::makeConstString("&")) {
                const bool or_equal = match26(tok);
                const std::string& op(tok->str());
                if ((match27(tok)) && (num1 < num2)) {
                    comparisonError(expr1, expr1->str(), num1, op, num2, or_equal ? false : true);
                } else if ((match28(tok)) && (num1 <= num2)) {
                    comparisonError(expr1, expr1->str(), num1, op, num2, or_equal ? true : false);
                }
            } else if (expr1->str() == MatchCompiler::makeConstString("|")) {
                if ((expr1->astOperand1()->valueType()) &&
                    (expr1->astOperand1()->valueType()->sign == ValueType::Sign::UNSIGNED)) {
                    const bool or_equal = match26(tok);
                    const std::string& op(tok->str());
                    if ((match27(tok)) && (num1 >= num2)) {
                        //"(a | 0x07) >= 7U" is always true for unsigned a
                        //"(a | 0x07) < 7U" is always false for unsigned a
                        comparisonError(expr1, expr1->str(), num1, op, num2, or_equal ? true : false);
                    } else if ((match28(tok)) && (num1 > num2)) {
                        //"(a | 0x08) <= 7U" is always false for unsigned a
                        //"(a | 0x07) > 6U" is always true for unsigned a
                        comparisonError(expr1, expr1->str(), num1, op, num2, or_equal ? false : true);
                    }
                }
            }
        }
    }
}

void CheckCondition::comparisonError(const Token *tok, const std::string &bitop, MathLib::bigint value1, const std::string &op, MathLib::bigint value2, bool result)
{
    std::ostringstream expression;
    expression << std::hex << "(X " << bitop << " 0x" << value1 << ") " << op << " 0x" << value2;

    const std::string errmsg("Expression '" + expression.str() + "' is always " + (result?"true":"false") + ".\n"
                             "The expression '" + expression.str() + "' is always " + (result?"true":"false") +
                             ". Check carefully constants and operators used, these errors might be hard to "
                             "spot sometimes. In case of complex expression it might help to split it to "
                             "separate expressions.");

    reportError(tok, Severity::style, "comparisonError", errmsg, CWE398, false);
}

bool CheckCondition::isOverlappingCond(const Token * const cond1, const Token * const cond2, bool pure) const
{
    if (!cond1 || !cond2)
        return false;

    // same expressions
    if (isSameExpression(mTokenizer->isCPP(), true, cond1, cond2, mSettings->library, pure, false))
        return true;

    // bitwise overlap for example 'x&7' and 'x==1'
    if (cond1->str() == MatchCompiler::makeConstString("&") && cond1->astOperand1() && cond2->astOperand2()) {
        const Token *expr1 = cond1->astOperand1();
        const Token *num1  = cond1->astOperand2();
        if (!num1) // unary operator&
            return false;
        if (!num1->isNumber())
            std::swap(expr1,num1);
        if (!num1->isNumber() || MathLib::isNegative(num1->str()))
            return false;

        if (!match29(cond2) || !cond2->astOperand1() || !cond2->astOperand2())
            return false;
        const Token *expr2 = cond2->astOperand1();
        const Token *num2  = cond2->astOperand2();
        if (!num2->isNumber())
            std::swap(expr2,num2);
        if (!num2->isNumber() || MathLib::isNegative(num2->str()))
            return false;

        if (!isSameExpression(mTokenizer->isCPP(), true, expr1, expr2, mSettings->library, pure, false))
            return false;

        const MathLib::bigint value1 = MathLib::toLongNumber(num1->str());
        const MathLib::bigint value2 = MathLib::toLongNumber(num2->str());
        if (cond2->str() == MatchCompiler::makeConstString("&"))
            return ((value1 & value2) == value2);
        return ((value1 & value2) > 0);
    }
    return false;
}


void CheckCondition::multiCondition()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eIf)
            continue;

        const Token * const cond1 = scope.classDef->next()->astOperand2();

        const Token * tok2 = scope.classDef->next();
        for (;;) {
            tok2 = tok2->link();
            if (!match15(tok2))
                break;
            tok2 = tok2->linkAt(1);
            if (!match30(tok2))
                break;
            tok2 = tok2->tokAt(4);

            if (cond1 &&
                tok2->astOperand2() &&
                !cond1->hasKnownValue() &&
                !tok2->astOperand2()->hasKnownValue() &&
                isOverlappingCond(cond1, tok2->astOperand2(), true))
                multiConditionError(tok2, cond1->linenr());
        }
    }
}

void CheckCondition::multiConditionError(const Token *tok, unsigned int line1)
{
    std::ostringstream errmsg;
    errmsg << "Expression is always false because 'else if' condition matches previous condition at line "
           << line1 << ".";

    reportError(tok, Severity::style, "multiCondition", errmsg.str(), CWE398, false);
}

//---------------------------------------------------------------------------
// - Opposite inner conditions => always false
// - (TODO) Same/Overlapping inner condition => always true
// - same condition after early exit => always false
//---------------------------------------------------------------------------

static bool isNonConstFunctionCall(const Token *ftok, const Library &library)
{
    if (library.isFunctionConst(ftok))
        return false;
    const Token *obj = ftok->next()->astOperand1();
    while (obj && obj->str() == MatchCompiler::makeConstString("."))
        obj = obj->astOperand1();
    if (!obj)
        return true;
    else if (obj->variable() && obj->variable()->isConst())
        return false;
    else if (ftok->function() && ftok->function()->isConst())
        return false;
    return true;
}

void CheckCondition::multiCondition2()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        const Token *condTok = nullptr;
        if (scope.type == Scope::eIf || scope.type == Scope::eWhile)
            condTok = scope.classDef->next()->astOperand2();
        else if (scope.type == Scope::eFor) {
            condTok = scope.classDef->next()->astOperand2();
            if (!condTok || condTok->str() != MatchCompiler::makeConstString(";"))
                continue;
            condTok = condTok->astOperand2();
            if (!condTok || condTok->str() != MatchCompiler::makeConstString(";"))
                continue;
            condTok = condTok->astOperand1();
        }
        if (!condTok)
            continue;
        const Token * const cond1 = condTok;

        if (!match15(scope.classDef->linkAt(1)))
            continue;

        bool nonConstFunctionCall = false;
        bool nonlocal = false; // nonlocal variable used in condition
        std::set<unsigned int> vars; // variables used in condition
        std::stack<const Token *> tokens;
        tokens.push(condTok);
        while (!tokens.empty()) {
            const Token *cond = tokens.top();
            tokens.pop();
            if (!cond)
                continue;

            if (match14(cond)) {
                nonConstFunctionCall = isNonConstFunctionCall(cond, mSettings->library);
                if (nonConstFunctionCall)
                    break;
            }

            if (cond->varId()) {
                vars.insert(cond->varId());
                const Variable *var = cond->variable();
                if (!nonlocal && var) {
                    if (!(var->isLocal() || var->isArgument()))
                        nonlocal = true;
                    else if ((var->isPointer() || var->isReference()) && !match31(cond->astParent()))
                        // TODO: if var is pointer check what it points at
                        nonlocal = true;
                }
            } else if (!nonlocal && cond->isName()) {
                // varid is 0. this is possibly a nonlocal variable..
                nonlocal = match32(cond->astParent()) || match33(cond) || (mTokenizer->isCPP() && cond->str() == MatchCompiler::makeConstString("this"));
            } else {
                tokens.push(cond->astOperand1());
                tokens.push(cond->astOperand2());
            }
        }

        if (nonConstFunctionCall)
            continue;

        // parse until second condition is reached..
        enum MULTICONDITIONTYPE { INNER, AFTER } type;
        const Token *tok;
        if (match34(scope.bodyStart)) {
            tok = scope.bodyEnd->next();
            type = MULTICONDITIONTYPE::AFTER;
        } else {
            tok = scope.bodyStart;
            type = MULTICONDITIONTYPE::INNER;
        }
        const Token * const endToken = tok->scope()->bodyEnd;

        for (; tok && tok != endToken; tok = tok->next()) {
            if (match35(tok)) {
                // Does condition modify tracked variables?
                if (const Token *op = findmatch36(tok, tok->linkAt(1)) ) {
                    bool bailout = false;
                    while (op) {
                        if (vars.find(op->astOperand1()->varId()) != vars.end()) {
                            bailout = true;
                            break;
                        }
                        if (nonlocal && op->astOperand1()->varId() == 0) {
                            bailout = true;
                            break;
                        }
                        op = findmatch36(op->next(), tok->linkAt(1)) ;
                    }
                    if (bailout)
                        break;
                }

                // Condition..
                const Token *cond2 = tok->next()->astOperand2();

                ErrorPath errorPath;

                if (type == MULTICONDITIONTYPE::INNER) {
                    std::stack<const Token *> tokens1;
                    tokens1.push(cond1);
                    while (!tokens1.empty()) {
                        const Token *firstCondition = tokens1.top();
                        tokens1.pop();
                        if (!firstCondition)
                            continue;
                        if (firstCondition->str() == MatchCompiler::makeConstString("&&")) {
                            tokens1.push(firstCondition->astOperand1());
                            tokens1.push(firstCondition->astOperand2());
                        } else if (!firstCondition->hasKnownValue()) {
                            if (isOppositeCond(false, mTokenizer->isCPP(), firstCondition, cond2, mSettings->library, true, true, &errorPath)) {
                                if (!isAliased(vars))
                                    oppositeInnerConditionError(firstCondition, cond2, errorPath);
                            } else if (isSameExpression(mTokenizer->isCPP(), true, firstCondition, cond2, mSettings->library, true, true, &errorPath)) {
                                identicalInnerConditionError(firstCondition, cond2, errorPath);
                            }
                        }
                    }
                } else {
                    std::stack<const Token *> tokens2;
                    tokens2.push(cond2);
                    while (!tokens2.empty()) {
                        const Token *secondCondition = tokens2.top();
                        tokens2.pop();
                        if (!secondCondition)
                            continue;
                        if (secondCondition->str() == MatchCompiler::makeConstString("||") || secondCondition->str() == MatchCompiler::makeConstString("&&")) {
                            tokens2.push(secondCondition->astOperand1());
                            tokens2.push(secondCondition->astOperand2());
                        } else if ((!cond1->hasKnownValue() || !secondCondition->hasKnownValue()) &&
                                   isSameExpression(mTokenizer->isCPP(), true, cond1, secondCondition, mSettings->library, true, true, &errorPath)) {
                            if (!isAliased(vars))
                                identicalConditionAfterEarlyExitError(cond1, secondCondition, errorPath);
                        }
                    }
                }
            }
            if (match37(tok) && nonlocal && isNonConstFunctionCall(tok, mSettings->library)) // non const function call -> bailout if there are nonlocal variables
                break;
            if (match38(tok) && tok->scope() == endToken->scope())
                break;
            if (match39(tok))
                break;
            // bailout if loop is seen.
            // TODO: handle loops better.
            if (match40(tok)) {
                const Token *tok1 = tok->next();
                const Token *tok2;
                if (match41(tok)) {
                    if (!match42(tok->linkAt(1)))
                        break;
                    tok2 = tok->linkAt(1)->linkAt(2);
                } else if (match16(tok)) {
                    tok2 = tok->linkAt(1);
                    if (match15(tok2))
                        tok2 = tok2->linkAt(1);
                    if (!tok2)
                        break;
                } else {
                    // Incomplete code
                    break;
                }
                bool changed = false;
                for (unsigned int varid : vars) {
                    if (isVariableChanged(tok1, tok2, varid, nonlocal, mSettings, mTokenizer->isCPP())) {
                        changed = true;
                        break;
                    }
                }
                if (changed)
                    break;
            }
            if ((tok->varId() && vars.find(tok->varId()) != vars.end()) ||
                (!tok->varId() && nonlocal)) {
                if (match43(tok))
                    break;
                if (match44(tok->astParent())) {
                    const Token *parent = tok;
                    while (match45(parent->astParent()) || (parent->astParent() && parent->astParent()->isUnaryOp("*")))
                        parent = parent->astParent();
                    if (match46(parent->astParent()))
                        break;
                }
                if (mTokenizer->isCPP() && match47(tok) && (!tok->valueType() || !tok->valueType()->isIntegral()))
                    break;
                if (isLikelyStreamRead(mTokenizer->isCPP(), tok->next()) || isLikelyStreamRead(mTokenizer->isCPP(), tok->previous()))
                    break;
                if (match48(tok)) {
                    const Token *tok2 = tok->linkAt(1);
                    while (match49(tok2))
                        tok2 = tok2->linkAt(1);
                    if (match50(tok2))
                        break;
                }
                if (match51(tok->previous()))
                    break;
                if (tok->variable() &&
                    !tok->variable()->isConst() &&
                    match52(tok)) {
                    const Function* function = tok->tokAt(2)->function();
                    if (!function || !function->isConst())
                        break;
                }
                if (match53(tok->previous()) && isParameterChanged(tok))
                    break;
            }
        }
    }
}

void CheckCondition::oppositeInnerConditionError(const Token *tok1, const Token* tok2, ErrorPath errorPath)
{
    const std::string s1(tok1 ? tok1->expressionString() : "x");
    const std::string s2(tok2 ? tok2->expressionString() : "!x");
    errorPath.emplace_back(ErrorPathItem(tok1, "outer condition: " + s1));
    errorPath.emplace_back(ErrorPathItem(tok2, "opposite inner condition: " + s2));

    const std::string msg("Opposite inner 'if' condition leads to a dead code block.\n"
                          "Opposite inner 'if' condition leads to a dead code block (outer condition is '" + s1 + "' and inner condition is '" + s2 + "').");
    reportError(errorPath, Severity::warning, "oppositeInnerCondition", msg, CWE398, false);
}

void CheckCondition::identicalInnerConditionError(const Token *tok1, const Token* tok2, ErrorPath errorPath)
{
    const std::string s1(tok1 ? tok1->expressionString() : "x");
    const std::string s2(tok2 ? tok2->expressionString() : "x");
    errorPath.emplace_back(ErrorPathItem(tok1, "outer condition: " + s1));
    errorPath.emplace_back(ErrorPathItem(tok2, "identical inner condition: " + s2));

    const std::string msg("Identical inner 'if' condition is always true.\n"
                          "Identical inner 'if' condition is always true (outer condition is '" + s1 + "' and inner condition is '" + s2 + "').");
    reportError(errorPath, Severity::warning, "identicalInnerCondition", msg, CWE398, false);
}

void CheckCondition::identicalConditionAfterEarlyExitError(const Token *cond1, const Token* cond2, ErrorPath errorPath)
{
    const std::string cond(cond1 ? cond1->expressionString() : "x");
    errorPath.emplace_back(ErrorPathItem(cond1, "first condition"));
    errorPath.emplace_back(ErrorPathItem(cond2, "second condition"));

    reportError(errorPath, Severity::warning, "identicalConditionAfterEarlyExit", "Identical condition '" + cond + "', second condition is always false", CWE398, false);
}

//---------------------------------------------------------------------------
//    if ((x != 1) || (x != 3))            // expression always true
//    if ((x == 1) && (x == 3))            // expression always false
//    if ((x < 1)  && (x > 3))             // expression always false
//    if ((x > 3)  || (x < 10))            // expression always true
//    if ((x > 5)  && (x != 1))            // second comparison always true
//
//    Check for suspect logic for an expression consisting of 2 comparison
//    expressions with a shared variable and constants and a logical operator
//    between them.
//
//    Suggest a different logical operator when the logical operator between
//    the comparisons is probably wrong.
//
//    Inform that second comparison is always true when first comparison is true.
//---------------------------------------------------------------------------

static std::string invertOperatorForOperandSwap(std::string s)
{
    if (s[0] == '<')
        s[0] = '>';
    else if (s[0] == '>')
        s[0] = '<';
    return s;
}

template <typename T>
static bool checkIntRelation(const std::string &op, const T value1, const T value2)
{
    return (op == MatchCompiler::makeConstString("==") && value1 == value2) ||
           (op == MatchCompiler::makeConstString("!=") && value1 != value2) ||
           (op == MatchCompiler::makeConstString(">")  && value1 >  value2) ||
           (op == MatchCompiler::makeConstString(">=") && value1 >= value2) ||
           (op == MatchCompiler::makeConstString("<")  && value1 <  value2) ||
           (op == MatchCompiler::makeConstString("<=") && value1 <= value2);
}

static bool checkFloatRelation(const std::string &op, const double value1, const double value2)
{
    return (op == MatchCompiler::makeConstString(">")  && value1 >  value2) ||
           (op == MatchCompiler::makeConstString(">=") && value1 >= value2) ||
           (op == MatchCompiler::makeConstString("<")  && value1 <  value2) ||
           (op == MatchCompiler::makeConstString("<=") && value1 <= value2);
}

template<class T>
T getvalue3(const T value1, const T value2)
{
    const T min = std::min(value1, value2);
    if (min== std::numeric_limits<T>::max())
        return min;
    else
        return min+1; // see #5895
}

template<>
double getvalue3(const double value1, const double value2)
{
    return (value1 + value2) / 2.0f;
}


template<class T>
static inline T getvalue(const int test, const T value1, const T value2)
{
    // test:
    // 1 => return value that is less than both value1 and value2
    // 2 => return value1
    // 3 => return value that is between value1 and value2
    // 4 => return value2
    // 5 => return value that is larger than both value1 and value2
    switch (test) {
    case 1:
        return std::numeric_limits<T>::lowest();
    case 2:
        return value1;
    case 3:
        return getvalue3<T>(value1, value2);
    case 4:
        return value2;
    case 5:
        return std::numeric_limits<T>::max();
    };
    return 0;
}

static bool parseComparison(const Token *comp, bool *not1, std::string *op, std::string *value, const Token **expr, bool* inconclusive)
{
    *not1 = false;
    while (comp && comp->str() == MatchCompiler::makeConstString("!")) {
        *not1 = !(*not1);
        comp = comp->astOperand1();
    }

    if (!comp)
        return false;

    const Token* op1 = comp->astOperand1();
    const Token* op2 = comp->astOperand2();
    if (!comp->isComparisonOp() || !op1 || !op2) {
        *op = "!=";
        *value = "0";
        *expr = comp;
    } else if (op1->isLiteral()) {
        if (op1->isExpandedMacro())
            return false;
        *op = invertOperatorForOperandSwap(comp->str());
        if (op1->enumerator() && op1->enumerator()->value_known)
            *value = MathLib::toString(op1->enumerator()->value);
        else
            *value = op1->str();
        *expr = op2;
    } else if (comp->astOperand2()->isLiteral()) {
        if (op2->isExpandedMacro())
            return false;
        *op = comp->str();
        if (op2->enumerator() && op2->enumerator()->value_known)
            *value = MathLib::toString(op2->enumerator()->value);
        else
            *value = op2->str();
        *expr = op1;
    } else {
        *op = "!=";
        *value = "0";
        *expr = comp;
    }

    *inconclusive = *inconclusive || ((*value)[0] == '\'' && !(*op == MatchCompiler::makeConstString("!=") || *op == MatchCompiler::makeConstString("==")));

    // Only float and int values are currently handled
    if (!MathLib::isInt(*value) && !MathLib::isFloat(*value) && (*value)[0] != '\'')
        return false;

    return true;
}

static std::string conditionString(bool not1, const Token *expr1, const std::string &op, const std::string &value1)
{
    if (expr1->astParent()->isComparisonOp())
        return std::string(not1 ? "!(" : "") +
               (expr1->isName() ? expr1->str() : std::string("EXPR")) +
               " " +
               op +
               " " +
               value1 +
               (not1 ? ")" : "");

    return std::string(not1 ? "!" : "") +
           (expr1->isName() ? expr1->str() : std::string("EXPR"));
}

static std::string conditionString(const Token * tok)
{
    if (!tok)
        return "";
    if (tok->isComparisonOp()) {
        bool inconclusive = false;
        bool not_;
        std::string op, value;
        const Token *expr;
        if (parseComparison(tok, &not_, &op, &value, &expr, &inconclusive) && expr->isName()) {
            return conditionString(not_, expr, op, value);
        }
    }
    if (match54(tok)) {
        if (tok->astOperand2())
            return conditionString(tok->astOperand1()) + " " + tok->str() + " " + conditionString(tok->astOperand2());
        return tok->str() + "(" + conditionString(tok->astOperand1()) + ")";

    }
    return tok->expressionString();
}

void CheckCondition::checkIncorrectLogicOperator()
{
    const bool printStyle = mSettings->isEnabled(Settings::STYLE);
    const bool printWarning = mSettings->isEnabled(Settings::WARNING);
    if (!printWarning && !printStyle)
        return;
    const bool printInconclusive = mSettings->inconclusive;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {

        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (!match55(tok) || !tok->astOperand1() || !tok->astOperand2())
                continue;


            // 'A && (!A || B)' is equivalent to 'A && B'
            // 'A || (!A && B)' is equivalent to 'A || B'
            if (printStyle &&
                ((tok->str() == MatchCompiler::makeConstString("||") && tok->astOperand2()->str() == MatchCompiler::makeConstString("&&")) ||
                 (tok->str() == MatchCompiler::makeConstString("&&") && tok->astOperand2()->str() == MatchCompiler::makeConstString("||")))) {
                const Token* tok2 = tok->astOperand2()->astOperand1();
                if (isOppositeCond(true, mTokenizer->isCPP(), tok->astOperand1(), tok2, mSettings->library, true, false)) {
                    std::string expr1(tok->astOperand1()->expressionString());
                    std::string expr2(tok->astOperand2()->astOperand1()->expressionString());
                    std::string expr3(tok->astOperand2()->astOperand2()->expressionString());
                    // make copy for later because the original string might get overwritten
                    const std::string expr1VerboseMsg = expr1;
                    const std::string expr2VerboseMsg = expr2;
                    const std::string expr3VerboseMsg = expr3;

                    if (expr1.length() + expr2.length() + expr3.length() > 50U) {
                        if (expr1[0] == '!' && expr2[0] != '!') {
                            expr1 = "!A";
                            expr2 = "A";
                        } else {
                            expr1 = "A";
                            expr2 = "!A";
                        }

                        expr3 = "B";
                    }

                    const std::string cond1 = expr1 + " " + tok->str() + " (" + expr2 + " " + tok->astOperand2()->str() + " " + expr3 + ")";
                    const std::string cond2 = expr1 + " " + tok->str() + " " + expr3;

                    const std::string cond1VerboseMsg = expr1VerboseMsg + " " + tok->str() + " " + expr2VerboseMsg + " " + tok->astOperand2()->str() + " " + expr3VerboseMsg;
                    const std::string cond2VerboseMsg = expr1VerboseMsg + " " + tok->str() + " " + expr3VerboseMsg;
                    // for the --verbose message, transform the actual condition and print it
                    const std::string msg = tok2->expressionString() + ". '" + cond1 + "' is equivalent to '" + cond2 + "'\n"
                                            "The condition '" + cond1VerboseMsg + "' is equivalent to '" + cond2VerboseMsg + "'.";
                    redundantConditionError(tok, msg, false);
                    continue;
                }
            }

            // Comparison #1 (LHS)
            const Token *comp1 = tok->astOperand1();
            if (comp1 && comp1->str() == tok->str())
                comp1 = comp1->astOperand2();

            // Comparison #2 (RHS)
            const Token *comp2 = tok->astOperand2();

            bool inconclusive = false;
            bool parseable = true;

            // Parse LHS
            bool not1;
            std::string op1, value1;
            const Token *expr1 = nullptr;
            parseable &= (parseComparison(comp1, &not1, &op1, &value1, &expr1, &inconclusive));

            // Parse RHS
            bool not2;
            std::string op2, value2;
            const Token *expr2 = nullptr;
            parseable &= (parseComparison(comp2, &not2, &op2, &value2, &expr2, &inconclusive));

            if (inconclusive && !printInconclusive)
                continue;

            const bool isfloat = astIsFloat(expr1, true) || MathLib::isFloat(value1) || astIsFloat(expr2, true) || MathLib::isFloat(value2);

            ErrorPath errorPath;

            // Opposite comparisons around || or && => always true or always false
            if (!isfloat && isOppositeCond(tok->str() == MatchCompiler::makeConstString("||"), mTokenizer->isCPP(), tok->astOperand1(), tok->astOperand2(), mSettings->library, true, true, &errorPath)) {

                const bool alwaysTrue(tok->str() == MatchCompiler::makeConstString("||"));
                incorrectLogicOperatorError(tok, conditionString(tok), alwaysTrue, inconclusive, errorPath);
                continue;
            }

            if (!parseable)
                continue;

            if (isSameExpression(mTokenizer->isCPP(), true, comp1, comp2, mSettings->library, true, true))
                continue; // same expressions => only report that there are same expressions
            if (!isSameExpression(mTokenizer->isCPP(), true, expr1, expr2, mSettings->library, true, true))
                continue;


            // don't check floating point equality comparisons. that is bad
            // and deserves different warnings.
            if (isfloat && (op1 == MatchCompiler::makeConstString("==") || op1 == MatchCompiler::makeConstString("!=") || op2 == MatchCompiler::makeConstString("==") || op2 == MatchCompiler::makeConstString("!=")))
                continue;


            const double d1 = (isfloat) ? MathLib::toDoubleNumber(value1) : 0;
            const double d2 = (isfloat) ? MathLib::toDoubleNumber(value2) : 0;
            const MathLib::bigint i1 = (isfloat) ? 0 : MathLib::toLongNumber(value1);
            const MathLib::bigint i2 = (isfloat) ? 0 : MathLib::toLongNumber(value2);
            const bool useUnsignedInt = (std::numeric_limits<MathLib::bigint>::max()==i1)||(std::numeric_limits<MathLib::bigint>::max()==i2);
            const MathLib::biguint u1 = (useUnsignedInt) ? MathLib::toLongNumber(value1) : 0;
            const MathLib::biguint u2 = (useUnsignedInt) ? MathLib::toLongNumber(value2) : 0;
            // evaluate if expression is always true/false
            bool alwaysTrue = true, alwaysFalse = true;
            bool firstTrue = true, secondTrue = true;
            for (int test = 1; test <= 5; ++test) {
                // test:
                // 1 => testvalue is less than both value1 and value2
                // 2 => testvalue is value1
                // 3 => testvalue is between value1 and value2
                // 4 => testvalue value2
                // 5 => testvalue is larger than both value1 and value2
                bool result1, result2;
                if (isfloat) {
                    const double testvalue = getvalue<double>(test, d1, d2);
                    result1 = checkFloatRelation(op1, testvalue, d1);
                    result2 = checkFloatRelation(op2, testvalue, d2);
                } else if (useUnsignedInt) {
                    const MathLib::biguint testvalue = getvalue<MathLib::biguint>(test, u1, u2);
                    result1 = checkIntRelation(op1, testvalue, u1);
                    result2 = checkIntRelation(op2, testvalue, u2);
                } else {
                    const MathLib::bigint testvalue = getvalue<MathLib::bigint>(test, i1, i2);
                    result1 = checkIntRelation(op1, testvalue, i1);
                    result2 = checkIntRelation(op2, testvalue, i2);
                }
                if (not1)
                    result1 = !result1;
                if (not2)
                    result2 = !result2;
                if (tok->str() == MatchCompiler::makeConstString("&&")) {
                    alwaysTrue &= (result1 && result2);
                    alwaysFalse &= !(result1 && result2);
                } else {
                    alwaysTrue &= (result1 || result2);
                    alwaysFalse &= !(result1 || result2);
                }
                firstTrue &= !(!result1 && result2);
                secondTrue &= !(result1 && !result2);
            }

            const std::string cond1str = conditionString(not1, expr1, op1, value1);
            const std::string cond2str = conditionString(not2, expr2, op2, value2);
            if (printWarning && (alwaysTrue || alwaysFalse)) {
                const std::string text = cond1str + " " + tok->str() + " " + cond2str;
                incorrectLogicOperatorError(tok, text, alwaysTrue, inconclusive, errorPath);
            } else if (printStyle && secondTrue) {
                const std::string text = "If '" + cond1str + "', the comparison '" + cond2str +
                                         "' is always true.";
                redundantConditionError(tok, text, inconclusive);
            } else if (printStyle && firstTrue) {
                //const std::string text = "The comparison " + cond1str + " is always " +
                //                         (firstTrue ? "true" : "false") + " when " +
                //                         cond2str + ".";
                const std::string text = "If '" + cond2str + "', the comparison '" + cond1str +
                                         "' is always true.";
                redundantConditionError(tok, text, inconclusive);
            }
        }
    }
}

void CheckCondition::incorrectLogicOperatorError(const Token *tok, const std::string &condition, bool always, bool inconclusive, ErrorPath errors)
{
    errors.emplace_back(tok, "");
    if (always)
        reportError(errors, Severity::warning, "incorrectLogicOperator",
                    "Logical disjunction always evaluates to true: " + condition + ".\n"
                    "Logical disjunction always evaluates to true: " + condition + ". "
                    "Are these conditions necessary? Did you intend to use && instead? Are the numbers correct? Are you comparing the correct variables?", CWE571, inconclusive);
    else
        reportError(errors, Severity::warning, "incorrectLogicOperator",
                    "Logical conjunction always evaluates to false: " + condition + ".\n"
                    "Logical conjunction always evaluates to false: " + condition + ". "
                    "Are these conditions necessary? Did you intend to use || instead? Are the numbers correct? Are you comparing the correct variables?", CWE570, inconclusive);
}

void CheckCondition::redundantConditionError(const Token *tok, const std::string &text, bool inconclusive)
{
    reportError(tok, Severity::style, "redundantCondition", "Redundant condition: " + text, CWE398, inconclusive);
}

//-----------------------------------------------------------------------------
// Detect "(var % val1) > val2" where val2 is >= val1.
//-----------------------------------------------------------------------------
void CheckCondition::checkModuloAlwaysTrueFalse()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (!tok->isComparisonOp())
                continue;
            const Token *num, *modulo;
            if (match57(tok->astOperand1()) && match56(tok->astOperand2())) {
                modulo = tok->astOperand1();
                num = tok->astOperand2();
            } else if (match56(tok->astOperand1()) && match57(tok->astOperand2())) {
                num = tok->astOperand1();
                modulo = tok->astOperand2();
            } else {
                continue;
            }

            if (match56(modulo->astOperand2()) &&
                MathLib::isLessEqual(modulo->astOperand2()->str(), num->str()))
                moduloAlwaysTrueFalseError(tok, modulo->astOperand2()->str());
        }
    }
}

void CheckCondition::moduloAlwaysTrueFalseError(const Token* tok, const std::string& maxVal)
{
    reportError(tok, Severity::warning, "moduloAlwaysTrueFalse",
                "Comparison of modulo result is predetermined, because it is always less than " + maxVal + ".", CWE398, false);
}

static int countPar(const Token *tok1, const Token *tok2)
{
    int par = 0;
    for (const Token *tok = tok1; tok && tok != tok2; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("("))
            ++par;
        else if (tok->str() == MatchCompiler::makeConstString(")"))
            --par;
        else if (tok->str() == MatchCompiler::makeConstString(";"))
            return -1;
    }
    return par;
}

//---------------------------------------------------------------------------
// Clarify condition '(x = a < 0)' into '((x = a) < 0)' or '(x = (a < 0))'
// Clarify condition '(a & b == c)' into '((a & b) == c)' or '(a & (b == c))'
//---------------------------------------------------------------------------
void CheckCondition::clarifyCondition()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const bool isC = mTokenizer->isC();

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match58(tok)) {
                for (const Token *tok2 = tok->tokAt(3); tok2; tok2 = tok2->next()) {
                    if (tok2->str() == MatchCompiler::makeConstString("(") || tok2->str() == MatchCompiler::makeConstString("["))
                        tok2 = tok2->link();
                    else if (tok2->tokType() == Token::eComparisonOp) {
                        // This might be a template
                        if (!isC && tok2->link())
                            break;
                        if (match59(tok2->astParent()))
                            break;
                        clarifyConditionError(tok, tok->strAt(2) == MatchCompiler::makeConstString("="), false);
                        break;
                    } else if (!tok2->isName() && !tok2->isNumber() && tok2->str() != MatchCompiler::makeConstString("."))
                        break;
                }
            } else if (tok->tokType() == Token::eBitOp && !tok->isUnaryOp("&")) {
                if (tok->astOperand2() && tok->astOperand2()->variable() && tok->astOperand2()->variable()->nameToken() == tok->astOperand2())
                    continue;

                // using boolean result in bitwise operation ! x [&|^]
                const ValueType* vt1 = tok->astOperand1() ? tok->astOperand1()->valueType() : nullptr;
                const ValueType* vt2 = tok->astOperand2() ? tok->astOperand2()->valueType() : nullptr;
                if (vt1 && vt1->type == ValueType::BOOL && !match60(tok->astOperand1()) && countPar(tok->astOperand1(), tok) == 0)
                    clarifyConditionError(tok, false, true);
                else if (vt2 && vt2->type == ValueType::BOOL && !match60(tok->astOperand2()) && countPar(tok, tok->astOperand2()) == 0)
                    clarifyConditionError(tok, false, true);
            }
        }
    }
}

void CheckCondition::clarifyConditionError(const Token *tok, bool assign, bool boolop)
{
    std::string errmsg;

    if (assign)
        errmsg = "Suspicious condition (assignment + comparison); Clarify expression with parentheses.";

    else if (boolop)
        errmsg = "Boolean result is used in bitwise operation. Clarify expression with parentheses.\n"
                 "Suspicious expression. Boolean result is used in bitwise operation. The operator '!' "
                 "and the comparison operators have higher precedence than bitwise operators. "
                 "It is recommended that the expression is clarified with parentheses.";
    else
        errmsg = "Suspicious condition (bitwise operator + comparison); Clarify expression with parentheses.\n"
                 "Suspicious condition. Comparison operators have higher precedence than bitwise operators. "
                 "Please clarify the condition with parentheses.";

    reportError(tok,
                Severity::style,
                "clarifyCondition",
                errmsg, CWE398, false);
}


void CheckCondition::alwaysTrueFalse()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {

            if (tok->link()) // don't write false positives when templates are used
                continue;
            if (!tok->hasKnownIntValue())
                continue;
            if (match61(tok))
                continue;
            if (match62(tok))
                continue;
            if (match55(tok))
                continue;
            if (match63(tok) && isSameExpression(mTokenizer->isCPP(), true, tok->astOperand1(), tok->astOperand2(), mSettings->library, true, true))
                continue;

            const bool constIfWhileExpression =
                tok->astParent() && match23(tok->astTop()->astOperand1()) &&
                (match55(tok->astParent()) || match23(tok->astParent()->astOperand1()));
            const bool constValExpr = tok->isNumber() && match64(tok->astParent()); // just one number in boolean expression
            const bool compExpr = match65(tok); // a compare expression

            if (!(constIfWhileExpression || constValExpr || compExpr))
                continue;

            // Don't warn in assertions. Condition is often 'always true' by intention.
            // If platform,defines,etc cause 'always false' then that is not dangerous neither.
            bool assertFound = false;
            for (const Token * tok2 = tok->astParent(); tok2 ; tok2 = tok2->astParent()) { // move backwards and try to find "assert"
                if (tok2->str() == MatchCompiler::makeConstString("(") && tok2->astOperand2()) {
                    const std::string& str = tok2->previous()->str();
                    if ((str.find("assert")!=std::string::npos || str.find("ASSERT")!=std::string::npos))
                        assertFound = true;
                    break;
                }
            }
            if (assertFound)
                continue;

            // Don't warn when there are expanded macros..
            bool isExpandedMacro = false;
            std::stack<const Token*> tokens;
            tokens.push(tok);
            while (!tokens.empty()) {
                const Token *tok2 = tokens.top();
                tokens.pop();
                if (!tok2)
                    continue;
                tokens.push(tok2->astOperand1());
                tokens.push(tok2->astOperand2());
                if (tok2->isExpandedMacro()) {
                    isExpandedMacro = true;
                    break;
                }
            }
            if (isExpandedMacro)
                continue;
            for (const Token *parent = tok; parent; parent = parent->astParent()) {
                if (parent->isExpandedMacro()) {
                    isExpandedMacro = true;
                    break;
                }
            }
            if (isExpandedMacro)
                continue;

            // don't warn when condition checks sizeof result
            bool hasSizeof = false;
            tokens.push(tok);
            while (!tokens.empty()) {
                const Token *tok2 = tokens.top();
                tokens.pop();
                if (!tok2)
                    continue;
                if (tok2->isNumber())
                    continue;
                if (match66(tok2->previous())) {
                    hasSizeof = true;
                    continue;
                }
                if (tok2->isComparisonOp() || tok2->isArithmeticalOp()) {
                    tokens.push(tok2->astOperand1());
                    tokens.push(tok2->astOperand2());
                } else
                    break;
            }
            if (tokens.empty() && hasSizeof)
                continue;

            alwaysTrueFalseError(tok, &tok->values().front());
        }
    }
}

void CheckCondition::alwaysTrueFalseError(const Token *tok, const ValueFlow::Value *value)
{
    const bool condvalue = value && (value->intvalue != 0);
    const std::string expr = tok ? tok->expressionString() : std::string("x");
    const std::string errmsg = "Condition '" + expr + "' is always " + (condvalue ? "true" : "false");
    const ErrorPath errorPath = getErrorPath(tok, value, errmsg);
    reportError(errorPath,
                Severity::style,
                "knownConditionTrueFalse",
                errmsg,
                (condvalue ? CWE571 : CWE570), false);
}

void CheckCondition::checkInvalidTestForOverflow()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {

        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (!tok->isComparisonOp() || !tok->astOperand1() || !tok->astOperand2())
                continue;

            const Token *calcToken, *exprToken;
            bool result;
            if (match67(tok) && tok->astOperand1()->str() == MatchCompiler::makeConstString("+")) {
                calcToken = tok->astOperand1();
                exprToken = tok->astOperand2();
                result = (tok->str() == MatchCompiler::makeConstString(">="));
            } else if (match68(tok) && tok->astOperand2()->str() == MatchCompiler::makeConstString("+")) {
                calcToken = tok->astOperand2();
                exprToken = tok->astOperand1();
                result = (tok->str() == MatchCompiler::makeConstString("<="));
            } else
                continue;

            // Only warn for signed integer overflows and pointer overflows.
            if (!(calcToken->valueType() && (calcToken->valueType()->pointer || calcToken->valueType()->sign == ValueType::Sign::SIGNED)))
                continue;
            if (!(exprToken->valueType() && (exprToken->valueType()->pointer || exprToken->valueType()->sign == ValueType::Sign::SIGNED)))
                continue;

            const Token *termToken;
            if (isSameExpression(mTokenizer->isCPP(), true, exprToken, calcToken->astOperand1(), mSettings->library, true, false))
                termToken = calcToken->astOperand2();
            else if (isSameExpression(mTokenizer->isCPP(), true, exprToken, calcToken->astOperand2(), mSettings->library, true, false))
                termToken = calcToken->astOperand1();
            else
                continue;

            if (!termToken)
                continue;

            // Only warn when termToken is always positive
            if (termToken->valueType() && termToken->valueType()->sign == ValueType::Sign::UNSIGNED)
                invalidTestForOverflow(tok, result);
            else if (termToken->isNumber() && MathLib::isPositive(termToken->str()))
                invalidTestForOverflow(tok, result);
        }
    }
}

void CheckCondition::invalidTestForOverflow(const Token* tok, bool result)
{
    const std::string errmsg = "Invalid test for overflow '" +
                               (tok ? tok->expressionString() : std::string("x + u < x")) +
                               "'. Condition is always " +
                               std::string(result ? "true" : "false") +
                               " unless there is overflow, and overflow is undefined behaviour.";
    reportError(tok, Severity::warning, "invalidTestForOverflow", errmsg, (result ? CWE571 : CWE570), false);
}


void CheckCondition::checkPointerAdditionResultNotNull()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {

        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (!tok->isComparisonOp() || !tok->astOperand1() || !tok->astOperand2())
                continue;

            // Macros might have pointless safety checks
            if (tok->isExpandedMacro())
                continue;

            const Token *calcToken, *exprToken;
            if (tok->astOperand1()->str() == MatchCompiler::makeConstString("+")) {
                calcToken = tok->astOperand1();
                exprToken = tok->astOperand2();
            } else if (tok->astOperand2()->str() == MatchCompiler::makeConstString("+")) {
                calcToken = tok->astOperand2();
                exprToken = tok->astOperand1();
            } else
                continue;

            // pointer comparison against NULL (ptr+12==0)
            if (calcToken->hasKnownIntValue())
                continue;
            if (!calcToken->valueType() || calcToken->valueType()->pointer==0)
                continue;
            if (!exprToken->hasKnownIntValue() || !exprToken->getValue(0))
                continue;

            pointerAdditionResultNotNullError(tok, calcToken);
        }
    }
}

void CheckCondition::pointerAdditionResultNotNullError(const Token *tok, const Token *calc)
{
    const std::string s = calc ? calc->expressionString() : "ptr+1";
    reportError(tok, Severity::warning, "pointerAdditionResultNotNull", "Comparison is wrong. Result of '" + s + "' can't be 0 unless there is pointer overflow, and pointer overflow is undefined behaviour.");
}
