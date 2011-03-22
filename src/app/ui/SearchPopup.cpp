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

#include "Constants.h"
#include "Log.h"
#include "MainSettings.h"
#include "SearchPopup.h"
#include "Utils.h"

#include <QLineEdit>

SearchPopup::SearchPopup() : QWidget() {
	ui.setupUi(this);
	setMaximumHeight(65);
	collapsed_ = true;
	
	ui.closeBtn->setIcon(QIcon(":close"));
	ui.findPrevBtn->setIcon(QIcon(":find_prev"));
	ui.findNextBtn->setIcon(QIcon(":find_next"));
	ui.findPrevBtn->setText("");
	ui.findNextBtn->setText("");
	
	ui.replacePrevBtn->setIcon(QIcon(":replace_prev"));
	ui.replaceNextBtn->setIcon(QIcon(":replace_next"));
	ui.replaceAllBtn->setIcon(QIcon(":replace_all"));
	ui.replacePrevBtn->setText("");
	ui.replaceNextBtn->setText("");
	ui.replaceAllBtn->setText("");
	ui.expandBtn->setText("");
	ui.expandBtn->setIcon(Utils::iconManager()->icon(SEARCH_REPLACE));
	
	ui.findCmb->setInsertPolicy(QComboBox::InsertAtTop);
	ui.replaceCmb->setInsertPolicy(QComboBox::InsertAtTop);
	
	connect(ui.closeBtn, SIGNAL(clicked()), SLOT(dismiss()));
	
	QLineEdit* findEdit = ui.findCmb->lineEdit();
	QLineEdit* replaceEdit = ui.replaceCmb->lineEdit();
	connect(findEdit, SIGNAL(textChanged(const QString&)), SLOT(onFindTextChanged(const QString&)));
	connect(ui.caseSensitiveChk, SIGNAL(toggled(bool)), SLOT(onCaseSensitiveChecked(bool)));
	
	connect(findEdit, SIGNAL(returnPressed()), SLOT(slotFindNext()));
	connect(replaceEdit, SIGNAL(returnPressed()), SLOT(slotReplaceNext()));
	
	connect(ui.findNextBtn, SIGNAL(clicked()), SLOT(slotFindNext()));
	connect(ui.findPrevBtn, SIGNAL(clicked()), SLOT(slotFindPrev()));
	connect(ui.replaceNextBtn, SIGNAL(clicked()), SLOT(slotReplaceNext()));
	connect(ui.replacePrevBtn, SIGNAL(clicked()), SLOT(slotReplacePrev()));
	connect(ui.replaceAllBtn, SIGNAL(clicked()), SLOT(slotReplaceAll()));
	connect(ui.expandBtn, SIGNAL(toggled(bool)), SLOT(expand(bool)));
	connect(ui.modeCmb, SIGNAL(currentIndexChanged(int)), SLOT(slotModeChanged(int)));
	
	bool caseSensitive = MainSettings::get(MainSettings::SearchCaseSensitive);
	int searchMode = MainSettings::get(MainSettings::SearchMode);
	ui.caseSensitiveChk->setChecked(caseSensitive);
	ui.modeCmb->setCurrentIndex(searchMode);
}

bool SearchPopup::isCollapsed() const {
	return collapsed_;
}

void SearchPopup::setSearchStatus(int index, int total) {
	ui.findCmb->setSearchStatus(index, total);
}

void SearchPopup::setFindText(const QString& text) {
	ui.findCmb->lineEdit()->setText(text);
}

void SearchPopup::setFocusOnFind(bool selectAll) {
	ui.findCmb->lineEdit()->setFocus();
	if ( selectAll )
		ui.findCmb->lineEdit()->selectAll();
}

void SearchPopup::setFocusOnReplace(bool selectAll) {
	ui.replaceCmb->lineEdit()->setFocus();
	if ( selectAll )
		ui.replaceCmb->lineEdit()->selectAll();
}

Juff::SearchParams SearchPopup::searchParams() const {
	return params_;
}

void SearchPopup::show() {
	QWidget::show();
	emit opened();
}

void SearchPopup::dismiss() {
	hide();
	emit closed();
}


void SearchPopup::expand(bool show) {
	if ( show ) {
		ui.replaceL->show();
		ui.replaceCmb->show();
		ui.replacePrevBtn->show();
		ui.replaceNextBtn->show();
		ui.replaceAllBtn->show();
		if ( !ui.expandBtn->isChecked() )
			ui.expandBtn->setChecked(true);
	}
	else {
		ui.replaceL->hide();
		ui.replaceCmb->hide();
		ui.replacePrevBtn->hide();
		ui.replaceNextBtn->hide();
		ui.replaceAllBtn->hide();
		if ( ui.expandBtn->isChecked() )
			ui.expandBtn->setChecked(false);
	}
}

////////////////////////////////////////////////////////////

void SearchPopup::onFindTextChanged(const QString& text) {
	LOGGER;
	params_.findWhat = text;
	params_.backwards = false;
	emit searchParamsChanged(params_);
}

void SearchPopup::onCaseSensitiveChecked(bool checked) {
	params_.caseSensitive = checked;
	emit searchParamsChanged(params_);
	MainSettings::set(MainSettings::SearchCaseSensitive, checked);
}

void SearchPopup::slotFindNext() {
	params_.backwards = false;
	emit findNextRequested();
}

void SearchPopup::slotFindPrev() {
	params_.backwards = true;
	emit findPrevRequested();
}

void SearchPopup::slotReplaceNext() {
	LOGGER;
	params_.backwards = false;
	params_.replace = true;
	params_.replaceWith = ui.replaceCmb->lineEdit()->text();
	emit replaceNextRequested();
}

void SearchPopup::slotReplacePrev() {
	LOGGER;
	params_.backwards = true;
	params_.replace = true;
	params_.replaceWith = ui.replaceCmb->lineEdit()->text();
	emit replacePrevRequested();
}

void SearchPopup::slotReplaceAll() {
	LOGGER;
	params_.backwards = false;
	params_.replace = true;
	params_.replaceWith = ui.replaceCmb->lineEdit()->text();
	emit replaceAllRequested();
}

void SearchPopup::slotModeChanged(int item) {
	switch ( item ) {
		case 0 :
			params_.mode = Juff::SearchParams::PlainText;
//			ui.caseSensitiveChk->show();
			break;
		
		case 1 :
			params_.mode = Juff::SearchParams::WholeWords;
//			ui.caseSensitiveChk->show();
			break;
		
		case 2 :
			params_.mode = Juff::SearchParams::RegExp;
//			ui.caseSensitiveChk->hide();
			break;
		
		case 3 :
			params_.mode = Juff::SearchParams::MultiLineRegExp;
//			ui.caseSensitiveChk->hide();
			break;
		
		default:
			params_.mode = Juff::SearchParams::PlainText;
//			ui.caseSensitiveChk->show();
	}
	
	emit searchParamsChanged(params_);
	MainSettings::set(MainSettings::SearchMode, item);
}
