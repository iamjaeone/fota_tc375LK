/**************************************************************************
**                                                                        *
**  FILE        :  uchar.h                                                *
**                                                                        *
**  DESCRIPTION :  Unicode utilities.                                     *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _UCHAR_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 96,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.3,2.5,21.1,21.2
#endif

#define _UCHAR_H        1

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

#ifndef _SIZE_T_DEF
typedef __size_t        size_t;
#define _SIZE_T_DEF     1
#endif

#ifndef _WCHAR_T
typedef __wchar_t       wchar_t;
#define _WCHAR_T        1
#endif

#ifndef _MBSTATE_T
#define _MBSTATE_T      1

typedef struct
{
        wchar_t         wc_value;       /* wide character value solved so far */
        unsigned short  n_bytes;        /* number of bytes of solved multibyte */
        unsigned short  encoding;       /* encoding rule for wide character <=> multibyte conversion */
} mbstate_t;

#endif

#if     __CHAR_BIT__ >= 16
typedef unsigned char           char16_t;
#else
typedef unsigned short          char16_t;
#endif

#if     __CHAR_BIT__ >= 32
typedef unsigned char           char32_t;
#elif   __SHORT_BIT__ >= 32
typedef unsigned short          char32_t;
#elif   __INT_BIT__ >= 32
typedef unsigned int            char32_t;
#else
typedef unsigned long           char32_t;
#endif

#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

extern  size_t  mbrtoc16(char16_t * restrict pc16, const char * restrict s, size_t n, mbstate_t * restrict ps);
extern  size_t  c16rtomb(char * restrict s, char16_t c16, mbstate_t * restrict ps);
extern  size_t  mbrtoc32(char32_t * restrict pc32, const char * restrict s, size_t n, mbstate_t * restrict ps);
extern  size_t  c32rtomb(char * restrict s, char32_t c32, mbstate_t * restrict ps);

#if   __MISRAC_VERSION__ > 0
#pragma nomisrac restore
#endif

#endif  /* _UCHAR_H */
