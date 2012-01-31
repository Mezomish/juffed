#ifndef __FIND_WORKER_H__
#define __FIND_WORKER_H__

#include <QtCore/QThread>
#include <QStringList>

class FindWorker : public QThread {
Q_OBJECT
public:
	struct Params {
		Params(
			const QString& text = "",
			const QString& dir = "",
			bool rec = false,
			int pattVar = 0,
			const QStringList& filePatts = QStringList()
		)
		{
			findText = text;
			startDir = dir;
			recursive = rec;
			patternVariant = pattVar;
			filePatterns = filePatts;
		}
		QString findText;
		QString startDir;
		bool recursive;
		int patternVariant;
		QStringList filePatterns;
	};
	
	FindWorker();

	void findInFiles(const QString& findText, const QStringList& files);
	void findInDir(const QString& findText, const QString& dirName);
	void setParams(const Params&);
	virtual void run();

signals:
	void matchFound(const QString&, int, int, const QString&);
	
private:
	void findInText(const QString& findText, const QString& text, const QString& fileName);

	Params params_;
};

#endif /* __FIND_WORKER_H__ */
