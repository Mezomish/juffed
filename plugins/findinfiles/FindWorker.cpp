#include <QDebug>

#include "FindWorker.h"
#include "Constants.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

FindWorker::FindWorker() : QThread() {
}

void FindWorker::setParams(const Params& params) {
	params_ = params;
}

void FindWorker::run() {
	if ( !params_.findText.isEmpty() )
		findInDir(params_.findText, params_.startDir);
}

void FindWorker::findInText(const QString& findText, const QString& text, const QString& fileName) {
	QStringList lines = text.split(LineSeparatorRx);
	int lineIndex = 0;
	foreach (QString line, lines) {
		int column = line.indexOf(findText);
		if ( column >= 0 ) {
			emit matchFound(fileName, lineIndex, column, line);
		}
		++lineIndex;
	}
}

void FindWorker::findInDir(const QString& findText, const QString& dirName) {
	QDir dir(dirName);
	if ( !dir.exists() )
		return;
	
	QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

	foreach(QString fileName, files) {
		// check if the file matches the pattern (if needed)
		bool proceed = false;
		if ( params_.patternVariant == 0 ) {
			proceed = true;
		}
		else if ( params_.patternVariant == 1 ) {
			// fileName must match one of patterns
			foreach (QString pattern, params_.filePatterns) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
				QRegularExpression rx(
					QRegularExpression::wildcardToRegularExpression(pattern),
					QRegularExpression::CaseInsensitiveOption
				);
				if ( rx.match(fileName).hasMatch() ) {
#else
				QRegExp rx(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
				if ( rx.exactMatch(fileName) ) {
#endif
					proceed = true;
					break;
				}
			}
		}
		else if ( params_.patternVariant == 2 ) {
			// fileName must NOT match any of patterns
			proceed = true;
			foreach (QString pattern, params_.filePatterns) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
				QRegularExpression rx(
					QRegularExpression::wildcardToRegularExpression(pattern),
					QRegularExpression::CaseInsensitiveOption
				);
				if ( rx.match(fileName).hasMatch() ) {
#else
				QRegExp rx(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
				if ( rx.exactMatch(fileName) ) {
#endif
					proceed = false;
					break;
				}
			}
		}
		
		if ( proceed ) {
			QString fullFileName = dir.absoluteFilePath(fileName);
			QFile file(fullFileName);
			if ( file.open(QIODevice::ReadOnly) ) {
				QByteArray all = file.readAll();
				QString text = QString::fromLocal8Bit(all.data());
				file.close();
				
				findInText(findText, text, fullFileName);
			}
			else {
				// TODO : error report
			}
		}
	}
	
	
	if ( params_.recursive ) {
		foreach (QString subDir, dirs) {
			findInDir(findText, dir.absoluteFilePath(subDir));
		}
	}
}

