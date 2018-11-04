#include "token.h"
#include "errorlogger.h"
#include "matchcompiler.h"
#include <string>
#include <cstring>
// pattern: std :: cout|cerr !!.
static bool match1(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("cout")) || (tok->str()==MatchCompiler::makeConstString("cerr"))))
        return false;
    tok = tok->next();
    if (tok && tok->str() == MatchCompiler::makeConstString("."))
        return false;
    return true;
}
// pattern: std :: cout|cerr
static bool match2(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("cout")) || (tok->str()==MatchCompiler::makeConstString("cerr"))))
        return false;
    return true;
}
// pattern: FILE *
static bool match3(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("FILE")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("*")))
        return false;
    return true;
}
// pattern: %var% =
static bool match4(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("=")))
        return false;
    return true;
}
// pattern: %name% (
static bool match5(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: return|throw
static bool match6(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("return")) || (tok->str()==MatchCompiler::makeConstString("throw"))))
        return false;
    return true;
}
// pattern: fopen_s|freopen_s|_wfopen_s|_wfreopen_s ( & %name%
static bool match7(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("fopen_s")) || (tok->str()==MatchCompiler::makeConstString("freopen_s")) || (tok->str()==MatchCompiler::makeConstString("_wfopen_s")) || (tok->str()==MatchCompiler::makeConstString("_wfreopen_s"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    tok = tok->next();
    if (!tok || !tok->isName())
        return false;
    return true;
}
// pattern: if|for|while|catch|switch
static bool match8(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("if")) || (tok->str()==MatchCompiler::makeConstString("for")) || (tok->str()==MatchCompiler::makeConstString("while")) || (tok->str()==MatchCompiler::makeConstString("catch")) || (tok->str()==MatchCompiler::makeConstString("switch"))))
        return false;
    return true;
}
// pattern: %name% .
static bool match9(const Token* tok) {
    if (!tok || !tok->isName())
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    return true;
}
// pattern: scanf|vscanf ( %str% ,
static bool match10(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("scanf")) || (tok->str()==MatchCompiler::makeConstString("vscanf"))))
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
    return true;
}
// pattern: sscanf|vsscanf|fscanf|vfscanf (
static bool match11(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sscanf")) || (tok->str()==MatchCompiler::makeConstString("vsscanf")) || (tok->str()==MatchCompiler::makeConstString("fscanf")) || (tok->str()==MatchCompiler::makeConstString("vfscanf"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %str% [,)]
static bool match12(const Token* tok) {
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %var% [,)]
static bool match13(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: char|wchar_t
static bool match14(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("wchar_t"))))
        return false;
    return true;
}
// pattern: swprintf (
static bool match15(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("swprintf")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: %str%
static bool match16(const Token* tok) {
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    return true;
}
// pattern: sprintf_s|swprintf_s (
static bool match17(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("sprintf_s")) || (tok->str()==MatchCompiler::makeConstString("swprintf_s"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: _snprintf_s|_snwprintf_s (
static bool match18(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("_snprintf_s")) || (tok->str()==MatchCompiler::makeConstString("_snwprintf_s"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: char|short|int|long
static bool match19(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("short")) || (tok->str()==MatchCompiler::makeConstString("int")) || (tok->str()==MatchCompiler::makeConstString("long"))))
        return false;
    return true;
}
// pattern: float|double
static bool match20(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("float")) || (tok->str()==MatchCompiler::makeConstString("double"))))
        return false;
    return true;
}
// pattern: bool|short|long|int|char|wchar_t
static bool match21(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("short")) || (tok->str()==MatchCompiler::makeConstString("long")) || (tok->str()==MatchCompiler::makeConstString("int")) || (tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("wchar_t"))))
        return false;
    return true;
}
// pattern: bool|char|short|wchar_t|int
static bool match22(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("short")) || (tok->str()==MatchCompiler::makeConstString("wchar_t")) || (tok->str()==MatchCompiler::makeConstString("int"))))
        return false;
    return true;
}
// pattern: char|short
static bool match23(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("short"))))
        return false;
    return true;
}
// pattern: bool|char|short|int|long
static bool match24(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("short")) || (tok->str()==MatchCompiler::makeConstString("int")) || (tok->str()==MatchCompiler::makeConstString("long"))))
        return false;
    return true;
}
// pattern: bool|char|short|int
static bool match25(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("short")) || (tok->str()==MatchCompiler::makeConstString("int"))))
        return false;
    return true;
}
// pattern: bool|_Bool
static bool match26(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("_Bool"))))
        return false;
    return true;
}
// pattern: bool|char|short|long|int
static bool match27(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("bool")) || (tok->str()==MatchCompiler::makeConstString("char")) || (tok->str()==MatchCompiler::makeConstString("short")) || (tok->str()==MatchCompiler::makeConstString("long")) || (tok->str()==MatchCompiler::makeConstString("int"))))
        return false;
    return true;
}
// pattern: %str% ,|)
static bool match28(const Token* tok) {
    if (!tok || !(tok->tokType()==Token::eString))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: %var%
static bool match29(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    return true;
}
// pattern: %type% ::
static bool match30(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    return true;
}
// pattern: static_cast|reinterpret_cast|const_cast <
static bool match31(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("static_cast")) || (tok->str()==MatchCompiler::makeConstString("reinterpret_cast")) || (tok->str()==MatchCompiler::makeConstString("const_cast"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: > (
static bool match32(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(">")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) ,|)
static bool match33(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString(",")) || (tok->str()==MatchCompiler::makeConstString(")"))))
        return false;
    return true;
}
// pattern: static_cast|reinterpret_cast|const_cast
static bool match34(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("static_cast")) || (tok->str()==MatchCompiler::makeConstString("reinterpret_cast")) || (tok->str()==MatchCompiler::makeConstString("const_cast"))))
        return false;
    return true;
}
// pattern: %var% . size|empty|c_str ( ) [,)]
static bool match35(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("size")) || (tok->str()==MatchCompiler::makeConstString("empty")) || (tok->str()==MatchCompiler::makeConstString("c_str"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: ] . size|empty|c_str ( ) [,)]
static bool match36(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("]")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("size")) || (tok->str()==MatchCompiler::makeConstString("empty")) || (tok->str()==MatchCompiler::makeConstString("c_str"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: %var% . at (
static bool match37(const Token* tok) {
    if (!tok || !(tok->varId() != 0))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(".")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("at")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("(")))
        return false;
    return true;
}
// pattern: ) [,)]
static bool match38(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString(")")))
        return false;
    tok = tok->next();
    if (!tok || tok->str().size()!=1U || !strchr(",)", tok->str()[0]))
        return false;
    return true;
}
// pattern: std :: vector|array <
static bool match39(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("vector")) || (tok->str()==MatchCompiler::makeConstString("array"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: std :: string|wstring
static bool match40(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("string")) || (tok->str()==MatchCompiler::makeConstString("wstring"))))
        return false;
    return true;
}
// pattern: %type% &
static bool match41(const Token* tok) {
    if (!tok || !(tok->isName() && tok->varId()==0U && !tok->isKeyword()))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("&")))
        return false;
    return true;
}
// pattern: std :: vector|array|bitset|deque|list|forward_list|map|multimap|multiset|priority_queue|queue|set|stack|hash_map|hash_multimap|hash_set|unordered_map|unordered_multimap|unordered_set|unordered_multiset <
static bool match42(const Token* tok) {
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("std")))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
        return false;
    tok = tok->next();
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("vector")) || (tok->str()==MatchCompiler::makeConstString("array")) || (tok->str()==MatchCompiler::makeConstString("bitset")) || (tok->str()==MatchCompiler::makeConstString("deque")) || (tok->str()==MatchCompiler::makeConstString("list")) || (tok->str()==MatchCompiler::makeConstString("forward_list")) || (tok->str()==MatchCompiler::makeConstString("map")) || (tok->str()==MatchCompiler::makeConstString("multimap")) || (tok->str()==MatchCompiler::makeConstString("multiset")) || (tok->str()==MatchCompiler::makeConstString("priority_queue")) || (tok->str()==MatchCompiler::makeConstString("queue")) || (tok->str()==MatchCompiler::makeConstString("set")) || (tok->str()==MatchCompiler::makeConstString("stack")) || (tok->str()==MatchCompiler::makeConstString("hash_map")) || (tok->str()==MatchCompiler::makeConstString("hash_multimap")) || (tok->str()==MatchCompiler::makeConstString("hash_set")) || (tok->str()==MatchCompiler::makeConstString("unordered_map")) || (tok->str()==MatchCompiler::makeConstString("unordered_multimap")) || (tok->str()==MatchCompiler::makeConstString("unordered_set")) || (tok->str()==MatchCompiler::makeConstString("unordered_multiset"))))
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("<")))
        return false;
    return true;
}
// pattern: const|struct
static bool match43(const Token* tok) {
    if (!tok || !((tok->str()==MatchCompiler::makeConstString("const")) || (tok->str()==MatchCompiler::makeConstString("struct"))))
        return false;
    return true;
}
// pattern: %any% ::
static bool match44(const Token* tok) {
    if (!tok || false)
        return false;
    tok = tok->next();
    if (!tok || !(tok->str()==MatchCompiler::makeConstString("::")))
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
#include "checkio.h"

#include "library.h"
#include "mathlib.h"
#include "settings.h"
#include "symboldatabase.h"
#include "token.h"
#include "tokenize.h"
#include "utils.h"
#include "valueflow.h"

#include <cctype>
#include <cstdlib>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

//---------------------------------------------------------------------------

// Register CheckIO..
namespace {
    CheckIO instance;
}

// CVE ID used:
static const CWE CWE119(119U);  // Improper Restriction of Operations within the Bounds of a Memory Buffer
static const CWE CWE398(398U);  // Indicator of Poor Code Quality
static const CWE CWE664(664U);  // Improper Control of a Resource Through its Lifetime
static const CWE CWE685(685U);  // Function Call With Incorrect Number of Arguments
static const CWE CWE686(686U);  // Function Call With Incorrect Argument Type
static const CWE CWE687(687U);  // Function Call With Incorrectly Specified Argument Value
static const CWE CWE704(704U);  // Incorrect Type Conversion or Cast
static const CWE CWE910(910U);  // Use of Expired File Descriptor

//---------------------------------------------------------------------------
//    std::cout << std::cout;
//---------------------------------------------------------------------------
void CheckIO::checkCoutCerrMisusage()
{
    if (mTokenizer->isC())
        return;

    const SymbolDatabase * const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart; tok && tok != scope->bodyEnd; tok = tok->next()) {
            if (match1(tok) && tok->next()->astParent() && tok->next()->astParent()->astOperand1() == tok->next()) {
                const Token* tok2 = tok->next();
                while (tok2->astParent() && tok2->astParent()->str() == MatchCompiler::makeConstString("<<")) {
                    tok2 = tok2->astParent();
                    if (tok2->astOperand2() && match2(tok2->astOperand2()->previous()))
                        coutCerrMisusageError(tok, tok2->astOperand2()->strAt(1));
                }
            }
        }
    }
}

void CheckIO::coutCerrMisusageError(const Token* tok, const std::string& streamName)
{
    reportError(tok, Severity::error, "coutCerrMisusage", "Invalid usage of output stream: '<< std::" + streamName + "'.", CWE398, false);
}

//---------------------------------------------------------------------------
// fflush(stdin) <- fflush only applies to output streams in ANSI C
// fread(); fwrite(); <- consecutive read/write statements require repositioning in between
// fopen("","r"); fwrite(); <- write to read-only file (or vice versa)
// fclose(); fread(); <- Use closed file
//---------------------------------------------------------------------------
enum OpenMode { CLOSED, READ_MODE, WRITE_MODE, RW_MODE, UNKNOWN_OM };
static OpenMode getMode(const std::string& str)
{
    if (str.find('+', 1) != std::string::npos)
        return RW_MODE;
    else if (str.find('w') != std::string::npos || str.find('a') != std::string::npos)
        return WRITE_MODE;
    else if (str.find('r') != std::string::npos)
        return READ_MODE;
    return UNKNOWN_OM;
}

struct Filepointer {
    OpenMode mode;
    unsigned int mode_indent;
    enum Operation {NONE, UNIMPORTANT, READ, WRITE, POSITIONING, OPEN, CLOSE, UNKNOWN_OP} lastOperation;
    unsigned int op_indent;
    enum AppendMode { UNKNOWN_AM, APPEND, APPEND_EX };
    AppendMode append_mode;
    explicit Filepointer(OpenMode mode_ = UNKNOWN_OM)
        : mode(mode_), mode_indent(0), lastOperation(NONE), op_indent(0), append_mode(UNKNOWN_AM) {
    }
};

namespace {
    const std::set<std::string> whitelist = { "clearerr", "feof", "ferror", "fgetpos", "ftell", "setbuf", "setvbuf", "ungetc", "ungetwc" };
}

void CheckIO::checkFileUsage()
{
    const bool windows = mSettings->isWindowsPlatform();
    const bool printPortability = mSettings->isEnabled(Settings::PORTABILITY);
    const bool printWarnings = mSettings->isEnabled(Settings::WARNING);

    std::map<unsigned int, Filepointer> filepointers;

    const SymbolDatabase* symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Variable* var : symbolDatabase->variableList()) {
        if (!var || !var->declarationId() || var->isArray() || !match3(var->typeStartToken()))
            continue;

        if (var->isLocal()) {
            if (var->nameToken()->strAt(1) == MatchCompiler::makeConstString("(")) // initialize by calling "ctor"
                filepointers.insert(std::make_pair(var->declarationId(), Filepointer(UNKNOWN_OM)));
            else
                filepointers.insert(std::make_pair(var->declarationId(), Filepointer(CLOSED)));
        } else {
            filepointers.insert(std::make_pair(var->declarationId(), Filepointer(UNKNOWN_OM)));
            // TODO: If all fopen calls we find open the file in the same type, we can set Filepointer::mode
        }
    }

    for (const Scope * scope : symbolDatabase->functionScopes) {
        unsigned int indent = 0;
        for (const Token *tok = scope->bodyStart; tok != scope->bodyEnd; tok = tok->next()) {
            if (tok->str() == MatchCompiler::makeConstString("{"))
                indent++;
            else if (tok->str() == MatchCompiler::makeConstString("}")) {
                indent--;
                for (std::map<unsigned int, Filepointer>::iterator i = filepointers.begin(); i != filepointers.end(); ++i) {
                    if (indent < i->second.mode_indent) {
                        i->second.mode_indent = 0;
                        i->second.mode = UNKNOWN_OM;
                    }
                    if (indent < i->second.op_indent) {
                        i->second.op_indent = 0;
                        i->second.lastOperation = Filepointer::UNKNOWN_OP;
                    }
                }
            } else if (tok->str() == MatchCompiler::makeConstString("return") || tok->str() == MatchCompiler::makeConstString("continue") || tok->str() == MatchCompiler::makeConstString("break") || mSettings->library.isnoreturn(tok)) { // Reset upon return, continue or break
                for (std::map<unsigned int, Filepointer>::iterator i = filepointers.begin(); i != filepointers.end(); ++i) {
                    i->second.mode_indent = 0;
                    i->second.mode = UNKNOWN_OM;
                    i->second.op_indent = 0;
                    i->second.lastOperation = Filepointer::UNKNOWN_OP;
                }
            } else if (match4(tok) &&
                       (tok->strAt(2) != MatchCompiler::makeConstString("fopen") && tok->strAt(2) != MatchCompiler::makeConstString("freopen") && tok->strAt(2) != MatchCompiler::makeConstString("tmpfile") &&
                        (windows ? (tok->str() != MatchCompiler::makeConstString("_wfopen") && tok->str() != MatchCompiler::makeConstString("_wfreopen")) : true))) {
                std::map<unsigned int, Filepointer>::iterator i = filepointers.find(tok->varId());
                if (i != filepointers.end()) {
                    i->second.mode = UNKNOWN_OM;
                    i->second.lastOperation = Filepointer::UNKNOWN_OP;
                }
            } else if (match5(tok) && tok->previous() && (!tok->previous()->isName() || match6(tok->previous()))) {
                std::string mode;
                const Token* fileTok = nullptr;
                Filepointer::Operation operation = Filepointer::NONE;

                if ((tok->str() == MatchCompiler::makeConstString("fopen") || tok->str() == MatchCompiler::makeConstString("freopen") || tok->str() == MatchCompiler::makeConstString("tmpfile") ||
                     (windows && (tok->str() == MatchCompiler::makeConstString("_wfopen") || tok->str() == MatchCompiler::makeConstString("_wfreopen")))) &&
                    tok->strAt(-1) == MatchCompiler::makeConstString("=")) {
                    if (tok->str() != MatchCompiler::makeConstString("tmpfile")) {
                        const Token* modeTok = tok->tokAt(2)->nextArgument();
                        if (modeTok && modeTok->tokType() == Token::eString)
                            mode = modeTok->strValue();
                    } else
                        mode = "wb+";
                    fileTok = tok->tokAt(-2);
                    operation = Filepointer::OPEN;
                } else if (windows && match7(tok)) {
                    const Token* modeTok = tok->tokAt(2)->nextArgument()->nextArgument();
                    if (modeTok && modeTok->tokType() == Token::eString)
                        mode = modeTok->strValue();
                    fileTok = tok->tokAt(3);
                    operation = Filepointer::OPEN;
                } else if ((tok->str() == MatchCompiler::makeConstString("rewind") || tok->str() == MatchCompiler::makeConstString("fseek") || tok->str() == MatchCompiler::makeConstString("fsetpos") || tok->str() == MatchCompiler::makeConstString("fflush")) ||
                           (windows && tok->str() == MatchCompiler::makeConstString("_fseeki64"))) {
                    fileTok = tok->tokAt(2);
                    if (printPortability && fileTok && tok->str() == MatchCompiler::makeConstString("fflush")) {
                        if (fileTok->str() == MatchCompiler::makeConstString("stdin"))
                            fflushOnInputStreamError(tok, fileTok->str());
                        else {
                            const Filepointer& f = filepointers[fileTok->varId()];
                            if (f.mode == READ_MODE)
                                fflushOnInputStreamError(tok, fileTok->str());
                        }
                    }
                    operation = Filepointer::POSITIONING;
                } else if (tok->str() == MatchCompiler::makeConstString("fgetc") || tok->str() == MatchCompiler::makeConstString("fgetwc") ||
                           tok->str() == MatchCompiler::makeConstString("fgets") || tok->str() == MatchCompiler::makeConstString("fgetws") || tok->str() == MatchCompiler::makeConstString("fread") ||
                           tok->str() == MatchCompiler::makeConstString("fscanf") || tok->str() == MatchCompiler::makeConstString("fwscanf") || tok->str() == MatchCompiler::makeConstString("getc") ||
                           (windows && (tok->str() == MatchCompiler::makeConstString("fscanf_s") || tok->str() == MatchCompiler::makeConstString("fwscanf_s")))) {
                    if (tok->str().find("scanf") != std::string::npos)
                        fileTok = tok->tokAt(2);
                    else
                        fileTok = tok->linkAt(1)->previous();
                    operation = Filepointer::READ;
                } else if (tok->str() == MatchCompiler::makeConstString("fputc") || tok->str() == MatchCompiler::makeConstString("fputwc") ||
                           tok->str() == MatchCompiler::makeConstString("fputs") || tok->str() == MatchCompiler::makeConstString("fputws") || tok->str() == MatchCompiler::makeConstString("fwrite") ||
                           tok->str() == MatchCompiler::makeConstString("fprintf") || tok->str() == MatchCompiler::makeConstString("fwprintf") || tok->str() == MatchCompiler::makeConstString("putcc") ||
                           (windows && (tok->str() == MatchCompiler::makeConstString("fprintf_s") || tok->str() == MatchCompiler::makeConstString("fwprintf_s")))) {
                    if (tok->str().find("printf") != std::string::npos)
                        fileTok = tok->tokAt(2);
                    else
                        fileTok = tok->linkAt(1)->previous();
                    operation = Filepointer::WRITE;
                } else if (tok->str() == MatchCompiler::makeConstString("fclose")) {
                    fileTok = tok->tokAt(2);
                    operation = Filepointer::CLOSE;
                } else if (whitelist.find(tok->str()) != whitelist.end()) {
                    fileTok = tok->tokAt(2);
                    if ((tok->str() == MatchCompiler::makeConstString("ungetc") || tok->str() == MatchCompiler::makeConstString("ungetwc")) && fileTok)
                        fileTok = fileTok->nextArgument();
                    operation = Filepointer::UNIMPORTANT;
                } else if (!match8(tok) && !mSettings->library.isFunctionConst(tok->str(), true)) {
                    const Token* const end2 = tok->linkAt(1);
                    if (scope->functionOf && scope->functionOf->isClassOrStruct() && !scope->function->isStatic() && ((tok->strAt(-1) != MatchCompiler::makeConstString("::") && tok->strAt(-1) != MatchCompiler::makeConstString(".")) || tok->strAt(-2) == MatchCompiler::makeConstString("this"))) {
                        if (!tok->function() || (tok->function()->nestedIn && tok->function()->nestedIn->isClassOrStruct())) {
                            for (std::map<unsigned int, Filepointer>::iterator i = filepointers.begin(); i != filepointers.end(); ++i) {
                                const Variable* var = symbolDatabase->getVariableFromVarId(i->first);
                                if (!var || !(var->isLocal() || var->isGlobal() || var->isStatic())) {
                                    i->second.mode = UNKNOWN_OM;
                                    i->second.mode_indent = 0;
                                    i->second.op_indent = indent;
                                    i->second.lastOperation = Filepointer::UNKNOWN_OP;
                                }
                            }
                            continue;
                        }
                    }
                    for (const Token* tok2 = tok->tokAt(2); tok2 != end2; tok2 = tok2->next()) {
                        if (tok2->varId() && filepointers.find(tok2->varId()) != filepointers.end()) {
                            fileTok = tok2;
                            operation = Filepointer::UNKNOWN_OP; // Assume that repositioning was last operation and that the file is opened now
                            break;
                        }
                    }
                }

                while (match9(fileTok))
                    fileTok = fileTok->tokAt(2);

                if (!fileTok || !fileTok->varId() || fileTok->strAt(1) == MatchCompiler::makeConstString("["))
                    continue;

                if (filepointers.find(fileTok->varId()) == filepointers.end()) { // function call indicates: Its a File
                    filepointers.insert(std::make_pair(fileTok->varId(), Filepointer(UNKNOWN_OM)));
                }
                Filepointer& f = filepointers[fileTok->varId()];

                switch (operation) {
                case Filepointer::OPEN:
                    f.mode = getMode(mode);
                    if (mode.find('a') != std::string::npos) {
                        if (f.mode == RW_MODE)
                            f.append_mode = Filepointer::APPEND_EX;
                        else
                            f.append_mode = Filepointer::APPEND;
                    } else
                        f.append_mode = Filepointer::UNKNOWN_AM;
                    f.mode_indent = indent;
                    break;
                case Filepointer::POSITIONING:
                    if (f.mode == CLOSED)
                        useClosedFileError(tok);
                    else if (f.append_mode == Filepointer::APPEND && tok->str() != MatchCompiler::makeConstString("fflush") && printWarnings)
                        seekOnAppendedFileError(tok);
                    break;
                case Filepointer::READ:
                    if (f.mode == CLOSED)
                        useClosedFileError(tok);
                    else if (f.mode == WRITE_MODE)
                        readWriteOnlyFileError(tok);
                    else if (f.lastOperation == Filepointer::WRITE)
                        ioWithoutPositioningError(tok);
                    break;
                case Filepointer::WRITE:
                    if (f.mode == CLOSED)
                        useClosedFileError(tok);
                    else if (f.mode == READ_MODE)
                        writeReadOnlyFileError(tok);
                    else if (f.lastOperation == Filepointer::READ)
                        ioWithoutPositioningError(tok);
                    break;
                case Filepointer::CLOSE:
                    if (f.mode == CLOSED)
                        useClosedFileError(tok);
                    else
                        f.mode = CLOSED;
                    f.mode_indent = indent;
                    break;
                case Filepointer::UNIMPORTANT:
                    if (f.mode == CLOSED)
                        useClosedFileError(tok);
                    break;
                case Filepointer::UNKNOWN_OP:
                    f.mode = UNKNOWN_OM;
                    f.mode_indent = 0;
                    break;
                default:
                    break;
                }
                if (operation != Filepointer::NONE && operation != Filepointer::UNIMPORTANT) {
                    f.op_indent = indent;
                    f.lastOperation = operation;
                }
            }
        }
        for (std::map<unsigned int, Filepointer>::iterator i = filepointers.begin(); i != filepointers.end(); ++i) {
            i->second.op_indent = 0;
            i->second.mode = UNKNOWN_OM;
            i->second.lastOperation = Filepointer::UNKNOWN_OP;
        }
    }
}

void CheckIO::fflushOnInputStreamError(const Token *tok, const std::string &varname)
{
    reportError(tok, Severity::portability,
                "fflushOnInputStream", "fflush() called on input stream '" + varname + "' may result in undefined behaviour on non-linux systems.", CWE398, false);
}

void CheckIO::ioWithoutPositioningError(const Token *tok)
{
    reportError(tok, Severity::error,
                "IOWithoutPositioning", "Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour.", CWE664, false);
}

void CheckIO::readWriteOnlyFileError(const Token *tok)
{
    reportError(tok, Severity::error,
                "readWriteOnlyFile", "Read operation on a file that was opened only for writing.", CWE664, false);
}

void CheckIO::writeReadOnlyFileError(const Token *tok)
{
    reportError(tok, Severity::error,
                "writeReadOnlyFile", "Write operation on a file that was opened only for reading.", CWE664, false);
}

void CheckIO::useClosedFileError(const Token *tok)
{
    reportError(tok, Severity::error,
                "useClosedFile", "Used file that is not opened.", CWE910, false);
}

void CheckIO::seekOnAppendedFileError(const Token *tok)
{
    reportError(tok, Severity::warning,
                "seekOnAppendedFile", "Repositioning operation performed on a file opened in append mode has no effect.", CWE398, false);
}


//---------------------------------------------------------------------------
// scanf without field width limits can crash with huge input data
//---------------------------------------------------------------------------
void CheckIO::invalidScanf()
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;

    const SymbolDatabase * const symbolDatabase = mTokenizer->getSymbolDatabase();
    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            const Token *formatToken = nullptr;
            if (match10(tok))
                formatToken = tok->tokAt(2);
            else if (match11(tok)) {
                const Token* nextArg = tok->tokAt(2)->nextArgument();
                if (nextArg && nextArg->tokType() == Token::eString)
                    formatToken = nextArg;
                else
                    continue;
            } else
                continue;

            bool format = false;

            // scan the string backwards, so we do not need to keep states
            const std::string &formatstr(formatToken->str());
            for (std::size_t i = 1; i < formatstr.length(); i++) {
                if (formatstr[i] == '%')
                    format = !format;

                else if (!format)
                    continue;

                else if (std::isdigit(formatstr[i]) || formatstr[i] == '*') {
                    format = false;
                }

                else if (std::isalpha((unsigned char)formatstr[i]) || formatstr[i] == '[') {
                    if (formatstr[i] == 's' || formatstr[i] == '[' || formatstr[i] == 'S' || (formatstr[i] == 'l' && formatstr[i+1] == 's'))  // #3490 - field width limits are only necessary for string input
                        invalidScanfError(tok);
                    format = false;
                }
            }
        }
    }
}

void CheckIO::invalidScanfError(const Token *tok)
{
    const std::string fname = (tok ? tok->str() : std::string("scanf"));
    reportError(tok, Severity::warning,
                "invalidscanf", fname + "() without field width limits can crash with huge input data.\n" +
                fname + "() without field width limits can crash with huge input data. Add a field width "
                "specifier to fix this problem.\n"
                "\n"
                "Sample program that can crash:\n"
                "\n"
                "#include <stdio.h>\n"
                "int main()\n"
                "{\n"
                "    char c[5];\n"
                "    scanf(\"%s\", c);\n"
                "    return 0;\n"
                "}\n"
                "\n"
                "Typing in 5 or more characters may make the program crash. The correct usage "
                "here is 'scanf(\"%4s\", c);', as the maximum field width does not include the "
                "terminating null byte.\n"
                "Source: http://linux.die.net/man/3/scanf\n"
                "Source: http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/libkern/stdio/scanf.c",
                CWE119, false);
}

//---------------------------------------------------------------------------
//    printf("%u", "xyz"); // Wrong argument type
//    printf("%u%s", 1); // Too few arguments
//    printf("", 1); // Too much arguments
//---------------------------------------------------------------------------

static bool findFormat(unsigned int arg, const Token *firstArg,
                       const Token **formatStringTok, const Token **formatArgTok)
{
    const Token* argTok = firstArg;

    for (unsigned int i = 0; i < arg && argTok; ++i)
        argTok = argTok->nextArgument();

    if (match12(argTok)) {
        *formatArgTok = argTok->nextArgument();
        *formatStringTok = argTok;
        return true;
    } else if (match13(argTok) &&
               argTok->variable() &&
               match14(argTok->variable()->typeStartToken()) &&
               (argTok->variable()->isPointer() ||
                (argTok->variable()->dimensions().size() == 1 &&
                 argTok->variable()->dimensionKnown(0) &&
                 argTok->variable()->dimension(0) != 0))) {
        *formatArgTok = argTok->nextArgument();
        if (!argTok->values().empty()) {
            const ValueFlow::Value &value = argTok->values().front();
            if (value.isTokValue() && value.tokvalue && value.tokvalue->tokType() == Token::eString) {
                *formatStringTok = value.tokvalue;
            }
        }
        return true;
    }
    return false;
}

// Utility function returning whether iToTest equals iTypename or iOptionalPrefix+iTypename
static inline bool typesMatch(const std::string& iToTest, const std::string& iTypename, const std::string& iOptionalPrefix = "std::")
{
    return (iToTest == iTypename) || (iToTest == iOptionalPrefix + iTypename);
}

void CheckIO::checkWrongPrintfScanfArguments()
{
    const SymbolDatabase *symbolDatabase = mTokenizer->getSymbolDatabase();
    const bool isWindows = mSettings->isWindowsPlatform();

    for (const Scope * scope : symbolDatabase->functionScopes) {
        for (const Token *tok = scope->bodyStart->next(); tok != scope->bodyEnd; tok = tok->next()) {
            if (!tok->isName()) continue;

            const Token* argListTok = nullptr; // Points to first va_list argument
            const Token* formatStringTok = nullptr; // Points to format string token

            bool scan = false;
            bool scanf_s = false;
            int formatStringArgNo = -1;

            if (tok->strAt(1) == MatchCompiler::makeConstString("(") && mSettings->library.formatstr_function(tok)) {
                formatStringArgNo = mSettings->library.formatstr_argno(tok);
                scan = mSettings->library.formatstr_scan(tok);
                scanf_s = mSettings->library.formatstr_secure(tok);
            }

            if (formatStringArgNo >= 0) {
                // formatstring found in library. Find format string and first argument belonging to format string.
                if (!findFormat(static_cast<unsigned int>(formatStringArgNo), tok->tokAt(2), &formatStringTok, &argListTok))
                    continue;
            } else if (match15(tok)) {
                if (match16(tok->tokAt(2)->nextArgument())) {
                    // Find third parameter and format string
                    if (!findFormat(1, tok->tokAt(2), &formatStringTok, &argListTok))
                        continue;
                } else {
                    // Find fourth parameter and format string
                    if (!findFormat(2, tok->tokAt(2), &formatStringTok, &argListTok))
                        continue;
                }
            } else if (isWindows && match17(tok)) {
                // template <size_t size> int sprintf_s(char (&buffer)[size], const char *format, ...);
                if (findFormat(1, tok->tokAt(2), &formatStringTok, &argListTok)) {
                    if (!formatStringTok)
                        continue;
                }
                // int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...);
                else if (findFormat(2, tok->tokAt(2), &formatStringTok, &argListTok)) {
                    if (!formatStringTok)
                        continue;
                }
            } else if (isWindows && match18(tok)) {
                // template <size_t size> int _snprintf_s(char (&buffer)[size], size_t count, const char *format, ...);
                if (findFormat(2, tok->tokAt(2), &formatStringTok, &argListTok)) {
                    if (!formatStringTok)
                        continue;
                }
                // int _snprintf_s(char *buffer, size_t sizeOfBuffer, size_t count, const char *format, ...);
                else if (findFormat(3, tok->tokAt(2), &formatStringTok, &argListTok)) {
                    if (!formatStringTok)
                        continue;
                }
            } else {
                continue;
            }

            if (!formatStringTok)
                continue;

            checkFormatString(tok, formatStringTok, argListTok, scan, scanf_s);
        }
    }
}

void CheckIO::checkFormatString(const Token * const tok,
                                const Token * const formatStringTok,
                                const Token *       argListTok,
                                const bool scan,
                                const bool scanf_s)
{
    const bool printWarning = mSettings->isEnabled(Settings::WARNING);
    const std::string &formatString = formatStringTok->str();

    // Count format string parameters..
    unsigned int numFormat = 0;
    unsigned int numSecure = 0;
    bool percent = false;
    const Token* argListTok2 = argListTok;
    std::set<unsigned int> parameterPositionsUsed;
    for (std::string::const_iterator i = formatString.begin(); i != formatString.end(); ++i) {
        if (*i == '%') {
            percent = !percent;
        } else if (percent && *i == '[') {
            while (i != formatString.end()) {
                if (*i == ']') {
                    numFormat++;
                    if (argListTok)
                        argListTok = argListTok->nextArgument();
                    percent = false;
                    break;
                }
                ++i;
            }
            if (scanf_s) {
                numSecure++;
                if (argListTok) {
                    argListTok = argListTok->nextArgument();
                }
            }
            if (i == formatString.end())
                break;
        } else if (percent) {
            percent = false;

            bool _continue = false;
            bool skip = false;
            std::string width;
            unsigned int parameterPosition = 0;
            bool hasParameterPosition = false;
            while (i != formatString.end() && *i != '[' && !std::isalpha((unsigned char)*i)) {
                if (*i == '*') {
                    skip = true;
                    if (scan)
                        _continue = true;
                    else {
                        numFormat++;
                        if (argListTok)
                            argListTok = argListTok->nextArgument();
                    }
                } else if (std::isdigit(*i)) {
                    width += *i;
                } else if (*i == '$') {
                    parameterPosition = static_cast<unsigned int>(std::atoi(width.c_str()));
                    hasParameterPosition = true;
                    width.clear();
                }
                ++i;
            }
            if (i != formatString.end() && *i == '[') {
                while (i != formatString.end()) {
                    if (*i == ']') {
                        if (!skip) {
                            numFormat++;
                            if (argListTok)
                                argListTok = argListTok->nextArgument();
                        }
                        break;
                    }
                    ++i;
                }
                if (scanf_s && !skip) {
                    numSecure++;
                    if (argListTok) {
                        argListTok = argListTok->nextArgument();
                    }
                }
                _continue = true;
            }
            if (i == formatString.end())
                break;
            if (_continue)
                continue;

            if (scan || *i != 'm') { // %m is a non-standard extension that requires no parameter on print functions.
                ++numFormat;

                // Handle parameter positions (POSIX extension) - Ticket #4900
                if (hasParameterPosition) {
                    if (parameterPositionsUsed.find(parameterPosition) == parameterPositionsUsed.end())
                        parameterPositionsUsed.insert(parameterPosition);
                    else // Parameter already referenced, hence don't consider it a new format
                        --numFormat;
                }

                // Perform type checks
                ArgumentInfo argInfo(argListTok, mSettings, mTokenizer->isCPP());

                if (argInfo.typeToken && !argInfo.isLibraryType(mSettings)) {
                    if (scan) {
                        std::string specifier;
                        bool done = false;
                        while (!done) {
                            switch (*i) {
                            case 's':
                                specifier += *i;
                                if (argInfo.variableInfo && argInfo.isKnownType() && argInfo.variableInfo->isArray() && (argInfo.variableInfo->dimensions().size() == 1) && argInfo.variableInfo->dimensions()[0].known) {
                                    if (!width.empty()) {
                                        const int numWidth = std::atoi(width.c_str());
                                        if (numWidth != (argInfo.variableInfo->dimension(0) - 1))
                                            invalidScanfFormatWidthError(tok, numFormat, numWidth, argInfo.variableInfo, 's');
                                    }
                                }
                                if (argListTok && argListTok->tokType() != Token::eString &&
                                    argInfo.isKnownType() && argInfo.isArrayOrPointer() &&
                                    (!match14(argInfo.typeToken) ||
                                     argInfo.typeToken->strAt(-1) == MatchCompiler::makeConstString("const"))) {
                                    if (!(argInfo.isArrayOrPointer() && argInfo.element && !argInfo.typeToken->isStandardType()))
                                        invalidScanfArgTypeError_s(tok, numFormat, specifier, &argInfo);
                                }
                                if (scanf_s) {
                                    numSecure++;
                                    if (argListTok) {
                                        argListTok = argListTok->nextArgument();
                                    }
                                }
                                done = true;
                                break;
                            case 'c':
                                if (argInfo.variableInfo && argInfo.isKnownType() && argInfo.variableInfo->isArray() && (argInfo.variableInfo->dimensions().size() == 1) && argInfo.variableInfo->dimensions()[0].known) {
                                    if (!width.empty()) {
                                        const int numWidth = std::atoi(width.c_str());
                                        if (numWidth > argInfo.variableInfo->dimension(0))
                                            invalidScanfFormatWidthError(tok, numFormat, numWidth, argInfo.variableInfo, 'c');
                                    }
                                }
                                if (scanf_s) {
                                    numSecure++;
                                    if (argListTok) {
                                        argListTok = argListTok->nextArgument();
                                    }
                                }
                                done = true;
                                break;
                            case 'x':
                            case 'X':
                            case 'u':
                            case 'o':
                                specifier += *i;
                                if (argInfo.typeToken->tokType() == Token::eString)
                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                else if (argInfo.isKnownType()) {
                                    if (!match19(argInfo.typeToken)) {
                                        if (argInfo.typeToken->isStandardType() || !argInfo.element)
                                            invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                    } else if (!argInfo.typeToken->isUnsigned() ||
                                               !argInfo.isArrayOrPointer() ||
                                               argInfo.typeToken->strAt(-1) == MatchCompiler::makeConstString("const")) {
                                        invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                    } else {
                                        switch (specifier[0]) {
                                        case 'h':
                                            if (specifier[1] == 'h') {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("char"))
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            } else if (argInfo.typeToken->str() != MatchCompiler::makeConstString("short"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        case 'l':
                                            if (specifier[1] == 'l') {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                                else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                         typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                         typesMatch(argInfo.typeToken->originalName(), "uintmax_t"))
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            } else if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || argInfo.typeToken->isLong())
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                     typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                     typesMatch(argInfo.typeToken->originalName(), "uintmax_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        case 'I':
                                            if (specifier.find("I64") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            } else if (specifier.find("I32") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("int") || argInfo.typeToken->isLong())
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            } else if (!typesMatch(argInfo.typeToken->originalName(), "size_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        case 'j':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "uintmax_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        case 'z':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "size_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        case 't':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        case 'L':
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                     typesMatch(argInfo.typeToken->originalName(), "uintmax_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        default:
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("int"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                     typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                     typesMatch(argInfo.typeToken->originalName(), "uintmax_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, true);
                                            break;
                                        }
                                    }
                                }
                                done = true;
                                break;
                            case 'n':
                            case 'd':
                            case 'i':
                                specifier += *i;
                                if (argInfo.typeToken->tokType() == Token::eString)
                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                else if (argInfo.isKnownType()) {
                                    if (!match19(argInfo.typeToken)) {
                                        if (argInfo.typeToken->isStandardType() || !argInfo.element)
                                            invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                    } else if (argInfo.typeToken->isUnsigned() ||
                                               !argInfo.isArrayOrPointer() ||
                                               argInfo.typeToken->strAt(-1) == MatchCompiler::makeConstString("const")) {
                                        invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                    } else {
                                        switch (specifier[0]) {
                                        case 'h':
                                            if (specifier[1] == 'h') {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("char"))
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            } else if (argInfo.typeToken->str() != MatchCompiler::makeConstString("short"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        case 'l':
                                            if (specifier[1] == 'l') {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                                else if (typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                         typesMatch(argInfo.typeToken->originalName(), "intmax_t"))
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            } else if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || argInfo.typeToken->isLong())
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                     typesMatch(argInfo.typeToken->originalName(), "intmax_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        case 'I':
                                            if (specifier.find("I64") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            } else if (specifier.find("I32") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("int") || argInfo.typeToken->isLong())
                                                    invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            } else if (!typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        case 'j':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "intmax_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        case 'z':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "ssize_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        case 't':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        case 'L':
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        default:
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("int"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                     argInfo.typeToken->originalName() == MatchCompiler::makeConstString("intmax_t"))
                                                invalidScanfArgTypeError_int(tok, numFormat, specifier, &argInfo, false);
                                            break;
                                        }
                                    }
                                }
                                done = true;
                                break;
                            case 'e':
                            case 'E':
                            case 'f':
                            case 'g':
                            case 'G':
                            case 'a':
                                specifier += *i;
                                if (argInfo.typeToken->tokType() == Token::eString)
                                    invalidScanfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                else if (argInfo.isKnownType()) {
                                    if (!match20(argInfo.typeToken)) {
                                        if (argInfo.typeToken->isStandardType())
                                            invalidScanfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                    } else if (!argInfo.isArrayOrPointer() ||
                                               argInfo.typeToken->strAt(-1) == MatchCompiler::makeConstString("const")) {
                                        invalidScanfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                    } else {
                                        switch (specifier[0]) {
                                        case 'l':
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("double") || argInfo.typeToken->isLong())
                                                invalidScanfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'L':
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("double") || !argInfo.typeToken->isLong())
                                                invalidScanfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                            break;
                                        default:
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("float"))
                                                invalidScanfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                            break;
                                        }
                                    }
                                }
                                done = true;
                                break;
                            case 'I':
                                if ((i+1 != formatString.end() && *(i+1) == '6' &&
                                     i+2 != formatString.end() && *(i+2) == '4') ||
                                    (i+1 != formatString.end() && *(i+1) == '3' &&
                                     i+2 != formatString.end() && *(i+2) == '2')) {
                                    specifier += *i++;
                                    specifier += *i++;
                                    if ((i+1) != formatString.end() && !isalpha(*(i+1))) {
                                        specifier += *i;
                                        invalidLengthModifierError(tok, numFormat, specifier);
                                        done = true;
                                    } else {
                                        specifier += *i++;
                                    }
                                } else {
                                    if ((i+1) != formatString.end() && !isalpha(*(i+1))) {
                                        specifier += *i;
                                        invalidLengthModifierError(tok, numFormat, specifier);
                                        done = true;
                                    } else {
                                        specifier += *i++;
                                    }
                                }
                                break;
                            case 'h':
                            case 'l':
                                if (i+1 != formatString.end() && *(i+1) == *i)
                                    specifier += *i++;
                            // fallthrough
                            case 'j':
                            case 'q':
                            case 't':
                            case 'z':
                            case 'L':
                                // Expect an alphabetical character after these specifiers
                                if (i != formatString.end() && !isalpha(*(i+1))) {
                                    specifier += *i;
                                    invalidLengthModifierError(tok, numFormat, specifier);
                                    done = true;
                                } else {
                                    specifier += *i++;
                                }
                                break;
                            default:
                                done = true;
                                break;
                            }
                        }
                    } else if (printWarning) {
                        std::string specifier;
                        bool done = false;
                        while (!done) {
                            switch (*i) {
                            case 's':
                                if (argListTok->tokType() != Token::eString &&
                                    argInfo.isKnownType() && !argInfo.isArrayOrPointer()) {
                                    if (!match14(argInfo.typeToken)) {
                                        if (!(!argInfo.isArrayOrPointer() && argInfo.element))
                                            invalidPrintfArgTypeError_s(tok, numFormat, &argInfo);
                                    }
                                }
                                done = true;
                                break;
                            case 'n':
                                if ((argInfo.isKnownType() && (!argInfo.isArrayOrPointer() || argInfo.typeToken->strAt(-1) == MatchCompiler::makeConstString("const"))) || argListTok->tokType() == Token::eString)
                                    invalidPrintfArgTypeError_n(tok, numFormat, &argInfo);
                                done = true;
                                break;
                            case 'c':
                            case 'x':
                            case 'X':
                            case 'o':
                                specifier += *i;
                                if (argInfo.typeToken->tokType() == Token::eString)
                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                else if (argInfo.isArrayOrPointer() && !argInfo.element) {
                                    // use %p on pointers and arrays
                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                } else if (argInfo.isKnownType()) {
                                    if (!match21(argInfo.typeToken)) {
                                        if (!(!argInfo.isArrayOrPointer() && argInfo.element))
                                            invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                    } else {
                                        switch (specifier[0]) {
                                        case 'h':
                                            if (specifier[1] == 'h') {
                                                if (!(argInfo.typeToken->str() == MatchCompiler::makeConstString("char") && argInfo.typeToken->isUnsigned()))
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (!(argInfo.typeToken->str() == MatchCompiler::makeConstString("short") && argInfo.typeToken->isUnsigned()))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'l':
                                            if (specifier[1] == 'l') {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                                else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("uintmax_t"))
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || argInfo.typeToken->isLong())
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                     argInfo.typeToken->originalName() == MatchCompiler::makeConstString("uintmax_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'j':
                                            if (argInfo.typeToken->originalName() != MatchCompiler::makeConstString("uintmax_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'z':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "size_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 't':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'I':
                                            if (specifier.find("I64") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (specifier.find("I32") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("int") || argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (!(typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("WPARAM") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("UINT_PTR") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("LONG_PTR") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("LPARAM") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("LRESULT")))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        default:
                                            if (!match22(argInfo.typeToken))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        }
                                    }
                                }
                                done = true;
                                break;
                            case 'd':
                            case 'i':
                                specifier += *i;
                                if (argInfo.typeToken->tokType() == Token::eString) {
                                    invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                } else if (argInfo.isArrayOrPointer() && !argInfo.element) {
                                    // use %p on pointers and arrays
                                    invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                } else if (argInfo.isKnownType()) {
                                    if (argInfo.typeToken->isUnsigned() && !match23(argInfo.typeToken)) {
                                        if (!(!argInfo.isArrayOrPointer() && argInfo.element))
                                            invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                    } else if (!match24(argInfo.typeToken)) {
                                        if (!(!argInfo.isArrayOrPointer() && argInfo.element))
                                            invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                    } else {
                                        switch (specifier[0]) {
                                        case 'h':
                                            if (specifier[1] == 'h') {
                                                if (!(argInfo.typeToken->str() == MatchCompiler::makeConstString("char") && !argInfo.typeToken->isUnsigned()))
                                                    invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            } else if (!(argInfo.typeToken->str() == MatchCompiler::makeConstString("short") && !argInfo.typeToken->isUnsigned()))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'l':
                                            if (specifier[1] == 'l') {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                                else if (typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("intmax_t"))
                                                    invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            } else if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || argInfo.typeToken->isLong())
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                     argInfo.typeToken->originalName() == MatchCompiler::makeConstString("intmax_t"))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'j':
                                            if (argInfo.typeToken->originalName() != MatchCompiler::makeConstString("intmax_t"))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 't':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t"))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'I':
                                            if (specifier.find("I64") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            } else if (specifier.find("I32") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("int") || argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            } else if (!typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t"))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'z':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "ssize_t"))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'L':
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        default:
                                            if (!match25(argInfo.typeToken))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t") ||
                                                     argInfo.typeToken->originalName() == MatchCompiler::makeConstString("intmax_t"))
                                                invalidPrintfArgTypeError_sint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        }
                                    }
                                }
                                done = true;
                                break;
                            case 'u':
                                specifier += *i;
                                if (argInfo.typeToken->tokType() == Token::eString) {
                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                } else if (argInfo.isArrayOrPointer() && !argInfo.element) {
                                    // use %p on pointers and arrays
                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                } else if (argInfo.isKnownType()) {
                                    if (!argInfo.typeToken->isUnsigned() && !match26(argInfo.typeToken)) {
                                        if (!(!argInfo.isArrayOrPointer() && argInfo.element))
                                            invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                    } else if (!match27(argInfo.typeToken)) {
                                        if (!(!argInfo.isArrayOrPointer() && argInfo.element))
                                            invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                    } else {
                                        switch (specifier[0]) {
                                        case 'h':
                                            if (specifier[1] == 'h') {
                                                if (!(argInfo.typeToken->str() == MatchCompiler::makeConstString("char") && argInfo.typeToken->isUnsigned()))
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (!(argInfo.typeToken->str() == MatchCompiler::makeConstString("short") && argInfo.typeToken->isUnsigned()))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'l':
                                            if (specifier[1] == 'l') {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                                else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                         argInfo.typeToken->originalName() == MatchCompiler::makeConstString("uintmax_t"))
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || argInfo.typeToken->isLong())
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                     argInfo.typeToken->originalName() == MatchCompiler::makeConstString("uintmax_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'j':
                                            if (argInfo.typeToken->originalName() != MatchCompiler::makeConstString("uintmax_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'z':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "size_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 't':
                                            if (!typesMatch(argInfo.typeToken->originalName(), "ptrdiff_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'I':
                                            if (specifier.find("I64") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (specifier.find("I32") != std::string::npos) {
                                                if (argInfo.typeToken->str() != MatchCompiler::makeConstString("int") || argInfo.typeToken->isLong())
                                                    invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            } else if (!typesMatch(argInfo.typeToken->originalName(), "size_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        case 'L':
                                            if (argInfo.typeToken->str() != MatchCompiler::makeConstString("long") || !argInfo.typeToken->isLong())
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        default:
                                            if (!match25(argInfo.typeToken))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            else if (typesMatch(argInfo.typeToken->originalName(), "size_t") ||
                                                     argInfo.typeToken->originalName() == MatchCompiler::makeConstString("intmax_t"))
                                                invalidPrintfArgTypeError_uint(tok, numFormat, specifier, &argInfo);
                                            break;
                                        }
                                    }
                                }
                                done = true;
                                break;
                            case 'p':
                                if (argInfo.typeToken->tokType() == Token::eString)
                                    ;// string literals are passed as pointers to literal start, okay
                                else if (argInfo.isKnownType() && !argInfo.isArrayOrPointer())
                                    invalidPrintfArgTypeError_p(tok, numFormat, &argInfo);
                                done = true;
                                break;
                            case 'e':
                            case 'E':
                            case 'f':
                            case 'g':
                            case 'G':
                                specifier += *i;
                                if (argInfo.typeToken->tokType() == Token::eString)
                                    invalidPrintfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                else if (argInfo.isArrayOrPointer() && !argInfo.element) {
                                    // use %p on pointers and arrays
                                    invalidPrintfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                } else if (argInfo.isKnownType()) {
                                    if (!match20(argInfo.typeToken)) {
                                        if (!(!argInfo.isArrayOrPointer() && argInfo.element))
                                            invalidPrintfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                    } else if ((specifier[0] == 'L' && (!argInfo.typeToken->isLong() || argInfo.typeToken->str() != MatchCompiler::makeConstString("double"))) ||
                                               (specifier[0] != 'L' && argInfo.typeToken->isLong()))
                                        invalidPrintfArgTypeError_float(tok, numFormat, specifier, &argInfo);
                                }
                                done = true;
                                break;
                            case 'h': // Can be 'hh' (signed char or unsigned char) or 'h' (short int or unsigned short int)
                            case 'l': { // Can be 'll' (long long int or unsigned long long int) or 'l' (long int or unsigned long int)
                                // If the next character is the same (which makes 'hh' or 'll') then expect another alphabetical character
                                const bool isSecondCharAvailable = ((i + 1) != formatString.end());
                                if (i != formatString.end() && isSecondCharAvailable && *(i + 1) == *i) {
                                    if (!isalpha(*(i + 2))) {
                                        std::string modifier;
                                        modifier += *i;
                                        modifier += *(i + 1);
                                        invalidLengthModifierError(tok, numFormat, modifier);
                                        done = true;
                                    } else {
                                        specifier = *i++;
                                        specifier += *i++;
                                    }
                                } else {
                                    if (i != formatString.end()) {
                                        if ((i + 1) != formatString.end() && !isalpha(*(i + 1))) {
                                            std::string modifier;
                                            modifier += *i;
                                            invalidLengthModifierError(tok, numFormat, modifier);
                                            done = true;
                                        } else {
                                            specifier = *i++;
                                        }
                                    } else {
                                        done = true;
                                    }
                                }
                            }
                            break;
                            case 'I': // Microsoft extension: I for size_t and ptrdiff_t, I32 for __int32, and I64 for __int64
                                if ((*(i+1) == '3' && *(i+2) == '2') ||
                                    (*(i+1) == '6' && *(i+2) == '4')) {
                                    specifier += *i++;
                                    specifier += *i++;
                                }
                            // fallthrough
                            case 'j': // intmax_t or uintmax_t
                            case 'z': // size_t
                            case 't': // ptrdiff_t
                            case 'L': // long double
                                // Expect an alphabetical character after these specifiers
                                if (i != formatString.end() && !isalpha(*(i+1))) {
                                    specifier += *i;
                                    invalidLengthModifierError(tok, numFormat, specifier);
                                    done = true;
                                } else {
                                    specifier += *i++;
                                }
                                break;
                            default:
                                done = true;
                                break;
                            }
                        }
                    }
                }

                if (argListTok)
                    argListTok = argListTok->nextArgument(); // Find next argument
            }
        }
    }

    // Count printf/scanf parameters..
    unsigned int numFunction = 0;
    while (argListTok2) {
        numFunction++;
        argListTok2 = argListTok2->nextArgument(); // Find next argument
    }

    if (printWarning) {
        // Check that all parameter positions reference an actual parameter
        for (unsigned int i : parameterPositionsUsed) {
            if ((i == 0) || (i > numFormat))
                wrongPrintfScanfPosixParameterPositionError(tok, tok->str(), i, numFormat);
        }
    }

    // Mismatching number of parameters => warning
    if ((numFormat + numSecure) != numFunction)
        wrongPrintfScanfArgumentsError(tok, tok->originalName().empty() ? tok->str() : tok->originalName(), numFormat + numSecure, numFunction);
}

// We currently only support string literals, variables, and functions.
/// @todo add non-string literals, and generic expressions

CheckIO::ArgumentInfo::ArgumentInfo(const Token * arg, const Settings *settings, bool _isCPP)
    : variableInfo(nullptr)
    , typeToken(nullptr)
    , functionInfo(nullptr)
    , tempToken(nullptr)
    , element(false)
    , _template(false)
    , address(false)
    , isCPP(_isCPP)
{
    if (!arg)
        return;

    // Use AST type info
    // TODO: This is a bailout so that old code is used in simple cases. Remove the old code and always use the AST type.
    if (!match28(arg) && !(match29(arg) && arg->variable() && arg->variable()->isArray())) {
        const Token *top = arg;
        while (top->astParent() && top->astParent()->str() != MatchCompiler::makeConstString(",") && top->astParent() != arg->previous())
            top = top->astParent();
        const ValueType *valuetype = top->argumentType();
        if (valuetype && valuetype->type >= ValueType::Type::BOOL) {
            typeToken = tempToken = new Token();
            if (valuetype->pointer && valuetype->constness & 1) {
                tempToken->str("const");
                tempToken->insertToken("a");
                tempToken = tempToken->next();
            }
            if (valuetype->type == ValueType::BOOL)
                tempToken->str("bool");
            else if (valuetype->type == ValueType::CHAR)
                tempToken->str("char");
            else if (valuetype->type == ValueType::SHORT)
                tempToken->str("short");
            else if (valuetype->type == ValueType::INT)
                tempToken->str("int");
            else if (valuetype->type == ValueType::LONG)
                tempToken->str("long");
            else if (valuetype->type == ValueType::LONGLONG) {
                tempToken->str("long");
                tempToken->isLong(true);
            } else if (valuetype->type == ValueType::FLOAT)
                tempToken->str("float");
            else if (valuetype->type == ValueType::DOUBLE)
                tempToken->str("double");
            else if (valuetype->type == ValueType::LONGDOUBLE) {
                tempToken->str("double");
                tempToken->isLong(true);
            }
            if (valuetype->isIntegral()) {
                if (valuetype->sign == ValueType::Sign::UNSIGNED)
                    tempToken->isUnsigned(true);
                else if (valuetype->sign == ValueType::Sign::SIGNED)
                    tempToken->isSigned(true);
            }
            if (!valuetype->originalTypeName.empty())
                tempToken->originalName(valuetype->originalTypeName);
            for (unsigned int p = 0; p < valuetype->pointer; p++)
                tempToken->insertToken("*");
            tempToken = const_cast<Token*>(typeToken);
            return;
        }
    }


    if (arg->tokType() == Token::eString) {
        typeToken = arg;
        return;
    } else if (arg->str() == MatchCompiler::makeConstString("&") || arg->tokType() == Token::eVariable ||
               arg->tokType() == Token::eFunction || match30(arg) ||
               (match31(arg) &&
                match32(arg->linkAt(1)) &&
                match33(arg->linkAt(1)->linkAt(1)))) {
        if (match34(arg)) {
            typeToken = arg->tokAt(2);
            while (typeToken->str() == MatchCompiler::makeConstString("const") || typeToken->str() == MatchCompiler::makeConstString("extern"))
                typeToken = typeToken->next();
            return;
        }
        if (arg->str() == MatchCompiler::makeConstString("&")) {
            address = true;
            arg = arg->next();
        }
        while (match30(arg))
            arg = arg->tokAt(2);
        if (!arg || !(arg->tokType() == Token::eVariable || arg->tokType() == Token::eFunction))
            return;
        const Token *varTok = nullptr;
        const Token *tok1 = arg->next();
        for (; tok1; tok1 = tok1->next()) {
            if (tok1->str() == MatchCompiler::makeConstString(",") || tok1->str() == MatchCompiler::makeConstString(")")) {
                if (tok1->previous()->str() == MatchCompiler::makeConstString("]")) {
                    varTok = tok1->linkAt(-1)->previous();
                    if (varTok->str() == MatchCompiler::makeConstString(")") && varTok->link()->previous()->tokType() == Token::eFunction) {
                        const Function * function = varTok->link()->previous()->function();
                        if (function && function->retType && function->retType->isEnumType()) {
                            if (function->retType->classScope->enumType)
                                typeToken = function->retType->classScope->enumType;
                            else {
                                tempToken = new Token();
                                tempToken->fileIndex(tok1->fileIndex());
                                tempToken->linenr(tok1->linenr());
                                tempToken->str("int");
                                typeToken = tempToken;
                            }
                        } else if (function && function->retDef) {
                            typeToken = function->retDef;
                            while (typeToken->str() == MatchCompiler::makeConstString("const") || typeToken->str() == MatchCompiler::makeConstString("extern"))
                                typeToken = typeToken->next();
                            functionInfo = function;
                            element = true;
                        }
                        return;
                    }
                } else if (tok1->previous()->str() == MatchCompiler::makeConstString(")") && tok1->linkAt(-1)->previous()->tokType() == Token::eFunction) {
                    const Function * function = tok1->linkAt(-1)->previous()->function();
                    if (function && function->retType && function->retType->isEnumType()) {
                        if (function->retType->classScope->enumType)
                            typeToken = function->retType->classScope->enumType;
                        else {
                            tempToken = new Token();
                            tempToken->fileIndex(tok1->fileIndex());
                            tempToken->linenr(tok1->linenr());
                            tempToken->str("int");
                            typeToken = tempToken;
                        }
                    } else if (function && function->retDef) {
                        typeToken = function->retDef;
                        while (typeToken->str() == MatchCompiler::makeConstString("const") || typeToken->str() == MatchCompiler::makeConstString("extern"))
                            typeToken = typeToken->next();
                        functionInfo = function;
                        element = false;
                    }
                    return;
                } else
                    varTok = tok1->previous();
                break;
            } else if (tok1->str() == MatchCompiler::makeConstString("(") || tok1->str() == MatchCompiler::makeConstString("{") || tok1->str() == MatchCompiler::makeConstString("["))
                tok1 = tok1->link();
            else if (tok1->link() && tok1->str() == MatchCompiler::makeConstString("<"))
                tok1 = tok1->link();

            // check for some common well known functions
            else if (isCPP && ((match35(tok1->previous()) && isStdContainer(tok1->previous())) ||
                               (match36(tok1->previous()) && isStdContainer(tok1->previous()->link()->previous())))) {
                tempToken = new Token();
                tempToken->fileIndex(tok1->fileIndex());
                tempToken->linenr(tok1->linenr());
                if (tok1->next()->str() == MatchCompiler::makeConstString("size")) {
                    // size_t is platform dependent
                    if (settings->sizeof_size_t == 8) {
                        tempToken->str("long");
                        if (settings->sizeof_long != 8)
                            tempToken->isLong(true);
                    } else if (settings->sizeof_size_t == 4) {
                        if (settings->sizeof_long == 4) {
                            tempToken->str("long");
                        } else {
                            tempToken->str("int");
                        }
                    }

                    tempToken->originalName("size_t");
                    tempToken->isUnsigned(true);
                } else if (tok1->next()->str() == MatchCompiler::makeConstString("empty")) {
                    tempToken->str("bool");
                } else if (tok1->next()->str() == MatchCompiler::makeConstString("c_str")) {
                    tempToken->str("const");
                    tempToken->insertToken("*");
                    if (typeToken->strAt(2) == MatchCompiler::makeConstString("string"))
                        tempToken->insertToken("char");
                    else
                        tempToken->insertToken("wchar_t");
                }
                typeToken = tempToken;
                return;
            }

            // check for std::vector::at() and std::string::at()
            else if (match37(tok1->previous()) &&
                     match38(tok1->linkAt(2))) {
                varTok = tok1->previous();
                variableInfo = varTok->variable();

                if (!variableInfo || !isStdVectorOrString()) {
                    variableInfo = nullptr;
                    typeToken = nullptr;
                }

                return;
            } else if (!(tok1->str() == MatchCompiler::makeConstString(".") || tok1->tokType() == Token::eVariable || tok1->tokType() == Token::eFunction))
                return;
        }

        if (varTok) {
            variableInfo = varTok->variable();
            element = tok1->previous()->str() == MatchCompiler::makeConstString("]");

            // look for std::vector operator [] and use template type as return type
            if (variableInfo) {
                if (element && isStdVectorOrString()) { // isStdVectorOrString sets type token if true
                    element = false;    // not really an array element
                } else if (variableInfo->isEnumType()) {
                    if (variableInfo->type() && variableInfo->type()->classScope && variableInfo->type()->classScope->enumType)
                        typeToken = variableInfo->type()->classScope->enumType;
                    else {
                        tempToken = new Token();
                        tempToken->fileIndex(tok1->fileIndex());
                        tempToken->linenr(tok1->linenr());
                        tempToken->str("int");
                        typeToken = tempToken;
                    }
                } else
                    typeToken = variableInfo->typeStartToken();
            }

            return;
        }
    }
}

CheckIO::ArgumentInfo::~ArgumentInfo()
{
    if (tempToken) {
        while (tempToken->next())
            tempToken->deleteNext();

        delete tempToken;
    }
}

namespace {
    const std::set<std::string> stl_vector = { "array", "vector" };
    const std::set<std::string> stl_string = { "string", "u16string", "u32string", "wstring" };
}

bool CheckIO::ArgumentInfo::isStdVectorOrString()
{
    if (!isCPP)
        return false;
    if (variableInfo->isStlType(stl_vector)) {
        typeToken = variableInfo->typeStartToken()->tokAt(4);
        _template = true;
        return true;
    } else if (variableInfo->isStlType(stl_string)) {
        tempToken = new Token();
        tempToken->fileIndex(variableInfo->typeStartToken()->fileIndex());
        tempToken->linenr(variableInfo->typeStartToken()->linenr());
        if (variableInfo->typeStartToken()->strAt(2) == MatchCompiler::makeConstString("string"))
            tempToken->str("char");
        else
            tempToken->str("wchar_t");
        typeToken = tempToken;
        return true;
    } else if (variableInfo->type() && !variableInfo->type()->derivedFrom.empty()) {
        const std::vector<Type::BaseInfo>& derivedFrom = variableInfo->type()->derivedFrom;
        for (std::size_t i = 0, size = derivedFrom.size(); i < size; ++i) {
            const Token* nameTok = derivedFrom[i].nameTok;
            if (match39(nameTok)) {
                typeToken = nameTok->tokAt(4);
                _template = true;
                return true;
            } else if (match40(nameTok)) {
                tempToken = new Token();
                tempToken->fileIndex(variableInfo->typeStartToken()->fileIndex());
                tempToken->linenr(variableInfo->typeStartToken()->linenr());
                if (nameTok->strAt(2) == MatchCompiler::makeConstString("string"))
                    tempToken->str("char");
                else
                    tempToken->str("wchar_t");
                typeToken = tempToken;
                return true;
            }
        }
    } else if (variableInfo->type()) {
        const Scope * classScope = variableInfo->type()->classScope;
        if (classScope) {
            for (const Function &func : classScope->functionList) {
                if (func.name() == MatchCompiler::makeConstString("operator[]")) {
                    if (match41(func.retDef)) {
                        typeToken = func.retDef;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static const std::set<std::string> stl_container = {
    "array", "bitset", "deque", "forward_list",
    "hash_map", "hash_multimap", "hash_set",
    "list", "map", "multimap", "multiset",
    "priority_queue", "queue", "set", "stack",
    "unordered_map", "unordered_multimap", "unordered_multiset", "unordered_set", "vector"
};

bool CheckIO::ArgumentInfo::isStdContainer(const Token *tok)
{
    if (!isCPP)
        return false;
    if (tok && tok->variable()) {
        const Variable* variable = tok->variable();
        if (variable->isStlType(stl_container)) {
            typeToken = variable->typeStartToken()->tokAt(4);
            return true;
        } else if (variable->isStlType(stl_string)) {
            typeToken = variable->typeStartToken();
            return true;
        } else if (variable->type() && !variable->type()->derivedFrom.empty()) {
            for (const Type::BaseInfo &baseInfo : variable->type()->derivedFrom) {
                const Token* nameTok = baseInfo.nameTok;
                if (match42(nameTok)) {
                    typeToken = nameTok->tokAt(4);
                    return true;
                } else if (match40(nameTok)) {
                    typeToken = nameTok;
                    return true;
                }
            }
        }
    }

    return false;
}

bool CheckIO::ArgumentInfo::isArrayOrPointer() const
{
    if (address)
        return true;
    else if (variableInfo && !_template) {
        return variableInfo->isArrayOrPointer();
    } else {
        const Token *tok = typeToken;
        while (match43(tok))
            tok = tok->next();
        if (tok && tok->strAt(1) == MatchCompiler::makeConstString("*"))
            return true;
    }
    return false;
}

bool CheckIO::ArgumentInfo::isComplexType() const
{
    if (variableInfo->type())
        return (true);

    const Token* varTypeTok = typeToken;
    if (varTypeTok->str() == MatchCompiler::makeConstString("std"))
        varTypeTok = varTypeTok->tokAt(2);

    return ((variableInfo->isStlStringType() || (varTypeTok->strAt(1) == MatchCompiler::makeConstString("<") && varTypeTok->linkAt(1) && varTypeTok->linkAt(1)->strAt(1) != MatchCompiler::makeConstString("::"))) && !variableInfo->isArrayOrPointer());
}

bool CheckIO::ArgumentInfo::isKnownType() const
{
    if (variableInfo)
        return (typeToken->isStandardType() || typeToken->next()->isStandardType() || isComplexType());
    else if (functionInfo)
        return (typeToken->isStandardType() || functionInfo->retType || match40(typeToken));

    return typeToken->isStandardType() || match40(typeToken);
}

bool CheckIO::ArgumentInfo::isLibraryType(const Settings *settings) const
{
    return typeToken && typeToken->isStandardType() && settings->library.podtype(typeToken->str());
}

void CheckIO::wrongPrintfScanfArgumentsError(const Token* tok,
        const std::string &functionName,
        unsigned int numFormat,
        unsigned int numFunction)
{
    const Severity::SeverityType severity = numFormat > numFunction ? Severity::error : Severity::warning;
    if (severity != Severity::error && !mSettings->isEnabled(Settings::WARNING))
        return;

    std::ostringstream errmsg;
    errmsg << functionName
           << " format string requires "
           << numFormat
           << " parameter" << (numFormat != 1 ? "s" : "") << " but "
           << (numFormat > numFunction ? "only " : "")
           << numFunction
           << (numFunction != 1 ? " are" : " is")
           << " given.";

    reportError(tok, severity, "wrongPrintfScanfArgNum", errmsg.str(), CWE685, false);
}

void CheckIO::wrongPrintfScanfPosixParameterPositionError(const Token* tok, const std::string& functionName,
        unsigned int index, unsigned int numFunction)
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;
    std::ostringstream errmsg;
    errmsg << functionName << ": ";
    if (index == 0) {
        errmsg << "parameter positions start at 1, not 0";
    } else {
        errmsg << "referencing parameter " << index << " while " << numFunction << " arguments given";
    }
    reportError(tok, Severity::warning, "wrongPrintfScanfParameterPositionError", errmsg.str(), CWE685, false);
}

void CheckIO::invalidScanfArgTypeError_s(const Token* tok, unsigned int numFormat, const std::string& specifier, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%" << specifier << " in format string (no. " << numFormat << ") requires a \'";
    if (specifier[0] == 's')
        errmsg << "char";
    else if (specifier[0] == 'S')
        errmsg << "wchar_t";
    errmsg << " *\' but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidScanfArgType_s", errmsg.str(), CWE686, false);
}
void CheckIO::invalidScanfArgTypeError_int(const Token* tok, unsigned int numFormat, const std::string& specifier, const ArgumentInfo* argInfo, bool isUnsigned)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%" << specifier << " in format string (no. " << numFormat << ") requires \'";
    if (specifier[0] == 'h') {
        if (specifier[1] == 'h')
            errmsg << (isUnsigned ? "unsigned " : "") << "char";
        else
            errmsg << (isUnsigned ? "unsigned " : "") << "short";
    } else if (specifier[0] == 'l') {
        if (specifier[1] == 'l')
            errmsg << (isUnsigned ? "unsigned " : "") << "long long";
        else
            errmsg << (isUnsigned ? "unsigned " : "") << "long";
    } else if (specifier.find("I32") != std::string::npos) {
        errmsg << (isUnsigned ? "unsigned " : "") << "__int32";
    } else if (specifier.find("I64") != std::string::npos) {
        errmsg << (isUnsigned ? "unsigned " : "") << "__int64";
    } else if (specifier[0] == 'I') {
        errmsg << (isUnsigned ? "size_t" : "ptrdiff_t");
    } else if (specifier[0] == 'j') {
        if (isUnsigned)
            errmsg << "uintmax_t";
        else
            errmsg << "intmax_t";
    } else if (specifier[0] == 'z') {
        if (specifier[1] == 'd')
            errmsg << "ssize_t";
        else
            errmsg << "size_t";
    } else if (specifier[0] == 't') {
        errmsg << (isUnsigned ? "unsigned " : "") << "ptrdiff_t";
    } else if (specifier[0] == 'L') {
        errmsg << (isUnsigned ? "unsigned " : "") << "long long";
    } else {
        errmsg << (isUnsigned ? "unsigned " : "") << "int";
    }
    errmsg << " *\' but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidScanfArgType_int", errmsg.str(), CWE686, false);
}
void CheckIO::invalidScanfArgTypeError_float(const Token* tok, unsigned int numFormat, const std::string& specifier, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%" << specifier << " in format string (no. " << numFormat << ") requires \'";
    if (specifier[0] == 'l' && specifier[1] != 'l')
        errmsg << "double";
    else if (specifier[0] == 'L')
        errmsg << "long double";
    else
        errmsg << "float";
    errmsg << " *\' but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidScanfArgType_float", errmsg.str(), CWE686, false);
}

void CheckIO::invalidPrintfArgTypeError_s(const Token* tok, unsigned int numFormat, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%s in format string (no. " << numFormat << ") requires \'char *\' but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidPrintfArgType_s", errmsg.str(), CWE686, false);
}
void CheckIO::invalidPrintfArgTypeError_n(const Token* tok, unsigned int numFormat, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%n in format string (no. " << numFormat << ") requires \'int *\' but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidPrintfArgType_n", errmsg.str(), CWE686, false);
}
void CheckIO::invalidPrintfArgTypeError_p(const Token* tok, unsigned int numFormat, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%p in format string (no. " << numFormat << ") requires an address but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidPrintfArgType_p", errmsg.str(), CWE686, false);
}
static void printfFormatType(std::ostream& os, const std::string& specifier, bool isUnsigned)
{
    os << "\'";
    if (specifier[0] == 'l') {
        if (specifier[1] == 'l')
            os << (isUnsigned ? "unsigned " : "") << "long long";
        else
            os << (isUnsigned ? "unsigned " : "") << "long";
    } else if (specifier[0] == 'h') {
        if (specifier[1] == 'h')
            os << (isUnsigned ? "unsigned " : "") << "char";
        else
            os << (isUnsigned ? "unsigned " : "") << "short";
    } else if (specifier.find("I32") != std::string::npos) {
        os << (isUnsigned ? "unsigned " : "") << "__int32";
    } else if (specifier.find("I64") != std::string::npos) {
        os << (isUnsigned ? "unsigned " : "") << "__int64";
    } else if (specifier[0] == 'I') {
        os << (isUnsigned ? "size_t" : "ptrdiff_t");
    } else if (specifier[0] == 'j') {
        if (isUnsigned)
            os << "uintmax_t";
        else
            os << "intmax_t";
    } else if (specifier[0] == 'z') {
        if (specifier[1] == 'd')
            os << "ssize_t";
        else
            os << "size_t";
    } else if (specifier[0] == 't') {
        os << (isUnsigned ? "unsigned " : "") << "ptrdiff_t";
    } else if (specifier[0] == 'L') {
        os << (isUnsigned ? "unsigned " : "") << "long long";
    } else {
        os << (isUnsigned ? "unsigned " : "") << "int";
    }
    os << "\'";
}

void CheckIO::invalidPrintfArgTypeError_uint(const Token* tok, unsigned int numFormat, const std::string& specifier, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%" << specifier << " in format string (no. " << numFormat << ") requires ";
    printfFormatType(errmsg, specifier, true);
    errmsg << " but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidPrintfArgType_uint", errmsg.str(), CWE686, false);
}

void CheckIO::invalidPrintfArgTypeError_sint(const Token* tok, unsigned int numFormat, const std::string& specifier, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%" << specifier << " in format string (no. " << numFormat << ") requires ";
    printfFormatType(errmsg, specifier, false);
    errmsg << " but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidPrintfArgType_sint", errmsg.str(), CWE686, false);
}
void CheckIO::invalidPrintfArgTypeError_float(const Token* tok, unsigned int numFormat, const std::string& specifier, const ArgumentInfo* argInfo)
{
    const Severity::SeverityType severity = getSeverity(argInfo);
    if (!mSettings->isEnabled(severity))
        return;
    std::ostringstream errmsg;
    errmsg << "%" << specifier << " in format string (no. " << numFormat << ") requires \'";
    if (specifier[0] == 'L')
        errmsg << "long ";
    errmsg << "double\' but the argument type is ";
    argumentType(errmsg, argInfo);
    errmsg << ".";
    reportError(tok, severity, "invalidPrintfArgType_float", errmsg.str(), CWE686, false);
}

Severity::SeverityType CheckIO::getSeverity(const CheckIO::ArgumentInfo *argInfo)
{
    return (argInfo && argInfo->typeToken && !argInfo->typeToken->originalName().empty()) ? Severity::portability : Severity::warning;
}

void CheckIO::argumentType(std::ostream& os, const ArgumentInfo * argInfo)
{
    if (argInfo) {
        os << "\'";
        const Token *type = argInfo->typeToken;
        if (type->tokType() == Token::eString) {
            if (type->isLong())
                os << "const wchar_t *";
            else
                os << "const char *";
        } else {
            if (type->originalName().empty()) {
                if (type->strAt(-1) == MatchCompiler::makeConstString("const"))
                    os << "const ";
                while (match43(type)) {
                    os << type->str() << " ";
                    type = type->next();
                }
                while (match44(type)) {
                    os << type->str() << "::";
                    type = type->tokAt(2);
                }
                type->stringify(os, false, true, false);
                if (type->strAt(1) == MatchCompiler::makeConstString("*") && !argInfo->element)
                    os << " *";
                else if (argInfo->variableInfo && !argInfo->element && argInfo->variableInfo->isArray())
                    os << " *";
                else if (type->strAt(1) == MatchCompiler::makeConstString("*") && argInfo->variableInfo && argInfo->element && argInfo->variableInfo->isArray())
                    os << " *";
                if (argInfo->address)
                    os << " *";
            } else {
                if (type->isUnsigned()) {
                    if (type->originalName() == MatchCompiler::makeConstString("__int64") || type->originalName() == MatchCompiler::makeConstString("__int32") || type->originalName() == MatchCompiler::makeConstString("ptrdiff_t"))
                        os << "unsigned ";
                }
                os << type->originalName();
                if (type->strAt(1) == MatchCompiler::makeConstString("*") || argInfo->address)
                    os << " *";
                os << " {aka ";
                type->stringify(os, false, true, false);
                if (type->strAt(1) == MatchCompiler::makeConstString("*") || argInfo->address)
                    os << " *";
                os << "}";
            }
        }
        os << "\'";
    } else
        os << "Unknown";
}

void CheckIO::invalidLengthModifierError(const Token* tok, unsigned int numFormat, const std::string& modifier)
{
    if (!mSettings->isEnabled(Settings::WARNING))
        return;
    std::ostringstream errmsg;
    errmsg << "'" << modifier << "' in format string (no. " << numFormat << ") is a length modifier and cannot be used without a conversion specifier.";
    reportError(tok, Severity::warning, "invalidLengthModifierError", errmsg.str(), CWE704, false);
}

void CheckIO::invalidScanfFormatWidthError(const Token* tok, unsigned int numFormat, int width, const Variable *var, char c)
{
    MathLib::bigint arrlen = 0;
    std::string varname;

    if (var) {
        arrlen = var->dimension(0);
        varname = var->name();
    }

    std::ostringstream errmsg;
    if (arrlen > width) {
        if (!mSettings->inconclusive || !mSettings->isEnabled(Settings::WARNING))
            return;
        errmsg << "Width " << width << " given in format string (no. " << numFormat << ") is smaller than destination buffer"
               << " '" << varname << "[" << arrlen << "]'.";
        reportError(tok, Severity::warning, "invalidScanfFormatWidth_smaller", errmsg.str(), CWE(0U), true);
    } else {
        errmsg << "Width " << width << " given in format string (no. " << numFormat << ") is larger than destination buffer '"
               << varname << "[" << arrlen << "]', use %" << (c == 'c' ? arrlen : (arrlen - 1)) << c << " to prevent overflowing it.";
        reportError(tok, Severity::error, "invalidScanfFormatWidth", errmsg.str(), CWE687, false);
    }
}
