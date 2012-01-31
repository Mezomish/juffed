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


#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QString>
#include <QList>
#include <QIcon>

class Symbol: public QList<Symbol *> {
public:
    enum SymbolType {
        SymbolUnknown,
        SymbolClass,
        SymbolStruct,
        SymbolNamespace,
        SymbolFunc,
        SymbolPrototype
    };

    enum SortType{
        SortByName,
        SortByLine
    };

    Symbol(const Symbol::SymbolType type=SymbolUnknown, const QString &name="", Symbol *parent = NULL);
//    Symbol(Symbol *parent, Symbol * preceding);
    virtual ~Symbol();

    void clear();

    QString name() const;
    void setName(const QString &name);

    QString text() const;
    void setText(const QString &text);

    QString detailedText() const;
    void setDetailedText(const QString &text);

    int line() const;
    void setLine(int line);

    bool hideIfEmpty() const { return mHideIfEmpty; };
    void setHideIsEmpty(bool value);

    const QIcon icon() const;

    bool expanded() const;
    void setExpanded(bool expanded, bool recursive = false);

    Symbol* find(const QString &name) const;
    Symbol* find(const QString &name, SymbolType type) const;

    void sync(const Symbol *symbol);

    SymbolType type() const { return mType; };
    void setSymbolType(SymbolType value);

    void setParent(Symbol *parent);

    void sort(SortType sortType, bool recursive);

    Symbol *relatedSymbol() const { return mRelatedSymbol; };
    void setRelatedSymbol(Symbol *value);
private:
    Symbol  *mParent;
    QString mName;
    QString mText;
    QString mDetailedText;
    int     mLine;
    bool    mExpanded;
    SymbolType mType;
    bool    mHideIfEmpty;
    Symbol *mRelatedSymbol;

};

typedef QListIterator<Symbol *> SymbolIterator;
typedef QMutableListIterator<Symbol *> SymbolMutableIterator;

#endif // SYMBOL_H
