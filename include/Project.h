#ifndef __JUFFED_PROJECT_H__
#define __JUFFED_PROJECT_H__

class QDomDocument;
class QDomElement;

#include <QObject>
#include <QStringList>

namespace Juff {

class Project : public QObject {
Q_OBJECT
public:
	Project(const QString& fileName);
	virtual ~Project();

	QString name() const;
	QString fileName() const;
	void setFileName(const QString&);
	void setName(const QString&);

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
