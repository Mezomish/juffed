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

#include "SettingsSelectItem.h"

#include <QtGui/QComboBox>

SettingsSelectItem::SettingsSelectItem(const QString& section, const QString& key, 
                                     QComboBox* cb, Mode mode)
{
	comboBox_ = cb;
	section_ = section;
	key_ = key;
	mode_ = mode;
	readValue();
	
	connect(cb, SIGNAL(activated(int)), SLOT(onSelected(int)));
}

void SettingsSelectItem::readValue() {
	if ( mode_ == IndexMode ) {
		curIndex_ = Settings::intValue(section_, key_);
		comboBox_->setCurrentIndex(curIndex_);
	}
	else {
		curString_ = Settings::stringValue(section_, key_);
		int index = comboBox_->findText(curString_);
		if ( index >= 0 )
			comboBox_->setCurrentIndex(index);
	}
//	curItem_ = Settings::boolValue(section_, key_, default_);
//	comboBox_->setComboed(curValue_);
}

void SettingsSelectItem::writeValue() {
	if ( mode_ == IndexMode ) {
		curIndex_ = comboBox_->currentIndex();
		Settings::setValue(section_, key_, curIndex_);
	}
	else {
		curString_ = comboBox_->currentText();
		Settings::setValue(section_, key_, curString_);
	}
	oneLessChanged();
//	if ( comboBox_->isComboed() != curValue_ ) {
//		curValue_ = comboBox_->isComboed();
//		Settings::setValue(section_, key_, curValue_);
//		oneLessChanged();
//	}
}

void SettingsSelectItem::onSelected(int item) {
	if ( mode_ == IndexMode ) {
		if ( comboBox_->currentIndex() == curIndex_ )
			oneLessChanged();
		else
			oneMoreChanged();
	}
	else {
		if ( comboBox_->currentText() == curString_ )
			oneLessChanged();
		else
			oneMoreChanged();
	}
//	if ( comboed != curValue_ )
//		oneMoreChanged();
//	else
//		oneLessChanged();
}
