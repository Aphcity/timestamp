#include <REG52.H> /* special function register declarations   */
/* for the intended 8051 derivative         */
#include <stdio.h>
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

TS_time_ time_struct;

uint32_t leapyear_check(uint32_t y)
{
    return (0 == y % 4 && y % 100 != 0) || y % 400 == 0;
}

const uint32_t month_buf[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t TS2RTC(uint32_t unixtimestamp, TS_time_ *RTC_out)
{
    uint32_t min_add, hour_add, day_add, mon_add = 0, year_add = 0;
    uint32_t day_temp, mon_temp, year_temp;
    uint32_t i, a, temp;

    RTC_out->h_year = 19;
    RTC_out->l_year = 70;
    RTC_out->mon = 1;
    RTC_out->day = 1;
    RTC_out->hour = 0;
    RTC_out->min = 0;
    RTC_out->sec = 0;

    min_add = unixtimestamp / 60;
    RTC_out->sec = (uint8_t)(unixtimestamp % 60);

    hour_add = min_add / 60;
    RTC_out->min = (uint8_t)(min_add % 60);

    day_add = hour_add / 24;
    RTC_out->hour = (uint8_t)(hour_add % 24);

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
    RTC_out->mon = (uint8_t)((mon_temp % 12 == 0) ? 12 : mon_temp % 12);

    if (year_add > 30)
    {
        year_add -= 30;
        RTC_out->h_year = (uint8_t)(20 + year_add / 100);
        year_add = year_add % 100;
        RTC_out->l_year = 0;
    }
    else
        RTC_out->h_year = 19;

    RTC_out->l_year += (uint8_t)year_add;

    if (RTC_out->h_year != 19 && RTC_out->l_year != 70 && RTC_out->mon != 1 && RTC_out->day != 1 && RTC_out->hour != 0 && RTC_out->min != 0 && RTC_out->sec != 0)
        return 1;
    return -1;
}

uint32_t timetostamp(TS_time_ t_in)
{
    uint32_t stamp = 0;
    uint32_t h_year = (uint32_t)t_in.h_year;
    uint32_t h_yearMULTI100 = h_year * 100;
    uint32_t l_year = (uint32_t)t_in.l_year;
    uint32_t year = h_yearMULTI100 + l_year;
    uint32_t y;
    uint32_t daySUB1 = (uint32_t)(t_in.day - 1);
    uint32_t day2stamp = daySUB1 * 86400;
    uint32_t hour = (uint32_t)t_in.hour;
    uint32_t hour2stamp = hour * 3600;
    uint32_t min2stamp = (uint32_t)t_in.min * 60;
    uint32_t sec2stamp = (uint32_t)t_in.sec;
    uint32_t m;

    for (y = 1970; y < year; y++)
        stamp += leapyear_check(y) ? 366 * 86400 : 365 * 86400;

    for (m = 0; m < (uint32_t)(t_in.mon - 1); m++)
        if (m == 1 && leapyear_check(year))
            stamp += 29 * 86400;
        else
            stamp += month_buf[m] * 86400;

    stamp += day2stamp + hour2stamp + min2stamp + sec2stamp;

    return stamp;
}

void main(void)
{
    uint32_t unixtimestamp;
    TS_time_ t_in;
    TS_time_ t_out;
    uint32_t stamp;

    uint32_t t_in_h_year;
    uint32_t t_in_l_year;
    uint32_t t_in_mon;
    uint32_t t_in_day;
    uint32_t t_in_hour;
    uint32_t t_in_min;
    uint32_t t_in_sec;

    unixtimestamp = 1704071461;
    t_out.h_year = 20;
    t_out.l_year = 24;
    t_out.mon = 1;
    t_out.day = 1;
    t_out.hour = 1;
    t_out.min = 11;
    t_out.sec = 1;

    L_pok_init();

    while (1)
    {
        TS2RTC(unixtimestamp, &t_in);
        t_in_h_year = (uint32_t)t_in.h_year;
        t_in_l_year = (uint32_t)t_in.l_year;
        t_in_mon = (uint32_t)t_in.mon;
        t_in_day = (uint32_t)t_in.day;
        t_in_hour = (uint32_t)t_in.hour;
        t_in_min = (uint32_t)t_in.min;
        t_in_sec = (uint32_t)t_in.sec;

        printf("%02lu%02lu-%02lu-%02lu %02lu:%02lu:%02lu\n", t_in.h_year, t_in.l_year, t_in.mon, t_in.day, t_in.hour, t_in.min, t_in.sec);

        stamp = timetostamp(t_out);
        printf("Unix timestamp: %lu\n", stamp);
    }
}