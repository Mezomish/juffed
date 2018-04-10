#ifndef QSCILEXERLISP_H
#define QSCILEXERLISP_H

#include <Qsci/qscilexer.h>

#define SCE_LISP_DEFAULT 0
#define SCE_LISP_COMMENT 1
#define SCE_LISP_NUMBER 2
#define SCE_LISP_KEYWORD 3
#define SCE_LISP_KEYWORD_KW 4
#define SCE_LISP_SYMBOL 5
#define SCE_LISP_STRING 6
#define SCE_LISP_STRINGEOL 8
#define SCE_LISP_IDENTIFIER 9
#define SCE_LISP_OPERATOR 10
#define SCE_LISP_SPECIAL 11
#define SCE_LISP_MULTI_COMMENT 12

class QsciLexerLisp : public QsciLexer
{
Q_OBJECT
public:
    explicit QsciLexerLisp(QObject *parent = 0);

    //! Returns the name of the language.
    const char *language() const { return "Lisp"; }

    //! Returns the name of the lexer.  Some lexers support a number of
    //! languages.
    const char *lexer() const { return "lisp"; }

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

#endif // QSCILEXERLISP_H
