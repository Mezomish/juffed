/*
JuffEd - A simple text editor
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
//#include "PluginManager.h"
#include "PluginSettings.h"
#include "TextDocSettings.h"
//#include "AutocompleteSettings.h"

#include "ui_MainSettingsPage.h"

class MainSettingsPage : public QWidget{
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
	}
	
	Ui::EditorSettingsPage ui;
	ColorButton* curLineColorBtn_;
	ColorButton* markersColorBtn_;
};

/*#include "ui_AutocompleteSettingsPage.h"

class AutocompleteSettingsPage : public QWidget {
public:
	AutocompleteSettingsPage() : QWidget() {
		ui.setupUi(this);
	}
	
	Ui::AutocompletePage ui;
};*/

#include "CharsetsSettingsPage.h"
//#include "PluginPage.h"
//#include "JuffPlugin.h"

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
//	pageAC_ = new AutocompleteSettingsPage();
	mp_->addPage(tr("General"), pageMain_);
	mp_->addPage(tr("View"), pageView_);
	mp_->addPage(tr("Editor"), pageEditor_);
//	mp_->addPage(tr("Autocompletion"), pageAC_);
	mp_->addPage(tr("Charsets"), pageCharsets_);

	//	plugins
//	pluginsMainPage_ = new QWidget();
//	mp_->addPage(tr("Plugins"), pluginsMainPage_);

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
	delete pageCharsets_;
	delete pageEditor_;
	delete pageMain_;
//	delete pageAC_;
	delete pageView_;
//	delete pluginsMainPage_;
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

	pageMain_->ui.saveSessionChk->setChecked(MainSettings::saveSessionOnClose());
	pageMain_->ui.syncOpenDlgChk->setChecked(MainSettings::syncOpenDialogToCurDoc());
	pageMain_->ui.makeBackupChk->setChecked(MainSettings::makeBackupOnSave());
	pageMain_->ui.singleInstanceChk->setChecked(MainSettings::singleInstance());
#ifndef Q_OS_UNIX
	pageMain_->ui.singleInstanceChk->hide();
#endif
	
	//	Editor page
	JUFFDEBUG("Initialization: editor page");
	pageEditor_->ui.fontCmb->setCurrentFont(TextDocSettings::font());
	pageEditor_->ui.fontSizeSpin->setValue(TextDocSettings::font().pointSize());
//	pageEditor_->ui.widthAdjustChk->setChecked(TextDocSettings::widthAdjust());
//	pageEditor_->ui.showLineNumbersChk->setChecked(TextDocSettings::showLineNumbers());
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
/*	pageAC_->ui.useDocumentChk->setChecked(AutocompleteSettings::useDocument());
	pageAC_->ui.useApiChk->setChecked(AutocompleteSettings::useApis());
	pageAC_->ui.replaceWordChk->setChecked(AutocompleteSettings::replaceWord());
	pageAC_->ui.thresholdSpin->setValue(AutocompleteSettings::threshold());
*/	
	//	charsets page
	pageCharsets_->init();
}

/*void SettingsDlg::addPluginsSettings() {
	PluginList plugins = PluginManager::instance()->plugins();
	foreach (JuffPlugin* plugin, plugins) {
		if (plugin != 0) {
			PluginPage* plPage = new PluginPage(plugin->name(), plugin->settingsPage());
			mp_->addChildPage(tr("Plugins"), plugin->name(), plPage);
			pluginPages_[plugin->name()] = plPage;
			plPage->enablePage(PluginSettings::pluginEnabled(plugin->name()));
		}
	}
}*/

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

	MainSettings::setSaveSessionOnClose(pageMain_->ui.saveSessionChk->isChecked());
	MainSettings::setSyncOpenDialogToCurDoc(pageMain_->ui.syncOpenDlgChk->isChecked());
	MainSettings::setMakeBackupOnSave(pageMain_->ui.makeBackupChk->isChecked());
	MainSettings::setSingleInstance(pageMain_->ui.singleInstanceChk->isChecked());

	//	Editor page
	QFont font(pageEditor_->ui.fontCmb->currentFont());
	font.setPointSize(pageEditor_->ui.fontSizeSpin->value());
	TextDocSettings::setFont(font);
//	TextDocSettings::setWidthAdjust(pageEditor_->ui.widthAdjustChk->isChecked());
//	TextDocSettings::setShowLineNumbers(pageEditor_->ui.showLineNumbersChk->isChecked());

	if (pageEditor_->ui.showLineLengthChk->isChecked()) {
		TextDocSettings::setLineLengthIndicator(pageEditor_->ui.lineLengthSpin->value());
	}
	else {
		TextDocSettings::setLineLengthIndicator(-pageEditor_->ui.lineLengthSpin->value());
	}
	TextDocSettings::setHighlightCurrentLine(pageEditor_->ui.hlCurLineChk->isChecked());
	TextDocSettings::setMarkersColor(pageEditor_->markersColorBtn_->color());
	TextDocSettings::setCurLineColor(pageEditor_->curLineColorBtn_->color());
	TextDocSettings::setReplaceTabsWithSpaces(pageEditor_->ui.replaceTabsChk->isChecked());
	TextDocSettings::setBackspaceUnindents(pageEditor_->ui.unindentChk->isChecked());
	TextDocSettings::setTabStopWidth(pageEditor_->ui.tabStopWidthSpin->value());
	TextDocSettings::setShowIndents(pageEditor_->ui.showIndentsChk->isChecked());

	//	Autocomplete
/*	AutocompleteSettings::setUseDocument(pageAC_->ui.useDocumentChk->isChecked());
	AutocompleteSettings::setUseApis(pageAC_->ui.useApiChk->isChecked());
	AutocompleteSettings::setReplaceWord(pageAC_->ui.replaceWordChk->isChecked());
	AutocompleteSettings::setThreshold(pageAC_->ui.thresholdSpin->value());
*/
	//	charsets
	pageCharsets_->applySettings();

	//	plugins
/*	PluginList plugins = PluginManager::instance()->plugins();
	foreach (JuffPlugin* plugin, plugins) {
		if (plugin != 0) {
			QString name = plugin->name();
			PluginPage* page = pluginPages_[name];
			if (page != 0) {
				PluginSettings::setPluginEnabled(plugin->name(), page->pageEnabled());
			}
		}
	}*/
	
	emit applied();
}

void SettingsDlg::ok() {
	apply();
	accept();
}

/*bool SettingsDlg::isPluginEnabled(const QString& pluginName) {
	PluginPage* page = pluginPages_.value(pluginName, 0);
	if (page != 0)
		return page->pageEnabled();
	else
		return false;
}*/
