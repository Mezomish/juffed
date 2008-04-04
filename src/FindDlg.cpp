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

#include "FindDlg.h"

//	Qt headers
#include <QtGui/QKeyEvent>

//	local headers
#include "types.h"

QString FindDlg::lastString_ = "";
bool FindDlg::matchCase_ = true;
bool FindDlg::backward_ = false;
bool FindDlg::regExpMode_ = false;

FindDlg::FindDlg(QWidget* parent) : 
	QDialog(parent) {

	uiFind.setupUi(this);

	uiFind.findWhatEd->setText(lastString_);
	uiFind.matchCaseChk->setChecked(matchCase_);
	uiFind.backwardChk->setChecked(backward_);
	uiFind.replaceChk->setChecked(false);
	uiFind.regexpChk->setChecked(regExpMode_);

	uiFind.findWhatEd->setFocus();
	uiFind.findWhatEd->selectAll();

	connect(uiFind.findBtn, SIGNAL(clicked()), SLOT(accept()));
	connect(uiFind.cancelBtn, SIGNAL(clicked()), SLOT(reject()));
	connect(uiFind.replaceChk, SIGNAL(toggled(bool)), SLOT(setReplaceMode(bool)));
}

FindDlg::~FindDlg() {
	lastString_ = uiFind.findWhatEd->text();
	matchCase_ = uiFind.matchCaseChk->isChecked();
	backward_ = uiFind.backwardChk->isChecked();
	regExpMode_ = uiFind.regexpChk->isChecked();
}

DocFindFlags FindDlg::flags() const {
	DocFindFlags flags(uiFind.matchCaseChk->isChecked(), uiFind.backwardChk->isChecked());
	return flags;
}

void FindDlg::keyPressEvent(QKeyEvent* e) {
	if (e->key() == Qt::Key_F && e->modifiers() & Qt::ControlModifier) {
		uiFind.replaceChk->toggle();
	}
	QDialog::keyPressEvent(e);
}

void FindDlg::setReplaceMode(bool replaceMode) {
	if (replaceMode) {
		uiFind.findBtn->setText(tr("Replace"));
		setWindowTitle(tr("Replace"));
	}
	else {
		uiFind.findBtn->setText(tr("Find"));
		setWindowTitle(tr("Find"));
	}
}

