/***************************************************************************
  A SymbolBrowser plugin for JuffEd editor.

  Copyright: 2010 Aleksander Sokoloff <asokol@mail.ru>

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

#include <QDebug>
#include "parserthread.h"
#include "parsers/parser.h"
#include "parsers/parser_cpp.cpp"
#include "parsers/parser_python.cpp"
#include "parsers/parser_perl.cpp"


/*****************************************************************************
 *****************************************************************************/
ParserThread::ParserThread()
{
    mTextBuf = NULL;
    mLanguage = LanguageUnknown;
    mSymbols = new Symbol();
    mParser = NULL;
}


/*****************************************************************************
 *****************************************************************************/
ParserThread::~ParserThread() {
    delete mSymbols;
    free(mTextBuf);
}


/*****************************************************************************
 *****************************************************************************/
void ParserThread::setText(const QString &text) {
    QByteArray array = text.toUtf8();

    mTextBufLen = array.length() + 1;
    mTextBuf = (char*)malloc(mTextBufLen * sizeof(char));

    strncpy(mTextBuf, array.data(), mTextBufLen);
}


/*****************************************************************************
 *****************************************************************************/
void ParserThread::setLanguage(Language language) {
    mLanguage = language;
}


/*****************************************************************************
 *****************************************************************************/
void ParserThread::stop() {
    if (mParser)
        mParser->stop();
}


/*****************************************************************************
 *****************************************************************************/
void ParserThread::run() {

    switch (mLanguage) {

        case LanguageC:
        case LanguageCpp:
        case LanguageCsharp:
        case LanguageJava:
        case LanguageVera:
            mParser = new Parser_Cpp();
            break;

        case LanguagePython:
            mParser = new Parser_Python();
            break;

        case LanguagePerl:
            mParser = new Parser_Perl();
            break;

        default:
            return;
    }

    mParser->setLanguage(mLanguage);
    mParser->run(mTextBuf, mTextBufLen, mSymbols);
    delete mParser;
    mParser = NULL;
}
