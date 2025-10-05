/**************************************************************************
**                                                                        *
**  FILE        :  libfloat.h                                             *
**                                                                        *
**  DESCRIPTION :  Floating point internals for libfloat.                 *
**                 These internals are also used by libc.                 *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _LIBFLOAT_H
#define _LIBFLOAT_H

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

#include <stdint.h>
#include "float_config.h"

/*
 * Define extra fastest minimum-width integer types, in addition to the
 * standard ones from <stdint.h>, for use in the floating point library.
 */

#ifndef         __FAST63__
#  define       __FAST63__      __FAST64__
#endif
#ifndef         __FAST57__
#  define       __FAST57__      __FAST63__
#endif
#ifndef         __FAST56__
#  define       __FAST56__      __FAST57__
#endif
#ifndef         __FAST54__
#  define       __FAST54__      __FAST56__
#endif
#ifndef         __FAST53__
#  define       __FAST53__      __FAST54__
#endif
#ifndef         __FAST52__
#  define       __FAST52__      __FAST53__
#endif
#ifndef         __FAST51__
#  define       __FAST51__      __FAST52__
#endif
#ifndef         __FAST31__
#  define       __FAST31__      __FAST32__
#endif
#ifndef         __FAST28__
#  define       __FAST28__      __FAST31__
#endif
#ifndef         __FAST27__
#  define       __FAST27__      __FAST28__
#endif
#ifndef         __FAST25__
#  define       __FAST25__      __FAST27__
#endif
#ifndef         __FAST23__
#  define       __FAST23__      __FAST24__
#endif
#ifndef         __FAST22__
#  define       __FAST22__      __FAST23__
#endif
#ifndef         __FAST15__
#  define       __FAST15__      __FAST16__
#endif
#ifndef         __FAST12__
#  define       __FAST12__      __FAST15__
#endif
#ifndef         __FAST11__
#  define       __FAST11__      __FAST12__
#endif
#ifndef         __FAST10__
#  define       __FAST10__      __FAST11__
#endif
#ifndef         __FAST9__
#  define       __FAST9__       __FAST10__
#endif
#ifndef         __FAST5__
#  define       __FAST5__       __FAST8__
#endif

#if     __LLONG_BIT__ > 32
typedef signed   __TYPE(__FAST63__)     int_fast63_t;
typedef unsigned __TYPE(__FAST63__)     uint_fast63_t;
typedef signed   __TYPE(__FAST57__)     int_fast57_t;
typedef unsigned __TYPE(__FAST57__)     uint_fast57_t;
typedef signed   __TYPE(__FAST56__)     int_fast56_t;
typedef unsigned __TYPE(__FAST56__)     uint_fast56_t;
typedef signed   __TYPE(__FAST54__)     int_fast54_t;
typedef unsigned __TYPE(__FAST54__)     uint_fast54_t;
typedef signed   __TYPE(__FAST53__)     int_fast53_t;
typedef unsigned __TYPE(__FAST53__)     uint_fast53_t;
typedef signed   __TYPE(__FAST52__)     int_fast52_t;
typedef unsigned __TYPE(__FAST52__)     uint_fast52_t;
typedef signed   __TYPE(__FAST51__)     int_fast51_t;
typedef unsigned __TYPE(__FAST51__)     uint_fast51_t;
typedef signed   __TYPE(__FAST31__)     int_fast31_t;
typedef unsigned __TYPE(__FAST31__)     uint_fast31_t;
#endif  /* __LLONG_BIT__ > 32 */
typedef signed   __TYPE(__FAST28__)     int_fast28_t;
typedef unsigned __TYPE(__FAST28__)     uint_fast28_t;
typedef signed   __TYPE(__FAST27__)     int_fast27_t;
typedef unsigned __TYPE(__FAST27__)     uint_fast27_t;
typedef signed   __TYPE(__FAST25__)     int_fast25_t;
typedef unsigned __TYPE(__FAST25__)     uint_fast25_t;
typedef signed   __TYPE(__FAST23__)     int_fast23_t;
typedef unsigned __TYPE(__FAST23__)     uint_fast23_t;
typedef signed   __TYPE(__FAST22__)     int_fast22_t;
typedef unsigned __TYPE(__FAST22__)     uint_fast22_t;
typedef signed   __TYPE(__FAST15__)     int_fast15_t;
typedef unsigned __TYPE(__FAST15__)     uint_fast15_t;
typedef signed   __TYPE(__FAST12__)     int_fast12_t;
typedef unsigned __TYPE(__FAST12__)     uint_fast12_t;
typedef signed   __TYPE(__FAST11__)     int_fast11_t;
typedef unsigned __TYPE(__FAST11__)     uint_fast11_t;
typedef signed   __TYPE(__FAST10__)     int_fast10_t;
typedef unsigned __TYPE(__FAST10__)     uint_fast10_t;
typedef signed   __TYPE(__FAST9__)      int_fast9_t;
typedef unsigned __TYPE(__FAST9__)      uint_fast9_t;
typedef signed   __TYPE(__FAST5__)      int_fast5_t;
typedef unsigned __TYPE(__FAST5__)      uint_fast5_t;

#define         INT63_C(value)          __ICON(__FAST63__,value,)
#define         UINT63_C(value)         __ICON(__FAST63__,value,U)
#define         INT57_C(value)          __ICON(__FAST57__,value,)
#define         UINT57_C(value)         __ICON(__FAST57__,value,U)
#define         INT56_C(value)          __ICON(__FAST56__,value,)
#define         UINT56_C(value)         __ICON(__FAST56__,value,U)
#define         INT54_C(value)          __ICON(__FAST54__,value,)
#define         UINT54_C(value)         __ICON(__FAST54__,value,U)
#define         INT53_C(value)          __ICON(__FAST53__,value,)
#define         UINT53_C(value)         __ICON(__FAST53__,value,U)
#define         INT52_C(value)          __ICON(__FAST52__,value,)
#define         UINT52_C(value)         __ICON(__FAST52__,value,U)
#define         INT51_C(value)          __ICON(__FAST51__,value,)
#define         UINT51_C(value)         __ICON(__FAST51__,value,U)
#define         INT31_C(value)          __ICON(__FAST31__,value,)
#define         UINT31_C(value)         __ICON(__FAST31__,value,U)
#define         INT28_C(value)          __ICON(__FAST28__,value,)
#define         UINT28_C(value)         __ICON(__FAST28__,value,U)
#define         INT27_C(value)          __ICON(__FAST27__,value,)
#define         UINT27_C(value)         __ICON(__FAST27__,value,U)
#define         INT25_C(value)          __ICON(__FAST25__,value,)
#define         UINT25_C(value)         __ICON(__FAST25__,value,U)
#define         INT23_C(value)          __ICON(__FAST23__,value,)
#define         UINT23_C(value)         __ICON(__FAST23__,value,U)
#define         INT22_C(value)          __ICON(__FAST22__,value,)
#define         UINT22_C(value)         __ICON(__FAST22__,value,U)
#define         INT15_C(value)          __ICON(__FAST15__,value,)
#define         UINT15_C(value)         __ICON(__FAST15__,value,U)
#define         INT12_C(value)          __ICON(__FAST12__,value,)
#define         UINT12_C(value)         __ICON(__FAST12__,value,U)
#define         INT11_C(value)          __ICON(__FAST11__,value,)
#define         UINT11_C(value)         __ICON(__FAST11__,value,U)
#define         INT10_C(value)          __ICON(__FAST10__,value,)
#define         UINT10_C(value)         __ICON(__FAST10__,value,U)
#define         INT9_C(value)           __ICON(__FAST9__,value,)
#define         UINT9_C(value)          __ICON(__FAST9__,value,U)
#define         INT5_C(value)           __ICON(__FAST5__,value,)
#define         UINT5_C(value)          __ICON(__FAST5__,value,U)


#define RS                      2                               /* Round and Sticky bits                */
#define GRS                     (RS+1)                          /* Guard, Round, Sticky bits            */

#define DBL_BIAS                1023                            /* (DBL_MAX_EXP - 1)    */
#define DBL_MAXEXP              1023                            /* should match DBL_MAX_EXP - 1         */
#define DBL_BIASED_INF_EXP      0x7ff                           /* The biased exponent of NAN and INFINITY      */
#define DBL_BIASED_MAX_EXP      0x7fe                           /* The maximal normal biased exponent   */
#define DBL_SIGNIFICAND_SIZE    52                              /* Without hidden bit.                  */
#define DBL_PRECISION           (DBL_SIGNIFICAND_SIZE+1)        /* precision including the hidden bit   */
#define DBL_EXP_SIZE            11                              /* exponent bits                        */

#define FLT_BIAS                127                             /* (FLT_MAX_EXP - 1) */
#define FLT_MAXEXP              127                             /* should match FLT_MAX_EXP - 1         */
#define FLT_BIASED_INF_EXP      0xff                            /* The biased exponent of NAN and INFINITY      */
#define FLT_BIASED_MAX_EXP      0xfe                            /* The maximal normal biased exponent   */
#define FLT_SIGNIFICAND_SIZE    23                              /* Without hidden bit.                  */
#define FLT_PRECISION           (FLT_SIGNIFICAND_SIZE+1)        /* precision including the hidden bit   */
#define FLT_EXP_SIZE            8                               /* exponent bits                        */

// Double constants
#define PI      0x1.921fb54442d18p+1    // PI           3.1415926535897932384626433832795028
#define PIO2    0x1.921fb54442d18p+0    // PI/2         1.5707963267948966192313216916397514
#define PIO4    0x1.921fb54442d18p-1    // PI/4         0.785398163397448309615660845819875699
#define PI3O4   0x1.2d97c7f3321d2p+1    // 3*PI/4       2.3561944901923449288469825374596271
#define SQRT2   0x1.6a09e667f3bcdp+0    // sqrt(2)      1.41421356237309504880168872420969798
#define SQRTO2  0x1.6a09e667f3bcdp-1    // 1/sqrt(2)    0.707106781186547524400844362104849088
#define LOG2    0x1.62e42fefa39efp-1    // log(2)       0.693147180559945309417232121458176575

// Float constants
#define PIF     0x1.921fb6p+1f          // PI           3.1415926535897932384626433832795028f
#define PIO2F   0x1.921fb6p+0f          // PI/2         1.5707963267948966192313216916397514f
#define PIO4F   0x1.921fb6p-1f          // PI/4         0.785398163397448309615660845819875699f
#define PI3O4F  0x1.2d97c8p+1f          // 3*PI/4       2.3561944901923449288469825374596271f
#define SQRT2F  0x1.6a09e6p+0f          // sqrtf(2)     1.41421356237309504880168872420969798f
#define SQRTO2F 0x1.6a09e6p-1f          // 1/sqrtf(2)   0.707106781186547524400844362104849088f
#define LOG2F   0x1.62e43p-1f           // log(2)       0.693147180559945309417232121458176575f

// Long double constant
#define LOG2L   0x1.62e42fefa39efp-1    // log(2)       0.693147180559945309417232121458176575

#define FLT16_BIAS              15                              /* (FLT16_MAX_EXP - 1)  */
#define FLT16_BIASED_INF_EXP    0x1f                            /* The biased exponent of NAN and INFINITY      */
#define FLT16_SIGNIFICAND_SIZE  10                              /* Without hidden bit.  */

/* Test for round modes, and try to be smart in case a round mode does not exist. */
#ifdef TASKING_FE_DOWNWARD
#define IS_DOWNWARD_ROUNDMODE(x)        ((x) == TASKING_FE_DOWNWARD)
#else
#define IS_DOWNWARD_ROUNDMODE(x)        (false)
#endif

#ifdef TASKING_FE_UPWARD
#define IS_UPWARD_ROUNDMODE(x)  ((x) == TASKING_FE_UPWARD)
#else
#define IS_UPWARD_ROUNDMODE(x)  (false)
#endif

#ifdef TASKING_FE_TOWARDZERO
#define IS_TOWARDZERO_ROUNDMODE(x)      ((x) == TASKING_FE_TOWARDZERO)
#else
#define IS_TOWARDZERO_ROUNDMODE(x)      (false)
#endif

/* FE_TONEAREST should always exist */
#define IS_TONEAREST_ROUNDMODE(x)       ((x) == TASKING_FE_TONEAREST)

/* create floats */

#if     __LLONG_BIT__ > 32
/* double functions */

/** Create double from integer parts. */
static inline double __pack(_Bool sign, uint_fast11_t biased_exponent, uint_fast53_t mantissa)
{
        union {double f; uint_fast64_t i;} u;

        u.i = ((uint_fast64_t)sign << 63) | (((uint_fast64_t)biased_exponent & 0x7ff) << 52) | (mantissa & ((1ULL<<52)-1));
        return u.f;
}

/** Create float non-signaling nan without a payload. */
static inline double __nan(void)
{
        union {double f; uint_fast64_t i;} u;
        u.i = 0xfffULL << 51;
        return u.f;
}

/** Create float infinity. */
static inline double __inf(void)
{
        union {double f; uint_fast64_t i;} u;
        u.i = 0x7ffULL << 52;
        return u.f;
}

/** Get double biased exponent. */
static inline int  __exp( double x)
{
        union {double f; uint_fast64_t i;} u = {x};
        return (u.i >> 52) & 0x7ff;
}

/** Get double mantissa without hidden bit. */
static inline uint_fast64_t __mant(double x)
{
        union {double f; uint_fast64_t i;} u = {x};
        return u.i & ((1ULL << 52) - 1);
}

/**
 * Get exp + mantissa (mask out sign).
 * Both exp and mantissa can be shifted right,
 * the only guarantee is that exp is on the left (ms) of mant.
 */
static inline uint_fast64_t __expmant(double x)
{
        union {double f; uint_fast64_t i;} u = {x};
        return u.i & ((1ULL << 63) - 1);
}

/** Get double sign. */
static inline _Bool __signbit(double x)
{
        union {double f; uint_fast64_t i;} u = {x};
        return u.i >> 63;
}

/** Set double sign. */
static inline double __set_sign(double x, _Bool sign)
{
        union {double f; uint_fast64_t i;} u = {x};
        const uint_fast64_t mask = (1ULL << 63) - 1;
        u.i &= mask;
        u.i |= (uint_fast64_t)sign << 63;
        return u.f;
}

/** Set double exp. */
static inline double __set_exp(double x, unsigned int exp)
{
        union {double f; uint_fast64_t i;} u = {x};
        const uint_fast64_t mask = 0x800fffffffffffffULL;
        u.i &= mask;
        u.i |= (uint_fast64_t)exp << 52;
        return u.f;
}

/** Copy sign of double. */
static inline double __copysign(double x, double y)
{
        return __set_sign( x, __signbit(y));
}

#endif  /* __LLONG_BIT__ > 32 */

/* float functions */
/** Create float from integer parts. */
static inline float __packf(_Bool sign, uint_fast8_t biased_exponent, uint_fast23_t mantissa)
{
        union {float f; uint_fast32_t i;} u;

#if defined( __CMCS__ ) || defined( __C51__ )
        u.i = ((uint_fast32_t)sign << 23) | (((uint_fast32_t)biased_exponent & 0xff) << 24) | (mantissa & ((1ULL<<23)-1));
#else
        u.i = ((uint_fast32_t)sign << 31) | (((uint_fast32_t)biased_exponent & 0xff) << 23) | (mantissa & ((1ULL<<23)-1));
#endif
        return u.f;
}

/** Create float non-signaling nan without a payload. */
static inline float __nanf(void)
{
        union {float f; uint_fast32_t i;} u;

        /* Although the meaning of the bits differ, the code
         * for MCS and 8051 is the same as for normal targets.
         */
        u.i = 0x1ffUL << 22;
        return u.f;
}

/** Create float infinity. */
static inline float __inff(void)
{
        union {float f; uint_fast32_t i;} u;

#if defined( __CMCS__ ) || defined( __C51__ )
        u.i = 0xffUL << 24;
#else
        u.i = 0xffUL << 23;
#endif
        return u.f;
}

/** Get float biased exponent */
static inline int  __expf (float x)
{
        union {float f; uint_fast32_t i;} u = {x};
#if defined( __CMCS__ ) || defined( __C51__ )
        return (u.i >> 24) & 0xff;
#else
        return (u.i >> 23) & 0xff;
#endif
}

/** Get float mantissa without hidden bit */
static inline uint_fast32_t __mantf(float x)
{
        union {float f; uint_fast32_t i;} u = {x};
        return u.i & ((((uint_fast32_t) 1) << 23) - 1);
}

/**
 * Get float exp + mantissa (mask out sign).
 * Both exp and mantissa can be shifted right,
 * the only guarantee is that exp is on the left (ms) of mant.
 */
static inline uint_fast32_t __expmantf(float x)
{
        union {float f; uint_fast32_t i;} u = {x};

#if defined( __CMCS__ ) || defined( __C51__ )
        const uint_fast32_t mask = ~(((uint_fast32_t) 1)  << 23);
        return u.i & mask;
#else
        return u.i & ((1ULL << 31) - 1);
#endif
}

/** Get float sign */
static inline _Bool __signbitf(float x)
{
        union {float f; uint_fast32_t i;} u = {x};

#if defined( __CMCS__ ) || defined( __C51__ )
        return (u.i >> 23) & 1;
#else
        return u.i >> 31;
#endif
}

/** Set float sign */
static inline float __set_signf(float x, _Bool sign)
{
        union {float f; uint_fast32_t i;} u = {x};

#if defined( __CMCS__ ) || defined( __C51__ )
        const uint_fast32_t mask = ~(((uint_fast32_t) 1)  << 23);
        u.i &= mask;
        u.i |= (uint_fast32_t)sign << 23;
#else
        const uint_fast32_t mask = (((uint_fast32_t) 1) << 31) - 1;
        u.i &= mask;
        u.i |= (uint_fast32_t)sign << 31;
#endif
        return u.f;
}

/** Set float exp  */
static inline float __set_expf(float x, unsigned int exp)
{
        union {float f; uint_fast32_t i;} u = {x};
#if defined( __CMCS__ ) || defined( __C51__ )
        const uint_fast32_t mask = 0x00ffffffUL;
        u.i &= mask;
        u.i |= (uint_fast32_t)exp << 24;
#else
        const uint_fast32_t mask = 0x807fffffUL;
        u.i &= mask;
        u.i |= (uint_fast32_t)exp << 23;
#endif
        return u.f;
}

/** Copy sign */
static inline float __copysignf(float x, float y)
{
        return __set_signf(x, __signbitf(y));
}

static inline uint_fast16_t __expmanth( _Float16 x )
{
        union {_Float16 h; uint_fast16_t i;} u = {x};
        return u.i & ((1UL << 15) - 1);
}

static inline _Bool __signbith( _Float16 x )
{
        union {_Float16 h; uint_fast16_t i;} u = {x};
        return u.i >> 15;
}

static inline int  __exph( _Float16 x )
{
        union {_Float16 h; uint_fast16_t i;} u = {x};
        return (u.i >> 10) & 0x1f;
}

static inline uint_fast16_t __manth( _Float16 x )
{
        union {_Float16 f; uint_fast16_t i;} u = {x};
        return u.i & ((1UL << 10) - 1);
}

static inline _Float16 __packh(_Bool sign, uint_fast5_t biased_exponent, uint_fast10_t mantissa)
{
        union {_Float16 h; uint_fast16_t i;} u;

        u.i = ((uint_fast16_t)sign << 15) | (((uint_fast16_t)biased_exponent & 0x1f) << 10) | (mantissa & ((1ULL<<10)-1));
        return u.h;
}

/* is_zero is not builtin */

static inline _Bool __is_zerof(float x)
{
#if FP_SUPPORT_SUBNORMALS && FP_EVALUATE_SUBNORMALS
        return __expmantf( x ) == 0;
#else
        return __expf( x ) == 0;
#endif
}

#if     __LLONG_BIT__ > 32
static inline _Bool __is_zero(double x)
{
#if FP_SUPPORT_SUBNORMALS && FP_EVALUATE_SUBNORMALS
        return __expmant( x ) == 0;
#else
        return __exp( x ) == 0;
#endif
}
#endif  /* __LLONG_BIT__ > 32 */

static inline _Bool __is_zeroh(_Float16 x)
{
#if FP_SUPPORT_SUBNORMALS && FP_EVALUATE_SUBNORMALS
        return __expmanth( x ) == 0;
#else
        return __exph( x ) == 0;
#endif
}


#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#endif
