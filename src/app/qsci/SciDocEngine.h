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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __JUFFED_SCI_DOC_ENGINE_H__
#define __JUFFED_SCI_DOC_ENGINE_H__

#include "DocEngine.h"
#include "StatusLabel.h"

#include <QObject>
#include <QKeySequence>

class SciDocEngine : public QObject, public DocEngine {
Q_OBJECT
public:
	SciDocEngine();
	virtual Juff::Document* createDoc(const QString&) const;
	virtual QString type() const;
	virtual QStringList syntaxList() const;

	virtual void initMenuActions(Juff::MenuID, QMenu*);
	virtual QWidgetList statusWidgets();
	virtual void activate(bool act = true);
	virtual void deactivate(bool deact = true);

public slots:
	void slotUpperCase();
	void slotLowerCase();
	void slotMoveUp();
	void slotMoveDown();
	void slotCommentLines();
	void slotCommentBlock();
	void slotDuplicate();
	void slotUnindent();
	void slotRemoveLines();

protected slots:
	void slotSyntaxChanged();
	void onMenuAboutToBeShown();
	void onDocFocused();

private:
	QAction* createAction(const QString&, const QKeySequence&, const char*);

	QMenu* syntaxMenu_;
	QMap<QString, QAction*> syntaxActions_;
	Juff::StatusLabel* syntaxLabel_;
};

#endif // __JUFFED_SCI_DOC_ENGINE_H__
