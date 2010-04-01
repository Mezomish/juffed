#ifndef __JUFFED_PLUGIN_NOTIFIER_H__
#define __JUFFED_PLUGIN_NOTIFIER_H__

#include <QObject>
#include <QString>

namespace Juff {

class Project;
class Document;

class PluginNotifier : public QObject {
Q_OBJECT
public:
	PluginNotifier();

signals:
	void docOpened(Juff::Document*);
	void docActivated(Juff::Document*);
	void docClosed(Juff::Document*);
	void docRenamed(Juff::Document*, const QString& oldName);
	void docModified(Juff::Document*);
	void docTextChanged(Juff::Document*);
	void docSyntaxChanged(Juff::Document*, const QString& oldSyntax);
	void docCharsetChanged(Juff::Document*, const QString& oldCharset);

	void projectOpened(Juff::Project*);
};

}

#endif // __JUFFED_PLUGIN_NOTIFIER_H__
