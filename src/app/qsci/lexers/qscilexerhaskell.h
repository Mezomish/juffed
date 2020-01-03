#ifndef QSCILEXERHASKELL_H
#define QSCILEXERHASKELL_H

#include <Qsci/qscilexer.h>

#define SCE_HA_DEFAULT 0
#define SCE_HA_IDENTIFIER 1
#define SCE_HA_KEYWORD 2
#define SCE_HA_NUMBER 3
#define SCE_HA_STRING 4
#define SCE_HA_CHARACTER 5
#define SCE_HA_CLASS 6
#define SCE_HA_MODULE 7
#define SCE_HA_CAPITAL 8
#define SCE_HA_DATA 9
#define SCE_HA_IMPORT 10
#define SCE_HA_OPERATOR 11
#define SCE_HA_INSTANCE 12
#define SCE_HA_COMMENTLINE 13
#define SCE_HA_COMMENTBLOCK 14
#define SCE_HA_COMMENTBLOCK2 15
#define SCE_HA_COMMENTBLOCK3 16

class QsciLexerHaskell : public QsciLexer
{
Q_OBJECT
public:
    explicit QsciLexerHaskell(QObject *parent = 0);

    //! Returns the name of the language.
    const char *language() const { return "Haskell"; }

    //! Returns the name of the lexer.  Some lexers support a number of
    //! languages.
    const char *lexer() const { return "haskell"; }

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

#endif
