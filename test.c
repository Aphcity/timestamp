#include <REG52.H> /* special function register declarations   */
/* for the intended 8051 derivative         */
#include <stdio.h>
#include <stdlib.h>
#include "timestamp.h"

#ifdef MONITOR51                /* Debugging with Monitor-51 needs   */
char code reserve[3] _at_ 0x23; /* space for serial interrupt if     */
#endif                          /* Stop Exection with Serial Intr.   */

void L_pok_init(void)
{
#ifndef MONITOR51
    SCON = 0x50;  /* SCON: mode 1, 8-bit UART, enable rcvr      */
    TMOD |= 0x20; /* TMOD: timer 1, mode 2, 8-bit reload        */
    TH1 = 221;    /* TH1:  reload value for 1200 baud @ 16MHz   */
    TR1 = 1;      /* TR1:  timer 1 run                          */
    TI = 1;       /* TI:   set TI to send first char of UART    */
#endif
}

uint8_t leapyear_check(uint16_t year_u16)
{
    return (0 == year_u16 % 4 && year_u16 % 100 != 0) || year_u16 % 400 == 0;
}

RTC_time_16 *RTC8to16(const RTC_time_8 *RTC8)
{
    RTC_time_16 RTC16;
    RTC16.h_year = (uint16_t)RTC8->h_year;
    RTC16.l_year = (uint16_t)RTC8->l_year;
    RTC16.mon = (uint16_t)RTC8->mon;
    RTC16.day = (uint16_t)RTC8->day;
    RTC16.hour = (uint16_t)RTC8->hour;
    RTC16.min = (uint16_t)RTC8->min;
    RTC16.sec = (uint16_t)RTC8->sec;
    return &RTC16;
}

RTC_time_32 *RTC16to32(const RTC_time_16 *RTC16)
{
    RTC_time_32 TS32;
    TS32.h_year = (uint32_t)RTC16->h_year;
    TS32.l_year = (uint32_t)RTC16->l_year;
    TS32.mon = (uint32_t)RTC16->mon;
    TS32.day = (uint32_t)RTC16->day;
    TS32.hour = (uint32_t)RTC16->hour;
    TS32.min = (uint32_t)RTC16->min;
    TS32.sec = (uint32_t)RTC16->sec;
    return &TS32;
}

RTC_time_32 *RTC8to32(const RTC_time_8 *RTC8)
{
    return RTC16to32(RTC8to16(RTC8));
}

// uint8_t RTC32to16(const RTC_time_32 *RTC32, RTC_time_16 *RTC16)
// {
//     uint16_t *arr;
//     arr = (uint16_t *)malloc(sizeof(uint16_t));
//     arr = (uint16_t *)&RTC32->h_year;
//     RTC16->h_year = *arr;
//     arr = (uint16_t *)&RTC32->l_year;
//     RTC16->l_year = *arr;
//     arr = (uint16_t *)&RTC32->mon;
//     RTC16->mon = *arr;
//     arr = (uint16_t *)&RTC32->day;
//     RTC16->day = *arr;
//     arr = (uint16_t *)&RTC32->hour;
//     RTC16->hour = *arr;
//     arr = (uint16_t *)&RTC32->min;
//     RTC16->min = *arr;
//     arr = (uint16_t *)&RTC32->sec;
//     RTC16->sec = *arr;
//     free(arr);
//     return 1;
// }

// uint8_t RTC16to8(const RTC_time_16 *RTC16, RTC_time_8 *RTC8)
// {
//     uint8_t *arr;
//     arr = (uint8_t *)malloc(sizeof(uint8_t));
//     arr = (uint8_t *)&RTC16->h_year;
//     RTC8->h_year = *arr;
//     arr = (uint8_t *)&RTC16->l_year;
//     RTC8->l_year = *arr;
//     arr = (uint8_t *)&RTC16->mon;
//     RTC8->mon = *arr;
//     arr = (uint8_t *)&RTC16->day;
//     RTC8->day = *arr;
//     arr = (uint8_t *)&RTC16->hour;
//     RTC8->hour = *arr;
//     arr = (uint8_t *)&RTC16->min;
//     RTC8->min = *arr;
//     arr = (uint8_t *)&RTC16->sec;
//     RTC8->sec = *arr;
//     free(arr);
//     return 1;
// }

// uint8_t RTC32to8(const RTC_time_32 *RTC32, RTC_time_8 *RTC8)
// {
//     RTC_time_16 *RTC16;
//     RTC32to16(RTC32, RTC16);
//     RTC16to8(RTC16, RTC8);
//     return 1;
// }

uint8_t RTC_init(RTC_time_8 *RTC8)
{
    RTC8->h_year = 19;
    RTC8->l_year = 70;
    RTC8->mon = 1;
    RTC8->day = 1;
    RTC8->hour = 0;
    RTC8->min = 0;
    RTC8->sec = 0;
    return 1;
}

uint8_t RTC_print(const RTC_time_8 *RTC8)
{
    RTC_time_16 *RTC;
    RTC = RTC8to16(RTC8);
    printf("RTC: %02u%02u-%02u-%02u %02u:%02u:%02u\n", RTC->h_year, RTC->l_year, RTC->mon, RTC->day, RTC->hour, RTC->min, RTC->sec);

    return 1;
}

const uint32_t month_buf[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t TS2RTC(uint32_t TS_in, RTC_time_8 *RTC_out)
{
    uint32_t min_add, hour_add, day_add, mon_add = 0, year_add = 0;
    uint32_t day_temp, mon_temp, year_temp;
    uint32_t i, a, temp;

    // RTC_time_32 *RTC_out_32;

    RTC_init(RTC_out);
    // RTC_out_32 = RTC8to32(RTC_out);

    min_add = TS_in / 60;
    RTC_out->sec = TS_in % 60;

    hour_add = min_add / 60;
    RTC_out->min = min_add % 60;

    day_add = hour_add / 24;
    RTC_out->hour = hour_add % 24;

    day_temp = RTC_out->hour;

    for (i = RTC_out->mon - 1;; i++)
    {
        a = i % 12;
        year_temp = RTC_out->h_year * 100 + RTC_out->l_year + (i / 12);
        temp = (a == 1 && leapyear_check(year_temp)) ? (month_buf[a] + 1) : month_buf[a];

        if (mon_add)
        {
            day_temp = 1;
        }

        if (day_add + day_temp > temp)
        {
            mon_add++;
            day_add -= (temp - day_temp + 1);
        }
        else
        {
            RTC_out->day += day_add;
            break;
        }
    }

    mon_temp = RTC_out->mon + mon_add;
    year_add = (mon_temp % 12 == 0) ? mon_temp / 12 - 1 : mon_temp / 12;
    RTC_out->mon = (mon_temp % 12 == 0) ? 12 : mon_temp % 12;

    if (year_add > 30)
    {
        year_add -= 30;
        RTC_out->h_year = 20 + year_add / 100;
        year_add = year_add % 100;
        RTC_out->l_year = 0;
    }
    else
        RTC_out->h_year = 19;

    RTC_out->l_year += year_add;

    // RTC32to8(RTC_out, RTC_out);
    return 1;
}

uint32_t RTC2TS(const RTC_time_8 *RTC_in)
{
    uint32_t TS_out, year, day2stamp, hour2stamp, min2stamp, sec2stamp;
    uint16_t y_u16;
    uint8_t m_u8;

    TS_out = 0;
    year = (uint32_t)RTC_in->h_year * 100L + (uint32_t)RTC_in->l_year;
    day2stamp = (uint32_t)(RTC_in->day - 1) * 86400L;
    hour2stamp = (uint32_t)RTC_in->hour * 3600L;
    min2stamp = (uint32_t)RTC_in->min * 60L;
    sec2stamp = (uint32_t)RTC_in->sec;

    for (y_u16 = 1970; y_u16 < year; y_u16++)
    {
        TS_out += leapyear_check(y_u16) ? 366 * 86400 : 365 * 86400;
    }

    for (m_u8 = 0; m_u8 < (RTC_in->mon - 1); m_u8++)
    {
        if (m_u8 == 1 && leapyear_check(year))
            TS_out += 29 * 86400;
        else
            TS_out += month_buf[m_u8] * 86400;
    }

    TS_out += (day2stamp + hour2stamp + min2stamp + sec2stamp);

    return TS_out;
}

void CRTCTS_init(void)
{
    uint32_t TS_to_RTC;
    RTC_time_8 RTC_from_TS;
    RTC_time_8 RTC_to_TS;
    uint32_t TS_from_RTC;

    TS_to_RTC = 1704071461;
    RTC_to_TS.h_year = 20;
    RTC_to_TS.l_year = 24;
    RTC_to_TS.mon = 12;
    RTC_to_TS.day = 1;
    RTC_to_TS.hour = 1;
    RTC_to_TS.min = 11;
    RTC_to_TS.sec = 1;

    L_pok_init();

    // while (0)
    while (1)
    {
        scanf("%lu", &TS_to_RTC);
        TS2RTC(TS_to_RTC, &RTC_from_TS);
        RTC_print(&RTC_from_TS);
        TS_from_RTC = RTC2TS(&RTC_to_TS);
        printf("Unix timestamp: %lu\n", TS_from_RTC);
    }
}

void main(void)
{
    CRTCTS_init();
}