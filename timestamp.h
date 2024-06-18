#include <stdio.h>

typedef signed char int8_t;      /**<  8bit integer type */
typedef signed short int16_t;    /**< 16bit integer type */
typedef signed int int32_t;      /**< 32bit integer type */
typedef unsigned char uint8_t;   /**<  8bit unsigned integer type */
typedef unsigned short uint16_t; /**< 16bit unsigned integer type */
typedef unsigned int uint32_t;   /**< 32bit unsigned integer type */

typedef struct
{
    uint8_t h_year;
    uint8_t l_year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} TS_time_;

extern uint32_t leapyear_check(uint32_t y);

extern uint32_t timetostamp(TS_time_ t_in);

extern uint8_t TS2RTC(uint32_t unixtimestamp, TS_time_ *RTC_out);
