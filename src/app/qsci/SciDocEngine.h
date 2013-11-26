/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __JUFFED_SCI_DOC_ENGINE_H__
#define __JUFFED_SCI_DOC_ENGINE_H__

#include "QSciLibConfig.h"

class QActionGroup;

#include "DocEngine.h"
#include "StatusLabel.h"
#include "SciDoc.h"

#include <QObject>
#include <QKeySequence>

class QSCI_ENGINE_EXPORT SciDocEngine : public QObject, public Juff::DocEngine {
Q_OBJECT
public:
	SciDocEngine();
	virtual Juff::Document* createDoc(const QString&) const;
	virtual QString type() const;
	virtual QStringList syntaxList() const;

	virtual Juff::ActionList mainMenuActions(Juff::MenuID);
	virtual QWidgetList statusWidgets();
	virtual void activate(bool act = true);
	virtual void deactivate(bool deact = true);
	virtual void onDocActivated(Juff::Document*);

	virtual bool getSettingsPages(QStringList&, QWidgetList&) const;
	virtual void getColorOptions(QList<ColorOption>&);
	
public slots:
	void slotUpperCase();
	void slotLowerCase();
	void slotMoveUp();
	void slotMoveDown();
	void slotCommentLines();
	void slotCommentBlock();
	void slotDuplicate();
	void slotUnindent();
	void slotInsertTab();
	void slotRemoveLines();
	void slotRemoveLineLeft();
	void slotRemoveLineRight();
	void slotEolChanged();
	void slotIndentationChanged();
	void slotIndentationWidthChanged();
	void slotFoldUnfoldAll();
	void slotMarkerAddRemove();
	void slotMarkerRemoveAll();
	void slotMarkerNext();
	void slotMarkerPrev();
	void slotGotoMarker();

	void slotShowLineNumbers();
	void slotWrapWords();
	void slotShowWhitespaces();
	void slotShowLineEndings();

protected slots:
	void slotSyntaxChanged();
	void onMenuAboutToBeShown();
	void onDocFocused();
	void updateMarkersMenu();
	void onMarkersMenuRequested(const QPoint&);

private:
	QAction* createAction(const QString&, const QKeySequence&, const char*);

	QMenu* syntaxMenu_;
	QMenu* eolMenu_;
	QMenu* indentationMenu_;

	QMenu* markersMenu_;
	QMap<QString, QAction*> syntaxActions_;
	QMap<SciDoc::Eol, QAction*> eolActions_;
	QMap<bool, QAction*> indentationActions_;
	QMap<int, QAction*> indentationWidthActions_;

	Juff::StatusLabel* syntaxLabel_;
	Juff::StatusLabel* eolLabel_;
	Juff::StatusLabel* indentationLabel_;

	QActionGroup* syntaxGroup_;
	QActionGroup* eolGroup_;
	QActionGroup* indentationGroup_;
	QActionGroup* indentationWidthGroup_;

	QAction* showLineNumsAct_;
	QAction* wrapWordsAct_;
	QAction* showWhitespacesAct_;
	QAction* showLineEndingsAct_;
};

#endif // __JUFFED_SCI_DOC_ENGINE_H__
