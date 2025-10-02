/**************************************************************************
**                                                                        *
**  FILE        :  time.h                                                 *
**                                                                        *
**  DESCRIPTION :  Include file with prototypes and macros for            *
**                 manipulating time.                                     *
**                 The low level time routine clock() strongly depends on *
**                 the hardware (e.g. RTC-hardware), so this routine may  *
**                 have to be customised. To determine the time in        *
**                 seconds, the value returned by clock() should be       *
**                 divided by the value of the macro CLOCKS_PER_SEC.      *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _TIME_H

#if   __MISRAC_VERSION__ == 1998
#pragma nomisrac 1,13,14,114,115
#elif __MISRAC_VERSION__ == 2004
#pragma nomisrac 1.1,6.3,20.1,20.2
#elif __MISRAC_VERSION__ == 2012
#pragma nomisrac 1.2,2.5,8.2,8.14,21.1,21.2
#endif

#define _TIME_H 1

#ifdef  __cplusplus
extern "C" {
#endif  /* defined(__cplusplus) */

#ifndef _SIZE_T_DEF
typedef __size_t        size_t;
#define _SIZE_T_DEF     1
#endif


#if __STDC_VERSION__ > 199409
typedef unsigned long long      clock_t;
#else
typedef unsigned long   clock_t;
#endif
typedef unsigned long   time_t;

#define TIME_MAX (time_t)(-1)
#define TIME_MIN (time_t)(0)

#ifdef  __cplusplus
#define NULL    (0)
#else   /* !defined(__cplusplus) */
#define NULL    ((void *) 0)
#endif  /* defined(__cplusplus) */

#ifndef CLOCKS_PER_SEC
# define CLOCKS_PER_SEC  ((clock_t)__clocks_per_sec)    /* resolution of clock() */
#endif
extern  clock_t __far           __clocks_per_sec;


#ifndef _STRUCT_TM
#define _STRUCT_TM      1

struct tm
{
        int     tm_sec;         /* seconds after the minute - [0, 59]   */
        int     tm_min;         /* minutes after the hour - [0, 59]     */
        int     tm_hour;        /* hours since midnight - [0, 23]       */
        int     tm_mday;        /* day of the month - [1, 31]           */
        int     tm_mon;         /* months since January - [0, 11]       */
        int     tm_year;        /* year since 1900                      */
        int     tm_wday;        /* days since Sunday - [0, 6]           */
        int     tm_yday;        /* days since January 1 - [0, 365]      */
        int     tm_isdst;       /* Daylight Saving Time flag            */
};

#endif /*_STRUCT_TM */

extern  char      * asctime(const struct tm *);
extern  char      * ctime(const time_t *);
extern  double      difftime(time_t, time_t);
extern  struct tm * gmtime(const time_t *);
extern  struct tm * localtime(const time_t *);
extern  time_t      time(time_t *);
extern  time_t      mktime(struct tm *);
extern  clock_t     clock(void);
extern  void        _timer_start(void);         /* start timer */
extern  clock_t     _timer_stop(void);          /* stop timer, return elapsed time */
extern  size_t      strftime(char * restrict, size_t, const char * restrict,
                             const struct tm * restrict);

extern  void        _stime(time_t *);           /* sets system time */
extern  int         _tzset(const char *);       /* sets time zone */

#ifdef  __cplusplus
}
#endif  /* defined(__cplusplus) */

#pragma nomisrac restore

#endif  /* _TIME_H */
