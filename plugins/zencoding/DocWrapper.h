#ifndef __ZEN_DOC_WRAPPER_H__
#define __ZEN_DOC_WRAPPER_H__

#include <QObject>
#include "Document.h"
#include "EditorSettings.h"

class DocWrapper : public QObject {
Q_OBJECT
public:
	DocWrapper() : QObject() {}
	void setDoc(Juff::Document* doc);
	
	Q_INVOKABLE bool usesTabs();
	Q_INVOKABLE int tabWidth();
	Q_INVOKABLE QString fileName();
	Q_INVOKABLE long selectionStart();
	Q_INVOKABLE long selectionEnd();
	Q_INVOKABLE void createSelection(const QVariant&, const QVariant&);
	Q_INVOKABLE void moveCursor(const QVariant&);
	Q_INVOKABLE long cursorPosition();
	Q_INVOKABLE QString selectedText();
	Q_INVOKABLE QString text();
	Q_INVOKABLE QString currentLineText();
	Q_INVOKABLE int currentLineLength();
	Q_INVOKABLE long currentLinePosition();
	Q_INVOKABLE void replaceContent(const QVariant&, const QVariant&, const QVariant&, const QVariant&);
	Q_INVOKABLE int eol();
	
private:
	Juff::Document* doc_;
};

#endif // __ZEN_DOC_WRAPPER_H__
