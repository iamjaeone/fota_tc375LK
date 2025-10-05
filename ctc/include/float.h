/**************************************************************************
**                                                                        *
**  FILE        :  float.h                                                *
**                                                                        *
**  DESCRIPTION :  Floating point characteristics                         *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _FLOAT_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 13,93,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 6.3,19.7,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,21.1,21.2
#endif

#define _FLOAT_H        1

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

/*
 * IEEE-754-1985 Format values:
 */
#define FLT_RADIX       (2)
#ifndef FLT_EVAL_METHOD
#define FLT_EVAL_METHOD (0)
#endif

extern  int     _fegetfltrounds(void);  
#define FLT_ROUNDS      (_fegetfltrounds())     /* convert from fegetround() */
        
#define FLT_MANT_DIG    (24)
#define FLT_DIG         (6)
#define FLT_EPSILON     (1.19209290E-07F)
#define FLT_MIN         (1.17549435E-38F)
#define FLT_MIN_EXP     (-125)
#define FLT_MIN_10_EXP  (-37)
#define FLT_MAX         (3.40282347E+38F)
#define FLT_MAX_EXP     (+128)
#define FLT_MAX_10_EXP  (+38)
#if     __STDC_VERSION__ >= 201112L
#define FLT_HAS_SUBNORM (0)
#define FLT_TRUE_MIN    (FLT_MIN)
#define FLT_DECIMAL_DIG (9)
#endif
        
#ifndef __SINGLE_FP__
#define DBL_MANT_DIG    (53)
#define DBL_DIG         (15)
#define DBL_EPSILON     (2.2204460492503131E-16)
#define DBL_MIN         (2.2250738585072014E-308)
#define DBL_MIN_EXP     (-1021)
#define DBL_MIN_10_EXP  (-307)
#define DBL_MAX         (1.7976931348623157E+308)
#define DBL_MAX_EXP     (+1024)
#define DBL_MAX_10_EXP  (+308)
#if     __STDC_VERSION__ >= 201112L
#define DBL_HAS_SUBNORM (0)
#define DBL_TRUE_MIN    (DBL_MIN)
#define DBL_DECIMAL_DIG (17)
#endif
#else
#define DBL_MANT_DIG    (FLT_MANT_DIG)
#define DBL_DIG         (FLT_DIG)
#define DBL_MIN_10_EXP  (FLT_MIN_10_EXP)
#define DBL_MAX_10_EXP  (FLT_MAX_10_EXP)
#define DBL_EPSILON     (FLT_EPSILON)
#define DBL_MIN         (FLT_MIN)
#define DBL_MAX         (FLT_MAX)
#define DBL_MIN_EXP     (FLT_MIN_EXP)
#define DBL_MAX_EXP     (FLT_MAX_EXP)
#if     __STDC_VERSION__ >= 201112L
#define DBL_HAS_SUBNORM (FLT_HAS_SUBNORM)
#define DBL_TRUE_MIN    (FLT_TRUE_MIN)
#define DBL_DECIMAL_DIG (FLT_DECIMAL_DIG)
#endif
#endif

#define LDBL_DIG        (DBL_DIG)
#define LDBL_MANT_DIG   (DBL_MANT_DIG)
#define LDBL_MIN_10_EXP (DBL_MIN_10_EXP)
#define LDBL_MAX_10_EXP (DBL_MAX_10_EXP)
#define LDBL_EPSILON    (DBL_EPSILON)
#define LDBL_MIN        (DBL_MIN)
#define LDBL_MAX        (DBL_MAX)
#define LDBL_MIN_EXP    (DBL_MIN_EXP)
#define LDBL_MAX_EXP    (DBL_MAX_EXP)
#if     __STDC_VERSION__ >= 201112L
#define LDBL_HAS_SUBNORM        (DBL_HAS_SUBNORM)
#define LDBL_TRUE_MIN   (DBL_TRUE_MIN)
#define LDBL_DECIMAL_DIG (DBL_DECIMAL_DIG)
#define DECIMAL_DIG     (LDBL_DECIMAL_DIG)
#else
#ifdef __SINGLE_FP__
#define DECIMAL_DIG     (9)
#else
#define DECIMAL_DIG     (17)
#endif
#endif

/*
 * IEEE-754-2008 binary16 interchange values:
 */
#ifdef __STDC_WANT_IEC_60559_TYPES_EXT__
#define FLT16_MANT_DIG          (11)
#define FLT16_DIG               (3)
#define FLT16_EPSILON           (9.765625e-4F)
#define FLT16_MIN               (6.103515625E-05F)
#define FLT16_MIN_EXP           (-13)
#define FLT16_MIN_10_EXP        (-4)
#define FLT16_MAX               (65504.0F)
#define FLT16_MAX_EXP           (+16)
#define FLT16_MAX_10_EXP        (+4)
#define FLT16_HAS_SUBNORM       (1)
#define FLT16_TRUE_MIN          (_FLT16_MIN)
#define FLT16_DECIMAL_DIG       (5)
#endif
        
/*
 * A subset of the IEEE-754-1985 Recommended Functions for both float and double
 */
#if     __STDC_VERSION__ < 199901L
float   copysignf( float, float );
int     _isinff( float );
int     _isfinitef( float );
int     _isnanf( float );
float   scalbf( float, int );
double  copysign( double, double );
int     _isinf( double );
int     _isfinite( double );
int     _isnan( double );
double  scalb( double, int );
#endif  /* __STDC_VERSION__ < 199901L */

#ifdef  __DSPC__

/*
 * DSP-C macros
 */

#define __W527(x)               (_Pragma("warning 527")(x)_Pragma("warning restore"))

/*
 * Number of integer bits in accum types.
 */
#define SSACCUM_INT_DIG         (__SSACCUM_INT_BIT__)
#define SACCUM_INT_DIG          (__SACCUM_INT_BIT__)
#define ACCUM_INT_DIG           (__ACCUM_INT_BIT__)
#define LACCUM_INT_DIG          (__LACCUM_INT_BIT__)
#define LLACCUM_INT_DIG         (__LLACCUM_INT_BIT__)

/*
 * Number of fractional bits in fract types.
 */
#define SSFRACT_FRACT_DIG       (__SSFRACT_BIT__ - 1)
#define SFRACT_FRACT_DIG        (__SFRACT_BIT__  - 1)
#define FRACT_FRACT_DIG         (__FRACT_BIT__   - 1)
#define LFRACT_FRACT_DIG        (__LFRACT_BIT__  - 1)
#define LLFRACT_FRACT_DIG       (__LLFRACT_BIT__ - 1)

/*
 * Number of fractional bits in accum types.
 */
#define SSACCUM_FRACT_DIG       (__SSACCUM_BIT__ - __SSACCUM_INT_BIT__ - 1)
#define SACCUM_FRACT_DIG        (__SACCUM_BIT__  - __SACCUM_INT_BIT__  - 1)
#define ACCUM_FRACT_DIG         (__ACCUM_BIT__   - __ACCUM_INT_BIT__   - 1)
#define LACCUM_FRACT_DIG        (__LACCUM_BIT__  - __LACCUM_INT_BIT__  - 1)
#define LLACCUM_FRACT_DIG       (__LLACCUM_BIT__ - __LLACCUM_INT_BIT__ - 1)

/*
 * Maximum value of fract types.
 */
#define SSFRACT_MAX             (__W527((__ssfract) 1.0))
#define SFRACT_MAX              (__W527((__sfract)  1.0))
#define FRACT_MAX               (__W527((__fract)   1.0))
#define LFRACT_MAX              (__W527((__lfract)  1.0))
#define LLFRACT_MAX             (__W527((__llfract) 1.0))

/*
 * Maximum value of accum types.
 */
#define SSACCUM_MAX             ((__ssaccum) (1ULL << SSACCUM_INT_DIG))
#define SACCUM_MAX              ((__saccum)  (1ULL << SACCUM_INT_DIG ))
#define ACCUM_MAX               ((__accum)   (1ULL << ACCUM_INT_DIG  ))
#define LACCUM_MAX              ((__laccum)  (1ULL << LACCUM_INT_DIG ))
#define LLACCUM_MAX             ((__llaccum) (1ULL << LLACCUM_INT_DIG))

/*
 * Minimum value of fract types.
 */
#define SSFRACT_MIN             ((__ssfract) -1.0)
#define SFRACT_MIN              ((__sfract)  -1.0)
#define FRACT_MIN               ((__fract)   -1.0)
#define LFRACT_MIN              ((__lfract)  -1.0)
#define LLFRACT_MIN             ((__llfract) -1.0)

/*
 * Minimum value of accum types.
 */
#define SSACCUM_MIN             ((__ssaccum) -(long long)(1ULL << SSACCUM_INT_DIG))
#define SACCUM_MIN              ((__saccum)  -(long long)(1ULL << SACCUM_INT_DIG ))
#define ACCUM_MIN               ((__accum)   -(long long)(1ULL << ACCUM_INT_DIG  ))
#define LACCUM_MIN              ((__laccum)  -(long long)(1ULL << LACCUM_INT_DIG ))
#define LLACCUM_MIN             ((__llaccum) -(long long)(1ULL << LLACCUM_INT_DIG))

/*
 * Epsilon of fract types.
 */
#define SSFRACT_EPSILON         ((__ssfract) (1.0 / (double)(1ULL << (long long)SSFRACT_FRACT_DIG)))
#define SFRACT_EPSILON          ((__sfract)  (1.0 / (double)(1ULL << (long long)SFRACT_FRACT_DIG )))
#define FRACT_EPSILON           ((__fract)   (1.0 / (double)(1ULL << (long long)FRACT_FRACT_DIG  )))
#define LFRACT_EPSILON          ((__lfract)  (1.0 / (double)(1ULL << (long long)LFRACT_FRACT_DIG )))
#define LLFRACT_EPSILON         ((__llfract) (1.0 / (double)(1ULL << (long long)LLFRACT_FRACT_DIG)))

/*
 * Epsilon of accum types.
 */
#define SSACCUM_EPSILON         ((__ssaccum) (1.0 / (double)(1ULL << (long long)SACCUM_FRACT_DIG )))
#define SACCUM_EPSILON          ((__saccum)  (1.0 / (double)(1ULL << (long long)SACCUM_FRACT_DIG )))
#define ACCUM_EPSILON           ((__accum)   (1.0 / (double)(1ULL << (long long)ACCUM_FRACT_DIG  )))
#define LACCUM_EPSILON          ((__laccum)  (1.0 / (double)(1ULL << (long long)LACCUM_FRACT_DIG )))
#define LLACCUM_EPSILON         ((__llaccum) (1.0 / (double)(1ULL << (long long)LLACCUM_FRACT_DIG)))

#endif  /* __DSPC__ */

#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#if   __MISRAC_VERSION__ >= 1998 && __MISRAC_VERSION__ <= 2012
#pragma nomisrac restore
#endif

#endif  /* _FLOAT_H */
