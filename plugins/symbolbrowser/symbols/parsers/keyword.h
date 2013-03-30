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


#ifndef KEYWORD_H
#define KEYWORD_H

#include "../common.h"
#include "vstring.h"

#define HASH_EXPONENT 7  /* must be less than 17 */

class Keywords
{
public:
    Keywords();
    ~Keywords();

    void addKeyword (const char *const string, Language language, int value);
    int lookupKeyword (const char *const string, Language language);

    #ifdef DEBUG
    void printKeywordTable (void);
    #endif
    int analyzeToken (vString *const name, Language language);

private:
    typedef struct sHashEntry {
            struct sHashEntry *next;
            const char *string;
            Language language;
            int value;
    } hashEntry;

    static const unsigned int TableSize = 1 << HASH_EXPONENT;
    hashEntry **table;

    hashEntry *getHashTableEntry (unsigned long hashedValue);
    unsigned long hashValue (const char *const string);
    hashEntry *newEntry(const char *const string, Language language, int value);
    #ifdef DEBUG
    void printEntry (const hashEntry *const entry);
    unsigned int printBucket (const unsigned int i);
    #endif

};

#endif // KEYWORD_H
