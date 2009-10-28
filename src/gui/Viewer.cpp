#include <QDebug>
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

#include "Viewer.h"

#include <QtCore/QMap>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>

#include "../Document.h"
#include "Functions.h"
#include "Log.h"
#include "MainSettings.h"
#include "TabWidget.h"

namespace Juff {
namespace GUI {

class Viewer::Interior {
public:
	Interior() {
		widget_ = new QSplitter(Qt::Horizontal);
		tw1_ = new TabWidget(0);
		widget_->addWidget(tw1_);
		
		curView_ = NULL;
	}
	~Interior() {
		delete widget_;
	}
	
	TabWidget* getTabWidget(Document* doc, int& index) {
		if ( !doc || doc->isNull() ) {
			index = -1;
			return NULL;
		}
		else {
			index = tw1_->indexOf(doc->widget());
			return tw1_;
		}
	}
	
	QSplitter* widget_;
	TabWidget* tw1_;
	QMap<QWidget*, QString> fileNamesMap_;
	QWidget* curView_;
};

Viewer::Viewer() : QObject() {
	vInt_ = new Interior();
	
	connect(vInt_->tw1_, SIGNAL(currentChanged(int)), this, SLOT(curIndexChanged(int)));

	connect(vInt_->tw1_, SIGNAL(requestFileName(int, QString&)), SLOT(onFileNameRequested(int, QString&)));
	connect(vInt_->tw1_, SIGNAL(tabCloseRequested(int)), SLOT(onTabCloseRequested(int)));
	connect(vInt_->tw1_, SIGNAL(newFileRequested()), SIGNAL(requestNewDoc()));
	connect(vInt_->tw1_, SIGNAL(docOpenRequested(const QString&)), SIGNAL(requestOpenDoc(const QString&)));
#if QT_VERSION >= 0x040500
	connect(vInt_->tw1_, SIGNAL(tabMoved(int, int)), this, SIGNAL(tabMoved(int, int)));
#endif
}

Viewer::~Viewer() {
	delete vInt_;
}

void Viewer::applySettings() {
	vInt_->tw1_->setTabPosition((QTabWidget::TabPosition)MainSettings::tabPosition());
	vInt_->tw1_->setCloseBtnOnTabs(MainSettings::closeButtonsOnTabs());
}

QWidget* Viewer::widget() {
	return vInt_->widget_;
}

void Viewer::addDoc(Document* doc, int panel) {
	if ( !doc || !doc->widget() )
		return;
	
	QWidget* w = doc->widget();
	if ( !w )
		JUFFDEBUG("W is NULL");
	
	vInt_->fileNamesMap_[w] = doc->fileName();

	TabWidget* tw = vInt_->tw1_;
	tw->addTab(w, getDocTitle(doc->fileName()));
	tw->setCurrentWidget(w);
	tw->enableCloseButton(true);
	tw->show();
	doc->init();
	tw->setTabToolTip(tw->indexOf(w), doc->fileName());
	
	if ( tw->count() == 1 ) {
		vInt_->curView_ = w;
		emit curDocChanged(w);
	}
}

void Viewer::setDocModified(Document* doc, bool modified) {
	int index = -1;
	TabWidget* tw = vInt_->getTabWidget(doc, index);
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
	TabWidget* tw = vInt_->getTabWidget(doc, index);
	if ( tw ) {
		QWidget* w = doc->widget();
		if ( vInt_->fileNamesMap_.contains(w) ) {
			QString fileName = doc->fileName();
			vInt_->fileNamesMap_[w] = fileName;
			tw->setTabText(index, getDocTitle(fileName));
			tw->setTabToolTip(index, fileName);
		}
	}
}

void Viewer::removeDoc(Document* doc) {
	if ( !doc || !doc->widget() )
		return;
	
	if ( doc ) {
		int index = -1;
		TabWidget* tw = vInt_->getTabWidget(doc, index);
		if ( tw ) {
			vInt_->fileNamesMap_.remove(doc->widget());
			tw->removeTab(index);

			if ( index == tw->currentIndex() ) {
				//	The doc was removed but the current 
				//	index remained the same
				
				vInt_->curView_ = tw->currentWidget();
				emit curDocChanged(vInt_->curView_);
			}
		}
	}
}

void Viewer::activateDoc(Document* doc) {
	if ( doc ) {
		int index = -1;
		TabWidget* tw = vInt_->getTabWidget(doc, index);
		if ( tw ) {
			tw->setCurrentWidget(doc->widget());
			doc->widget()->setFocus();
		}
	}
}

void Viewer::nextDoc() {
	JUFFENTRY;
	
	TabWidget* tw = vInt_->tw1_;
	
	if ( tw ) {
		int n = tw->count();
		int newIndex = tw->currentIndex() + 1;
		if ( newIndex >= n )
			newIndex -= n;
		tw->setCurrentIndex( newIndex );
	}
}

void Viewer::prevDoc() {
	JUFFENTRY;
	
	TabWidget* tw = vInt_->tw1_;
	
	if ( tw ) {
		int n = tw->count();
		int newIndex = tw->currentIndex() - 1;
		if ( newIndex < 0 )
			newIndex += n;
		tw->setCurrentIndex( newIndex );
	}
}

QWidget* Viewer::curDoc() const {
	return vInt_->curView_;
}


void Viewer::curIndexChanged(int i) {
	JUFFENTRY;
	TabWidget* tw = qobject_cast<TabWidget*>(sender());
	if ( tw ) {
		QWidget* w = tw->widget(i);
		vInt_->curView_ = w;
		emit curDocChanged(w);
	}
}

void Viewer::onFileNameRequested(int index, QString& name) {
	JUFFENTRY;
	
	TabWidget* tw = qobject_cast<TabWidget*>(sender());
	if ( tw ) {
		QWidget* w = tw->widget(index);
		emit requestDocName(w, name);
	}
}

void Viewer::onTabCloseRequested(int index) {
	JUFFENTRY;
	
	TabWidget* tw = qobject_cast<TabWidget*>(sender());
	if ( tw ) {
		QWidget* w = tw->widget(index);
		emit requestDocClose(w);
	}
}

int Viewer::curPanel() const {
	JUFFENTRY;
	return 0;
}

void Viewer::getViewsList(int panel, QWidgetList& list) const {
	list.clear();
	if ( panel == 1 ) {
		int count = vInt_->tw1_->count();
		for (int i = 0; i < count; ++i) {
			list << vInt_->tw1_->widget(i);
		}
	}
}

}	//	namespace GUI
}	//	namespace Juff
