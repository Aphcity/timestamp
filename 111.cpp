#include "stdafx.h"
#include <stdio.h>
#include "timestamp.h"

int main(char* argv[], int argc)
{
    int unixtimestamp=0;

	scanf("%d",&unixtimestamp);

	time_struct tt_in;

    tt_in = stamptotime(unixtimestamp);
    printf("%02d%02d-%02d-%02d %02d:%02d:%02d\n",
        tt_in.h_year, tt_in.l_year, tt_in.mon, tt_in.day, tt_in.hour, tt_in.min, tt_in.sec);

    return 0;
}
