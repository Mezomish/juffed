/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

#include "IconManager.h"

#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QString>

#ifdef Q_OS_WIN
#include "AppInfo.win.h"
#else
#include "AppInfo.h"
#endif

class IMInterior {
public:
	IMInterior() : default_(true) {
	}
	
	QMap<QString, QIcon> iconMap_;
	bool default_;
};

IconManager::IconManager() {
	imInt_ = new IMInterior();
}

IconManager::~IconManager() {
	delete imInt_;
}

IconManager* IconManager::instance() {
	if (instance_ == 0)
		instance_ = new IconManager();
	return instance_;
}

void IconManager::loadTheme(const QString& iconTheme) {
	if (iconTheme.compare("<default>") == 0) {
		imInt_->default_ = true;
		imInt_->iconMap_.clear();
		return;
	}
	
	QDir iconDir(AppInfo::configDir() + "/icons/" + iconTheme);
	if (!iconDir.exists()) {
		return;
	}

	QString themeFileName = iconDir.filePath(iconTheme + ".theme");
	QFile file(themeFileName);
	if (file.open(QIODevice::ReadOnly)) {
		QString key, value;
		imInt_->iconMap_.clear();
		imInt_->default_ = false;
		while (!file.atEnd()) {
			QString line = file.readLine();
			key = line.section('=', 0, 0).simplified();
			value = line.section('=', 1, 1).simplified();
			imInt_->iconMap_[key] = QIcon(iconDir.absolutePath() + "/" + value);
		}
		file.close();
	}
}

QIcon IconManager::icon(const QString& action) const {
	if (imInt_->default_)
		return QIcon(QString(":%1").arg(action));
	else
		return imInt_->iconMap_.value(action, QIcon());
}

IconManager* IconManager::instance_ = 0;
