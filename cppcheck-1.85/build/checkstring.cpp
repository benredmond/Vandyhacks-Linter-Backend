#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %var% [
static bool match1(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] =
static bool match2(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: * %var% =
static bool match3(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: memcmp|strncmp|strcmp|stricmp|strverscmp|bcmp|strcmpi|strcasecmp|strncasecmp|strncasecmp_l|strcasecmp_l|wcsncasecmp|wcscasecmp|wmemcmp|wcscmp|wcscasecmp_l|wcsncasecmp_l|wcsncmp|_mbscmp|_memicmp|_memicmp_l|_stricmp|_wcsicmp|_mbsicmp|_stricmp_l|_wcsicmp_l|_mbsicmp_l
static bool match4(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("memcmp")) || (tok->str()==MatchCompiler::makeConstString("strncmp")) || (tok->str()==MatchCompiler::makeConstString("strcmp")) || (tok->str()==MatchCompiler::makeConstString("stricmp")) || (tok->str()==MatchCompiler::makeConstString("strverscmp")) || (tok->str()==MatchCompiler::makeConstString("bcmp")) || (tok->str()==MatchCompiler::makeConstString("strcmpi")) || (tok->str()==MatchCompiler::makeConstString("strcasecmp")) || (tok->str()==MatchCompiler::makeConstString("strncasecmp")) || (tok->str()==MatchCompiler::makeConstString("strncasecmp_l")) || (tok->str()==MatchCompiler::makeConstString("strcasecmp_l")) || (tok->str()==MatchCompiler::makeConstString("wcsncasecmp")) || (tok->str()==MatchCompiler::makeConstString("wcscasecmp")) || (tok->str()==MatchCompiler::makeConstString("wmemcmp")) || (tok->str()==MatchCompiler::makeConstString("wcscmp")) || (tok->str()==MatchCompiler::makeConstString("wcscasecmp_l")) || (tok->str()==MatchCompiler::makeConstString("wcsncasecmp_l")) || (tok->str()==MatchCompiler::makeConstString("wcsncmp")) || (tok->str()==MatchCompiler::makeConstString("_mbscmp")) || (tok->str()==MatchCompiler::makeConstString("_memicmp")) || (tok->str()==MatchCompiler::makeConstString("_memicmp_l")) || (tok->str()==MatchCompiler::makeConstString("_stricmp")) || (tok->str()==MatchCompiler::makeConstString("_wcsicmp")) || (tok->str()==MatchCompiler::makeConstString("_mbsicmp")) || (tok->str()==MatchCompiler::makeConstString("_stricmp_l")) || (tok->str()==MatchCompiler::makeConstString("_wcsicmp_l")) || (tok->str()==MatchCompiler::makeConstString("_mbsicmp_l"))))
        return false;
    return true;
}
// pattern: %str% , %str% ,|)
static bool match5(const Token* tok) {
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: %name% , %name% ,|)
static bool match6(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
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
// pattern: %name% . c_str ( ) , %name% . c_str ( ) ,|)
static bool match7(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("c_str")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(",")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("c_str")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: QString :: compare ( %str% , %str% )
static bool match8(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("QString")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("compare")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
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
    return true;
}
// pattern: !!+ %str% ==|!= %str% !!+
static bool match9(const Token* tok) {
    if (tok && tok->str() == MatchCompiler::makeConstString("+"))
        return false;
    tok = tok ? tok->next() : NULL;
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("+"))
        return false;
    return true;
}
// pattern: %char%|%num%|%str%
static bool match10(const Token* tok) {
    if (!tok || !((tok->tokType()==Token::eChar) || tok->isNumber() || (tok->tokType()==Token::eString)))
        return false;
    return true;
}
// pattern: [.*]
static bool match11(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(".*", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% (
static bool match12(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %str% &&
static bool match13(const Token* tok) {
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    return true;
}
// pattern: && %str% )
static bool match14(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: %num% )
static bool match15(const Token* tok) {
    if (!tok || !tok->isNumber())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: . substr (
static bool match16(const Token* tok) {
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
// pattern: ]|)|>
static bool match17(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("]")) || (tok->str()==MatchCompiler::makeConstString(")")) || (tok->str()==MatchCompiler::makeConstString(">"))))
        return false;
    return true;
}
// pattern: .|::
static bool match18(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: %str% ==|!=
static bool match19(const Token* tok) {
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    return true;
}
// pattern: ==|!= %str% !!+
static bool match20(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("==")) || (tok->str()==MatchCompiler::makeConstString("!="))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("+"))
        return false;
    return true;
}
// pattern: &&|%oror%|( %str%|%char% &&|%oror%|)
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType()==Token::eString) || (tok->tokType()==Token::eChar)))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("&&")) || (tok->tokType() == Token::eLogicalOp && tok->str()==MatchCompiler::makeConstString("||")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: ( %str%|%char% )
static bool match22(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType()==Token::eString) || (tok->tokType()==Token::eChar)))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: if|while ( %str%|%char% )
static bool match23(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("while"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->tokType()==Token::eString) || (tok->tokType()==Token::eChar)))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: (
static bool match24(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: 0
static bool match25(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("0")))
        return false;
    return true;
}
// pattern: strcmp (
static bool match26(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("strcmp")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: sprintf|snprintf|swprintf (
static bool match27(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sprintf")) || (tok->str()==MatchCompiler::makeConstString("snprintf")) || (tok->str()==MatchCompiler::makeConstString("swprintf"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: sprintf
static bool match28(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sprintf")))
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
#include "checkstring.h"

#include "astutils.h"
#include "errorlogger.h"
#include "mathlib.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "utils.h"

#include <cstddef>
#include <list>
#include <vector>
#include <stack>
#include <utility>

//---------------------------------------------------------------------------

// Register this check class (by creating a static instance of it)
namespace {
    CheckString instance;
}

// CWE ids used:
static const struct CWE CWE570(570U);   // Expression is Always False
static const struct CWE CWE571(571U);   // Expression is Always True
static const struct CWE CWE595(595U);   // Comparison of Object References Instead of Object Contents
static const struct CWE CWE628(628U);   // Function Call with Incorrectly Specified Arguments
static const struct CWE CWE665(665U);   // Improper Initialization
static const struct CWE CWE758(758U);   // Reliance on Undefined, Unspecified, or Implementation-Defined Behavior

//---------------------------------------------------------------------------
// Writing string literal is UB
//---------------------------------------------------------------------------
void CheckString::stringLiteralWrite()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (!tok->variable() || !tok->variable()->isPointer())
                continue;
            const Token *str = tok->getValueTokenMinStrSize();
            if (!str)
                continue;
            if (match1(tok) && match2(tok->linkAt(1)))
                stringLiteralWriteError(tok, str);
            else if (match3(tok->previous()))
                stringLiteralWriteError(tok, str);
        }
    }
}

void CheckString::stringLiteralWriteError(const Token *tok, const Token *strValue)
{
    std::list<const Token *> callstack;
    callstack.push_back(tok);
    if (strValue)
        callstack.push_back(strValue);

    std::string errmsg("Modifying string literal");
    if (strValue) {
        std::string s = strValue->strValue();
        if (s.size() > 15U)
            s = s.substr(0,13) + "..";
        errmsg += " \"" + s + "\"";
    }
    errmsg += " directly or indirectly is undefined behaviour.";

    reportError(callstack, Severity::error, "stringLiteralWrite", errmsg, CWE758, false);
}

//---------------------------------------------------------------------------
// Check for string comparison involving two static strings.
// if(strcmp("00FF00","00FF00")==0) // <- statement is always true
//---------------------------------------------------------------------------
void CheckString::checkAlwaysTrueOrFalseStringCompare()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    for (const Token* tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (tok->isName() && tok->strAt(1) == MatchCompiler::makeConstString("(") && match4(tok)) {
            if (match5(tok->tokAt(2))) {
                const std::string &str1 = tok->strAt(2);
                const std::string &str2 = tok->strAt(4);
                if (!tok->isExpandedMacro() && !tok->tokAt(2)->isExpandedMacro() && !tok->tokAt(4)->isExpandedMacro())
                    alwaysTrueFalseStringCompareError(tok, str1, str2);
                tok = tok->tokAt(5);
            } else if (match6(tok->tokAt(2))) {
                const std::string &str1 = tok->strAt(2);
                const std::string &str2 = tok->strAt(4);
                if (str1 == str2)
                    alwaysTrueStringVariableCompareError(tok, str1, str2);
                tok = tok->tokAt(5);
            } else if (match7(tok->tokAt(2))) {
                const std::string &str1 = tok->strAt(2);
                const std::string &str2 = tok->strAt(8);
                if (str1 == str2)
                    alwaysTrueStringVariableCompareError(tok, str1, str2);
                tok = tok->tokAt(13);
            }
        } else if (tok->isName() && match8(tok)) {
            const std::string &str1 = tok->strAt(4);
            const std::string &str2 = tok->strAt(6);
            alwaysTrueFalseStringCompareError(tok, str1, str2);
            tok = tok->tokAt(7);
        } else if (match9(tok)) {
            const std::string &str1 = tok->strAt(1);
            const std::string &str2 = tok->strAt(3);
            alwaysTrueFalseStringCompareError(tok, str1, str2);
            tok = tok->tokAt(5);
        }
        if (!tok)
            break;
    }
}

void CheckString::alwaysTrueFalseStringCompareError(const Token *tok, const std::string& str1, const std::string& str2)
{
    const std::size_t stringLen = 10;
    const std::string string1 = (str1.size() < stringLen) ? str1 : (str1.substr(0, stringLen-2) + "..");
    const std::string string2 = (str2.size() < stringLen) ? str2 : (str2.substr(0, stringLen-2) + "..");

    reportError(tok, Severity::warning, "staticStringCompare",
                "Unnecessary comparison of static strings.\n"
                "The compared strings, '" + string1 + "' and '" + string2 + "', are always " + (str1==str2?"identical":"unequal") + ". "
                "Therefore the comparison is unnecessary and looks suspicious.", (str1==str2)?CWE571:CWE570, false);
}

void CheckString::alwaysTrueStringVariableCompareError(const Token *tok, const std::string& str1, const std::string& str2)
{
    reportError(tok, Severity::warning, "stringCompare",
                "Comparison of identical string variables.\n"
                "The compared strings, '" + str1 + "' and '" + str2 + "', are identical. "
                "This could be a logic bug.", CWE571, false);
}


//-----------------------------------------------------------------------------
// Detect "str == '\0'" where "*str == '\0'" is correct.
// Comparing char* with each other instead of using strcmp()
//-----------------------------------------------------------------------------
void CheckString::checkSuspiciousStringCompare()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (tok->tokType() != Token::eComparisonOp)
                continue;

            const Token* varTok = tok->astOperand1();
            const Token* litTok = tok->astOperand2();
            if (!varTok || !litTok)  // <- failed to create AST for comparison
                continue;
            if (match10(varTok))
                std::swap(varTok, litTok);
            else if (!match10(litTok))
                continue;

            // Pointer addition?
            if (varTok->str() == MatchCompiler::makeConstString("+") && mTokenizer->isC()) {
                const Token * const tokens[2] = { varTok->astOperand1(), varTok->astOperand2() };
                for (int nr = 0; nr < 2; nr++) {
                    const Token *t = tokens[nr];
                    while (t && (t->str() == MatchCompiler::makeConstString(".") || t->str() == MatchCompiler::makeConstString("::")))
                        t = t->astOperand2();
                    if (t && t->variable() && t->variable()->isPointer())
                        varTok = t;
                }
            }

            if (varTok->str() == MatchCompiler::makeConstString("*")) {
                if (!mTokenizer->isC() || varTok->astOperand2() != nullptr || litTok->tokType() != Token::eString)
                    continue;
                varTok = varTok->astOperand1();
            }

            while (varTok && (varTok->str() == MatchCompiler::makeConstString(".") || varTok->str() == MatchCompiler::makeConstString("::")))
                varTok = varTok->astOperand2();
            if (!varTok || !varTok->isName())
                continue;

            const Variable *var = varTok->variable();

            while (match11(varTok->astParent()))
                varTok = varTok->astParent();
            const std::string varname = varTok->expressionString();

            const bool ischar(litTok->tokType() == Token::eChar);
            if (litTok->tokType() == Token::eString) {
                if (mTokenizer->isC() || (var && var->isArrayOrPointer()))
                    suspiciousStringCompareError(tok, varname);
            } else if (ischar && var && var->isPointer()) {
                suspiciousStringCompareError_char(tok, varname);
            }
        }
    }
}

void CheckString::suspiciousStringCompareError(const Token* tok, const std::string& var)
{
    reportError(tok, Severity::warning, "literalWithCharPtrCompare",
                "$symbol:" + var + "\nString literal compared with variable '$symbol'. Did you intend to use strcmp() instead?", CWE595, false);
}

void CheckString::suspiciousStringCompareError_char(const Token* tok, const std::string& var)
{
    reportError(tok, Severity::warning, "charLiteralWithCharPtrCompare",
                "$symbol:" + var + "\nChar literal compared with pointer '$symbol'. Did you intend to dereference it?", CWE595, false);
}


//---------------------------------------------------------------------------
// Adding C-string and char with operator+
//---------------------------------------------------------------------------

static bool isChar(const Variable* var)
{
    return (var && !var->isPointer() && !var->isArray() && var->typeStartToken()->str() == MatchCompiler::makeConstString("char"));
}

void CheckString::strPlusChar()
{
    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("+")) {
                if (tok->astOperand1() && (tok->astOperand1()->tokType() == Token::eString)) { // string literal...
                    if (tok->astOperand2() && (tok->astOperand2()->tokType() == Token::eChar || isChar(tok->astOperand2()->variable()))) // added to char variable or char constant
                        strPlusCharError(tok);
                }
            }
        }
    }
}

void CheckString::strPlusCharError(const Token *tok)
{
    reportError(tok, Severity::error, "strPlusChar", "Unusual pointer arithmetic. A value of type 'char' is added to a string literal.", CWE665, false);
}

//---------------------------------------------------------------------------
// Implicit casts of string literals to bool
// Comparing string literal with strlen() with wrong length
//---------------------------------------------------------------------------
void CheckString::checkIncorrectStringCompare()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            // skip "assert(str && ..)" and "assert(.. && str)"
            if ((endsWith(tok->str(), "assert", 6) || endsWith(tok->str(), "ASSERT", 6)) &&
                match12(tok) &&
                (match13(tok->tokAt(2)) || match14(tok->next()->link()->tokAt(-2))))
                tok = tok->next()->link();

            if (match16(tok) && match15(tok->tokAt(3)->nextArgument())) {
                const MathLib::biguint clen = MathLib::toULongNumber(tok->linkAt(2)->strAt(-1));
                const Token* begin = tok->previous();
                for (;;) { // Find start of statement
                    while (begin->link() && match17(begin))
                        begin = begin->link()->previous();
                    if (match18(begin->previous()))
                        begin = begin->tokAt(-2);
                    else
                        break;
                }
                begin = begin->previous();
                const Token* end = tok->linkAt(2)->next();
                if (match19(begin->previous()) && begin->strAt(-2) != MatchCompiler::makeConstString("+")) {
                    const std::size_t slen = Token::getStrLength(begin->previous());
                    if (clen != slen) {
                        incorrectStringCompareError(tok->next(), "substr", begin->strAt(-1));
                    }
                } else if (match20(end)) {
                    const std::size_t slen = Token::getStrLength(end->next());
                    if (clen != slen) {
                        incorrectStringCompareError(tok->next(), "substr", end->strAt(1));
                    }
                }
            } else if (match21(tok) && !match22(tok)) {
                incorrectStringBooleanError(tok->next(), tok->strAt(1));
            } else if (match23(tok) && !tok->tokAt(2)->getValue(0)) {
                incorrectStringBooleanError(tok->tokAt(2), tok->strAt(2));
            }
        }
    }
}

void CheckString::incorrectStringCompareError(const Token *tok, const std::string& func, const std::string &string)
{
    reportError(tok, Severity::warning, "incorrectStringCompare", "$symbol:" + func + "\nString literal " + string + " doesn't match length argument for $symbol().", CWE570, false);
}

void CheckString::incorrectStringBooleanError(const Token *tok, const std::string& string)
{
    const bool charLiteral = string[0] == '\'';
    const std::string literalType = charLiteral ? "char" : "string";
    const std::string result = (string == MatchCompiler::makeConstString("\'\\0\'")) ? "false" : "true";
    reportError(tok,
                Severity::warning,
                charLiteral ? "incorrectCharBooleanError" : "incorrectStringBooleanError",
                "Conversion of " + literalType + " literal " + string + " to bool always evaluates to " + result + '.', CWE571, false);
}

//---------------------------------------------------------------------------
// always true: strcmp(str,"a")==0 || strcmp(str,"b")
// TODO: Library configuration for string comparison functions
//---------------------------------------------------------------------------
void CheckString::overlappingStrcmp()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (tok->str() != MatchCompiler::makeConstString("||"))
                continue;
            std::list<const Token *> equals0;
            std::list<const Token *> notEquals0;
            std::stack<const Token *> tokens;
            tokens.push(tok);
            while (!tokens.empty()) {
                const Token * const t = tokens.top();
                tokens.pop();
                if (!t)
                    continue;
                if (t->str() == MatchCompiler::makeConstString("||")) {
                    tokens.push(t->astOperand1());
                    tokens.push(t->astOperand2());
                    continue;
                }
                if (t->str() == MatchCompiler::makeConstString("==")) {
                    if (match24(t->astOperand1()) && match25(t->astOperand2()))
                        equals0.push_back(t->astOperand1());
                    else if (match24(t->astOperand2()) && match25(t->astOperand1()))
                        equals0.push_back(t->astOperand2());
                    continue;
                }
                if (t->str() == MatchCompiler::makeConstString("!=")) {
                    if (match24(t->astOperand1()) && match25(t->astOperand2()))
                        notEquals0.push_back(t->astOperand1());
                    else if (match24(t->astOperand2()) && match25(t->astOperand1()))
                        notEquals0.push_back(t->astOperand2());
                    continue;
                }
                if (t->str() == MatchCompiler::makeConstString("!") && match24(t->astOperand1()))
                    equals0.push_back(t->astOperand1());
                else if (t->str() == MatchCompiler::makeConstString("("))
                    notEquals0.push_back(t);
            }

            for (const Token *eq0 : equals0) {
                for (const Token * ne0 : notEquals0) {
                    if (!match26(eq0->previous()))
                        continue;
                    if (!match26(ne0->previous()))
                        continue;
                    const std::vector<const Token *> args1 = getArguments(eq0->previous());
                    const std::vector<const Token *> args2 = getArguments(ne0->previous());
                    if (args1.size() != 2 || args2.size() != 2)
                        continue;
                    if (args1[1]->isLiteral() &&
                        args2[1]->isLiteral() &&
                        args1[1]->str() != args2[1]->str() &&
                        isSameExpression(mTokenizer->isCPP(), true, args1[0], args2[0], mSettings->library, true, false))
                        overlappingStrcmpError(eq0, ne0);
                }
            }
        }
    }
}

void CheckString::overlappingStrcmpError(const Token *eq0, const Token *ne0)
{
    std::string eq0Expr(eq0 ? eq0->expressionString() : std::string("strcmp(x,\"abc\")"));
    if (eq0 && eq0->astParent()->str() == MatchCompiler::makeConstString("!"))
        eq0Expr = "!" + eq0Expr;
    else
        eq0Expr += " == 0";

    const std::string ne0Expr = (ne0 ? ne0->expressionString() : std::string("strcmp(x,\"def\")")) + " != 0";

    reportError(ne0, Severity::warning, "overlappingStrcmp", "The expression '" + ne0Expr + "' is suspicious. It overlaps '" + eq0Expr + "'.");
}

//---------------------------------------------------------------------------
// Overlapping source and destination passed to sprintf().
// TODO: Library configuration for overlapping arguments
//---------------------------------------------------------------------------
void CheckString::sprintfOverlappingData()
{
    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (!match27(tok))
                continue;

            const std::vector<const Token *> args = getArguments(tok);

            const int formatString = match28(tok) ? 1 : 2;
            for (unsigned int argnr = formatString + 1; argnr < args.size(); ++argnr) {
                const bool same = isSameExpression(mTokenizer->isCPP(),
                                                   false,
                                                   args[0],
                                                   args[argnr],
                                                   mSettings->library,
                                                   true,
                                                   false);
                if (same) {
                    sprintfOverlappingDataError(args[argnr], args[argnr]->expressionString());
                }
            }
        }
    }
}

void CheckString::sprintfOverlappingDataError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::error, "sprintfOverlappingData",
                "$symbol:" + varname + "\n"
                "Undefined behavior: Variable '$symbol' is used as parameter and destination in s[n]printf().\n"
                "The variable '$symbol' is used both as a parameter and as destination in "
                "s[n]printf(). The origin and destination buffers overlap. Quote from glibc (C-library) "
                "documentation (http://www.gnu.org/software/libc/manual/html_mono/libc.html#Formatted-Output-Functions): "
                "\"If copying takes place between objects that overlap as a result of a call "
                "to sprintf() or snprintf(), the results are undefined.\"", CWE628, false);
}
