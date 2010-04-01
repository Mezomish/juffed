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
