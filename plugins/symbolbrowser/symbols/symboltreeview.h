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


#ifndef SYMBOLTREEVIEW_H
#define SYMBOLTREEVIEW_H

#include <QWidget>
#include <QHash>
#include <QTreeWidget>
#include <QTimer>
#include <QStringList>
#include "symbols/common.h"
#include "symbols/docsymbols.h"


class QAction;

class SymbolTreeView : public QTreeWidget
{
Q_OBJECT
public:
    explicit SymbolTreeView(QWidget *parent = 0);
    virtual ~SymbolTreeView();

    bool settingsDetail() const { return mSettingsDetail; };
    void setSettingsDetail(bool value) { mSettingsDetail = value; };

    bool settingsSort() const { return mSettingsSort; };
    void setSettingsSort(bool value) { mSettingsSort = value; };

    bool settingsExpand() const { return mSettingsExpand; };
    void setSettingsExpand(bool value) { mSettingsExpand = value; };

    bool settingsActivateOnSingleClick() const { return mSettingsActivateOnSingleClick; };
    void setSettingsActivateOnSingleClick(bool value) { mSettingsActivateOnSingleClick = value; };

public slots:
    void docActivated(const QString &docName);
    void docClosed(const QString &docName);
    void docRenamed(const QString &oldDocName, const QString newDocName);

    void refresh();

    void setDetailed(bool sorted);
    void setSorted(bool sorted);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    //void timerEvent(QTimerEvent * event );

    virtual Language docLanguage(const QString &docName) = 0;
    virtual void docText(const QString &docName, QString &text) = 0;

private:
    QHash<QString, DocSymbols *>  mDocuments;
    DocSymbols * mSymbols;

    QAction *mActDetailed;
    QAction *mActSorted;

    int mTimerId;
    QTimer mRefreshTimer;

    bool mSettingsDetail;
    bool mSettingsSort;
    bool mSettingsExpand;
    bool mSettingsActivateOnSingleClick;

    void rebuildChildren(const Symbol *parentSymbol, QTreeWidgetItem *parentTreeItem);
    void setTreeItem(const Symbol *symbol, QTreeWidgetItem *treeItem);
    Symbol *symbolByItem(QTreeWidgetItem *item);
    QAction *createRelationAction(const Symbol *symbol, QObject *parent);

    void getItemPath(const QTreeWidgetItem *item, QStringList &result) const;
    QTreeWidgetItem *itemByPath(const QStringList &path) const;

signals:
    void skipToLine(int lineNum);

private slots:
    void doRefresh();
    void onSymbolsChanged();

    void onItemExpanded(QTreeWidgetItem *item);
    void onItemCollapsed(QTreeWidgetItem * item);
    void onItemClicked(QTreeWidgetItem * item, int column );
    void onItemActivated(QTreeWidgetItem * item, int column );
    void relatedMenuTriggered();

};

#endif // SYMBOLTREEVIEW_H
