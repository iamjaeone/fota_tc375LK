/**************************************************************************
**                                                                        *
**  FILE        :  sevt.h                                                 *
**                                                                        *
**  DESCRIPTION : Single Entry Vector Table, for core tc1.6.x.            *
**                                                                        *
**                Interrupt handlers can be installed with the            *
**                _sevt_isr_install(), in _sevt_isrs_tc0/1/2 arrays       *
**                The SEVT ISR arrays can be switch with the              *
**                _sevt_isr_install_array.                                *
**                                                                        *
**  Copyright 1996-2022 Altium BV                                         *
**                                                                        *
**************************************************************************/

#ifndef _SEVT_H
#define _SEVT_H

typedef void(*_sevt_isr_type)(void);
extern void _sevt_isr_install( unsigned char irpn, _sevt_isr_type isr, unsigned int core );
extern void _sevt_isr_install_array( _sevt_isr_type * isr_array, unsigned int core );

extern __far _sevt_isr_type _sevt_isrs1_tc0[256];
extern __far _sevt_isr_type _sevt_isrs1_tc1[256];
extern __far _sevt_isr_type _sevt_isrs1_tc2[256];

#endif /* _SEVT_H */
