/**************************************************************************
**                                                                        *
**  FILE        :  signal.h                                               *
**                                                                        *
**  DESCRIPTION :  Include file with prototypes and macros for handling   *
**                 various signals.                                       *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _SIGNAL_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 13,114,115
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 6.3,20.1,20.2
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.3,2.5,8.2,21.1,21.2
#endif

#define _SIGNAL_H       1

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

typedef int     sig_atomic_t;
typedef void    signalfunction( int );

#define _NSIG           7 /* one more then last code */

#define SIGINT          1
#define SIGILL          2
#define SIGFPE          3
#define SIGSEGV         4
#define SIGTERM         5
#define SIGABRT         6
#define SIG_DFL         ((void (*)(int))0)
#define SIG_IGN         ((void (*)(int))1)
#define SIG_ERR         ((void (*)(int))-1)

extern  signalfunction *signal(int, signalfunction *);
extern  int raise( int );

#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#pragma nomisrac restore

#endif  /* _SIGNAL_H */
