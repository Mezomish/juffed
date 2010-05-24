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

#include "SettingsCheckItem.h"
#include "Settings.h"

#include <QtGui/QCheckBox>

SettingsCheckItem::SettingsCheckItem(const QString& section, const QString& key, 
                                     QCheckBox* cb)
{
	checkBox_ = cb;
	section_ = section;
	key_ = key;
//	default_ = Settings::defaultValue(section, key);
	readValue();
	
	connect(cb, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
}

void SettingsCheckItem::readValue() {
	curValue_ = Settings::instance()->boolValue(section_, key_);
	checkBox_->setChecked(curValue_);
}

void SettingsCheckItem::writeValue() {
	if ( checkBox_->isChecked() != curValue_ ) {
		curValue_ = checkBox_->isChecked();
		Settings::instance()->setValue(section_, key_, curValue_);
		oneLessChanged();
	}
}

void SettingsCheckItem::onChecked(bool checked) {
	if ( checked != curValue_ )
		oneMoreChanged();
	else
		oneLessChanged();
}
