/**************************************************************************
**                                                                        *
**  FILE        :  math.h                                                 *
**                                                                        *
**  DESCRIPTION :  Include file with prototypes for mathematical          *
**                 functions                                              *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _MATH_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 1,13,14,93,96,114,115
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 1.1,5.7,6.3,19.4,19.7,20.1,20.2
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 1.2,2.3,2.5,8.2,21.1,21.2
#endif

#define _MATH_H 1

#include "typeinfo.h"

#ifdef __cplusplus
#define NAN       (0.0f/0.0f)
#define INFINITY  (1.0f/0.0f)
#else
/* Don't complain about overflow. */
#define INFINITY (_Pragma("STDC FENV_ACCESS OFF") _Pragma("warning 527") (1e34f*1e34f) _Pragma("warning restore") _Pragma("STDC FENV_ACCESS restore"))
/* Don't complain about division by zero. */
#define NAN (_Pragma("STDC FENV_ACCESS OFF") _Pragma("warning 585") (0.0f/0.0f) _Pragma("warning restore") _Pragma("STDC FENV_ACCESS restore"))
#endif

#define HUGE_VALF       (INFINITY)
#define HUGE_VAL        ((double)(INFINITY))
#define HUGE_VALL       (HUGE_VAL)

#ifndef FLT_EVAL_METHOD
#   define      FLT_EVAL_METHOD (0)
#endif

#if     FLT_EVAL_METHOD == 0
typedef float           float_t;
typedef double          double_t;
#elif   FLT_EVAL_METHOD == 1
typedef double          float_t;
typedef double          double_t;
#elif   FLT_EVAL_METHOD == 2
typedef long double     float_t;
typedef long double     double_t;
#else
#   error unsupported floating point evaluation method
#endif

#if     __STDC_VERSION__ >= 199901L

#define FP_INFINITE     (0)
#define FP_NAN          (1)
#define FP_NORMAL       (2)
#define FP_SUBNORMAL    (3)
#define FP_ZERO         (4)


#ifdef __FPU__
# define FP_FAST_FMAF           (1)
# ifdef __SINGLE_FP__
#  define FP_FAST_FMA           (1)
# endif
#endif



#define FP_ILOGBNAN     __SMAX(__INT)   /* INT_MAX */
#define FP_ILOGB0       __SMIN(__INT)   /* INT_MIN */

#define MATH_ERRNO      (1)
#define MATH_ERREXCEPT  (2)
#define math_errhandling (MATH_ERRNO)

#endif  /* __STDC_VERSION__ >= 199901L */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern  double          sin             ( double );
extern  float           sinf            ( float );
extern  long double     sinl            ( long double );
extern  double          cos             ( double );
extern  float           cosf            ( float );
extern  long double     cosl            ( long double );
extern  double          tan             ( double );
extern  float           tanf            ( float );
extern  long double     tanl            ( long double );
extern  double          asin            ( double );
extern  float           asinf           ( float );
extern  long double     asinl           ( long double );
extern  double          acos            ( double );
extern  float           acosf           ( float );
extern  long double     acosl           ( long double );
extern  double          atan            ( double );
extern  float           atanf           ( float );
extern  long double     atanl           ( long double );
extern  double          atan2           ( double, double );
extern  float           atan2f          ( float, float );
extern  long double     atan2l          ( long double, long double );
extern  double          sinh            ( double );
extern  float           sinhf           ( float );
extern  long double     sinhl           ( long double );
extern  double          cosh            ( double );
extern  float           coshf           ( float );
extern  long double     coshl           ( long double );
extern  double          tanh            ( double );
extern  float           tanhf           ( float );
extern  long double     tanhl           ( long double );
extern  double          asinh           ( double );
extern  float           asinhf          ( float );
extern  long double     asinhl          ( long double );
extern  double          acosh           ( double );
extern  float           acoshf          ( float );
extern  long double     acoshl          ( long double );
extern  double          atanh           ( double );
extern  float           atanhf          ( float );
extern  long double     atanhl          ( long double );
extern  double          exp             ( double );
extern  float           expf            ( float );
extern  long double     expl            ( long double );
extern  double          exp2            ( double );
extern  float           exp2f           ( float );
extern  long double     exp2l           ( long double );
extern  double          expm1           ( double );
extern  float           expm1f          ( float );
extern  long double     expm1l          ( long double );
extern  double          frexp           ( double, int * );
extern  float           frexpf          ( float, int * );
extern  long double     frexpl          ( long double, int * );
extern  int             ilogb           ( double );
extern  int             ilogbf          ( float );
extern  int             ilogbl          ( long double );
extern  double          ldexp           ( double, int );
extern  float           ldexpf          ( float, int );
extern  long double     ldexpl          ( long double, int );
extern  double          log             ( double );
extern  float           logf            ( float );
extern  long double     logl            ( long double );
extern  double          log10           ( double );
extern  float           log10f          ( float );
extern  long double     log10l          ( long double );
extern  double          log1p           ( double );
extern  float           log1pf          ( float );
extern  long double     log1pl          ( long double );
extern  double          log2            ( double );
extern  float           log2f           ( float );
extern  long double     log2l           ( long double );
extern  double          logb            ( double );
extern  float           logbf           ( float );
extern  long double     logbl           ( long double );
extern  double          modf            ( double, double * );
extern  float           modff           ( float, float * );
extern  long double     modfl           ( long double, long double * );
#if     __STDC_VERSION__ >= 199901L
extern  double          scalbn          ( double, int );
extern  float           scalbnf         ( float, int );
extern  long double     scalbnl         ( long double, int );
extern  double          scalbln         ( double, long );
extern  float           scalblnf        ( float, long );
extern  long double     scalblnl        ( long double, long );
#endif  /* __STDC_VERSION__ >= 199901L */
extern  double          cbrt            ( double );
extern  float           cbrtf           ( float );
extern  long double     cbrtl           ( long double );
extern  double          fabs            ( double );
extern  float           fabsf           ( float );
extern  long double     fabsl           ( long double );
extern  double          hypot           ( double, double );
extern  float           hypotf          ( float, float );
extern  long double     hypotl          ( long double, long double );
extern  double          pow             ( double, double );
extern  float           powf            ( float, float );
extern  long double     powl            ( long double, long double );
extern  double          _powprecise     ( double, double );
extern  float           _powprecisef    ( float, float );
extern  long double     _powprecisel    ( long double, long double );
extern  double          sqrt            ( double );
extern  float           sqrtf           ( float );
extern  long double     sqrtl           ( long double );
extern  double          erf             ( double );
extern  float           erff            ( float );
extern  long double     erfl            ( long double );
extern  double          erfc            ( double );
extern  float           erfcf           ( float );
extern  long double     erfcl           ( long double );
extern  double          lgamma          ( double );
extern  float           lgammaf         ( float );
extern  long double     lgammal         ( long double );
extern  double          tgamma          ( double );
extern  float           tgammaf         ( float );
extern  long double     tgammal         ( long double );
extern  double          ceil            ( double );
extern  float           ceilf           ( float );
extern  long double     ceill           ( long double );
extern  double          floor           ( double );
extern  float           floorf          ( float );
extern  long double     floorl          ( long double );
extern  double          nearbyint       ( double );
extern  float           nearbyintf      ( float );
extern  long double     nearbyintl      ( long double );
extern  double          rint            ( double );
extern  float           rintf           ( float );
extern  long double     rintl           ( long double );
extern  long            lrint           ( double );
extern  long            lrintf          ( float );
extern  long            lrintl          ( long double );
#if     __STDC_VERSION__ >= 199901L
extern  long long       llrint          ( double );
extern  long long       llrintf         ( float );
extern  long long       llrintl         ( long double );
#endif  /* __STDC_VERSION__ >= 199901L */
extern  double          round           ( double );
extern  float           roundf          ( float );
extern  long double     roundl          ( long double );
extern  long            lround          ( double );
extern  long            lroundf         ( float );
extern  long            lroundl         ( long double );
#if     __STDC_VERSION__ >= 199901L
extern  long long       llround         ( double );
extern  long long       llroundf        ( float );
extern  long long       llroundl        ( long double );
#endif  /* __STDC_VERSION__ >= 199901L */
extern  double          trunc           ( double );
extern  float           truncf          ( float );
extern  long double     truncl          ( long double );
extern  double          fmod            ( double, double );
extern  float           fmodf           ( float, float );
extern  long double     fmodl           ( long double, long double );
extern  double          remainder       ( double, double );
extern  float           remainderf      ( float, float );
extern  long double     remainderl      ( long double, long double );
extern  double          remquo          ( double, double, int * );
extern  float           remquof         ( float, float, int * );
extern  long double     remquol         ( long double, long double, int * );
#if     __STDC_VERSION__ >= 199901L
extern  double          copysign        ( double, double );
extern  float           copysignf       ( float, float );
extern  long double     copysignl       ( long double, long double );
#endif  /* __STDC_VERSION__ >= 199901L */
extern  double          nan             ( const char * );
extern  float           nanf            ( const char * );
extern  long double     nanl            ( const char * );
extern  double          nextafter       ( double, double );
extern  float           nextafterf      ( float, float );
extern  long double     nextafterl      ( long double, long double );
extern  double          nexttoward      ( double, long double );
extern  float           nexttowardf     ( float, long double );
extern  long double     nexttowardl     ( long double, long double );
extern  double          fdim            ( double, double );
extern  float           fdimf           ( float, float );
extern  long double     fdiml           ( long double, long double );
extern  double          fmax            ( double, double );
extern  float           fmaxf           ( float, float );
extern  long double     fmaxl           ( long double, long double );
extern  double          fmin            ( double, double );
extern  float           fminf           ( float, float );
extern  long double     fminl           ( long double, long double );
extern  double          fma             ( double, double, double );
extern  float           fmaf            ( float, float, float );
extern  long double     fmal            ( long double, long double, long double );
#if     (__STDC_VERSION__ >= 199901L) || defined(__cplusplus)
extern  int             _fpclassify             ( double );
extern  int             _fpclassifyf            ( float );
extern  int             _fpclassifyl            ( long double );
extern  int             _isfinite               ( double );
extern  int             _isfinitef              ( float );
extern  int             _isfinitel              ( long double );
extern  int             _isinf                  ( double );
extern  int             _isinff                 ( float );
extern  int             _isinfl                 ( long double );
extern  int             _isnan                  ( double );
extern  int             _isnanf                 ( float );
extern  int             _isnanl                 ( long double );
extern  int             _isnormal               ( double );
extern  int             _isnormalf              ( float );
extern  int             _isnormall              ( long double );
extern  int             _signbit                ( double );
extern  int             _signbitf               ( float );
extern  int             _signbitl               ( long double );
extern  double          _edom_raise_invalid     ( void );
extern  float           _edom_raise_invalidf    ( void );
extern  long double     _edom_raise_invalidl    ( void );

#define fpclassify(x)           (__tgmath__fpclassify__1(x))
#define isfinite(x)             (__tgmath__isfinite__1(x))
#define isinf(x)                (__tgmath__isinf__1(x))
#define isnan(x)                (__tgmath__isnan__1(x))
#define isnormal(x)             (__tgmath__isnormal__1(x))
#define signbit(x)              (__tgmath__signbit__1(x))
extern  int             _isgreater              ( double, double );
extern  int             _isgreaterf             ( float, float );
extern  int             _isgreaterl             ( long double, long double );
extern  int             _isgreaterequal         ( double, double );
extern  int             _isgreaterequalf        ( float, float );
extern  int             _isgreaterequall        ( long double, long double );
extern  int             _isless                 ( double, double );
extern  int             _islessf                ( float, float );
extern  int             _islessl                ( long double, long double );
extern  int             _islessequal            ( double, double );
extern  int             _islessequalf           ( float, float );
extern  int             _islessequall           ( long double, long double );
extern  int             _islessgreater          ( double, double );
extern  int             _islessgreaterf         ( float, float );
extern  int             _islessgreaterl         ( long double, long double );
extern  int             _isunordered            ( double, double );
extern  int             _isunorderedf           ( float, float );
extern  int             _isunorderedl           ( long double, long double );
#define isgreater(x,y)          (__tgmath__isgreater__2(x,y))
#define isgreaterequal(x,y)     (__tgmath__isgreaterequal__2(x,y))
#define isless(x,y)             (__tgmath__isless__2(x,y))
#define islessequal(x,y)        (__tgmath__islessequal__2(x,y))
#define islessgreater(x,y)      (__tgmath__islessgreater__2(x,y))
#define isunordered(x,y)        (__tgmath__isunordered__2(x,y))
#endif  /* __STDC_VERSION__ >= 199901L */

/* do not wrap the fabs macro's result in parentheses, that messes up the STL when including cmath */
#define fabs(x)                 __fabs(x)
#define fabsf(x)                __fabsf(x)
#define fabsl(x)                __fabs(x)
#ifdef  __PRECISE_LIB_FP__
#define pow(x,y)                _powprecise(x,y)
#define powf(x,y)               _powprecisef(x,y)
#define powl(x,y)               _powprecisel(x,y)
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus
inline int  __tgmath__fpclassify__1( float x ) throw()                          { return _fpclassifyf( x ); }
inline int  __tgmath__fpclassify__1( double x ) throw()                         { return _fpclassify( x ); }
inline int  __tgmath__fpclassify__1( long double x ) throw()                    { return _fpclassifyl( x ); }
inline bool __tgmath__isfinite__1( float x ) throw()                            { return _isfinitef( x ); }
inline bool __tgmath__isfinite__1( double x ) throw()                           { return _isfinite( x ); }
inline bool __tgmath__isfinite__1( long double x ) throw()                      { return _isfinitel( x ); }
inline bool __tgmath__isinf__1( float x ) throw()                               { return _isinff( x ); }
inline bool __tgmath__isinf__1( double x ) throw()                              { return _isinf( x ); }
inline bool __tgmath__isinf__1( long double x ) throw()                         { return _isinfl( x ); }
inline bool __tgmath__isnan__1( float x ) throw()                               { return _isnanf( x ); }
inline bool __tgmath__isnan__1( double x ) throw()                              { return _isnan( x ); }
inline bool __tgmath__isnan__1( long double x ) throw()                         { return _isnanl( x ); }
inline bool __tgmath__isnormal__1( float x ) throw()                            { return _isnormalf( x ); }
inline bool __tgmath__isnormal__1( double x ) throw()                           { return _isnormal( x ); }
inline bool __tgmath__isnormal__1( long double x ) throw()                      { return _isnormall( x ); }
inline bool __tgmath__signbit__1( float x ) throw()                             { return _signbitf( x ); }
inline bool __tgmath__signbit__1( double x ) throw()                            { return _signbit( x ); }
inline bool __tgmath__signbit__1( long double x ) throw()                       { return _signbitl( x ); }

inline bool __tgmath__isgreater__2( float x, float y ) throw()                  { return _isgreaterf( x, y ); }
inline bool __tgmath__isgreater__2( double x, double y ) throw()                { return _isgreater( x, y ); }
inline bool __tgmath__isgreater__2( long double x, long double y ) throw()      { return _isgreaterl( x, y ); }
inline bool __tgmath__isgreaterequal__2( float x, float y ) throw()             { return _isgreaterequalf( x, y ); }
inline bool __tgmath__isgreaterequal__2( double x, double y ) throw()           { return _isgreaterequal( x, y ); }
inline bool __tgmath__isgreaterequal__2(long double x, long double y) throw()   { return _isgreaterequall( x, y ); }
inline bool __tgmath__isless__2( float x, float y ) throw()                     { return _islessf( x, y ); }
inline bool __tgmath__isless__2( double x, double y ) throw()                   { return _isless( x, y ); }
inline bool __tgmath__isless__2( long double x, long double y ) throw()         { return _islessl( x, y ); }
inline bool __tgmath__islessequal__2( float x, float y ) throw()                { return _islessequalf( x, y ); }
inline bool __tgmath__islessequal__2( double x, double y ) throw()              { return _islessequal( x, y ); }
inline bool __tgmath__islessequal__2( long double x, long double y ) throw()    { return _islessequall( x, y ); }
inline bool __tgmath__islessgreater__2( float x, float y ) throw()              { return _islessgreaterf( x, y ); }
inline bool __tgmath__islessgreater__2( double x, double y ) throw()            { return _islessgreater( x, y ); }
inline bool __tgmath__islessgreater__2( long double x, long double y ) throw()  { return _islessgreaterl( x, y ); }
inline bool __tgmath__isunordered__2( float x, float y ) throw()                { return _isunorderedf( x, y ); }
inline bool __tgmath__isunordered__2( double x, double y ) throw()              { return _isunordered( x, y ); }
inline bool __tgmath__isunordered__2( long double x, long double y ) throw()    { return _isunorderedl( x, y ); }
#endif /* __cplusplus */

#pragma nomisrac restore

#endif  /* _MATH_H */
