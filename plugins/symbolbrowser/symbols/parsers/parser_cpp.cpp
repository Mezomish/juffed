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

#ifndef PARSER_CPP
#define PARSER_CPP

#include <setjmp.h>
#include "../common.h"
#include "vstring.h"
#include "parserex.h"
#include "keyword.h"
#include "../symbol.h"

#define langType Language

//#define DEBUG

/*
*   MACROS
*/

#define activeToken(st)     ((st)->token [(int) (st)->tokenIndex])
#define parentDecl(st)      ((st)->parent == NULL ? \
                            DECL_NONE : (st)->parent->declaration)
#define isType(token,t)     (boolean) ((token)->type == (t))
#define insideEnumBody(st)  ((st)->parent == NULL ? false : \
                            (boolean) ((st)->parent->declaration == DECL_ENUM))
#define isExternCDecl(st,c) (boolean) ((c) == STRING_SYMBOL  && \
                    ! (st)->haveQualifyingName  && (st)->scope == SCOPE_EXTERN)

#define isOneOf(c,s)        (boolean) (strchr ((s), (c)) != NULL)

#define isHighChar(c)       ((c) != EOF && (unsigned char)(c) >= 0xc0)

/*
*   DATA DECLARATIONS
*/

enum { NumTokens = 3 };

typedef enum eException {
        ExceptionNone, ExceptionEOF, ExceptionFormattingError,
        ExceptionBraceFormattingError
} exception_t;

/*  Used to specify type of keyword.
 */
typedef enum eKeywordId {
        KEYWORD_NONE = -1,
        KEYWORD_ATTRIBUTE, KEYWORD_ABSTRACT,
        KEYWORD_BOOLEAN, KEYWORD_BYTE, KEYWORD_BAD_STATE, KEYWORD_BAD_TRANS,
        KEYWORD_BIND, KEYWORD_BIND_VAR, KEYWORD_BIT,
        KEYWORD_CASE, KEYWORD_CATCH, KEYWORD_CHAR, KEYWORD_CLASS, KEYWORD_CONST,
        KEYWORD_CONSTRAINT, KEYWORD_COVERAGE_BLOCK, KEYWORD_COVERAGE_DEF,
        KEYWORD_DEFAULT, KEYWORD_DELEGATE, KEYWORD_DELETE, KEYWORD_DO,
        KEYWORD_DOUBLE,
        KEYWORD_ELSE, KEYWORD_ENUM, KEYWORD_EXPLICIT, KEYWORD_EXTERN,
        KEYWORD_EXTENDS, KEYWORD_EVENT,
        KEYWORD_FINAL, KEYWORD_FLOAT, KEYWORD_FOR, KEYWORD_FOREACH,
        KEYWORD_FRIEND, KEYWORD_FUNCTION,
        KEYWORD_GOTO,
        KEYWORD_IF, KEYWORD_IMPLEMENTS, KEYWORD_IMPORT, KEYWORD_INLINE, KEYWORD_INT,
        KEYWORD_INOUT, KEYWORD_INPUT, KEYWORD_INTEGER, KEYWORD_INTERFACE,
        KEYWORD_INTERNAL,
        KEYWORD_LOCAL, KEYWORD_LONG,
        KEYWORD_M_BAD_STATE, KEYWORD_M_BAD_TRANS, KEYWORD_M_STATE, KEYWORD_M_TRANS,
        KEYWORD_MUTABLE,
        KEYWORD_NAMESPACE, KEYWORD_NEW, KEYWORD_NEWCOV, KEYWORD_NATIVE,
        KEYWORD_OPERATOR, KEYWORD_OUTPUT, KEYWORD_OVERLOAD, KEYWORD_OVERRIDE,
        KEYWORD_PACKED, KEYWORD_PORT, KEYWORD_PACKAGE, KEYWORD_PRIVATE,
        KEYWORD_PROGRAM, KEYWORD_PROTECTED, KEYWORD_PUBLIC,
        KEYWORD_REGISTER, KEYWORD_RETURN,
        KEYWORD_SHADOW, KEYWORD_STATE,
        KEYWORD_SHORT, KEYWORD_SIGNED, KEYWORD_STATIC, KEYWORD_STRING,
        KEYWORD_STRUCT, KEYWORD_SWITCH, KEYWORD_SYNCHRONIZED,
        KEYWORD_TASK, KEYWORD_TEMPLATE, KEYWORD_THIS, KEYWORD_THROW,
        KEYWORD_THROWS, KEYWORD_TRANSIENT, KEYWORD_TRANS, KEYWORD_TRANSITION,
        KEYWORD_TRY, KEYWORD_TYPEDEF, KEYWORD_TYPENAME,
        KEYWORD_UINT, KEYWORD_ULONG, KEYWORD_UNION, KEYWORD_UNSIGNED, KEYWORD_USHORT,
        KEYWORD_USING,
        KEYWORD_VIRTUAL, KEYWORD_VOID, KEYWORD_VOLATILE,
        KEYWORD_WCHAR_T, KEYWORD_WHILE
} keywordId;

/*  Used to determine whether keyword is valid for the current language and
 *  what its ID is.
 */
typedef struct sKeywordDesc {
        const char *name;
        keywordId id;
        short isValid [5]; /* indicates languages for which kw is valid */
} keywordDesc;

/*  Used for reporting the type of object parsed by nextToken ().
 */
typedef enum eTokenType {
        TOKEN_NONE,          /* none */
        TOKEN_ARGS,          /* a parenthetical pair and its contents */
        TOKEN_BRACE_CLOSE,
        TOKEN_BRACE_OPEN,
        TOKEN_COLON,         /* the colon character */
        TOKEN_COMMA,         /* the comma character */
        TOKEN_DOUBLE_COLON,  /* double colon indicates nested-name-specifier */
        TOKEN_KEYWORD,
        TOKEN_NAME,          /* an unknown name */
        TOKEN_PACKAGE,       /* a Java package name */
        TOKEN_PAREN_NAME,    /* a single name in parentheses */
        TOKEN_SEMICOLON,     /* the semicolon character */
        TOKEN_SPEC,          /* a storage class specifier, qualifier, type, etc. */
        TOKEN_COUNT
} tokenType;

/*  This describes the scoping of the current statement.
 */
typedef enum eTagScope {
        SCOPE_GLOBAL,        /* no storage class specified */
        SCOPE_STATIC,        /* static storage class */
        SCOPE_EXTERN,        /* external storage class */
        SCOPE_FRIEND,        /* declares access only */
        SCOPE_TYPEDEF,       /* scoping depends upon context */
        SCOPE_COUNT
} tagScope;

typedef enum eDeclaration {
        DECL_NONE,
        DECL_BASE,           /* base type (default) */
        DECL_CLASS,
        DECL_ENUM,
        DECL_EVENT,
        DECL_FUNCTION,
        DECL_IGNORE,         /* non-taggable "declaration" */
        DECL_INTERFACE,
        DECL_NAMESPACE,
        DECL_NOMANGLE,       /* C++ name demangling block */
        DECL_PACKAGE,
        DECL_PROGRAM,        /* Vera program */
        DECL_STRUCT,
        DECL_TASK,           /* Vera task */
        DECL_UNION,
        DECL_COUNT
} declType;

typedef enum eVisibilityType {
        ACCESS_UNDEFINED,
        ACCESS_LOCAL,
        ACCESS_PRIVATE,
        ACCESS_PROTECTED,
        ACCESS_PUBLIC,
        ACCESS_DEFAULT,      /* Java-specific */
        ACCESS_COUNT
} accessType;

/*  Information about the parent class of a member (if any).
 */
typedef struct sMemberInfo {
        accessType access;           /* access of current statement */
        accessType accessDefault;    /* access default for current statement */
} memberInfo;

typedef struct sTokenInfo {
        tokenType     type;
        keywordId     keyword;
        vString*      name;          /* the name of the token */
        unsigned long lineNumber;    /* line number of tag */
//        fpos_t        filePosition;  /* file position of line containing name */
} tokenInfo;

typedef enum eImplementation {
        IMP_DEFAULT,
        IMP_ABSTRACT,
        IMP_VIRTUAL,
        IMP_PURE_VIRTUAL,
        IMP_COUNT
} impType;

/*  Describes the statement currently undergoing analysis.
 */
typedef struct sStatementInfo {
        tagScope	scope;
        declType	declaration;    /* specifier associated with TOKEN_SPEC */
        boolean		gotName;        /* was a name parsed yet? */
        boolean		haveQualifyingName;  /* do we have a name we are considering? */
        boolean		gotParenName;   /* was a name inside parentheses parsed yet? */
        boolean		gotArgs;        /* was a list of parameters parsed yet? */
        boolean		isPointer;      /* is 'name' a pointer? */
        boolean         inFunction;     /* are we inside of a function? */
        boolean		assignment;     /* have we handled an '='? */
        boolean		notVariable;    /* has a variable declaration been disqualified ? */
        impType		implementation; /* abstract or concrete implementation? */
        unsigned int    tokenIndex;     /* currently active token */
        tokenInfo*	token [(int) NumTokens];
        tokenInfo*	context;        /* accumulated scope of current statement */
        tokenInfo*	blockName;      /* name of current block */
        memberInfo	member;         /* information regarding parent class/struct */
        vString*	parentClasses;  /* parent classes */
        struct sStatementInfo *parent;  /* statement we are nested within */
} statementInfo;

/*  Describes the type of tag being generated.
 */
typedef enum eTagType {
        TAG_UNDEFINED,
        TAG_CLASS,       /* class name */
        TAG_ENUM,        /* enumeration name */
        TAG_ENUMERATOR,  /* enumerator (enumeration value) */
        TAG_EVENT,       /* event */
        TAG_FIELD,       /* field (Java) */
        TAG_FUNCTION,    /* function definition */
        TAG_INTERFACE,   /* interface declaration */
        TAG_LOCAL,       /* local variable definition */
        TAG_MEMBER,      /* structure, class or interface member */
        TAG_METHOD,      /* method declaration */
        TAG_NAMESPACE,   /* namespace name */
        TAG_PACKAGE,     /* package name */
        TAG_PROGRAM,     /* program name */
        TAG_PROPERTY,    /* property name */
        TAG_PROTOTYPE,   /* function prototype or declaration */
        TAG_STRUCT,      /* structure name */
        TAG_TASK,        /* task name */
        TAG_TYPEDEF,     /* typedef name */
        TAG_UNION,       /* union name */
        TAG_VARIABLE,    /* variable definition */
        TAG_EXTERN_VAR,  /* external variable declaration */
        TAG_COUNT        /* must be last */
} tagType;

typedef struct sParenInfo {
        boolean isPointer;
        boolean isParamList;
        boolean isKnrParamList;
        boolean isNameCandidate;
        boolean invalidContents;
        boolean nestedArgs;
        unsigned int parameterCount;
} parenInfo;


/*
*   DATA DEFINITIONS
*/

/* Used to index into the CKinds table. */
typedef enum {
        CK_UNDEFINED = -1,
        CK_CLASS, CK_DEFINE, CK_ENUMERATOR, CK_FUNCTION,
        CK_ENUMERATION, CK_LOCAL, CK_MEMBER, CK_NAMESPACE, CK_PROTOTYPE,
        CK_STRUCT, CK_TYPEDEF, CK_UNION, CK_VARIABLE,
        CK_EXTERN_VARIABLE
} cKind;

static kindOption CKinds [] = {
        { true,  'c', "class",      "classes"},
        { true,  'd', "macro",      "macro definitions"},
        { true,  'e', "enumerator", "enumerators (values inside an enumeration)"},
        { true,  'f', "function",   "function definitions"},
        { true,  'g', "enum",       "enumeration names"},
        { false, 'l', "local",      "local variables"},
        { true,  'm', "member",     "class, struct, and union members"},
        { true,  'n', "namespace",  "namespaces"},
        { false, 'p', "prototype",  "function prototypes"},
        { true,  's', "struct",     "structure names"},
        { true,  't', "typedef",    "typedefs"},
        { true,  'u', "union",      "union names"},
        { true,  'v', "variable",   "variable definitions"},
        { false, 'x', "externvar",  "external and forward variable declarations"},
};

typedef enum {
        CSK_UNDEFINED = -1,
        CSK_CLASS, CSK_DEFINE, CSK_ENUMERATOR, CSK_EVENT, CSK_FIELD,
        CSK_ENUMERATION, CSK_INTERFACE, CSK_LOCAL, CSK_METHOD,
        CSK_NAMESPACE, CSK_PROPERTY, CSK_STRUCT, CSK_TYPEDEF
} csharpKind;

static kindOption CsharpKinds [] = {
        { true,  'c', "class",      "classes"},
        { true,  'd', "macro",      "macro definitions"},
        { true,  'e', "enumerator", "enumerators (values inside an enumeration)"},
        { true,  'E', "event",      "events"},
        { true,  'f', "field",      "fields"},
        { true,  'g', "enum",       "enumeration names"},
        { true,  'i', "interface",  "interfaces"},
        { false, 'l', "local",      "local variables"},
        { true,  'm', "method",     "methods"},
        { true,  'n', "namespace",  "namespaces"},
        { true,  'p', "property",   "properties"},
        { true,  's', "struct",     "structure names"},
        { true,  't', "typedef",    "typedefs"},
};

/* Used to index into the JavaKinds table. */
typedef enum {
        JK_UNDEFINED = -1,
        JK_CLASS, JK_ENUM_CONSTANT, JK_FIELD, JK_ENUM, JK_INTERFACE,
        JK_LOCAL, JK_METHOD, JK_PACKAGE, JK_ACCESS, JK_CLASS_PREFIX
} javaKind;

static kindOption JavaKinds [] = {
        { true,  'c', "class",         "classes"},
        { true,  'e', "enum constant", "enum constants"},
        { true,  'f', "field",         "fields"},
        { true,  'g', "enum",          "enum types"},
        { true,  'i', "interface",     "interfaces"},
        { false, 'l', "local",         "local variables"},
        { true,  'm', "method",        "methods"},
        { true,  'p', "package",       "packages"},
};

/* Used to index into the VeraKinds table. */
typedef enum {
        VK_UNDEFINED = -1,
        VK_CLASS, VK_DEFINE, VK_ENUMERATOR, VK_FUNCTION,
        VK_ENUMERATION, VK_LOCAL, VK_MEMBER, VK_PROGRAM, VK_PROTOTYPE,
        VK_TASK, VK_TYPEDEF, VK_VARIABLE,
        VK_EXTERN_VARIABLE
} veraKind;

static kindOption VeraKinds [] = {
        { true,  'c', "class",      "classes"},
        { true,  'd', "macro",      "macro definitions"},
        { true,  'e', "enumerator", "enumerators (values inside an enumeration)"},
        { true,  'f', "function",   "function definitions"},
        { true,  'g', "enum",       "enumeration names"},
        { false, 'l', "local",      "local variables"},
        { true,  'm', "member",     "class, struct, and union members"},
        { true,  'p', "program",    "programs"},
        { false, 'P', "prototype",  "function prototypes"},
        { true,  't', "task",       "tasks"},
        { true,  'T', "typedef",    "typedefs"},
        { true,  'v', "variable",   "variable definitions"},
        { false, 'x', "externvar",  "external variable declarations"}
};

static const keywordDesc KeywordTable [] = {
        /*                                              C++            */
        /*                                       ANSI C  |  C# Java    */
        /*                                            |  |  |  |  Vera */
        /* keyword          keyword ID                |  |  |  |  |    */
        { "__attribute__",  KEYWORD_ATTRIBUTE,      { 1, 1, 1, 0, 0 } },
        { "abstract",       KEYWORD_ABSTRACT,       { 0, 0, 1, 1, 0 } },
        { "bad_state",      KEYWORD_BAD_STATE,      { 0, 0, 0, 0, 1 } },
        { "bad_trans",      KEYWORD_BAD_TRANS,      { 0, 0, 0, 0, 1 } },
        { "bind",           KEYWORD_BIND,           { 0, 0, 0, 0, 1 } },
        { "bind_var",       KEYWORD_BIND_VAR,       { 0, 0, 0, 0, 1 } },
        { "bit",            KEYWORD_BIT,            { 0, 0, 0, 0, 1 } },
        { "boolean",        KEYWORD_BOOLEAN,        { 0, 0, 0, 1, 0 } },
        { "byte",           KEYWORD_BYTE,           { 0, 0, 0, 1, 0 } },
        { "case",           KEYWORD_CASE,           { 1, 1, 1, 1, 0 } },
        { "catch",          KEYWORD_CATCH,          { 0, 1, 1, 0, 0 } },
        { "char",           KEYWORD_CHAR,           { 1, 1, 1, 1, 0 } },
        { "class",          KEYWORD_CLASS,          { 0, 1, 1, 1, 1 } },
        { "const",          KEYWORD_CONST,          { 1, 1, 1, 1, 0 } },
        { "constraint",     KEYWORD_CONSTRAINT,     { 0, 0, 0, 0, 1 } },
        { "coverage_block", KEYWORD_COVERAGE_BLOCK, { 0, 0, 0, 0, 1 } },
        { "coverage_def",   KEYWORD_COVERAGE_DEF,   { 0, 0, 0, 0, 1 } },
        { "do",             KEYWORD_DO,             { 1, 1, 1, 1, 0 } },
        { "default",        KEYWORD_DEFAULT,        { 1, 1, 1, 1, 0 } },
        { "delegate",       KEYWORD_DELEGATE,       { 0, 0, 1, 0, 0 } },
        { "delete",         KEYWORD_DELETE,         { 0, 1, 0, 0, 0 } },
        { "double",         KEYWORD_DOUBLE,         { 1, 1, 1, 1, 0 } },
        { "else",           KEYWORD_ELSE,           { 1, 1, 1, 1, 0 } },
        { "enum",           KEYWORD_ENUM,           { 1, 1, 1, 1, 1 } },
        { "event",          KEYWORD_EVENT,          { 0, 0, 1, 0, 1 } },
        { "explicit",       KEYWORD_EXPLICIT,       { 0, 1, 1, 0, 0 } },
        { "extends",        KEYWORD_EXTENDS,        { 0, 0, 0, 1, 1 } },
        { "extern",         KEYWORD_EXTERN,         { 1, 1, 1, 0, 1 } },
        { "final",          KEYWORD_FINAL,          { 0, 0, 0, 1, 0 } },
        { "float",          KEYWORD_FLOAT,          { 1, 1, 1, 1, 0 } },
        { "for",            KEYWORD_FOR,            { 1, 1, 1, 1, 0 } },
        { "foreach",        KEYWORD_FOREACH,        { 0, 0, 1, 0, 0 } },
        { "friend",         KEYWORD_FRIEND,         { 0, 1, 0, 0, 0 } },
        { "function",       KEYWORD_FUNCTION,       { 0, 0, 0, 0, 1 } },
        { "goto",           KEYWORD_GOTO,           { 1, 1, 1, 1, 0 } },
        { "if",             KEYWORD_IF,             { 1, 1, 1, 1, 0 } },
        { "implements",     KEYWORD_IMPLEMENTS,     { 0, 0, 0, 1, 0 } },
        { "import",         KEYWORD_IMPORT,         { 0, 0, 0, 1, 0 } },
        { "inline",         KEYWORD_INLINE,         { 0, 1, 0, 0, 0 } },
        { "inout",          KEYWORD_INOUT,          { 0, 0, 0, 0, 1 } },
        { "input",          KEYWORD_INPUT,          { 0, 0, 0, 0, 1 } },
        { "int",            KEYWORD_INT,            { 1, 1, 1, 1, 0 } },
        { "integer",        KEYWORD_INTEGER,        { 0, 0, 0, 0, 1 } },
        { "interface",      KEYWORD_INTERFACE,      { 0, 0, 1, 1, 1 } },
        { "internal",       KEYWORD_INTERNAL,       { 0, 0, 1, 0, 0 } },
        { "local",          KEYWORD_LOCAL,          { 0, 0, 0, 0, 1 } },
        { "long",           KEYWORD_LONG,           { 1, 1, 1, 1, 0 } },
        { "m_bad_state",    KEYWORD_M_BAD_STATE,    { 0, 0, 0, 0, 1 } },
        { "m_bad_trans",    KEYWORD_M_BAD_TRANS,    { 0, 0, 0, 0, 1 } },
        { "m_state",        KEYWORD_M_STATE,        { 0, 0, 0, 0, 1 } },
        { "m_trans",        KEYWORD_M_TRANS,        { 0, 0, 0, 0, 1 } },
        { "mutable",        KEYWORD_MUTABLE,        { 0, 1, 0, 0, 0 } },
        { "namespace",      KEYWORD_NAMESPACE,      { 0, 1, 1, 0, 0 } },
        { "native",         KEYWORD_NATIVE,         { 0, 0, 0, 1, 0 } },
        { "new",            KEYWORD_NEW,            { 0, 1, 1, 1, 0 } },
        { "newcov",         KEYWORD_NEWCOV,         { 0, 0, 0, 0, 1 } },
        { "operator",       KEYWORD_OPERATOR,       { 0, 1, 1, 0, 0 } },
        { "output",         KEYWORD_OUTPUT,         { 0, 0, 0, 0, 1 } },
        { "overload",       KEYWORD_OVERLOAD,       { 0, 1, 0, 0, 0 } },
        { "override",       KEYWORD_OVERRIDE,       { 0, 0, 1, 0, 0 } },
        { "package",        KEYWORD_PACKAGE,        { 0, 0, 0, 1, 0 } },
        { "packed",         KEYWORD_PACKED,         { 0, 0, 0, 0, 1 } },
        { "port",           KEYWORD_PORT,           { 0, 0, 0, 0, 1 } },
        { "private",        KEYWORD_PRIVATE,        { 0, 1, 1, 1, 0 } },
        { "program",        KEYWORD_PROGRAM,        { 0, 0, 0, 0, 1 } },
        { "protected",      KEYWORD_PROTECTED,      { 0, 1, 1, 1, 1 } },
        { "public",         KEYWORD_PUBLIC,         { 0, 1, 1, 1, 1 } },
        { "register",       KEYWORD_REGISTER,       { 1, 1, 0, 0, 0 } },
        { "return",         KEYWORD_RETURN,         { 1, 1, 1, 1, 0 } },
        { "shadow",         KEYWORD_SHADOW,         { 0, 0, 0, 0, 1 } },
        { "short",          KEYWORD_SHORT,          { 1, 1, 1, 1, 0 } },
        { "signed",         KEYWORD_SIGNED,         { 1, 1, 0, 0, 0 } },
        { "state",          KEYWORD_STATE,          { 0, 0, 0, 0, 1 } },
        { "static",         KEYWORD_STATIC,         { 1, 1, 1, 1, 1 } },
        { "string",         KEYWORD_STRING,         { 0, 0, 1, 0, 1 } },
        { "struct",         KEYWORD_STRUCT,         { 1, 1, 1, 0, 0 } },
        { "switch",         KEYWORD_SWITCH,         { 1, 1, 1, 1, 0 } },
        { "synchronized",   KEYWORD_SYNCHRONIZED,   { 0, 0, 0, 1, 0 } },
        { "task",           KEYWORD_TASK,           { 0, 0, 0, 0, 1 } },
        { "template",       KEYWORD_TEMPLATE,       { 0, 1, 0, 0, 0 } },
        { "this",           KEYWORD_THIS,           { 0, 1, 1, 1, 0 } },
        { "throw",          KEYWORD_THROW,          { 0, 1, 1, 1, 0 } },
        { "throws",         KEYWORD_THROWS,         { 0, 0, 0, 1, 0 } },
        { "trans",          KEYWORD_TRANS,          { 0, 0, 0, 0, 1 } },
        { "transition",     KEYWORD_TRANSITION,     { 0, 0, 0, 0, 1 } },
        { "transient",      KEYWORD_TRANSIENT,      { 0, 0, 0, 1, 0 } },
        { "try",            KEYWORD_TRY,            { 0, 1, 1, 0, 0 } },
        { "typedef",        KEYWORD_TYPEDEF,        { 1, 1, 1, 0, 1 } },
        { "typename",       KEYWORD_TYPENAME,       { 0, 1, 0, 0, 0 } },
        { "uint",           KEYWORD_UINT,           { 0, 0, 1, 0, 0 } },
        { "ulong",          KEYWORD_ULONG,          { 0, 0, 1, 0, 0 } },
        { "union",          KEYWORD_UNION,          { 1, 1, 0, 0, 0 } },
        { "unsigned",       KEYWORD_UNSIGNED,       { 1, 1, 1, 0, 0 } },
        { "ushort",         KEYWORD_USHORT,         { 0, 0, 1, 0, 0 } },
        { "using",          KEYWORD_USING,          { 0, 1, 1, 0, 0 } },
        { "virtual",        KEYWORD_VIRTUAL,        { 0, 1, 1, 0, 1 } },
        { "void",           KEYWORD_VOID,           { 1, 1, 1, 1, 1 } },
        { "volatile",       KEYWORD_VOLATILE,       { 1, 1, 1, 1, 0 } },
        { "wchar_t",        KEYWORD_WCHAR_T,        { 1, 1, 1, 0, 0 } },
        { "while",          KEYWORD_WHILE,          { 1, 1, 1, 1, 0 } }
};



class Parser_Cpp: public ParserEx {

public:
    Parser_Cpp();

protected:
    void parse();

private:
    bool isLanguage(langType value) { return value == language(); };
    //bool isIgnoreToken ( const char *const name, bool *const pIgnoreParens, const char **const replacement);
    Keywords keywords;

    jmp_buf Exception;

    langType Lang_c;
    langType Lang_cpp;
    langType Lang_csharp;
    langType Lang_java;
    langType Lang_vera;
    vString *Signature;
    boolean CollectingSignature;
    /* Number used to uniquely identify anonymous structs and unions. */
    int AnonymousID;
    statementInfo *CurrentStatement;

    void initToken (tokenInfo* const token);
    void advanceToken (statementInfo* const st);
    tokenInfo *prevToken (const statementInfo *const st, unsigned int n);
    void setToken (statementInfo *const st, const tokenType type);
    void retardToken (statementInfo *const st);
    tokenInfo *newToken (void);
    void deleteToken (tokenInfo *const token);
    const char *accessString (const accessType access);
    const char *implementationString (const impType imp);
    #ifdef DEBUG
    const char *tokenString (const tokenType type);
    const char *scopeString (const tagScope scope);
    const char *declString (const declType declaration);
    const char *keywordString (const keywordId keyword);
    void printTockenInfo(tokenInfo *const token);
    void  printStatementInfo(const statementInfo *const st);
    #endif
    boolean isContextualKeyword (const tokenInfo *const token);
    boolean isContextualStatement (const statementInfo *const st);
    boolean isMember (const statementInfo *const st);
    void initMemberInfo (statementInfo *const st);
    void reinitStatement (statementInfo *const st, const boolean partial);
    void initStatement (statementInfo *const st, statementInfo *const parent);
    cKind cTagKind (const tagType type);
    csharpKind csharpTagKind (const tagType type);
    javaKind javaTagKind (const tagType type);
    veraKind veraTagKind (const tagType type);
    const char *tagName (const tagType type);
    int tagLetter (const tagType type);
    boolean includeTag (const tagType type, const boolean isFileScope);
    tagType declToTagType (const declType declaration);
    const char* accessField (const statementInfo *const st);
    void addContextSeparator (vString *const scope);
//    void addOtherFields (tagEntryInfo* const tag, const tagType type,
//                                                            const statementInfo *const st,
//                                                            vString *const scope, vString *const typeRef);
    void findScopeHierarchy (vString *const string,
                                                                    const statementInfo *const st);
//    void makeExtraTagEntry (const tagType type, tagEntryInfo *const e,
//                                                               vString *const scope);

    Symbol::SymbolType declToSymbolType (const declType declaration);
    Symbol *getParent(const statementInfo *const st);
    void makeTag (const tokenInfo *const token,
                                             const statementInfo *const st,
                                             boolean isFileScope, const tagType type);
    boolean isValidTypeSpecifier (const declType declaration);
    void qualifyEnumeratorTag (const statementInfo *const st, const tokenInfo *const nameToken);
    void qualifyFunctionTag (const statementInfo *const st, const tokenInfo *const nameToken);
    void qualifyFunctionDeclTag (const statementInfo *const st, const tokenInfo *const nameToken);
    void qualifyCompoundTag (const statementInfo *const st, const tokenInfo *const nameToken);
    void qualifyBlockTag (statementInfo *const st, const tokenInfo *const nameToken);
    void qualifyVariableTag (const statementInfo *const st,
                                                                    const tokenInfo *const nameToken);
    int skipToOneOf (const char *const chars);
    int skipToNonWhite (void);
    void skipToFormattedBraceMatch (void);
    void skipToMatch (const char *const pair);
    void skipParens (void);
    void skipBraces (void);
    keywordId analyzeKeyword (const char *const name);
    void analyzeIdentifier (tokenInfo *const token);
    void readIdentifier (tokenInfo *const token, const int firstChar);
    void readPackageName (tokenInfo *const token, const int firstChar);
    void readPackageOrNamespace (statementInfo *const st, const declType declaration);
    void processName (statementInfo *const st);
    void readOperator (statementInfo *const st);
    void copyToken (tokenInfo *const dest, const tokenInfo *const src);
    void setAccess (statementInfo *const st, const accessType access);
    void discardTypeList (tokenInfo *const token);
    void addParentClass (statementInfo *const st, tokenInfo *const token);
    void readParents (statementInfo *const st, const int qualifier);
    void skipStatement (statementInfo *const st);
    void processInterface (statementInfo *const st);
    void processToken (tokenInfo *const token, statementInfo *const st);
    void restartStatement (statementInfo *const st);
    void skipMemIntializerList (tokenInfo *const token);
    void skipMacro (statementInfo *const st);
    boolean skipPostArgumentStuff(statementInfo *const st, parenInfo *const info);
    void skipJavaThrows (statementInfo *const st);
    void analyzePostParens (statementInfo *const st, parenInfo *const info);
    boolean languageSupportsGenerics (void);
    void processAngleBracket (void);
    void parseJavaAnnotation (statementInfo *const st);
    int parseParens (statementInfo *const st, parenInfo *const info);
    void initParenInfo (parenInfo *const info);
    void analyzeParens (statementInfo *const st);
    void addContext (statementInfo *const st, const tokenInfo* const token);
    boolean inheritingDeclaration (declType decl);
    void processColon (statementInfo *const st);
    int skipInitializer (statementInfo *const st);
    void processInitializer (statementInfo *const st);
    void parseIdentifier (statementInfo *const st, const int c);
    void parseGeneralToken (statementInfo *const st, const int c);
    void nextToken (statementInfo *const st);
    statementInfo *newStatement (statementInfo *const parent);
    void deleteStatement (void);
    void deleteAllStatements (void);
    boolean isStatementEnd (const statementInfo *const st);
    void checkStatementEnd (statementInfo *const st);
    void nest (statementInfo *const st, const unsigned int nestLevel);
    void tagCheck (statementInfo *const st);
    void createTags (const unsigned int nestLevel, statementInfo *const parent);
    void buildKeywordHash (const langType language, unsigned int idx);
    void initializeCParser (const langType language);
    void initializeCppParser (const langType language);
    void initializeCsharpParser (const langType language);
    void initializeJavaParser (const langType language);
    void initializeVeraParser (const langType language);

    bool findCTags (const unsigned int passCount);
    QString tagTypeToStr(const tagType type);
};



/*****************************************************************************
 *****************************************************************************/
Parser_Cpp::Parser_Cpp(/*Parser_Cpp * parent*/) {
    AnonymousID = 0;
    CurrentStatement = NULL;
    CollectingSignature = false;
    Lang_c = Lang_cpp = Lang_csharp = Lang_java = Lang_vera = LanguageUnknown;
    Signature = NULL;
}

/*
*   Token management
*/

void Parser_Cpp::initToken (tokenInfo* const token)
{
    token->type         = TOKEN_NONE;
    token->keyword	= KEYWORD_NONE;
    token->lineNumber	= getSourceLineNumber();
//    token->filePosition	= q->getInputFilePosition();
    vStringClear (token->name);
}

void Parser_Cpp::advanceToken (statementInfo* const st)
{
    if (st->tokenIndex >= (unsigned int) NumTokens - 1)
        st->tokenIndex = 0;
    else
        ++st->tokenIndex;
    initToken (st->token [st->tokenIndex]);
}

tokenInfo *Parser_Cpp::prevToken (const statementInfo *const st, unsigned int n)
{
    unsigned int tokenIndex;
    unsigned int num = (unsigned int) NumTokens;
    Assert (n < num);
    tokenIndex = (st->tokenIndex + num - n) % num;
    return st->token [tokenIndex];
}

void Parser_Cpp::setToken (statementInfo *const st, const tokenType type)
{
    tokenInfo *token;
    token = activeToken (st);
    initToken (token);
    token->type = type;
}

void Parser_Cpp::retardToken (statementInfo *const st)
{
    if (st->tokenIndex == 0)
        st->tokenIndex = (unsigned int) NumTokens - 1;
    else
        --st->tokenIndex;
    setToken (st, TOKEN_NONE);
}

tokenInfo *Parser_Cpp::newToken (void)
{
    tokenInfo *const token = xMalloc (1, tokenInfo);
    token->name = vStringNew ();
    initToken (token);
    return token;
}

void Parser_Cpp::deleteToken (tokenInfo *const token)
{
    if (token != NULL)
    {
        vStringDelete (token->name);
        eFree (token);
    }
}

const char *Parser_Cpp::accessString (const accessType access)
{
    const char *const names [] = {
        "?", "local", "private", "protected", "public", "default"
            };
    Assert (sizeof (names) / sizeof (names [0]) == ACCESS_COUNT);
    Assert ((int) access < ACCESS_COUNT);
    return names [(int) access];
}

const char *Parser_Cpp::implementationString (const impType imp)
{
    static const char *const names [] ={
        "?", "abstract", "virtual", "pure virtual"
            };
    Assert (sizeof (names) / sizeof (names [0]) == IMP_COUNT);
    Assert ((int) imp < IMP_COUNT);
    return names [(int) imp];
}

/*
*   Debugging functions
*/

#ifdef DEBUG

#define boolString(c)   ((c) ? "TRUE" : "FALSE")

const char *Parser_Cpp::tokenString (const tokenType type)
{
    static const char *const names [] = {
        "none", "args", "}", "{", "colon", "comma", "double colon", "keyword",
        "name", "package", "paren-name", "semicolon", "specifier"
    };
    Assert (sizeof (names) / sizeof (names [0]) == TOKEN_COUNT);
    Assert ((int) type < TOKEN_COUNT);
    return names [(int) type];
}

const char *Parser_Cpp::scopeString (const tagScope scope)
{
    static const char *const names [] = {
        "global", "static", "extern", "friend", "typedef"
            };
    Assert (sizeof (names) / sizeof (names [0]) == SCOPE_COUNT);
    Assert ((int) scope < SCOPE_COUNT);
    return names [(int) scope];
}

const char *Parser_Cpp::declString (const declType declaration)
{
    static const char *const names [] = {
        "?", "base", "class", "enum", "event", "function", "ignore",
        "interface", "namespace", "no mangle", "package", "program",
        "struct", "task", "union",
    };
    Assert (sizeof (names) / sizeof (names [0]) == DECL_COUNT);
    Assert ((int) declaration < DECL_COUNT);
    return names [(int) declaration];
}

const char *Parser_Cpp::keywordString (const keywordId keyword)
{
    const size_t count = sizeof (KeywordTable) / sizeof (KeywordTable [0]);
    const char *name = "none";
    size_t i;
    for (i = 0  ;  i < count  ;  ++i)
    {
        const keywordDesc *p = &KeywordTable [i];
        if (p->id == keyword)
        {
            name = p->name;
            break;
        }
    }
    return name;
}

void Parser_Cpp::printTockenInfo(tokenInfo *const token)
{
    if (isType (token, TOKEN_NAME))
        fprintf(stderr, "type: %-12s: %-13s   line: %lu\n",
                tokenString (token->type), vStringValue (token->name),
                token->lineNumber);

    else if (isType (token, TOKEN_KEYWORD))
        fprintf(stderr, "type: %-12s: %-13s   line: %lu\n",
                tokenString (token->type), keywordString (token->keyword),
                token->lineNumber);

    else
        fprintf(stderr, "type: %-12s                  line: %lu\n",
                tokenString (token->type), token->lineNumber);
}

void Parser_Cpp::printStatementInfo (const statementInfo *const st)
{
    unsigned int i;
    fprintf(stderr, "scope: %s   decl: %s   gotName: %s   gotParenName: %s\n",
            scopeString (st->scope), declString (st->declaration),
            boolString (st->gotName), boolString (st->gotParenName));
    fprintf(stderr, "haveQualifyingName: %s\n", boolString (st->haveQualifyingName));
    fprintf(stderr, "access: %s   default: %s\n", accessString (st->member.access),
            accessString (st->member.accessDefault));
    fprintf (stderr, "token  : ");
    printTockenInfo(activeToken (st));
    for (i = 1  ;  i < (unsigned int) NumTokens  ;  ++i)
    {
        fprintf(stderr, "prev %u : ", i);
        printTockenInfo(prevToken (st, i));
    }
    fprintf(stderr, "context: ");
    printTockenInfo(st->context);
    fprintf(stderr, "------------------\n");
    fprintf(stderr, "parent set=%s\n", boolString (st->parent != NULL));
}

#endif

/*
*   Statement management
*/

boolean Parser_Cpp::isContextualKeyword (const tokenInfo *const token)
{
    boolean result;
    switch (token->keyword)
    {
        case KEYWORD_CLASS:
        case KEYWORD_ENUM:
        case KEYWORD_INTERFACE:
        case KEYWORD_NAMESPACE:
        case KEYWORD_STRUCT:
        case KEYWORD_UNION:
            result = true;
            break;

        default: result = false; break;
    }
    return result;
}

boolean Parser_Cpp::isContextualStatement (const statementInfo *const st)
{
    boolean result = false;
    if (st != NULL) switch (st->declaration)
    {
        case DECL_CLASS:
        case DECL_ENUM:
        case DECL_INTERFACE:
        case DECL_NAMESPACE:
        case DECL_STRUCT:
        case DECL_UNION:
            result = true;
            break;

        default: result = false; break;
    }
    return result;
}

boolean Parser_Cpp::isMember (const statementInfo *const st)
{
    boolean result;
    if (isType (st->context, TOKEN_NAME))
        result = true;
    else
        result = (boolean)
            (st->parent != NULL && isContextualStatement (st->parent));
    return result;
}

void Parser_Cpp::initMemberInfo (statementInfo *const st)
{
    accessType accessDefault = ACCESS_UNDEFINED;

    if (st->parent != NULL) switch (st->parent->declaration)
    {
        case DECL_ENUM:
            accessDefault = (isLanguage (Lang_java) ? ACCESS_PUBLIC : ACCESS_UNDEFINED);
            break;
        case DECL_NAMESPACE:
            accessDefault = ACCESS_UNDEFINED;
            break;

        case DECL_CLASS:
            if (isLanguage (Lang_java))
                accessDefault = ACCESS_DEFAULT;
            else
                accessDefault = ACCESS_PRIVATE;
            break;

        case DECL_INTERFACE:
        case DECL_STRUCT:
        case DECL_UNION:
            accessDefault = ACCESS_PUBLIC;
            break;

        default: break;
    }
    st->member.accessDefault = accessDefault;
    st->member.access		 = accessDefault;
}

void Parser_Cpp::reinitStatement (statementInfo *const st, const boolean partial)
{
    unsigned int i;

    if (! partial)
    {
        st->scope = SCOPE_GLOBAL;
        if (isContextualStatement (st->parent))
            st->declaration = DECL_BASE;
        else
            st->declaration = DECL_NONE;
    }
    st->gotParenName	= false;
    st->isPointer	= false;
    st->inFunction	= false;
    st->assignment	= false;
    st->notVariable	= false;
    st->implementation	= IMP_DEFAULT;
    st->gotArgs		= false;
    st->gotName		= false;
    st->haveQualifyingName = false;
    st->tokenIndex	= 0;

    if (st->parent != NULL)
        st->inFunction = st->parent->inFunction;

    for (i = 0  ;  i < (unsigned int) NumTokens  ;  ++i)
        initToken (st->token [i]);

    initToken (st->context);

    /*	Keep the block name, so that a variable following after a comma will
     *	still have the structure name.
     */
    if (! partial)
        initToken (st->blockName);

    vStringClear (st->parentClasses);

    /*  Init member info.
     */
    if (! partial)
        st->member.access = st->member.accessDefault;
}

void Parser_Cpp::initStatement (statementInfo *const st, statementInfo *const parent)
{
    st->parent = parent;
    initMemberInfo (st);
    reinitStatement (st, false);
}

/*
*   Tag generation functions
*/
cKind Parser_Cpp::cTagKind (const tagType type)
{
    cKind result = CK_UNDEFINED;
    switch (type)
    {
        case TAG_CLASS:      result = CK_CLASS;       break;
        case TAG_ENUM:       result = CK_ENUMERATION; break;
        case TAG_ENUMERATOR: result = CK_ENUMERATOR;  break;
        case TAG_FUNCTION:   result = CK_FUNCTION;    break;
        case TAG_LOCAL:      result = CK_LOCAL;       break;
        case TAG_MEMBER:     result = CK_MEMBER;      break;
        case TAG_NAMESPACE:  result = CK_NAMESPACE;   break;
        case TAG_PROTOTYPE:  result = CK_PROTOTYPE;   break;
        case TAG_STRUCT:     result = CK_STRUCT;      break;
        case TAG_TYPEDEF:    result = CK_TYPEDEF;     break;
        case TAG_UNION:      result = CK_UNION;       break;
        case TAG_VARIABLE:   result = CK_VARIABLE;    break;
        case TAG_EXTERN_VAR: result = CK_EXTERN_VARIABLE; break;

        default: Assert ("Bad C tag type" == NULL); break;
    }
    return result;
}

csharpKind Parser_Cpp::csharpTagKind (const tagType type)
{
    csharpKind result = CSK_UNDEFINED;
    switch (type)
    {
        case TAG_CLASS:      result = CSK_CLASS;           break;
        case TAG_ENUM:       result = CSK_ENUMERATION;     break;
        case TAG_ENUMERATOR: result = CSK_ENUMERATOR;      break;
        case TAG_EVENT:      result = CSK_EVENT;           break;
        case TAG_FIELD:      result = CSK_FIELD ;          break;
        case TAG_INTERFACE:  result = CSK_INTERFACE;       break;
        case TAG_LOCAL:      result = CSK_LOCAL;           break;
        case TAG_METHOD:     result = CSK_METHOD;          break;
        case TAG_NAMESPACE:  result = CSK_NAMESPACE;       break;
        case TAG_PROPERTY:   result = CSK_PROPERTY;        break;
        case TAG_STRUCT:     result = CSK_STRUCT;          break;
        case TAG_TYPEDEF:    result = CSK_TYPEDEF;         break;

        default: Assert ("Bad C# tag type" == NULL); break;
    }
    return result;
}

javaKind Parser_Cpp::javaTagKind (const tagType type)
{
    javaKind result = JK_UNDEFINED;
    switch (type)
    {
        case TAG_CLASS:      result = JK_CLASS;         break;
        case TAG_ENUM:       result = JK_ENUM;          break;
        case TAG_ENUMERATOR: result = JK_ENUM_CONSTANT; break;
        case TAG_FIELD:      result = JK_FIELD;         break;
        case TAG_INTERFACE:  result = JK_INTERFACE;     break;
        case TAG_LOCAL:      result = JK_LOCAL;         break;
        case TAG_METHOD:     result = JK_METHOD;        break;
        case TAG_PACKAGE:    result = JK_PACKAGE;       break;

        default: Assert ("Bad Java tag type" == NULL); break;
    }
    return result;
}

veraKind Parser_Cpp::veraTagKind (const tagType type) {
    veraKind result = VK_UNDEFINED;
    switch (type)
    {
        case TAG_CLASS:      result = VK_CLASS;           break;
        case TAG_ENUM:       result = VK_ENUMERATION;     break;
        case TAG_ENUMERATOR: result = VK_ENUMERATOR;      break;
        case TAG_FUNCTION:   result = VK_FUNCTION;        break;
        case TAG_LOCAL:      result = VK_LOCAL;           break;
        case TAG_MEMBER:     result = VK_MEMBER;          break;
        case TAG_PROGRAM:    result = VK_PROGRAM;         break;
        case TAG_PROTOTYPE:  result = VK_PROTOTYPE;       break;
        case TAG_TASK:       result = VK_TASK;            break;
        case TAG_TYPEDEF:    result = VK_TYPEDEF;         break;
        case TAG_VARIABLE:   result = VK_VARIABLE;        break;
        case TAG_EXTERN_VAR: result = VK_EXTERN_VARIABLE; break;

        default: Assert ("Bad Vera tag type" == NULL); break;
    }
    return result;
}

const char *Parser_Cpp::tagName (const tagType type)
{
    const char* result;
    if (isLanguage (Lang_csharp))
        result = CsharpKinds [csharpTagKind (type)].name;
    else if (isLanguage (Lang_java))
        result = JavaKinds [javaTagKind (type)].name;
    else if (isLanguage (Lang_vera))
        result = VeraKinds [veraTagKind (type)].name;
    else
        result = CKinds [cTagKind (type)].name;
    return result;
}

int Parser_Cpp::tagLetter (const tagType type)
{
    int result;
    if (isLanguage (Lang_csharp))
        result = CsharpKinds [csharpTagKind (type)].letter;
    else if (isLanguage (Lang_java))
        result = JavaKinds [javaTagKind (type)].letter;
    else if (isLanguage (Lang_vera))
        result = VeraKinds [veraTagKind (type)].letter;
    else
        result = CKinds [cTagKind (type)].letter;
    return result;
}

boolean Parser_Cpp::includeTag (const tagType type, const boolean isFileScope)
{
    boolean result;
    if (isFileScope  &&  false /*! Option.include.fileScope*/)
        result = false;
    else if (isLanguage (Lang_csharp))
        result = CsharpKinds [csharpTagKind (type)].enabled;
    else if (isLanguage (Lang_java))
        result = JavaKinds [javaTagKind (type)].enabled;
    else if (isLanguage (Lang_vera))
        result = VeraKinds [veraTagKind (type)].enabled;
    else
        result = CKinds [cTagKind (type)].enabled;
    return result;
}

tagType Parser_Cpp::declToTagType (const declType declaration)
{
    tagType type = TAG_UNDEFINED;

    switch (declaration)
    {
        case DECL_CLASS:        type = TAG_CLASS;       break;
        case DECL_ENUM:         type = TAG_ENUM;        break;
        case DECL_EVENT:        type = TAG_EVENT;       break;
        case DECL_FUNCTION:     type = TAG_FUNCTION;    break;
        case DECL_INTERFACE:    type = TAG_INTERFACE;   break;
        case DECL_NAMESPACE:    type = TAG_NAMESPACE;   break;
        case DECL_PROGRAM:      type = TAG_PROGRAM;     break;
        case DECL_TASK:         type = TAG_TASK;        break;
        case DECL_STRUCT:       type = TAG_STRUCT;      break;
        case DECL_UNION:        type = TAG_UNION;       break;

        default: Assert ("Unexpected declaration" == NULL); break;
    }
    return type;
}

const char* Parser_Cpp::accessField (const statementInfo *const st)
{
    const char* result = NULL;
    if (isLanguage (Lang_cpp)  &&  st->scope == SCOPE_FRIEND)
        result = "friend";
    else if (st->member.access != ACCESS_UNDEFINED)
        result = accessString (st->member.access);
    return result;
}

void Parser_Cpp::addContextSeparator (vString *const scope)
{
    if (isLanguage (Lang_c)  ||  isLanguage (Lang_cpp))
        vStringCatS (scope, "::");
    else if (isLanguage (Lang_java) || isLanguage (Lang_csharp))
        vStringCatS (scope, ".");
}


//void Parser_Cpp::addOtherFields (tagEntryInfo* const tag, const tagType type,
//                                                        const statementInfo *const st,
//                                                        vString *const scope, vString *const typeRef)
//{
//    /*  For selected tag types, append an extension flag designating the
//     *  parent object in which the tag is defined.
//     */
//    switch (type)
//    {
//    default: break;
//
//    case TAG_FUNCTION:
//    case TAG_METHOD:
//    case TAG_PROTOTYPE:
//        if (vStringLength (Signature) > 0)
//            tag->extensionFields.signature = vStringValue (Signature);
//    case TAG_CLASS:
//    case TAG_ENUM:
//    case TAG_ENUMERATOR:
//    case TAG_EVENT:
//    case TAG_FIELD:
//    case TAG_INTERFACE:
//    case TAG_MEMBER:
//    case TAG_NAMESPACE:
//    case TAG_PROPERTY:
//    case TAG_STRUCT:
//    case TAG_TASK:
//    case TAG_TYPEDEF:
//    case TAG_UNION:
//        if (vStringLength (scope) > 0  &&
//            (isMember (st) || st->parent->declaration == DECL_NAMESPACE))
//        {
//            if (isType (st->context, TOKEN_NAME))
//                tag->extensionFields.scope [0] = tagName (TAG_CLASS);
//            else
//                tag->extensionFields.scope [0] =
//                        tagName (declToTagType (parentDecl (st)));
//            tag->extensionFields.scope [1] = vStringValue (scope);
//        }
//        if ((type == TAG_CLASS  ||  type == TAG_INTERFACE  ||
//             type == TAG_STRUCT) && vStringLength (st->parentClasses) > 0)
//        {
//
//            tag->extensionFields.inheritance =
//                    vStringValue (st->parentClasses);
//        }
//        if (st->implementation != IMP_DEFAULT &&
//            (isLanguage (Lang_cpp) || isLanguage (Lang_csharp) ||
//             isLanguage (Lang_java)))
//        {
//            tag->extensionFields.implementation =
//                    implementationString (st->implementation);
//        }
//        if (isMember (st))
//        {
//            tag->extensionFields.access = accessField (st);
//        }
//        break;
//    }
//
//    /* Add typename info, type of the tag and name of struct/union/etc. */
//    if ((type == TAG_TYPEDEF || type == TAG_VARIABLE || type == TAG_MEMBER)
//        && isContextualStatement(st))
//        {
//        char *p;
//
//        tag->extensionFields.typeRef [0] =
//                tagName (declToTagType (st->declaration));
//        p = vStringValue (st->blockName->name);
//
//        /*  If there was no {} block get the name from the token before the
//         *  name (current token is ';' or ',', previous token is the name).
//         */
//        if (p == NULL || *p == '\0')
//        {
//            tokenInfo *const prev2 = prevToken (st, 2);
//            if (isType (prev2, TOKEN_NAME))
//                p = vStringValue (prev2->name);
//        }
//
//        /* Prepend the scope name if there is one. */
//        if (vStringLength (scope) > 0)
//        {
//            vStringCopy(typeRef, scope);
//            addContextSeparator (typeRef);
//            vStringCatS(typeRef, p);
//            p = vStringValue (typeRef);
//        }
//        tag->extensionFields.typeRef [1] = p;
//    }
//}


void Parser_Cpp::findScopeHierarchy (vString *const string, const statementInfo *const st)
{
    vStringClear (string);

    if (isType (st->context, TOKEN_NAME)) {
        vStringCopy (string, st->context->name);
    }

    if (st->parent != NULL)
    {
        vString *temp = vStringNew ();
        const statementInfo *s;
        for (s = st->parent  ;  s != NULL  ;  s = s->parent)
        {
            if (isContextualStatement (s) ||
                s->declaration == DECL_NAMESPACE ||
                s->declaration == DECL_PROGRAM)
            {
                vStringCopy (temp, string);
                vStringClear (string);
                Assert (isType (s->blockName, TOKEN_NAME));

                if (isType (s->context, TOKEN_NAME) &&
                    vStringLength (s->context->name) > 0)
                {

                    vStringCat (string, s->context->name);
                    addContextSeparator (string);
                }

                vStringCat (string, s->blockName->name);

                if (vStringLength (temp) > 0)
                    addContextSeparator (string);

                vStringCat (string, temp);
            }
        }
        vStringDelete (temp);
    }
}


//void Parser_Cpp::makeExtraTagEntry (const tagType type, tagEntryInfo *const e,
//                                                           vString *const scope)
//{
//    if (Option.include.qualifiedTags  &&
//        scope != NULL  &&  vStringLength (scope) > 0)
//    {
//        vString *const scopedName = vStringNew ();
//
//        if (type != TAG_ENUMERATOR)
//            vStringCopy (scopedName, scope);
//        else
//        {
//            /* remove last component (i.e. enumeration name) from scope */
//            const char* const sc = vStringValue (scope);
//            const char* colon = strrchr (sc, ':');
//            if (colon != NULL)
//            {
//                while (*colon == ':'  &&  colon > sc)
//                    --colon;
//                vStringNCopy (scopedName, scope, colon + 1 - sc);
//            }
//        }
//        if (vStringLength (scopedName) > 0)
//        {
//            addContextSeparator (scopedName);
//            vStringCatS (scopedName, e->name);
//            e->name = vStringValue (scopedName);
//            makeTagEntry (e);
//        }
//        vStringDelete (scopedName);
//    }
//}


#ifdef DEBUG
/*****************************************************************************
 *
 *****************************************************************************/
QString Parser_Cpp::tagTypeToStr(const tagType type) {
    switch (type) {
    case TAG_UNDEFINED:     return "TAG_UNDEFINED";
    case TAG_CLASS:         return "TAG_CLASS";       /* class name */
    case TAG_ENUM:          return "TAG_ENUM";        /* enumeration name */
    case TAG_ENUMERATOR:    return "TAG_ENUMERATOR";  /* enumerator (enumeration value) */
    case TAG_EVENT:         return "TAG_EVENT";       /* event */
    case TAG_FIELD:         return "TAG_FIELD";       /* field (Java) */
    case TAG_FUNCTION:      return "TAG_FUNCTION";    /* function definition */
    case TAG_INTERFACE:     return "TAG_INTERFACE";   /* interface declaration */
    case TAG_LOCAL:         return "TAG_LOCAL";       /* local variable definition */
    case TAG_MEMBER:        return "TAG_MEMBER";      /* structure, class or interface member */
    case TAG_METHOD:        return "TAG_METHOD";      /* method declaration */
    case TAG_NAMESPACE:     return "TAG_NAMESPACE";   /* namespace name */
    case TAG_PACKAGE:       return "TAG_PACKAGE";     /* package name */
    case TAG_PROGRAM:       return "TAG_PROGRAM";     /* program name */
    case TAG_PROPERTY:      return "TAG_PROPERTY";    /* property name */
    case TAG_PROTOTYPE:     return "TAG_PROTOTYPE";   /* function prototype or declaration */
    case TAG_STRUCT:        return "TAG_STRUCT";      /* structure name */
    case TAG_TASK:          return "TAG_TASK";        /* task name */
    case TAG_TYPEDEF:       return "TAG_TYPEDEF";     /* typedef name */
    case TAG_UNION:         return "TAG_UNION";       /* union name */
    case TAG_VARIABLE:      return "TAG_VARIABLE";    /* variable definition */
    case TAG_EXTERN_VAR:    return "TAG_EXTERN_VAR";  /* external variable declaration */
    case TAG_COUNT:         return "TAG_COUNT";       /* mus */
    default:                return "Unknown tagType";
    }
}
#endif


/*****************************************************************************
 *
 *****************************************************************************/
Symbol::SymbolType Parser_Cpp::declToSymbolType (const declType declaration)
{
    Symbol::SymbolType type = Symbol::SymbolUnknown;

    switch (declaration)
    {
        case DECL_CLASS:        type = Symbol::SymbolClass;     break;
        case DECL_FUNCTION:     type = Symbol::SymbolFunc;      break;
        case DECL_STRUCT:       type = Symbol::SymbolStruct;    break;
//        case DECL_UNION:        type = Symbol::SymbolUnknown;   break;
//      case DECL_ENUM:         type = TAG_ENUM;        break;
//      case DECL_EVENT:        type = TAG_EVENT;       break;
//      case DECL_INTERFACE:    type = TAG_INTERFACE;   break;
        case DECL_NAMESPACE:    type = Symbol::SymbolNamespace;   break;
//      case DECL_PROGRAM:      type = TAG_PROGRAM;     break;
//      case DECL_TASK:         type = TAG_TASK;        break;

        default: Assert ("Unexpected declaration" == NULL); break;
    }
    return type;
}


/*****************************************************************************
 *
 *****************************************************************************/
Symbol *Parser_Cpp::getParent(const statementInfo *const st) {
    Symbol *result = mSymbols;

    QList<const statementInfo*> list;

    const statementInfo *s;
    for (s = st->parent  ;  s != NULL  ;  s = s->parent) {
        if (isContextualStatement (s) ||
            s->declaration == DECL_NAMESPACE ||
            s->declaration == DECL_PROGRAM)
        {
            list.prepend(s);
        }
    }

    Symbol *prev = mSymbols;
    result = mSymbols;
    for(int i=0; i<list.count(); ++i) {
        s = list.at(i);
        QString name = vStringToQString(s->blockName->name);
        Symbol::SymbolType type = declToSymbolType(s == NULL ? DECL_NONE : s->declaration);

        result = prev->find(name, type);
        if (!result) {
            result = new Symbol(type, name, prev);
        }

        prev = result;
    }



    if (isType (st->context, TOKEN_NAME)) {
        QString name = vStringToQString(st->context->name);
        prev = result;
        result = prev->find(name);
        if (!result)
            result = new Symbol(Symbol::SymbolClass, vStringToQString(st->context->name), prev);
    }



    return result;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Parser_Cpp::makeTag (const tokenInfo *const token,
                                         const statementInfo *const st,
                                         boolean isFileScope, const tagType type)
{
    Q_UNUSED(isFileScope);

    if ( type == TAG_METHOD || type == TAG_FUNCTION )

    {
        QString name = vStringToQString(token->name);
        Symbol *symbol = NULL;
        Symbol *parent =  getParent(st);


        symbol =  new Symbol(Symbol::SymbolFunc, name, parent);
        symbol->setDetailedText(name + ' ' + vStringToQString(Signature));
        symbol->setLine(token->lineNumber);

        Symbol *proto = parent->find(name, Symbol::SymbolPrototype);

        if (proto) {
            proto->setParent(NULL);
            symbol->setRelatedSymbol(proto);
        }

        return;
    }


    Symbol *symbol = NULL;
    QString name = vStringToQString(token->name);

    switch (type) {

        case TAG_MEMBER:    /* structure, class or interface member */
            break;

        case TAG_STRUCT:    /* structure name */
            symbol =  new Symbol(Symbol::SymbolStruct, name);
            break;

        case TAG_CLASS:     /* class name */
            symbol =  new Symbol(Symbol::SymbolClass, name);
            break;

        case TAG_METHOD:    /* method declaration */
            symbol =  new Symbol(Symbol::SymbolFunc, name);
            symbol->setDetailedText(name + vStringToQString(Signature));
            break;

        case TAG_FUNCTION:  /* function definition */
            symbol =  new Symbol(Symbol::SymbolFunc, name);
            symbol->setDetailedText(name + vStringToQString(Signature));
            break;

        case TAG_PROTOTYPE: /* function prototype or declaration */
            symbol =  new Symbol(Symbol::SymbolPrototype, name);
            symbol->setDetailedText(name + vStringToQString(Signature));
            break;

        case TAG_NAMESPACE:   /* namespace name */
            symbol =  new Symbol(Symbol::SymbolNamespace, name);
            break;

        default:
            #ifdef DEBUG
            qDebug() << QString("Token '%1' have unknown type '%2'.").arg(vStringToQString(token->name)).arg(tagTypeToStr(type));
            #endif
            break;
    }


    if (symbol) {
        Symbol *parent = getParent(st);
        symbol->setParent(parent);
        symbol->setLine(token->lineNumber);


       // vString *scope = vStringNew ();
        //findScopeHierarchy (scope, st);
        //vStringDelete (scope);

    }



    //            TAG_CLASS,       /* class name */
    //            TAG_ENUM,        /* enumeration name */
    //            TAG_ENUMERATOR,  /* enumerator (enumeration value) */
    //            TAG_EVENT,       /* event */
    //            TAG_FIELD,       /* field (Java) */
    //            TAG_FUNCTION,    /* function definition */
    //            TAG_INTERFACE,   /* interface declaration */
    //            TAG_LOCAL,       /* local variable definition */
    //            TAG_MEMBER,      /* structure, class or interface member */


    //            TAG_PACKAGE,     /* package name */
    //            TAG_PROGRAM,     /* program name */
    //            TAG_PROPERTY,    /* property name */
    //            TAG_PROTOTYPE,   /* function prototype or declaration */
    //            TAG_STRUCT,      /* structure name */
    //            TAG_TASK,        /* task name */
    //            TAG_TYPEDEF,     /* typedef name */
    //            TAG_UNION,       /* union name */
    //            TAG_VARIABLE,    /* variable definition */
    //            TAG_EXTERN_VAR,  /* external variable declaration */
    //            TAG_COUNT        /* must be last */


#if 0
    /*  Nothing is really of file scope when it appears in a header file.
     */
    isFileScope = (boolean) (isFileScope && ! isHeaderFile ());

    if (isType (token, TOKEN_NAME)  &&  vStringLength (token->name) > 0  &&
        includeTag (type, isFileScope))
    {
        vString *scope = vStringNew ();
        /* Use "typeRef" to store the typename from addOtherFields() until
         * it's used in makeTagEntry().
         */
        vString *typeRef = vStringNew ();
        tagEntryInfo e;

        initTagEntry (&e, vStringValue (token->name));

        e.lineNumber	= token->lineNumber;
        e.filePosition= token->filePosition;
        e.isFileScope	= isFileScope;
        e.kindName	= tagName (type);
        e.kind	= tagLetter (type);

        findScopeHierarchy (scope, st);
        addOtherFields (&e, type, st, scope, typeRef);

        makeTagEntry (&e);
        makeExtraTagEntry (type, &e, scope);
        vStringDelete (scope);
        vStringDelete (typeRef);
    }
#endif
}

boolean Parser_Cpp::isValidTypeSpecifier (const declType declaration)
{
    boolean result;
    switch (declaration)
    {
      case DECL_BASE:
      case DECL_CLASS:
      case DECL_ENUM:
      case DECL_EVENT:
      case DECL_STRUCT:
      case DECL_UNION:
        result = true;
        break;

      default:
        result = false;
        break;
    }
    return result;
}

void Parser_Cpp::qualifyEnumeratorTag (const statementInfo *const st,
                                                                  const tokenInfo *const nameToken)
{
    if (isType (nameToken, TOKEN_NAME))
        makeTag (nameToken, st, true, TAG_ENUMERATOR);
}

void Parser_Cpp::qualifyFunctionTag (const statementInfo *const st,
                                                                const tokenInfo *const nameToken)
{
    if (isType (nameToken, TOKEN_NAME))
    {
        tagType type;
        const boolean isFileScope =
                (boolean) (st->member.access == ACCESS_PRIVATE ||
                           (!isMember (st)  &&  st->scope == SCOPE_STATIC));
        if (isLanguage (Lang_java) || isLanguage (Lang_csharp))
            type = TAG_METHOD;
        else if (isLanguage (Lang_vera)  &&  st->declaration == DECL_TASK)
            type = TAG_TASK;
        else
            type = TAG_FUNCTION;
        makeTag (nameToken, st, isFileScope, type);
    }
}

void Parser_Cpp::qualifyFunctionDeclTag (const statementInfo *const st,
                                                                        const tokenInfo *const nameToken)
{
    if (! isType (nameToken, TOKEN_NAME))
        ;
    else if (isLanguage (Lang_java) || isLanguage (Lang_csharp))
        qualifyFunctionTag (st, nameToken);
    else if (st->scope == SCOPE_TYPEDEF)
        makeTag (nameToken, st, true, TAG_TYPEDEF);
    else if (isValidTypeSpecifier (st->declaration) && ! isLanguage (Lang_csharp))
        makeTag (nameToken, st, true, TAG_PROTOTYPE);
}

void Parser_Cpp::qualifyCompoundTag (const statementInfo *const st,
                                                                const tokenInfo *const nameToken)
{
    if (isType (nameToken, TOKEN_NAME))
    {
        const tagType type = declToTagType (st->declaration);
        const boolean fileScoped = (boolean)
                                   (!(isLanguage (Lang_java) ||
                                      isLanguage (Lang_csharp) ||
                                      isLanguage (Lang_vera)));

        if (type != TAG_UNDEFINED)
            makeTag (nameToken, st, fileScoped, type);
    }
}

void Parser_Cpp::qualifyBlockTag (statementInfo *const st,
                                                         const tokenInfo *const nameToken)
{
    switch (st->declaration)
    {
        case DECL_CLASS:
        case DECL_ENUM:
        case DECL_INTERFACE:
        case DECL_NAMESPACE:
        case DECL_PROGRAM:
        case DECL_STRUCT:
        case DECL_UNION:
            qualifyCompoundTag (st, nameToken);
            break;
        default: break;
    }
}

void Parser_Cpp::qualifyVariableTag (const statementInfo *const st,
                                                                const tokenInfo *const nameToken)
{
    /*	We have to watch that we do not interpret a declaration of the
     *	form "struct tag;" as a variable definition. In such a case, the
     *	token preceding the name will be a keyword.
     */
    if (! isType (nameToken, TOKEN_NAME))
        ;
    else if (st->scope == SCOPE_TYPEDEF)
        makeTag (nameToken, st, true, TAG_TYPEDEF);
    else if (st->declaration == DECL_EVENT)
        makeTag (nameToken, st, (boolean) (st->member.access == ACCESS_PRIVATE),
                 TAG_EVENT);
    else if (st->declaration == DECL_PACKAGE)
        makeTag (nameToken, st, false, TAG_PACKAGE);
    else if (isValidTypeSpecifier (st->declaration))
    {
        if (st->notVariable)
            ;
        else if (isMember (st))
        {
            if (isLanguage (Lang_java) || isLanguage (Lang_csharp))
                makeTag (nameToken, st,
                         (boolean) (st->member.access == ACCESS_PRIVATE), TAG_FIELD);
            else if (st->scope == SCOPE_GLOBAL  ||  st->scope == SCOPE_STATIC)
                makeTag (nameToken, st, true, TAG_MEMBER);
        }
        else
        {
            if (st->scope == SCOPE_EXTERN  ||  ! st->haveQualifyingName)
                makeTag (nameToken, st, false, TAG_EXTERN_VAR);
            else if (st->inFunction)
                makeTag (nameToken, st, (boolean) (st->scope == SCOPE_STATIC),
                         TAG_LOCAL);
            else
                makeTag (nameToken, st, (boolean) (st->scope == SCOPE_STATIC),
                         TAG_VARIABLE);
        }
    }
}

/*
*   Parsing functions
*/

int Parser_Cpp::skipToOneOf (const char *const chars)
{

    int c;
    do
        c = cppGetc ();
    while (c != EOF  &&  c != '\0'  &&  strchr (chars, c) == NULL);
    return c;
}

/*  Skip to the next non-white character.
 */
int Parser_Cpp::skipToNonWhite (void)
{
    boolean found = false;
    int c;

//#if 0
//    do
//            c =cppGetc ();
//    while (isspace (c));
//#else
    while (1)
    {
        c = cppGetc ();
        if (isspace (c))
            found = true;
        else
            break;
    }
    if (CollectingSignature && found)
        vStringPut (Signature, ' ');
//#endif

    return c;
}

/*  Skips to the next brace in column 1. This is intended for cases where
 *  preprocessor constructs result in unbalanced braces.
 */
void Parser_Cpp::skipToFormattedBraceMatch (void)
{
    int c, next;

    c = cppGetc ();
    next = cppGetc ();
    while (c != EOF  &&  (c != '\n'  ||  next != '}'))
    {
        c = next;
        next = cppGetc ();
    }
}

/*  Skip to the matching character indicated by the pair string. If skipping
 *  to a matching brace and any brace is found within a different level of a
 *  #if conditional statement while brace formatting is in effect, we skip to
 *  the brace matched by its formatting. It is assumed that we have already
 *  read the character which starts the group (i.e. the first character of
 *  "pair").
 */
void Parser_Cpp::skipToMatch (const char *const pair)
{
    const boolean braceMatching = (boolean) (strcmp ("{}", pair) == 0);
    const boolean braceFormatting = (boolean) (isBraceFormat () && braceMatching);
    const unsigned int initialLevel = getDirectiveNestLevel ();
    const int begin = pair [0], end = pair [1];
    //const unsigned long inputLineNumber = getInputLineNumber ();
    int matchLevel = 1;
    int c = '\0';

    while (matchLevel > 0  &&  (c = skipToNonWhite ()) != EOF)
    {
        if (CollectingSignature)
            vStringPut (Signature, c);
        if (c == begin)
        {
            ++matchLevel;
            if (braceFormatting  &&  getDirectiveNestLevel () != initialLevel)
            {
                skipToFormattedBraceMatch ();
                break;
            }
        }
        else if (c == end)
        {
            --matchLevel;
            if (braceFormatting  &&  getDirectiveNestLevel () != initialLevel)
            {
                skipToFormattedBraceMatch ();
                break;
            }
        }
    }
    if (c == EOF)
    {
        verbose ("%s: failed to find match for '%c' at line %lu\n",
                 getInputFileName (), begin, inputLineNumber);
        if (braceMatching)
            longjmp (Exception, (int) ExceptionBraceFormattingError);
        else
            longjmp (Exception, (int) ExceptionFormattingError);
    }
}

void Parser_Cpp::skipParens (void)
{
    const int c = skipToNonWhite ();

    if (c == '(')
        skipToMatch ("()");
    else
        cppUngetc (c);
}

void Parser_Cpp::skipBraces (void)
{
    const int c = skipToNonWhite ();

    if (c == '{')
        skipToMatch ("{}");
    else
        cppUngetc (c);
}

keywordId Parser_Cpp::analyzeKeyword (const char *const name)
{
    const keywordId id = (keywordId) keywords.lookupKeyword(name, language());//getSourceLanguage ());
    return id;
}

void Parser_Cpp::analyzeIdentifier (tokenInfo *const token)
{
    //char *const name = vStringValue (token->name);
    const char *replacement = NULL;
    boolean parensToo = false;

    if (isLanguage (Lang_java) || true /* ! isIgnoreToken (name, &parensToo, &replacement)*/)
    {
        if (replacement != NULL)
            token->keyword = analyzeKeyword (replacement);
        else
            token->keyword = analyzeKeyword (vStringValue (token->name));

        if (token->keyword == KEYWORD_NONE)
            token->type = TOKEN_NAME;
        else
            token->type = TOKEN_KEYWORD;

    }
    else
    {
        initToken (token);
        if (parensToo)
        {
            int c = skipToNonWhite ();

            if (c == '(')
                skipToMatch ("()");
        }
    }
}

void Parser_Cpp::readIdentifier (tokenInfo *const token, const int firstChar)
{
    vString *const name = token->name;
    int c = firstChar;
    boolean first = true;

    initToken (token);

    /* Bug #1585745: strangely, C++ destructors allow whitespace between
     * the ~ and the class name. */
    if (isLanguage (Lang_cpp) && firstChar == '~')
    {
        vStringPut (name, c);
        c = skipToNonWhite ();
    }

    do
    {
        vStringPut (name, c);
        if (CollectingSignature)
        {
            if (!first)
                vStringPut (Signature, c);
            first = false;
        }
        c = cppGetc ();
    } while (isident (c) || ((isLanguage (Lang_java) || isLanguage (Lang_csharp)) && (isHighChar (c) || c == '.')));
    vStringTerminate (name);
    cppUngetc (c);        /* unget non-identifier character */

    analyzeIdentifier (token);
}

void Parser_Cpp::readPackageName (tokenInfo *const token, const int firstChar)
{
    vString *const name = token->name;
    int c = firstChar;

    initToken (token);

    while (isident (c)  ||  c == '.')
    {
        vStringPut (name, c);
        c = cppGetc ();
    }
    vStringTerminate (name);
    cppUngetc (c);        /* unget non-package character */
}

void Parser_Cpp::readPackageOrNamespace (statementInfo *const st, const declType declaration)
{
    st->declaration = declaration;

    if (declaration == DECL_NAMESPACE && !isLanguage (Lang_csharp))
    {
        /* In C++ a namespace is specified one level at a time. */
        return;
    }
    else
    {
        /* In C#, a namespace can also be specified like a Java package name. */
        tokenInfo *const token = activeToken (st);
        Assert (isType (token, TOKEN_KEYWORD));
        readPackageName (token, skipToNonWhite ());
        token->type = TOKEN_NAME;
        st->gotName = true;
        st->haveQualifyingName = true;
    }
}

void Parser_Cpp::processName (statementInfo *const st)
{
    Assert (isType (activeToken (st), TOKEN_NAME));
    if (st->gotName  &&  st->declaration == DECL_NONE)
        st->declaration = DECL_BASE;
    st->gotName = true;
    st->haveQualifyingName = true;
}

void Parser_Cpp::readOperator (statementInfo *const st)
{
    const char *const acceptable = "+-*/%^&|~!=<>,[]";
    const tokenInfo* const prev = prevToken (st,1);
    tokenInfo *const token = activeToken (st);
    vString *const name = token->name;
    int c = skipToNonWhite ();

    /*  When we arrive here, we have the keyword "operator" in 'name'.
     */
    if (isType (prev, TOKEN_KEYWORD) && (prev->keyword == KEYWORD_ENUM ||
                                         prev->keyword == KEYWORD_STRUCT || prev->keyword == KEYWORD_UNION))
        ;        /* ignore "operator" keyword if preceded by these keywords */
    else if (c == '(')
    {
        /*  Verify whether this is a valid function call (i.e. "()") operator.
         */
        if (cppGetc () == ')')
        {
            vStringPut (name, ' ');  /* always separate operator from keyword */
            c = skipToNonWhite ();
            if (c == '(')
                vStringCatS (name, "()");
        }
        else
        {
            skipToMatch ("()");
            c = cppGetc ();
        }
    }
    else if (isident1 (c))
    {
        /*  Handle "new" and "delete" operators, and conversion functions
         *  (per 13.3.1.1.2 [2] of the C++ spec).
         */
        boolean whiteSpace = true;  /* default causes insertion of space */
        do
        {
            if (isspace (c))
                whiteSpace = true;
            else
            {
                if (whiteSpace)
                {
                    vStringPut (name, ' ');
                    whiteSpace = false;
                }
                vStringPut (name, c);
            }
            c = cppGetc ();
        } while (! isOneOf (c, "(;")  &&  c != EOF);
        vStringTerminate (name);
    }
    else if (isOneOf (c, acceptable))
    {
        vStringPut (name, ' ');  /* always separate operator from keyword */
        do
        {
            vStringPut (name, c);
            c = cppGetc ();
        } while (isOneOf (c, acceptable));
        vStringTerminate (name);
    }

    cppUngetc (c);

    token->type	= TOKEN_NAME;
    token->keyword = KEYWORD_NONE;
    processName (st);
}

void Parser_Cpp::copyToken (tokenInfo *const dest, const tokenInfo *const src)
{
    dest->type         = src->type;
    dest->keyword      = src->keyword;
 //   dest->filePosition = src->filePosition;
    dest->lineNumber   = src->lineNumber;
    vStringCopy (dest->name, src->name);
}

void Parser_Cpp::setAccess (statementInfo *const st, const accessType access)
{
    if (isMember (st))
    {
        if (isLanguage (Lang_cpp))
        {
            int c = skipToNonWhite ();

            if (c == ':')
                reinitStatement (st, false);
            else
                cppUngetc (c);

            st->member.accessDefault = access;
        }
        st->member.access = access;
    }
}

void Parser_Cpp::discardTypeList (tokenInfo *const token)
{
    int c = skipToNonWhite ();
    while (isident1 (c))
    {
        readIdentifier (token, c);
        c = skipToNonWhite ();
        if (c == '.'  ||  c == ',')
            c = skipToNonWhite ();
    }
    cppUngetc (c);
}

void Parser_Cpp::addParentClass (statementInfo *const st, tokenInfo *const token)
{
    if (vStringLength (token->name) > 0  &&
        vStringLength (st->parentClasses) > 0)
    {
        vStringPut (st->parentClasses, ',');
    }
    vStringCat (st->parentClasses, token->name);
}

void Parser_Cpp::readParents (statementInfo *const st, const int qualifier)
{
    tokenInfo *const token = newToken ();
    tokenInfo *const parent = newToken ();
    int c;

    do
    {
        c = skipToNonWhite ();
        if (isident1 (c))
        {
            readIdentifier (token, c);
            if (isType (token, TOKEN_NAME))
                vStringCat (parent->name, token->name);
            else
            {
                addParentClass (st, parent);
                initToken (parent);
            }
        }
        else if (c == qualifier)
            vStringPut (parent->name, c);
        else if (c == '<')
            skipToMatch ("<>");
        else if (isType (token, TOKEN_NAME))
        {
            addParentClass (st, parent);
            initToken (parent);
        }
    } while (c != '{'  &&  c != EOF);
    cppUngetc (c);
    deleteToken (parent);
    deleteToken (token);
}

void Parser_Cpp::skipStatement (statementInfo *const st)
{
    st->declaration = DECL_IGNORE;
    skipToOneOf (";");
}

void Parser_Cpp::processInterface (statementInfo *const st)
{
    st->declaration = DECL_INTERFACE;
}

void Parser_Cpp::processToken (tokenInfo *const token, statementInfo *const st)
{
    switch (token->keyword)        /* is it a reserved word? */
    {
        default: break;

        case KEYWORD_NONE:      processName (st);                       break;
        case KEYWORD_ABSTRACT:  st->implementation = IMP_ABSTRACT;      break;
        case KEYWORD_ATTRIBUTE: skipParens (); initToken (token);       break;
        case KEYWORD_BIND:      st->declaration = DECL_BASE;            break;
        case KEYWORD_BIT:       st->declaration = DECL_BASE;            break;
        case KEYWORD_CATCH:     skipParens (); skipBraces ();           break;
        case KEYWORD_CHAR:      st->declaration = DECL_BASE;            break;
        case KEYWORD_CLASS:     st->declaration = DECL_CLASS;           break;
        case KEYWORD_CONST:     st->declaration = DECL_BASE;            break;
        case KEYWORD_DOUBLE:    st->declaration = DECL_BASE;            break;
        case KEYWORD_ENUM:      st->declaration = DECL_ENUM;            break;
        case KEYWORD_EXTENDS:   readParents (st, '.');
            setToken (st, TOKEN_NONE);              break;
        case KEYWORD_FLOAT:     st->declaration = DECL_BASE;            break;
        case KEYWORD_FUNCTION:  st->declaration = DECL_BASE;            break;
        case KEYWORD_FRIEND:    st->scope       = SCOPE_FRIEND;         break;
        case KEYWORD_GOTO:      skipStatement (st);                     break;
        case KEYWORD_IMPLEMENTS:readParents (st, '.');
            setToken (st, TOKEN_NONE);              break;
        case KEYWORD_IMPORT:    skipStatement (st);                     break;
        case KEYWORD_INT:       st->declaration = DECL_BASE;            break;
        case KEYWORD_INTEGER:   st->declaration = DECL_BASE;            break;
        case KEYWORD_INTERFACE: processInterface (st);                  break;
        case KEYWORD_LOCAL:     setAccess (st, ACCESS_LOCAL);           break;
        case KEYWORD_LONG:      st->declaration = DECL_BASE;            break;
        case KEYWORD_OPERATOR:  readOperator (st);                      break;
        case KEYWORD_PRIVATE:   setAccess (st, ACCESS_PRIVATE);         break;
        case KEYWORD_PROGRAM:   st->declaration = DECL_PROGRAM;         break;
        case KEYWORD_PROTECTED: setAccess (st, ACCESS_PROTECTED);       break;
        case KEYWORD_PUBLIC:    setAccess (st, ACCESS_PUBLIC);          break;
        case KEYWORD_RETURN:    skipStatement (st);                     break;
        case KEYWORD_SHORT:     st->declaration = DECL_BASE;            break;
        case KEYWORD_SIGNED:    st->declaration = DECL_BASE;            break;
        case KEYWORD_STRING:    st->declaration = DECL_BASE;            break;
        case KEYWORD_STRUCT:    st->declaration = DECL_STRUCT;          break;
        case KEYWORD_TASK:      st->declaration = DECL_TASK;            break;
        case KEYWORD_THROWS:    discardTypeList (token);                break;
        case KEYWORD_UNION:     st->declaration = DECL_UNION;           break;
        case KEYWORD_UNSIGNED:  st->declaration = DECL_BASE;            break;
        case KEYWORD_USING:     skipStatement (st);                     break;
        case KEYWORD_VOID:      st->declaration = DECL_BASE;            break;
        case KEYWORD_VOLATILE:  st->declaration = DECL_BASE;            break;
        case KEYWORD_VIRTUAL:   st->implementation = IMP_VIRTUAL;       break;
        case KEYWORD_WCHAR_T:   st->declaration = DECL_BASE;            break;

        case KEYWORD_NAMESPACE: readPackageOrNamespace (st, DECL_NAMESPACE); break;
        case KEYWORD_PACKAGE:   readPackageOrNamespace (st, DECL_PACKAGE);   break;

        case KEYWORD_EVENT:
            if (isLanguage (Lang_csharp))
                st->declaration = DECL_EVENT;
            break;

        case KEYWORD_TYPEDEF:
            reinitStatement (st, false);
            st->scope = SCOPE_TYPEDEF;
            break;

        case KEYWORD_EXTERN:
            if (! isLanguage (Lang_csharp) || !st->gotName)
            {
                reinitStatement (st, false);
                st->scope = SCOPE_EXTERN;
                st->declaration = DECL_BASE;
            }
            break;

                case KEYWORD_STATIC:
            if (! (isLanguage (Lang_java) || isLanguage (Lang_csharp)))
            {
                reinitStatement (st, false);
                st->scope = SCOPE_STATIC;
                st->declaration = DECL_BASE;
            }
            break;

                case KEYWORD_FOR:
                case KEYWORD_FOREACH:
                case KEYWORD_IF:
                case KEYWORD_SWITCH:
                case KEYWORD_WHILE:
            {
                int c = skipToNonWhite ();
                if (c == '(')
                    skipToMatch ("()");
                break;
            }
        }
    }

/*
*   Parenthesis handling functions
*/

void Parser_Cpp::restartStatement (statementInfo *const st)
{
    tokenInfo *const save = newToken ();
    tokenInfo *token = activeToken (st);

    copyToken (save, token);
    DebugStatement ( if (debug (DEBUG_PARSE)) printf ("<ES>");)
        reinitStatement (st, false);
    token = activeToken (st);
    copyToken (token, save);
    deleteToken (save);
    processToken (token, st);
}

/*  Skips over a the mem-initializer-list of a ctor-initializer, defined as:
 *
 *  mem-initializer-list:
 *    mem-initializer, mem-initializer-list
 *
 *  mem-initializer:
 *    [::] [nested-name-spec] class-name (...)
 *    identifier
 */
void Parser_Cpp::skipMemIntializerList (tokenInfo *const token)
{
    int c;

    do
    {
        c = skipToNonWhite ();
        while (isident1 (c)  ||  c == ':')
        {
            if (c != ':')
                readIdentifier (token, c);
            c = skipToNonWhite ();
        }
        if (c == '<')
        {
            skipToMatch ("<>");
            c = skipToNonWhite ();
        }
        if (c == '(')
        {
            skipToMatch ("()");
            c = skipToNonWhite ();
        }
    } while (c == ',');
    cppUngetc (c);
}

void Parser_Cpp::skipMacro (statementInfo *const st)
{
    tokenInfo *const prev2 = prevToken (st, 2);

    if (isType (prev2, TOKEN_NAME))
        retardToken (st);
    skipToMatch ("()");
}

/*  Skips over characters following the parameter list. This will be either
 *  non-ANSI style function declarations or C++ stuff. Our choices:
 *
 *  C (K&R):
 *    int func ();
 *    int func (one, two) int one; float two; {...}
 *  C (ANSI):
 *    int func (int one, float two);
 *    int func (int one, float two) {...}
 *  C++:
 *    int foo (...) [const|volatile] [throw (...)];
 *    int foo (...) [const|volatile] [throw (...)] [ctor-initializer] {...}
 *    int foo (...) [const|volatile] [throw (...)] try [ctor-initializer] {...}
 *        catch (...) {...}
 */
boolean Parser_Cpp::skipPostArgumentStuff (
                statementInfo *const st, parenInfo *const info)
{
    tokenInfo *const token = activeToken (st);
    unsigned int parameters = info->parameterCount;
    unsigned int elementCount = 0;
    boolean restart = false;
    boolean end = false;
    int c = skipToNonWhite ();

    do
    {
        switch (c)
        {
            case ')':                               break;
            case ':': skipMemIntializerList (token);break;  /* ctor-initializer */
            case '[': skipToMatch ("[]");           break;
            case '=': cppUngetc (c); end = true;    break;
            case '{': cppUngetc (c); end = true;    break;
            case '}': cppUngetc (c); end = true;    break;

            case '(':
                if (elementCount > 0)
                    ++elementCount;
                skipToMatch ("()");
                break;

            case ';':
                if (parameters == 0  ||  elementCount < 2)
                {
                    cppUngetc (c);
                    end = true;
                }
                else if (--parameters == 0)
                    end = true;
                break;

            default:
                if (isident1 (c))
                {
                    readIdentifier (token, c);
                    switch (token->keyword)
                    {
                        case KEYWORD_ATTRIBUTE: skipParens ();  break;
                        case KEYWORD_THROW:     skipParens ();  break;
                        case KEYWORD_TRY:                       break;

                        case KEYWORD_CONST:
                        case KEYWORD_VOLATILE:
                            if (vStringLength (Signature) > 0)
                            {
                                vStringPut (Signature, ' ');
                                vStringCat (Signature, token->name);
                            }
                            break;

                        case KEYWORD_CATCH:
                        case KEYWORD_CLASS:
                        case KEYWORD_EXPLICIT:
                        case KEYWORD_EXTERN:
                        case KEYWORD_FRIEND:
                        case KEYWORD_INLINE:
                        case KEYWORD_MUTABLE:
                        case KEYWORD_NAMESPACE:
                        case KEYWORD_NEW:
                        case KEYWORD_NEWCOV:
                        case KEYWORD_OPERATOR:
                        case KEYWORD_OVERLOAD:
                        case KEYWORD_PRIVATE:
                        case KEYWORD_PROTECTED:
                        case KEYWORD_PUBLIC:
                        case KEYWORD_STATIC:
                        case KEYWORD_TEMPLATE:
                        case KEYWORD_TYPEDEF:
                        case KEYWORD_TYPENAME:
                        case KEYWORD_USING:
                        case KEYWORD_VIRTUAL:
                            /* Never allowed within parameter declarations. */
                            restart = true;
                            end = true;
                            break;

                        default:
                            if (isType (token, TOKEN_NONE))
                            ;
                            else if (info->isKnrParamList  &&  info->parameterCount > 0)
                                ++elementCount;
                            else
                            {
                                /*  If we encounter any other identifier immediately
                                 *  following an empty parameter list, this is almost
                                 *  certainly one of those Microsoft macro "thingies"
                                 *  that the automatic source code generation sticks
                                 *  in. Terminate the current statement.
                                 */
                                restart = true;
                                end = true;
                            }
                            break;
                    }
                }
        }
        if (! end)
        {
            c = skipToNonWhite ();
            if (c == EOF)
                end = true;
        }
    } while (! end);

    if (restart)
        restartStatement (st);
    else
        setToken (st, TOKEN_NONE);

    return (boolean) (c != EOF);
}

void Parser_Cpp::skipJavaThrows (statementInfo *const st)
{
    tokenInfo *const token = activeToken (st);
    int c = skipToNonWhite ();

    if (isident1 (c))
    {
        readIdentifier (token, c);
        if (token->keyword == KEYWORD_THROWS)
        {
            do
            {
                c = skipToNonWhite ();
                if (isident1 (c))
                {
                    readIdentifier (token, c);
                    c = skipToNonWhite ();
                }
            } while (c == '.'  ||  c == ',');
        }
    }
    cppUngetc (c);
    setToken (st, TOKEN_NONE);
}

void Parser_Cpp::analyzePostParens (statementInfo *const st, parenInfo *const info)
{
    //const unsigned long inputLineNumber = getInputLineNumber ();
    int c = skipToNonWhite ();

    cppUngetc (c);
    if (isOneOf (c, "{;,="))
        ;
    else if (isLanguage (Lang_java))
        skipJavaThrows (st);
    else
    {
        if (! skipPostArgumentStuff (st, info))
        {
            verbose (
                    "%s: confusing argument declarations beginning at line %lu\n",
                    getInputFileName (), inputLineNumber);
            longjmp (Exception, (int) ExceptionFormattingError);
        }
    }
}

boolean Parser_Cpp::languageSupportsGenerics (void)
{
    return (boolean) (isLanguage (Lang_cpp) || isLanguage (Lang_csharp) ||
                      isLanguage (Lang_java));
}

void Parser_Cpp::processAngleBracket (void)
{
    int c = cppGetc ();
    if (c == '>') {
        /* already found match for template */
    } else if (languageSupportsGenerics () && c != '<' && c != '=') {
        /* this is a template */
        cppUngetc (c);
        skipToMatch ("<>");
    } else if (c == '<') {
        /* skip "<<" or "<<=". */
        c = cppGetc ();
        if (c != '=') {
            cppUngetc (c);
        }
    } else {
        cppUngetc (c);
    }
}

void Parser_Cpp::parseJavaAnnotation (statementInfo *const st)
{
    /*
     * @Override
     * @Target(ElementType.METHOD)
     * @SuppressWarnings(value = "unchecked")
     *
     * But watch out for "@interface"!
     */
    tokenInfo *const token = activeToken (st);

    int c = skipToNonWhite ();
    readIdentifier (token, c);
    if (token->keyword == KEYWORD_INTERFACE)
    {
        /* Oops. This was actually "@interface" defining a new annotation. */
        processInterface (st);
    }
    else
    {
        /* Bug #1691412: skip any annotation arguments. */
        skipParens ();
    }
}

int Parser_Cpp::parseParens (statementInfo *const st, parenInfo *const info)
{
    tokenInfo *const token = activeToken (st);
    unsigned int identifierCount = 0;
    unsigned int depth = 1;
    boolean firstChar = true;
    int nextChar = '\0';

    CollectingSignature = true;
    vStringClear (Signature);
    vStringPut (Signature, '(');
    info->parameterCount = 1;
    do
    {
        int c = skipToNonWhite ();
        vStringPut (Signature, c);

        switch (c)
        {
            case '&':
            case '*':
                info->isPointer = true;
                info->isKnrParamList = false;
                if (identifierCount == 0)
                    info->isParamList = false;
                initToken (token);
                break;

            case ':':
                info->isKnrParamList = false;
                break;

            case '.':
                info->isNameCandidate = false;
                c = cppGetc ();
                if (c != '.')
                {
                   cppUngetc (c);
                    info->isKnrParamList = false;
                }
                else
                {
                    c =cppGetc ();
                    if (c != '.')
                    {
                       cppUngetc (c);
                        info->isKnrParamList = false;
                    }
                    else
                        vStringCatS (Signature, "..."); /* variable arg list */
                }
                break;

            case ',':
                info->isNameCandidate = false;
                if (info->isKnrParamList)
                {
                    ++info->parameterCount;
                    identifierCount = 0;
                }
                break;

            case '=':
                info->isKnrParamList = false;
                info->isNameCandidate = false;
                if (firstChar)
                {
                    info->isParamList = false;
                    skipMacro (st);
                    depth = 0;
                }
                break;

            case '[':
                info->isKnrParamList = false;
                skipToMatch ("[]");
                break;

            case '<':
               info->isKnrParamList = false;
               processAngleBracket ();
               break;

            case ')':
               if (firstChar)
                   info->parameterCount = 0;
               --depth;
               break;

            case '(':
               info->isKnrParamList = false;
               if (firstChar)
               {
                   info->isNameCandidate = false;
                  cppUngetc (c);
                   vStringClear (Signature);
                   skipMacro (st);
                   depth = 0;
                   vStringChop (Signature);
               }
               else if (isType (token, TOKEN_PAREN_NAME))
               {
                   c = skipToNonWhite ();
                   if (c == '*')        /* check for function pointer */
                   {
                       skipToMatch ("()");
                       c = skipToNonWhite ();
                       if (c == '(')
                           skipToMatch ("()");
                       else
                          cppUngetc (c);
                   }
                   else
                   {
                      cppUngetc (c);
                      cppUngetc ('(');
                       info->nestedArgs = true;
                   }
                }
                else
                    ++depth;
                break;

            default:
                if (c == '@' && isLanguage (Lang_java))
                {
                    parseJavaAnnotation(st);
                }
                else if (isident1 (c))
                {
                    if (++identifierCount > 1)
                        info->isKnrParamList = false;
                    readIdentifier (token, c);
                    if (isType (token, TOKEN_NAME)  &&  info->isNameCandidate)
                        token->type = TOKEN_PAREN_NAME;
                    else if (isType (token, TOKEN_KEYWORD))
                    {
                        if (token->keyword != KEYWORD_CONST &&
                            token->keyword != KEYWORD_VOLATILE)
                        {
                            info->isKnrParamList = false;
                            info->isNameCandidate = false;
                        }
                    }
                }
                else
                {
                    info->isParamList     = false;
                    info->isKnrParamList  = false;
                    info->isNameCandidate = false;
                    info->invalidContents = true;
                }
                break;
        }
        firstChar = false;
    } while (! info->nestedArgs  &&  depth > 0  &&
             (info->isKnrParamList  ||  info->isNameCandidate));

    if (! info->nestedArgs) while (depth > 0)
    {
        skipToMatch ("()");
        --depth;
    }

    if (! info->isNameCandidate)
        initToken (token);

    vStringTerminate (Signature);
    if (info->isKnrParamList)
        vStringClear (Signature);
    CollectingSignature = false;
    return nextChar;
}

void Parser_Cpp::initParenInfo (parenInfo *const info)
{
    info->isPointer				= false;
    info->isParamList			= true;
    info->isKnrParamList		= isLanguage (Lang_c);
    info->isNameCandidate		= true;
    info->invalidContents		= false;
    info->nestedArgs			= false;
    info->parameterCount		= 0;
}

void Parser_Cpp::analyzeParens (statementInfo *const st)
{
    tokenInfo *const prev = prevToken (st, 1);

    if (st->inFunction  &&  ! st->assignment)
        st->notVariable = true;
    if (! isType (prev, TOKEN_NONE))  /* in case of ignored enclosing macros */
    {
        tokenInfo *const token = activeToken (st);
        parenInfo info;
        int c;

        initParenInfo (&info);
        parseParens (st, &info);
        c = skipToNonWhite ();
        cppUngetc (c);
        if (info.invalidContents)
            reinitStatement (st, false);
        else if (info.isNameCandidate  &&  isType (token, TOKEN_PAREN_NAME)  &&
                 ! st->gotParenName  &&
                 (! info.isParamList || ! st->haveQualifyingName  ||
                  c == '('  ||
                  (c == '='  &&  st->implementation != IMP_VIRTUAL) ||
                  (st->declaration == DECL_NONE  &&  isOneOf (c, ",;"))))
        {
            token->type = TOKEN_NAME;
            processName (st);
            st->gotParenName = true;
            if (! (c == '('  &&  info.nestedArgs))
                st->isPointer = info.isPointer;
        }
        else if (! st->gotArgs  &&  info.isParamList)
        {
            st->gotArgs = true;
            setToken (st, TOKEN_ARGS);
            advanceToken (st);
            if (st->scope != SCOPE_TYPEDEF)
                analyzePostParens (st, &info);
        }
        else
            setToken (st, TOKEN_NONE);
    }
}

/*
*   Token parsing functions
*/

void Parser_Cpp::addContext (statementInfo *const st, const tokenInfo* const token)
{
    if (isType (token, TOKEN_NAME))
    {
        if (vStringLength (st->context->name) > 0)
        {
            if (isLanguage (Lang_c)  ||  isLanguage (Lang_cpp))
                vStringCatS (st->context->name, "::");
            else if (isLanguage (Lang_java) || isLanguage (Lang_csharp))
                vStringCatS (st->context->name, ".");
        }
        vStringCat (st->context->name, token->name);
        st->context->type = TOKEN_NAME;
    }
}

boolean Parser_Cpp::inheritingDeclaration (declType decl)
{
    /* C# supports inheritance for enums. C++0x will too, but not yet. */
    if (decl == DECL_ENUM)
    {
        return (boolean) (isLanguage (Lang_csharp));
    }
    return (boolean) (
            decl == DECL_CLASS ||
            decl == DECL_STRUCT ||
            decl == DECL_INTERFACE);
}

void Parser_Cpp::processColon (statementInfo *const st)
{
    int c = (isLanguage (Lang_cpp) ?cppGetc () : skipToNonWhite ());
    const boolean doubleColon = (boolean) (c == ':');

    if (doubleColon)
    {
        setToken (st, TOKEN_DOUBLE_COLON);
        st->haveQualifyingName = false;
    }
    else
    {
        cppUngetc (c);
        if ((isLanguage (Lang_cpp) || isLanguage (Lang_csharp))  &&
            inheritingDeclaration (st->declaration))
        {
            readParents (st, ':');
        }
        else if (parentDecl (st) == DECL_STRUCT)
        {
            c = skipToOneOf (",;");
            if (c == ',')
                setToken (st, TOKEN_COMMA);
            else if (c == ';')
                setToken (st, TOKEN_SEMICOLON);
        }
        else
        {
            const tokenInfo *const prev  = prevToken (st, 1);
            const tokenInfo *const prev2 = prevToken (st, 2);
            if (prev->keyword == KEYWORD_DEFAULT ||
                prev2->keyword == KEYWORD_CASE ||
                st->parent != NULL)
            {
                reinitStatement (st, false);
            }
        }
    }
}

/*  Skips over any initializing value which may follow an '=' character in a
 *  variable definition.
 */
int Parser_Cpp::skipInitializer (statementInfo *const st)
{
    boolean done = false;
    int c;

    while (! done)
    {
        c = skipToNonWhite ();

        if (c == EOF)
            longjmp (Exception, (int) ExceptionFormattingError);
        else switch (c)
        {
        case ',':
        case ';': done = true; break;

        case '0':
            if (st->implementation == IMP_VIRTUAL)
                st->implementation = IMP_PURE_VIRTUAL;
            break;

        case '[': skipToMatch ("[]"); break;
        case '(': skipToMatch ("()"); break;
        case '{': skipToMatch ("{}"); break;
        case '<': processAngleBracket(); break;

        case '}':
            if (insideEnumBody (st))
                done = true;
            else if (! isBraceFormat ())
            {
                verbose ("%s: unexpected closing brace at line %lu\n",
                         getInputFileName (), getInputLineNumber ());
                longjmp (Exception, (int) ExceptionBraceFormattingError);
            }
            break;

                        default: break;
                        }
    }
    return c;
}

void Parser_Cpp::processInitializer (statementInfo *const st)
{
    const boolean inEnumBody = insideEnumBody (st);
    int c =cppGetc ();

    if (c != '=')
    {
        cppUngetc (c);
        c = skipInitializer (st);
        st->assignment = true;
        if (c == ';')
            setToken (st, TOKEN_SEMICOLON);
        else if (c == ',')
            setToken (st, TOKEN_COMMA);
        else if (c == '}'  &&  inEnumBody)
        {
            cppUngetc (c);
            setToken (st, TOKEN_COMMA);
        }
        if (st->scope == SCOPE_EXTERN)
            st->scope = SCOPE_GLOBAL;
    }
}

void Parser_Cpp::parseIdentifier (statementInfo *const st, const int c)
{
    tokenInfo *const token = activeToken (st);

    readIdentifier (token, c);

    if (! isType (token, TOKEN_NONE))
        processToken (token, st);
}

void Parser_Cpp::parseGeneralToken (statementInfo *const st, const int c)
{
    const tokenInfo *const prev = prevToken (st, 1);

    if (isident1 (c) || (isLanguage (Lang_java) && isHighChar (c)))
    {
        parseIdentifier (st, c);
        if (isType (st->context, TOKEN_NAME) &&
            isType (activeToken (st), TOKEN_NAME) && isType (prev, TOKEN_NAME))
        {
            initToken (st->context);
        }
    }
    else if (c == '.' || c == '-')
    {
        if (! st->assignment)
            st->notVariable = true;
        if (c == '-')
        {
            int c2 =cppGetc ();
            if (c2 != '>')
                cppUngetc (c2);
        }
    }
    else if (c == '!' || c == '>')
    {
        int c2 =cppGetc ();
        if (c2 != '=')
            cppUngetc (c2);
    }
    else if (c == '@' && isLanguage (Lang_java))
    {
        parseJavaAnnotation (st);
    }
    else if (isExternCDecl (st, c))
    {
        st->declaration = DECL_NOMANGLE;
        st->scope = SCOPE_GLOBAL;
    }
}

/*  Reads characters from the pre-processor and assembles tokens, setting
 *  the current statement state.
 */
void Parser_Cpp::nextToken (statementInfo *const st)
{
    tokenInfo *token;
    do
    {
        int c = skipToNonWhite ();

        switch (c)
        {
            case EOF: longjmp (Exception, (int) ExceptionEOF);  break;
            case '(': analyzeParens (st);                       break;
            case '<': processAngleBracket ();                   break;
            case '*': st->haveQualifyingName = false;           break;
            case ',': setToken (st, TOKEN_COMMA);               break;
            case ':': processColon (st);                        break;
            case ';': setToken (st, TOKEN_SEMICOLON);           break;
            case '=': processInitializer (st);                  break;
            case '[': skipToMatch ("[]");                       break;
            case '{': setToken (st, TOKEN_BRACE_OPEN);          break;
            case '}': setToken (st, TOKEN_BRACE_CLOSE);         break;
            default:  parseGeneralToken (st, c);                break;
        }
        token = activeToken (st);
    } while (isType (token, TOKEN_NONE));
}

/*
*   Scanning support functions
*/



statementInfo *Parser_Cpp::newStatement (statementInfo *const parent)
{
    statementInfo *const st = xMalloc (1, statementInfo);
    unsigned int i;

    for (i = 0  ;  i < (unsigned int) NumTokens  ;  ++i)
        st->token [i] = newToken ();

    st->context = newToken ();
    st->blockName = newToken ();
    st->parentClasses = vStringNew ();

    initStatement (st, parent);
    CurrentStatement = st;

    return st;
}

void Parser_Cpp::deleteStatement (void)
{
    statementInfo *const st = CurrentStatement;
    statementInfo *const parent = st->parent;
    unsigned int i;

    for (i = 0  ;  i < (unsigned int) NumTokens  ;  ++i)
    {
        deleteToken (st->token [i]);       st->token [i] = NULL;
    }
    deleteToken (st->blockName);           st->blockName = NULL;
    deleteToken (st->context);             st->context = NULL;
    vStringDelete (st->parentClasses);     st->parentClasses = NULL;
    eFree (st);
    CurrentStatement = parent;
}

void Parser_Cpp::deleteAllStatements (void)
{
    while (CurrentStatement != NULL)
        deleteStatement ();
}

boolean Parser_Cpp::isStatementEnd (const statementInfo *const st)
{
    const tokenInfo *const token = activeToken (st);
    boolean isEnd;

    if (isType (token, TOKEN_SEMICOLON))
        isEnd = true;
    else if (isType (token, TOKEN_BRACE_CLOSE))
        /* Java and C# do not require semicolons to end a block. Neither do C++
                     * namespaces. All other blocks require a semicolon to terminate them.
                     */
        isEnd = (boolean) (isLanguage (Lang_java) || isLanguage (Lang_csharp) ||
                           ! isContextualStatement (st));
    else
        isEnd = false;

    return isEnd;
}

void Parser_Cpp::checkStatementEnd (statementInfo *const st)
{
    const tokenInfo *const token = activeToken (st);

    if (isType (token, TOKEN_COMMA))
        reinitStatement (st, true);
    else if (isStatementEnd (st))
    {
        DebugStatement ( if (debug (DEBUG_PARSE)) printf ("<ES>"); )
            reinitStatement (st, false);
        cppEndStatement ();
    }
    else
    {
        cppBeginStatement ();
        advanceToken (st);
    }
}

void Parser_Cpp::nest (statementInfo *const st, const unsigned int nestLevel)
{
    switch (st->declaration)
    {
        case DECL_CLASS:
        case DECL_ENUM:
        case DECL_INTERFACE:
        case DECL_NAMESPACE:
        case DECL_NOMANGLE:
        case DECL_STRUCT:
        case DECL_UNION:
            createTags (nestLevel, st);
            break;

        case DECL_FUNCTION:
        case DECL_TASK:
            st->inFunction = true;
            /* fall through */
        default:
            if (includeTag (TAG_LOCAL, false))
                createTags (nestLevel, st);
            else
                skipToMatch ("{}");
            break;
    }
    advanceToken (st);
    setToken (st, TOKEN_BRACE_CLOSE);
}

void Parser_Cpp::tagCheck (statementInfo *const st)
{
    const tokenInfo *const token = activeToken (st);
    const tokenInfo *const prev  = prevToken (st, 1);
    const tokenInfo *const prev2 = prevToken (st, 2);

    switch (token->type)
    {
        case TOKEN_NAME:
            if (insideEnumBody (st))
                qualifyEnumeratorTag (st, token);
            break;
//#if 0
//    case TOKEN_PACKAGE:
//        if (st->haveQualifyingName)
//            makeTag (token, st, false, TAG_PACKAGE);
//        break;
//#endif
        case TOKEN_BRACE_OPEN:
            if (isType (prev, TOKEN_ARGS))
            {
                if (st->haveQualifyingName)
                {
                    if (! isLanguage (Lang_vera))
                        st->declaration = DECL_FUNCTION;
                    if (isType (prev2, TOKEN_NAME))
                        copyToken (st->blockName, prev2);
                    qualifyFunctionTag (st, prev2);
                }
            }
            else if (isContextualStatement (st) ||
                     st->declaration == DECL_NAMESPACE ||
                     st->declaration == DECL_PROGRAM)
            {
                if (isType (prev, TOKEN_NAME))
                    copyToken (st->blockName, prev);
                else
                {
                    /*  For an anonymous struct or union we use a unique ID
                     *  a number, so that the members can be found.
                     */
                    char buf [20];  /* length of "_anon" + digits  + null */
                    sprintf (buf, "__anon%d", ++AnonymousID);
                    vStringCopyS (st->blockName->name, buf);
                    st->blockName->type = TOKEN_NAME;
                    st->blockName->keyword = KEYWORD_NONE;
                }
                qualifyBlockTag (st, prev);
            }
            else if (isLanguage (Lang_csharp))
                makeTag (prev, st, false, TAG_PROPERTY);
            break;

        case TOKEN_SEMICOLON:
        case TOKEN_COMMA:
            if (insideEnumBody (st))
            ;
            else if (isType (prev, TOKEN_NAME))
            {
                if (isContextualKeyword (prev2))
                    makeTag (prev, st, true, TAG_EXTERN_VAR);
                else
                    qualifyVariableTag (st, prev);
            }
            else if (isType (prev, TOKEN_ARGS)  &&  isType (prev2, TOKEN_NAME))
            {
                if (st->isPointer)
                    qualifyVariableTag (st, prev2);
                else
                    qualifyFunctionDeclTag (st, prev2);
            }
            if (isLanguage (Lang_java) && token->type == TOKEN_SEMICOLON && insideEnumBody (st))
            {
                /* In Java, after an initial enum-like part,
                 * a semicolon introduces a class-like part.
                 * See Bug #1730485 for the full rationale. */
                st->parent->declaration = DECL_CLASS;
            }
            break;

        default: break;
    }
}

/*  Parses the current file and decides whether to write out and tags that
 *  are discovered.
 */
void Parser_Cpp::createTags (const unsigned int nestLevel,
                                                statementInfo *const parent)
{
    statementInfo *const st = newStatement (parent);

    DebugStatement ( if (nestLevel > 0) debugParseNest (true, nestLevel); )
    while (true)
    {
        tokenInfo *token;

        nextToken (st);
        token = activeToken(st);

        if (isType (token, TOKEN_BRACE_CLOSE))
        {
            if (nestLevel > 0)
                break;
            else
            {
                verbose ("%s: unexpected closing brace at line %lu\n",
                         getInputFileName (), getInputLineNumber ());
                longjmp (Exception, (int) ExceptionBraceFormattingError);
            }
        }
        else if (isType (token, TOKEN_DOUBLE_COLON))
        {
            addContext (st, prevToken (st, 1));
            advanceToken (st);
        }
        else
        {
            tagCheck (st);
            if (isType (token, TOKEN_BRACE_OPEN))
                nest (st, nestLevel + 1);
            checkStatementEnd (st);
        }
    }
    deleteStatement ();
    DebugStatement ( if (nestLevel > 0) debugParseNest (false, nestLevel - 1); )
}

boolean Parser_Cpp::findCTags (const unsigned int passCount)
{
    exception_t exception;
    boolean retry;

    Assert (passCount < 3);
    cppInit ((boolean) (passCount > 1), isLanguage (Lang_csharp));
    Signature = vStringNew ();

    exception = (exception_t) setjmp (Exception);
    retry = false;
    if (exception == ExceptionNone) {
        createTags (0, NULL);
    }
    else
    {
        deleteAllStatements ();
        if (exception == ExceptionBraceFormattingError  &&  passCount == 1)
        {
            retry = true;
            verbose ("%s: retrying file with fallback brace matching algorithm\n",
                     getInputFileName ());
        }
    }
    vStringDelete (Signature);
    cppTerminate ();

    return retry;
}

void Parser_Cpp::buildKeywordHash (const langType language, unsigned int idx)
{
    const size_t count = sizeof (KeywordTable) / sizeof (KeywordTable [0]);
    size_t i;
    for (i = 0  ;  i < count  ;  ++i)
    {
        const keywordDesc* const p = &KeywordTable [i];
        if (p->isValid [idx])
            keywords.addKeyword (p->name, language, (int) p->id);
    }
}

void Parser_Cpp::initializeCParser (const langType language)
{
    Lang_c = language;
    buildKeywordHash (language, 0);
}

void Parser_Cpp::initializeCppParser (const langType language)
{
    Lang_cpp = language;
    buildKeywordHash (language, 1);
}

void Parser_Cpp::initializeCsharpParser (const langType language)
{
    Lang_csharp = language;
    buildKeywordHash (language, 2);
}

void Parser_Cpp::initializeJavaParser (const langType language)
{
    Lang_java = language;
    buildKeywordHash (language, 3);
}

void Parser_Cpp::initializeVeraParser (const langType language)
{
    Lang_vera = language;
    buildKeywordHash (language, 4);
}


/*****************************************************************************
 *****************************************************************************/
void Parser_Cpp::parse() {

    switch (language()) {
        case LanguageC:      initializeCParser(language());       break;
        case LanguageCpp:    initializeCppParser(language());     break;
        case LanguageCsharp: initializeCsharpParser(language());  break;
        case LanguageJava:   initializeJavaParser(language());    break;
        case LanguageVera:   initializeVeraParser(language());    break;
        default: return;
    }



    int passCount=1;

    while (findCTags(passCount)) {
        resetText();
        passCount++;
    }

}

#endif // PARSER_CPP
