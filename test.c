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

TS_time_32 time_struct;

uint8_t leapyear_check(uint16_t year_u16)
{
    return (0 == year_u16 % 4 && year_u16 % 100 != 0) || year_u16 % 400 == 0;
}

const uint32_t month_buf[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t TS2RTC(uint32_t TS_in, TS_time_32 *RTC_out)
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

    min_add = TS_in / 60;
    RTC_out->sec = (uint8_t)(TS_in % 60);

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

uint32_t RTC2TS(const TS_time_32 *RTC_in)
{
    uint32_t TS_out = 0;
    uint32_t h_year = (uint32_t)RTC_in->h_year;
    uint32_t h_yearMULTI100 = h_year * 100;
    uint32_t l_year = (uint32_t)RTC_in->l_year;
    uint32_t year = h_yearMULTI100 + l_year;
    uint16_t y_u16;
    uint32_t daySUB1 = (uint32_t)(RTC_in->day - 1);
    uint32_t day2stamp = daySUB1 * 86400;
    uint32_t hour = (uint32_t)RTC_in->hour;
    uint32_t hour2stamp = hour * 3600;
    uint32_t min2stamp = (uint32_t)RTC_in->min * 60;
    uint32_t sec2stamp = (uint32_t)RTC_in->sec;
    uint8_t m_u8;

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
    TS_time_32 RTC_from_TS;
    TS_time_32 RTC_to_TS;
    uint32_t TS_from_RTC;

    uint32_t RTC_from_TS_h_year;
    uint32_t RTC_from_TS_l_year;
    uint32_t RTC_from_TS_mon;
    uint32_t RTC_from_TS_day;
    uint32_t RTC_from_TS_hour;
    uint32_t RTC_from_TS_min;
    uint32_t RTC_from_TS_sec;

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
        RTC_from_TS_h_year = (uint32_t)RTC_from_TS.h_year;
        RTC_from_TS_l_year = (uint32_t)RTC_from_TS.l_year;
        RTC_from_TS_mon = (uint32_t)RTC_from_TS.mon;
        RTC_from_TS_day = (uint32_t)RTC_from_TS.day;
        RTC_from_TS_hour = (uint32_t)RTC_from_TS.hour;
        RTC_from_TS_min = (uint32_t)RTC_from_TS.min;
        RTC_from_TS_sec = (uint32_t)RTC_from_TS.sec;

        printf("%02lu%02lu-%02lu-%02lu %02lu:%02lu:%02lu\n", RTC_from_TS.h_year, RTC_from_TS.l_year, RTC_from_TS.mon, RTC_from_TS.day, RTC_from_TS.hour, RTC_from_TS.min, RTC_from_TS.sec);

        TS_from_RTC = RTC2TS(&RTC_to_TS);
        printf("Unix timestamp: %lu\n", TS_from_RTC);
    }
}

void main(void)
{
    CRTCTS_init();
}