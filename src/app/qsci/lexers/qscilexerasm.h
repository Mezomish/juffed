#ifndef QSCILEXERASM_H
#define QSCILEXERASM_H

#include <Qsci/qscilexer.h>

#define SCE_ASM_DEFAULT 0
#define SCE_ASM_COMMENT 1
#define SCE_ASM_NUMBER 2
#define SCE_ASM_STRING 3
#define SCE_ASM_OPERATOR 4
#define SCE_ASM_IDENTIFIER 5
#define SCE_ASM_CPUINSTRUCTION 6
#define SCE_ASM_MATHINSTRUCTION 7
#define SCE_ASM_REGISTER 8
#define SCE_ASM_DIRECTIVE 9
#define SCE_ASM_DIRECTIVEOPERAND 10
#define SCE_ASM_COMMENTBLOCK 11
#define SCE_ASM_CHARACTER 12
#define SCE_ASM_STRINGEOL 13
#define SCE_ASM_EXTINSTRUCTION 14

class QsciLexerASM : public QsciLexer
{
Q_OBJECT
public:
    explicit QsciLexerASM(QObject *parent = 0);
    virtual ~QsciLexerASM();

    //! Returns the name of the language.
    const char *language() const { return "Assembly"; }

    //! Returns the name of the lexer.  Some lexers support a number of
    //! languages.
    const char *lexer() const { return "asm"; }

    //! \internal Returns the string of characters that comprise a word.
    const char *wordCharacters() const;

    //! Returns the set of keywords for the keyword set \a set recognised by
    //! the lexer as a space separated string.
    const char *keywords(int set) const;

    //! Returns the descriptive name for style number \a style.  If the style
    //! is invalid for this language then an empty QString is returned.  This
    //! is intended to be used in user preference dialogs.
    QString description(int style) const;

    //! Returns the foreground colour of the text for a style.
    QColor defaultColor(int style) const;

    virtual const char * blockStartKeyword (int *style=0) const;
    virtual const char * blockStart (int *style=0) const;
    virtual const char * blockEnd (int *style=0) const;

signals:

public slots:

};

#endif // QSCILEXERASM_H
