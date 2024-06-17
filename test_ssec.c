#include "stdafx.h"
#include <stdio.h>
#include "timestamp_ssec.h"

int main()
{
    uint64_t unixtimestamp = 0;

    scanf("%lld", &unixtimestamp);

    time_struct tt_in;

    tt_in = stamptotime(unixtimestamp);
    printf("%02d%02d-%02d-%02d %02d:%02d:%02d:%03d\n",
           tt_in.h_year, tt_in.l_year, tt_in.mon, tt_in.day, tt_in.hour, tt_in.min, tt_in.sec, tt_in.ssec);

    // time_struct t_out = {20, 24, 6, 14, 12, 0, 0, 64}; // 219950899264
    // time_struct t_out = {19, 70, 1, 1, 0, 0, 1, 64}; // Jun 14 2024 12:00:00:064
    // time_struct t_out = {19, 70, 12, 11, 0, 0, 1, 1}; // 3804364929
    // time_struct t_out = {19, 70, 2, 1, 0, 0, 0, 1}; // Jun 14 2024 12:00:00:064
    // time_struct t_out = {19, 70, 11, 1, 0, 0, 1, 64}; // Jun 14 2024 12:00:00:064
    // time_struct t_out = {19, 70, 1, 1, 0, 0, 1, 64}; // Jun 14 2024 12:00:00:064
    // time_struct t_out = {19, 70, 1, 1, 0, 0, 1, 64}; // Jun 14 2024 12:00:00:064
    // time_struct t_out = {19, 70, 1, 1, 0, 0, 1, 64}; // Jun 14 2024 12:00:00:064
    // time_struct t_out = {20, 00, 6, 14, 12, 0, 0, 64}; // Jun 14 2024 12:00:00:064
    time_struct t_out = {21, 00, 12, 14, 12, 0, 0, 64}; // 528956006464

    uint64_t stamp = timetostamp(t_out);
    printf("Unix timestamp: %llu\n", stamp);

    return 0;
}
