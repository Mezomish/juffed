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

#include "SettingsItem.h"

SettingsChangeNotifier::SettingsChangeNotifier() : QObject() {
}

void SettingsChangeNotifier::notifyHasChanged() {
	emit hasChangedItems(true);
}

void SettingsChangeNotifier::notifyNoChanged() {
	emit hasChangedItems(false);
}


int SettingsItem::counter_ = 0;
SettingsChangeNotifier* SettingsItem::notifier_ = NULL;

SettingsItem::SettingsItem() {
	if ( NULL == notifier_ )
		notifier_ = new SettingsChangeNotifier();
}

SettingsChangeNotifier* SettingsItem::notifier() {
	return notifier_;
}

void SettingsItem::oneMoreChanged() {
	++counter_;
	if ( 1 == counter_ ) {
		// was 0 before increase
		notifier_->notifyHasChanged();
	}
}

void SettingsItem::oneLessChanged() {
	--counter_;
	if ( 0 == counter_ ) {
		notifier_->notifyNoChanged();
	}
}
