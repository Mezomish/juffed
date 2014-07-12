#ifndef _FIND_IN_FILES_PLUGIN_H_
#define _FIND_IN_FILES_PLUGIN_H_

class QTreeWidgetItem;

#include <QtCore/QObject>

#include "JuffPlugin.h"

class FindInFilesPlugin : public QObject, public JuffPlugin {
Q_OBJECT
#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID JuffPlugin_iid)
#endif
Q_INTERFACES(JuffPlugin)
public:
	FindInFilesPlugin();
	virtual ~FindInFilesPlugin();

	//	plugin name
	virtual QString name() const;
	//	plugin description
	virtual QString description() const;
	//	engines
	virtual QString targetEngine() const;
	
	virtual void init();
	
	//	controls
	//	plugin toolbar
	virtual QToolBar* toolBar() const;
	virtual Juff::ActionList mainMenuActions(Juff::MenuID) const;
	//	widget that will be put to dock
	virtual QWidgetList dockList() const;
	virtual Qt::DockWidgetArea dockPosition(QWidget*) const;

private slots:
	void slotSearchInFiles();
	void slotStopSearch();
	void slotAdvSearch();
	void slotItemDoubleClicked(QTreeWidgetItem*, int);
	void slotMatchFound(const QString, int, int, const QString&);
	void onStarted();
	void onFinished();

private:
	void startSearch();
	void showDock();
	void findInFiles(const QString& findText, const QStringList& list);
	void findInText(const QString& findText, const QString& text, const QString& fileName);

	class PluginInterior;
	PluginInterior* pInt_;
};

#endif
