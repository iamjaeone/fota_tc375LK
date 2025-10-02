/**************************************************************************
**                                                                        *
**  FILE        :  stdarg.h                                               *
**                                                                        *
**  DESCRIPTION :  Include file with prototypes and macros for usage with *
**                 variable argument lists                                *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _STDARG_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 14,93,96,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,19.7,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,21.1,21.2
#endif

#define _STDARG_H       1

typedef char * va_list;

#define va_start(ap,last) ((ap) = __dotdotdot__())
#define __va_size(type)   ((sizeof(type) + 3) & ~ 3u)
#define va_arg(ap,type)   (*(type *)(((ap) += __va_size(type)) - __va_size(type)))
#define va_end(ap)        ((void) 0)  
#define va_copy(dest,src) ((dest) = (src))

#pragma nomisrac restore

#endif  /* _STDARG_H */
