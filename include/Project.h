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

#ifndef __JUFFED_PROJECT_H__
#define __JUFFED_PROJECT_H__

#include "LibConfig.h"

class QDomDocument;
class QDomElement;

#include <QObject>
#include <QStringList>

namespace Juff {

class LIBJUFF_EXPORT Project : public QObject {
Q_OBJECT
public:
	Project(const QString& fileName);
	virtual ~Project();

	QString name() const;
	QString fileName() const;
	void setFileName(const QString&);
	void setName(const QString&);
	bool isNoname() const;

	bool hasSubProjects() const;
	bool hasFiles() const;
	int subProjectCount() const;
	int fileCount() const;
	bool addSubProject(Project*);
	bool addFile(const QString&);
	bool removeSubProject(int);
	bool removeSubProject(Project*);
	bool removeFile(int);
	bool removeFile(const QString&);

	Project* subProject(int) const;
	QStringList files() const;

signals:
	void renamed(const QString& oldName);
	void fileAdded(const QString&);
	void fileRemoved(const QString&);
	void subProjectAdded(Juff::Project*);
	void subProjectRemoved(Juff::Project*);
	void aboutToBeClosed();

private:
	bool save();
	bool load();

	void parseProject(QDomElement&, Project*);
	void storeProject(QDomDocument&, QDomElement&, Project*);

	QString fileName_;
	QString name_;
	QList<Project*> subProjects_;
	QStringList files_;
};

} // namespace Juff

#endif // __JUFFED_PROJECT_H__
