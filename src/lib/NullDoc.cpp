#include "NullDoc.h"

Juff::Document* NullDoc::instance_ = NULL;

Juff::Document* NullDoc::instance() {
	if (instance_ == NULL)
		instance_ = new NullDoc();
	return instance_;
}

NullDoc::NullDoc() : Document("") {
}

NullDoc::NullDoc(const QString&) : Document("") {
}
