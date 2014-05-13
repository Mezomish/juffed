#ifndef _FAVORITES_PLUGIN_H_
#define _FAVORITES_PLUGIN_H_

#include <QtCore/QObject>

#include <JuffPlugin.h>

class FavoritesPlugin : public QObject, public JuffPlugin {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
Q_INTERFACES(JuffPlugin)
public:
	FavoritesPlugin();
	virtual ~FavoritesPlugin();

	//	info
	virtual QString name() const;
	virtual QString description() const;
	virtual QString targetEngine() const;

	virtual QMenu* menu() const;

private slots:
	void addCurrent();
	void manageFavorites();
	void openFavorite();

private:
	void load();
	void save();
	void initMenu();

	QStringList favorites_;
	QMenu* menu_;
	QAction* addAct_;
	QAction* manageAct_;
};

#endif
