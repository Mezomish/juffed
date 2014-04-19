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


#include "keyword.h"


/*****************************************************************************
 *****************************************************************************/
Keywords::Keywords() {
    unsigned int i;
    table = xMalloc (TableSize, hashEntry*);
    for (i = 0  ;  i < TableSize  ;  ++i)
        table [i] = NULL;
}


/*****************************************************************************
 *****************************************************************************/
Keywords::~Keywords() {
    if (table != NULL)
    {
        unsigned int i;

        for (i = 0  ;  i < TableSize  ;  ++i)
        {
            hashEntry *entry = table[i];

            while (entry != NULL)
            {
                hashEntry *next = entry->next;
                eFree (entry);
                entry = next;
            }
        }
        eFree (table);
    }
}


/*****************************************************************************
 *****************************************************************************/
Keywords::hashEntry *Keywords::getHashTableEntry (unsigned long hashedValue)
{
    hashEntry *entry;

    Assert (hashedValue < TableSize);
    entry = table [hashedValue];

    return entry;
}


/*****************************************************************************
 *****************************************************************************/
unsigned long Keywords::hashValue (const char *const string)
{
    unsigned long value = 0;
    const unsigned char *p;

    Assert (string != NULL);

    /*  We combine the various words of the multiword key using the method
         *  described on page 512 of Vol. 3 of "The Art of Computer Programming".
         */
    for (p = (const unsigned char *) string  ;  *p != '\0'  ;  ++p)
    {
        value <<= 1;
        if (value & 0x00000100L)
            value = (value & 0x000000ffL) + 1L;
        value ^= *p;
    }
    /*  Algorithm from page 509 of Vol. 3 of "The Art of Computer Programming"
         *  Treats "value" as a 16-bit integer plus 16-bit fraction.
         */
    value *= 40503L;               /* = 2^16 * 0.6180339887 ("golden ratio") */
    value &= 0x0000ffffL;          /* keep fractional part */
    value >>= 16 - HASH_EXPONENT;  /* scale up by hash size and move down */

    return value;
}


/*****************************************************************************
 *****************************************************************************/
Keywords::hashEntry *Keywords::newEntry (
                const char *const string, Language language, int value)
{
    hashEntry *const entry = xMalloc (1, hashEntry);

    entry->next     = NULL;
    entry->string   = string;
    entry->language = language;
    entry->value    = value;

    return entry;
}


/*****************************************************************************
  Note that it is assumed that a "value" of zero means an undefined keyword
  and clients of this function should observe this. Also, all keywords added
  should be added in lower case. If we encounter a case-sensitive language
  whose keywords are in upper case, we will need to redesign this.
 *****************************************************************************/
void Keywords::addKeyword (const char *const string, Language language, int value)
{
    const unsigned long hashedValue = hashValue (string);
    hashEntry *entry = getHashTableEntry (hashedValue);

    if (entry == NULL)
    {
        table [hashedValue] = newEntry (string, language, value);
    }
    else
    {
        hashEntry *prev = NULL;

        while (entry != NULL)
        {
            if (language == entry->language  &&
                strcmp (string, entry->string) == 0)
            {
                Assert (("Already in table" == NULL));
            }
            prev = entry;
            entry = entry->next;
        }
        if (entry == NULL)
        {
            Assert (prev != NULL);
            prev->next = newEntry (string, language, value);
        }
    }
}


/*****************************************************************************
 *****************************************************************************/
int Keywords::lookupKeyword (const char *const string, Language language)
{
    const unsigned long hashedValue = hashValue (string);
    hashEntry *entry = getHashTableEntry (hashedValue);
    int result = -1;

    while (entry != NULL)
    {
        if (language == entry->language  &&  strcmp (string, entry->string) == 0)
        {
            result = entry->value;
            break;
        }
        entry = entry->next;
    }
    return result;
}


/*****************************************************************************
 *****************************************************************************/
int Keywords::analyzeToken (vString *const name, Language language)
{
    vString *keyword = vStringNew ();
    int result;
    vStringCopyToLower (keyword, name);
    result = lookupKeyword (vStringValue (keyword), language);
    vStringDelete (keyword);
    return result;
}

#ifdef DEBUG


/*****************************************************************************
 *****************************************************************************/
void Keywords::printEntry (const hashEntry *const entry)
{
    printf ("  %-15s %-7s\n", entry->string, getLanguageName (entry->language));
}


/*****************************************************************************
 *****************************************************************************/
unsigned int Keywords::printBucket (const unsigned int i)
{
    hashEntry **const table = getHashTable ();
    hashEntry *entry = table [i];
    unsigned int measure = 1;
    boolean first = true;

    printf ("%2d:", i);
    if (entry == NULL)
        printf ("\n");
    else while (entry != NULL)
    {
        if (! first)
            printf ("    ");
        else
        {
            printf (" ");
            first = false;
        }
        printEntry (entry);
        entry = entry->next;
        measure = 2 * measure;
    }
    return measure - 1;
}


/*****************************************************************************
 *****************************************************************************/
void Keywords::printKeywordTable (void)
{
//    unsigned long emptyBucketCount = 0;
//    unsigned long measure = 0;
//    unsigned int i;
//
//    for (i = 0  ;  i < TableSize  ;  ++i)
//    {
//        const unsigned int pass = printBucket (i);
//
//        measure += pass;
//        if (pass == 0)
//            ++emptyBucketCount;
//    }
//
//    printf ("spread measure = %ld\n", measure);
//    printf ("%ld empty buckets\n", emptyBucketCount);
}

#endif
