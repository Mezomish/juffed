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

#include <QtCore>
#include <QWidget>
#include <QVBoxLayout>

#include <QTreeWidgetItem>

//#include <QHeaderView>
#include <QStringList>
#include <QMenu>

#include <QGroupBox>
#include <QCheckBox>

#include <Document.h>
#include <PluginSettings.h>
#include <EditorSettings.h>
#include "SymbolBrowser.h"


/*****************************************************************************
 *****************************************************************************/
JuffSymbolTreeView::JuffSymbolTreeView(SymbolBrowser *plugin, QWidget *parent): SymbolTreeView(parent) {
    mPlugin = plugin;
}


/*****************************************************************************
 *
 *****************************************************************************/
Language JuffSymbolTreeView::docLanguage(const QString &docName) {
    QString syntaxStr = mPlugin->api()->document(docName)->syntax();

    if (syntaxStr == "C++")         {

        if (QFileInfo(docName).suffix().toUpper() == "C")
            return LanguageC;
        else
            return LanguageCpp;
    }

    if (syntaxStr == "C#")          return LanguageCsharp;
    if (syntaxStr == "java")        return LanguageJava;

    if (syntaxStr == "Python")      return LanguagePython;
    if (syntaxStr == "Perl")      return LanguagePerl;
    if (syntaxStr == "Makefile")    return LanguageMakefile;

    if (syntaxStr == "none")        return LanguageUnknown;

    //qDebug() << QString("Unknown syntax: %1").arg(syntaxStr);
    return LanguageUnknown;

}


/*****************************************************************************
 *
 *****************************************************************************/
void JuffSymbolTreeView::docText(const QString &docName, QString &text) {
    mPlugin->api()->document(docName)->getText(text);
}



 /*********************************************************
  *
  *********************************************************/
SymbolBrowser::SymbolBrowser(): QObject(), JuffPlugin(), panel(NULL) {
    mSettingsActivateOnSingleClick = true;
    mSettingsDetail = true;
    mSettingsExpand = true;
    mSettingsSort = true;
}


/*********************************************************
 *
 *********************************************************/
SymbolBrowser::~SymbolBrowser(){
    if ( panel != NULL )
        delete panel;
}


/*****************************************************************************
 *
 *****************************************************************************/
void SymbolBrowser::init() {
    mSettingsDetail = PluginSettings::getBool(this, "Detail");
    mSettingsSort = PluginSettings::getBool(this, "Sort");
    mSettingsExpand = PluginSettings::getBool(this, "Expand");
    mSettingsActivateOnSingleClick = PluginSettings::getBool(this, "ActivateOnSingleClick");

    panel = new QWidget();
    panel->setWindowTitle(tr("Symbol browser"));


    mView = new JuffSymbolTreeView(this, panel);
    mView->setSettingsSort(mSettingsSort);
    mView->setSettingsDetail(mSettingsDetail);
    mView->setSettingsExpand(mSettingsExpand);
    mView->setSettingsActivateOnSingleClick(mSettingsActivateOnSingleClick);

    QPalette plt = mView->palette();
    plt.setColor(QPalette::Base, EditorSettings::get(EditorSettings::DefaultBgColor));
    plt.setColor(QPalette::Text, EditorSettings::get(EditorSettings::DefaultFontColor));
    mView->setPalette(plt);

    connect(mView, SIGNAL(skipToLine(int)), this, SLOT(skipToLine(int)));

    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->addWidget(mView);
    layout->setMargin(0);
    layout->setSpacing(2);

    panel->setLayout(layout);

    connect(api(), SIGNAL(docActivated(Juff::Document*)), this, SLOT(onDocActivated(Juff::Document*)));
    connect(api(), SIGNAL(docRenamed(Juff::Document*,QString)), this, SLOT(onDocRenamed(Juff::Document*,QString)));
    connect(api(), SIGNAL(docClosed(Juff::Document*)), this, SLOT(onDocClosed(Juff::Document*)));

    connect(api(), SIGNAL(docTextChanged(Juff::Document*)), mView, SLOT(refresh()));
    connect(api(), SIGNAL(docSyntaxChanged(Juff::Document*,QString)), mView, SLOT(refresh()));
}


/************************************************
 *
 ************************************************/
QString SymbolBrowser::name() const {
    return "Symbol_browser";
}


/************************************************
 *
 ************************************************/
QString SymbolBrowser::title() const {
    return tr("Symbol browser");
}


/************************************************
 *
 ************************************************/
QString SymbolBrowser::description() const {
    return "";
}


/************************************************
 *
 ************************************************/
QString SymbolBrowser::targetEngine() const {
    return "all";
}


/************************************************
 *
 ************************************************/
QWidgetList SymbolBrowser::dockList() const {
    QWidgetList wList;
    wList << panel;
    return wList;
}

Qt::DockWidgetArea SymbolBrowser::dockPosition(QWidget* w) const {
    if ( w == panel ) {
        return Qt::RightDockWidgetArea;
    }
    else {
        return JuffPlugin::dockPosition(w);
    }
}

/************************************************
 *
 ************************************************/
void SymbolBrowser::onDocActivated(Juff::Document *document) {
    if (!document->isNull())
        mView->docActivated(document->fileName());
}


/************************************************
 *
 ************************************************/
void SymbolBrowser::onDocClosed(Juff::Document *document) {
    mView->docClosed(document->fileName());
}


/************************************************
 *
 ************************************************/
void SymbolBrowser::onDocRenamed(Juff::Document *document, const QString& oldName) {
    mView->docRenamed(oldName, document->fileName());
}


/************************************************
 *
 ************************************************/
void SymbolBrowser::skipToLine(int lineNum) {
    api()->currentDocument()->gotoLine(lineNum);
}


/************************************************
 *
 ************************************************/
QWidget* SymbolBrowser::settingsPage() const {
    QWidget *panel = new QWidget();

    /* Default group .............................. */
    QGroupBox *groupDefault = new QGroupBox(panel);
    groupDefault->setTitle(tr("By default:"));

    QCheckBox *detailCbk = new QCheckBox(tr("Display functions parameters"), groupDefault);
    detailCbk->setChecked(mSettingsDetail);
    connect(detailCbk, SIGNAL(toggled(bool)), this, SLOT(settingsDetailCbkToggled(bool)));

    QCheckBox *sortCbk = new QCheckBox(tr("Sort by name"), groupDefault);
    sortCbk->setChecked(mSettingsSort);
    connect(sortCbk, SIGNAL(toggled(bool)), this, SLOT(settingsSortCbkToggled(bool)));

    QCheckBox *expandCbk = new QCheckBox(tr("Automatically expand nodes"), groupDefault);
    expandCbk->setChecked(mSettingsExpand);
    connect(expandCbk, SIGNAL(toggled(bool)), this, SLOT(settingsExpandCbkToggled(bool)));

    /* Behaviour group ............................ */
    QGroupBox *groupBehaviour = new QGroupBox(panel);
    groupBehaviour->setTitle(tr("Behaviour:"));

    QCheckBox *activateOnSingleClickCbk = new QCheckBox(tr("Activate on single click"), groupBehaviour);
    activateOnSingleClickCbk->setChecked(mSettingsActivateOnSingleClick);
    connect(activateOnSingleClickCbk, SIGNAL(toggled(bool)), this, SLOT(settingsActivateOnSingleClickCbkToggled(bool)));


    /* Layouts ************************************ */
    QVBoxLayout* groupDefaultLayout = new QVBoxLayout(groupDefault);
    groupDefaultLayout->addWidget(detailCbk);
    groupDefaultLayout->addWidget(sortCbk);
    groupDefaultLayout->addWidget(expandCbk);

    QVBoxLayout* groupBehaviourLayout = new QVBoxLayout(groupBehaviour);
    groupBehaviourLayout->addWidget(activateOnSingleClickCbk);

    QVBoxLayout* panelLayout = new QVBoxLayout(panel);
    panelLayout->addWidget(groupBehaviour);
    panelLayout->addWidget(groupDefault);
    panelLayout->addStretch();

    return panel;
}


/************************************************
 *
 ************************************************/
void SymbolBrowser::applySettings(){
    mView->setSettingsDetail(mSettingsDetail);
    PluginSettings::set(this, "Detail", mView->settingsDetail());

    mView->setSettingsSort(mSettingsSort);
    PluginSettings::set(this, "Sort", mView->settingsSort());

    mView->setSettingsExpand(mSettingsExpand);
    PluginSettings::set(this, "Expand", mView->settingsExpand());

    mView->setSettingsActivateOnSingleClick(mSettingsActivateOnSingleClick);
    PluginSettings::set(this, "ActivateOnSingleClick", mView->settingsActivateOnSingleClick());
}


/************************************************
 *
 ************************************************/
void SymbolBrowser::settingsDetailCbkToggled(bool checked){
    mSettingsDetail = checked;
}


/************************************************
 *
 ************************************************/
void SymbolBrowser::settingsSortCbkToggled(bool checked){
    mSettingsSort = checked;
}


/************************************************
 *
 ************************************************/
void SymbolBrowser::settingsExpandCbkToggled(bool checked){
    mSettingsExpand = checked;
}

/************************************************
 *
 ************************************************/
void SymbolBrowser::settingsActivateOnSingleClickCbkToggled(bool checked){
    mSettingsActivateOnSingleClick = checked;
}


Q_EXPORT_PLUGIN2(symbolbrowser, SymbolBrowser)
