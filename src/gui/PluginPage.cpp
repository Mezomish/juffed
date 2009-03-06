/*
JuffEd - A simple text editor
Copyright 2007-2009 Mikhail Murzin

This program is free software; you can redistribute it and/orx
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

#include "PluginPage.h"

#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>

PluginPage::PluginPage(const QString& pluginName, QWidget* page) : 
	QWidget(), 
	enabled_(true), 
	name_(pluginName) {

	page_ = page;

	QVBoxLayout* vBox = new QVBoxLayout();

	usePluginChk_ = new QCheckBox(tr("Enabled"));
	connect(usePluginChk_, SIGNAL(toggled(bool)), SLOT(enablePage(bool)));
	vBox->addWidget(usePluginChk_);

	if (page != 0)
		vBox->addWidget(page);
	vBox->addStretch();
	
	setLayout(vBox);
}

PluginPage::~PluginPage() {
	delete usePluginChk_;
}

bool PluginPage::pageEnabled() const {
	return enabled_;
}

void PluginPage::enablePage(bool e) {
	enabled_ = e;
	if (sender() == 0)
		usePluginChk_->setChecked(e);
	if (page_ != 0) {
		page_->setEnabled(e);
	}
}
