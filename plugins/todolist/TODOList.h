#ifndef __JUFFED_TODO_LIST_PLUGIN_H__
#define __JUFFED_TODO_LIST_PLUGIN_H__

class QTreeWidgetItem;

#include <QtCore/QObject>

#include "JuffPlugin.h"

class TODOListPlugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)
public:
	TODOListPlugin();
	virtual ~TODOListPlugin();

	virtual QString name() const;
	virtual QString description() const;
	virtual QString targetEngine() const;
	
	virtual void init();
	
	virtual QWidgetList dockList() const;
	virtual Qt::DockWidgetArea dockPosition(QWidget*) const;

private slots:
	void onDocActivated(Juff::Document *document);
	void onDocClosed(Juff::Document *document);
	void addItem(const QString&, int, int, const QColor&);
	void onItemDoubleClicked(QTreeWidgetItem*, int);
	void onParserFinished();

private:
	void showDock();
	void hideDock();

	class PluginInterior;
	PluginInterior* pInt_;
};

#endif // __JUFFED_TODO_LIST_PLUGIN_H__
