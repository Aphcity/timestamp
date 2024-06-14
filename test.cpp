#include "stdafx.h"
#include <stdio.h>
#include "timestamp.h"

int main()
{
    int unixtimestamp = 0;

    scanf("%d", &unixtimestamp);

    time_struct tt_in;

    tt_in = stamptotime(unixtimestamp);
    printf("%02d%02d-%02d-%02d %02d:%02d:%02d\n",
           tt_in.h_year, tt_in.l_year, tt_in.mon, tt_in.day, tt_in.hour, tt_in.min, tt_in.sec);

    time_struct t_out = {20, 24, 6, 14, 12, 0, 0}; // Jun 14 2024 120000
    uint32_t stamp = timetostamp(t_out);
    printf("Unix timestamp: %u\n", stamp);

    return 0;
}
