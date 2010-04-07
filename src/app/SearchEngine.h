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
	bool startFind(Juff::Document*, const Juff::SearchParams&);
	
	JuffMW* mw_;
	Juff::DocHandlerInt* handler_;
};

#endif // __JUFFED_SEARCH_ENGINE_H__
