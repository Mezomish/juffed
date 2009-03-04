#ifndef _JUFF_PARAMETER_H_
#define _JUFF_PARAMETER_H_

#include <QtCore/QString>
#include <QtCore/QVariant>

namespace Juff {

class Param {
public:
	Param() { type_ = None; }
	Param(int n) { type_ = Int; data_ = n; }
	Param(bool b) { type_ = Bool; data_ = b; }
	Param(const QString& str) { type_ = String; data_ = str; }
	
	bool isEmpty() const { return type_ == None; }
	bool isInt() const { return type_ == Int; }
	bool isBool() const { return type_ == Bool; }
	bool isString() const { return type_ == String; }

	QString toString() const { return data_.toString(); }
	int toInt() const { return data_.toInt(); }
	bool toBool() const { return data_.toBool(); }

private:
	enum Type {
		None,
		Int,
		Bool,
		String,
	};
	
	Type type_;
	QVariant data_;
};

}	//	namespace Juff

#endif
