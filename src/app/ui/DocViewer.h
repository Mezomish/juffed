#ifndef __JUFF_DOC_VIEWER_H__
#define __JUFF_DOC_VIEWER_H__

class QSplitter;

#include <QWidget>

#include "Document.h"

namespace Juff {

class TabWidget;
class DocHandlerInt;

class DocViewer : public QWidget {
Q_OBJECT
public:
	DocViewer(Juff::DocHandlerInt*);

	void applySettings();
	
	/**
	* Returns the index of currently selected panel.
	*/
	PanelIndex currentPanel() const;
	PanelIndex panelOf(Juff::Document*);
	void addDoc(Juff::Document*, PanelIndex panel);
	void removeDoc(Juff::Document*);
	Juff::Document* currentDoc() const;
	Juff::Document* currentDoc(PanelIndex) const;
	Juff::Document* document(const QString&) const;
	Juff::Document* documentAt(int index, PanelIndex panel) const;
	bool activateDoc(const QString&);
	void showPanel(PanelIndex);
	void hidePanel(PanelIndex);

	/**
	* Returns the number of documents opened at a specific panel or at both panels.
	*/
	int docCount(PanelIndex) const;
	
	/**
	* Returns the list of documents opened at a specific panel or at both panels.
	*/
	DocList docList(PanelIndex) const;
	
	/**
	* Returns the list of document names opened at a specific panel or at both panels.
	* Otherwise returns an empty list
	*/
	QStringList docNamesList(PanelIndex) const;



signals:
	void docActivated(Juff::Document*);
	
private slots:
	void nextDoc();
	void prevDoc();
	void goToNumberedDoc();
	
//protected slots:
	void onDocMoveRequested(Juff::Document*, Juff::TabWidget*);
	void onTabRemoved(Juff::TabWidget*);
	void onDocStackCalled(bool);
	void onCtrlTabSelected();
	void onCurrentChanged(int);

	void onDocModified(bool);
	void onDocFocused();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *e);

private:
	Juff::TabWidget* anotherPanel(Juff::TabWidget*) const;
	void buildCtrlTabMenu(int curItem);
	
	QAction* nextAct_;
	QAction* prevAct_;

	Juff::DocHandlerInt* handler_;
	QSplitter* spl_;
	Juff::TabWidget* tab1_;
	Juff::TabWidget* tab2_;
	Juff::TabWidget* curTab_;
	Juff::Document* curDoc_;
	QList<Juff::Document*> docStack_;
	QMenu ctrlTabMenu_;
};

} // namespace Juff

#endif // __JUFF_DOC_VIEWER_H__




#ifdef ALKJDSADLKASJ

class DocViewer : public QWidget {
Q_OBJECT
public:
	DocViewer(Juff::DocHandlerInt*);

protected:

private:
	void closePanel(Juff::TabWidget*);

	Juff::Document* curDoc_;
};
#endif
