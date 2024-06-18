// #include "stdafx.h"
#include <REG52.H> /* special function register declarations   */
                   /* for the intended 8051 derivative         */
#include <stdio.h>
#include "timestamp.h"

#ifdef MONITOR51                /* Debugging with Monitor-51 needs   */
char code reserve[3] _at_ 0x23; /* space for serial interrupt if     */
#endif                          /* Stop Exection with Serial Intr.   */

/*------------------------------------------------
The main C function.  Program execution starts
here after stack initialization.
------------------------------------------------*/
void L_pok_init(void)
{
/*------------------------------------------------
Setup the serial port for 1200 baud at 16MHz.
------------------------------------------------*/
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

const uint8_t month_buf[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

TS_time_ stamptotime(uint32_t stamp)
{

    TS_time_ t_out = {19, 70, 1, 1, 0, 0, 0};
    uint32_t min_add, hour_add, day_add, mon_add = 0, year_add = 0;
    uint32_t day_temp, mon_temp, year_temp;
    uint32_t i;
    uint32_t a;
    uint32_t temp;

    min_add = (t_out.sec + stamp) / 60;
    t_out.sec = (t_out.sec + stamp) % 60;

    hour_add = (t_out.min + min_add) / 60;
    t_out.min = (t_out.min + min_add) % 60;

    day_add = (t_out.hour + hour_add) / 24;
    t_out.hour = (t_out.hour + hour_add) % 24;

    day_temp = t_out.day;

    for (i = t_out.mon - 1;; i++)
    {
        a = i % 12;
        year_temp = t_out.h_year * 100 + t_out.l_year + (i / 12);
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
            t_out.day += day_add;
            break;
        }
    }

    mon_temp = t_out.mon + mon_add;
    year_add = (mon_temp % 12 == 0) ? mon_temp / 12 - 1 : mon_temp / 12;
    t_out.mon = (mon_temp % 12 == 0) ? 12 : mon_temp % 12;

    if (year_add > 30)
    {
        year_add -= 30;
        t_out.h_year = 20 + year_add / 100;
        year_add = year_add % 100;
        t_out.l_year = 0;
    }
    else
    {
        t_out.h_year = 19;
    }

    t_out.l_year += year_add;

    return t_out;
}

uint8_t TS2RTC(uint32_t unixtimestamp, TS_time_ *RTC_out)
{
    uint32_t min_add, hour_add, day_add, mon_add = 0, year_add = 0;
    uint32_t day_temp, mon_temp, year_temp;
    uint32_t i;
    uint32_t a;
    uint32_t temp;

    RTC_out->h_year = 19;
    RTC_out->l_year = 70;
    RTC_out->mon = 1;
    RTC_out->day = 1;
    RTC_out->hour = 0;
    RTC_out->min = 0;
    RTC_out->sec = 0;

    min_add = (RTC_out->sec + unixtimestamp) / 60;
    RTC_out->sec = (RTC_out->sec + unixtimestamp) % 60;

    hour_add = (RTC_out->min + min_add) / 60;
    RTC_out->min = (RTC_out->min + min_add) % 60;

    day_add = (RTC_out->hour + hour_add) / 24;
    RTC_out->hour = (RTC_out->hour + hour_add) % 24;

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
    {
        RTC_out->h_year = 19;
    }

    RTC_out->l_year += year_add;

    if (RTC_out->h_year != 19 && RTC_out->l_year != 70 && RTC_out->mon != 1 && RTC_out->day != 1 && RTC_out->hour != 0 && RTC_out->min != 0 && RTC_out->sec != 0)
        return 1;
    return -1;
}

uint32_t timetostamp(TS_time_ t_in)
{
    uint32_t stamp = 0;
    uint32_t year = t_in.h_year * 100 + t_in.l_year;
    uint32_t y;
    uint8_t m;

    for (y = 1970; y < year; y++)
    {
        stamp += leapyear_check(y) ? 366 * 86400 : 365 * 86400;
    }

    for (m = 0; m < t_in.mon - 1; m++)
    {
        if (m == 1 && leapyear_check(year))
        {
            stamp += 29 * 86400;
        }
        else
        {
            stamp += month_buf[m] * 86400;
        }
    }

    stamp += (t_in.day - 1) * 86400;
    stamp += t_in.hour * 3600;
    stamp += t_in.min * 60;
    stamp += t_in.sec;

    return stamp;
}

void main(void)
{

    uint32_t unixtimestamp;

    TS_time_ t_in;
    TS_time_ t_out;
    uint32_t stamp;

    unixtimestamp = 114514;
    // t_out = {20, 24, 12, 14, 12, 0, 0}; // Jun 14 2024 120000
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
        // scanf("%lu", &unixtimestamp);

        if (TS2RTC(unixtimestamp, &t_in) != -1)
            printf("%02d%02d-%02d-%02d %02d:%02d:%02d\n", (int)t_in.h_year, (int)t_in.l_year, (int)t_in.mon, (int)t_in.day, (int)t_in.hour, (int)t_in.min, (int)t_in.sec);
        else
            printf("Illegal Input!\n");

        // t_in = stamptotime(unixtimestamp);
        // printf("%02d%02d-%02d-%02d %02d:%02d:%02d\n", (int)t_in.h_year, (int)t_in.l_year, (int)t_in.mon, (int)t_in.day, (int)t_in.hour, (int)t_in.min, (int)t_in.sec);

        stamp = timetostamp(t_out);
        printf("Unix timestamp: %lu\n", stamp);
    }
}
