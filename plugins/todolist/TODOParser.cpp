#include "TODOParser.h"

TODOParser::TODOParser() : QThread() {
	cancelled_ = false;
	
	// TODO : need to make it configurable
	keywords_ << "todo" << "fixme" << "bug" << "hack";
	colors_["todo"]  = QColor(200, 255, 210);
	colors_["fixme"] = QColor(200, 215, 255);
	colors_["bug"]   = QColor(255, 140, 140);
	colors_["hack"]  = QColor(255, 240, 140);
}

TODOParser::~TODOParser() {
}

void TODOParser::launch(const QString& text) {
	lines_ = text.split(QRegExp("\r\n|\n|\r"));
	start();
}

void TODOParser::cancel() {
	cancelled_ = true;
}

void TODOParser::run() {
	int index = 0;
	foreach (QString line, lines_) {
		if ( cancelled_ ) {
			return;
		}
		
		foreach (QString keyword, keywords_) {
			QRegExp rx(QString(".*(%1)\\s*\\:.*").arg(keyword), Qt::CaseInsensitive );
			int pos = line.indexOf(rx);
			if ( pos >= 0 ) {
				QColor color = colors_.value(keyword, Qt::white);
				emit itemFound(line, index, rx.pos(1), color);
			}
		}
		++index;
	}
}
