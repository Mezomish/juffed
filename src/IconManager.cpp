#include "IconManager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QString>

#include "AppInfo.h"

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
