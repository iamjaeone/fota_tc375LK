/**************************************************************************
**                                                                        *
**  FILE        :  setjmp.h                                               *
**                                                                        *
**  DESCRIPTION :  Include file with prototypes for non-local jump        *
**                 functions                                              *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _SETJMP_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 1,13,114,115
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 1.1,6.3,20.1,20.2
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 1.2,2.5,8.2,21.1,21.2
#endif

#define _SETJMP_H       1

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

#define BUF_SIZ 16

typedef int * jmp_buf[BUF_SIZ];

extern  int setjmp      ( jmp_buf );
extern  void longjmp    ( jmp_buf, int ) __noreturn__;

#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#endif  /* _SETJMP_H */
