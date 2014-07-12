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


#ifndef __FUNC_LIST_PLUGIN_H__
#define __FUNC_LIST_PLUGIN_H__

#include <QObject>
#include <QHash>
#include <QAction>

#include <JuffPlugin.h>
#include "symbols/docsymbols.h"
#include "symbols/common.h"
#include "symbols/symboltreeview.h"

class QWidget;
class SymbolTreeView;
class QTreeWidgetItem;
class SymbolBrowser;

class JuffSymbolTreeView: public SymbolTreeView {
Q_OBJECT
public:
    JuffSymbolTreeView(SymbolBrowser *plugin, QWidget *parent);

protected:
    Language docLanguage(const QString &docName);
    void docText(const QString &docName, QString &text);

private:
    SymbolBrowser *mPlugin;
};


class SymbolBrowser : public QObject, public JuffPlugin {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
Q_INTERFACES(JuffPlugin)

friend class JuffSymbolTreeView;

public:
    SymbolBrowser();
    virtual ~SymbolBrowser();
    virtual void init();

    // Plugin information functions
    virtual QString name() const;
    virtual QString title() const;
    virtual QString description() const;
    virtual QString targetEngine() const;

    // Plugin controls functions
    virtual QWidgetList dockList() const;
    virtual Qt::DockWidgetArea dockPosition(QWidget*) const;

    QWidget* settingsPage() const;
    void applySettings();

private:
    QWidget * panel;
    SymbolTreeView * mView;

    bool mSettingsDetail;
    bool mSettingsSort;
    bool mSettingsExpand;
    bool mSettingsActivateOnSingleClick;

private slots:
    void onDocActivated(Juff::Document *document);
    void onDocClosed(Juff::Document *document);
    void onDocRenamed(Juff::Document *document, const QString& oldName);

    void settingsDetailCbkToggled(bool checked);
    void settingsSortCbkToggled(bool checked);
    void settingsExpandCbkToggled(bool checked);
    void settingsActivateOnSingleClickCbkToggled(bool checked);

    void skipToLine(int lineNum);

};

#endif
