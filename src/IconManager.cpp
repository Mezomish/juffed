#include "IconManager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QString>

#include "AppInfo.h"

class IMInterior {
public:
	IMInterior() {
	}
	
	QMap<QString, QIcon> iconMap_;
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
	//	first try to find specified theme at user's home dir
	QDir iconDir(AppInfo::configDir() + "/icons/" + iconTheme);
	if (!iconDir.exists()) {
		//	if doesn't exist, try to find it in app's dir
		iconDir = QDir(QCoreApplication::applicationDirPath() + "/icons/" + iconTheme);
		if (!iconDir.exists())
			return;
	}

	QString themeFileName = iconDir.filePath(iconTheme + ".theme");
	QFile file(themeFileName);
	if (file.open(QIODevice::ReadOnly)) {
		QString key, value;
		imInt_->iconMap_.clear();
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
	return imInt_->iconMap_.value(action, QIcon());
}

IconManager* IconManager::instance_ = 0;
