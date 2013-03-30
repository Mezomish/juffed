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
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
***************************************************************************/


#include <QDebug>
#include "symbol.h"
#include <QtAlgorithms>


/*****************************************************************************
 *
 *****************************************************************************/
Symbol::Symbol(const Symbol::SymbolType type, const QString &name, Symbol *parent): QList<Symbol *>() {
    mType = type;
    mName = QString(name);
    mLine = -1;
    mExpanded = false;
    mHideIfEmpty = false;
    mParent = NULL;
    setParent(parent);
    mRelatedSymbol = NULL;
}


/*****************************************************************************
 *
 *****************************************************************************/
//Symbol::Symbol(Symbol *parent, Symbol * preceding): QList<Symbol *>() {
//    if (parent){
//        int n = parent->indexOf(preceding);
//        parent->insert(n+1, this);
//    }
//}


/*****************************************************************************
 *
 *****************************************************************************/
Symbol::~Symbol() {
    qDeleteAll(begin(), end());
    delete mRelatedSymbol;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::clear() {
    qDeleteAll(begin(), end());
    QList<Symbol*>::clear();
}


/*****************************************************************************
 *
 *****************************************************************************/
QString Symbol::name() const {
    return mName;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setName(const QString &name) {
    mName = QString(name);
}


/*****************************************************************************
 *
 *****************************************************************************/
QString Symbol::text() const {
    if (!mText.isEmpty())
        return mText;
    else
        return mName;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setText(const QString &text) {
    mText = QString(text);
}


/*****************************************************************************
 *
 *****************************************************************************/
QString Symbol::detailedText() const {
    if (!mDetailedText.isEmpty())
        return mDetailedText;
    else
        return text();
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setDetailedText(const QString &text) {
    mDetailedText = QString(text);
}


/*****************************************************************************
 *
 *****************************************************************************/
int Symbol::line() const {
    return mLine;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setLine(int line) {
    mLine = line;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setHideIsEmpty(bool value) {
    mHideIfEmpty = value;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setSymbolType(SymbolType value) {
    mType = value;
}


/*****************************************************************************
 *
 *****************************************************************************/
const QIcon Symbol::icon() const{

    switch (mType) {
        case Symbol::SymbolClass:       return QIcon(":icon_class");
        case Symbol::SymbolStruct:      return QIcon(":icon_class");
        case Symbol::SymbolFunc:        return QIcon(":icon_func");
//        case Symbol::SymbolFunc:
//        if (mRelatedSymbol)         return QIcon(":icon_func+proto");
//        else                        return QIcon(":icon_func");

        case Symbol::SymbolNamespace:   return QIcon(":icon_namespace");
        case Symbol::SymbolPrototype:   return QIcon(":icon_func");
        case Symbol::SymbolUnknown:     return QIcon();
    }

    return QIcon();
}


/*****************************************************************************
 *
 *****************************************************************************/
bool Symbol::expanded() const {
    return mExpanded;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setExpanded(bool expanded, bool recursive /*= false*/) {
    mExpanded = expanded;
    if (recursive){
        int size = this->size();
        for (int i=0; i<size; ++i)
            at(i)->setExpanded(expanded, recursive);
    }
}


/*****************************************************************************
 *
 *****************************************************************************/
Symbol* Symbol::find(const QString &name) const {
    int size = this->size();
    for (int i=0; i<size; ++i){
        Symbol *s = at(i);
        if (s->name() == name)
            return s;
    }

    return NULL;
}


/*****************************************************************************
 *
 *****************************************************************************/
Symbol* Symbol::find(const QString &name, SymbolType type) const {
    int size = this->size();
    for (int i=0; i<size; ++i){
        Symbol *s = at(i);
        if ((s->name() == name) && (s->type() == type))
            return s;
    }

    return NULL;
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::sync(const Symbol *symbol){
    setExpanded(symbol->expanded());

    int sz = size();
    for (int i=0; i<sz; ++i){
        Symbol *m = at(i);
        const Symbol *s = symbol->find(m->name());
        if (s){
            m->sync(s);
        }

    }

}


/*****************************************************************************
 *
 *****************************************************************************/
bool lesThenName(const Symbol *s1, const Symbol *s2){
    return s1->name() < s2->name();
}


/*****************************************************************************
 *
 *****************************************************************************/
bool lesThenLine(const Symbol *s1, const Symbol *s2){
    return s1->line() < s2->line();
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setParent(Symbol *parent) {
    int n;
    if (mParent && (n=mParent->indexOf(this)) > -1 )
        mParent->takeAt(n);

    mParent = parent;

    if (mParent)
        mParent->append(this);
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::sort(SortType sortType, bool recursive){
    switch (sortType) {
        case SortByLine: qSort(this->begin(), this->end(), lesThenLine);
        case SortByName: qSort(this->begin(), this->end(), lesThenName);
    }

    if (recursive) {
        int len = count();
        for(int i=0; i<len; ++i)
            at(i)->sort(sortType, recursive);
    }
}


/*****************************************************************************
 *
 *****************************************************************************/
void Symbol::setRelatedSymbol(Symbol *value) {
    mRelatedSymbol = value;
}
