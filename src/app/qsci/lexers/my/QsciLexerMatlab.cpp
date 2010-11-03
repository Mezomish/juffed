#include "QsciLexerMatlab.h"

QsciLexerMatlab::QsciLexerMatlab(QObject *parent) : QsciLexer(parent) {
}

const char *QsciLexerMatlab::keywords(int set) const {
	if ( set == 1 )
		return "break case catch classdef continue else elseif end for "
		       "function global if otherwise parfor persistent return "
		       "switch try while";
	return 0;
}

QString QsciLexerMatlab::description(int) const {
	// TODO : add descriptions here
	return "";
}

QColor QsciLexerMatlab::defaultColor(int style) const {
	switch (style) {
		case Default :
			return Qt::black;
		
		case Comment :
			return QColor(160, 160, 160);
		
		case Command :
			return QColor(80, 80, 120);
		
		case Number :
			return QColor(40, 160, 40);
		
		case Keyword :
			return QColor(40, 40, 200);
		
		case String :
			return QColor(200, 30, 30);
		
		case Operator :
			return QColor(200, 60, 200);
		
		case Identifier :
			return QColor(20, 40, 40);
		
		case DoubleQuotedString :
			return QColor(140, 30, 30);
	}
	return Qt::black;
}
