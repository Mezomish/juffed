/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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
#include "MultiPage.h"
#include "MainSettings.h"
#include "TextDocSettings.h"

#include "ui_MainSettingsPage.h"

class MainSettingsPage : public QWidget{
public:
	MainSettingsPage() : QWidget() {
		ui.setupUi(this);
	}
	Ui::MainSettingsPage ui;
};

#include "ui_ViewSettingsPage.h"

class ViewSettingsPage : public QWidget {
public:
	ViewSettingsPage() : QWidget() {
		ui.setupUi(this);
	}
	Ui::ViewSettingsPage ui;
};

#include "ui_EditorSettingsPage.h"

class EditorSettingsPage : public QWidget {
public:
	EditorSettingsPage() : QWidget () {
		ui.setupUi(this);

		//	Creating a ColorButton extension. It doesn't demand to be deleted 
		//	manually 'cause it will be deleted automatically when it's parent 
		//	button is deleted
		markersColorBtn_ = new ColorButton(ui.markerColorBtn, TextDocSettings::markersColor());
	}
	
	Ui::EditorSettingsPage ui;
	ColorButton* markersColorBtn_;
};

#include "CharsetsSettingsPage.h"

/////////////////////////////////////////////////////////////

SettingsDlg::SettingsDlg(QWidget* parent) : QDialog(parent) {

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
	mp_->addPage(tr("General"), pageMain_);
	mp_->addPage(tr("View"), pageView_);
	mp_->addPage(tr("Editor"), pageEditor_);
	mp_->addPage(tr("Charsets"), pageCharsets_);

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
	delete pageCharsets_;
	delete pageEditor_;
	delete pageMain_;
}

void SettingsDlg::init() {
	//	icon themes
	pageView_->ui.iconThemeCmb->clear();
	pageView_->ui.iconThemeCmb->addItem("<default>");
	QDir iconsDir(AppInfo::configDir() + "/icons");
	if (iconsDir.exists()) {
		pageView_->ui.iconThemeCmb->addItems(iconsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot));
	}
	
	int tabPos = MainSettings::tabPosition();
	QString iconTheme = MainSettings::iconTheme();
	int style = MainSettings::toolButtonStyle();
	pageView_->ui.tabPositionCmb->setCurrentIndex(tabPos);
	if (iconTheme.isEmpty())
		pageView_->ui.iconThemeCmb->setCurrentIndex(pageView_->ui.iconThemeCmb->findText("<default>"));
	else
		pageView_->ui.iconThemeCmb->setCurrentIndex(pageView_->ui.iconThemeCmb->findText(iconTheme));
	pageView_->ui.toolButtonStyleCmb->setCurrentIndex(style);
	pageView_->ui.iconSizeCmb->setCurrentIndex(MainSettings::iconSize());
	

	//	Main page
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
	int autoSaveInterval = MainSettings::autoSaveInterval();
	if (autoSaveInterval > 0) {
		pageMain_->ui.autoSaveChk->setChecked(true);
		pageMain_->ui.autoSaveIntervalSpin->setValue(autoSaveInterval);
	}
	else {
		pageMain_->ui.autoSaveChk->setChecked(false);
		pageMain_->ui.autoSaveIntervalSpin->setValue(-autoSaveInterval);
	}
	pageMain_->ui.singleInstanceChk->setChecked(MainSettings::singleInstance());
#ifndef Q_OS_UNIX
	pageMain_->ui.singleInstanceChk->hide();
#endif
	
	//	Editor page
	pageEditor_->ui.fontCmb->setCurrentFont(TextDocSettings::font());
	pageEditor_->ui.fontSizeSpin->setValue(TextDocSettings::font().pointSize());
	pageEditor_->ui.widthAdjustChk->setChecked(TextDocSettings::widthAdjust());
	pageEditor_->ui.showLineNumbersChk->setChecked(TextDocSettings::showLineNumbers());
	pageEditor_->ui.tabStopWidthSpin->setValue(TextDocSettings::tabStopWidth());
	int chars = TextDocSettings::lineLengthIndicator();
	if (chars > 0) {
		pageEditor_->ui.showLineLengthChk->setChecked(true);
		pageEditor_->ui.lineLengthSpin->setValue(chars);
	}
	else {
		pageEditor_->ui.showLineLengthChk->setChecked(false);
		pageEditor_->ui.lineLengthSpin->setValue(-chars);
	}
	pageEditor_->ui.showIndentsChk->setChecked(TextDocSettings::showIndents());
	pageEditor_->ui.hlCurLineChk->setChecked(TextDocSettings::highlightCurrentLine());
	pageEditor_->ui.replaceTabsChk->setChecked(TextDocSettings::replaceTabsWithSpaces());
	pageEditor_->ui.unindentChk->setChecked(TextDocSettings::backspaceUnindents());
	
	pageCharsets_->init();
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

	MainSettings::setSaveSessionOnClose(pageMain_->ui.saveSessionChk->isChecked());
	MainSettings::setSyncOpenDialogToCurDoc(pageMain_->ui.syncOpenDlgChk->isChecked());
	MainSettings::setMakeBackupOnSave(pageMain_->ui.makeBackupChk->isChecked());
	int asInterval = pageMain_->ui.autoSaveIntervalSpin->value();
	if (pageMain_->ui.autoSaveChk->isChecked())
		MainSettings::setAutoSaveInterval(asInterval);
	else		
		MainSettings::setAutoSaveInterval(-asInterval);
	MainSettings::setSingleInstance(pageMain_->ui.singleInstanceChk->isChecked());

	//	Editor page
	QFont font(pageEditor_->ui.fontCmb->currentFont());
	font.setPointSize(pageEditor_->ui.fontSizeSpin->value());
	TextDocSettings::setFont(font);
	TextDocSettings::setWidthAdjust(pageEditor_->ui.widthAdjustChk->isChecked());
	TextDocSettings::setShowLineNumbers(pageEditor_->ui.showLineNumbersChk->isChecked());
	TextDocSettings::setTabStopWidth(pageEditor_->ui.tabStopWidthSpin->value());

	if (pageEditor_->ui.showLineLengthChk->isChecked()) {
		TextDocSettings::setLineLengthIndicator(pageEditor_->ui.lineLengthSpin->value());
	}
	else {
		TextDocSettings::setLineLengthIndicator(-pageEditor_->ui.lineLengthSpin->value());
	}
	TextDocSettings::setShowIndents(pageEditor_->ui.showIndentsChk->isChecked());
	TextDocSettings::setHighlightCurrentLine(pageEditor_->ui.hlCurLineChk->isChecked());
	TextDocSettings::setReplaceTabsWithSpaces(pageEditor_->ui.replaceTabsChk->isChecked());
	TextDocSettings::setBackspaceUnindents(pageEditor_->ui.unindentChk->isChecked());
	TextDocSettings::setMarkersColor(pageEditor_->markersColorBtn_->color());

	pageCharsets_->applySettings();

	emit applied();
}

void SettingsDlg::ok() {
	apply();
	accept();
}
