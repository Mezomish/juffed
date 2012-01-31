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

#include "parserex.h"

#define isspacetab(c)			((c) == SPACE || (c) == TAB)
#define stringMatch(s1,s2)		(strcmp (s1,s2) == 0)


/*****************************************************************************
 *****************************************************************************/
ParserEx::ParserEx() : Parser() {
    BraceFormat = false;
    ungetch = '\0';   /* ungetch characters */
    ungetch2 = '\0';  /* ungetch characters */
    resolveRequired = false; /* resolveRequired */
    hasAtLiteralStrings = false; /* hasAtLiteralStrings */


    directive.state = DRCTV_NONE;  /* current directive being processed */
    directive.accept = false;      /* is a directive syntactically permitted? */
    directive.name = NULL;         /* macro name */
    directive.nestLevel = 0;       /* level 0 is not used */

    for (int i=0; i<MaxCppNestingLevel; ++i ) {
        directive.ifdef[i].ignoreAllBranches = false;
        directive.ifdef[i].singleBranch = false;
        directive.ifdef[i].branchChosen = false;
        directive.ifdef[i].ignoring = false;
    }
}


/*****************************************************************************
 *****************************************************************************/
ParserEx::~ParserEx() {

}

/*****************************************************************************
 *****************************************************************************/
bool ParserEx::isBraceFormat (void) {
    return BraceFormat;
}


/*****************************************************************************
 *****************************************************************************/
unsigned int ParserEx::getDirectiveNestLevel (void) {
    return directive.nestLevel;
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::cppInit (const bool state, const bool hasAtLiteralStrings) {
    BraceFormat = state;

    ungetch         = '\0';
    ungetch2        = '\0';
    resolveRequired = FALSE;
    this->hasAtLiteralStrings = hasAtLiteralStrings;

    directive.state     = DRCTV_NONE;
    directive.accept    = TRUE;
    directive.nestLevel = 0;

    directive.ifdef [0].ignoreAllBranches = FALSE;
    directive.ifdef [0].singleBranch = FALSE;
    directive.ifdef [0].branchChosen = FALSE;
    directive.ifdef [0].ignoring     = FALSE;

    if (directive.name == NULL)
        directive.name = vStringNew ();
    else
        vStringClear (directive.name);
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::cppTerminate (void) {
    if (directive.name != NULL)
    {
        vStringDelete (directive.name);
        directive.name = NULL;
    }
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::cppBeginStatement (void){
    resolveRequired = TRUE;
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::cppEndStatement (void) {
    resolveRequired = FALSE;
}


/*
 *   Scanning functions
 *
 *   This section handles preprocessor directives.  It strips out all
 *   directives and may emit a tag for #define directives.
 */

/*****************************************************************************
  This puts a character back into the input queue for the source File.
  Up to two characters may be ungotten.
 *****************************************************************************/
void ParserEx::cppUngetc (const int c)
{
    Assert (ungetch2 == '\0');
    ungetch2 = ungetch;
    ungetch = c;
}

/*****************************************************************************
  Reads a directive, whose first character is given by "c", into "name".
 *****************************************************************************/
bool ParserEx::readDirective (int c, char *const name, unsigned int maxLength)
{
    unsigned int i;

    for (i = 0  ;  i < maxLength - 1  ;  ++i)
    {
        if (i > 0)
        {
            c = fileGetc ();
            if (c == EOF  ||  ! isalpha (c))
            {
                fileUngetc (c);
                break;
            }
        }
        name [i] = c;
    }
    name [i] = '\0';  /* null terminate */

    return (bool) isspacetab(c);
}


/*****************************************************************************
 Reads an identifier, whose first character is given by "c", into "tag",
 together with the file location and corresponding line number.
 *****************************************************************************/
void ParserEx::readIdentifier (int c, vString *const name)
{
    vStringClear (name);
    do
    {
        vStringPut (name, c);
    } while (c = fileGetc (), (c != EOF  &&  isident(c)));
    fileUngetc (c);
    vStringTerminate (name);
}


/*****************************************************************************
 *****************************************************************************/
ParserEx::conditionalInfo *ParserEx::currentConditional (void)
{
    return &directive.ifdef [directive.nestLevel];
}


/*****************************************************************************
 *****************************************************************************/
bool ParserEx::isIgnore (void)
{
    return directive.ifdef [directive.nestLevel].ignoring;
}


/*****************************************************************************
 *****************************************************************************/
bool ParserEx::setIgnore (const bool ignore)
{
    return directive.ifdef [directive.nestLevel].ignoring = ignore;
}


/*****************************************************************************
 *****************************************************************************/
bool ParserEx::isIgnoreBranch (void)
{
    conditionalInfo *const ifdef = currentConditional ();

    /*  Force a single branch if an incomplete statement is discovered
     *  en route. This may have allowed earlier branches containing complete
     *  statements to be followed, but we must follow no further branches.
     */
    if (resolveRequired  &&  ! BraceFormat)
        ifdef->singleBranch = TRUE;

    /*  We will ignore this branch in the following cases:
     *
     *  1.  We are ignoring all branches (conditional was within an ignored
     *        branch of the parent conditional)
     *  2.  A branch has already been chosen and either of:
     *      a.  A statement was incomplete upon entering the conditional
     *      b.  A statement is incomplete upon encountering a branch
     */
    return (bool) (ifdef->ignoreAllBranches ||
                      (ifdef->branchChosen  &&  ifdef->singleBranch));
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::chooseBranch (void)
{
    if (! BraceFormat)
    {
        conditionalInfo *const ifdef = currentConditional ();

        ifdef->branchChosen = (bool) (ifdef->singleBranch ||
                                         resolveRequired);
    }
}


/*****************************************************************************
  Pushes one nesting level for an #if directive, indicating whether or not
   the branch should be ignored and whether a branch has already been chosen.
 *****************************************************************************/
bool ParserEx::pushConditional (const bool firstBranchChosen)
{
    const bool ignoreAllBranches = isIgnore ();  /* current ignore */
    bool ignoreBranch = FALSE;

    if (directive.nestLevel < (unsigned int) MaxCppNestingLevel - 1)
    {
        conditionalInfo *ifdef;

        ++directive.nestLevel;
        ifdef = currentConditional ();

        /*  We take a snapshot of whether there is an incomplete statement in
         *  progress upon encountering the preprocessor conditional. If so,
         *  then we will flag that only a single branch of the conditional
         *  should be followed.
         */
        ifdef->ignoreAllBranches = ignoreAllBranches;
        ifdef->singleBranch      = resolveRequired;
        ifdef->branchChosen      = firstBranchChosen;
        ifdef->ignoring = (bool) (ignoreAllBranches || (
                ! firstBranchChosen  &&  ! BraceFormat  &&
                (ifdef->singleBranch /*|| !Option.if0*/)));

        ignoreBranch = ifdef->ignoring;
    }
    return ignoreBranch;
}


/*****************************************************************************
   Pops one nesting level for an #endif directive.
 *****************************************************************************/
bool ParserEx::popConditional (void)
{
    if (directive.nestLevel > 0)
        --directive.nestLevel;

    return isIgnore ();
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::makeDefineTag (const char *const name)
{
Q_UNUSED(name)
//    qDebug() << "ParserEx::makeDefineTag" << name;
//    const bool isFileScope = (bool) (! isHeaderFile ());
//
//    if (includingDefineTags () &&
//        (! isFileScope  ||  Option.include.fileScope))
//    {
//        tagEntryInfo e;
//        initTagEntry (&e, name);
//        e.lineNumberEntry = (bool) (Option.locate != EX_PATTERN);
//        e.isFileScope  = isFileScope;
//        e.truncateLine = TRUE;
//        e.kindName     = "macro";
//        e.kind         = 'd';
//        makeTagEntry (&e);
//    }
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::directiveDefine (const int c)
{
    if (isident1 (c))
    {
        readIdentifier (c, directive.name);
        if (! isIgnore ())
            makeDefineTag (vStringValue (directive.name));
    }
    directive.state = DRCTV_NONE;
}


/*****************************************************************************
 *****************************************************************************/
void ParserEx::directivePragma (int c)
{
    if (isident1 (c))
    {
        readIdentifier (c, directive.name);
        if (stringMatch (vStringValue (directive.name), "weak"))
        {
            /* generate macro tag for weak name */
            do
            {
                c = fileGetc ();
            } while (c == SPACE);
            if (isident1 (c))
            {
                readIdentifier (c, directive.name);
                makeDefineTag (vStringValue (directive.name));
            }
        }
    }
    directive.state = DRCTV_NONE;
}


/*****************************************************************************
 *****************************************************************************/
bool ParserEx::directiveIf (const int c)
{
    DebugStatement ( const bool ignore0 = isIgnore (); )
            const bool ignore = pushConditional ((bool) (c != '0'));

    directive.state = DRCTV_NONE;
    DebugStatement ( debugCppNest (TRUE, directive.nestLevel);
    if (ignore != ignore0) debugCppIgnore (ignore); )

            return ignore;
}


/*****************************************************************************
 *****************************************************************************/
bool ParserEx::directiveHash (const int c)
{
    bool ignore = FALSE;
    char directiveStr [MaxDirectiveName];
    DebugStatement( const bool ignore0 = isIgnore (); )

    readDirective (c, directiveStr, MaxDirectiveName);
    if (stringMatch (directiveStr, "define"))
        directive.state = DRCTV_DEFINE;
    else if (stringMatch (directiveStr, "undef"))
        directive.state = DRCTV_UNDEF;
    else if (strncmp (directiveStr, "if", (size_t) 2) == 0)
        directive.state = DRCTV_IF;
    else if (stringMatch (directiveStr, "elif")  ||
             stringMatch (directiveStr, "else"))
    {
        ignore = setIgnore (isIgnoreBranch ());
        if (! ignore  &&  stringMatch (directiveStr, "else"))
            chooseBranch ();
        directive.state = DRCTV_NONE;
        DebugStatement ( if (ignore != ignore0) debugCppIgnore (ignore); )
    }
    else if (stringMatch (directiveStr, "endif"))
    {
        DebugStatement ( debugCppNest (FALSE, directive.nestLevel); )
        ignore = popConditional ();
        directive.state = DRCTV_NONE;
        DebugStatement ( if (ignore != ignore0) debugCppIgnore (ignore); )
    }
    else if (stringMatch (directiveStr, "pragma"))
        directive.state = DRCTV_PRAGMA;
    else
        directive.state = DRCTV_NONE;

    return ignore;
}


/*****************************************************************************
  Handles a pre-processor directive whose first character is given by "c".
 *****************************************************************************/
bool ParserEx::handleDirective (const int c)
{
    bool ignore = isIgnore ();

    switch (directive.state)
    {
    case DRCTV_NONE:    ignore = isIgnore ();        break;
    case DRCTV_DEFINE:  directiveDefine (c);         break;
    case DRCTV_HASH:    ignore = directiveHash (c);  break;
    case DRCTV_IF:      ignore = directiveIf (c);    break;
    case DRCTV_PRAGMA:  directivePragma (c);         break;
    case DRCTV_UNDEF:   directiveDefine (c);         break;
    }
    return ignore;
}


/*****************************************************************************
  Called upon reading of a slash ('/') characters, determines whether a
  comment is encountered, and its type.
 *****************************************************************************/
ParserEx::Comment ParserEx::isComment (void)
{
        Comment comment;
        const int next = fileGetc ();

        if (next == '*')
                comment = COMMENT_C;
        else if (next == '/')
                comment = COMMENT_CPLUS;
        else
        {
                fileUngetc (next);
                comment = COMMENT_NONE;
        }
        return comment;
}


/*****************************************************************************
 Skips over a C style comment. According to ANSI specification a comment
 is treated as white space, so we perform this substitution.
 *****************************************************************************/
int ParserEx::skipOverCComment (void)
{
    int c = fileGetc ();

    while (c != EOF)
    {
        if (c != '*')
            c = fileGetc ();
        else
        {
            const int next = fileGetc ();

            if (next != '/')
                c = next;
            else
            {
                c = SPACE;  /* replace comment with space */
                break;
            }
        }
    }
    return c;
}

/*****************************************************************************
  Skips over a C++ style comment.
 *****************************************************************************/
int ParserEx::skipOverCplusComment (void)
{
    int c;

    while ((c = fileGetc ()) != EOF)
    {
        if (c == BACKSLASH)
            fileGetc ();  /* throw away next character, too */
        else if (c == NEWLINE)
            break;
    }
    return c;
}


/*****************************************************************************
 Skips to the end of a string, returning a special character to
 symbolically represent a generic string.
 *****************************************************************************/
int ParserEx::skipToEndOfString (bool ignoreBackslash)
{
    int c;

    while ((c = fileGetc ()) != EOF)
    {
        if (c == BACKSLASH && ! ignoreBackslash)
            fileGetc ();  /* throw away next character, too */
        else if (c == DOUBLE_QUOTE)
            break;
    }
    return STRING_SYMBOL;  /* symbolic representation of string */
}


/*****************************************************************************
  Skips to the end of the three (possibly four) 'c' sequence, returning a
  special character to symbolically represent a generic character.
  Also detects Vera numbers that include a base specifier (ie. 'b1010).
 *****************************************************************************/
int ParserEx::skipToEndOfChar (void)
{
    int c;
    int count = 0, veraBase = '\0';

    while ((c = fileGetc ()) != EOF)
    {
        ++count;
        if (c == BACKSLASH)
            fileGetc ();  /* throw away next character, too */
        else if (c == SINGLE_QUOTE)
            break;
        else if (c == NEWLINE)
        {
            fileUngetc (c);
            break;
        }
        else if (count == 1  &&  strchr ("DHOB", toupper (c)) != NULL)
            veraBase = c;
        else if (veraBase != '\0'  &&  ! isalnum (c))
        {
            fileUngetc (c);
            break;
        }
    }
    return CHAR_SYMBOL;  /* symbolic representation of character */
}


/*****************************************************************************
 This function returns the next character, stripping out comments,
 C pre-processor directives, and the contents of single and double
 quoted strings. In short, strip anything which places a burden upon
 the tokenizer.
 *****************************************************************************/
extern int ParserEx::cppGetc (void)
{
    boolean bDirective = FALSE;
    boolean ignore = FALSE;
    int c;

    if (ungetch != '\0')
    {
        c = ungetch;
        ungetch = ungetch2;
        ungetch2 = '\0';
        return c;  /* return here to avoid re-calling debugPutc () */
    }
    else do
    {
        c = fileGetc ();
        process:
        switch (c)
        {
        case EOF:
            ignore    = FALSE;
            bDirective = FALSE;
            break;

        case TAB:
        case SPACE:
            break;  /* ignore most white space */

        case NEWLINE:
            if (bDirective  &&  ! ignore)
                bDirective = FALSE;
            directive.accept = TRUE;
            break;

        case DOUBLE_QUOTE:
            directive.accept = FALSE;
            c = skipToEndOfString (FALSE);
            break;

        case '#':
            if (directive.accept)
            {
                bDirective = TRUE;
                directive.state  = DRCTV_HASH;
                directive.accept = FALSE;
            }
            break;

                        case SINGLE_QUOTE:
            directive.accept = FALSE;
            c = skipToEndOfChar ();
            break;

                        case '/':
            {
                const Comment comment = isComment ();

                if (comment == COMMENT_C)
                    c = skipOverCComment ();
                else if (comment == COMMENT_CPLUS)
                {
                    c = skipOverCplusComment ();
                    if (c == NEWLINE)
                        fileUngetc (c);
                }
                else
                    directive.accept = FALSE;
                break;
            }

                        case BACKSLASH:
            {
                int next = fileGetc ();

                if (next == NEWLINE)
                    continue;
                else if (next == '?')
                    cppUngetc (next);
                else
                    fileUngetc (next);
                break;
            }

                        case '?':
            {
                int next = fileGetc ();
                if (next != '?')
                    fileUngetc (next);
                else
                {
                    next = fileGetc ();
                    switch (next)
                    {
                    case '(':          c = '[';       break;
                    case ')':          c = ']';       break;
                    case '<':          c = '{';       break;
                    case '>':          c = '}';       break;
                    case '/':          c = BACKSLASH; goto process;
                    case '!':          c = '|';       break;
                    case SINGLE_QUOTE: c = '^';       break;
                    case '-':          c = '~';       break;
                    case '=':          c = '#';       goto process;
                    default:
                        fileUngetc (next);
                        cppUngetc ('?');
                        break;
                    }
                }
            } break;

                        default:
            if (c == '@' && hasAtLiteralStrings)
            {
                int next = fileGetc ();
                if (next == DOUBLE_QUOTE)
                {
                    directive.accept = FALSE;
                    c = skipToEndOfString (TRUE);
                    break;
                }
            }
            directive.accept = FALSE;
            if (bDirective)
                ignore = handleDirective (c);
            break;
        }
    } while (bDirective || ignore);

    DebugStatement ( debugPutc (DEBUG_CPP, c); )
    DebugStatement ( if (c == NEWLINE)
    debugPrintf (DEBUG_CPP, "%6ld: ", getInputLineNumber () + 1); )

    return c;
}

