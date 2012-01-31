#ifndef __TODO_PLUGIN_PARSER_H__
#define __TODO_PLUGIN_PARSER_H__

#include <QColor>
#include <QMap>
#include <QThread>

class TODOParser : public QThread {
Q_OBJECT
public:
	TODOParser();	
	virtual ~TODOParser();	

	void launch(const QString&);	
	void cancel();
	
signals:
	void itemFound(const QString&, int, int, const QColor&);
	
protected:
	virtual void run();
	
	bool cancelled_;
	QStringList lines_;
	QStringList keywords_;
	QMap <QString, QColor> colors_;
};

#endif // __TODO_PLUGIN_PARSER_H__
