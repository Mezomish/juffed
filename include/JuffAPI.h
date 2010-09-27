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
	virtual void openDoc(const QString&, Juff::PanelIndex panel = Juff::PanelCurrent);

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

	///////////////////////////////////////
	// Document notifications
	///////////////////////////////////////

	/**
	* Emitted when the document \param doc is opened (or created).
	*/
	void docOpened(Juff::Document* doc, Juff::PanelIndex);
	
	/**
	* Emitted when the document \param doc is activated. It happens when
	* user opens, closes or switches documents. If there is no documents 
	* any more then \param doc is a NullDoc.
	*/
	void docActivated(Juff::Document* doc);
	
	/**
	* Emitted when the document \param doc is closed.
	*/
	void docClosed(Juff::Document* doc);
	
	/**
	* Emitted when the document \param doc is renamed.
	* By the moment the signal is emitted the \param doc already has 
	* a new name. The old file name can be found in \param oldName.
	*/
	void docRenamed(Juff::Document* doc, const QString& oldName);
	
	/**
	* Emitted when the document \param doc changes its modification status.
	* The current status can be obtained by calling doc->isModified();
	*/
	void docModified(Juff::Document* doc);
	
	/**
	* Emitted when the document \param doc changes its text.
	*/
	void docTextChanged(Juff::Document* doc);
	
	/**
	* Emitted when the syntax highlighting scheme for document \param doc 
	* is changed.
	* The old syntax scheme can be found in \param oldSyntax.
	*/
	void docSyntaxChanged(Juff::Document* doc, const QString& oldSyntax);
	
	/**
	* Emitted when the encoding for document \param doc is changed.
	* The old encoding can be found in \param oldEncoding.
	*/
	void docCharsetChanged(Juff::Document* doc, const QString& oldEncoding);


	///////////////////////////////////////
	// Project notifications
	///////////////////////////////////////

	/**
	* Emitted when the project \param prj is opened.
	*/
	void projectOpened(Juff::Project* prj);
	
	/**
	* Emitted when the project \param prj is renamed.
	* By the time the signal is emitted \param prj already has a new name 
	* and a new path. The old name and path can be found in \param oldName 
	* and \param oldPath respectively.
	*/
	void projectRenamed(Juff::Project* prj, const QString& oldName, const QString& oldPath);
	
	/**
	* Emitted when the file \param fileName is added to the project \param prj.
	*/
	void projectFileAdded(Juff::Project* prj, const QString& fileName);
	
	/**
	* Emitted when the file \param fileName is removed from th project \param prj.
	*/
	void projectFileRemoved(Juff::Project* prj, const QString& fileName);
	
	/**
	* Emitted when the project \param subPrj is added to the project \param prj
	* as a sub-project.
	*/
	void projectSubProjectAdded(Juff::Project* prj, Juff::Project* subPrj);
	
	/**
	* Emitted when the sub-project \param subPrj is removed from the project \param prj.
	*/
	void projectSubProjectRemoved(Juff::Project* prj, Juff::Project* subPrj);
	
	/**
	* Emitted when the project \param prj is going to be closed.
	*/
	void projectAboutToBeClosed(Juff::Project* prj);


	///////////////////////////////////////
	// Misc
	///////////////////////////////////////

	/**
	* Emitted after all settings were applied
	*/
	void settingsApplied();
	
private:
	class Interior;
	Interior* int_;
};

#endif // __JUFFED_PLUGIN_API_H__
