#ifndef __JUFFED_PLUGIN_API_H__
#define __JUFFED_PLUGIN_API_H__

#include "Document.h"
#include "DocHandlerInt.h"
#include "PluginNotifier.h"
#include "Project.h"

class JuffAPI {
public:
	/**
	* Returns current document or NullDoc if there is no current document.
	* See "Document.h" for details.
	*/
	Juff::Document* currentDocument() const;
	
	/**
	* Returns current project.
	* See "Project.h" for details.
	*/
	Juff::Project* currentProject() const;
	
	/**
	* Returns PluginNotifier object that emits signals when certain events occur.
	* See "PluginNotifier.h" for details.
	*/
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
	
	
	/// Constructor
	JuffAPI(Juff::DocHandlerInt*, Juff::PluginNotifier*);
	/// Destructor
	virtual ~JuffAPI();
private:
	class Interior;
	Interior* int_;
};

#endif // __JUFFED_PLUGIN_API_H__
