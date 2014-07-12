/***************************************************************************
  A SymbolBrowser plugin for JuffEd editor.

  Copyright: 2010 Aleksander Sokoloff <asokol@mail.ru>

  Some of the code based on a ctags project http://ctags.sourceforge.net/

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
***************************************************************************/

#ifndef PARSER_PYTHON
#define PARSER_PYTHON

#include <QDebug>
#include "../common.h"
#include "vstring.h"
#include "parser.h"
#include "../symbol.h"

//#define SHOW_IMPORTS
//#define SHOW_VARIABLES


/*****************************************************************************
 *****************************************************************************/
class PythonSymbol: public Symbol {
public:
    PythonSymbol(const Symbol::SymbolType type=SymbolUnknown, const QString &name="", Symbol *parent = NULL): Symbol(type, name, parent) {};
    int indent() const { return mIndent; }
    void setIndent(int value) { mIndent = value; };

private:
    int mIndent;
};




static char const * const singletriple = "'''";
static char const * const doubletriple = "\"\"\"";



class Parser_Python: public Parser {

public:
    Parser_Python();

protected:
    void parse();

private:
    QList<PythonSymbol*> nestingSymbols;
    PythonSymbol* getParent(int indent);
    void checkParent(int indent, const vString  *parent);
    void addNestingSymbol(PythonSymbol *symbol);

    bool isIdentifierFirstCharacter (int c) const;
    bool isIdentifierCharacter (int c) const;

    const char *skipString (const char *cp);
    const char *skipEverything (const char *cp);
    const char *skipIdentifier (const char *cp);
    const char *findDefinitionOrClass (const char *cp);
    const char *skipSpace (const char *cp);
    const char *parseIdentifier (const char *cp, vString *const identifier) const;

    PythonSymbol* makeClass (const char *cp, vString *const className, Symbol *parent);
    PythonSymbol* makeFunction (const char *cp, vString *const def,  Symbol *parent);

    void parseImports (const char *cp);
    vString *parseArgs(const char *buf);

    char const *find_triple_start(char const *string, char const **which);
    void find_triple_end(char const *string, char const **which);
    const char *findVariable(const char *line);
    const char *skipTypeDecl(const char *cp, bool *is_class);
    int calcIndent(const char *line);
};


/*****************************************************************************
 *****************************************************************************/
Parser_Python::Parser_Python() {

}



/*****************************************************************************
*****************************************************************************/
bool Parser_Python::isIdentifierFirstCharacter (int c) const
{
    return (bool) (isalpha (c) || c == '_');
}


/*****************************************************************************
*****************************************************************************/
bool Parser_Python::isIdentifierCharacter (int c) const
{
    return (bool) (isalnum (c) || c == '_');
}


/*****************************************************************************
 Skip a single or double quoted string.
 *****************************************************************************/
const char *Parser_Python::skipString (const char *cp)
{
    const char *start = cp;
    int escaped = 0;
    for (cp++; *cp; cp++)
    {
        if (escaped)
            escaped--;
        else if (*cp == '\\')
            escaped++;
        else if (*cp == *start)
            return cp + 1;
    }
    return cp;
}


/*****************************************************************************
 Skip everything up to an identifier start.
 *****************************************************************************/
const char *Parser_Python::skipEverything (const char *cp)
{
    for (; *cp; cp++)
    {
        if (*cp == '"' || *cp == '\'')
        {
            cp = skipString(cp);
            if (!*cp) break;
        }
        if (isIdentifierFirstCharacter ((int) *cp))
            return cp;
    }
    return cp;
}


/*****************************************************************************
 Skip an identifier.
 *****************************************************************************/
const char *Parser_Python::skipIdentifier (const char *cp)
{
    while (isIdentifierCharacter ((int) *cp))
        cp++;
    return cp;
}


/*****************************************************************************
 *****************************************************************************/
const char *Parser_Python::findDefinitionOrClass (const char *cp)
{
    while (*cp)
    {
        cp = skipEverything (cp);
        if (!strncmp(cp, "def", 3) || !strncmp(cp, "class", 5) ||
            !strncmp(cp, "cdef", 4) || !strncmp(cp, "cpdef", 5))
        {
            return cp;
        }
        cp = skipIdentifier (cp);
    }
    return NULL;
}


/*****************************************************************************
 *****************************************************************************/
const char *Parser_Python::skipSpace (const char *cp)
{
    while (isspace ((int) *cp))
        ++cp;
    return cp;
}


/*****************************************************************************
 Starting at ''cp'', parse an identifier into ''identifier''.
 *****************************************************************************/
const char *Parser_Python::parseIdentifier (const char *cp, vString *const identifier) const
{
    vStringClear (identifier);
    while (isIdentifierCharacter ((int) *cp))
    {
        vStringPut (identifier, (int) *cp);
        ++cp;
    }
    vStringTerminate (identifier);
    return cp;
}


/*****************************************************************************
  Given a string with the contents of the line directly after the "class"
  keyword, extract all necessary information and create a tag.
 *****************************************************************************/
PythonSymbol* Parser_Python::makeClass (const char *cp, vString *const className, Symbol *parent)
{
    int lineNum = getSourceLineNumber() - 1;
    cp = parseIdentifier (cp, className);
    vString *const inheritance = parseArgs(cp);

    Symbol * parentSymbol = (parent) ? parent : mSymbols;

    QString name = vStringToQString(className);
    PythonSymbol *symbol = new PythonSymbol(Symbol::SymbolClass, name, parentSymbol);
    symbol->setDetailedText(QString("%1 (%2)").arg(name).arg(vStringToQString(inheritance).simplified()));
    symbol->setLine(lineNum);


    vStringDelete (inheritance);
    return symbol;
}


/*****************************************************************************
 Given a string with the contents of a line directly after the "def" keyword,
 extract all relevant information and create a symbol.
 *****************************************************************************/
PythonSymbol* Parser_Python::makeFunction (const char *cp, vString *const def,  Symbol *parent)
{
    int lineNum = getSourceLineNumber() - 1;
    cp = parseIdentifier(cp, def);
    vString *const args = parseArgs(cp);
    Symbol * parentSymbol = (parent) ? parent : mSymbols;

    QString name = vStringToQString(def);
    PythonSymbol *symbol = new PythonSymbol(Symbol::SymbolFunc, name, parentSymbol);
    symbol->setDetailedText(QString("%1 (%2)").arg(name).arg(vStringToQString(args)).simplified());
    symbol->setLine(lineNum);

    vStringDelete (args);
    return symbol;

}


/*****************************************************************************
 Badly
 *****************************************************************************/
void Parser_Python::parseImports (const char *cp)
{
    const char *pos;
    vString *name, *name_next;

    cp = skipEverything (cp);

    if ((pos = strstr (cp, "import")) == NULL)
        return;

    cp = pos + 6;

    /* continue only if there is some space between the keyword and the identifier */
    if (! isspace (*cp))
        return;

    cp++;
    cp = skipSpace (cp);

    name = vStringNew ();
    name_next = vStringNew ();

    cp = skipEverything (cp);
    while (*cp)
    {
        cp = parseIdentifier (cp, name);

        cp = skipEverything (cp);
        /* we parse the next possible import statement as well to be able to ignore 'foo' in
         * 'import foo as bar' */
        parseIdentifier (cp, name_next);

        /* take the current tag only if the next one is not "as" */
        if (strcmp (vStringValue (name_next), "as") != 0 &&
            strcmp (vStringValue (name), "as") != 0)
        {
            Symbol *symbol = new Symbol(Symbol::SymbolNamespace, vStringToQString(name), mSymbols);
            symbol->setLine(getInputLineNumber());
        }
    }
    vStringDelete (name);
    vStringDelete (name_next);
}


/*****************************************************************************
 *****************************************************************************/
vString *Parser_Python::parseArgs(const char *buf)
{
    vString *const res = vStringNew ();
    const char *cp = skipSpace (buf);
    if (*cp == '(') {
        ++cp;

        while (*cp != ')') {
            if (*cp == '\0') {
                /* Closing parenthesis can be in follow up line. */
                cp = (const char *) fileReadLine ();
                if (!cp) break;
                vStringPut (res, ' ');
                continue;
            }

            vStringPut(res, *cp);
            ++cp;
        }

        vStringTerminate (res);
    }

    return res;
}


/*****************************************************************************

 *****************************************************************************/
PythonSymbol* Parser_Python::getParent(int indent) {
    PythonSymbol *s = NULL;

    for (int i=0; i<nestingSymbols.count(); ++i) {
        if (nestingSymbols.at(i)->indent() >= indent)
            break;
        s = nestingSymbols.at(i);
    }

    return s;
}


/*****************************************************************************
 Check whether parent's indentation level is higher than the current level and
 if so, remove it.
*****************************************************************************/
void Parser_Python::checkParent(int indent, const vString  *parent) {
    QString parentStr = vStringToQString(parent);

    for (int i=0; i<nestingSymbols.count(); ++i) {
        PythonSymbol *ns = nestingSymbols.at(i);

        if (parentStr == ns->name()) {

            if (indent <= ns->indent()) {
                nestingSymbols.removeAt(i);
            }

            break;
        }
    }
}


/*****************************************************************************
 *****************************************************************************/
void Parser_Python::addNestingSymbol(PythonSymbol *symbol) {

    QList<PythonSymbol*>::iterator i;
    for (i=nestingSymbols.begin(); i != nestingSymbols.end(); ++i) {
        if (symbol->indent() <= (*i)->indent())
            break;
    }

    nestingSymbols.erase(i, nestingSymbols.end());
    nestingSymbols.append(symbol);

}


/*****************************************************************************
 Return a pointer to the start of the next triple string, or NULL. Store
 the kind of triple string in "which" if the return is not NULL.
*****************************************************************************/
char const *Parser_Python::find_triple_start(char const *string, char const **which)
{
    char const *cp = string;

    for (; *cp; cp++)
    {
        if (*cp == '"' || *cp == '\'')
        {
            if (strncmp(cp, doubletriple, 3) == 0)
            {
                *which = doubletriple;
                return cp;
            }
            if (strncmp(cp, singletriple, 3) == 0)
            {
                *which = singletriple;
                return cp;
            }
            cp = skipString(cp);
            if (!*cp) break;
        }
    }
    return NULL;
}


/*****************************************************************************
 Find the end of a triple string as pointed to by "which", and update "which"
 with any other triple strings following in the given string.
 *****************************************************************************/
void Parser_Python::find_triple_end(char const *string, char const **which)
{
    char const *s = string;
    while (1)
    {
        /* Check if the string ends in the same line. */
        s = strstr (s, *which);
        if (!s) break;
        s += 3;
        *which = NULL;
        /* If yes, check if another one starts in the same line. */
        s = find_triple_start(s, which);
        if (!s) break;
        s += 3;
    }
}


/*****************************************************************************

 *****************************************************************************/
const char *Parser_Python::findVariable(const char *line)
{
    /* Parse global and class variable names (C.x) from assignment statements.
     * Object attributes (obj.x) are ignored.
     * Assignment to a tuple 'x, y = 2, 3' not supported.
     * TODO: ignore duplicate tags from reassignment statements. */
    const char *cp, *sp, *eq, *start;

    cp = strstr(line, "=");
    if (!cp)
        return NULL;

    eq = cp + 1;
    while (*eq)
    {
        if (*eq == '=')
            return NULL;	/* ignore '==' operator and 'x=5,y=6)' function lines */

        if (*eq == '(' || *eq == '#')
            break;	/* allow 'x = func(b=2,y=2,' lines and comments at the end of line */

        eq++;
    }

    /* go backwards to the start of the line, checking we have valid chars */
    start = cp - 1;
    while (start >= line && isspace ((int) *start))
        --start;

    while (start >= line && isIdentifierCharacter ((int) *start))
        --start;

    if (!isIdentifierFirstCharacter(*(start + 1)))
        return NULL;

    sp = start;
    while (sp >= line && isspace ((int) *sp))
        --sp;

    if ((sp + 1) != line)	/* the line isn't a simple variable assignment */
        return NULL;

    /* the line is valid, parse the variable name */
    ++start;
    return start;
}


/*****************************************************************************
 Skip type declaration that optionally follows a cdef/cpdef
 *****************************************************************************/
const char *Parser_Python::Parser_Python::skipTypeDecl(const char *cp, bool *is_class)
{
    const char *lastStart = cp, *ptr = cp;
    int loopCount = 0;
    ptr = skipSpace(cp);
    if (!strncmp("extern", ptr, 6)) {
        ptr += 6;
        ptr = skipSpace(ptr);
        if (!strncmp("from", ptr, 4)) { return NULL; }
    }
    if (!strncmp("class", ptr, 5)) {
        ptr += 5 ;
        *is_class = true;
        ptr = skipSpace(ptr);
        return ptr;
    }
    /* limit so that we don't pick off "int item=obj()" */
    while (*ptr && loopCount++ < 2) {
        while (*ptr && *ptr != '=' && *ptr != '(' && !isspace(*ptr)) ptr++;
        if (!*ptr || *ptr == '=') return NULL;
        if (*ptr == '(') {
            return lastStart; /* if we stopped on a '(' we are done */
        }
        ptr = skipSpace(ptr);
        lastStart = ptr;
        while (*lastStart == '*') lastStart++;  /* cdef int *identifier */
    }
    return NULL;
}


/*****************************************************************************
 *****************************************************************************/
int Parser_Python::calcIndent(const char *line) {
    int indent = 0;

    while(true) {
        switch (*line) {
            case ' ':
                indent+= 1;
                break;

            case '\t':
                indent+= 8;
                break;

            default:
                return indent;
        }
        line++;
    }

}


/*****************************************************************************
 *****************************************************************************/
void Parser_Python::parse()
{
    vString *const continuation = vStringNew ();
    vString *const name = vStringNew ();
    vString *const parent = vStringNew();

    const char *line;
    int line_skip = 0;
    char const *longStringLiteral = NULL;

    while ((line = (const char *) fileReadLine ()) != NULL)
    {
        const char *cp = line, *candidate;
        char const *longstring, *keyword;
        int indent;

        cp = skipSpace (cp);

        if (*cp == '\0')  /* skip blank line */
            continue;

        /* Skip comment if we are not inside a multi-line string. */
        if (*cp == '#' && !longStringLiteral)
            continue;

        /* Deal with line continuation. */
        if (!line_skip) vStringClear(continuation);


        vStringCatS(continuation, line);
        vStringStripTrailing(continuation);


        if (vStringLast(continuation) == '\\')
        {
            vStringChop(continuation);
            vStringCatS(continuation, " ");
            line_skip = 1;
            continue;
        }



        cp = line = vStringValue(continuation);
        cp = skipSpace (cp);
        indent = calcIndent(line);
        line_skip = 0;

        checkParent(indent, parent);

        /* Deal with multiline string ending. */
        if (longStringLiteral)
        {
            find_triple_end(cp, &longStringLiteral);
            continue;
        }

        /* Deal with multiline string start. */
        longstring = find_triple_start(cp, &longStringLiteral);
        if (longstring)
        {
            longstring += 3;
            find_triple_end(longstring, &longStringLiteral);
            /* We don't parse for any tags in the rest of the line. */
            continue;
        }

        /* Deal with def and class keywords. */
        keyword = findDefinitionOrClass (cp);
        if (keyword)
        {
            bool found = false;
            bool is_class = false;

            if (!strncmp (keyword, "def", 3) && isspace(keyword[3]))
            {
                cp = skipSpace (keyword + 3);
                found = true;
            }
            else if (!strncmp (keyword, "class", 5) && isspace(keyword[5]))
            {
                cp = skipSpace (keyword + 5);
                found = true;
                is_class = true;
            }
            else if (!strncmp (keyword, "cdef", 4) && isspace(keyword[4]))
            {
                cp = skipSpace(keyword + 4);
                candidate = skipTypeDecl (cp, &is_class);
                if (candidate)
                {
                    found = true;
                    cp = candidate;
                }

            }
            else if (!strncmp (keyword, "cpdef", 5) && isspace(keyword[5]))
            {
                cp = skipSpace(keyword + 5);
                candidate = skipTypeDecl (cp, &is_class);
                if (candidate)
                {
                    found = true;
                    cp = candidate;
                }
            }

            if (found)
            {
                PythonSymbol *symbol = NULL;
                PythonSymbol *parentSymbol;

                parentSymbol = getParent(indent);

                if (is_class)
                    symbol = makeClass (cp, name, parentSymbol);
                else
                    symbol = makeFunction(cp, name, parentSymbol);

                symbol->setIndent(indent);
                nestingSymbols.append(symbol);

                addNestingSymbol(symbol);
             //   continue;
            }
        }


        /* Find global and class variables */
#ifdef SHOW_VARIABLES
        char const *variable = findVariable(line);
        if (variable)
        {
            const char *start = variable;

            vStringClear (name);
            while (isIdentifierCharacter ((int) *start))
            {
                vStringPut (name, (int) *start);
                ++start;
            }
            vStringTerminate (name);

            PythonSymbol *symbol = NULL;
            PythonSymbol *parentSymbol = getParent(indent);


            symbol = new PythonSymbol(parentSymbol, vStringToQString(name));
            symbol->setIndent(indent);
            symbol->setLine(this->getSourceLineNumber());
            symbol->setIconType(Symbol::IconVar);
        }
#endif

        /* Find and parse imports */
#ifdef SHOW_IMPORTS
        parseImports(line);
#endif
    }

    /* Clean up all memory we allocated. */
    vStringDelete (parent);
    vStringDelete (name);
    vStringDelete (continuation);
}

#endif // PARSER_PYTHON
