#include <stdio.h>

typedef signed char int8_t;      /**<  8bit integer type */
typedef signed short int16_t;    /**< 16bit integer type */
typedef signed long int32_t;      /**< 32bit integer type */
typedef unsigned char uint8_t;   /**<  8bit unsigned integer type */
typedef unsigned short uint16_t; /**< 16bit unsigned integer type */
typedef unsigned long uint32_t;   /**< 32bit unsigned integer type */
//typedef unsigned long uint64_t;  /**< 64bit unsigned integer type */

typedef struct
{
    uint32_t h_year;
    uint32_t l_year;
    uint32_t mon;
    uint32_t day;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
} TS_time_;

extern uint32_t leapyear_check(uint32_t y);

extern uint32_t timetostamp(TS_time_ t_in);

extern uint32_t TS2RTC(uint32_t unixtimestamp, TS_time_ *RTC_out);
