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

#include "SearchComboBox.h"

#include <QLineEdit>

SearchComboBox::SearchComboBox(QWidget* parent) : QComboBox(parent) {
	setEditable(true);
	setCompleter(0);
	
	lineEdit()->setTextMargins(0, 0, StatusWidth, 0);
	statusL_ = new QLabel("", lineEdit());
	statusL_->setMaximumWidth(StatusWidth);
	statusL_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	
	QPalette plt = statusL_->palette();
	plt.setColor(QPalette::Text, plt.color(QPalette::Disabled, QPalette::Text));
	statusL_->setPalette(plt);
	
	QFont fnt = statusL_->font();
	fnt.setPointSize(fnt.pointSize() - 2);
	statusL_->setFont(fnt);
}

void SearchComboBox::setSearchStatus(int index, int total) {
	QPalette plt = lineEdit()->palette();
	if ( lineEdit()->text().isEmpty() ) {
		statusL_->setText("");
		plt.setColor(QPalette::Base, QPalette().color(QPalette::Base));
	}
	else {
		statusL_->setText(tr("%1 of %2").arg(index + 1).arg(total));
		
		if ( total == 0 ) {
			plt.setColor(QPalette::Base, QColor(255, 180, 180));
		}
		else {
			plt.setColor(QPalette::Base, QPalette().color(QPalette::Base));
		}
	}
	lineEdit()->setPalette(plt);
}

void SearchComboBox::resizeEvent(QResizeEvent* e) {
	QComboBox::resizeEvent(e);
	statusL_->setGeometry(lineEdit()->width() - StatusWidth, 0, StatusWidth, lineEdit()->height());
}
