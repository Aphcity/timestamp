#include <stdio.h>

typedef signed char int8_t;      /**<  8bit integer type */
typedef signed short int16_t;    /**< 16bit integer type */
typedef signed long int32_t;     /**< 32bit integer type */
typedef unsigned char uint8_t;   /**<  8bit unsigned integer type */
typedef unsigned short uint16_t; /**< 16bit unsigned integer type */
typedef unsigned long uint32_t;  /**< 32bit unsigned integer type */

typedef struct
{
    uint32_t h_year;
    uint32_t l_year;
    uint32_t mon;
    uint32_t day;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
} RTC_time_32;

typedef struct
{
    uint16_t h_year;
    uint16_t l_year;
    uint16_t mon;
    uint16_t day;
    uint16_t hour;
    uint16_t min;
    uint16_t sec;
} RTC_time_16;

typedef struct
{
    uint8_t h_year;
    uint8_t l_year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} RTC_time_8;

extern uint8_t leapyear_check(uint16_t year_u16);

extern uint32_t RTC2TS(const RTC_time_8 *RTC_in);

extern uint32_t TS2RTC(uint32_t TS_in, RTC_time_8 *RTC_out);

extern void CRTCTS_init(void);

extern RTC_time_16 *RTC8to16(const RTC_time_8 *RTC8);

extern RTC_time_32 *RTC16to32(const RTC_time_16 *RTC16);

extern RTC_time_32 *RTC8to32(const RTC_time_8 *RTC8);

// extern uint8_t RTC32to16(const RTC_time_32 *RTC32, RTC_time_16 *RTC16);

// extern uint8_t RTC16to8(const RTC_time_16 *RTC16, RTC_time_8 *RTC8);

// extern uint8_t RTC32to8(const RTC_time_32 *RTC32, RTC_time_8 *RTC8);

extern uint8_t RTC_init(RTC_time_8 *RTC8);

extern uint8_t RTC_print(const RTC_time_8 *RTC8);