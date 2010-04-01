#ifndef __JUFFED_DOC_HANDLER_INT_H__
#define __JUFFED_DOC_HANDLER_INT_H__

namespace Juff {

class Document;

class DocHandlerInt {
public:
	/**
	* Returns current document. If there is no documents then returns a NullDoc.
	*/
	virtual Juff::Document* curDoc() const = 0;

	/**
	* Opens a document with a given file name or activates it if it is already opened.
	*/
	virtual void openDoc(const QString&) = 0;

	/**
	* Closes the document with a given file name.
	*/
	virtual void closeDoc(const QString&) = 0;

	/**
	* Saves the document with a given file name.
	*/
	virtual void saveDoc(const QString&) = 0;

	/**
	* Returns the number of currently opened documents.
	*/
	virtual int docCount() const = 0;

	/**
	* Returns the list of currently opened documents.
	*/
	virtual QStringList docList() const = 0;
};

}

#endif // __JUFFED_DOC_HANDLER_INT_H__
