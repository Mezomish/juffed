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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "ColorButton.h"
#include "Log.h"
#include "SettingsColorItem.h"
#include "Settings.h"

//#include <QtGui/QCheckBox>

SettingsColorItem::SettingsColorItem(const QString& section, const QString& key, 
                                     ColorButton* cb)
{
	colorBtn_ = cb;
	section_ = section;
	key_ = key;
//	default_ = Settings::defaultValue(section, key);
	readValue();
	
//	connect(cb, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
}

void SettingsColorItem::readValue() {
	colorBtn_->setColor(Settings::instance()->colorValue(section_, key_, colorBtn_->color()));
}

void SettingsColorItem::writeValue() {
	LOGGER;
	Settings::instance()->setValue(section_, key_, colorBtn_->color());
//	oneLessChanged();
}

//void SettingsCheckItem::onChecked(bool checked) {
//	if ( checked != curValue_ )
//		oneMoreChanged();
//	else
//		oneLessChanged();
//}
