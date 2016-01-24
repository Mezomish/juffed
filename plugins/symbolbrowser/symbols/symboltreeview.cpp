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

#include <QMenu>
#include <QDebug>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include "symboltreeview.h"

#define LineNumRole Qt::UserRole
#define SymbolNameRole LineNumRole+1
#define TIMER_DELAY 250

/*****************************************************************************
 *****************************************************************************/
SymbolTreeView::SymbolTreeView(QWidget *parent) : QTreeWidget(parent) {
    mSymbols = NULL;
    setFocusPolicy(Qt::StrongFocus);


    mActSorted = new QAction(tr("Sort by name"), this);
    mActSorted->setCheckable(true);
    mActSorted->setEnabled(false);
    connect(mActSorted, SIGNAL(toggled(bool)), this, SLOT(setSorted(bool)));


    mActDetailed = new QAction(tr("Display functions parameters"), this);
    mActDetailed->setCheckable(true);
    mActDetailed->setEnabled(false);
    connect(mActDetailed, SIGNAL(toggled(bool)), this, SLOT(setDetailed(bool)));


#ifdef DEBUG_TREE
    setColumnCount(2);
    setHeaderLabels(QStringList() << "Name" << "Line");
#else
    header()->hide();
#endif

    mRefreshTimer.setInterval(TIMER_DELAY);
    mRefreshTimer.setSingleShot(true);

    connect(&mRefreshTimer, SIGNAL(timeout()), this, SLOT(doRefresh()));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),    this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),   this, SLOT(onItemCollapsed(QTreeWidgetItem*)));

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onItemActivated(QTreeWidgetItem*,int)));

}


/*****************************************************************************
 *
 *****************************************************************************/
SymbolTreeView::~SymbolTreeView() {

}

/*****************************************************************************
 *
 *****************************************************************************/
QAction *SymbolTreeView::createRelationAction(const Symbol *symbol, QObject *parent) {
    QAction *act = new QAction(symbol->icon(), "", parent);
    switch (symbol->type()) {
        case Symbol::SymbolFunc:        act->setText(tr("Show the definition"));    break;
        //case Symbol::SymbolPrototype:   act->setText(tr("Show the declaration"));   break;
        default:                        act->setText(tr("Show the declaration"));   break;
    }

    act->setData(symbol->line());
    connect(act, SIGNAL(triggered()), this, SLOT(relatedMenuTriggered()));
    return act;
}

/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::contextMenuEvent(QContextMenuEvent *event) {
    if (!mSymbols) return;

    QMenu menu(this);

    QTreeWidgetItem *item = itemAt(event->pos());

    if (item) {
        Symbol *symbol = symbolByItem(item);

        if (symbol) {
            if (symbol->relatedSymbol())
                menu.addAction(createRelationAction(symbol->relatedSymbol(), &menu));            

            menu.addAction(createRelationAction(symbol, &menu));
            menu.addSeparator();
        }
    }

    menu.addAction(mActSorted);
    mActSorted->setChecked(mSymbols->sorted());

    menu.addAction(mActDetailed);
    mActDetailed->setChecked(mSymbols->detailed());

    menu.exec(event->globalPos());
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::relatedMenuTriggered() {
    QAction *act = qobject_cast<QAction*>(sender());
    if (!act)  return;

    bool ok;
    int lineNum = act->data().toInt(&ok);
    if (ok && lineNum > -1)
        emit skipToLine(lineNum);
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::docActivated(const QString &docName){

    if (mDocuments.contains(docName)){
        mSymbols = mDocuments.value(docName);
    }
    else {
        mSymbols = new DocSymbols();
        mDocuments.insert(docName, mSymbols);
        mSymbols->setDocName(docName);
        mSymbols->setDetailed(mSettingsDetail);
        mSymbols->setSorted(mSettingsSort);
        mSymbols->setAllExpanded(mSettingsExpand);
        connect(mSymbols, SIGNAL(changed()), this, SLOT(onSymbolsChanged()));
    }

    refresh();
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::docClosed(const QString &docName){
    DocSymbols *symb = mDocuments.value(docName);
    if (symb == mSymbols)
        mSymbols = NULL;

    delete symb;
    mDocuments.remove(docName);

    if (mDocuments.count() == 0) {
        clear();
        setEnabled(false);
    }
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::docRenamed(const QString &oldDocName, const QString newDocName) {
    DocSymbols *symb = mDocuments.value(oldDocName);
    mDocuments.remove(oldDocName);
    mDocuments[newDocName] = symb;
    symb->setDocName(newDocName);
    refresh();
}


/*****************************************************************************
 *****************************************************************************/
void SymbolTreeView::refresh() {
    mRefreshTimer.start();
}


/*****************************************************************************
 *****************************************************************************/
void SymbolTreeView::doRefresh() {
    setEnabled(mDocuments.count());

    if (! mSymbols) return;

    QString s;
    docText(mSymbols->docName(), s);
    mSymbols->refresh(s, docLanguage(mSymbols->docName()));

}


/*****************************************************************************
 *****************************************************************************/
void SymbolTreeView::setTreeItem(const Symbol *symbol, QTreeWidgetItem *treeItem){
    if (mSymbols->detailed())
        treeItem->setText(0, symbol->detailedText());
    else
        treeItem->setText(0, symbol->text());

    treeItem->setToolTip(0, symbol->detailedText());

    treeItem->setData(0, SymbolNameRole, symbol->name());
    treeItem->setData(0, LineNumRole, symbol->line());
    treeItem->setExpanded(symbol->expanded());
    treeItem->setIcon(0, symbol->icon());

#ifdef DEBUG_TREE
    if (symbol->line() > -1)
        treeItem->setText(1, QString("%1").arg(symbol->line() + 1 ));
#endif
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::onSymbolsChanged() {
    // Save selection
    QStringList selPath;
    QList<QTreeWidgetItem *> sel =  selectedItems();
    if (sel.length())
        getItemPath(sel.at(0), selPath);

    clear();
    int size = mSymbols->symbols()->size();

    for (int i=0; i<size; ++i){
        Symbol *node = mSymbols->symbols()->at(i);
        if (!node->hideIfEmpty() || node->count()) {
            QTreeWidgetItem * item = new QTreeWidgetItem(this);
            setTreeItem(node, item);

            rebuildChildren(node, item);
        }
    }

    mActDetailed->setEnabled(this->topLevelItemCount());
    mActSorted->setEnabled(mActDetailed->isEnabled());

    // Restore selection
    QTreeWidgetItem *selItem = itemByPath(selPath);
    if (selItem)
        selItem->setSelected(true);
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::getItemPath(const QTreeWidgetItem *item, QStringList &result) const {
    const QTreeWidgetItem *i = item;
    while (i) {
        result.prepend(i->data(0, SymbolNameRole).toString());
        i = i->parent();
    }
}


/*****************************************************************************
 *
 *****************************************************************************/
QTreeWidgetItem *SymbolTreeView::itemByPath(const QStringList &path) const {
    if (!path.count()) return NULL;        // Empty path, do nothing.
    if (!topLevelItemCount()) return NULL; // Empty tree, do nothing.

    // Find on the top level of the tree
    QTreeWidgetItem *item = NULL;
    QString str = path.at(0);
    for (int i=0; i<topLevelItemCount(); ++i) {
        if ( topLevelItem(i)->data(0, SymbolNameRole).toString() == str ) {
            item = topLevelItem(i);
            break;
        }
    }

    if (!item) return NULL; // Top level not found, exit.

    for (int i=1; i<path.count(); ++i) {
        str = path.at(i);

        bool found = false;
        for (int j=0; j < item->childCount(); ++j) {
            if ( item->child(j)->data(0, SymbolNameRole).toString() == str ) {
                item = item->child(j);
                found = true;
                break;
            }
        }
        if (!found) break;
    }

    return item;
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::rebuildChildren(const Symbol *parentSymbol, QTreeWidgetItem *parentTreeItem){
    int size = parentSymbol->size();

    for (int i=0; i<size; ++i){
        Symbol const *node = parentSymbol->at(i);
        if (!node->hideIfEmpty() || node->count()) {
            QTreeWidgetItem * item = new QTreeWidgetItem(parentTreeItem);
            setTreeItem(node, item);

            rebuildChildren(node, item);
        }
    }

}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::setSorted(bool sorted){
    if (!mSymbols) return;
    mSymbols->setSorted(sorted);
    refresh();
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::setDetailed(bool detailed){
    if (!mSymbols) return;
    mSymbols->setDetailed(detailed);
    refresh();
}


/*****************************************************************************
 *
 *****************************************************************************/
Symbol *SymbolTreeView::symbolByItem(QTreeWidgetItem *item){
    if (!mSymbols) return NULL;

    QStringList path;
    QTreeWidgetItem *i = item;
    while (i){
        path.prepend(i->data(0, SymbolNameRole).toString());
        i = i->parent();
    }

    return mSymbols->symbolByPath(path);
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::onItemExpanded(QTreeWidgetItem *item){
    Symbol *symb = symbolByItem(item);
    if (symb)
        symb->setExpanded(true);
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::onItemCollapsed(QTreeWidgetItem * item){
    Symbol *symb = symbolByItem(item);
    if (symb)
        symb->setExpanded(false);
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::onItemClicked(QTreeWidgetItem * item, int column){
    if (mSettingsActivateOnSingleClick)
        onItemActivated(item, column);
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolTreeView::onItemActivated(QTreeWidgetItem * item, int /*column*/){
    bool ok;
    int lineNum = item->data(0, LineNumRole).toInt(&ok);
    if (ok && lineNum > -1)
        emit skipToLine(lineNum);
}
