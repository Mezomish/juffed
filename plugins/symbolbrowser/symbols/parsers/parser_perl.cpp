/***************************************************************************
  A SymbolBrowser Perl plugin for JuffEd editor.

  Copyright: 2011 Aleksander Zaitsev <shura0@yandex.ru>
  Some of the code based on python parser by Alexander Sokoloff <asokol@mail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/

#ifndef PARSER_PERL
#define PARSER_PERL

#include <QDebug>
#include "../common.h"
#include "vstring.h"
#include "parser.h"
#include "../symbol.h"

//#define SHOW_IMPORTS
//#define SHOW_VARIABLES


/*****************************************************************************
 *****************************************************************************/
class PerlSymbol: public Symbol {
public:
    PerlSymbol(const Symbol::SymbolType type=SymbolUnknown, const QString &name="", Symbol *parent = NULL): Symbol(type, name, parent) {};
    int indent() const { return mIndent; }
    void setIndent(int value) { mIndent = value; };

private:
    int mIndent;
};



class Parser_Perl: public Parser {

public:
    Parser_Perl();

protected:
    void parse();

private:
    QList<PerlSymbol*> nestingSymbols;
    PerlSymbol* getParent(int indent);
    //void checkParent(int indent, const vString  *parent);
    //void addNestingSymbol(PerlSymbol *symbol);

    bool isIdentifierFirstCharacter (char c) const;
    bool isIdentifierCharacter (int c) const;

    const char *skipString (const char *cp);
    const char *skipEverything (const char *cp);
    const char *skipIdentifier (const char *cp);
    const char *findDefinitionOrClass (const char *cp);
    const char *skipSpace (const char *cp);
    const char *parseIdentifier (const char *cp, QString *identifier) const;

    PerlSymbol* makeClass (const char *cp, Symbol *parent);
    PerlSymbol* makeFunction (const char *cp, QString *def,  Symbol *parent);

    QString parseArgs();

    //const char *findVariable(const char *line);
    int calcIndent(const char* cp);
};


/*****************************************************************************
 *****************************************************************************/
Parser_Perl::Parser_Perl() {

}



/*****************************************************************************
*****************************************************************************/
bool Parser_Perl::isIdentifierFirstCharacter (char c) const
{
    return (bool) (isalpha (c) || c == '_');
}


/*****************************************************************************
*****************************************************************************/
bool Parser_Perl::isIdentifierCharacter (int c) const
{
    return (bool) (isalnum (c) || c == '_');
}

/****************************************************************************/
int Parser_Perl::calcIndent(const char *line)
{
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
 Skip a single or double quoted string.
 *****************************************************************************/
const char *Parser_Perl::skipString (const char *cp)
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
const char *Parser_Perl::skipEverything (const char *cp)
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
const char *Parser_Perl::skipIdentifier (const char *cp)
{
    while (isIdentifierCharacter ((int) *cp))
        cp++;
    return cp;
}


/*****************************************************************************
 *****************************************************************************/
const char *Parser_Perl::findDefinitionOrClass (const char *cp)
{
    while (*cp)
    {
        cp = skipEverything (cp);
        if (!strncmp(cp, "sub", 3) || !strncmp(cp, "package", 7))
        {
            return cp;
        }
        cp = skipIdentifier (cp);
    }
    return NULL;
}


/*****************************************************************************
 *****************************************************************************/
const char *Parser_Perl::skipSpace (const char *cp)
{
    while (isspace ((int) *cp))
        ++cp;
    return cp;
}


/*****************************************************************************
 Starting at ''cp'', parse an identifier into ''identifier''.
 *****************************************************************************/
const char *Parser_Perl::parseIdentifier (const char *cp, QString *identifier) const
{
    std::string str;
    while (isIdentifierCharacter ((int)*cp))
    {
        str+=*cp;
        ++cp;
    }
    identifier->append(str.c_str());
    return cp;
}


/*****************************************************************************
  Given a string with the contents of the line directly after the "class"
  keyword, extract all necessary information and create a tag.
 *****************************************************************************/
PerlSymbol* Parser_Perl::makeClass (const char *cp, Symbol *parent)
{
    int lineNum = getSourceLineNumber()-1;
    QString *name=new QString();
    cp = parseIdentifier (cp, name);

    Symbol * parentSymbol = (parent) ? parent : mSymbols;

    PerlSymbol *symbol = new PerlSymbol(Symbol::SymbolClass, *name, parentSymbol);
    symbol->setDetailedText(QString("%1").arg(*name).simplified());
    symbol->setLine(lineNum);
    if(name)delete name;
    return symbol;
}


/*****************************************************************************
 Given a string with the contents of a line directly after the "def" keyword,
 extract all relevant information and create a symbol.
 *****************************************************************************/
PerlSymbol* Parser_Perl::makeFunction (const char *cp, QString *def,  Symbol *parent)
{
    int lineNum = getSourceLineNumber()-1;
    cp = parseIdentifier(cp, def);
    QString args = parseArgs();
    Symbol * parentSymbol = (parent) ? parent : mSymbols;
    PerlSymbol *symbol = new PerlSymbol(Symbol::SymbolFunc, *def, parentSymbol);
    symbol->setDetailedText(QString("%1 (%2)").arg(*def).arg(args).simplified());
    symbol->setLine(lineNum);
    def->clear();
    return symbol;

}

/*****************************************************************************
 *****************************************************************************/
QString Parser_Perl::parseArgs()
{
    QString res;
    const char *cp;
    while ((cp=(const char*)fileReadLine())!=NULL)
    {
        cp = skipSpace (cp);
        if(*cp=='\0')continue;
        if(*cp=='#') continue;
        if(strstr(cp,"{"))
            if(!(strstr(cp,"shift") || strstr(cp,"@_")))
                continue;
        if(strstr(cp,"shift") || strstr(cp, "@_"))
        {
        	if(strstr(cp,"$"))
  			  cp=strstr(cp,"$");
			else
			{
				if(strstr(cp,"@"))
				{
					if((strstr(cp,"@") != strstr(cp,"@_")))
					    cp=strstr(cp,"@");
					else
						cp=NULL;
				}
				else
					cp=NULL;
			}
            if(cp)
            {
                printf("no $!!\n");
                if(!res.isEmpty())res.append(", ");
                while(*cp !='=' && *cp!='\0' && *cp!=')' && *cp!=';')
                {
                  if(*cp!=' ')
                  	res.append(*cp);
                  cp++;
                }
            }else
                continue;
        }
        if(strstr(cp,"}"))break;
    }
    return res;
}


/*****************************************************************************

 *****************************************************************************/



/*****************************************************************************
 *****************************************************************************/


/*****************************************************************************
 *****************************************************************************/
void Parser_Perl::parse()
{
    QString continuation;
    QString *name=new QString();
    QString parent;
    PerlSymbol *parentSymbol=NULL;

    const char *line;
    int line_skip = 0;
    char const *longStringLiteral = NULL;

    while ((line = (const char *) fileReadLine ()) != NULL)
    {
        const char *cp = line;
        char const *keyword;
        //int indent;
        cp = skipSpace (cp);
        if (*cp == '\0')  /* skip blank line */
            continue;
        /* Skip comment if we are not inside a multi-line string. */
        if (*cp == '#' && !longStringLiteral)
            continue;
        /* Deal with line continuation. */
        if (!line_skip) continuation="";
        continuation+=QString(line);
        //vStringStripTrailing(continuation);


        cp = line = continuation.toLatin1().data();
        cp = skipSpace (cp);
        line_skip = 0;

        /* Deal with def and class keywords. */
        keyword = findDefinitionOrClass (cp);
        //printf("LINE_:%s\n",line);
        if (keyword)
        {
            bool found = FALSE;
            bool is_class = FALSE;

            if (!strncmp (keyword, "sub ", 4) && isspace(keyword[3]))
            {
                cp = skipSpace (keyword + 4);
                found = TRUE;
            }
            else if (!strncmp (keyword, "package", 7) && isspace(keyword[7]))
            {
                cp = skipSpace (keyword + 7);
                found = TRUE;
                is_class = TRUE;
            }

            if (found)
            {
                PerlSymbol *symbol = NULL;

                if (is_class)
                {
                    symbol = makeClass (cp, 0);
                    parentSymbol=symbol;
                }
                else
                    symbol = makeFunction(cp, name, parentSymbol);
            }
        }


        /* Find global and class variables */
#ifdef SHOW_VARIABLES
        //char const *variable = findVariable(line);
        char const* variable;
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

            PerlSymbol *symbol = NULL;
            PerlSymbol *parentSymbol = getParent(indent);


            symbol = new PerlSymbol(parentSymbol, vStringToQString(name));
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
    if(name)delete name;
}

#endif // PARSER_PERL
