#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %name% .|::
static bool match1(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(".")) || (tok->str()==MatchCompiler::makeConstString("::"))))
        return false;
    return true;
}
// pattern: %name% (
static bool match2(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [
static bool match3(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: & %name% [
static bool match4(const Token* tok) {
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
// pattern: %name% . %var% [
static bool match5(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: asm (
static bool match6(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("asm")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ++|--
static bool match7(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("++")) || (tok->str()==MatchCompiler::makeConstString("--"))))
        return false;
    return true;
}
// pattern: %var%
static bool match8(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: = %varid% ;
static bool match9(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
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
// pattern: ;
template<class T> static T * findmatch10(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: goto
template<class T> static T * findmatch11(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("goto")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: [;{}] %var% = & %var%
static bool match12(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
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
    return true;
}
// pattern: [;{}] * %var% = & %var%
static bool match13(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
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
    return true;
}
// pattern: [;{}] %var% = &| %var% ;
static bool match14(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("&"))))
        tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: [;{}] %var% . %var% = & %var%
static bool match15(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
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
    return true;
}
// pattern: [;{}] %var% . %var% = %var% ;
static bool match16(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
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
// pattern: [;{}] * %var% = %var% ;
static bool match17(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
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
// pattern: %type% * *
static bool match18(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: [;{}] %var% [
static bool match19(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}", tok->str()[0]))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    return true;
}
// pattern: ] = & %var%
static bool match20(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
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
    return true;
}
// pattern: return %var% ;
static bool match21(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: = & %var%
static bool match22(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: return & %var% ;
static bool match23(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
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
// pattern: %name% ( %var% ) ;
static bool match24(const Token* tok) {
    if (!tok || !tok->isName())
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
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: delete [| ]| (| %var% !![
static bool match25(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("["))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("]"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("["))
        return false;
    return true;
}
// pattern: %var%
template<class T> static T * findmatch26(T * start_tok) {
    for (; start_tok; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->varId() != 0))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: %name% ( & %var% ) ;
static bool match27(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: delete [| ]| (| & %var% !![
static bool match28(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("delete")))
        return false;
    tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("["))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("]"))))
        tok = tok->next();
    if (tok && ((tok->str()==MatchCompiler::makeConstString("("))))
        tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("["))
        return false;
    return true;
}
// pattern: std ::
static bool match29(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: > ::
static bool match30(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: %var% [=(]
static bool match31(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("=(", tok->str()[0]))
        return false;
    return true;
}
// pattern: %var% [);.]
static bool match32(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(");.", tok->str()[0]))
        return false;
    return true;
}
// pattern: return %name% (
static bool match33(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("return")))
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
static bool match34(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
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
// Auto variables checks
//---------------------------------------------------------------------------

#include "checkautovariables.h"

#include "astutils.h"
#include "errorlogger.h"
#include "library.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "valueflow.h"

#include <cstddef>
#include <list>

//---------------------------------------------------------------------------


// Register this check class into cppcheck by creating a static instance of it..
namespace {
    CheckAutoVariables instance;
}

static const CWE CWE398(398U);  // Indicator of Poor Code Quality
static const CWE CWE562(562U);  // Return of Stack Variable Address
static const CWE CWE590(590U);  // Free of Memory not on the Heap

static bool isPtrArg(const Token *tok)
{
    const Variable *var = tok->variable();
    return (var && var->isArgument() && var->isPointer());
}

static bool isGlobalPtr(const Token *tok)
{
    const Variable *var = tok->variable();
    return (var && var->isGlobal() && var->isPointer());
}

static bool isArrayArg(const Token *tok)
{
    const Variable *var = tok->variable();
    return (var && var->isArgument() && var->isArray());
}

static bool isRefPtrArg(const Token *tok)
{
    const Variable *var = tok->variable();
    return (var && var->isArgument() && var->isReference() && var->isPointer());
}

static bool isNonReferenceArg(const Token *tok)
{
    const Variable *var = tok->variable();
    return (var && var->isArgument() && !var->isReference() && (var->isPointer() || var->valueType()->type >= ValueType::Type::CONTAINER || var->type()));
}

static bool isAutoVar(const Token *tok)
{
    const Variable *var = tok->variable();

    if (!var || !var->isLocal() || var->isStatic())
        return false;

    if (var->isReference()) {
        // address of reference variable can be taken if the address
        // of the variable it points at is not a auto-var
        // TODO: check what the reference variable references.
        return false;
    }

    if (match1(tok)) {
        do {
            tok = tok->tokAt(2);
        } while (match1(tok));
        if (match2(tok))
            return false;
    }
    return true;
}

static bool isAutoVarArray(const Token *tok)
{
    if (!tok)
        return false;

    // &x[..]
    if (tok->isUnaryOp("&") && match3(tok->astOperand1()))
        return isAutoVarArray(tok->astOperand1()->astOperand1());

    // x+y
    if (tok->str() == MatchCompiler::makeConstString("+"))
        return isAutoVarArray(tok->astOperand1()) || isAutoVarArray(tok->astOperand2());

    // x-intexpr
    if (tok->str() == MatchCompiler::makeConstString("-"))
        return isAutoVarArray(tok->astOperand1()) &&
               tok->astOperand2() &&
               tok->astOperand2()->valueType() &&
               tok->astOperand2()->valueType()->isIntegral();

    const Variable *var = tok->variable();
    if (!var)
        return false;

    // Variable
    if (var->isLocal() && !var->isStatic() && var->isArray() && !var->isPointer())
        return true;

    // ValueFlow
    if (var->isPointer() && !var->isArgument()) {
        for (std::list<ValueFlow::Value>::const_iterator it = tok->values().begin(); it != tok->values().end(); ++it) {
            const ValueFlow::Value &val = *it;
            if (val.isTokValue() && isAutoVarArray(val.tokvalue))
                return true;
        }
    }

    return false;
}

// Verification that we really take the address of a local variable
static bool checkRvalueExpression(const Token * const vartok)
{
    const Variable * const var = vartok->variable();
    if (var == nullptr)
        return false;

    if (match4(vartok->previous()) && var->isPointer())
        return false;

    const Token * const next = vartok->next();
    // &a.b[0]
    if (match5(vartok) && !var->isPointer()) {
        const Variable *var2 = next->next()->variable();
        return var2 && !var2->isPointer();
    }

    return ((next->str() != MatchCompiler::makeConstString(".") || (!var->isPointer() && (!var->isClass() || var->type()))) && next->strAt(2) != MatchCompiler::makeConstString("."));
}

static bool variableIsUsedInScope(const Token* start, unsigned int varId, const Scope *scope)
{
    if (!start) // Ticket #5024
        return false;

    for (const Token *tok = start; tok && tok != scope->bodyEnd; tok = tok->next()) {
        if (tok->varId() == varId)
            return true;
        const Scope::ScopeType scopeType = tok->scope()->type;
        if (scopeType == Scope::eFor || scopeType == Scope::eDo || scopeType == Scope::eWhile) // In case of loops, better checking would be necessary
            return true;
        if (match6(tok))
            return true;
    }
    return false;
}

void CheckAutoVariables::assignFunctionArg()
{
    const bool printStyle = mSettings->isEnabled(Settings::STYLE);
    const bool printWarning = mSettings->isEnabled(Settings::WARNING);
    if (!printStyle && !printWarning)
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart; tok && tok != scope->bodyEnd; tok = tok->next()) {
            // TODO: What happens if this is removed?
            if (tok->astParent())
                continue;
            if (!(tok->isAssignmentOp() || match7(tok)) || !match8(tok->astOperand1()))
                continue;
            const Token* const vartok = tok->astOperand1();
            if (isNonReferenceArg(vartok) &&
                !match9(vartok->next(), vartok->varId()) &&
                !variableIsUsedInScope(findmatch10(vartok->next()) , vartok->varId(), scope) &&
                !findmatch11(vartok, scope->bodyEnd) ) {
                if (vartok->variable()->isPointer() && printWarning)
                    errorUselessAssignmentPtrArg(vartok);
                else if (printStyle)
                    errorUselessAssignmentArg(vartok);
            }
        }
    }
}

static bool reassignedGlobalPointer(const Token *vartok, unsigned int pointerVarId, const Settings *settings, bool cpp)
{
    return isVariableChanged(vartok,
                             vartok->variable()->scope()->bodyEnd,
                             pointerVarId,
                             true,
                             settings,
                             cpp);
}


void CheckAutoVariables::autoVariables()
{
    const bool printInconclusive = mSettings->inconclusive;
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart; tok && tok != scope->bodyEnd; tok = tok->next()) {
            // Critical assignment
            if (match12(tok) && isRefPtrArg(tok->next()) && isAutoVar(tok->tokAt(4))) {
                if (checkRvalueExpression(tok->tokAt(4)))
                    errorAutoVariableAssignment(tok->next(), false);
            } else if (match13(tok) && isPtrArg(tok->tokAt(2)) && isAutoVar(tok->tokAt(5))) {
                if (checkRvalueExpression(tok->tokAt(5)))
                    errorAutoVariableAssignment(tok->next(), false);
            } else if (mSettings->isEnabled(Settings::WARNING) && match14(tok) && isGlobalPtr(tok->next())) {
                const Token * const pointer = tok->next();
                if (isAutoVarArray(tok->tokAt(3))) {
                    const Token * const array = tok->tokAt(3);
                    if (!reassignedGlobalPointer(array, pointer->varId(), mSettings, mTokenizer->isCPP()))
                        errorAssignAddressOfLocalArrayToGlobalPointer(pointer, array);
                } else if (isAutoVar(tok->tokAt(4))) {
                    const Token * const variable = tok->tokAt(4);
                    if (!reassignedGlobalPointer(variable, pointer->varId(), mSettings, mTokenizer->isCPP()))
                        errorAssignAddressOfLocalVariableToGlobalPointer(pointer, variable);
                }
            } else if (match15(tok)) {
                // TODO: check if the parameter is only changed temporarily (#2969)
                if (printInconclusive && isPtrArg(tok->next())) {
                    const Token * const var2tok = tok->tokAt(6);
                    if (isAutoVar(var2tok) && checkRvalueExpression(var2tok))
                        errorAutoVariableAssignment(tok->next(), true);
                }
                tok = tok->tokAt(6);
            } else if (match16(tok)) {
                // TODO: check if the parameter is only changed temporarily (#2969)
                if (printInconclusive && isPtrArg(tok->next())) {
                    if (isAutoVarArray(tok->tokAt(5)))
                        errorAutoVariableAssignment(tok->next(), true);
                }
                tok = tok->tokAt(5);
            } else if (match17(tok)) {
                const Variable * var1 = tok->tokAt(2)->variable();
                if (var1 && var1->isArgument() && match18(var1->nameToken()->tokAt(-3))) {
                    if (isAutoVarArray(tok->tokAt(4)))
                        errorAutoVariableAssignment(tok->next(), false);
                }
                tok = tok->tokAt(4);
            } else if (match19(tok) && match20(tok->linkAt(2)) &&
                       (isPtrArg(tok->next()) || isArrayArg(tok->next())) && isAutoVar(tok->linkAt(2)->tokAt(3))) {
                const Token* const varTok = tok->linkAt(2)->tokAt(3);
                if (checkRvalueExpression(varTok))
                    errorAutoVariableAssignment(tok->next(), false);
            }
            // Critical return
            else if (match21(tok) && isAutoVar(tok->next())) {
                const std::list<ValueFlow::Value> &values = tok->next()->values();
                const ValueFlow::Value *value = nullptr;
                for (std::list<ValueFlow::Value>::const_iterator it = values.begin(); it != values.end(); ++it) {
                    if (!it->isTokValue())
                        continue;
                    if (!mSettings->inconclusive && it->isInconclusive())
                        continue;
                    if (!match22(it->tokvalue->previous()))
                        continue;
                    if (!isAutoVar(it->tokvalue->next()))
                        continue;
                    if (!value || value->isInconclusive())
                        value = &(*it);
                }

                if (value)
                    errorReturnAddressToAutoVariable(tok, value);
            }

            else if (match23(tok)) {
                const Token* varTok = tok->tokAt(2);
                if (isAutoVar(varTok))
                    errorReturnAddressToAutoVariable(tok);
                else if (varTok->varId()) {
                    const Variable * var1 = varTok->variable();
                    if (var1 && var1->isArgument() && var1->typeEndToken()->str() != MatchCompiler::makeConstString("&"))
                        errorReturnAddressOfFunctionParameter(tok, varTok->str());
                }
            }
            // Invalid pointer deallocation
            else if ((match24(tok) && mSettings->library.dealloc(tok)) ||
                     (mTokenizer->isCPP() && match25(tok))) {
                tok = findmatch26(tok->next()) ;
                if (isAutoVarArray(tok))
                    errorInvalidDeallocation(tok);
            } else if ((match27(tok) && mSettings->library.dealloc(tok)) ||
                       (mTokenizer->isCPP() && match28(tok))) {
                tok = findmatch26(tok->next()) ;
                if (isAutoVar(tok))
                    errorInvalidDeallocation(tok);
            }
        }
    }
}

//---------------------------------------------------------------------------

void CheckAutoVariables::returnPointerToLocalArray()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope * scope : symbolDatabase->functionScopes) {
        if (!scope->function)
            continue;

        const Token *tok = scope->function->tokenDef;

        // have we reached a function that returns a pointer
        if (tok->previous() && tok->previous()->str() == MatchCompiler::makeConstString("*")) {
            for (const Token *tok2 = scope->bodyStart->next(); tok2 && tok2 != scope->bodyEnd; tok2 = tok2->next()) {
                // Return pointer to local array variable..
                if (tok2 ->str() == MatchCompiler::makeConstString("return") && isAutoVarArray(tok2->astOperand1())) {
                    errorReturnPointerToLocalArray(tok2);
                }
            }
        }
    }
}

void CheckAutoVariables::errorReturnAddressToAutoVariable(const Token *tok)
{
    reportError(tok, Severity::error, "returnAddressOfAutoVariable", "Address of an auto-variable returned.", CWE562, false);
}

void CheckAutoVariables::errorReturnAddressToAutoVariable(const Token *tok, const ValueFlow::Value *value)
{
    reportError(tok, Severity::error, "returnAddressOfAutoVariable", "Address of auto-variable '" + value->tokvalue->astOperand1()->expressionString() + "' returned", CWE562, false);
}

void CheckAutoVariables::errorReturnPointerToLocalArray(const Token *tok)
{
    reportError(tok, Severity::error, "returnLocalVariable", "Pointer to local array variable returned.", CWE562, false);
}

void CheckAutoVariables::errorAutoVariableAssignment(const Token *tok, bool inconclusive)
{
    if (!inconclusive) {
        reportError(tok, Severity::error, "autoVariables",
                    "Address of local auto-variable assigned to a function parameter.\n"
                    "Dangerous assignment - the function parameter is assigned the address of a local "
                    "auto-variable. Local auto-variables are reserved from the stack which "
                    "is freed when the function ends. So the pointer to a local variable "
                    "is invalid after the function ends.", CWE562, false);
    } else {
        reportError(tok, Severity::error, "autoVariables",
                    "Address of local auto-variable assigned to a function parameter.\n"
                    "Function parameter is assigned the address of a local auto-variable. "
                    "Local auto-variables are reserved from the stack which is freed when "
                    "the function ends. The address is invalid after the function ends and it "
                    "might 'leak' from the function through the parameter.",
                    CWE562,
                    true);
    }
}

void CheckAutoVariables::errorAssignAddressOfLocalArrayToGlobalPointer(const Token *pointer, const Token *array)
{
    const std::string pointerName = pointer ? pointer->str() : std::string("pointer");
    const std::string arrayName   = array ? array->str() : std::string("array");
    reportError(pointer, Severity::warning, "autoVariablesAssignGlobalPointer",
                "$symbol:" + arrayName + "\nAddress of local array $symbol is assigned to global pointer " + pointerName +" and not reassigned before $symbol goes out of scope.", CWE562, false);
}

void CheckAutoVariables::errorAssignAddressOfLocalVariableToGlobalPointer(const Token *pointer, const Token *variable)
{
    const std::string pointerName = pointer ? pointer->str() : std::string("pointer");
    const std::string variableName = variable ? variable->str() : std::string("variable");
    reportError(pointer, Severity::warning, "autoVariablesAssignGlobalPointer",
                "$symbol:" + variableName + "\nAddress of local variable $symbol is assigned to global pointer " + pointerName +" and not reassigned before $symbol goes out of scope.", CWE562, false);
}

void CheckAutoVariables::errorReturnAddressOfFunctionParameter(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::error, "returnAddressOfFunctionParameter",
                "$symbol:" + varname + "\n"
                "Address of function parameter '$symbol' returned.\n"
                "Address of the function parameter '$symbol' becomes invalid after the function exits because "
                "function parameters are stored on the stack which is freed when the function exits. Thus the returned "
                "value is invalid.", CWE562, false);
}

void CheckAutoVariables::errorUselessAssignmentArg(const Token *tok)
{
    reportError(tok,
                Severity::style,
                "uselessAssignmentArg",
                "Assignment of function parameter has no effect outside the function.", CWE398, false);
}

void CheckAutoVariables::errorUselessAssignmentPtrArg(const Token *tok)
{
    reportError(tok,
                Severity::warning,
                "uselessAssignmentPtrArg",
                "Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?", CWE398, false);
}

//---------------------------------------------------------------------------

// return temporary?
bool CheckAutoVariables::returnTemporary(const Token *tok)
{
    bool func = false;     // Might it be a function call?
    bool retref = false;   // is there such a function that returns a reference?
    bool retvalue = false; // is there such a function that returns a value?

    const Function *function = tok->function();
    if (function) {
        // Ticket #5478: Only functions or operator equal might return a temporary
        if (function->type != Function::eOperatorEqual && function->type != Function::eFunction)
            return false;
        retref = function->tokenDef->strAt(-1) == MatchCompiler::makeConstString("&");
        if (!retref) {
            const Token *start = function->retDef;
            if (start->str() == MatchCompiler::makeConstString("const"))
                start = start->next();
            if (start->str() == MatchCompiler::makeConstString("::"))
                start = start->next();

            if (match29(start)) {
                if (start->strAt(3) != MatchCompiler::makeConstString("<") || !match30(start->linkAt(3)))
                    retvalue = true;
                else
                    retref = true; // Assume that a reference is returned
            } else {
                if (start->type())
                    retvalue = true;
                else
                    retref = true;
            }
        }
        func = true;
    }
    if (!func && tok->type())
        return true;

    return (!retref && retvalue);
}

//---------------------------------------------------------------------------

static bool astHasAutoResult(const Token *tok)
{
    if (tok->astOperand1() && !astHasAutoResult(tok->astOperand1()))
        return false;
    if (tok->astOperand2() && !astHasAutoResult(tok->astOperand2()))
        return false;

    if (tok->isOp()) {
        if (tok->tokType() == Token::eIncDecOp)
            return false;
        if ((tok->str() == MatchCompiler::makeConstString("<<") || tok->str() == MatchCompiler::makeConstString(">>")) && tok->astOperand1()) {
            const Token* tok2 = tok->astOperand1();
            while (tok2 && tok2->isUnaryOp("*"))
                tok2 = tok2->astOperand1();
            return tok2 && tok2->variable() && !tok2->variable()->isClass() && !tok2->variable()->isStlType(); // Class or unknown type on LHS: Assume it is a stream
        }
        return true;
    }

    if (tok->isLiteral())
        return true;

    if (tok->isName()) {
        // TODO: check function calls, struct members, arrays, etc also
        if (!tok->variable())
            return false;
        if (tok->variable()->isStlType())
            return true;
        if (tok->variable()->isClass() || tok->variable()->isPointer() || tok->variable()->isReference()) // TODO: Properly handle pointers/references to classes in symbol database
            return false;

        return true;
    }

    return false;
}

void CheckAutoVariables::returnReference()
{
    if (mTokenizer->isC())
        return;

    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope * scope : symbolDatabase->functionScopes) {
        if (!scope->function)
            continue;

        const Token *tok = scope->function->tokenDef;

        // have we reached a function that returns a reference?
        if (tok->previous() && tok->previous()->str() == MatchCompiler::makeConstString("&")) {
            for (const Token *tok2 = scope->bodyStart->next(); tok2 && tok2 != scope->bodyEnd; tok2 = tok2->next()) {
                if (!tok2->scope()->isExecutable()) {
                    tok2 = tok2->scope()->bodyEnd;
                    if (!tok2)
                        break;
                    continue;
                }

                // Skip over lambdas
                const Token *lambdaEndToken = findLambdaEndToken(tok2);
                if (lambdaEndToken)
                    tok2 = lambdaEndToken->next();

                if (tok2->str() == MatchCompiler::makeConstString("("))
                    tok2 = tok2->link();

                if (tok2->str() != MatchCompiler::makeConstString("return"))
                    continue;

                // return..
                if (match21(tok2)) {
                    // is the returned variable a local variable?
                    if (isAutoVar(tok2->next())) {
                        const Variable *var1 = tok2->next()->variable();
                        // If reference variable is used, check what it references
                        if (match31(var1->nameToken())) {
                            const Token *tok3 = var1->nameToken()->tokAt(2);
                            if (!match32(tok3))
                                continue;

                            // Only report error if variable that is referenced is
                            // a auto variable
                            if (!isAutoVar(tok3))
                                continue;
                        }

                        // report error..
                        errorReturnReference(tok2);
                    }
                }

                // return reference to temporary..
                else if (match33(tok2) &&
                         match34(tok2->linkAt(2))) {
                    if (returnTemporary(tok2->next())) {
                        // report error..
                        errorReturnTempReference(tok2);
                    }
                }

                // Return reference to a literal or the result of a calculation
                else if (tok2->astOperand1() && (tok2->astOperand1()->isCalculation() || tok2->next()->isLiteral()) && astHasAutoResult(tok2->astOperand1())) {
                    errorReturnTempReference(tok2);
                }
            }
        }
    }
}

void CheckAutoVariables::errorReturnReference(const Token *tok)
{
    reportError(tok, Severity::error, "returnReference", "Reference to auto variable returned.", CWE562, false);
}

void CheckAutoVariables::errorReturnTempReference(const Token *tok)
{
    reportError(tok, Severity::error, "returnTempReference", "Reference to temporary returned.", CWE562, false);
}

void CheckAutoVariables::errorInvalidDeallocation(const Token *tok)
{
    reportError(tok,
                Severity::error,
                "autovarInvalidDeallocation",
                "Deallocation of an auto-variable results in undefined behaviour.\n"
                "The deallocation of an auto-variable results in undefined behaviour. You should only free memory "
                "that has been allocated dynamically.", CWE590, false);
}
