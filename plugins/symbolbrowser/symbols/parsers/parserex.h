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

  You should have rceived a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/

#ifndef PARSEREX_H
#define PARSEREX_H
#include "parser.h"
#include "vstring.h"

class ParserEx : public Parser
{
public:
    ParserEx();
    virtual ~ParserEx();

protected:
    enum eCharacters {
            /* white space characters */
            SPACE         = ' ',
            NEWLINE       = '\n',
            CRETURN       = '\r',
            FORMFEED      = '\f',
            TAB           = '\t',
            VTAB          = '\v',

            /* some hard to read characters */
            DOUBLE_QUOTE  = '"',
            SINGLE_QUOTE  = '\'',
            BACKSLASH     = '\\',

            STRING_SYMBOL = ('S' + 0x80),
            CHAR_SYMBOL   = ('C' + 0x80)
    };


    void cppInit (const bool state, const bool hasAtLiteralStrings);
    void cppTerminate (void);

    bool isBraceFormat (void);
    unsigned int getDirectiveNestLevel (void);
    void cppBeginStatement (void);
    void cppEndStatement (void);
    void cppUngetc (const int c);
    int cppGetc (void);
    int skipOverCComment (void);


    /*****************************************************************************
      Is the character valid as a character of a C identifier?
      VMS allows '$' in identifiers.
    *****************************************************************************/
    bool isident(const char c) const { return (isalnum(c) || (c) == '_' || (c) == '$'); };

    /*****************************************************************************
      Is the character valid as the first character of a C identifier?
      C++ allows '~' in destructors. VMS allows '$' in identifiers.
    *****************************************************************************/
    bool isident1(const char c) const { return (isalpha(c) || (c) == '_' || (c) == '~' || (c) == '$'); }

private:
    enum Comment { COMMENT_NONE, COMMENT_C, COMMENT_CPLUS };

    enum eCppLimits {
            MaxCppNestingLevel = 20,
            MaxDirectiveName = 10
    };

    /*  Defines the one nesting level of a preprocessor conditional.
     */
    typedef struct sConditionalInfo {
            bool ignoreAllBranches;  /* ignoring parent conditional branch */
            bool singleBranch;       /* choose only one branch */
            bool branchChosen;       /* branch already selected */
            bool ignoring;           /* current ignore state */
    } conditionalInfo;

    enum eState {
            DRCTV_NONE,    /* no known directive - ignore to end of line */
            DRCTV_DEFINE,  /* "#define" encountered */
            DRCTV_HASH,    /* initial '#' read; determine directive */
            DRCTV_IF,      /* "#if" or "#ifdef" encountered */
            DRCTV_PRAGMA,  /* #pragma encountered */
            DRCTV_UNDEF    /* "#undef" encountered */
    };


    /*  Defines the current state of the pre-processor. */
    int	ungetch, ungetch2;      /* ungotten characters, if any */
    bool resolveRequired;       /* must resolve if/else/elif/endif branch */
    bool hasAtLiteralStrings;   /* supports @"c:\" strings */
    struct sDirective {
        enum eState state;      /* current directive being processed */
        bool accept;            /* is a directive syntactically permitted? */
        vString * name;         /* macro name */
        unsigned int nestLevel; /* level 0 is not used */
        conditionalInfo ifdef [MaxCppNestingLevel];
    } directive;

    /*  Use brace formatting to detect end of block. */
    bool BraceFormat;


    bool readDirective (int c, char *const name, unsigned int maxLength);
    void readIdentifier (int c, vString *const name);
    conditionalInfo *currentConditional (void);
    bool isIgnore (void);
    bool setIgnore (const bool ignore);
    bool isIgnoreBranch (void);
    void chooseBranch (void);
    bool pushConditional (const bool firstBranchChosen);
    bool popConditional (void);
    void makeDefineTag (const char *const name);
    void directiveDefine (const int c);
    void directivePragma (int c);
    bool directiveIf (const int c);
    bool directiveHash (const int c);
    bool handleDirective (const int c);
    Comment isComment (void);
    int skipOverCplusComment (void);
    int skipToEndOfString (bool ignoreBackslash);
    int skipToEndOfChar (void);

};

#endif // PARSEREX_H
