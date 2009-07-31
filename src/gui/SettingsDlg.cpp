#include <QDebug>

/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "SettingsDlg.h"

//	Qt headers
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

//	Local headers
#ifdef Q_OS_WIN
#include "AppInfo.win.h"
#else
#include "AppInfo.h"
#endif

#include "ColorButton.h"
#include "IconManager.h"
#include "MultiPage.h"
#include "MainSettings.h"
#include "PluginManager.h"
#include "PluginSettings.h"
#include "TextDocSettings.h"
#include "AutocompleteSettings.h"
#include "SettingsItem.h"

#include "ui_MainSettingsPage.h"

#include <juffed/SettingsCheckItem.h>

class MainSettingsPage : public QWidget {
public:
	MainSettingsPage() : QWidget() {
		JUFFENTRY;
		ui.setupUi(this);
	}
	Ui::MainSettingsPage ui;
};

#include "ui_ViewSettingsPage.h"

class ViewSettingsPage : public QWidget {
public:
	ViewSettingsPage() : QWidget() {
		JUFFENTRY;
		ui.setupUi(this);
	}
	Ui::ViewSettingsPage ui;
};

#include "ui_EditorSettingsPage.h"

class EditorSettingsPage : public QWidget {
public:
	EditorSettingsPage() : QWidget () {
		JUFFENTRY;
	
		ui.setupUi(this);

		//	Creating ColorButton extensions. We shouldn't delete them
		//	manually 'cause they will be deleted automatically when their 
		//	parent buttons are deleted
		curLineColorBtn_ = new ColorButton(ui.curLineColorBtn, TextDocSettings::curLineColor());
		markersColorBtn_ = new ColorButton(ui.markerColorBtn, TextDocSettings::markersColor());
		fontColorBtn_ = new ColorButton(ui.fontColorBtn, TextDocSettings::defaultFontColor());
		bgColorBtn_ = new ColorButton(ui.bgColorBtn, TextDocSettings::defaultBgColor());
		braceColorBtn_ = new ColorButton(ui.braceColorBtn, TextDocSettings::matchedBraceBgColor());
		indentsColorBtn_ = new ColorButton(ui.indentColorBtn, TextDocSettings::indentsColor());
		selectionBgColorBtn_ = new ColorButton(ui.selectionBgColorBtn, TextDocSettings::selectionBgColor());
	}
	
	Ui::EditorSettingsPage ui;
	ColorButton* curLineColorBtn_;
	ColorButton* markersColorBtn_;
	ColorButton* fontColorBtn_;
	ColorButton* bgColorBtn_;
	ColorButton* braceColorBtn_;
	ColorButton* indentsColorBtn_;
	ColorButton* selectionBgColorBtn_;
};

#include "ui_AutocompleteSettingsPage.h"

class AutocompleteSettingsPage : public QWidget {
public:
	AutocompleteSettingsPage() : QWidget() {
		ui.setupUi(this);
	}
	
	Ui::AutocompletePage ui;
};

#include "FileTypesPage.h"
#include "CharsetsSettingsPage.h"
#include "PluginPage.h"
#include "JuffPlugin.h"

/////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg(QWidget* parent) : QDialog(parent) {
	JUFFENTRY;

	setWindowTitle(tr("Settings"));

	//	create buttons
	okBtn_ = new QPushButton(tr("OK"), this);
	applyBtn_ = new QPushButton(tr("Apply"), this);
	cancelBtn_ = new QPushButton(tr("Cancel"), this);
	connect(okBtn_, SIGNAL(clicked()), SLOT(ok()));
	connect(applyBtn_, SIGNAL(clicked()), SLOT(apply()));
	connect(cancelBtn_, SIGNAL(clicked()), SLOT(reject()));
	//	create multipage
	mp_ = new MultiPage();
	pageMain_ = new MainSettingsPage();
	pageView_ = new ViewSettingsPage();
	pageEditor_ = new EditorSettingsPage();
	pageCharsets_ = new CharsetsSettingsPage();
	pageAC_ = new AutocompleteSettingsPage();
	fileTypesPage_ = new FileTypesPage();
	mp_->addPage(tr("General"), pageMain_);
	mp_->addPage(tr("View"), pageView_);
	mp_->addPage(tr("Editor"), pageEditor_);
	mp_->addPage(tr("Autocompletion"), pageAC_);
	mp_->addPage(tr("Charsets"), pageCharsets_);
	mp_->addPage(tr("File types"), fileTypesPage_);

	//	plugins
	pluginsMainPage_ = new QWidget();
	mp_->addPage(tr("Plugins"), pluginsMainPage_);

	// layouts
	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	btnLayout->addWidget(okBtn_);
	btnLayout->addWidget(applyBtn_);
	btnLayout->addWidget(cancelBtn_);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(mp_);
	mainLayout->addLayout(btnLayout);
	setLayout(mainLayout);
	
	init();
}

SettingsDlg::~SettingsDlg() {
	items_.clear();
	delete pageCharsets_;
	delete pageEditor_;
	delete pageMain_;
	delete pageAC_;
	delete fileTypesPage_;
	delete pageView_;
	delete pluginsMainPage_;
	delete mp_;
}

void SettingsDlg::init() {
	JUFFENTRY;
	
	//	icon themes
	JUFFDEBUG("Initialization: icon theme");
	QStringList themes = IconManager::instance()->themeList();
	pageView_->ui.iconThemeCmb->clear();
	pageView_->ui.iconThemeCmb->addItem("<default>");
	pageView_->ui.iconThemeCmb->addItems(themes);
	
	QString iconTheme = MainSettings::iconTheme();
	if ( iconTheme.isEmpty() || !themes.contains(iconTheme) ) {
		pageView_->ui.iconThemeCmb->setCurrentIndex(pageView_->ui.iconThemeCmb->findText("<default>"));
	}
	else {
		pageView_->ui.iconThemeCmb->setCurrentIndex(pageView_->ui.iconThemeCmb->findText(iconTheme));
	}

	JUFFDEBUG("Initialization: GUI settings");
	int tabPos = MainSettings::tabPosition();
	pageView_->ui.tabPositionCmb->setCurrentIndex(tabPos);
	
	int style = MainSettings::toolButtonStyle();
	pageView_->ui.toolButtonStyleCmb->setCurrentIndex(style);
	pageView_->ui.iconSizeCmb->setCurrentIndex(MainSettings::iconSize());
	

	//	Main page
	JUFFDEBUG("Initialization: main page");
	int startupVariant = MainSettings::startupVariant();
	switch (startupVariant) {
		case 1:
			pageMain_->ui.openLastSessionBtn->setChecked(true);
			break;

		case 2:
			pageMain_->ui.openEmptySessionBtn->setChecked(true);
			break;

		case 0:
		default:
			pageMain_->ui.showSessionDlgBtn->setChecked(true);
	}

	items_ << new SettingsCheckItem("main", "saveSessionOnClose", pageMain_->ui.saveSessionChk, true)
	       << new SettingsCheckItem("main", "exitOnLastDocClosed", pageMain_->ui.exitOnLastDocClosedChk, false)
	       << new SettingsCheckItem("main", "syncOpenDialogToCurDoc", pageMain_->ui.syncOpenDlgChk, true)
	       << new SettingsCheckItem("main", "makeBackupOnSave", pageMain_->ui.makeBackupChk, true)
	       << new SettingsCheckItem("main", "stripTrailingSpaces", pageMain_->ui.stripSpacesChk, false)
	       << new SettingsCheckItem("main", "singleInstance", pageMain_->ui.singleInstanceChk, true)
	;
#ifndef Q_OS_UNIX
	pageMain_->ui.singleInstanceChk->hide();
#endif

#if QT_VERSION >= 0x040500
	items_ << new SettingsCheckItem("main", "closeButtonsOnTabs", pageView_->ui.closeBtnsChk, true);
#else
	pageView_->ui.closeBtnsChk->hide();
#endif
	
	connect(SettingsItem::notifier(), SIGNAL(hasChangedItems(bool)), SLOT(somethingChanged(bool)));

	//	Editor page
	JUFFDEBUG("Initialization: editor page");
	pageEditor_->ui.fontCmb->setCurrentFont(TextDocSettings::font());
	pageEditor_->ui.fontSizeSpin->setValue(TextDocSettings::font().pointSize());
	int chars = TextDocSettings::lineLengthIndicator();
	if (chars > 0) {
		pageEditor_->ui.showLineLengthChk->setChecked(true);
		pageEditor_->ui.lineLengthSpin->setValue(chars);
	}
	else {
		pageEditor_->ui.showLineLengthChk->setChecked(false);
		pageEditor_->ui.lineLengthSpin->setValue(-chars);
	}
	pageEditor_->ui.hlCurLineChk->setChecked(TextDocSettings::highlightCurrentLine());
	pageEditor_->ui.tabStopWidthSpin->setValue(TextDocSettings::tabStopWidth());
	pageEditor_->ui.showIndentsChk->setChecked(TextDocSettings::showIndents());
	pageEditor_->ui.replaceTabsChk->setChecked(TextDocSettings::replaceTabsWithSpaces());
	pageEditor_->ui.unindentChk->setChecked(TextDocSettings::backspaceUnindents());

	//	Autocomplete
	pageAC_->ui.useDocumentChk->setChecked(AutocompleteSettings::useDocument());
	pageAC_->ui.useApiChk->setChecked(AutocompleteSettings::useApis());
	pageAC_->ui.replaceWordChk->setChecked(AutocompleteSettings::replaceWord());
	pageAC_->ui.matchCaseChk->setChecked(AutocompleteSettings::caseSensitive());
	pageAC_->ui.thresholdSpin->setValue(AutocompleteSettings::threshold());
	
	//	charsets page
	pageCharsets_->init();
}

void SettingsDlg::addPluginSettingsPage(const QString& name, QWidget* page) {
	PluginPage* plPage = new PluginPage(name, page);
	mp_->addChildPage(tr("Plugins"), name, plPage);
	pluginPages_[name] = plPage;
	plPage->enablePage(PluginSettings::pluginEnabled(name));
}

int SettingsDlg::exec() {
	init();
	return QDialog::exec();
}

void SettingsDlg::apply() {
	MainSettings::setTabPosition(pageView_->ui.tabPositionCmb->currentIndex());
	MainSettings::setIconTheme(pageView_->ui.iconThemeCmb->currentText());
	MainSettings::setToolButtonStyle(pageView_->ui.toolButtonStyleCmb->currentIndex());
	MainSettings::setIconSize(pageView_->ui.iconSizeCmb->currentIndex());
	
	int startupVariant = 0;
	if (pageMain_->ui.openLastSessionBtn->isChecked()) {
		startupVariant = 1;
	}
	else if (pageMain_->ui.openEmptySessionBtn->isChecked()) {
		startupVariant = 2;
	}
	MainSettings::setStartupVariant(startupVariant);

	foreach (SettingsItem* sItem, items_) {
		sItem->writeValue();
	}

	//	Editor page
	QFont font(pageEditor_->ui.fontCmb->currentFont());
	font.setPointSize(pageEditor_->ui.fontSizeSpin->value());
	TextDocSettings::setFont(font);

	if (pageEditor_->ui.showLineLengthChk->isChecked()) {
		TextDocSettings::setLineLengthIndicator(pageEditor_->ui.lineLengthSpin->value());
	}
	else {
		TextDocSettings::setLineLengthIndicator(-pageEditor_->ui.lineLengthSpin->value());
	}
	TextDocSettings::setHighlightCurrentLine(pageEditor_->ui.hlCurLineChk->isChecked());
	// colors
	TextDocSettings::setMarkersColor(pageEditor_->markersColorBtn_->color());
	TextDocSettings::setCurLineColor(pageEditor_->curLineColorBtn_->color());
	TextDocSettings::setDefaultFontColor(pageEditor_->fontColorBtn_->color());
	TextDocSettings::setDefaultBgColor(pageEditor_->bgColorBtn_->color());
	TextDocSettings::setMatchedBraceBgColor(pageEditor_->braceColorBtn_->color());
	TextDocSettings::setIndentsColor(pageEditor_->indentsColorBtn_->color());
	TextDocSettings::setSelectionBgColor(pageEditor_->selectionBgColorBtn_->color());

	TextDocSettings::setReplaceTabsWithSpaces(pageEditor_->ui.replaceTabsChk->isChecked());
	TextDocSettings::setBackspaceUnindents(pageEditor_->ui.unindentChk->isChecked());
	TextDocSettings::setTabStopWidth(pageEditor_->ui.tabStopWidthSpin->value());
	TextDocSettings::setShowIndents(pageEditor_->ui.showIndentsChk->isChecked());

	//	Autocomplete
	AutocompleteSettings::setUseDocument(pageAC_->ui.useDocumentChk->isChecked());
	AutocompleteSettings::setUseApis(pageAC_->ui.useApiChk->isChecked());
	AutocompleteSettings::setReplaceWord(pageAC_->ui.replaceWordChk->isChecked());
	AutocompleteSettings::setCaseSensitive(pageAC_->ui.matchCaseChk->isChecked());
	AutocompleteSettings::setThreshold(pageAC_->ui.thresholdSpin->value());

	//	charsets
	pageCharsets_->applySettings();

	//	plugins
	QStringList plugins = pluginPages_.keys();
	foreach (QString plName, plugins) {
		Log::debug(plName);
		PluginPage* page = pluginPages_[plName];
		if ( page ) {
			Log::debug(QString("Plugin '%1' was %2").arg(plName).arg( page->pageEnabled() ? "ENABLED" : "DISABLED" ));
			PluginSettings::setPluginEnabled(plName, page->pageEnabled());
		}
	}

	fileTypesPage_->apply();

	emit applied();
}

void SettingsDlg::ok() {
	apply();
	accept();
}

bool SettingsDlg::isPluginEnabled(const QString& pluginName) {
	PluginPage* page = pluginPages_.value(pluginName, 0);
	if (page != 0)
		return page->pageEnabled();
	else
		return false;
}


void SettingsDlg::somethingChanged(bool changed) {
	qDebug() << "Something is changed:" << (changed ? "TRUE" : "FALSE");
}
