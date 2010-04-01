#ifndef __JUFFED_VIEWER_H__
#define __JUFFED_VIEWER_H__

class QAction;

namespace Juff {
	class Document;
	class TabWidget;
}

//#include <QTabWidget>
#include <QList>
#include <QSplitter>

class DocViewer : public QWidget {
Q_OBJECT
public:
	DocViewer();
	void addDoc(Juff::Document*);
	Juff::Document* currentDoc() const;
	Juff::Document* document(const QString&) const;
	bool activateDoc(const QString&);

	/**
	* Returns the number of documents opened at a specific panel or at both panels.
	* If \param panel == 0 then returns the number of ALL documents
	* If \param panel == 1 then returns the number of documents opened at the 1st panel
	* If \param panel == 2 then returns the number of documents opened at the 2nd panel
	* Otherwise returns 0
	*/
	int docCount(int panel = 0) const;

	/**
	* Returns the list of documents opened at a specific panel or at both panels.
	* If \param panel == 0 then returns ALL documents
	* If \param panel == 1 then returns documents opened at the 1st panel
	* If \param panel == 2 then returns documents opened at the 2nd panel
	* Otherwise returns an empty list
	*/
	QList<Juff::Document*> docList(int panel = 0) const;

	/**
	* Returns the list of document names opened at a specific panel or at both panels.
	* If \param panel == 0 then returns names of ALL documents
	* If \param panel == 1 then returns names of documents opened at the 1st panel
	* If \param panel == 2 then returns names of documents opened at the 2nd panel
	* Otherwise returns an empty list
	*/
	QStringList docNamesList(int panel = 0) const;

signals:
	void docActivated(Juff::Document*);

public slots:
	void nextDoc();
	void prevDoc();

private slots:
	void goToNumberedDoc();
	void onDocModified(bool);
	void onDocCloseRequested(Juff::Document*, Juff::TabWidget*);
	void onDocCloneRequested(Juff::Document*, Juff::TabWidget*);
	void onDocMoveRequested(Juff::Document*, Juff::TabWidget*);
	void onTabRemoved(Juff::TabWidget*);
	void onDocFocused();

private:
	void addDoc(Juff::Document*, Juff::TabWidget*);
	void closePanel(Juff::TabWidget*);
	Juff::TabWidget* anotherPanel(Juff::TabWidget*);

	QAction* nextAct_;
	QAction* prevAct_;

	Juff::TabWidget* tab1_;
	Juff::TabWidget* tab2_;
	Juff::TabWidget* curTab_;
	QSplitter* spl_;
	Juff::Document* curDoc_;
};

#endif // __JUFFED_VIEWER_H__
