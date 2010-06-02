#include <Qsci/qscilexerhaskell.h>


QsciLexerHaskell::QsciLexerHaskell(QObject *parent) :
    QsciLexer(parent)
{
}

const char * QsciLexerHaskell::keywords(int set) const
{
  // Keywords
  if (set == 1)
    return "as case class data default deriving do else hiding if import in "
        "infix infixl infixr instance let module newtype of proc qualified rec then type where";

  return 0;
}

QString QsciLexerHaskell::description(int style) const
{
  switch (style)
  {
  case SCE_HA_DEFAULT:
    return tr("Default");
  case SCE_HA_COMMENTLINE:
    return tr("Comment Line");
  case SCE_HA_COMMENTBLOCK:
    return tr("Comment Block 1");
  case SCE_HA_COMMENTBLOCK2:
    return tr("Comment Block 2");
  case SCE_HA_COMMENTBLOCK3:
    return tr("Comment Block 3");
  case SCE_HA_MODULE:
    return tr("Module");
  case SCE_HA_CHARACTER:
    return tr("Character");
  case SCE_HA_STRING:
    return tr("String");
  case SCE_HA_CAPITAL:
    return tr("Capital");
  case SCE_HA_IDENTIFIER:
    return tr("Identifier");
  case SCE_HA_CLASS:
    return tr("Class");
  case SCE_HA_NUMBER:
    return tr("Number");
  case SCE_HA_KEYWORD:
    return tr("Keyword");
  case SCE_HA_DATA:
    return tr("Data");
  case SCE_HA_IMPORT:
    return tr("Import");
  case SCE_HA_INSTANCE:
    return tr("Instance");
  case SCE_HA_OPERATOR:
    return tr("Operator");
  }

  return "";
}

QColor QsciLexerHaskell::defaultColor(int style) const
{
  switch (style)
  {
  case SCE_HA_DEFAULT:
    return Qt::black;

  case SCE_HA_CLASS:
  case SCE_HA_MODULE:
  case SCE_HA_INSTANCE:
    return Qt::darkRed;

  case SCE_HA_CAPITAL:
  case SCE_HA_DATA:
    return Qt::darkGray;

  case SCE_HA_COMMENTLINE:
  case SCE_HA_COMMENTBLOCK:
  case SCE_HA_COMMENTBLOCK2:
  case SCE_HA_COMMENTBLOCK3:
    return Qt::darkGreen;

  case SCE_HA_CHARACTER:
  case SCE_HA_STRING:
    return Qt::darkCyan;

  case SCE_HA_KEYWORD:
    return Qt::darkBlue;

  case SCE_HA_OPERATOR:
    return Qt::darkMagenta;

  case SCE_HA_IDENTIFIER:
    return Qt::magenta;

  case SCE_HA_IMPORT:
    return Qt::darkYellow;

  case SCE_HA_NUMBER:
    return Qt::blue;
  }

  return Qt::black;
}
