#ifndef __JUFFED_LEXER_MATLAB_H__
#define __JUFFED_LEXER_MATLAB_H__

#include <Qsci/qscilexer.h>

class QsciLexerMatlab : public QsciLexer {
Q_OBJECT
public:
	enum {
		Default,
		Comment,
		Command,
		Number,
		Keyword,
		String,
		Operator,
		Identifier,
		DoubleQuotedString,
	};
	
	QsciLexerMatlab(QObject *parent = 0);
	virtual ~QsciLexerMatlab() {}

	const char *language() const { return "Matlab"; }
	const char *lexer() const { return "matlab"; }
	
	const char *keywords(int set) const;
	QString description(int style) const;
	QColor defaultColor(int style) const;
};

#endif // __JUFFED_LEXER_MATLAB_H__
