/**************************************************************************
**                                                                        *
**  FILE        :  stdalign.h                                             *
**                                                                        *
**  DESCRIPTION :  Alignment.                                             *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _STDALIGN_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,21.1,21.2
#endif

#define _STDALIGN_H     1

#define alignas _Alignas
#define __alignas_is_defined    1

#define alignof _Alignof
#define __alignof_is_defined    1

#if   __MISRAC_VERSION__ > 0
#pragma nomisrac restore
#endif

#endif  /* _STDALIGN_H */
