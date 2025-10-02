/**************************************************************************
**                                                                        *
**  FILE        :  stddef.h                                               *
**                                                                        *
**  DESCRIPTION :  Include file with macros for common use                *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _STDDEF_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 93,96,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.7,19.10,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.3,2.5,21.1,21.2
#endif

#define _STDDEF_H       1

#ifndef _SIZE_T_DEF
typedef __size_t        size_t;
#define _SIZE_T_DEF     1
#endif

#ifndef _WCHAR_T
typedef __wchar_t       wchar_t;
#define _WCHAR_T        1
#endif

typedef __ptrdiff_t     ptrdiff_t;

#ifdef  __cplusplus
#define NULL    (0)
#else   /* defined(__cplusplus) */
#define NULL    ((void *) 0)
#endif  /* defined(__cplusplus) */

#ifdef  __cplusplus
#define offsetof( _type, _member )      ((size_t)(__INTADDR__((( char *)&((( _type *)0)->_member)) - ( char *)0)))
#else   /* defined(__cplusplus) */
#define offsetof( _type, _member )      ((size_t)((( char *)&(( _type *)0)->_member) - ( char *)0))
#endif  /* defined(__cplusplus) */

#pragma nomisrac restore

#endif  /* _STDDEF_H */
