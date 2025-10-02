/**************************************************************************
**                                                                        *
**  FILE        :  typeinfo.h                                             *
**                                                                        *
**  DESCRIPTION :  Macros to obtain type information                      *
**                                                                        *
**  This file defines a number of internal macros that are used by        *
**  other header files to obtain information about the integer types.     *
**  The "name" argument of these macros should one of:                    *
**     __CHAR, __SHORT, __INT, __LONG, __LLONG.                           *
**  The sizes of these types are defined by the following compiler        *
**  builtin macros:                                                       *
**                                                                        *
**  __CHAR_BIT__                                                          *
**  __SHORT_BIT__                                                         *
**  __INT_BIT__                                                           *
**  __LONG_BIT__                                                          *
**  __LLONG_BIT__        (undefined for ISO-C90)                          *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _TYPEINFO_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 93,96,98,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,19.7,19.10,19.12,19.13,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,20.10,21.1,21.2
#endif

#define _TYPEINFO_H     1

/* Type name */

#define         __CHAR_TYPE__           char
#define         __SHORT_TYPE__          short int
#define         __INT_TYPE__            int
#define         __LONG_TYPE__           long int
#define         __LLONG_TYPE__          long long int

/* Integer constant suffix */

#if     __CHAR_BIT__ == __INT_BIT__
#  define       __CHAR_SUFF__(u)        u
#else
#  define       __CHAR_SUFF__(u)        
#endif
#if     __SHORT_BIT__ == __INT_BIT__
#  define       __SHORT_SUFF__(u)       u
#else
#  define       __SHORT_SUFF__(u)       
#endif
#define         __INT_SUFF__(u)         u
#define         __LONG_SUFF__(u)        u ## L
#define         __LLONG_SUFF__(u)       u ## LL

/* Helper macros */

#define         __TYPE2(name)           name ## _TYPE__
#define         __SUFFIX(name,u)        __SUFFIX2(name,u)
#define         __SUFFIX2(name,u)       name ## _SUFF__(u)
#define         __ICON2(con,suff)       __ICON3(con,suff)
#define         __ICON3(con,suff)       con ## suff
#define         __BITMASK(bits,suff)    ((((1##suff<<((bits)-1))-1)<<1)+1)
#define         __SMAX2(name,suff)      __BITMASK(name ## _BIT__-1,suff)
#define         __UMAX2(name,suff)      __BITMASK(name ## _BIT__,suff)

/* Macros available to other header files */

#define         __TYPE(name)            __TYPE2(name)
#define         __ICON(name,con,u)      __ICON2(con,__SUFFIX(name,u))
#define         __SMIN(name)            (-__SMAX(name)-1)
#define         __SMAX(name)            __SMAX2(name,__SUFFIX(name,))
#define         __UMIN(name)            __ICON(name,0,U)
#define         __UMAX(name)            __UMAX2(name,__SUFFIX(name,U))

#if   __MISRAC_VERSION__ >= 1998 && __MISRAC_VERSION__ <= 2012
#pragma nomisrac restore
#endif

#endif  /* _TYPEINFO_H */
