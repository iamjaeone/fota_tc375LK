/**************************************************************************
**                                                                        *
**  FILE        :  assert.h                                               *
**                                                                        *
**  DESCRIPTION :  Include file with prototypes and macros to add         *
**                 diagnostics to programs                                *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 92,93,114,124
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,19.6,19.7,19.13,20.1,20.9
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,20.5,20.10,20.12,21.1,21.2
#endif

#undef assert

#ifndef _STDIO_H
#include <stdio.h>      /* prototype for 'printf' */
#endif  /* _STDIO_H */

#ifndef _STDLIB_H
#include <stdlib.h>     /* prototype for 'abort' */
#endif  /* _STDLIB_H */

#ifndef NDEBUG

/* The assert macro doesn't use the __func__ macro for the function name as
 * this will increase the amount of data generated.
 */
#define assert(_expr)                                                           \
        (_Pragma("nomisrac")                                                    \
         (void) (!(_expr) ?                                                     \
          printf("Assertion failed: (%s) file %s, line %d\n",                   \
                 #_expr, __FILE__, __LINE__), abort() : (void) 0                \
         )                                                                      \
         _Pragma("nomisrac restore")                                            \
        )

#else

#define assert(_expr)           ((void) 0)

#endif  /* NDEBUG */

#if   __MISRAC_VERSION__ >= 1998 && __MISRAC_VERSION__ <= 2012
#pragma nomisrac restore
#endif
