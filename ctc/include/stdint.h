/**************************************************************************
**                                                                        *
**  FILE        :  stdint.h                                               *
**                                                                        *
**  DESCRIPTION :  Integer Types                                          *
**                                                                        *
**  This header file uses the following predefined macros:                *
**                                                                        *
**  __CHAR_BIT__                                                          *
**  __SHORT_BIT__                                                         *
**  __INT_BIT__                                                           *
**  __LONG_BIT__                                                          *
**  __LLONG_BIT__        (undefined for ISO-C90)                          *
**                                                                        *
**  __INTPTR_BIT__                                                        *
**  __PTRDIFF_BIT__                                                       *
**  __SIZE_BIT__                                                          *
**  __WCHAR_BIT__                                                         *
**                                                                        *
**  __WCHAR_UNSIGNED__                                                    *
**                                                                        *
**  The fastest minimum-width integer types are by default equal to the   *
**  corresponding minimum-width integer types, but they may be overruled  *
**  by the compiler with predefined macros like:                          *
**                                                                        *
**    #define __FAST8__  __LEAST32__                                      *
**    #define __FAST16__ __LEAST32__                                      *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _STDINT_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 90,93,96,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,19.7,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.3,2.5,21.1,21.2
#endif

#define _STDINT_H       1

#include "typeinfo.h"

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

/* Exact-width integer types */

#if     __CHAR_BIT__ == 8
#  define       __EXACT8__      __CHAR
#endif

#if     __INT_BIT__ == 16
#  define       __EXACT16__     __INT
#elif   __SHORT_BIT__ == 16
#  define       __EXACT16__     __SHORT
#elif   __CHAR_BIT__ == 16
#  define       __EXACT16__     __CHAR
#endif

#if     __INT_BIT__ == 24
#  define       __EXACT24__     __INT
#endif

#if     __INT_BIT__ == 32
#  define       __EXACT32__     __INT
#elif   __LONG_BIT__ == 32
#  define       __EXACT32__     __LONG
#elif   __SHORT_BIT__ == 32
#  define       __EXACT32__     __SHORT
#elif   __CHAR_BIT__ == 32
#  define       __EXACT32__     __CHAR
#endif

#if     __LONG_BIT__ == 48
#  define       __EXACT48__     __LONG
#endif

#if     __INT_BIT__ == 64
#  define       __EXACT64__     __INT
#elif   __LONG_BIT__ == 64
#  define       __EXACT64__     __LONG
#elif   __LLONG_BIT__ == 64
#  define       __EXACT64__     __LLONG
#elif   __SHORT_BIT__ == 64
#  define       __EXACT64__     __SHORT
#elif   __CHAR_BIT__ == 64
#  define       __EXACT64__     __CHAR
#endif

#if     __LLONG_BIT__ == 72
#  define       __EXACT72__     __LLONG
#endif

/* Minimum-width integer types */

#define         __LEAST8__      __CHAR  /* char is at least 8 bit */

#if     __CHAR_BIT__ >= 16
#  define       __LEAST16__     __CHAR
#else
#  define       __LEAST16__     __SHORT /* short is at least 16 bit */
#endif

#if     __CHAR_BIT__ >= 24
#  define       __LEAST24__     __CHAR
#elif   __SHORT_BIT__ >= 24
#  define       __LEAST24__     __SHORT
#elif   __INT_BIT__ >= 24
#  define       __LEAST24__     __INT
#else
#  define       __LEAST24__     __LONG  /* long is at least 32 bit */
#endif

#if     __CHAR_BIT__ >= 32
#  define       __LEAST32__     __CHAR
#elif   __SHORT_BIT__ >= 32
#  define       __LEAST32__     __SHORT
#elif   __INT_BIT__ >= 32
#  define       __LEAST32__     __INT
#else
#  define       __LEAST32__     __LONG  /* long is at least 32 bit */
#endif

#if     __CHAR_BIT__ >= 48
#  define       __LEAST48__     __CHAR
#elif   __SHORT_BIT__ >= 48
#  define       __LEAST48__     __SHORT
#elif   __INT_BIT__ >= 48
#  define       __LEAST48__     __INT
#elif   __LONG_BIT__ >= 48
#  define       __LEAST48__     __LONG
#elif   __LLONG_BIT__ >= 48
#  define       __LEAST48__     __LLONG /* __LEAST48__ is optional */
#endif

#if     __CHAR_BIT__ >= 64
#  define       __LEAST64__     __CHAR
#elif   __SHORT_BIT__ >= 64
#  define       __LEAST64__     __SHORT
#elif   __INT_BIT__ >= 64
#  define       __LEAST64__     __INT
#elif   __LONG_BIT__ >= 64
#  define       __LEAST64__     __LONG
#elif   __LLONG_BIT__ >= 64
#  define       __LEAST64__     __LLONG /* __LEAST64__ is optional */
#endif

#if     __CHAR_BIT__ >= 72
#  define       __LEAST72__     __CHAR
#elif   __SHORT_BIT__ >= 72
#  define       __LEAST72__     __SHORT
#elif   __INT_BIT__ >= 72
#  define       __LEAST72__     __INT
#elif   __LONG_BIT__ >= 72
#  define       __LEAST72__     __LONG
#elif   __LLONG_BIT__ >= 72
#  define       __LEAST72__     __LLONG /* __LEAST72__ is optional */
#endif

/* Fastest minimum-width integer types (may be overruled by the compiler) */

#if     ! defined __FAST8__
#  define       __FAST8__       __LEAST8__
#endif
#if     ! defined __FAST16__
#  define       __FAST16__      __LEAST16__
#endif
#if     ! defined __FAST24__
#  define       __FAST24__      __LEAST24__
#endif
#if     ! defined __FAST32__
#  define       __FAST32__      __LEAST32__
#endif
#if     ! defined __FAST48__ && defined __LEAST48__
#  define       __FAST48__      __LEAST48__
#endif
#if     ! defined __FAST64__ && defined __LEAST64__
#  define       __FAST64__      __LEAST64__
#endif
#if     ! defined __FAST72__ && defined __LEAST72__
#  define       __FAST72__      __LEAST72__
#endif

/* Integer type capable of holding object pointers */

#if     __INTPTR_BIT__ <= __INT_BIT__
#  define       __INTPTR__      __INT
#elif   __INTPTR_BIT__ <= __LONG_BIT__
#  define       __INTPTR__      __LONG
#else
#  define       __INTPTR__      __LLONG
#endif

/* Greatest-width integer type */

#if     __LLONG_BIT__ > __LONG_BIT__
#  define       __INTMAX__      __LLONG
#else
#  define       __INTMAX__      __LONG
#endif

/* Integer type capable of holding pointer differences */

#if     __PTRDIFF_BIT__ <= __INT_BIT__
#  define       __PTRDIFF__     __INT
#elif   __PTRDIFF_BIT__ <= __LONG_BIT__
#  define       __PTRDIFF__     __LONG
#else
#  define       __PTRDIFF__     __LLONG
#endif

/* Result type of the sizeof operator */

#if     __SIZE_BIT__ <= __INT_BIT__
#  define       __SIZE__        __INT
#elif   __SIZE_BIT__ <= __LONG_BIT__
#  define       __SIZE__        __LONG
#else
#  define       __SIZE__        __LLONG
#endif

/* Integer type capable of holding wide characters */

#if     __WCHAR_BIT__ <= __CHAR_BIT__ && __WCHAR_BIT__ != __INT_BIT__
#  define       __WCHAR__       __CHAR
#elif   __WCHAR_BIT__ <= __SHORT_BIT__ && __WCHAR_BIT__ != __INT_BIT__
#  define       __WCHAR__       __SHORT
#else
#  define       __WCHAR__       __INT
#endif

/*
 * 7.18.1.1 Exact-width integer types
 */
#ifdef  __EXACT8__
typedef signed   __TYPE(__EXACT8__)     int8_t;
typedef unsigned __TYPE(__EXACT8__)     uint8_t;
#endif
#ifdef  __EXACT16__
typedef signed   __TYPE(__EXACT16__)    int16_t;
typedef unsigned __TYPE(__EXACT16__)    uint16_t;
#endif
#ifdef  __EXACT24__
typedef signed   __TYPE(__EXACT24__)    int24_t;
typedef unsigned __TYPE(__EXACT24__)    uint24_t;
#endif
#ifdef  __EXACT32__
typedef signed   __TYPE(__EXACT32__)    int32_t;
typedef unsigned __TYPE(__EXACT32__)    uint32_t;
#endif
#ifdef  __EXACT48__
typedef signed   __TYPE(__EXACT48__)    int48_t;
typedef unsigned __TYPE(__EXACT48__)    uint48_t;
#endif
#ifdef  __EXACT64__
typedef signed   __TYPE(__EXACT64__)    int64_t;
typedef unsigned __TYPE(__EXACT64__)    uint64_t;
#endif
#ifdef  __EXACT72__
typedef signed   __TYPE(__EXACT72__)    int72_t;
typedef unsigned __TYPE(__EXACT72__)    uint72_t;
#endif

/*
 * 7.18.1.2 Minimum-width integer types
 */
typedef signed   __TYPE(__LEAST8__)     int_least8_t;
typedef unsigned __TYPE(__LEAST8__)     uint_least8_t;
typedef signed   __TYPE(__LEAST16__)    int_least16_t;
typedef unsigned __TYPE(__LEAST16__)    uint_least16_t;
typedef signed   __TYPE(__LEAST24__)    int_least24_t;
typedef unsigned __TYPE(__LEAST24__)    uint_least24_t;
typedef signed   __TYPE(__LEAST32__)    int_least32_t;
typedef unsigned __TYPE(__LEAST32__)    uint_least32_t;
#ifdef __LEAST48__
typedef signed   __TYPE(__LEAST48__)    int_least48_t;
typedef unsigned __TYPE(__LEAST48__)    uint_least48_t;
#endif
#ifdef __LEAST64__
typedef signed   __TYPE(__LEAST64__)    int_least64_t;
typedef unsigned __TYPE(__LEAST64__)    uint_least64_t;
#endif
#ifdef __LEAST72__
typedef signed   __TYPE(__LEAST72__)    int_least72_t;
typedef unsigned __TYPE(__LEAST72__)    uint_least72_t;
#endif

/*
 * 7.18.1.3 Fastest minimum-width integer types
 */
typedef signed   __TYPE(__FAST8__)      int_fast8_t;
typedef unsigned __TYPE(__FAST8__)      uint_fast8_t;
typedef signed   __TYPE(__FAST16__)     int_fast16_t;
typedef unsigned __TYPE(__FAST16__)     uint_fast16_t;
typedef signed   __TYPE(__FAST24__)     int_fast24_t;
typedef unsigned __TYPE(__FAST24__)     uint_fast24_t;
typedef signed   __TYPE(__FAST32__)     int_fast32_t;
typedef unsigned __TYPE(__FAST32__)     uint_fast32_t;
#ifdef __FAST48__
typedef signed   __TYPE(__FAST48__)     int_fast48_t;
typedef unsigned __TYPE(__FAST48__)     uint_fast48_t;
#endif
#ifdef __FAST64__
typedef signed   __TYPE(__FAST64__)     int_fast64_t;
typedef unsigned __TYPE(__FAST64__)     uint_fast64_t;
#endif
#ifdef __FAST72__
typedef signed   __TYPE(__FAST72__)     int_fast72_t;
typedef unsigned __TYPE(__FAST72__)     uint_fast72_t;
#endif

/*
 * 7.18.1.4 Integer types capable of holding object pointers
 */
typedef __intptr_t                      intptr_t;
typedef __uintptr_t                     uintptr_t;

/*
 * 7.18.1.5 Greatest-width integer types
 */
typedef signed   __TYPE(__INTMAX__)     intmax_t;
typedef unsigned __TYPE(__INTMAX__)     uintmax_t;

/*
 * 7.18.2.1 Limits of exact-width integer types
 */
#ifdef  __EXACT8__
#  define       INT8_MIN                __SMIN(__EXACT8__)
#  define       INT8_MAX                __SMAX(__EXACT8__)
#  define       UINT8_MAX               __UMAX(__EXACT8__)
#endif

#ifdef  __EXACT16__
#  define       INT16_MIN               __SMIN(__EXACT16__)
#  define       INT16_MAX               __SMAX(__EXACT16__)
#  define       UINT16_MAX              __UMAX(__EXACT16__)
#endif

#ifdef  __EXACT24__
#  define       INT24_MIN               __SMIN(__EXACT24__)
#  define       INT24_MAX               __SMAX(__EXACT24__)
#  define       UINT24_MAX              __UMAX(__EXACT24__)
#endif

#ifdef  __EXACT32__
#  define       INT32_MIN               __SMIN(__EXACT32__)
#  define       INT32_MAX               __SMAX(__EXACT32__)
#  define       UINT32_MAX              __UMAX(__EXACT32__)
#endif

#ifdef  __EXACT48__
#  define       INT48_MIN               __SMIN(__EXACT48__)
#  define       INT48_MAX               __SMAX(__EXACT48__)
#  define       UINT48_MAX              __UMAX(__EXACT48__)
#endif

#ifdef  __EXACT64__
#  define       INT64_MIN               __SMIN(__EXACT64__)
#  define       INT64_MAX               __SMAX(__EXACT64__)
#  define       UINT64_MAX              __UMAX(__EXACT64__)
#endif

#ifdef  __EXACT72__
#  define       INT72_MIN               __SMIN(__EXACT72__)
#  define       INT72_MAX               __SMAX(__EXACT72__)
#  define       UINT72_MAX              __UMAX(__EXACT72__)
#endif

/*
 * 7.18.2.2 Limits of minimum-width integer types
 */
#define         INT_LEAST8_MIN          __SMIN(__LEAST8__)
#define         INT_LEAST8_MAX          __SMAX(__LEAST8__)
#define         UINT_LEAST8_MAX         __UMAX(__LEAST8__)

#define         INT_LEAST16_MIN         __SMIN(__LEAST16__)
#define         INT_LEAST16_MAX         __SMAX(__LEAST16__)
#define         UINT_LEAST16_MAX        __UMAX(__LEAST16__)

#define         INT_LEAST24_MIN         __SMIN(__LEAST24__)
#define         INT_LEAST24_MAX         __SMAX(__LEAST24__)
#define         UINT_LEAST24_MAX        __UMAX(__LEAST24__)

#define         INT_LEAST32_MIN         __SMIN(__LEAST32__)
#define         INT_LEAST32_MAX         __SMAX(__LEAST32__)
#define         UINT_LEAST32_MAX        __UMAX(__LEAST32__)

#ifdef  __LEAST48__
#  define       INT_LEAST48_MIN         __SMIN(__LEAST48__)
#  define       INT_LEAST48_MAX         __SMAX(__LEAST48__)
#  define       UINT_LEAST48_MAX        __UMAX(__LEAST48__)
#endif

#ifdef  __LEAST64__
#  define       INT_LEAST64_MIN         __SMIN(__LEAST64__)
#  define       INT_LEAST64_MAX         __SMAX(__LEAST64__)
#  define       UINT_LEAST64_MAX        __UMAX(__LEAST64__)
#endif

#ifdef  __LEAST72__
#  define       INT_LEAST72_MIN         __SMIN(__LEAST72__)
#  define       INT_LEAST72_MAX         __SMAX(__LEAST72__)
#  define       UINT_LEAST72_MAX        __UMAX(__LEAST72__)
#endif

/*
 * 7.18.2.3 Limits of fastest minimum-width integer types
 */
#define         INT_FAST8_MIN           __SMIN(__FAST8__)
#define         INT_FAST8_MAX           __SMAX(__FAST8__)
#define         UINT_FAST8_MAX          __UMAX(__FAST8__)

#define         INT_FAST16_MIN          __SMIN(__FAST16__)
#define         INT_FAST16_MAX          __SMAX(__FAST16__)
#define         UINT_FAST16_MAX         __UMAX(__FAST16__)

#define         INT_FAST24_MIN          __SMIN(__FAST24__)
#define         INT_FAST24_MAX          __SMAX(__FAST24__)
#define         UINT_FAST24_MAX         __UMAX(__FAST24__)

#define         INT_FAST32_MIN          __SMIN(__FAST32__)
#define         INT_FAST32_MAX          __SMAX(__FAST32__)
#define         UINT_FAST32_MAX         __UMAX(__FAST32__)

#ifdef  __FAST48__
#  define       INT_FAST48_MIN          __SMIN(__FAST48__)
#  define       INT_FAST48_MAX          __SMAX(__FAST48__)
#  define       UINT_FAST48_MAX         __UMAX(__FAST48__)
#endif

#ifdef  __FAST64__
#  define       INT_FAST64_MIN          __SMIN(__FAST64__)
#  define       INT_FAST64_MAX          __SMAX(__FAST64__)
#  define       UINT_FAST64_MAX         __UMAX(__FAST64__)
#endif

#ifdef  __FAST72__
#  define       INT_FAST72_MIN          __SMIN(__FAST72__)
#  define       INT_FAST72_MAX          __SMAX(__FAST72__)
#  define       UINT_FAST72_MAX         __UMAX(__FAST72__)
#endif

/*
 * 7.18.2.4 Limits of integer types capable of holding object pointers
 */
#define         INTPTR_MIN              __SMIN(__INTPTR__)
#define         INTPTR_MAX              __SMAX(__INTPTR__)
#define         UINTPTR_MAX             __UMAX(__INTPTR__)

/*
 * 7.18.2.5 Limits of greatest-width integer types
 */
#define         INTMAX_MIN              __SMIN(__INTMAX__)
#define         INTMAX_MAX              __SMAX(__INTMAX__)
#define         UINTMAX_MAX             __UMAX(__INTMAX__)

/*
 * 7.18.3 Limits of other integer types
 */
#define         PTRDIFF_MIN             __SMIN(__PTRDIFF__)
#define         PTRDIFF_MAX             __SMAX(__PTRDIFF__)

#define         SIG_ATOMIC_MIN          __SMIN(__INT)
#define         SIG_ATOMIC_MAX          __SMAX(__INT)

#define         SIZE_MAX                __UMAX(__SIZE__)

#ifndef         WCHAR_MIN
#  ifdef        __WCHAR_UNSIGNED__
#    define     WCHAR_MIN               __UMIN(__WCHAR__)
#  else
#    define     WCHAR_MIN               __SMIN(__WCHAR__)
#  endif
#endif
#ifndef         WCHAR_MAX
#  ifdef        __WCHAR_UNSIGNED__
#    define     WCHAR_MAX               __UMAX(__WCHAR__)
#  else
#    define     WCHAR_MAX               __SMAX(__WCHAR__)
#  endif
#endif

#define         WINT_MIN                __UMIN(__INT)
#define         WINT_MAX                __UMAX(__INT)

/*
 * 7.18.4.1 Macros for minimum-width integer constants
 */
#define         INT8_C(value)           __ICON(__LEAST8__,value,)
#define         UINT8_C(value)          __ICON(__LEAST8__,value,U)
#define         INT16_C(value)          __ICON(__LEAST16__,value,)
#define         UINT16_C(value)         __ICON(__LEAST16__,value,U)
#define         INT24_C(value)          __ICON(__LEAST24__,value,)
#define         UINT24_C(value)         __ICON(__LEAST24__,value,U)
#define         INT32_C(value)          __ICON(__LEAST32__,value,)
#define         UINT32_C(value)         __ICON(__LEAST32__,value,U)
#ifdef  __LEAST48__
#  define       INT48_C(value)          __ICON(__LEAST48__,value,)
#  define       UINT48_C(value)         __ICON(__LEAST48__,value,U)
#endif
#ifdef  __LEAST64__
#  define       INT64_C(value)          __ICON(__LEAST64__,value,)
#  define       UINT64_C(value)         __ICON(__LEAST64__,value,U)
#endif
#ifdef  __LEAST72__
#  define       INT72_C(value)          __ICON(__LEAST72__,value,)
#  define       UINT72_C(value)         __ICON(__LEAST72__,value,U)
#endif

/*
 * 7.18.4.2 Macros for greatest-width integer constants
 */
#define         INTMAX_C(value)         __ICON(__INTMAX__,value,)
#define         UINTMAX_C(value)        __ICON(__INTMAX__,value,U)

#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#if   __MISRAC_VERSION__ >= 1998 && __MISRAC_VERSION__ <= 2012
#pragma nomisrac restore
#endif

#endif  /* _STDINT_H */
