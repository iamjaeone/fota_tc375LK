/**************************************************************************
**                                                                        *
**  FILE        :  stdnoreturn.h                                          *
**                                                                        *
**  DESCRIPTION :  _Noreturn function specifier.                          *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _STDNORETURN_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,21.1,21.2
#endif

#define _STDNORETURN_H  1

#define noreturn        _Noreturn

#if   __MISRAC_VERSION__ > 0
#pragma nomisrac restore
#endif

#endif  /* _STDNORETURN_H */
