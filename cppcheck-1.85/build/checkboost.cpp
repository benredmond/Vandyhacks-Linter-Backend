#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: BOOST_FOREACH (
static bool match1(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("BOOST_FOREACH")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %var% ) {
static bool match2(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("{")))
        return false;
    return true;
}
// pattern: %varid% . insert|erase|push_back|push_front|pop_front|pop_back|clear|swap|resize|assign|merge|remove|remove_if|reverse|sort|splice|unique|pop|push
static bool match3(const Token* tok, const unsigned int varid) {
    if (varid==0U)
        throw InternalError(tok, "Internal error. Token::Match called with varid 0. Please report this to Cppcheck developers");
    if (!tok || !(tok->isName() && tok->varId()==varid))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("insert")) || (tok->str()==MatchCompiler::makeConstString("erase")) || (tok->str()==MatchCompiler::makeConstString("push_back")) || (tok->str()==MatchCompiler::makeConstString("push_front")) || (tok->str()==MatchCompiler::makeConstString("pop_front")) || (tok->str()==MatchCompiler::makeConstString("pop_back")) || (tok->str()==MatchCompiler::makeConstString("clear")) || (tok->str()==MatchCompiler::makeConstString("swap")) || (tok->str()==MatchCompiler::makeConstString("resize")) || (tok->str()==MatchCompiler::makeConstString("assign")) || (tok->str()==MatchCompiler::makeConstString("merge")) || (tok->str()==MatchCompiler::makeConstString("remove")) || (tok->str()==MatchCompiler::makeConstString("remove_if")) || (tok->str()==MatchCompiler::makeConstString("reverse")) || (tok->str()==MatchCompiler::makeConstString("sort")) || (tok->str()==MatchCompiler::makeConstString("splice")) || (tok->str()==MatchCompiler::makeConstString("unique")) || (tok->str()==MatchCompiler::makeConstString("pop")) || (tok->str()==MatchCompiler::makeConstString("push"))))
        return false;
    return true;
}
// pattern: ;
template<class T> static T * findmatch4(T * start_tok, const Token * end) {
    for (; start_tok && start_tok != end; start_tok = start_tok->next()) {

    T * tok = start_tok;
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        continue;
    return start_tok;
    }
    return NULL;
}
// pattern: ; break|return|throw
static bool match5(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(";")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("break")) || (tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
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

#include "checkboost.h"

#include "errorlogger.h"
#include "symboldatabase.h"
#include "token.h"

#include <cstddef>

// Register this check class (by creating a static instance of it)
namespace {
    CheckBoost instance;
}

static const CWE CWE664(664);

void CheckBoost::checkBoostForeachModification()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart->next(); tok && tok != scope->bodyEnd; tok = tok->next()) {
            if (!match1(tok))
                continue;

            const Token *containerTok = tok->next()->link()->previous();
            if (!match2(containerTok))
                continue;

            const Token *tok2 = containerTok->tokAt(2);
            const Token *end = tok2->link();
            for (; tok2 != end; tok2 = tok2->next()) {
                if (match3(tok2, containerTok->varId())) {
                    const Token* nextStatement = findmatch4(tok2->linkAt(3), end) ;
                    if (!match5(nextStatement))
                        boostForeachError(tok2);
                    break;
                }
            }
        }
    }
}

void CheckBoost::boostForeachError(const Token *tok)
{
    reportError(tok, Severity::error, "boostForeachError",
                "BOOST_FOREACH caches the end() iterator. It's undefined behavior if you modify the container inside.", CWE664, false
               );
}
