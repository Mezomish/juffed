#ifndef __JUFFED_SCI_DOC_H__
#define __JUFFED_SCI_DOC_H__

#include "Document.h"

class SciDoc : public Juff::Document {
Q_OBJECT
public:
	SciDoc(const QString&);
	virtual ~SciDoc();

	virtual int lineCount() const;
	virtual bool isModified() const;
	virtual bool hasSelectedText() const;
	virtual bool getSelection(int&, int&, int&, int&) const;
	virtual bool getSelectedText(QString&) const;
	virtual bool getCursorPos(int&, int&) const;
	virtual QString syntax() const;
	virtual void setModified(bool);
	virtual void setSelection(int, int, int, int);
	virtual void removeSelectedText();
	virtual void replaceSelectedText(const QString&);
	virtual void insertText(const QString&);
	virtual void setCursorPos(int, int);
	virtual void setSyntax(const QString& lexName);


	virtual QString type() const;
//	virtual Juff::Document* createClone();
	virtual bool supportsAction(Juff::ActionID) const;
	virtual void init();
	virtual void print();
	virtual void reload();
	virtual bool save(QString& error);

	virtual void undo();
	virtual void redo();
	virtual void cut();
	virtual void copy();
	virtual void paste();
	virtual void gotoLine(int);

	virtual void setWrapWords(bool);
	virtual void setShowLineNumbers(bool);
	virtual void setShowWhitespaces(bool);
	virtual void setShowLineEndings(bool);
	virtual void zoomIn();
	virtual void zoomOut();
	virtual void zoom100();
	
	virtual bool wrapWords() const;
	virtual bool lineNumbersVisible() const;
	virtual bool whitespacesVisible() const;
	virtual bool lineEndingsVisible() const;
	
	void toUpperCase();
	void toLowerCase();
	void swapLines();
	void moveUp();
	void moveDown();
	

private slots:
	void onCursorMoved(int, int);
	void onMarginClicked(int, int, Qt::KeyboardModifiers);
	void onLineCountChanged();
	void onEditFocused();

protected:
	SciDoc(Juff::Document*);
//	virtual void updateClone();

private:
	void readFile();
	void setLexer(const QString& lexName);
	void applySettings();

	class Interior;
	Interior* int_;
};

#endif // __JUFFED_SCI_DOC_H__
