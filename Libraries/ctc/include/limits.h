/**************************************************************************
**                                                                        *
**  FILE        :  limits.h                                               *
**                                                                        *
**  DESCRIPTION :  Sizes of Integral Types                                *
**                                                                        *
**  This header file uses the following predefined macros:                *
**                                                                        *
**  __CHAR_BIT__                                                          *
**  __SHORT_BIT__                                                         *
**  __INT_BIT__                                                           *
**  __LONG_BIT__                                                          *
**  __LLONG_BIT__        (undefined for ISO-C90)                          *
**                                                                        *
**  __CHAR_UNSIGNED__                                                     *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _LIMITS_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,21.1,21.2
#endif

#define _LIMITS_H       1

#include "typeinfo.h"

#define         CHAR_BIT                __CHAR_BIT__

#define         SCHAR_MIN               __SMIN(__CHAR)
#define         SCHAR_MAX               __SMAX(__CHAR)
#define         UCHAR_MAX               __UMAX(__CHAR)

#ifdef  __CHAR_UNSIGNED__
#  define       CHAR_MIN                __UMIN(__CHAR)
#  define       CHAR_MAX                __UMAX(__CHAR)
#else
#  define       CHAR_MIN                __SMIN(__CHAR)
#  define       CHAR_MAX                __SMAX(__CHAR)
#endif

#define         MB_LEN_MAX              1

#define         SHRT_MIN                __SMIN(__SHORT)
#define         SHRT_MAX                __SMAX(__SHORT)
#define         USHRT_MAX               __UMAX(__SHORT)

#define         INT_MIN                 __SMIN(__INT)
#define         INT_MAX                 __SMAX(__INT)
#define         UINT_MAX                __UMAX(__INT)

#define         LONG_MIN                __SMIN(__LONG)
#define         LONG_MAX                __SMAX(__LONG)
#define         ULONG_MAX               __UMAX(__LONG)

#ifdef  __LLONG_BIT__
#  define       LLONG_MIN               __SMIN(__LLONG)
#  define       LLONG_MAX               __SMAX(__LLONG)
#  define       ULLONG_MAX              __UMAX(__LLONG)
#endif

#if   __MISRAC_VERSION__ >= 1998 && __MISRAC_VERSION__ <= 2012
#pragma nomisrac restore
#endif

#endif  /* _LIMITS_H */
