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

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


#ifndef DOCSYMBOLS_H
#define DOCSYMBOLS_H

#include <QObject>
#include <QStringList>
#include <QString>
#include "symbol.h"
#include "parserthread.h"
#include "common.h"

class DocSymbols : public QObject {
Q_OBJECT
public:
    DocSymbols();
    ~DocSymbols();
    Symbol  *symbols() const { return mSymbols; };

    void    refresh(const QString &text, const Language language);
    Symbol  *symbolByPath(const QStringList &path);

    bool detailed() const { return mDetailed; };
    void setDetailed(bool detailed);

    bool sorted() const { return mSorted; };
    void setSorted(bool sorted);

    bool allExpanded() const { return mAllExpanded; };
    void setAllExpanded(bool expandAll);

    const QString &docName() const { return mDocName; };
    void setDocName(const QString &value);
signals:
    void changed();

protected slots:
    void onParserFinished();

private:
    Symbol *mSymbols;
    bool mDetailed;
    bool mSorted;
    bool mAllExpanded;
    QString mDocName;
    ParserThread  *lastParser;

};

#endif // DOCSYMBOLS_H
