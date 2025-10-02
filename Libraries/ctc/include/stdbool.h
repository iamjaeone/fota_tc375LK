/**************************************************************************
**                                                                        *
**  FILE        :  stdbool.h                                              *
**                                                                        *
**  DESCRIPTION :  Boolean type and values                                *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _STDBOOL_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 19.4,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,21.1,21.2
#endif

#define _STDBOOL_H      1

/*
 * 7.16 Boolean type and values
 */
#define bool                            _Bool
#ifdef  __cplusplus
#define true                            1
#define false                           0
#else
#define true                            _Pragma("boolean") 1 _Pragma("boolean restore")
#define false                           _Pragma("boolean") 0 _Pragma("boolean restore")
#endif
#define __bool_true_false_are_defined   1

#pragma nomisrac restore

#endif  /* _STDBOOL_H */
