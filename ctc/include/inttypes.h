/**************************************************************************
**                                                                        *
**  FILE        :  inttypes.h                                             *
**                                                                        *
**  DESCRIPTION :  Format conversion of integer types                     *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _INTTYPES_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 13,14,93,96,98,114
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 5.7,6.3,19.4,19.7,19.12,19.13,20.1
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 2.5,8.14,20.10,21.1,21.2
#endif

#define _INTTYPES_H     1

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

#ifndef _WCHAR_T
typedef __wchar_t       wchar_t;
#define _WCHAR_T        1
#endif


typedef struct
{
        intmax_t        quot;
        intmax_t        rem;
}                       imaxdiv_t;


#if ! defined __cplusplus || defined __STDC_FORMAT_MACROS

/* Format modifiers */

#define         __CHAR_MOD__            hh
#define         __SHORT_MOD__           h
#define         __INT_MOD__     
#define         __LONG_MOD__            l
#define         __LLONG_MOD__           ll

/* Helper macros */

#define         __FORMAT(name,spec)     __FORMAT2(name,spec)
#define         __FORMAT2(type,spec)    __FORMAT3(type##_MOD__,spec)
#define         __FORMAT3(macro,spec)   __FORMAT4(macro,spec)
#define         __FORMAT4(mod,spec)     __FORMAT5(mod##spec)
#define         __FORMAT5(format)       #format

/*
 * 7.8.1 Macros for format specifiers
 */

#ifdef  __EXACT8__
#define         PRId8           __FORMAT(__EXACT8__,d)
#define         PRIi8           __FORMAT(__EXACT8__,i)
#define         PRIo8           __FORMAT(__EXACT8__,o)
#define         PRIu8           __FORMAT(__EXACT8__,u)
#define         PRIx8           __FORMAT(__EXACT8__,x)
#define         PRIX8           __FORMAT(__EXACT8__,X)
#define         SCNd8           __FORMAT(__EXACT8__,d)
#define         SCNi8           __FORMAT(__EXACT8__,i)
#define         SCNo8           __FORMAT(__EXACT8__,o)
#define         SCNu8           __FORMAT(__EXACT8__,u)
#define         SCNx8           __FORMAT(__EXACT8__,x)
#endif

#ifdef  __EXACT16__
#define         PRId16          __FORMAT(__EXACT16__,d)
#define         PRIi16          __FORMAT(__EXACT16__,i)
#define         PRIo16          __FORMAT(__EXACT16__,o)
#define         PRIu16          __FORMAT(__EXACT16__,u)
#define         PRIx16          __FORMAT(__EXACT16__,x)
#define         PRIX16          __FORMAT(__EXACT16__,X)
#define         SCNd16          __FORMAT(__EXACT16__,d)
#define         SCNi16          __FORMAT(__EXACT16__,i)
#define         SCNo16          __FORMAT(__EXACT16__,o)
#define         SCNu16          __FORMAT(__EXACT16__,u)
#define         SCNx16          __FORMAT(__EXACT16__,x)
#endif

#ifdef  __EXACT24__
#define         PRId24          __FORMAT(__EXACT24__,d)
#define         PRIi24          __FORMAT(__EXACT24__,i)
#define         PRIo24          __FORMAT(__EXACT24__,o)
#define         PRIu24          __FORMAT(__EXACT24__,u)
#define         PRIx24          __FORMAT(__EXACT24__,x)
#define         PRIX24          __FORMAT(__EXACT24__,X)
#define         SCNd24          __FORMAT(__EXACT24__,d)
#define         SCNi24          __FORMAT(__EXACT24__,i)
#define         SCNo24          __FORMAT(__EXACT24__,o)
#define         SCNu24          __FORMAT(__EXACT24__,u)
#define         SCNx24          __FORMAT(__EXACT24__,x)
#endif

#ifdef  __EXACT32__
#define         PRId32          __FORMAT(__EXACT32__,d)
#define         PRIi32          __FORMAT(__EXACT32__,i)
#define         PRIo32          __FORMAT(__EXACT32__,o)
#define         PRIu32          __FORMAT(__EXACT32__,u)
#define         PRIx32          __FORMAT(__EXACT32__,x)
#define         PRIX32          __FORMAT(__EXACT32__,X)
#define         SCNd32          __FORMAT(__EXACT32__,d)
#define         SCNi32          __FORMAT(__EXACT32__,i)
#define         SCNo32          __FORMAT(__EXACT32__,o)
#define         SCNu32          __FORMAT(__EXACT32__,u)
#define         SCNx32          __FORMAT(__EXACT32__,x)
#endif

#ifdef  __EXACT48__
#define         PRId48          __FORMAT(__EXACT48__,d)
#define         PRIi48          __FORMAT(__EXACT48__,i)
#define         PRIo48          __FORMAT(__EXACT48__,o)
#define         PRIu48          __FORMAT(__EXACT48__,u)
#define         PRIx48          __FORMAT(__EXACT48__,x)
#define         PRIX48          __FORMAT(__EXACT48__,X)
#define         SCNd48          __FORMAT(__EXACT48__,d)
#define         SCNi48          __FORMAT(__EXACT48__,i)
#define         SCNo48          __FORMAT(__EXACT48__,o)
#define         SCNu48          __FORMAT(__EXACT48__,u)
#define         SCNx48          __FORMAT(__EXACT48__,x)
#endif

#ifdef  __EXACT64__
#define         PRId64          __FORMAT(__EXACT64__,d)
#define         PRIi64          __FORMAT(__EXACT64__,i)
#define         PRIo64          __FORMAT(__EXACT64__,o)
#define         PRIu64          __FORMAT(__EXACT64__,u)
#define         PRIx64          __FORMAT(__EXACT64__,x)
#define         PRIX64          __FORMAT(__EXACT64__,X)
#define         SCNd64          __FORMAT(__EXACT64__,d)
#define         SCNi64          __FORMAT(__EXACT64__,i)
#define         SCNo64          __FORMAT(__EXACT64__,o)
#define         SCNu64          __FORMAT(__EXACT64__,u)
#define         SCNx64          __FORMAT(__EXACT64__,x)
#endif

#ifdef  __EXACT72__
#define         PRId72          __FORMAT(__EXACT72__,d)
#define         PRIi72          __FORMAT(__EXACT72__,i)
#define         PRIo72          __FORMAT(__EXACT72__,o)
#define         PRIu72          __FORMAT(__EXACT72__,u)
#define         PRIx72          __FORMAT(__EXACT72__,x)
#define         PRIX72          __FORMAT(__EXACT72__,X)
#define         SCNd72          __FORMAT(__EXACT72__,d)
#define         SCNi72          __FORMAT(__EXACT72__,i)
#define         SCNo72          __FORMAT(__EXACT72__,o)
#define         SCNu72          __FORMAT(__EXACT72__,u)
#define         SCNx72          __FORMAT(__EXACT72__,x)
#endif

#define         PRIdLEAST8      __FORMAT(__LEAST8__,d)
#define         PRIiLEAST8      __FORMAT(__LEAST8__,i)
#define         PRIoLEAST8      __FORMAT(__LEAST8__,o)
#define         PRIuLEAST8      __FORMAT(__LEAST8__,u)
#define         PRIxLEAST8      __FORMAT(__LEAST8__,x)
#define         PRIXLEAST8      __FORMAT(__LEAST8__,X)
#define         SCNdLEAST8      __FORMAT(__LEAST8__,d)
#define         SCNiLEAST8      __FORMAT(__LEAST8__,i)
#define         SCNoLEAST8      __FORMAT(__LEAST8__,o)
#define         SCNuLEAST8      __FORMAT(__LEAST8__,u)
#define         SCNxLEAST8      __FORMAT(__LEAST8__,x)

#define         PRIdLEAST16     __FORMAT(__LEAST16__,d)
#define         PRIiLEAST16     __FORMAT(__LEAST16__,i)
#define         PRIoLEAST16     __FORMAT(__LEAST16__,o)
#define         PRIuLEAST16     __FORMAT(__LEAST16__,u)
#define         PRIxLEAST16     __FORMAT(__LEAST16__,x)
#define         PRIXLEAST16     __FORMAT(__LEAST16__,X)
#define         SCNdLEAST16     __FORMAT(__LEAST16__,d)
#define         SCNiLEAST16     __FORMAT(__LEAST16__,i)
#define         SCNoLEAST16     __FORMAT(__LEAST16__,o)
#define         SCNuLEAST16     __FORMAT(__LEAST16__,u)
#define         SCNxLEAST16     __FORMAT(__LEAST16__,x)

#define         PRIdLEAST24     __FORMAT(__LEAST24__,d)
#define         PRIiLEAST24     __FORMAT(__LEAST24__,i)
#define         PRIoLEAST24     __FORMAT(__LEAST24__,o)
#define         PRIuLEAST24     __FORMAT(__LEAST24__,u)
#define         PRIxLEAST24     __FORMAT(__LEAST24__,x)
#define         PRIXLEAST24     __FORMAT(__LEAST24__,X)
#define         SCNdLEAST24     __FORMAT(__LEAST24__,d)
#define         SCNiLEAST24     __FORMAT(__LEAST24__,i)
#define         SCNoLEAST24     __FORMAT(__LEAST24__,o)
#define         SCNuLEAST24     __FORMAT(__LEAST24__,u)
#define         SCNxLEAST24     __FORMAT(__LEAST24__,x)

#define         PRIdLEAST32     __FORMAT(__LEAST32__,d)
#define         PRIiLEAST32     __FORMAT(__LEAST32__,i)
#define         PRIoLEAST32     __FORMAT(__LEAST32__,o)
#define         PRIuLEAST32     __FORMAT(__LEAST32__,u)
#define         PRIxLEAST32     __FORMAT(__LEAST32__,x)
#define         PRIXLEAST32     __FORMAT(__LEAST32__,X)
#define         SCNdLEAST32     __FORMAT(__LEAST32__,d)
#define         SCNiLEAST32     __FORMAT(__LEAST32__,i)
#define         SCNoLEAST32     __FORMAT(__LEAST32__,o)
#define         SCNuLEAST32     __FORMAT(__LEAST32__,u)
#define         SCNxLEAST32     __FORMAT(__LEAST32__,x)

#ifdef  __LEAST48__
#define         PRIdLEAST48     __FORMAT(__LEAST48__,d)
#define         PRIiLEAST48     __FORMAT(__LEAST48__,i)
#define         PRIoLEAST48     __FORMAT(__LEAST48__,o)
#define         PRIuLEAST48     __FORMAT(__LEAST48__,u)
#define         PRIxLEAST48     __FORMAT(__LEAST48__,x)
#define         PRIXLEAST48     __FORMAT(__LEAST48__,X)
#define         SCNdLEAST48     __FORMAT(__LEAST48__,d)
#define         SCNiLEAST48     __FORMAT(__LEAST48__,i)
#define         SCNoLEAST48     __FORMAT(__LEAST48__,o)
#define         SCNuLEAST48     __FORMAT(__LEAST48__,u)
#define         SCNxLEAST48     __FORMAT(__LEAST48__,x)
#endif

#ifdef  __LEAST64__
#define         PRIdLEAST64     __FORMAT(__LEAST64__,d)
#define         PRIiLEAST64     __FORMAT(__LEAST64__,i)
#define         PRIoLEAST64     __FORMAT(__LEAST64__,o)
#define         PRIuLEAST64     __FORMAT(__LEAST64__,u)
#define         PRIxLEAST64     __FORMAT(__LEAST64__,x)
#define         PRIXLEAST64     __FORMAT(__LEAST64__,X)
#define         SCNdLEAST64     __FORMAT(__LEAST64__,d)
#define         SCNiLEAST64     __FORMAT(__LEAST64__,i)
#define         SCNoLEAST64     __FORMAT(__LEAST64__,o)
#define         SCNuLEAST64     __FORMAT(__LEAST64__,u)
#define         SCNxLEAST64     __FORMAT(__LEAST64__,x)
#endif

#ifdef  __LEAST72__
#define         PRIdLEAST72     __FORMAT(__LEAST72__,d)
#define         PRIiLEAST72     __FORMAT(__LEAST72__,i)
#define         PRIoLEAST72     __FORMAT(__LEAST72__,o)
#define         PRIuLEAST72     __FORMAT(__LEAST72__,u)
#define         PRIxLEAST72     __FORMAT(__LEAST72__,x)
#define         PRIXLEAST72     __FORMAT(__LEAST72__,X)
#define         SCNdLEAST72     __FORMAT(__LEAST72__,d)
#define         SCNiLEAST72     __FORMAT(__LEAST72__,i)
#define         SCNoLEAST72     __FORMAT(__LEAST72__,o)
#define         SCNuLEAST72     __FORMAT(__LEAST72__,u)
#define         SCNxLEAST72     __FORMAT(__LEAST72__,x)
#endif

#define         PRIdFAST8       __FORMAT(__FAST8__,d)
#define         PRIiFAST8       __FORMAT(__FAST8__,i)
#define         PRIoFAST8       __FORMAT(__FAST8__,o)
#define         PRIuFAST8       __FORMAT(__FAST8__,u)
#define         PRIxFAST8       __FORMAT(__FAST8__,x)
#define         PRIXFAST8       __FORMAT(__FAST8__,X)
#define         SCNdFAST8       __FORMAT(__FAST8__,d)
#define         SCNiFAST8       __FORMAT(__FAST8__,i)
#define         SCNoFAST8       __FORMAT(__FAST8__,o)
#define         SCNuFAST8       __FORMAT(__FAST8__,u)
#define         SCNxFAST8       __FORMAT(__FAST8__,x)

#define         PRIdFAST16      __FORMAT(__FAST16__,d)
#define         PRIiFAST16      __FORMAT(__FAST16__,i)
#define         PRIoFAST16      __FORMAT(__FAST16__,o)
#define         PRIuFAST16      __FORMAT(__FAST16__,u)
#define         PRIxFAST16      __FORMAT(__FAST16__,x)
#define         PRIXFAST16      __FORMAT(__FAST16__,X)
#define         SCNdFAST16      __FORMAT(__FAST16__,d)
#define         SCNiFAST16      __FORMAT(__FAST16__,i)
#define         SCNoFAST16      __FORMAT(__FAST16__,o)
#define         SCNuFAST16      __FORMAT(__FAST16__,u)
#define         SCNxFAST16      __FORMAT(__FAST16__,x)

#define         PRIdFAST24      __FORMAT(__FAST24__,d)
#define         PRIiFAST24      __FORMAT(__FAST24__,i)
#define         PRIoFAST24      __FORMAT(__FAST24__,o)
#define         PRIuFAST24      __FORMAT(__FAST24__,u)
#define         PRIxFAST24      __FORMAT(__FAST24__,x)
#define         PRIXFAST24      __FORMAT(__FAST24__,X)
#define         SCNdFAST24      __FORMAT(__FAST24__,d)
#define         SCNiFAST24      __FORMAT(__FAST24__,i)
#define         SCNoFAST24      __FORMAT(__FAST24__,o)
#define         SCNuFAST24      __FORMAT(__FAST24__,u)
#define         SCNxFAST24      __FORMAT(__FAST24__,x)

#define         PRIdFAST32      __FORMAT(__FAST32__,d)
#define         PRIiFAST32      __FORMAT(__FAST32__,i)
#define         PRIoFAST32      __FORMAT(__FAST32__,o)
#define         PRIuFAST32      __FORMAT(__FAST32__,u)
#define         PRIxFAST32      __FORMAT(__FAST32__,x)
#define         PRIXFAST32      __FORMAT(__FAST32__,X)
#define         SCNdFAST32      __FORMAT(__FAST32__,d)
#define         SCNiFAST32      __FORMAT(__FAST32__,i)
#define         SCNoFAST32      __FORMAT(__FAST32__,o)
#define         SCNuFAST32      __FORMAT(__FAST32__,u)
#define         SCNxFAST32      __FORMAT(__FAST32__,x)

#ifdef  __FAST48__
#define         PRIdFAST48      __FORMAT(__FAST48__,d)
#define         PRIiFAST48      __FORMAT(__FAST48__,i)
#define         PRIoFAST48      __FORMAT(__FAST48__,o)
#define         PRIuFAST48      __FORMAT(__FAST48__,u)
#define         PRIxFAST48      __FORMAT(__FAST48__,x)
#define         PRIXFAST48      __FORMAT(__FAST48__,X)
#define         SCNdFAST48      __FORMAT(__FAST48__,d)
#define         SCNiFAST48      __FORMAT(__FAST48__,i)
#define         SCNoFAST48      __FORMAT(__FAST48__,o)
#define         SCNuFAST48      __FORMAT(__FAST48__,u)
#define         SCNxFAST48      __FORMAT(__FAST48__,x)
#endif

#ifdef  __FAST64__
#define         PRIdFAST64      __FORMAT(__FAST64__,d)
#define         PRIiFAST64      __FORMAT(__FAST64__,i)
#define         PRIoFAST64      __FORMAT(__FAST64__,o)
#define         PRIuFAST64      __FORMAT(__FAST64__,u)
#define         PRIxFAST64      __FORMAT(__FAST64__,x)
#define         PRIXFAST64      __FORMAT(__FAST64__,X)
#define         SCNdFAST64      __FORMAT(__FAST64__,d)
#define         SCNiFAST64      __FORMAT(__FAST64__,i)
#define         SCNoFAST64      __FORMAT(__FAST64__,o)
#define         SCNuFAST64      __FORMAT(__FAST64__,u)
#define         SCNxFAST64      __FORMAT(__FAST64__,x)
#endif

#ifdef  __FAST72__
#define         PRIdFAST72      __FORMAT(__FAST72__,d)
#define         PRIiFAST72      __FORMAT(__FAST72__,i)
#define         PRIoFAST72      __FORMAT(__FAST72__,o)
#define         PRIuFAST72      __FORMAT(__FAST72__,u)
#define         PRIxFAST72      __FORMAT(__FAST72__,x)
#define         PRIXFAST72      __FORMAT(__FAST72__,X)
#define         SCNdFAST72      __FORMAT(__FAST72__,d)
#define         SCNiFAST72      __FORMAT(__FAST72__,i)
#define         SCNoFAST72      __FORMAT(__FAST72__,o)
#define         SCNuFAST72      __FORMAT(__FAST72__,u)
#define         SCNxFAST72      __FORMAT(__FAST72__,x)
#endif

#define         PRIdMAX         __FORMAT(__INTMAX__,d)
#define         PRIiMAX         __FORMAT(__INTMAX__,i)
#define         PRIoMAX         __FORMAT(__INTMAX__,o)
#define         PRIuMAX         __FORMAT(__INTMAX__,u)
#define         PRIxMAX         __FORMAT(__INTMAX__,x)
#define         PRIXMAX         __FORMAT(__INTMAX__,X)
#define         SCNdMAX         __FORMAT(__INTMAX__,d)
#define         SCNiMAX         __FORMAT(__INTMAX__,i)
#define         SCNoMAX         __FORMAT(__INTMAX__,o)
#define         SCNuMAX         __FORMAT(__INTMAX__,u)
#define         SCNxMAX         __FORMAT(__INTMAX__,x)

#define         PRIdPTR         __FORMAT(__INTPTR__,d)
#define         PRIiPTR         __FORMAT(__INTPTR__,i)
#define         PRIoPTR         __FORMAT(__INTPTR__,o)
#define         PRIuPTR         __FORMAT(__INTPTR__,u)
#define         PRIxPTR         __FORMAT(__INTPTR__,x)
#define         PRIXPTR         __FORMAT(__INTPTR__,X)
#define         SCNdPTR         __FORMAT(__INTPTR__,d)
#define         SCNiPTR         __FORMAT(__INTPTR__,i)
#define         SCNoPTR         __FORMAT(__INTPTR__,o)
#define         SCNuPTR         __FORMAT(__INTPTR__,u)
#define         SCNxPTR         __FORMAT(__INTPTR__,x)

#endif /* ! defined __cplusplus || defined __STDC_FORMAT_MACROS */

/*
 * 7.8.2 Functions for greatest-width integer types
 */
extern  intmax_t        imaxabs(intmax_t j);
extern  imaxdiv_t       imaxdiv(intmax_t numer, intmax_t denom);
extern  intmax_t        strtoimax(const char * restrict nptr, char ** restrict endptr, int base);
extern  uintmax_t       strtoumax(const char * restrict nptr, char ** restrict endptr, int base);
extern  intmax_t        wcstoimax(const wchar_t * restrict nptr, wchar_t ** restrict endptr, int base);
extern  uintmax_t       wcstoumax(const wchar_t * restrict nptr, wchar_t ** restrict endptr, int base);

#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#if   __MISRAC_VERSION__ >= 1998 && __MISRAC_VERSION__ <= 2012
#pragma nomisrac restore
#endif

#endif  /* _INTTYPES_H */
