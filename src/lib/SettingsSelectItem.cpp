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

#include "SettingsSelectItem.h"

#include <QtGui/QComboBox>

SettingsSelectItem::SettingsSelectItem(const QString& section, const QString& key, 
                                     QComboBox* cb, int defaultValue)
{
	comboBox_ = cb;
	section_ = section;
	key_ = key;
//	default_ = defaultValue;
	readValue();
	
	connect(cb, SIGNAL(activated(int)), SLOT(onSelected(int)));
	
//	qDebug() << "section:" << section << ", key:" << key;
}

void SettingsSelectItem::readValue() {
//	curItem_ = Settings::boolValue(section_, key_, default_);
//	comboBox_->setComboed(curValue_);
}

void SettingsSelectItem::writeValue() {
//	if ( comboBox_->isComboed() != curValue_ ) {
//		curValue_ = comboBox_->isComboed();
//		Settings::setValue(section_, key_, curValue_);
//		oneLessChanged();
//	}
}

void SettingsSelectItem::onSelected(int item) {
//	if ( comboed != curValue_ )
//		oneMoreChanged();
//	else
//		oneLessChanged();
}
