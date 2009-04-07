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

#include "FindDlg.h"

//	Qt headers
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>

namespace Juff {
namespace GUI {

QString FindDlg::lastString_ = "";
QString FindDlg::lastReplaceText_ = "";
bool FindDlg::matchCase_ = true;
bool FindDlg::backward_ = false;
bool FindDlg::regExpMode_ = false;
QStringList FindDlg::strings_;
QStringList FindDlg::replaces_;

FindDlg::FindDlg(QWidget* parent) : 
	QDialog(parent) {

	uiFind.setupUi(this);

	foreach (QString str, strings_)
		uiFind.findCmb->addItem(str);
	foreach (QString repl, replaces_)
		uiFind.replaceCmb->addItem(repl);

	uiFind.findCmb->setEditText(lastString_);
	uiFind.replaceCmb->setEditText(lastReplaceText_);
	uiFind.matchCaseChk->setChecked(matchCase_);
	uiFind.backwardChk->setChecked(backward_);
	uiFind.replaceChk->setChecked(false);
	uiFind.regexpChk->setChecked(regExpMode_);

	uiFind.findCmb->setFocus();
	uiFind.findCmb->lineEdit()->selectAll();

	connect(uiFind.findBtn, SIGNAL(clicked()), SLOT(accept()));
	connect(uiFind.cancelBtn, SIGNAL(clicked()), SLOT(reject()));
	connect(uiFind.replaceChk, SIGNAL(toggled(bool)), SLOT(setReplaceMode(bool)));
}

FindDlg::~FindDlg() {
	lastString_ = uiFind.findCmb->currentText();
	strings_.prepend(lastString_);
	lastReplaceText_ = uiFind.replaceCmb->currentText();
	replaces_.prepend(lastReplaceText_);
	matchCase_ = uiFind.matchCaseChk->isChecked();
	backward_ = uiFind.backwardChk->isChecked();
	regExpMode_ = uiFind.regexpChk->isChecked();
}

DocFindFlags FindDlg::flags() const {
	return DocFindFlags(uiFind.replaceChk->isChecked(), uiFind.matchCaseChk->isChecked(), 
			uiFind.backwardChk->isChecked(), uiFind.regexpChk->isChecked());
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
		uiFind.replaceCmb->setFocus();
		uiFind.replaceCmb->lineEdit()->selectAll();
	}
	else {
		uiFind.findBtn->setText(tr("Find"));
		setWindowTitle(tr("Find"));
	}
	if (uiFind.replaceChk->isChecked() != replaceMode)
		uiFind.replaceChk->setChecked(replaceMode);
}

}	//	namespace GUI
}	//	namespace Juff
