/**************************************************************************
**                                                                        *
**  FILE        :  iso646.h                                               *
**                                                                        *
**  DESCRIPTION :  Alternative spellings.                                 *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _ISO646_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,21.1,21.2
#endif

#define _ISO646_H       1

#define and     &&
#define and_eq  &=
#define bitand  &
#define bitor   |
#define compl   ~
#define not     !
#define not_eq  !=
#define or      ||
#define or_eq   |=
#define xor     ^
#define xor_eq  ^=

#pragma nomisrac restore

#endif /* _ISO646_H */
