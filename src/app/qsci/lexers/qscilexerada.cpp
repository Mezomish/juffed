#include <Qsci/qscilexerada.h>


QsciLexerAda::QsciLexerAda(QObject *parent) :
    QsciLexer(parent)
{
}

const char * QsciLexerAda::keywords(int set) const
{
  // Keywords
  if (set == 1)
    return "abort else new return abs elsif not reverse abstract end null accept entry select access exception of separate aliased exit or subtype all others synchronized and for out array function overriding tagged at task generic package terminate begin goto pragma then body private type if procedure case in protected until constant interface use is raise declare range when delay limited record while delta loop rem with digits renames do mod requeue xor";

  return 0;
}

QString QsciLexerAda::description(int style) const
{
  switch (style)
  {
  case SCE_ADA_DEFAULT:
    return tr("Default");
  case SCE_ADA_COMMENTLINE:
    return tr("Comment");
  case SCE_ADA_ILLEGAL:
    return tr("Illegal");
  case SCE_ADA_CHARACTER:
    return tr("Character");
  case SCE_ADA_CHARACTEREOL:
    return tr("Unfinished Character");
  case SCE_ADA_STRING:
    return tr("String");
  case SCE_ADA_STRINGEOL:
    return tr("Unfinished String");
  case SCE_ADA_IDENTIFIER:
    return tr("Identifier");
  case SCE_ADA_LABEL:
    return tr("Label");
  case SCE_ADA_NUMBER:
    return tr("Number");
  case SCE_ADA_WORD:
    return tr("Keyword");
  case SCE_ADA_DELIMITER:
    return tr("Delimiter");
  }

  return "";
}

QColor QsciLexerAda::defaultColor(int style) const
{
  switch (style)
  {
  case SCE_ADA_DEFAULT:
    return Qt::black;

  case SCE_ADA_COMMENTLINE:
    return Qt::darkGreen;

  case SCE_ADA_CHARACTER:
  case SCE_ADA_STRING:
    return Qt::darkCyan;

  case SCE_ADA_WORD:
    return Qt::darkBlue;

  case SCE_ADA_DELIMITER:
    return Qt::darkMagenta;

  case SCE_ADA_STRINGEOL:
  case SCE_ADA_CHARACTEREOL:
  case SCE_ADA_ILLEGAL:
    return Qt::red;

  case SCE_ADA_IDENTIFIER:
    return Qt::magenta;

  case SCE_ADA_LABEL:
    return Qt::darkYellow;

  case SCE_ADA_NUMBER:
    return Qt::blue;
  }

  return Qt::black;
}
