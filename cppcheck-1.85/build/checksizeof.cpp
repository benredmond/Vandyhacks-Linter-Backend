#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: sizeof ( %num% )
static bool match1(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
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
// pattern: sizeof %num%
static bool match2(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isNumber())
        return false;
    return true;
}
// pattern: sizeof ( %var% )
static bool match3(const Token* tok) {
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
    return true;
}
// pattern: sizeof %var% !![
static bool match4(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("["))
        return false;
    return true;
}
// pattern: malloc|alloca|calloc (
static bool match5(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("malloc")) || (tok->str()==MatchCompiler::makeConstString("alloca")) || (tok->str()==MatchCompiler::makeConstString("calloc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% =
static bool match6(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: > ( malloc|alloca|calloc (
static bool match7(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("malloc")) || (tok->str()==MatchCompiler::makeConstString("alloca")) || (tok->str()==MatchCompiler::makeConstString("calloc"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: memset (
static bool match8(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("memset")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: memcpy|memcmp|memmove|strncpy|strncmp|strncat (
static bool match9(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("memcpy")) || (tok->str()==MatchCompiler::makeConstString("memcmp")) || (tok->str()==MatchCompiler::makeConstString("memmove")) || (tok->str()==MatchCompiler::makeConstString("strncpy")) || (tok->str()==MatchCompiler::makeConstString("strncmp")) || (tok->str()==MatchCompiler::makeConstString("strncat"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: / sizeof
static bool match10(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("/")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    return true;
}
// pattern: sizeof (
static bool match11(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% ::|.
static bool match12(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("::")) || (tok->str()==MatchCompiler::makeConstString("."))))
        return false;
    return true;
}
// pattern: sizeof ( &
static bool match13(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: sizeof (|&
static bool match14(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("&"))))
        return false;
    return true;
}
// pattern: %var% [|(
static bool match15(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("[")) || (tok->str()==MatchCompiler::makeConstString("("))))
        return false;
    return true;
}
// pattern: sizeof (| sizeof
static bool match16(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    return true;
}
// pattern: ( void )
static bool match17(const Token* tok) {
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
// pattern: static_cast<void>
static bool match18(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("static_cast<void>")))
        return false;
    return true;
}
// pattern: ) * sizeof
static bool match19(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    return true;
}
// pattern: sizeof ( )
static bool match20(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("sizeof")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    return true;
}
// pattern: +|++|--|+=|-=
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("+")) || (tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--")) || (tok->str()==MatchCompiler::makeConstString("+=")) || (tok->str()==MatchCompiler::makeConstString("-="))))
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
#include "checksizeof.h"

#include "errorlogger.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"

#include <cstddef>
//---------------------------------------------------------------------------

// Register this check class (by creating a static instance of it)
namespace {
    CheckSizeof instance;
}

// CWE IDs used:
static const struct CWE CWE398(398U);   // Indicator of Poor Code Quality
static const struct CWE CWE467(467U);   // Use of sizeof() on a Pointer Type
static const struct CWE CWE682(682U);   // Incorrect Calculation
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CheckSizeof::checkSizeofForNumericParameter()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match1(tok) ||
                match2(tok)) {
                sizeofForNumericParameterError(tok);
            }
        }
    }
}

void CheckSizeof::sizeofForNumericParameterError(const Token *tok)
{
    reportError(tok, Severity::warning,
                "sizeofwithnumericparameter", "Suspicious usage of 'sizeof' with a numeric constant as parameter.\n"
                "It is unusual to use a constant value with sizeof. For example, 'sizeof(10)'"
                " returns 4 (in 32-bit systems) or 8 (in 64-bit systems) instead of 10. 'sizeof('A')'"
                " and 'sizeof(char)' can return different results.", CWE682, false);
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CheckSizeof::checkSizeofForArrayParameter()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (match3(tok) ||
                match4(tok)) {
                const Token* varTok = tok->next();
                if (varTok->str() == MatchCompiler::makeConstString("(")) {
                    varTok = varTok->next();
                }

                const Variable *var = varTok->variable();
                if (var && var->isArray() && var->isArgument() && !var->isReference())
                    sizeofForArrayParameterError(tok);
            }
        }
    }
}

void CheckSizeof::sizeofForArrayParameterError(const Token *tok)
{
    reportError(tok, Severity::warning,
                "sizeofwithsilentarraypointer", "Using 'sizeof' on array given as function argument "
                "returns size of a pointer.\n"
                "Using 'sizeof' for array given as function argument returns the size of a pointer. "
                "It does not return the size of the whole array in bytes as might be "
                "expected. For example, this code:\n"
                "     int f(char a[100]) {\n"
                "         return sizeof(a);\n"
                "     }\n"
                "returns 4 (in 32-bit systems) or 8 (in 64-bit systems) instead of 100 (the "
                "size of the array in bytes).", CWE467, false
               );
}

void CheckSizeof::checkSizeofForPointerSize()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token* tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            const Token* tokSize;
            const Token* tokFunc;
            const Token *variable = nullptr;
            const Token *variable2 = nullptr;

            // Find any function that may use sizeof on a pointer
            // Once leaving those tests, it is mandatory to have:
            // - variable matching the used pointer
            // - tokVar pointing on the argument where sizeof may be used
            if (match5(tok->tokAt(2))) {
                if (match6(tok))
                    variable = tok;
                else if (tok->strAt(1) == MatchCompiler::makeConstString(")") && match6(tok->linkAt(1)->tokAt(-2)))
                    variable = tok->linkAt(1)->tokAt(-2);
                else if (tok->link() && match7(tok) && match6(tok->link()->tokAt(-3)))
                    variable = tok->link()->tokAt(-3);
                tokSize = tok->tokAt(4);
                tokFunc = tok->tokAt(2);
            } else if (match8(tok) && tok->strAt(-1) != MatchCompiler::makeConstString(".")) {
                variable = tok->tokAt(2);
                tokSize = variable->nextArgument();
                if (tokSize)
                    tokSize = tokSize->nextArgument();
                tokFunc = tok;
            } else if (match9(tok) && tok->strAt(-1) != MatchCompiler::makeConstString(".")) {
                variable = tok->tokAt(2);
                variable2 = variable->nextArgument();
                if (!variable2)
                    continue;
                tokSize = variable2->nextArgument();
                tokFunc = tok;
            } else {
                continue;
            }

            if (tokSize && tokFunc->str() == MatchCompiler::makeConstString("calloc"))
                tokSize = tokSize->nextArgument();

            if (tokSize) {
                const Token * const paramsListEndTok = tokFunc->linkAt(1);
                for (const Token* tok2 = tokSize; tok2 != paramsListEndTok; tok2 = tok2->next()) {
                    if (match10(tok2)) {
                        // Allow division with sizeof(char)
                        if (match11(tok2->next())) {
                            const Token *sztok = tok2->tokAt(2)->astOperand2();
                            const ValueType *vt = ((sztok != nullptr) ? sztok->valueType() : nullptr);
                            if (vt && vt->type == ValueType::CHAR && vt->pointer == 0)
                                continue;
                        }

                        divideBySizeofError(tok2, tokFunc->str());
                    }
                }
            }

            if (!variable || !tokSize)
                continue;

            while (match12(variable))
                variable = variable->tokAt(2);

            while (match12(variable2))
                variable2 = variable2->tokAt(2);

            // Ensure the variables are in the symbol database
            // Also ensure the variables are pointers
            // Only keep variables which are pointers
            const Variable *var = variable->variable();
            if (!var || !var->isPointer() || var->isArray()) {
                variable = nullptr;
            }

            if (variable2) {
                var = variable2->variable();
                if (!var || !var->isPointer() || var->isArray()) {
                    variable2 = nullptr;
                }
            }

            // If there are no pointer variable at this point, there is
            // no need to continue
            if (variable == nullptr && variable2 == nullptr) {
                continue;
            }

            // Jump to the next sizeof token in the function and in the parameter
            // This is to allow generic operations with sizeof
            for (; tokSize && tokSize->str() != MatchCompiler::makeConstString(")") && tokSize->str() != MatchCompiler::makeConstString(",") && tokSize->str() != MatchCompiler::makeConstString("sizeof"); tokSize = tokSize->next()) {}

            if (tokSize->str() != MatchCompiler::makeConstString("sizeof"))
                continue;

            // Now check for the sizeof usage: Does the level of pointer indirection match?
            if (tokSize->linkAt(1)->strAt(-1) == MatchCompiler::makeConstString("*")) {
                if (variable && variable->valueType() && variable->valueType()->pointer == 1 && variable->valueType()->type != ValueType::VOID)
                    sizeofForPointerError(variable, variable->str());
                else if (variable2 && variable2->valueType() && variable2->valueType()->pointer == 1 && variable2->valueType()->type != ValueType::VOID)
                    sizeofForPointerError(variable2, variable2->str());
            }

            if (match13(tokSize))
                tokSize = tokSize->tokAt(3);
            else if (match14(tokSize))
                tokSize = tokSize->tokAt(2);
            else
                tokSize = tokSize->next();

            while (match12(tokSize))
                tokSize = tokSize->tokAt(2);

            if (match15(tokSize))
                continue;

            // Now check for the sizeof usage again. Once here, everything using sizeof(varid) or sizeof(&varid)
            // looks suspicious
            if (variable && tokSize->varId() == variable->varId())
                sizeofForPointerError(variable, variable->str());
            if (variable2 && tokSize->varId() == variable2->varId())
                sizeofForPointerError(variable2, variable2->str());
        }
    }
}

void CheckSizeof::sizeofForPointerError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::warning, "pointerSize",
                "Size of pointer '" + varname + "' used instead of size of its data.\n"
                "Size of pointer '" + varname + "' used instead of size of its data. "
                "This is likely to lead to a buffer overflow. You probably intend to "
                "write 'sizeof(*" + varname + ")'.", CWE467, false);
}

void CheckSizeof::divideBySizeofError(const Token *tok, const std::string &memfunc)
{
    reportError(tok, Severity::warning, "sizeofDivisionMemfunc",
                "Division by result of sizeof(). " + memfunc + "() expects a size in bytes, did you intend to multiply instead?", CWE682, false);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CheckSizeof::sizeofsizeof()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match16(tok)) {
            sizeofsizeofError(tok);
            tok = tok->next();
        }
    }
}

void CheckSizeof::sizeofsizeofError(const Token *tok)
{
    reportError(tok, Severity::warning,
                "sizeofsizeof", "Calling 'sizeof' on 'sizeof'.\n"
                "Calling sizeof for 'sizeof looks like a suspicious code and "
                "most likely there should be just one 'sizeof'. The current "
                "code is equivalent to 'sizeof(size_t)'", CWE682, false);
}

//-----------------------------------------------------------------------------

void CheckSizeof::sizeofCalculation()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const bool printInconclusive = mSettings->inconclusive;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (!match11(tok))
            continue;

        // ignore if the `sizeof` result is cast to void inside a macro, i.e. the calculation is
        // expected to be parsed but skipped, such as in a disabled custom ASSERT() macro
        if (tok->isExpandedMacro() && tok->previous()) {
            const Token *cast_end = (tok->previous()->str() == MatchCompiler::makeConstString("(")) ? tok->previous() : tok;
            if (match17(cast_end->tokAt(-3)) ||
                match18(cast_end->previous())) {
                continue;
            }
        }

        const Token *argument = tok->next()->astOperand2();
        if (argument && argument->isCalculation() && (!argument->isExpandedMacro() || printInconclusive))
            sizeofCalculationError(argument, argument->isExpandedMacro());
    }
}

void CheckSizeof::sizeofCalculationError(const Token *tok, bool inconclusive)
{
    reportError(tok, Severity::warning,
                "sizeofCalculation", "Found calculation inside sizeof().", CWE682, inconclusive);
}

//-----------------------------------------------------------------------------

void CheckSizeof::sizeofFunction()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match11(tok)) {

            // ignore if the `sizeof` result is cast to void inside a macro, i.e. the calculation is
            // expected to be parsed but skipped, such as in a disabled custom ASSERT() macro
            if (tok->isExpandedMacro() && tok->previous()) {
                const Token *cast_end = (tok->previous()->str() == MatchCompiler::makeConstString("(")) ? tok->previous() : tok;
                if (match17(cast_end->tokAt(-3)) ||
                    match18(cast_end->previous())) {
                    continue;
                }
            }

            if (const Token *argument = tok->next()->astOperand2()) {
                const Token *checkToken = argument->previous();
                if (checkToken->tokType() == Token::eName)
                    break;
                const Function * fun = checkToken->function();
                // Don't report error if the function is overloaded
                if (fun && fun->nestedIn->functionMap.count(checkToken->str()) == 1) {
                    sizeofFunctionError(tok);
                }
            }
        }
    }
}

void CheckSizeof::sizeofFunctionError(const Token *tok)
{
    reportError(tok, Severity::warning,
                "sizeofFunctionCall", "Found function call inside sizeof().", CWE682, false);
}

//-----------------------------------------------------------------------------
// Check for code like sizeof()*sizeof() or sizeof(ptr)/value
//-----------------------------------------------------------------------------
void CheckSizeof::suspiciousSizeofCalculation()
{
    if (!mSettings->isEnabled(Settings::WARNING) || !mSettings->inconclusive)
        return;

    // TODO: Use AST here. This should be possible as soon as sizeof without brackets is correctly parsed
    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match11(tok)) {
            const Token* const end = tok->linkAt(1);
            const Variable* var = end->previous()->variable();
            if (end->strAt(-1) == MatchCompiler::makeConstString("*") || (var && var->isPointer() && !var->isArray())) {
                if (end->strAt(1) == MatchCompiler::makeConstString("/"))
                    divideSizeofError(tok);
            } else if (match19(end) && end->next()->astOperand1() == tok->next())
                multiplySizeofError(tok);
        }
    }
}

void CheckSizeof::multiplySizeofError(const Token *tok)
{
    reportError(tok, Severity::warning,
                "multiplySizeof", "Multiplying sizeof() with sizeof() indicates a logic error.", CWE682, true);
}

void CheckSizeof::divideSizeofError(const Token *tok)
{
    reportError(tok, Severity::warning,
                "divideSizeof", "Division of result of sizeof() on pointer type.\n"
                "Division of result of sizeof() on pointer type. sizeof() returns the size of the pointer, "
                "not the size of the memory area it points to.", CWE682, true);
}

void CheckSizeof::sizeofVoid()
{
    if (!mSettings->isEnabled(Settings::PORTABILITY))
        return;

    for (const Token *tok = mTokenizer->tokens(); tok; tok = tok->next()) {
        if (match20(tok)) { // "sizeof(void)" gets simplified to sizeof ( )
            sizeofVoidError(tok);
        } else if (match11(tok) && tok->next()->astOperand2()) {
            const ValueType *vt = tok->next()->astOperand2()->valueType();
            if (vt && vt->type == ValueType::Type::VOID && vt->pointer == 0U)
                sizeofDereferencedVoidPointerError(tok, tok->strAt(3));
        } else if (tok->str() == MatchCompiler::makeConstString("-")) {
            // only warn for: 'void *' - 'integral'
            const ValueType *vt1  = tok->astOperand1() ? tok->astOperand1()->valueType() : nullptr;
            const ValueType *vt2  = tok->astOperand2() ? tok->astOperand2()->valueType() : nullptr;
            const bool op1IsvoidPointer = (vt1 && vt1->type == ValueType::Type::VOID && vt1->pointer == 1U);
            const bool op2IsIntegral    = (vt2 && vt2->isIntegral() && vt2->pointer == 0U);
            if (op1IsvoidPointer && op2IsIntegral)
                arithOperationsOnVoidPointerError(tok, tok->astOperand1()->expressionString(), vt1->str());
        } else if (match21(tok)) { // Arithmetic operations on variable of type "void*"
            const ValueType *vt1 = tok->astOperand1() ? tok->astOperand1()->valueType() : nullptr;
            const ValueType *vt2 = tok->astOperand2() ? tok->astOperand2()->valueType() : nullptr;

            const bool voidpointer1 = (vt1 && vt1->type == ValueType::Type::VOID && vt1->pointer == 1U);
            const bool voidpointer2 = (vt2 && vt2->type == ValueType::Type::VOID && vt2->pointer == 1U);

            if (voidpointer1)
                arithOperationsOnVoidPointerError(tok, tok->astOperand1()->expressionString(), vt1->str());

            if (!tok->isAssignmentOp() && voidpointer2)
                arithOperationsOnVoidPointerError(tok, tok->astOperand2()->expressionString(), vt2->str());
        }
    }
}

void CheckSizeof::sizeofVoidError(const Token *tok)
{
    const std::string message = "Behaviour of 'sizeof(void)' is not covered by the ISO C standard.";
    const std::string verbose = message + " A value for 'sizeof(void)' is defined only as part of a GNU C extension, which defines 'sizeof(void)' to be 1.";
    reportError(tok, Severity::portability, "sizeofVoid", message + "\n" + verbose, CWE682, false);
}

void CheckSizeof::sizeofDereferencedVoidPointerError(const Token *tok, const std::string &varname)
{
    const std::string message = "'*" + varname + "' is of type 'void', the behaviour of 'sizeof(void)' is not covered by the ISO C standard.";
    const std::string verbose = message + " A value for 'sizeof(void)' is defined only as part of a GNU C extension, which defines 'sizeof(void)' to be 1.";
    reportError(tok, Severity::portability, "sizeofDereferencedVoidPointer", message + "\n" + verbose, CWE682, false);
}

void CheckSizeof::arithOperationsOnVoidPointerError(const Token* tok, const std::string &varname, const std::string &vartype)
{
    const std::string message = "'$symbol' is of type '" + vartype + "'. When using void pointers in calculations, the behaviour is undefined.";
    const std::string verbose = message + " Arithmetic operations on 'void *' is a GNU C extension, which defines the 'sizeof(void)' to be 1.";
    reportError(tok, Severity::portability, "arithOperationsOnVoidPointer", "$symbol:" + varname + '\n' + message + '\n' + verbose, CWE467, false);
}
