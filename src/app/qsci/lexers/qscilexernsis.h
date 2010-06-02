#ifndef QSCILEXERNSIS_H
#define QSCILEXERNSIS_H

#include <Qsci/qscilexer.h>

// located in SciLexer.h
#define SCLEX_NSIS 43

#define SCE_NSIS_DEFAULT 0
#define SCE_NSIS_COMMENT 1
#define SCE_NSIS_STRINGDQ 2
#define SCE_NSIS_STRINGLQ 3
#define SCE_NSIS_STRINGRQ 4
#define SCE_NSIS_FUNCTION 5
#define SCE_NSIS_VARIABLE 6
#define SCE_NSIS_LABEL 7
#define SCE_NSIS_USERDEFINED 8
#define SCE_NSIS_SECTIONDEF 9
#define SCE_NSIS_SUBSECTIONDEF 10
#define SCE_NSIS_IFDEFINEDEF 11
#define SCE_NSIS_MACRODEF 12
#define SCE_NSIS_STRINGVAR 13
#define SCE_NSIS_NUMBER 14
// ADDED for Scintilla v1.63
#define SCE_NSIS_SECTIONGROUP 15
#define SCE_NSIS_PAGEEX 16
#define SCE_NSIS_FUNCTIONDEF 17
#define SCE_NSIS_COMMENTBOX 18

class QsciLexerNSIS : public QsciLexer
{
Q_OBJECT
public:
    explicit QsciLexerNSIS(QObject *parent = 0);

    virtual ~QsciLexerNSIS();

    //! Returns the name of the language.
    const char *language() const { return "NSIS Script"; }

    //! Returns the name of the lexer.  Some lexers support a number of
    //! languages.
    const char *lexer() const { return "nsis"; }

    //! Returns the set of keywords for the keyword set \a set recognised by
    //! the lexer as a space separated string.
    const char *keywords(int set) const;

    //! Returns the descriptive name for style number \a style.  If the style
    //! is invalid for this language then an empty QString is returned.  This
    //! is intended to be used in user preference dialogs.
    QString description(int style) const;

    //! Returns the foreground colour of the text for a style.
    QColor defaultColor(int style) const;

};

#endif // QSCILEXERNSIS_H
