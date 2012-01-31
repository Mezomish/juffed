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

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


#ifndef _VSTRING_H
#define _VSTRING_H

/*
*   INCLUDE FILES
*/
//#include "general.h"  /* must always come first */
#include <assert.h>
#include <QString>
#include <QDebug>

typedef bool boolean;

#if defined(HAVE_STDLIB_H)
# include <stdlib.h>  /* to define size_t */
#endif

/*
*   MACROS
*/


/***********************
 ***********************/
#ifdef DEBUG
#include <assert.h>
# define debug(level)           ((Option.debugLevel & (long)(level)) != 0)
# define DebugStatement(x)      x
# define PrintStatus(x)         if (debug(DEBUG_STATUS)) printf x;
# define Assert(c)              assert(c)
# define verbose(format, ...)   fprintf(stderr, __VA_ARGS__)
#else
# define DebugStatement(x)
# define PrintStatus(x)
# define Assert(c)
# define verbose(format, ...)
# ifndef NDEBUG
#  define NDEBUG
# endif
#endif


#define xMalloc(n,Type)    (Type *)eMalloc((size_t)(n) * sizeof (Type))
#define xCalloc(n,Type)    (Type *)eCalloc((size_t)(n), sizeof (Type))
#define xRealloc(p,n,Type) (Type *)eRealloc((p), (n) * sizeof (Type))


#ifndef DEBUG
# define VSTRING_PUTC_MACRO 1
#endif
#ifdef VSTRING_PUTC_MACRO
#define vStringPut(s,c) \
        (void)(((s)->length + 1 == (s)->size ? vStringAutoResize (s) : 0), \
        ((s)->buffer [(s)->length] = (c)), \
        ((c) == '\0' ? 0 : ((s)->buffer [++(s)->length] = '\0')))
#endif

#define vStringValue(vs)      ((vs)->buffer)
#define vStringItem(vs,i)     ((vs)->buffer[i])
#define vStringLast(vs)       ((vs)->buffer[(vs)->length - 1])
#define vStringLength(vs)     ((vs)->length)
#define vStringSize(vs)       ((vs)->size)
#define vStringCat(vs,s)      vStringCatS((vs), vStringValue((s)))
#define vStringNCat(vs,s,l)   vStringNCatS((vs), vStringValue((s)), (l))
#define vStringCopy(vs,s)     vStringCopyS((vs), vStringValue((s)))
#define vStringNCopy(vs,s,l)  vStringNCopyS((vs), vStringValue((s)), (l))
#define vStringChar(vs,i)     ((vs)->buffer[i])
#define vStringTerminate(vs)  vStringPut(vs, '\0')
#define vStringLower(vs)      toLowerString((vs)->buffer)
#define vStringUpper(vs)      toUpperString((vs)->buffer)

/*
*   DATA DECLARATIONS
*/

typedef struct sVString {
        size_t  length;  /* size of buffer used */
        size_t  size;    /* allocated size of buffer */
        char   *buffer;  /* location of buffer */
} vString;

/*
*   FUNCTION PROTOTYPES
*/
extern boolean vStringAutoResize (vString *const string);
extern void vStringClear (vString *const string);
extern vString *vStringNew (void);
extern void vStringDelete (vString *const string);
#ifndef VSTRING_PUTC_MACRO
extern void vStringPut (vString *const string, const int c);
#endif
extern void vStringStripNewline (vString *const string);
extern void vStringStripLeading (vString *const string);
extern void vStringChop (vString *const string);
extern void vStringStripTrailing (vString *const string);
extern void vStringCatS (vString *const string, const char *const s);
extern void vStringNCatS (vString *const string, const char *const s, const size_t length);
extern vString *vStringNewCopy (const vString *const string);
extern vString *vStringNewInit (const char *const s);
extern void vStringCopyS (vString *const string, const char *const s);
extern void vStringNCopyS (vString *const string, const char *const s, const size_t length);
extern void vStringCopyToLower (vString *const dest, const vString *const src);
extern void vStringSetLength (vString *const string);

extern QString vStringToQString(const vString *const string);




/* Memory allocation functions */
#ifdef NEED_PROTO_MALLOC
extern void *malloc (size_t);
extern void *realloc (void *ptr, size_t);
#endif
extern void *eMalloc (const size_t size);
extern void *eCalloc (const size_t count, const size_t size);
extern void *eRealloc (void *const ptr, const size_t size);
extern void eFree (void *const ptr);


QDebug& operator<< (QDebug &debug, const vString *v);




#endif  /* _VSTRING_H */


