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

#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QTextStream>
#include <QByteArray>
#include "../common.h"

class QString;
class Symbol;

class Parser: public QObject {
Q_OBJECT
public:
    Parser();
    virtual ~Parser();

    void run(char *textBuf, int textBufLen, Symbol *symbols);
    void stop();

    Language language() const { return mLanguage; };
    void setLanguage(Language value) { mLanguage = value; };

protected:
    int fileGetc();
    void fileUngetc(int c);
    const char *fileReadLine();
    void resetText();

    unsigned long getSourceLineNumber();
    unsigned long getInputLineNumber() ;

    virtual void parse()=0;

    Symbol *mSymbols;

private:
    Language    mLanguage;
    int mUngetChar;
    char *mTextBuf;
    char *mTextBufEnd;

    char *mCurPos;
    char *mRChar;
    char *mNChar;
    bool mStopped;
    unsigned long mLineNum;
};


typedef struct sKindOption {
        bool enabled;             /* are tags for kind enabled? */
        int letter;               /* kind letter */
        const char* name;         /* kind name */
        const char* description;  /* displayed in --help output */
} kindOption;

#endif // PARSER_H
