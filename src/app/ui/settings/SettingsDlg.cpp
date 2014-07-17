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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "SettingsDlg.h"

//	Qt headers
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

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
#include "SettingsItem.h"
#include "SettingsPage.h"


#include "SettingsCheckItem.h"
#include "SettingsColorItem.h"
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
		;
		
		QMap<QString, QString> lngs;
		lngs["auto"] = QObject::tr("auto-detect");
		lngs["cs"] = QObject::tr("Czech");
		lngs["de"] = QObject::tr("German");
		lngs["en"] = QObject::tr("English");
		lngs["fr"] = QObject::tr("French");
		lngs["pl"] = QObject::tr("Polish");
		lngs["pt"] = QObject::tr("Portuguese");
		lngs["ru"] = QObject::tr("Russian");
		lngs["sp"] = QObject::tr("Spanish");
		lngs["sk"] = QObject::tr("Slovak");
		lngs["zh"] = QObject::tr("Chinese (simplified)");
		
		ui.languageCmb->clear();
		QMap<QString, QString>::iterator it = lngs.begin();
		for ( ; it != lngs.end(); ++it) {
			ui.languageCmb->addItem(it.value(), it.key());
		}
		QString language = MainSettings::get(MainSettings::Language).left(2);
		int index = ui.languageCmb->findData(language);
		if ( index >= 0 )
			ui.languageCmb->setCurrentIndex(index);
		else
			ui.languageCmb->setCurrentIndex(0);
	}
	
	virtual void apply() {
		SettingsPage::apply();
		int index = ui.languageCmb->currentIndex();
		MainSettings::set(MainSettings::Language, ui.languageCmb->itemData(index).toString());
	}

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
//		JUFFDEBUG("Initialization: icon theme");
//		QStringList themes = IconManager::instance()->themeList();
//		ui.iconThemeCmb->clear();
//		ui.iconThemeCmb->addItem("<default>");
//		ui.iconThemeCmb->addItems(themes);

		items_
//			  << new SettingsSelectItem("main", "iconTheme", ui.iconThemeCmb, SettingsSelectItem::StringMode)
			  << new SettingsSelectItem("main", "toolButtonStyle", ui.toolButtonStyleCmb, SettingsSelectItem::IndexModeWithDefaultValue)
			  << new SettingsSelectItem("main", "iconSize", ui.iconSizeCmb, SettingsSelectItem::IndexModeWithDefaultValue)
			  << new SettingsSelectItem("main", "tabPosition", ui.tabPositionCmb, SettingsSelectItem::IndexModeWithDefaultValue)
			  << new SettingsCheckItem("main", "fsHideToolbar", ui.fsHideToolbarChk)
			  << new SettingsCheckItem("main", "fsHideStatusbar", ui.fsHideStatusbarChk)
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
		ui.setupUi(this);

	}
	
	virtual void init() {
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
			<< new SettingsCheckItem("editor", "replaceTabsWithSpaces", ui.replaceTabsChk)
			<< new SettingsCheckItem("editor", "backspaceUnindents", ui.unindentChk)
			<< new SettingsCheckItem("editor", "autoDetectIndentation", ui.autoDetectIndentationChk);
	}
	
	virtual void apply() {
//		LOGGER;
		
		QFont font(ui.fontCmb->currentFont());
		font.setPointSize(ui.fontSizeSpin->value());
		EditorSettings::setFont(font);

		if ( ui.showLineLengthChk->isChecked() ) {
			EditorSettings::set(EditorSettings::LineLengthIndicator, ui.lineLengthSpin->value());
		}
		else {
			EditorSettings::set(EditorSettings::LineLengthIndicator, -ui.lineLengthSpin->value());
		}

		EditorSettings::set(EditorSettings::TabWidth, ui.tabStopWidthSpin->value());
		
		SettingsPage::apply();
	}
	
private:
	Ui::EditorSettingsPage ui;
};

class ColorSettingsPage : public SettingsPage {
public:
	ColorSettingsPage(QWidget* parent) : SettingsPage(parent) {
		btnCount_ = 0;
		QVBoxLayout* vBox = new QVBoxLayout(this);
		grid_ = new QGridLayout();
		
		vBox->addLayout(grid_);
		vBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
		setLayout(vBox);
		
		vBox->setContentsMargins( 9, 0, 0, 9 );
		grid_->setContentsMargins( 0, 0, 0, 0 );
		grid_->setVerticalSpacing( 3 );
	}
	
	void init() {
	}
	
	void addColor(const QString& title, const QString& section, const QString& key, const QColor& color) {
		QPushButton* btn = new QPushButton(this);
		btn->setMaximumWidth(80);
		
		//	Creating ColorButton extension. We shouldn't delete them
		//	manually 'cause they will be deleted automatically when their 
		//	parent buttons are deleted
		ColorButton* colorBtn = new ColorButton(btn, color);
		items_ << new SettingsColorItem(section, key, colorBtn);
		
		grid_->addWidget(new QLabel(title), btnCount_, 0);
		grid_->addWidget(btn, btnCount_, 1);
		++btnCount_;
	}
	
	int btnCount_;
	QGridLayout* grid_;
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

class PluginsMainPage : public SettingsPage {
public:
	PluginsMainPage(QWidget* parent) : SettingsPage(parent) {}
	virtual void init() {}
};

//#include "CharsetsSettingsPage.h"
#include "JuffPlugin.h"

/////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg(QWidget* parent) : QDialog(parent) {
//	LOGGER;

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

	colorsPage_ = new ColorSettingsPage(this);
	pages_ << mp_->addPage(tr("General"), new MainSettingsPage(this));
	pages_ << mp_->addPage(tr("View"), new ViewSettingsPage(this));
	pages_ << mp_->addPage(tr("Editor"), new EditorSettingsPage(this));
	pages_ << mp_->addPage(tr("Colors"), colorsPage_);
	pages_ << mp_->addPage(tr("Autocompletion"), new AutocompleteSettingsPage(this));
	pages_ << mp_->addPage(tr("Charsets"), new CharsetsSettingsPage());
	pages_ << mp_->addPage(tr("Plugins"), new PluginsMainPage(this));

	colorsPage_->addColor(tr("Default font color"), "editor", "defaultFontColor", EditorSettings::get(EditorSettings::DefaultFontColor));
	colorsPage_->addColor(tr("Default background color"), "editor", "defaultBgColor", EditorSettings::get(EditorSettings::DefaultBgColor));
	colorsPage_->addColor(tr("Selection background color"), "editor", "selectionBgColor", EditorSettings::get(EditorSettings::SelectionBgColor));
	
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
}

SettingsDlg::~SettingsDlg() {
//	items_.clear();
	delete mp_;
}

void SettingsDlg::init() {
//	LOGGER;
	
//	if ( SettingsItem::notifier() != 0 )
//		connect(SettingsItem::notifier(), SIGNAL(hasChangedItems(bool)), SLOT(somethingChanged(bool)));

	foreach (SettingsPage* page, pages_)
		page->init();
}

void SettingsDlg::addColorSetting(const QString& title, const QString& section, const QString& key, const QColor& color) {
	colorsPage_->addColor(title, section, key, color);
}

void SettingsDlg::addPluginSettingsPage(const QString& name, const QString& title, QWidget* page) {
//	LOGGER;
	PluginPage* plPage = new PluginPage(title, page, this);
	mp_->addChildPage(tr("Plugins"), title, plPage);
	pluginPages_[name] = plPage;
	plPage->enablePage(PluginSettings::pluginEnabled(name));
}

void SettingsDlg::setEditorsPages(const QWidgetList& list) {
//	LOGGER;
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
	resize(sizeHint());
	return QDialog::exec();
}

void SettingsDlg::apply() {
	//	plugins
	QStringList plugins = pluginPages_.keys();
	foreach (QString plName, plugins) {
//		Log::debug(plName);
		PluginPage* page = pluginPages_[plName];
		if ( page ) {
//			Log::debug(QString("Plugin '%1' was %2").arg(plName).arg( page->pageEnabled() ? "ENABLED" : "DISABLED" ));
			PluginSettings::setPluginEnabled(plName, page->pageEnabled());
		}
	}

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

void SettingsDlg::addPages(const QStringList& titles, const QWidgetList& pages) {
//	LOGGER;
	if ( titles.count() != pages.count() ) {
		return;
	}
	
	for ( int i = 0; i < titles.count(); ++i ) {
		SettingsPage* page = qobject_cast<SettingsPage*>(pages[i]);
		if ( page != 0 ) {
			pages_ << mp_->addPage(titles[i], page);
		}
	}
}

//void SettingsDlg::somethingChanged(bool changed) {
//	JUFFWARNING(QString("Something is changed: ").arg(changed));
//}
