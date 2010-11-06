/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#include "../../PluginManager.h"
#include "AutocompleteSettings.h"
#include "CharsetsSettingsPage.h"
#include "ColorButton.h"
#include "CommandStorage.h"
#include "EditorSettings.h"
#include "IconManager.h"
#include "Log.h"
#include "MainSettings.h"
#include "MultiPage.h"
#include "PluginPage.h"
#include "PluginSettings.h"
//#include "PrintSettings.h"
#include "SettingsItem.h"
#include "SettingsPage.h"


#include "SettingsCheckItem.h"
#include "SettingsSelectItem.h"

#include "ui_MainSettingsPage.h"
class MainSettingsPage : public SettingsPage {
public:
	MainSettingsPage(QWidget* parent) : SettingsPage(parent) {
		ui.setupUi(this);
	}

	virtual void init() {
		items_
			  << new SettingsCheckItem("main", "exitOnLastDocClosed", ui.exitOnLastDocClosedChk)
			  << new SettingsCheckItem("main", "syncOpenDialogToCurDoc", ui.syncOpenDlgChk)
			  << new SettingsCheckItem("main", "makeBackupOnSave", ui.makeBackupChk)
			  << new SettingsCheckItem("main", "stripTrailingSpaces", ui.stripSpacesChk)
			  << new SettingsCheckItem("main", "singleInstance", ui.singleInstanceChk)
//			  << new SettingsCheckItem("main", "fsHideMenubar", ui.fsHideMenubarChk)
			  << new SettingsCheckItem("main", "fsHideToolbar", ui.fsHideToolbarChk)
			  << new SettingsCheckItem("main", "fsHideStatusbar", ui.fsHideStatusbarChk)
		;
		
		QMap<QString, QString> lngs;
		lngs["cs"] = tr("Czech");
		lngs["de"] = tr("German");
		lngs["en"] = tr("English");
		lngs["fr"] = tr("French");
		lngs["pl"] = tr("Polish");
		lngs["pt"] = tr("Portuguese");
		lngs["ru"] = tr("Russian");
		lngs["sp"] = tr("Spanish");
		lngs["zh"] = tr("Chinese (simplified)");
		
		ui.languageCmb->clear();
		QMap<QString, QString>::iterator it = lngs.begin();
		for ( ; it != lngs.end(); it++) {
			ui.languageCmb->addItem(it.value(), it.key());
		}
		QString language = MainSettings::get(MainSettings::Language).left(2);
		int index = ui.languageCmb->findData(language);
		if ( index >= 0 )
			ui.languageCmb->setCurrentIndex(index);
		else
			ui.languageCmb->setCurrentIndex(ui.languageCmb->findText(tr("English")));
	}
	
	virtual void apply() {
		SettingsPage::apply();
		int index = ui.languageCmb->currentIndex();
		MainSettings::set(MainSettings::Language, ui.languageCmb->itemData(index).toString());
	}
	
/*	void init(QList<SettingsItem*>& items) {
		JUFFDEBUG("Initialization: main page");
//		int startupVariant = MainSettings::startupVariant();
//		switch (startupVariant) {
//			case 1:
//				ui.openLastSessionBtn->setChecked(true);
//				break;
//
//			case 2:
//				ui.openEmptySessionBtn->setChecked(true);
//				break;
//
//			case 0:
//			default:
//				ui.showSessionDlgBtn->setChecked(true);
//		}

#ifndef Q_OS_UNIX
//		ui.singleInstanceChk->hide();
#endif
	}*/

private:
	Ui::MainSettingsPage ui;
};


#include "ui_ViewSettingsPage.h"

class ViewSettingsPage : public SettingsPage {
public:
	ViewSettingsPage(QWidget* parent) : SettingsPage(parent) {
		ui.setupUi(this);
		
		ui.iconThemeCmb->hide();
		ui.label_2->hide();
	}

	virtual void init() {
		//	icon themes
		JUFFDEBUG("Initialization: icon theme");
//		QStringList themes = IconManager::instance()->themeList();
//		ui.iconThemeCmb->clear();
//		ui.iconThemeCmb->addItem("<default>");
//		ui.iconThemeCmb->addItems(themes);

		items_
//			  << new SettingsSelectItem("main", "iconTheme", ui.iconThemeCmb, SettingsSelectItem::StringMode)
			  << new SettingsSelectItem("main", "toolButtonStyle", ui.toolButtonStyleCmb, SettingsSelectItem::IndexMode)
			  << new SettingsSelectItem("main", "iconSize", ui.iconSizeCmb, SettingsSelectItem::IndexMode)
			  << new SettingsSelectItem("main", "tabPosition", ui.tabPositionCmb, SettingsSelectItem::IndexMode)
		;
	}
	
	virtual void apply() {
		SettingsPage::apply();
	}
	
	Ui::ViewSettingsPage ui;
};


#include "ui_EditorSettingsPage.h"

class EditorSettingsPage : public SettingsPage {
public:
	EditorSettingsPage(QWidget* parent) : SettingsPage(parent) {
//		LOGGER;
	
		ui.setupUi(this);

		//	Creating ColorButton extensions. We shouldn't delete them
		//	manually 'cause they will be deleted automatically when their 
		//	parent buttons are deleted
//		curLineColorBtn_ = new ColorButton(ui.curLineColorBtn, EditorSettings::get(EditorSettings::CurLineColor));
//		markersColorBtn_ = new ColorButton(ui.markerColorBtn, TextDocSettings::markersColor());
		fontColorBtn_ = new ColorButton(ui.fontColorBtn, EditorSettings::get(EditorSettings::DefaultFontColor));
		bgColorBtn_ = new ColorButton(ui.bgColorBtn, EditorSettings::get(EditorSettings::DefaultBgColor));
//		braceColorBtn_ = new ColorButton(ui.braceColorBtn, TextDocSettings::matchedBraceBgColor());
//		indentsColorBtn_ = new ColorButton(ui.indentColorBtn, TextDocSettings::indentsColor());
		selectionBgColorBtn_ = new ColorButton(ui.selectionBgColorBtn, EditorSettings::get(EditorSettings::SelectionBgColor));
	}
	
	virtual void init() {
//		Log::debug("Initialization: editor page");
		ui.fontCmb->setCurrentFont(EditorSettings::font());
		ui.fontSizeSpin->setValue(EditorSettings::font().pointSize());
		int chars = EditorSettings::get(EditorSettings::LineLengthIndicator);
		if (chars > 0) {
			ui.showLineLengthChk->setChecked(true);
			ui.lineLengthSpin->setValue(chars);
		}
		else {
			ui.showLineLengthChk->setChecked(false);
			ui.lineLengthSpin->setValue(-chars);
		}
		ui.tabStopWidthSpin->setValue(EditorSettings::get(EditorSettings::TabWidth));

		items_
//			<< new SettingsCheckItem("editor", "highlightCurrentLine", ui.hlCurLineChk)
//			<< new SettingsCheckItem("editor", "showIndents", ui.showIndentsChk)
			<< new SettingsCheckItem("editor", "replaceTabsWithSpaces", ui.replaceTabsChk)
			<< new SettingsCheckItem("editor", "backspaceUnindents", ui.unindentChk);
	}
	
	virtual void apply() {
		LOGGER;
		
		QFont font(ui.fontCmb->currentFont());
		font.setPointSize(ui.fontSizeSpin->value());
		EditorSettings::setFont(font);

		if ( ui.showLineLengthChk->isChecked() ) {
			EditorSettings::set(EditorSettings::LineLengthIndicator, ui.lineLengthSpin->value());
		}
		else {
			EditorSettings::set(EditorSettings::LineLengthIndicator, -ui.lineLengthSpin->value());
		}
		// colors
	//	TextDocSettings::setMarkersColor(pageEditor_->markersColorBtn_->color());
//		EditorSettings::set(EditorSettings::CurLineColor, curLineColorBtn_->color());
		EditorSettings::set(EditorSettings::DefaultFontColor, fontColorBtn_->color());
		EditorSettings::set(EditorSettings::DefaultBgColor, bgColorBtn_->color());
	//	TextDocSettings::setMatchedBraceBgColor(pageEditor_->braceColorBtn_->color());
	//	TextDocSettings::setIndentsColor(pageEditor_->indentsColorBtn_->color());
		EditorSettings::set(EditorSettings::SelectionBgColor, selectionBgColorBtn_->color());

		EditorSettings::set(EditorSettings::TabWidth, ui.tabStopWidthSpin->value());
		
		SettingsPage::apply();
	}
	
private:
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

class AutocompleteSettingsPage : public SettingsPage {
public:
	AutocompleteSettingsPage(QWidget* parent) : SettingsPage(parent) {
		ui.setupUi(this);
		
		ui.matchCaseChk->hide();
	}
	
	virtual void init() {
		items_ << new SettingsCheckItem("autocomplete", "useDocument", ui.useDocumentChk)
			<< new SettingsCheckItem("autocomplete", "useApis", ui.useApiChk)
			<< new SettingsCheckItem("autocomplete", "replaceWord", ui.replaceWordChk)
			<< new SettingsCheckItem("autocomplete", "caseSensitive", ui.matchCaseChk);
		ui.thresholdSpin->setValue(AutocompleteSettings::get(AutocompleteSettings::Threshold));
	}
	
	virtual void apply() {
		AutocompleteSettings::set(AutocompleteSettings::Threshold, ui.thresholdSpin->value());
		SettingsPage::apply();
	}
	
	Ui::AutocompletePage ui;
};

/*class PrintingPage: public QWidget {
public:
	PrintingPage() : QWidget() {
		QVBoxLayout* vBox = new QVBoxLayout(this);
		keepColorsChk_  = new QCheckBox(QObject::tr("Keep syntax highlighting"));
		keepBgColorChk_ = new QCheckBox(QObject::tr("Keep background color"));
		alwaysWrapChk_  = new QCheckBox(QObject::tr("Always wrap text"));
		vBox->addWidget(keepColorsChk_);
		vBox->addWidget(keepBgColorChk_);
		vBox->addWidget(alwaysWrapChk_);
		vBox->addStretch();
		vBox->setMargin(0);
	}
	void init(QList<SettingsItem*>& items) {
		items << new SettingsCheckItem("printing", "keepColors", keepColorsChk_)
			  << new SettingsCheckItem("printing", "keepBgColor", keepBgColorChk_)
			  << new SettingsCheckItem("printing", "alwaysWrap", alwaysWrapChk_);
	}
	QCheckBox* keepColorsChk_;
	QCheckBox* keepBgColorChk_;
	QCheckBox* alwaysWrapChk_;
};*/

class PluginsMainPage : public SettingsPage {
public:
	PluginsMainPage(QWidget* parent) : SettingsPage(parent) {}
	virtual void init() {}
};

//#include "FileTypesPage.h"
//#include "CharsetsSettingsPage.h"
//#include "PluginPage.h"
#include "JuffPlugin.h"

/////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg(QWidget* parent) : QDialog(parent) {
	LOGGER;

	setWindowTitle(tr("Settings"));
	setMinimumSize(600, 400);

	//	create buttons
	okBtn_ = new QPushButton(tr("OK"), this);
	applyBtn_ = new QPushButton(tr("Apply"), this);
	cancelBtn_ = new QPushButton(tr("Cancel"), this);
	connect(okBtn_, SIGNAL(clicked()), SLOT(ok()));
	connect(applyBtn_, SIGNAL(clicked()), SLOT(apply()));
	connect(cancelBtn_, SIGNAL(clicked()), SLOT(reject()));
	//	create multipage
	mp_ = new MultiPage();

	pages_ << mp_->addPage(tr("General"), new MainSettingsPage(this));
	pages_ << mp_->addPage(tr("View"), new ViewSettingsPage(this));
	pages_ << mp_->addPage(tr("Editor"), new EditorSettingsPage(this));
	pages_ << mp_->addPage(tr("Autocompletion"), new AutocompleteSettingsPage(this));
	pages_ << mp_->addPage(tr("Charsets"), new CharsetsSettingsPage());
	pages_ << mp_->addPage(tr("Plugins"), new PluginsMainPage(this));
//		<< mp_->addPage(tr("File types"), new FileTypesPage())
//		<< mp_->addPage(tr("Printing"), new PrintingPage())
	;

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
	
//	init();
}

SettingsDlg::~SettingsDlg() {
//	items_.clear();
	delete mp_;
}

void SettingsDlg::init() {
	LOGGER;
	
//	if ( SettingsItem::notifier() != 0 )
//		connect(SettingsItem::notifier(), SIGNAL(hasChangedItems(bool)), SLOT(somethingChanged(bool)));

	foreach (SettingsPage* page, pages_)
		page->init();
}

void SettingsDlg::addPluginSettingsPage(const QString& name, const QString& title, QWidget* page) {
//	LOGGER;
	PluginPage* plPage = new PluginPage(title, page, this);
	mp_->addChildPage(tr("Plugins"), title, plPage);
	pluginPages_[name] = plPage;
	plPage->enablePage(PluginSettings::pluginEnabled(name));
}

void SettingsDlg::setEditorsPages(const QWidgetList& list) {
	LOGGER;
	foreach (QWidget* w, list) {
		SettingsPage* page = qobject_cast<SettingsPage*>(w);
		if ( page != 0 ) {
			mp_->addChildPage(tr("Editor"), w->windowTitle(), page);
			pages_ << page;
		}
	}
}


int SettingsDlg::exec() {
	init();
	return QDialog::exec();
}

void SettingsDlg::apply() {
	LOGGER;
//	foreach (SettingsItem* sItem, items_) {
//		sItem->writeValue();
//	}

	//	Editor page
/*	QFont font(pageEditor_->ui.fontCmb->currentFont());
	font.setPointSize(pageEditor_->ui.fontSizeSpin->value());
	EditorSettings::setFont(font);

	if ( pageEditor_->ui.showLineLengthChk->isChecked() ) {
		EditorSettings::set(EditorSettings::LineLengthIndicator, pageEditor_->ui.lineLengthSpin->value());
	}
	else {
		EditorSettings::set(EditorSettings::LineLengthIndicator, -pageEditor_->ui.lineLengthSpin->value());
	}
	// colors
//	TextDocSettings::setMarkersColor(pageEditor_->markersColorBtn_->color());
	EditorSettings::set(EditorSettings::CurLineColor, pageEditor_->curLineColorBtn_->color());
	EditorSettings::set(EditorSettings::DefaultFontColor, pageEditor_->fontColorBtn_->color());
	EditorSettings::set(EditorSettings::DefaultBgColor, pageEditor_->bgColorBtn_->color());
//	TextDocSettings::setMatchedBraceBgColor(pageEditor_->braceColorBtn_->color());
//	TextDocSettings::setIndentsColor(pageEditor_->indentsColorBtn_->color());
	EditorSettings::set(EditorSettings::SelectionBgColor, pageEditor_->selectionBgColorBtn_->color());

	EditorSettings::set(EditorSettings::TabWidth, pageEditor_->ui.tabStopWidthSpin->value());

	//	Autocomplete
//	AutocompleteSettings::setThreshold(pageAC_->ui.thresholdSpin->value());

	//	charsets
	pageCharsets_->applySettings();*/

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
/*
//	fileTypesPage_->apply();
*/

	foreach (SettingsPage* page, pages_)
		page->apply();
	
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


//void SettingsDlg::somethingChanged(bool changed) {
//	JUFFWARNING(QString("Something is changed: ").arg(changed));
//}
