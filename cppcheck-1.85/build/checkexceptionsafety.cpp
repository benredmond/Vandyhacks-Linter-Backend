#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: try {
static bool match1(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("try")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: if ( ! std :: uncaught_exception ( ) ) {
static bool match2(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("if")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("!")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("uncaught_exception")))
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
    return true;
}
// pattern: [ ]
static bool match3(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("[")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    return true;
}
// pattern: %var% ;
static bool match4(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    return true;
}
// pattern: %varid% =
static bool match5(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: [,(] &| %varid% [,)]
static bool match6(const Token* tok, const unsigned int varid) {
    if (!tok || tok->str().size()!=1U || !strchr(",(", tok->str()[0]))
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
// pattern: catch (
static bool match7(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("catch")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: throw %varid% ;
static bool match8(const Token* tok, const unsigned int varid) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("throw")))
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
#include "checkexceptionsafety.h"

#include "settings.h"
#include "symboldatabase.h"

#include <cstddef>
#include <set>
#include <utility>

//---------------------------------------------------------------------------

// Register CheckExceptionSafety..
namespace {
    CheckExceptionSafety instance;
}


//---------------------------------------------------------------------------

void CheckExceptionSafety::destructors()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    // Perform check..
    for (const Scope * scope : symbolDatabase->functionScopes) {
        const Function * function = scope->function;
        if (!function)
            continue;
        // only looking for destructors
        if (function->type == Function::eDestructor) {
            // Inspect this destructor.
            for (const Token *tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
                // Skip try blocks
                if (match1(tok)) {
                    tok = tok->next()->link();
                }

                // Skip uncaught exceptions
                else if (match2(tok)) {
                    tok = tok->next()->link(); // end of if ( ... )
                    tok = tok->next()->link(); // end of { ... }
                }

                // throw found within a destructor
                else if (tok->str() == MatchCompiler::makeConstString("throw")) {
                    destructorsError(tok, scope->className);
                    break;
                }
            }
        }
    }
}




void CheckExceptionSafety::deallocThrow()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const bool printInconclusive = mSettings->inconclusive;
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    // Deallocate a global/member pointer and then throw exception
    // the pointer will be a dead pointer
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            // only looking for delete now
            if (tok->str() != MatchCompiler::makeConstString("delete"))
                continue;

            // Check if this is something similar with: "delete p;"
            tok = tok->next();
            if (match3(tok))
                tok = tok->tokAt(2);
            if (!tok || tok == scope->bodyEnd)
                break;
            if (!match4(tok))
                continue;

            // we only look for global variables
            const Variable *var = tok->variable();
            if (!var || !(var->isGlobal() || var->isStatic()))
                continue;

            const unsigned int varid(tok->varId());

            // Token where throw occurs
            const Token *throwToken = nullptr;

            // is there a throw after the deallocation?
            const Token* const end2 = tok->scope()->bodyEnd;
            for (const Token *tok2 = tok; tok2 != end2; tok2 = tok2->next()) {
                // Throw after delete -> Dead pointer
                if (tok2->str() == MatchCompiler::makeConstString("throw")) {
                    if (printInconclusive) { // For inconclusive checking, throw directly.
                        deallocThrowError(tok2, tok->str());
                        break;
                    }
                    throwToken = tok2;
                }

                // Variable is assigned -> Bail out
                else if (match5(tok2, varid)) {
                    if (throwToken) // For non-inconclusive checking, wait until we find an assignment to it. Otherwise we assume it is safe to leave a dead pointer.
                        deallocThrowError(throwToken, tok2->str());
                    break;
                }
                // Variable passed to function. Assume it becomes assigned -> Bail out
                else if (match6(tok2, varid)) // TODO: No bailout if passed by value or as const reference
                    break;
            }
        }
    }
}

//---------------------------------------------------------------------------
//      catch(const exception & err)
//      {
//         throw err;            // <- should be just "throw;"
//      }
//---------------------------------------------------------------------------
void CheckExceptionSafety::checkRethrowCopy()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eCatch)
            continue;

        const unsigned int varid = scope.bodyStart->tokAt(-2)->varId();
        if (varid) {
            for (const Token* tok = scope.bodyStart->next(); tok && tok != scope.bodyEnd; tok = tok->next()) {
                if (match7(tok) && tok->next()->link() && tok->next()->link()->next()) { // Don't check inner catch - it is handled in another iteration of outer loop.
                    tok = tok->next()->link()->next()->link();
                    if (!tok)
                        break;
                } else if (match8(tok, varid))
                    rethrowCopyError(tok, tok->strAt(1));
            }
        }
    }
}

//---------------------------------------------------------------------------
//    try {} catch (std::exception err) {} <- Should be "std::exception& err"
//---------------------------------------------------------------------------
void CheckExceptionSafety::checkCatchExceptionByValue()
{
    if (!mSettings->isEnabled(Settings::STYLE))
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope &scope : symbolDatabase->scopeList) {
        if (scope.type != Scope::eCatch)
            continue;

        // Find a pass-by-value declaration in the catch(), excluding basic types
        // e.g. catch (std::exception err)
        const Variable *var = scope.bodyStart->tokAt(-2)->variable();
        if (var && var->isClass() && !var->isPointer() && !var->isReference())
            catchExceptionByValueError(scope.classDef);
    }
}


static const Token * functionThrowsRecursive(const Function * function, std::set<const Function *> & recursive)
{
    // check for recursion and bail if found
    if (!recursive.insert(function).second)
        return nullptr;

    if (!function->functionScope)
        return nullptr;

    for (const Token *tok = function->functionScope->bodyStart->next();
         tok != function->functionScope->bodyEnd; tok = tok->next()) {
        if (tok->str() == MatchCompiler::makeConstString("try")) {
            // just bail for now
            break;
        }
        if (tok->str() == MatchCompiler::makeConstString("throw")) {
            return tok;
        } else if (tok->function()) {
            const Function * called = tok->function();
            // check if called function has an exception specification
            if (called->isThrow() && called->throwArg) {
                return tok;
            } else if (called->isNoExcept() && called->noexceptArg &&
                       called->noexceptArg->str() != MatchCompiler::makeConstString("true")) {
                return tok;
            } else if (functionThrowsRecursive(called, recursive)) {
                return tok;
            }
        }
    }

    return nullptr;
}

static const Token * functionThrows(const Function * function)
{
    std::set<const Function *>  recursive;

    return functionThrowsRecursive(function, recursive);
}

//--------------------------------------------------------------------------
//    void func() noexcept { throw x; }
//    void func() throw() { throw x; }
//    void func() __attribute__((nothrow)); void func() { throw x; }
//--------------------------------------------------------------------------
void CheckExceptionSafety::nothrowThrows()
{
    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope * scope : symbolDatabase->functionScopes) {
        const Function* function = scope->function;
        if (!function)
            continue;

        // check noexcept and noexcept(true) functions
        if (function->isNoExcept() &&
            (!function->noexceptArg || function->noexceptArg->str() == MatchCompiler::makeConstString("true"))) {
            const Token *throws = functionThrows(function);
            if (throws)
                noexceptThrowError(throws);
        }

        // check throw() functions
        else if (function->isThrow() && !function->throwArg) {
            const Token *throws = functionThrows(function);
            if (throws)
                noexceptThrowError(throws);
        }

        // check __attribute__((nothrow)) or __declspec(nothrow) functions
        else if (function->isAttributeNothrow()) {
            const Token *throws = functionThrows(function);
            if (throws)
                noexceptThrowError(throws);
        }
    }
}

//--------------------------------------------------------------------------
//    void func() { functionWithExceptionSpecification(); }
//--------------------------------------------------------------------------
void CheckExceptionSafety::unhandledExceptionSpecification()
{
    if (!mSettings->isEnabled(Settings::STYLE) || !mSettings->inconclusive)
        return;

    const SymbolDatabase* const symbolDatabase = mTokenizer->getSymbolDatabase();

    for (const Scope * scope : symbolDatabase->functionScopes) {
        // only check functions without exception epecification
        if (scope->function && !scope->function->isThrow() &&
            scope->className != MatchCompiler::makeConstString("main") && scope->className != MatchCompiler::makeConstString("wmain") &&
            scope->className != MatchCompiler::makeConstString("_tmain") && scope->className != MatchCompiler::makeConstString("WinMain")) {
            for (const Token *tok = scope->function->functionScope->bodyStart->next();
                 tok != scope->function->functionScope->bodyEnd; tok = tok->next()) {
                if (tok->str() == MatchCompiler::makeConstString("try")) {
                    break;
                } else if (tok->function()) {
                    const Function * called = tok->function();
                    // check if called function has an exception specification
                    if (called->isThrow() && called->throwArg) {
                        unhandledExceptionSpecificationError(tok, called->tokenDef, scope->function->name());
                        break;
                    }
                }
            }
        }
    }
}

