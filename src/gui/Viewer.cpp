/*
JuffEd - A simple text editor
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

#include "Viewer.h"

#include <QtCore/QMap>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>

#include "Document.h"
#include "Functions.h"
#include "Log.h"
#include "MainSettings.h"
#include "TabWidget.h"

namespace Juff {
namespace GUI {

class Viewer::Interior {
public:
	Interior() {
//		widget_ = new QWidget();
		
//		QVBoxLayout* vBox = new QVBoxLayout();
		widget_ = new QSplitter(Qt::Horizontal);
		tw1_ = new TabWidget(0);
		tw2_ = new TabWidget(0);
		widget_->addWidget(tw1_);
		widget_->addWidget(tw2_);
		
		tw2_->hide();
		
		curView_ = NULL;
//		vBox->addWidget(tabWidget_);
//		widget_->setLayout(vBox);
	}
	~Interior() {
		delete widget_;
//		delete tabWidget_;
	}
	
	QTabWidget* getTabWidget(Document* doc, int& index) {
		if ( !doc || doc->isNull() ) {
			index = -1;
			return NULL;
		}
		else {
			QTabWidget* tw = NULL;
			QWidget* w = doc->widget();
			if ( (index = tw1_->indexOf(w)) >= 0 ) {
				//	this doc belongs to the 1st panel
				tw = tw1_;
			}
			else if ( (index = tw2_->indexOf(w)) >= 0 ) {
				//	this doc belongs to the 2nd panel
				tw = tw2_;
			}
			else {
				Log::debug("Oops.... nobody holds the document");
			}
			return tw;
		}
	}
	
	QSplitter* widget_;
	QTabWidget* tw1_;
	QTabWidget* tw2_;
	QMap<QWidget*, QString> fileNamesMap_;
	QWidget* curView_;
};

Viewer::Viewer() : QObject() {
	vInt_ = new Interior();
	
	connect(vInt_->tw1_, SIGNAL(currentChanged(int)), this, SLOT(curIndexChanged(int)));
	connect(vInt_->tw2_, SIGNAL(currentChanged(int)), this, SLOT(curIndexChanged(int)));

	connect(vInt_->tw1_, SIGNAL(requestFileName(int, QString&)), SLOT(onFileNameRequested(int, QString&)));
	connect(vInt_->tw1_, SIGNAL(tabCloseRequested(int)), SLOT(onTabCloseRequested(int)));
	connect(vInt_->tw2_, SIGNAL(requestFileName(int, QString&)), SLOT(onFileNameRequested(int, QString&)));
	connect(vInt_->tw2_, SIGNAL(tabCloseRequested(int)), SLOT(onTabCloseRequested(int)));
}

Viewer::~Viewer() {
	delete vInt_;
}

void Viewer::applySettings() {
	vInt_->tw1_->setTabPosition((QTabWidget::TabPosition)MainSettings::tabPosition());
	vInt_->tw2_->setTabPosition((QTabWidget::TabPosition)MainSettings::tabPosition());
}

QWidget* Viewer::widget() {
	return vInt_->widget_;
}

void Viewer::addDoc(Document* doc, int panel/*, const QString& fileName*/) {
//	JUFFENTRY;

	if ( !doc || !doc->widget() )
		return;
	
//	JUFFDEBUG("here");
	
	QWidget* w = doc->widget();
	if ( !w )
		JUFFDEBUG("W is NULL");
	
	vInt_->fileNamesMap_[w] = doc->fileName();
	connect(doc, SIGNAL(activated()), SLOT(docActivated()));

	if ( panel == 1 ) {
//		JUFFDEBUG("111111111111111");
		vInt_->tw1_->addTab(w, getDocTitle(doc->fileName()));
		vInt_->tw1_->setCurrentWidget(w);
		vInt_->tw1_->show();
	}
	else {
//		JUFFDEBUG("222222222222222");
		vInt_->tw2_->addTab(w, getDocTitle(doc->fileName()));
		vInt_->tw2_->setCurrentWidget(w);
		vInt_->tw2_->show();
	}
	
/*	if ( vInt_->tw1_->isHidden() || vInt_->tw2_->isHidden() ) {
		int w = vInt_->widget_->width();
		vInt_->widget_->setSizes(QList<int>() << w/2 << w/2);
	}*/
	
	w->setFocus();
	
}

void Viewer::setDocModified(Document* doc, bool modified) {
	int index = -1;
	QTabWidget* tw = vInt_->getTabWidget(doc, index);
	if ( tw ) {
		QString title = getDocTitle(doc->fileName());
		if ( modified ) {
			title += "*";
		}
		tw->setTabText(index, title);
	}
}

void Viewer::updateDocTitle(Document* doc) {
	int index = -1;
	QTabWidget* tw = vInt_->getTabWidget(doc, index);
	if ( tw ) {
		QWidget* w = doc->widget();
		if ( vInt_->fileNamesMap_.contains(w) ) {
			vInt_->fileNamesMap_[w] = doc->fileName();
			tw->setTabText(index, getDocTitle(doc->fileName()));
		}
	}
}

void Viewer::removeDoc(Document* doc) {
	if ( !doc || !doc->widget() )
		return;
	
	if ( doc ) {
		int index = -1;
		QTabWidget* tw = vInt_->getTabWidget(doc, index);
		if ( tw ) {
			vInt_->fileNamesMap_.remove(doc->widget());
			tw->removeTab(index);

			if ( tw->count() == 0 ) {
				//	This doc was the last doc in this 
				//	tab widget. Hide it and set focus to
				//	another tab widget
				
				tw->hide();
				
				//	get another tab widget and set focus to it's current view
				QTabWidget* tw2 = (tw == vInt_->tw1_ ? vInt_->tw2_ : vInt_->tw1_);
				if ( tw2 ) {
					QWidget* w = tw2->currentWidget();
					if ( w ) {
						w->setFocus();
					}
				}
			}
			else if ( index == tw->currentIndex() ) {
				//	The doc was removed but the current 
				//	index remained the same
				
				vInt_->curView_ = tw->currentWidget();
				emit curDocChanged(vInt_->curView_);
			}
		}
	}
}

void Viewer::activateDoc(Document* doc) {
//	JUFFENTRY;
	if ( doc ) {
		int index = -1;
		QTabWidget* tw = vInt_->getTabWidget(doc, index);
		if ( tw ) {
			JUFFDEBUG("CURRENT!!!!!!!");
			tw->setCurrentWidget(doc->widget());
		}
	}
}

void Viewer::nextDoc() {
	JUFFENTRY;
	
	QTabWidget* tw = 0;
	if ( vInt_->tw1_->currentWidget()->hasFocus() )
		tw = vInt_->tw1_;
	else
		tw = vInt_->tw2_;
	
	if ( tw ) {
		int index = tw->currentIndex();
		int n = tw->count();
		tw->setCurrentIndex( (index + 1) % n);
	}
}

void Viewer::prevDoc() {
	JUFFENTRY;
	
	QTabWidget* tw = 0;
	if ( vInt_->tw1_->currentWidget()->hasFocus() )
		tw = vInt_->tw1_;
	else
		tw = vInt_->tw2_;
	
	if ( tw ) {
		int index = tw->currentIndex();
		int n = tw->count();
		tw->setCurrentIndex( (index + n - 1) % n);
	}
}


void Viewer::curIndexChanged(int i) {
//	JUFFENTRY;
	QTabWidget* tw = qobject_cast<QTabWidget*>(sender());
	if ( tw ) {
		QWidget* w = tw->widget(i);
		Log::debug(i);
		if ( w )
			w->setFocus();
		
		vInt_->curView_ = w;
		emit curDocChanged(w);
	}
}

void Viewer::docActivated() {
//	JUFFENTRY;
	
/*	Document* doc = qobject_cast<Document*>(sender());
	
	if ( doc ) {
		Log::debug("OK!!!");
		Log::debug(doc->fileName());
		vInt_->curView_ = doc->widget();
		emit curDocChanged(doc->widget());
	}*/
}

void Viewer::onFileNameRequested(int index, QString& name) {
	JUFFENTRY;
	
	QTabWidget* tw = qobject_cast<QTabWidget*>(sender());
	if ( tw ) {
		QWidget* w = tw->widget(index);
		emit requestDocName(w, name);
	}
}

void Viewer::onTabCloseRequested(int index) {
	JUFFENTRY;
	
	QTabWidget* tw = qobject_cast<QTabWidget*>(sender());
	if ( tw ) {
		QWidget* w = tw->widget(index);
		emit requestDocClose(w);
	}
}

int Viewer::curPanel() const {
	JUFFENTRY;
/*	QWidget* w1 = vInt_->tw1_->currentWidget();
	QWidget* w2 = vInt_->tw2_->currentWidget();
	if ( w1 && w1->hasFocus() ) {
		return 1;
	}
	else if ( w2 && w2->hasFocus() ) {
		return 2;
	}
	else {
		return 0;
	}*/
	return 0;
}

QWidget* Viewer::curView() const {
//	JUFFENTRY;
	return vInt_->curView_;
}

void Viewer::getViewsList(int panel, QWidgetList& list) const {
	list.clear();
	if ( panel == 1 ) {
		int count = vInt_->tw1_->count();
		for (int i = 0; i < count; ++i) {
			list << vInt_->tw1_->widget(i);
		}
	}
	else if ( panel == 2 ) {
		int count = vInt_->tw2_->count();
		for (int i = 0; i < count; ++i) {
			list << vInt_->tw2_->widget(i);
		}
	}
}

}	//	namespace GUI
}	//	namespace Juff
