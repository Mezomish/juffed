#ifndef __JUFFED_SEARCH_ENGINE_H__
#define __JUFFED_SEARCH_ENGINE_H__

class JuffMW;

#include "Types.h"

namespace Juff {
	class Document;
	class DocHandlerInt;
}

class SearchEngine : QObject {
Q_OBJECT
public:
	SearchEngine(Juff::DocHandlerInt*, JuffMW*);

	void find(Juff::Document*);
	void findNext(Juff::Document*);
	void findPrev(Juff::Document*);

protected slots:
	void onSearchRequested(const Juff::SearchParams&);

private:
	void keepVariables(Juff::Document*);
	void clearSelection(Juff::Document*);

	bool startFind(Juff::Document*);

	bool performSearch(Juff::Document*);
	int findAt(const QString&, bool forward, int& length);
	
	JuffMW* mw_;
	Juff::DocHandlerInt* handler_;
	bool steppedOver_;
	int startLine_;
	int startCol_;
	QString fileName_;
	Juff::SearchParams params_;
};

#endif // __JUFFED_SEARCH_ENGINE_H__
