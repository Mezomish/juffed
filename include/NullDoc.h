#ifndef __JUFFED_NULL_DOC_H__
#define __JUFFED_NULL_DOC_H__

#include "Document.h"

class NullDoc : public Juff::Document {
public:
	static Juff::Document* instance();
	virtual bool isNull() const { return true; }
	
	virtual QString type() const { return ""; }
//	virtual Juff::Document* createClone() { return this; }

private:
	NullDoc();
	NullDoc(const QString&);
	static Juff::Document* instance_;
};

#endif // __JUFFED_NULL_DOC_H__
