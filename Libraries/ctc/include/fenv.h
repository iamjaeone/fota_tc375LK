/**************************************************************************
**                                                                        *
**  FILE        :  fenv.h                                                 *
**                                                                        *
**  DESCRIPTION :  Floating-point environment                             *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _FENV_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 1,13,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 1.1,6.3,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 1.2,2.5,21.1,21.2
#endif

#define _FENV_H 1

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

typedef short   fexcept_t;
        

typedef struct
{
        unsigned int    _fenv;
} fenv_t;


#define FE_DIVBYZERO    (1 << 4)
#define FE_INEXACT      (1 << 2)
#define FE_INVALID      (1 << 6)
#define FE_OVERFLOW     (1 << 5)
#define FE_UNDERFLOW    (1 << 3)

#define FE_ALL_EXCEPT   (FE_DIVBYZERO|FE_INEXACT|FE_INVALID|FE_OVERFLOW|FE_UNDERFLOW)

#define FE_TONEAREST    (0)
#define FE_UPWARD       (1)
#define FE_DOWNWARD     (2)
#define FE_TOWARDZERO   (3)

#define FE_DFL_ENV      ((const fenv_t*)-1)

extern  int     feclearexcept(int excepts);
extern  int     fegetexceptflag(fexcept_t *flagp, int excepts);
extern  int     feraiseexcept(int excepts);
extern  int     fesetexceptflag(const fexcept_t *flagp, int excepts);
extern  int     fetestexcept(int excepts);
extern  int     fegetround(void);
extern  int     fesetround(int round);
extern  int     fegetenv(fenv_t *envp);
extern  int     feholdexcept(fenv_t *envp);
extern  int     fesetenv(const fenv_t *envp);
extern  int     feupdateenv(const fenv_t *envp);

extern  int     _fesetround(int round);
        
        
#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#pragma nomisrac restore

#endif  /* _FENV_H */
