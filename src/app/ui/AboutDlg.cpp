/*
JuffEd - An advanced text editor
Copyright 2007-2010 Murzin Mikhail

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "AboutDlg.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPixmap>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>

#include "Log.h"

class TextBrowserPage : public QWidget {
public:
	TextBrowserPage(QWidget* parent) : QWidget(parent) {
		tb_ = new QTextBrowser(this);
		tb_->setOpenExternalLinks(true);
	}

	void setText(const QString& text, bool html = true) {
		if (html)
			tb_->setHtml(text);
		else
			tb_->setText(text);
	}

private:
	virtual void resizeEvent(QResizeEvent* e) {
		QSize sz = e->size();
		tb_->setGeometry(0, 0, sz.width(), sz.height());
	}
	
	QTextBrowser* tb_;
};

class AboutDlg::Interior {
public:
	Interior(QWidget* parent) {
		QHBoxLayout* hBox = new QHBoxLayout();
		iconL_ = new QLabel();
		nameL_ = new QLabel();
		hBox->addWidget(iconL_);
		hBox->addWidget(nameL_);

		closeBtn_ = new QPushButton(AboutDlg::tr("Close"));
		parent->connect(closeBtn_, SIGNAL(clicked()), parent, SLOT(accept()));
		QHBoxLayout* hBox2 = new QHBoxLayout();
		hBox2->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
		hBox2->addWidget(closeBtn_);
		hBox2->setMargin(0);
		hBox2->setSpacing(0);
		
		QVBoxLayout* vBox = new QVBoxLayout();
		tabWidget_ = new QTabWidget(parent);
		
		vBox->addLayout(hBox);
		vBox->addWidget(tabWidget_);
		vBox->addLayout(hBox2);

		parent->setLayout(vBox);

		QFont font = nameL_->font();
		font.setPointSize(font.pointSize() + 2);
		nameL_->setFont(font);
		
		aboutPage_ = new QLabel(0);
		aboutPage_->setTextFormat(Qt::RichText);
		aboutPage_->setWordWrap(true);
		aboutPage_->setAlignment(Qt::AlignCenter);
		tabWidget_->addTab(aboutPage_, AboutDlg::tr("About"));
	}

	//	Labels
	QLabel* iconL_;
	QLabel* nameL_;

	QLabel* aboutPage() {
		return aboutPage_;
	}

	TextBrowserPage* getPage(const QString& title) {
		return pagesMap_.value(title, NULL);
	}

	void addPage(const QString& title) {
		if ( !pagesMap_.contains(title) ) {
			TextBrowserPage* page = new TextBrowserPage(0);
			pagesMap_[title] = page;
			tabWidget_->addTab(page, title);
		}
	}

private:
	QTabWidget* tabWidget_;
	QLabel* aboutPage_;
	QMap<QString, TextBrowserPage*> pagesMap_;
	QPushButton* closeBtn_;
};

AboutDlg::AboutDlg(QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/) : QDialog(parent, f) {
	dlgInt_ = new Interior(this);
	
	dlgInt_->aboutPage();
	dlgInt_->addPage(tr("Authors"));
	dlgInt_->addPage(tr("Plugins"));
	dlgInt_->addPage(tr("Translations"));
	dlgInt_->addPage(tr("Thanks"));
	dlgInt_->addPage(tr("License"));
	
	connect(dlgInt_->aboutPage(), SIGNAL(linkActivated(const QString&)), SLOT(gotoUrl(const QString&)));
}

AboutDlg::~AboutDlg() {
	delete dlgInt_;
}

void AboutDlg::gotoUrl(const QUrl& url) {
	QDesktopServices::openUrl(url);
}

void AboutDlg::gotoUrl(const QString& url) {
	QDesktopServices::openUrl(QUrl(url));
}

void AboutDlg::setProgramName(const QString& name) {
	QString labelText = QString("&nbsp;&nbsp;<b>%1</b>").arg(name);
	dlgInt_->nameL_->setText(labelText);
}

void AboutDlg::setIcon(const QIcon& icon) {
	QSize size = icon.actualSize(QSize(64, 64));
	dlgInt_->iconL_->setPixmap(icon.pixmap(size));
}

void AboutDlg::setText(const QString& text) {
	dlgInt_->aboutPage()->setText(text);
}

void AboutDlg::setPageText(const QString& pageTitle, const QString& text, bool isHtml /* = true */) {
	TextBrowserPage* page = dlgInt_->getPage(pageTitle);
	if ( page != NULL ) {
		page->setText(text, isHtml);
	}
}
