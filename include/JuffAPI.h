#ifndef __JUFFED_PLUGIN_API_H__
#define __JUFFED_PLUGIN_API_H__

#include "Document.h"
#include "DocHandlerInt.h"
#include "PluginNotifier.h"
#include "Project.h"

class JuffAPI {
public:
	JuffAPI(Juff::DocHandlerInt*, Juff::PluginNotifier*);
	virtual ~JuffAPI();

	Juff::Document* currentDocument() const;
	Juff::Project* currentProject() const;
	Juff::PluginNotifier* notifier() const;


	/**
	* Opens a document with a given file name or activates it if it is already opened.
	*/
	virtual void openDoc(const QString&);

	/**
	* Closes the document with a given file name.
	*/
	virtual void closeDoc(const QString&);

	/**
	* Saves the document with a given file name.
	*/
	virtual void saveDoc(const QString&);

	/**
	* Returns the number of currently opened documents.
	*/
	virtual int docCount() const;

	/**
	* Returns the list of currently opened documents.
	*/
	virtual QStringList docList() const;
	
private:
	class Interior;
	Interior* int_;
};

#endif // __JUFFED_PLUGIN_API_H__
