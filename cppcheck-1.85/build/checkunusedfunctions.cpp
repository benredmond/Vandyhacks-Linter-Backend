#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: %name% (
static bool match1(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %name% <
static bool match2(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > (
static bool match3(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: [;{}.,()[=+-/|!?:]
static bool match4(const Token* tok) {
    if (!tok || tok->str().size()!=1U || !strchr(";{}.,()[=+-/|!?:", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% :: %name%
static bool match5(const Token* tok) {
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
// pattern: %name% [(),;]:}>]
static bool match6(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr("(),;]:}>", tok->str()[0]))
        return false;
    return true;
}
// pattern: %name% (|<
static bool match7(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("(")) || (tok->str()==MatchCompiler::makeConstString("<"))))
        return false;
    return true;
}
// pattern: ) const|throw|{
static bool match8(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("throw")) || (tok->str()==MatchCompiler::makeConstString("{"))))
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
#include "checkunusedfunctions.h"

#include "errorlogger.h"
#include "library.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "tokenlist.h"

#include <tinyxml2.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <istream>
#include <utility>
//---------------------------------------------------------------------------



// Register this check class
CheckUnusedFunctions CheckUnusedFunctions::instance;

static const struct CWE CWE561(561U);   // Dead Code


//---------------------------------------------------------------------------
// FUNCTION USAGE - Check for unused functions etc
//---------------------------------------------------------------------------

void CheckUnusedFunctions::parseTokens(const Tokenizer &tokenizer, const char FileName[], const Settings *settings)
{
    const bool doMarkup = settings->library.markupFile(FileName);
    const SymbolDatabase* symbolDatabase = tokenizer.getSymbolDatabase();

    // Function declarations..
    for (std::size_t i = 0; i < symbolDatabase->functionScopes.size(); i++) {
        const Scope* scope = symbolDatabase->functionScopes[i];
        const Function* func = scope->function;
        if (!func || !func->token || scope->bodyStart->fileIndex() != 0)
            continue;

        // Don't warn about functions that are marked by __attribute__((constructor)) or __attribute__((destructor))
        if (func->isAttributeConstructor() || func->isAttributeDestructor() || func->type != Function::eFunction || func->isOperator())
            continue;

        // Don't care about templates
        if (tokenizer.isCPP()) {
            const Token *retDef = func->retDef;
            while (retDef && retDef->isName())
                retDef = retDef->previous();
            if (retDef && retDef->str() == MatchCompiler::makeConstString(">"))
                continue;
        }

        mFunctionDecl.emplace_back(func);

        FunctionUsage &usage = mFunctions[func->name()];

        if (!usage.lineNumber)
            usage.lineNumber = func->token->linenr();

        // No filename set yet..
        if (usage.filename.empty()) {
            usage.filename = tokenizer.list.getSourceFilePath();
        }
        // Multiple files => filename = "+"
        else if (usage.filename != tokenizer.list.getSourceFilePath()) {
            //func.filename = "+";
            usage.usedOtherFile |= usage.usedSameFile;
        }
    }

    // Function usage..
    for (const Token *tok = tokenizer.tokens(); tok; tok = tok->next()) {

        // parsing of library code to find called functions
        if (settings->library.isexecutableblock(FileName, tok->str())) {
            const Token * markupVarToken = tok->tokAt(settings->library.blockstartoffset(FileName));
            // not found
            if (!markupVarToken)
                continue;
            int scope = 0;
            bool start = true;
            // find all function calls in library code (starts with '(', not if or while etc)
            while ((scope || start) && markupVarToken) {
                if (markupVarToken->str() == settings->library.blockstart(FileName)) {
                    scope++;
                    if (start) {
                        start = false;
                    }
                } else if (markupVarToken->str() == settings->library.blockend(FileName))
                    scope--;
                else if (!settings->library.iskeyword(FileName, markupVarToken->str())) {
                    mFunctionCalls.insert(markupVarToken->str());
                    if (mFunctions.find(markupVarToken->str()) != mFunctions.end())
                        mFunctions[markupVarToken->str()].usedOtherFile = true;
                    else if (markupVarToken->next()->str() == MatchCompiler::makeConstString("(")) {
                        FunctionUsage &func = mFunctions[markupVarToken->str()];
                        func.filename = tokenizer.list.getSourceFilePath();
                        if (func.filename.empty() || func.filename == MatchCompiler::makeConstString("+"))
                            func.usedOtherFile = true;
                        else
                            func.usedSameFile = true;
                    }
                }
                markupVarToken = markupVarToken->next();
            }
        }

        if (!doMarkup // only check source files
            && settings->library.isexporter(tok->str()) && tok->next() != nullptr) {
            const Token * propToken = tok->next();
            while (propToken && propToken->str() != MatchCompiler::makeConstString(")")) {
                if (settings->library.isexportedprefix(tok->str(), propToken->str())) {
                    const Token* nextPropToken = propToken->next();
                    const std::string& value = nextPropToken->str();
                    if (mFunctions.find(value) != mFunctions.end()) {
                        mFunctions[value].usedOtherFile = true;
                    }
                    mFunctionCalls.insert(value);
                }
                if (settings->library.isexportedsuffix(tok->str(), propToken->str())) {
                    const Token* prevPropToken = propToken->previous();
                    const std::string& value = prevPropToken->str();
                    if (value != MatchCompiler::makeConstString(")") && mFunctions.find(value) != mFunctions.end()) {
                        mFunctions[value].usedOtherFile = true;
                    }
                    mFunctionCalls.insert(value);
                }
                propToken = propToken->next();
            }
        }

        if (doMarkup && settings->library.isimporter(FileName, tok->str()) && tok->next()) {
            const Token * propToken = tok->next();
            if (propToken->next()) {
                propToken = propToken->next();
                while (propToken && propToken->str() != MatchCompiler::makeConstString(")")) {
                    const std::string& value = propToken->str();
                    if (!value.empty()) {
                        mFunctions[value].usedOtherFile = true;
                        mFunctionCalls.insert(value);
                        break;
                    }
                    propToken = propToken->next();
                }
            }
        }

        if (settings->library.isreflection(tok->str())) {
            const int argIndex = settings->library.reflectionArgument(tok->str());
            if (argIndex >= 0) {
                const Token * funcToken = tok->next();
                int index = 0;
                std::string value;
                while (funcToken) {
                    if (funcToken->str()==MatchCompiler::makeConstString(",")) {
                        if (++index == argIndex)
                            break;
                        value.clear();
                    } else
                        value += funcToken->str();
                    funcToken = funcToken->next();
                }
                if (index == argIndex) {
                    value = value.substr(1, value.length() - 2);
                    mFunctions[value].usedOtherFile = true;
                    mFunctionCalls.insert(value);
                }
            }
        }

        const Token *funcname = nullptr;

        if (tok->scope()->isExecutable() && match1(tok)) {
            funcname = tok;
        } else if (tok->scope()->isExecutable() && match2(tok) && match3(tok->linkAt(1))) {
            funcname = tok;
        } else if (match4(tok)) {
            funcname = tok->next();
            if (funcname && funcname->str() == MatchCompiler::makeConstString("&"))
                funcname = funcname->next();
            if (funcname && funcname->str() == MatchCompiler::makeConstString("::"))
                funcname = funcname->next();
            while (match5(funcname))
                funcname = funcname->tokAt(2);

            if (!match6(funcname))
                continue;
        }

        if (!funcname)
            continue;

        // funcname ( => Assert that the end parentheses isn't followed by {
        if (match7(funcname)) {
            const Token *ftok = funcname->next();
            if (ftok->str() == MatchCompiler::makeConstString("<"))
                ftok = ftok->link();
            if (match8(ftok->linkAt(1)))
                funcname = nullptr;
        }

        if (funcname) {
            FunctionUsage &func = mFunctions[ funcname->str()];
            const std::string& called_from_file = tokenizer.list.getSourceFilePath();

            if (func.filename.empty() || func.filename == MatchCompiler::makeConstString("+") || func.filename != called_from_file)
                func.usedOtherFile = true;
            else
                func.usedSameFile = true;

            mFunctionCalls.insert(funcname->str());
        }
    }
}


static bool isOperatorFunction(const std::string & funcName)
{
    /* Operator functions are invalid function names for C, so no need to check
     * this in here. As result the returned error function might be incorrect.
     *
     * List of valid operators can be found at:
     * http://en.cppreference.com/w/cpp/language/operators
     *
     * Conversion functions must be a member function (at least for gcc), so no
     * need to cover them for unused functions.
     *
     * To speed up the comparison, not the whole list of operators is used.
     * Instead only the character after the operator prefix is checked to be a
     * none alpa numeric value, but the '_', to cover function names like
     * "operator_unused". In addition the following valid operators are checked:
     * - new
     * - new[]
     * - delete
     * - delete[]
     */
    const std::string operatorPrefix = "operator";
    if (funcName.compare(0, operatorPrefix.length(), operatorPrefix) != 0) {
        return false;
    }

    // Taking care of funcName == MatchCompiler::makeConstString("operator"), which is no valid operator
    if (funcName.length() == operatorPrefix.length()) {
        return false;
    }

    const char firstOperatorChar = funcName[operatorPrefix.length()];
    if (firstOperatorChar == '_') {
        return false;
    }

    if (!std::isalnum(firstOperatorChar)) {
        return true;
    }

    const std::vector<std::string> additionalOperators = {
        "new", "new[]", "delete", "delete[]"
    };


    return std::find(additionalOperators.begin(), additionalOperators.end(), funcName.substr(operatorPrefix.length())) != additionalOperators.end();;
}



bool CheckUnusedFunctions::check(ErrorLogger * const errorLogger, const Settings& settings)
{
    bool errors = false;
    for (std::map<std::string, FunctionUsage>::const_iterator it = mFunctions.begin(); it != mFunctions.end(); ++it) {
        const FunctionUsage &func = it->second;
        if (func.usedOtherFile || func.filename.empty())
            continue;
        if (it->first == MatchCompiler::makeConstString("main") ||
            (settings.isWindowsPlatform() && (it->first == MatchCompiler::makeConstString("WinMain") || it->first == MatchCompiler::makeConstString("_tmain"))) ||
            it->first == MatchCompiler::makeConstString("if"))
            continue;
        if (!func.usedSameFile) {
            if (isOperatorFunction(it->first))
                continue;
            std::string filename;
            if (func.filename != MatchCompiler::makeConstString("+"))
                filename = func.filename;
            unusedFunctionError(errorLogger, filename, func.lineNumber, it->first);
            errors = true;
        } else if (! func.usedOtherFile) {
            /** @todo add error message "function is only used in <file> it can be static" */
            /*
            std::ostringstream errmsg;
            errmsg << "The function '" << it->first << "' is only used in the file it was declared in so it should have local linkage.";
            mErrorLogger->reportErr( errmsg.str() );
            errors = true;
            */
        }
    }
    return errors;
}

void CheckUnusedFunctions::unusedFunctionError(ErrorLogger * const errorLogger,
        const std::string &filename, unsigned int lineNumber,
        const std::string &funcname)
{
    std::list<ErrorLogger::ErrorMessage::FileLocation> locationList;
    if (!filename.empty()) {
        ErrorLogger::ErrorMessage::FileLocation fileLoc;
        fileLoc.setfile(filename);
        fileLoc.line = lineNumber;
        locationList.push_back(fileLoc);
    }

    const ErrorLogger::ErrorMessage errmsg(locationList, emptyString, Severity::style, "$symbol:" + funcname + "\nThe function '$symbol' is never used.", "unusedFunction", CWE561, false);
    if (errorLogger)
        errorLogger->reportErr(errmsg);
    else
        reportError(errmsg);
}

Check::FileInfo *CheckUnusedFunctions::getFileInfo(const Tokenizer *tokenizer, const Settings *settings) const
{
    if (!settings->isEnabled(Settings::UNUSED_FUNCTION))
        return nullptr;
    if (settings->jobs == 1 && settings->buildDir.empty())
        instance.parseTokens(*tokenizer, tokenizer->list.getFiles().front().c_str(), settings);
    return nullptr;
}

bool CheckUnusedFunctions::analyseWholeProgram(const std::list<Check::FileInfo*> &fileInfo, const Settings& settings, ErrorLogger &errorLogger)
{
    (void)fileInfo;
    return check(&errorLogger, settings);
}

CheckUnusedFunctions::FunctionDecl::FunctionDecl(const Function *f)
    : functionName(f->name()), lineNumber(f->token->linenr())
{
}

std::string CheckUnusedFunctions::analyzerInfo() const
{
    std::ostringstream ret;
    for (std::list<FunctionDecl>::const_iterator it = mFunctionDecl.begin(); it != mFunctionDecl.end(); ++it) {
        ret << "    <functiondecl"
            << " functionName=\"" << ErrorLogger::toxml(it->functionName) << '\"'
            << " lineNumber=\"" << it->lineNumber << "\"/>\n";
    }
    for (std::set<std::string>::const_iterator it = mFunctionCalls.begin(); it != mFunctionCalls.end(); ++it) {
        ret << "    <functioncall functionName=\"" << ErrorLogger::toxml(*it) << "\"/>\n";
    }
    return ret.str();
}

namespace {
    struct Location {
        Location() : lineNumber(0) {}
        Location(const std::string &f, const int l) : fileName(f), lineNumber(l) {}
        std::string fileName;
        int lineNumber;
    };
}

void CheckUnusedFunctions::analyseWholeProgram(ErrorLogger * const errorLogger, const std::string &buildDir)
{
    std::map<std::string, Location> decls;
    std::set<std::string> calls;

    const std::string filesTxt(buildDir + "/files.txt");
    std::ifstream fin(filesTxt.c_str());
    std::string filesTxtLine;
    while (std::getline(fin, filesTxtLine)) {
        const std::string::size_type firstColon = filesTxtLine.find(':');
        if (firstColon == std::string::npos)
            continue;
        const std::string::size_type secondColon = filesTxtLine.find(':', firstColon+1);
        if (secondColon == std::string::npos)
            continue;
        const std::string xmlfile = buildDir + '/' + filesTxtLine.substr(0,firstColon);
        const std::string sourcefile = filesTxtLine.substr(secondColon+1);

        tinyxml2::XMLDocument doc;
        const tinyxml2::XMLError error = doc.LoadFile(xmlfile.c_str());
        if (error != tinyxml2::XML_SUCCESS)
            continue;

        const tinyxml2::XMLElement * const rootNode = doc.FirstChildElement();
        if (rootNode == nullptr)
            continue;

        for (const tinyxml2::XMLElement *e = rootNode->FirstChildElement(); e; e = e->NextSiblingElement()) {
            if (std::strcmp(e->Name(), "FileInfo") != 0)
                continue;
            const char *checkattr = e->Attribute("check");
            if (checkattr == nullptr || std::strcmp(checkattr,"CheckUnusedFunctions") != 0)
                continue;
            for (const tinyxml2::XMLElement *e2 = e->FirstChildElement(); e2; e2 = e2->NextSiblingElement()) {
                const char* functionName = e2->Attribute("functionName");
                if (functionName == nullptr)
                    continue;
                if (std::strcmp(e2->Name(),"functioncall") == 0) {
                    calls.insert(functionName);
                    continue;
                } else if (std::strcmp(e2->Name(),"functiondecl") == 0) {
                    const char* lineNumber = e2->Attribute("lineNumber");
                    if (lineNumber)
                        decls[functionName] = Location(sourcefile, std::atoi(lineNumber));
                }
            }
        }
    }

    for (std::map<std::string, Location>::const_iterator decl = decls.begin(); decl != decls.end(); ++decl) {
        const std::string &functionName = decl->first;

        if (functionName == MatchCompiler::makeConstString("main") || functionName == MatchCompiler::makeConstString("WinMain") || functionName == MatchCompiler::makeConstString("_tmain") ||
            functionName == MatchCompiler::makeConstString("if"))
            continue;

        if (calls.find(functionName) == calls.end() && !isOperatorFunction(functionName)) {
            const Location &loc = decl->second;
            unusedFunctionError(errorLogger, loc.fileName, loc.lineNumber, functionName);
        }
    }
}
