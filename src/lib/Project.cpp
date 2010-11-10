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

#include "Project.h"

#include "AppInfo.h"
#include "Functions.h"
#include "Log.h"

#include <QFileInfo>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

namespace Juff {

Project::Project(const QString& fileName) : QObject() {
	LOGGER;
	
	fileName_ = ( fileName.isEmpty() ? AppInfo::defaultPrjPath() : fileName );
	name_     = ( fileName_ == AppInfo::defaultPrjPath() ? "" : QFileInfo(fileName).fileName() );
	
	Log::debug(name_);
	Log::debug(fileName_);
	
	load();
}

Project::~Project() {
	if ( !fileName().isEmpty() )
		save();
	
	emit aboutToBeClosed();
}

QString Project::name() const {
	return name_;
}

QString Project::fileName() const {
	return fileName_;
}

void Project::setFileName(const QString& fn) {
	fileName_ = fn;
}

void Project::setName(const QString& name) {
	QString oldName = name_;
	name_ = name;
	
	emit renamed(oldName);
}

bool Project::isNoname() const {
	return name().isEmpty();
}


bool Project::hasSubProjects() const {
	return subProjects_.count() > 0;
}

bool Project::hasFiles() const {
	return files_.count() > 0;
}

int Project::subProjectCount() const {
	return subProjects_.count();
}

int Project::fileCount() const {
	return files_.count();
}

bool Project::addSubProject(Project* sub) {
	if ( hasFiles() ) {
		return false;
	}
	else {
		if ( !subProjects_.contains(sub) ) {
			subProjects_.append(sub);
			save();
			emit subProjectAdded(sub);
			return true;
		}
		else {
			return false;
		}
	}
}

bool Project::addFile(const QString& file) {
//	LOGGER;
	
	if ( hasSubProjects() ) {
		return false;
	}
	else {
		if ( !files_.contains(file) ) {
			files_.append(file);
			save();
			emit fileAdded(file);
			
			return true;
		}
		else {
			return false;
		}
	}
}

bool Project::removeSubProject(int index) {
	if ( index >=0 && index < subProjects_.count() ) {
		Juff::Project* sub = subProjects_.takeAt(index);
		save();
		emit subProjectRemoved(sub);
		delete sub;
		return true;
	}
	return false;
}

bool Project::removeSubProject(Project* prj) {
	return removeSubProject(subProjects_.indexOf(prj));
}

bool Project::removeFile(int index) {
	if ( index >=0 && index < files_.count() ) {
		QString file = files_.takeAt(index);
		save();
		emit fileRemoved(file);
		
		return true;
	}
	return false;
}

bool Project::removeFile(const QString& file) {
	return removeFile(files_.indexOf(file));
}



Project* Project::subProject(int index) const {
	return subProjects_.value(index, NULL);
}

QStringList Project::files() const {
	return files_;
}


bool Project::save() {
//	LOGGER;
	
	QFile file(fileName());
	if ( file.open(QIODevice::WriteOnly) ) {
		QDomDocument doc("JuffEd_Project");
		QDomElement prj = doc.createElement("project");
		prj.setAttribute("name", name());
		doc.appendChild(prj);
		storeProject(doc, prj, this);
		
		file.write(doc.toByteArray());
		file.close();
	}
	else {
		return false;
	}
	
	return true;
}

void Project::storeProject(QDomDocument& doc, QDomElement& prjEl, Project* prj) {
//	LOGGER;
	
	if ( prj->hasFiles() ) {
		// files
		foreach (QString file, files_) {
//			if ( !Juff::isNoname(file) ) {
//				QDomElement el = doc.createElement("file");
//				el.setAttribute("path", file);
//				prjEl.appendChild(el);
//			}
		}
	}
	else if ( prj->hasSubProjects() ) {
		// subprojects
		foreach (Project* subPrj, subProjects_) {
			QDomElement el = doc.createElement("project");
			el.setAttribute("name", subPrj->name());
			prjEl.appendChild(el);
			storeProject(doc, el, subPrj);
		}
	}
}

bool Project::load() {
	LOGGER;
	
	QDomDocument doc("JuffEd_Project");

	QFile file(fileName());
	if ( !file.open(QIODevice::ReadOnly) ) {
		Log::warning(QString("Can't open file '%1'").arg(fileName()));
		return false;
	}
	else {
		Log::debug(QString("File '%1' opened successfully").arg(fileName()), true);
	}

	QString err;
	int errLine, errCol;
	if ( !doc.setContent(&file, &err, &errLine, &errCol) ) {
		Log::debug(QString("File %1: XML reading error: '%2', line %3, column %4")
				.arg(fileName()).arg(err).arg(errLine).arg(errCol));
		file.close();
		return false;
	}
	else {
		Log::debug(QString("File '%1' was parsed successfully").arg(fileName()), true);
	}
	file.close();

	QDomElement docElem = doc.documentElement();
	parseProject(docElem, this);
/*	QDomNode prjNode = docElem.firstChild();
	while( !prjNode.isNull() ) {
		QDomElement prjEl = prjNode.toElement();
		Log::debug("TAG");
		Log::debug(prjEl.tagName());
		if( !prjEl.isNull() ) {
			if ( prjEl.tagName().toLower().compare("project") == 0 ) {
				parseProject(prjEl, this);
			}
		}
		prjNode = prjNode.nextSibling();
	}*/
	
	return true;
}

void Project::parseProject(QDomElement& prjEl, Project* prj) {
	LOGGER;
	QDomNode subNode = prjEl.firstChild();

	while ( !subNode.isNull() ) {
		QDomElement subEl = subNode.toElement();
		QString tagName = subEl.tagName().toLower();
		if ( tagName.compare("file") == 0 ) {
			QString path = subEl.attribute("path", "");
			if ( !path.isEmpty() && !prj->hasSubProjects() ) {
				prj->addFile(path);
			}
		}
		else if ( tagName.compare("project") == 0 ) {
			QString name = subEl.attribute("name", "");
			if ( !name.isEmpty() && !prj->hasFiles() ) {
				Project* subPrj = new Project("");
				subPrj->setName(name);
				parseProject(subEl, subPrj);
			}
		}

		subNode = subNode.nextSibling();
	}
}

} // namespace Juff
