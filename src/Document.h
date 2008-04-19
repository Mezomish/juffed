
/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

/**
 * Abstract class of document
 */

#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include <QtGui/QAbstractButton>
#include <QtGui/QMessageBox>

#include "CommandStorage.h"
//#include "DocView.h"
#include "Log.h"

class DocView;

namespace Juff {
	
class Document : public QObject {
Q_OBJECT
public:
	enum Status {
		StatusSuccess,
		StatusCancel,
		StatusFileNotFound,
		StatusErrorOpenFile,
		StatusErrorSaveFile,
		StatusWrongFileName,
		StatusUnknownError
	};

	enum SaveRequest {
		SaveYes,
		SaveNo,
//		SaveAll,
		SaveCancel
	};
	
	Document(const QString& fileName, DocView* view);
	virtual ~Document();

	const QString& fileName() const;
	bool isModified() const;
	DocView* view() const;
	
	virtual Document::Status open() = 0;
	virtual Document::Status save() = 0;
	virtual Document::Status saveAs() = 0;
	virtual Document::Status reload() = 0;
	virtual Document::SaveRequest confirmForClose() = 0;

	virtual void processTheCommand(CommandID) = 0;
	virtual void applySettings();
	
	/**
	 *	Starts monitoring of external modification of the file
	 */
	void extModMonitoringStart();
	
	/**
	 *	Stops monitoring of external modification of the file
	 */
	void extModMonitoringStop();
	
	
	/**
	 *	This method should be reimplemented only in derived NullDoc class
	 */
	virtual bool isNull() { return false; }

signals:
	void fileNameChanged();

public slots:
	void setModified(bool mod);

protected:
	void setFileName(const QString& fileName);	
	QDateTime lastModified() const;
	void setLastModified(const QDateTime& dt);
	
private slots:
	void checkLastModified();
	
private:
	QString fileName_;
	DocView* view_;
	bool modified_;
	QDateTime lastModified_;
	QTimer* modCheckTimer_;
	QMutex checkingMutex_;
};

}	//	namespace Juff

#endif
