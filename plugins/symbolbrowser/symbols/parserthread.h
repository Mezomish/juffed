/***************************************************************************
  A SymbolBrowser plugin for JuffEd editor.

  Copyright: 2010 Aleksander Sokoloff <asokol@mail.ru>

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


#ifndef PARSERTHREAD_H
#define PARSERTHREAD_H

#include <QThread>
#include "common.h"
#include "symbol.h"

class Parser;

class ParserThread : public QThread
{
Q_OBJECT
public:
    ParserThread();
    ~ParserThread();
    Symbol *symbols() const { return mSymbols; };

  //  QString text() const { return mText; }
    void setText(const QString &text);

    Language language() const { return mLanguage; };
    void setLanguage(Language language);
    void stop();
protected:
    Symbol *mSymbols;
    void run();

private:
//    QString mText;
    Language mLanguage;
    char *mTextBuf;
    int mTextBufLen;
    Parser *mParser;
};

#endif // PARSERTHREAD_H
