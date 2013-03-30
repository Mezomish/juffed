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
#include "docsymbols.h"


/************************************************
 *
 ************************************************/
DocSymbols::DocSymbols(): QObject() {
    mSymbols = new Symbol();
    mSorted = false;
    mDetailed = false;
    lastParser = NULL;
}


/************************************************
 *
 ************************************************/
DocSymbols::~DocSymbols(){
    delete mSymbols;
}


/************************************************
 *
 ************************************************/
void DocSymbols::refresh(const QString &text, const  Language language) {
    if (lastParser)
        lastParser->stop();

    lastParser = new ParserThread();
    connect(lastParser, SIGNAL(finished()), this, SLOT(onParserFinished()));

    lastParser->setLanguage(language);
    lastParser->setText(text);
    lastParser->start();
}


/************************************************
 *
 ************************************************/
void DocSymbols::onParserFinished() {
    ParserThread *parser = qobject_cast<ParserThread*>(sender());

    if (! parser ) return;

    if (parser == lastParser) {
        Symbol *result = parser->symbols();
        if (mAllExpanded)
            result->setExpanded(true, true);
        else
            result->sync(mSymbols); // Copy expanded state.

        mSymbols->clear();
        while (!result->isEmpty()) {
            mSymbols->prepend(result->takeLast());
        }

        if (mSorted) {
            mSymbols->sort(Symbol::SortByName, true);
        }
        emit changed();
        lastParser = NULL;
    }

    delete parser;

}


/************************************************
 *
 ************************************************/
Symbol  *DocSymbols::symbolByPath(const QStringList &path){
    Symbol *symb=mSymbols;
    foreach (QString s, path){
        symb = symb->find(s);
        if (! symb) return NULL;
    }

    return symb;
}



/************************************************
 *
 ************************************************/
void DocSymbols::setDetailed(bool detailed) {
    mDetailed = detailed;
}


/************************************************
 *
 ************************************************/
void DocSymbols::setSorted(bool sorted) {
    mSorted = sorted;
}


/************************************************
 *
 ************************************************/
void DocSymbols::setAllExpanded(bool allExpanded) {
    mAllExpanded = allExpanded;
}


/************************************************
 *
 ************************************************/
void DocSymbols::setDocName(const QString &value) {
    mDocName = value;
}
