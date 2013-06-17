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

#include "IconManager.h"

#include <QIcon>
#include <QMap>

#include "Constants.h"

static QMap<QString, QString> IconNames;

IconManager::IconManager() {
	IconNames[FILE_NEW]        = "document-new";
	IconNames[FILE_OPEN]       = "document-open";
	IconNames[FILE_SAVE]       = "document-save";
	IconNames[FILE_SAVE_AS]    = "document-save-as";
	IconNames[FILE_RELOAD]     = "view-refresh";
	IconNames[FILE_PRINT]      = "document-print";
	IconNames[FILE_EXIT]       = "application-exit";
	
	IconNames[EDIT_UNDO]       = "edit-undo";
	IconNames[EDIT_REDO]       = "edit-redo";
	IconNames[EDIT_CUT]        = "edit-cut";
	IconNames[EDIT_COPY]       = "edit-copy";
	IconNames[EDIT_PASTE]      = "edit-paste";
	
	IconNames[SEARCH_FIND]     = "edit-find";
	IconNames[SEARCH_REPLACE]  = "edit-find-replace";
	
	IconNames[VIEW_ZOOM_IN]    = "zoom-in";
	IconNames[VIEW_ZOOM_OUT]   = "zoom-out";
	IconNames[VIEW_ZOOM_100]   = "zoom-original";
	IconNames[VIEW_FULLSCREEN] = "view-fullscreen";
	
	IconNames[TOOLS_SETTINGS]  = "preferences-system";
}

QIcon IconManager::icon(const QString& key) const {
	QString module = key.section(':', 0, 0);
	QString name = key.section(':', 1, 1);
	if ( module.compare("main") == 0 ) {
		return defaultIcon(key);
	}
	
	return QIcon();
}

QIcon IconManager::defaultIcon(const QString& key) const {
	if ( IconNames.contains(key) ) {
		QString iconName = IconNames[key];
		QIcon icon = QIcon::fromTheme(iconName);
		if (icon.isNull()) {
			icon.addFile(QString(":%1/%2.png").arg(16).arg(iconName));
			icon.addFile(QString(":%1/%2.png").arg(24).arg(iconName));
			icon.addFile(QString(":%1/%2.png").arg(32).arg(iconName));
		}

		return icon;
	}
	else
		return QIcon();
}
