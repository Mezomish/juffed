#ifndef QSCILEXERADA_H
#define QSCILEXERADA_H

#include <Qsci/qscilexer.h>

#define SCE_ADA_DEFAULT 0
#define SCE_ADA_WORD 1
#define SCE_ADA_IDENTIFIER 2
#define SCE_ADA_NUMBER 3
#define SCE_ADA_DELIMITER 4
#define SCE_ADA_CHARACTER 5
#define SCE_ADA_CHARACTEREOL 6
#define SCE_ADA_STRING 7
#define SCE_ADA_STRINGEOL 8
#define SCE_ADA_LABEL 9
#define SCE_ADA_COMMENTLINE 10
#define SCE_ADA_ILLEGAL 11

class QsciLexerAda : public QsciLexer
{
Q_OBJECT
public:
    explicit QsciLexerAda(QObject *parent = 0);

    //! Returns the name of the language.
    const char *language() const { return "Ada"; }

    //! Returns the name of the lexer.  Some lexers support a number of
    //! languages.
    const char *lexer() const { return "ada"; }

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

#endif // QSCILEXERADA_H
