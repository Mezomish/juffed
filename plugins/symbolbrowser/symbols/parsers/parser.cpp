/***************************************************************************
  A SymbolBrowser plugin for JuffEd editor.

  Copyright: 2010 Aleksander Sokoloff <asokol@mail.ru>

  Some of the code based on a ctags project http://ctags.sourceforge.net/

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have rceived a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
***************************************************************************/

#include <stdio.h>
#include <QDebug>
#include <QStringList>
#include "parser.h"
#include "../symbol.h"


/*****************************************************************************
 *****************************************************************************/
Parser::Parser(): QObject() {
    mTextBuf = mTextBufEnd = NULL;
    mNChar = mRChar = NULL;
    mUngetChar = EOF;
    mStopped = true;
    mLineNum = 0;
}


/*****************************************************************************
 *****************************************************************************/
Parser::~Parser() {

}


/*****************************************************************************
 *****************************************************************************/
void Parser::run(char *textBuf, int textBufLen, Symbol *symbols) {
    mSymbols = symbols;
    mTextBuf = textBuf;
    mTextBufEnd =  mTextBuf;
    mTextBufEnd += textBufLen;
    resetText();

    mStopped = false;
    parse();
}

/*****************************************************************************
 *****************************************************************************/
void Parser::stop() {
    mStopped = true;
}


/*****************************************************************************
 This function reads a single character, performing newline
 canonicalization.
 To not mix use of fileReadLine () and fileGetc () for the same file.
 *****************************************************************************/
int Parser::fileGetc(){
    if (mStopped)
        return EOF;

    if (mCurPos == mTextBufEnd)
        return EOF;

    if (mUngetChar != EOF){
        int res = mUngetChar;
        mUngetChar = EOF;
        return res;
    }

    char result = *mCurPos;
    if (mCurPos>mTextBuf && *(mCurPos-1) == '\n')
        mLineNum++;

    mCurPos++;
    return result;
}


/*****************************************************************************
 An alternative interface to fileGetc (). Do not mix use of fileReadLine()
 and fileGetc() for the same file. The returned string does not contain
 the terminating newline. A NULL return value means that all lines in the
 file have been read and we are at the end of file.
 *****************************************************************************/
const char *Parser::fileReadLine(){
    if (mStopped || mCurPos == mTextBufEnd)
        return NULL;

    if (mRChar) *mRChar = '\r';
    if (mNChar) *mNChar = '\n';

    char *start = mCurPos;
    for(; mCurPos < mTextBufEnd; ++mCurPos) {

        if (char(*mCurPos) == '\n') {
            mNChar = mCurPos;
            mRChar = NULL;
            *mCurPos = '\0';
            mCurPos++;
            break;
        }

        if (char(*mCurPos) == '\r') {
            mRChar = mCurPos;
            mNChar = NULL;
            *mCurPos = '\0';
            mCurPos += 2;
            break;
        }

    }

    // For first text line mLineNum already have 0.
    if (mCurPos > mTextBuf) ++mLineNum;

    return start;
}


/*****************************************************************************
 *****************************************************************************/
void Parser::fileUngetc(int c){
    mUngetChar = c;
}


/*****************************************************************************
 *****************************************************************************/
unsigned long Parser::getSourceLineNumber(){
    return mLineNum;
}


/*****************************************************************************
 *****************************************************************************/
unsigned long Parser::getInputLineNumber(){
    return mLineNum;
}

/************************************************
 *
 ************************************************/
void Parser::resetText() {
    mCurPos = mTextBuf;
    if (mRChar) *mRChar = '\r';
    if (mNChar) *mNChar = '\n';
    mRChar = NULL;
    mNChar = NULL;
}

