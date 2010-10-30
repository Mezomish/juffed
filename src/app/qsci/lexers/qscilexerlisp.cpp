#include "qscilexerlisp.h"

QsciLexerLisp::QsciLexerLisp(QObject *parent) :
    QsciLexer(parent)
{
}

const char * QsciLexerLisp::keywords(int set) const
{
  // Functions and operators
  if (set == 1)
    return "princ eval apply funcall quote complement backquote lambda set "
        "setq setf gensym make intern name value plist get getf putprop "
        "remprop make aref car cdr caar cadr cdar cddr caaar caadr cadar caddr "
        "cdaar cdadr cddar cdddr caaaar caaadr caadar caaddr cadaar cadadr caddar "
        "cadddr cdaaar cdaadr cdadar cdaddr cddaar cddadr cdddar cddddr cons append "
        "reverse last nth nthcdr member assoc subst sublis nsubst nsublis remove "
        "length mapc mapcar mapl mapcan mapcon rplaca rplacd nconc delete symbolp "
        "numberp boundp listp consp minusp zerop plusp evenp oddp eq eql equal "
        "cond let l prog prog1 prog2 progn dolist dotimes cerror errset backtrace "
        "evalhook truncate rem min max abs sin cos tan expt exp sqrt random logand "
        "logior logxor lognot bignums logeqv lognand lognor logorc2 logtest logbitp "
        "logcount length nil";

  // Keys
  if (set == 2)
    return "not defun + - * / = < > <= >= identity function defun defmacro "
        "hash array list maplist atom null case and or if go return do "
        "catch throw error break continue float integer nil symbol";

  return 0;
}

QString QsciLexerLisp::description(int style) const
{
  switch (style)
  {
  case SCE_LISP_DEFAULT:
    return tr("Default");
  case SCE_LISP_COMMENT:
    return tr("Comment");
  case SCE_LISP_MULTI_COMMENT:
    return tr("Multiline Comment");
  case SCE_LISP_SYMBOL:
    return tr("Symbol");
  case SCE_LISP_STRING:
    return tr("String");
  case SCE_LISP_STRINGEOL:
    return tr("Unfinished String");
  case SCE_LISP_IDENTIFIER:
    return tr("Identifier");
  case SCE_LISP_OPERATOR:
    return tr("Operator");
  case SCE_LISP_SPECIAL:
    return tr("Special");
  case SCE_LISP_NUMBER:
    return tr("Number");
  case SCE_LISP_KEYWORD:
    return tr("Keyword");
  case SCE_LISP_KEYWORD_KW:
    return tr("Keyword 2");
  }

  return "";
}

QColor QsciLexerLisp::defaultColor(int style) const
{
  switch (style)
  {
  case SCE_LISP_DEFAULT:
    return Qt::black;

  case SCE_LISP_COMMENT:
  case SCE_LISP_MULTI_COMMENT:
    return Qt::darkGreen;

  case SCE_LISP_SYMBOL:
    return Qt::darkCyan;

  case SCE_LISP_KEYWORD:
    return Qt::darkBlue;

  case SCE_LISP_KEYWORD_KW:
    return Qt::darkMagenta;

  case SCE_LISP_STRINGEOL:
    return Qt::red;

  case SCE_LISP_IDENTIFIER:
    return Qt::magenta;

  case SCE_LISP_OPERATOR:
    return Qt::darkYellow;

  case SCE_LISP_SPECIAL:
    return Qt::blue;

  case SCE_LISP_NUMBER:
    return Qt::blue;
  }

  return Qt::black;
}
