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

#include "MultiPage.h"

//	Qt headers
#include <QtCore/QString>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtCore/QVector>

class MultiPageInterior {
public:
	MultiPageInterior(QWidget* mp) {
		list_ = new QListWidget();
		list_->setFixedWidth(150);
		
		panel_ = new QWidget();
		panelLayout_ = new QHBoxLayout();
		panelLayout_->setMargin(0);
		panel_->setLayout(panelLayout_);

		QHBoxLayout* hbox = new QHBoxLayout();
		hbox->setMargin(0);
		hbox->addWidget(list_);
		hbox->addWidget(panel_);
		mp->setLayout(hbox);
	}

	QListWidget* list_;
	QWidget* panel_;
	QHBoxLayout* panelLayout_;

	QVector<QWidget*> pages_;
};


MultiPage::MultiPage(QWidget* parent) : QWidget(parent) {
	mpInt_ = new MultiPageInterior(this);
	connect(mpInt_->list_, SIGNAL(currentRowChanged(int)), SLOT(setCurrentIndex(int)));
}

MultiPage::~MultiPage() {
	delete mpInt_;
}

void MultiPage::addPage(const QString& str, QWidget* w) {
	mpInt_->list_->addItem(str);
	w->setParent(mpInt_->panel_);
	mpInt_->pages_.append(w);
	w->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	if (pageCount() == 1)
		selectPage(0);
	else
		selectPage(currentIndex());
}

int MultiPage::pageCount() const {
	return mpInt_->pages_.count();
}

QWidget* MultiPage::currentPage() const {
	return mpInt_->pages_.at(currentIndex());
}

void MultiPage::selectPage(int page) {
	mpInt_->list_->setCurrentRow(page);
	setCurrentIndex(page);
}

int MultiPage::pageIndex(QWidget* w) {
	return mpInt_->pages_.indexOf(w);
}

int MultiPage::currentIndex() const {
	return mpInt_->list_->currentRow();
}

void MultiPage::setCurrentIndex(int index) {
	if (index >=0 && index < mpInt_->pages_.count()) {
		foreach (QWidget* w, mpInt_->pages_)
			w->hide();
		QWidget* page = mpInt_->pages_.at(index);
		while (mpInt_->panelLayout_->count() > 0)
			mpInt_->panelLayout_->removeItem(mpInt_->panelLayout_->itemAt(0));
		mpInt_->panelLayout_->addWidget(page);
		page->show();
	}
}
