#ifndef __JUFFED_PLUGIN_API_H__
#define __JUFFED_PLUGIN_API_H__

#include <QObject>

#include "Document.h"
#include "DocHandlerInt.h"
#include "Project.h"

namespace Juff {
	class PluginNotifier;
};

class JuffAPI : public QObject {
Q_OBJECT
public:
	/**
	* Returns current document or NullDoc if there is no current document.
	* See "Document.h" for details.
	*/
	Juff::Document* currentDocument() const;
	
	/**
	* Returns document by file name or NullDoc if there is no such document opened.
	* See "Document.h" for details.
	*/
	Juff::Document* document(const QString& fileName) const;
	
	/**
	* Returns current project.
	* See "Project.h" for details.
	*/
	Juff::Project* currentProject() const;


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
	
signals:
	// document notifications
	void docOpened(Juff::Document*);
	void docActivated(Juff::Document*);
	void docClosed(Juff::Document*);
	void docRenamed(Juff::Document*, const QString& oldName);
	void docModified(Juff::Document*);
	void docTextChanged(Juff::Document*);
	void docSyntaxChanged(Juff::Document*, const QString& oldSyntax);
	void docCharsetChanged(Juff::Document*, const QString& oldCharset);

	// project notifications
	void projectOpened(Juff::Project*);
	void projectRenamed(Juff::Project*, const QString& oldName, const QString& oldPath);
	void projectFileAdded(Juff::Project*, const QString&);
	void projectFileRemoved(Juff::Project*, const QString&);
	void projectSubProjectAdded(Juff::Project*, Juff::Project*);
	void projectSubProjectRemoved(Juff::Project*, Juff::Project*);
	void projectAboutToBeClosed(Juff::Project*);
	
private:
	class Interior;
	Interior* int_;
};

#endif // __JUFFED_PLUGIN_API_H__
