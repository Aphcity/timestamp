#include <stdint.h>

typedef signed char int8_t;      /**<  8bit integer type */
typedef signed short int16_t;    /**< 16bit integer type */
typedef signed int int32_t;      /**< 32bit integer type */
typedef unsigned char uint8_t;   /**<  8bit unsigned integer type */
typedef unsigned short uint16_t; /**< 16bit unsigned integer type */
typedef unsigned int uint32_t;   /**< 32bit unsigned integer type */

typedef struct time_stamp
{
    uint8_t h_year;
    uint8_t l_year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} time_struct;

uint32_t leapyear_check(uint32_t y)
{
    if ((0 == y % 4 && y % 100 != 0) || y % 400 == 0)
        return 1;
    return 0;
}

time_struct stamptotime(uint32_t stamp)
{
    const uint8_t month_buf[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    time_struct t_out;
    uint32_t min_add, hour_add, day_add, mon_add, year_add, day_temp, year_temp;

    t_out.h_year = 19;
    t_out.l_year = 70;
    t_out.mon = 1;
    t_out.day = 1;
    t_out.hour = 0;
    t_out.min = 0;
    t_out.sec = 0;

    min_add = (t_out.sec + stamp) / 60;
    t_out.sec = (t_out.sec + stamp) % 60;

    hour_add = (t_out.min + min_add) / 60;
    t_out.min = (t_out.min + min_add) % 60;

    day_add = (t_out.hour + hour_add) / 24;
    t_out.hour = (t_out.hour + hour_add) % 24;

    mon_add = 0;
    day_temp = t_out.day;

    for (uint32_t i = t_out.mon - 1;; i++)
    {
        uint8_t a = i % 12;

        year_temp = t_out.h_year * 100 + t_out.l_year + (i / 12);

        uint32_t temp = (a == 1 && leapyear_check(year_temp)) ? (month_buf[a] + 1) : month_buf[a];

        /* If we update the month, initial the day to the '1' */
        if (mon_add)
            day_temp = 1;

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

    uint32_t mon_temp = t_out.mon + mon_add;
    year_add = (mon_temp % 12 == 0) ? mon_temp / 12 - 1 : mon_temp / 12;
    t_out.mon = (mon_temp % 12 == 0) ? 12 : mon_temp % 12;

    // 大于30年时重置基准至2000年1月1日0时0分0秒
    if (year_add > 30)
    {
        year_add -= 30;
        t_out.h_year = 20 + year_add / 100;
        year_add = year_add % 100;
        t_out.l_year = 0;
    }
    else
        t_out.h_year = 19;

    t_out.l_year += year_add;

    return t_out;
}

uint32_t timetostamp(time_struct t_in)
{
    const uint8_t month_buf[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint32_t stamp = 0;
    uint32_t year = t_in.h_year * 100 + t_in.l_year;

    // 计算已过年的秒数
    for (uint32_t y = 1970; y < year; y++)
        stamp += leapyear_check(y) ? 366 * 86400 : 365 * 86400;

    // 计算已过月的秒数
    for (uint8_t m = 0; m < t_in.mon - 1; m++)
    {
        if (m == 1 && leapyear_check(year)) // February in a leap year
            stamp += 29 * 86400;
        else
            stamp += month_buf[m] * 86400;
    }

    // 计算已过天的秒数
    stamp += (t_in.day - 1) * 86400;

    // 计算已过时分秒的秒数
    stamp += t_in.hour * 3600;
    stamp += t_in.min * 60;
    stamp += t_in.sec;

    // 返回 int
    return stamp;
}