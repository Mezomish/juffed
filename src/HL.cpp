/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "HL.h"

//	Local headers
#include "HLStorage.h"

HL::HL(QTextDocument* doc) : QSyntaxHighlighter(doc) {
}

HL::~HL() {
}

void HL::changeFileName(const QString& name) {
	if (!HLStorage::instance()->getScheme(name, sch_)) {
		sch_ = HLScheme();
	}
	rehighlight();
}

void HL::highlightBlock(const QString& str) {
	highlightTheString(str);
}

bool HL::findBlockBegin(const QString& str, int& blockIndex, int& begPos) {
	begPos = -1;
	int pos = -1;
	int i = 0;
	QVector<HLBlock>::iterator blIt = sch_.blocks.begin();
	while (blIt != sch_.blocks.end()) {
		HLBlock& bl = *blIt;
		pos = str.isEmpty() || bl.begin.isEmpty() ? -1 : bl.begin.indexIn(str);
		if (pos >= 0) {
			//	the beginning of i-th block is found
			if (begPos < 0) {
				//	no one block was found before
				begPos = pos;
				blockIndex = i;
				bl.begLen = bl.begin.matchedLength();
			}
			else {
				//	begPos is already containing the position of some block
				if (pos < begPos) {
					//	store the position of the block beginning if it goes earlier
					begPos = pos;
					blockIndex = i;
					bl.begLen = bl.begin.matchedLength();
				}
			}
		}
		i++;
		blIt++;
	}
	return (begPos >= 0);
}

void HL::highlightTheString(const QString& string, int indent, int blockType) {
	int blType(blockType);
	if (indent == 0) {
		blType = previousBlockState() - 1;
	}

	//	check if some block is opened
	if (blType >= 0) {
		HLBlock& blockRule = sch_.blocks[blType];

		//	find the block end
		int blEndIndex = string.isEmpty() ? -1 : blockRule.end.indexIn(string);
		int blBegLen = blockRule.begLen;
		if (blEndIndex >= 0) {	//	found
			blockRule.endLen = blockRule.end.matchedLength();
			int blEndLen = blockRule.endLen;
		
			int from(0), len (0);
			if (indent > 0) {
				from = indent - blBegLen;
				len = blEndIndex + blEndLen + blBegLen;
			}
			else {
				from = indent;
				len = blEndIndex + blEndLen;
			}
			setFormat(from, len, blockRule.fmt);
			setCurrentBlockState(0);

			//	continue the search with remining part 
			highlightTheString(string.right(string.length() - blEndIndex - blEndLen), indent + blEndIndex + blEndLen, -1);
		}
		else {
			//	block end not found
			int from(0), len (0);
			if (indent > 0) {
				from = indent - blBegLen;
				len = string.length() + blBegLen;
			}
			else {
				from = indent;
				len = string.length();
			}

			setFormat(from, len, blockRule.fmt);
			setCurrentBlockState(blType + 1);
		}
	}
	else {
		//	no one block is opened
		int blIndex(-1), begPos(-1);
		
		//	find the comment
		int cIndex = string.isEmpty() || sch_.comment.isEmpty() ? -1 : string.indexOf(sch_.comment);
		if (findBlockBegin(string, blIndex, begPos)) {	
			//	there is a block beginning
			if (cIndex >= 0 && cIndex < begPos) {	
				//	comment is found and it goes the first
				setFormat(indent + cIndex, string.length() - cIndex, sch_.commentFmt);
				highlightTheString(string.left(cIndex), indent, blType);
			}
			else {	
				//	comment is not found or goes after the block beginning
				applyRules(string.left(begPos), indent);	// highlight the beginning of the string

				setCurrentBlockState(blIndex + 1);
				int blBegLen = sch_.blocks[blIndex].begLen;
				highlightTheString(string.right(string.length() - begPos - blBegLen), indent + begPos + blBegLen, blIndex);
			}
		}
		else {	
			//	no block beginning
			if (cIndex >= 0) {	
				//	comment is found
				setFormat(indent + cIndex, string.length() - cIndex, sch_.commentFmt);
				highlightTheString(string.left(cIndex), indent, blType);
			}
			else {	
				//	there is no comment
				applyRules(string, indent);
				setCurrentBlockState(0);
			}
		}
	}
}

void HL::applyRules(const QString& str, int indent) {
	//	rules
	foreach (HLRule rule, sch_.rules) {
		if (!rule.regExp.isValid() || rule.regExp.isEmpty())
			continue;
			
		QRegExp expr(rule.regExp);
		int index = str.isEmpty() ? -1 : expr.indexIn(str);
		while (index >= 0) {
			QStringList matches = expr.capturedTexts();
			QString exprString = matches[0];
			int length = exprString.length();
			if (length == 0)
				break;

			if (rule.formats.count() == 1 && rule.formats.contains(0)) {	
				//	only style for the whole expression is set
				setFormat(index + indent, expr.matchedLength(), rule.formats[0]);
			}
			else {
				int matchCount = matches.count();
				for (int i = 1; i < matchCount; i++) {
					if (!rule.formats.contains(i))
						continue;
					QString match = matches[i];
					int position = exprString.indexOf(match);
					setFormat(index + indent + position, match.length(), rule.formats[i]);
				}
			}
			index = expr.indexIn(str, index + length);
		}
	}
}

