/**************************************************************************
**                                                                        *
**  FILE        :  ctype.h                                                *
**                                                                        *
**  DESCRIPTION :  Include file with macros and definitions for functions *
**                 testing characters                                     *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _CTYPE_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 1,13,93,114,115
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 1.1,6.3,19.7,20.1,20.2
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 1.2,2.5,8.2,21.1,21.2
#endif

#define _CTYPE_H        1

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

#define __CT_WHITE      0x01
#define __CT_CNTRL      0x02
#define __CT_SPACE      0x04
#define __CT_UPPER      0x08
#define __CT_LOWER      0x10
#define __CT_DIGIT      0x20
#define __CT_PUNCT      0x40
#define __CT_HEXAD      0x80

#define _toupper(c)     ((c)-'a'+'A')
#define _tolower(c)     ((c)-'A'+'a')

#define __CTYPE_SIZE    257
extern __far    const unsigned char   __ctype[__CTYPE_SIZE];

extern  int     isalpha(int);
extern  int     isupper(int);
extern  int     islower(int);
extern  int     isdigit(int);
extern  int     isxdigit(int);
extern  int     isspace(int);
extern  int     isblank(int);
extern  int     ispunct(int);
extern  int     isalnum(int);
extern  int     isprint(int);
extern  int     isgraph(int);
extern  int     iscntrl(int);
extern  int     isascii(int);
extern  int     toascii(int);
extern  int     toupper(int);
extern  int     tolower(int);

#if     __CHAR_BIT__ == 8 || defined __CTYPE_MACROS
#if     __CHAR_BIT__ > 8
#  define __ISX(c,test) ((unsigned)(c) <= 255 && (test))
#else
#  define __ISX(c,test) (test)
#endif
#define isalnum(c)      (__ISX(c,(__ctype+1)[c]&(__CT_UPPER|__CT_LOWER|__CT_DIGIT)))
#define isalpha(c)      (__ISX(c,(__ctype+1)[c]&(__CT_UPPER|__CT_LOWER)))
#define isblank(c)      (__ISX(c,((__ctype+1)[c]+__CT_WHITE)&__CT_SPACE))
#define iscntrl(c)      (__ISX(c,(__ctype+1)[c]&(__CT_CNTRL|__CT_WHITE)))
#define isdigit(c)      (__ISX(c,(__ctype+1)[c]&__CT_DIGIT))
#define isgraph(c)      (__ISX(c,(__ctype+1)[c]&(__CT_PUNCT|__CT_UPPER|__CT_LOWER|__CT_DIGIT)))
#define islower(c)      (__ISX(c,(__ctype+1)[c]&__CT_LOWER))
#define isprint(c)      (__ISX(c,(__ctype+1)[c]&(__CT_PUNCT|__CT_UPPER|__CT_LOWER|__CT_DIGIT|__CT_SPACE)))
#define ispunct(c)      (__ISX(c,(__ctype+1)[c]&__CT_PUNCT))
#define isspace(c)      (__ISX(c,(__ctype+1)[c]&(__CT_SPACE|__CT_WHITE)))
#define isupper(c)      (__ISX(c,(__ctype+1)[c]&__CT_UPPER))
#define isxdigit(c)     (__ISX(c,(__ctype+1)[c]&(__CT_DIGIT|__CT_HEXAD)))
#endif
#ifdef __cplusplus
}
#endif  /* defined(__cplusplus) */

#pragma nomisrac restore

#endif  /* _CTYPE_H */
