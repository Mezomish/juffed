#ifndef _DOC_LIST_PLUGIN_H_
#define _DOC_LIST_PLUGIN_H_

class DocListPanel;
class QTreeWidgetItem;

#include <QObject>

#include "JuffPlugin.h"

class DocListPlugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)
public:
	DocListPlugin();
	virtual ~DocListPlugin();
	virtual void init();

	//	info
	virtual QString name() const;
	virtual QString description() const;
	virtual QString targetEngine() const;

	//	controls
	virtual QWidgetList dockList() const;
	virtual bool dockVisible(QWidget*) const;

public slots:
	void onDocOpened(Juff::Document*, Juff::PanelIndex);
	void onDocActivated(Juff::Document*);
	void onDocClosed(Juff::Document*);
	void onDocRenamed(Juff::Document*, const QString& oldName);
	void onDocModified(Juff::Document*);

//	virtual void onTabMoved(int from, int to);
//
//	virtual void applySettings();

protected slots:
	void docClicked(QTreeWidgetItem*, int);

private:
	DocListPanel* panel_;
};

#endif
