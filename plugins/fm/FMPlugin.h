#ifndef _FM_PLUGIN_H_
#define _FM_PLUGIN_H_

class QLineEdit;
class QAction;
class TreeView;

#include <QDebug>
#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QFileSystemModel>

#include <JuffPlugin.h>

class FMPlugin : public QObject, public JuffPlugin {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
Q_INTERFACES(JuffPlugin)
public:
	FMPlugin();
	virtual ~FMPlugin();

	virtual void init();

	//	info
	virtual QString name() const;
	virtual QString title() const;
	virtual QString description() const;
	virtual QString targetEngine() const;

	//	controls
	virtual QWidgetList dockList() const;
	virtual bool dockVisible(QWidget*) const;

	virtual void applySettings();
	QWidget * settingsPage() const;

protected:
	virtual void onDocSaved(const QString&);

protected slots:
	void itemDoubleClicked(const QModelIndex&);
	void home();
	void up();
	void back();
	void curFileDir();
	void favorites();
	void newDir();
	void addToFavorites();
	void goToFavorite();
	void manageFavorites();
	void textEntered();
	void treeCheckBox_toggled(bool value);
    void showHiddenBox_toggled(bool value);

	void onDirChanged(const QString&);

private:
	void cd(const QString&, bool addToHistory = true);
	void initFavoritesMenu();

	bool showAsTree;
    bool showHidden;
    int sortColumn;

	QWidget* w_;
	TreeView* tree_;
    QFileSystemModel* model_;
	QLineEdit* pathEd_;
	QAction* backBtn_;
	QStack<QString> history_;
	QStringList favorites_;
	QMenu* favoritesMenu_;
	QAction* addToFavoritesAct_;
	QAction* manageFavoritesAct_;
	QFileSystemWatcher fsWatcher_;
};

#endif
