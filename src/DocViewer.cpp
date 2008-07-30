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

#include "DocViewer.h"

//	Qt headers
#include <QtCore/QString>
#include <QtGui/QTabBar>

//	local headers
#include "MainSettings.h"
#include "TabWidget.h"

#include "Log.h"

class DocViewerInterior {
public:
	DocViewerInterior(QWidget* parent) {
		tabWidget_ = new TabWidget(parent);
	}
	~DocViewerInterior() {
		delete tabWidget_;
	}
	
	QTabWidget* tabWidget_;
};

DocViewer::DocViewer(QWidget* parent) : QObject() {
	vInt_ = new DocViewerInterior(parent);
	
	connect(vInt_->tabWidget_, SIGNAL(currentChanged(int)), SLOT(onChangeCurrent(int)));
	connect(vInt_->tabWidget_, SIGNAL(tabCloseRequested(int)), SLOT(onTabCloseRequested(int)));
	connect(vInt_->tabWidget_, SIGNAL(requestFileName(int, QString&)), SLOT(fileNameRequested(int, QString&)));
}

DocViewer::~DocViewer() {
	JUFFDTOR;

	delete vInt_;
}

QWidget* DocViewer::widget() {
	return vInt_->tabWidget_;
}

void DocViewer::addDocView(const QString& label, QWidget* w) {
	vInt_->tabWidget_->addTab(w, label);
	setCurrentView(w);
	w->setFocus();
}

void DocViewer::setDocViewTitle(QWidget* w, const QString& title) {
	vInt_->tabWidget_->setTabText(vInt_->tabWidget_->indexOf(w), title);
}

void DocViewer::setCurrentView(QWidget* w) {
	vInt_->tabWidget_->setCurrentWidget(w);
	w->setFocus();
}

QWidget* DocViewer::currentView() {
	return vInt_->tabWidget_->currentWidget();
}

void DocViewer::nextView() {
	int count = vInt_->tabWidget_->count();
	if (count == 0)
		return;
	
	int index = vInt_->tabWidget_->currentIndex();
	vInt_->tabWidget_->setCurrentIndex( (index + 1) % count);
	vInt_->tabWidget_->currentWidget()->setFocus();
}

void DocViewer::prevView() {
	int count = vInt_->tabWidget_->count();
	if (count == 0)
		return;
	
	int index = vInt_->tabWidget_->currentIndex();
	vInt_->tabWidget_->setCurrentIndex( (index - 1 + count) % count);
	vInt_->tabWidget_->currentWidget()->setFocus();
}


void DocViewer::onChangeCurrent(int index) {
	emit docViewChanged(vInt_->tabWidget_->widget(index));
}

void DocViewer::onTabCloseRequested(int index) {
	emit docViewCloseRequested(vInt_->tabWidget_->widget(index));
}

void DocViewer::fileNameRequested(int index, QString& fileName) {
	emit requestFileName(vInt_->tabWidget_->widget(index), fileName);
}

void DocViewer::updateCurrentViewInfo() {
	if (vInt_->tabWidget_->count() == 0)
		emit docViewChanged(0);
	else {
		vInt_->tabWidget_->currentWidget()->setFocus();
		emit docViewChanged(vInt_->tabWidget_->currentWidget());
	}
}

void DocViewer::applySettings() {
	vInt_->tabWidget_->setTabPosition((QTabWidget::TabPosition)MainSettings::tabPosition());
}

void DocViewer::getViewsOrder(QWidgetList& list) {
	list.clear();
	int count = vInt_->tabWidget_->count();
	for (int i = 0; i < count; ++i) {
		list << vInt_->tabWidget_->widget(i);
	}
}
