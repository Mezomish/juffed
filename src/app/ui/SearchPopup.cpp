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

#include "SearchPopup.h"

#include <QLineEdit>

SearchPopup::SearchPopup() : QWidget() {
	ui.setupUi(this);
	setMaximumHeight(65);
	
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
	
	connect(ui.closeBtn, SIGNAL(clicked()), SLOT(dismiss()));
	
	QLineEdit* findEdit = ui.findCmb->lineEdit();
	connect(findEdit, SIGNAL(textChanged(const QString&)), SLOT(onTextChanged(const QString&)));
	connect(findEdit, SIGNAL(returnPressed()), SLOT(slotFindNext()));
	
	connect(ui.findNextBtn, SIGNAL(clicked()), SLOT(slotFindNext()));
	connect(ui.findPrevBtn, SIGNAL(clicked()), SLOT(slotFindPrev()));
	connect(ui.caseSensitiveChk, SIGNAL(toggled(bool)), SLOT(onCaseSensitiveChecked(bool)));
	connect(ui.wholeWordsChk, SIGNAL(toggled(bool)), SLOT(onWholeWordsChecked(bool)));
}

void SearchPopup::setFindText(const QString& text) {
	ui.findCmb->lineEdit()->setText(text);
}

void SearchPopup::setFindFocus(bool selectAll) {
	ui.findCmb->lineEdit()->setFocus();
	if ( selectAll )
		ui.findCmb->lineEdit()->selectAll();
}

Juff::SearchParams SearchPopup::searchParams() const {
	return params_;
}

void SearchPopup::dismiss() {
	emit closed();
	hide();
}

void SearchPopup::showReplace() {
	ui.replaceL->show();
	ui.replaceCmb->show();
	ui.replacePrevBtn->show();
	ui.replaceNextBtn->show();
	ui.replaceAllBtn->show();
}

void SearchPopup::hideReplace() {
	ui.replaceL->hide();
	ui.replaceCmb->hide();
	ui.replacePrevBtn->hide();
	ui.replaceNextBtn->hide();
	ui.replaceAllBtn->hide();
}

void SearchPopup::highlightRed(bool highlight) {
	QPalette plt = ui.findCmb->lineEdit()->palette();
	if ( highlight )
		plt.setColor(QPalette::Base, QColor(255, 200, 200));
	else
		plt.setColor(QPalette::Base, QColor(255, 255, 255));
	ui.findCmb->lineEdit()->setPalette(plt);
}


void SearchPopup::onTextChanged(const QString& text) {
	params_.findWhat = text;
	params_.backwards = false;
	emit searchRequested();
}

void SearchPopup::onCaseSensitiveChecked(bool checked) {
	params_.caseSensitive = checked;
	emit searchRequested();
}

void SearchPopup::onWholeWordsChecked(bool checked) {
	params_.wholeWords = checked;
	emit searchRequested();
}

void SearchPopup::slotFindNext() {
	params_.backwards = false;
	emit findNext();
}

void SearchPopup::slotFindPrev() {
	params_.backwards = true;
	emit findPrev();
}


