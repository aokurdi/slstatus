/* See LICENSE file for copyright and license details. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../util.h"

const char *
cpu_freq(void)
{
    uintmax_t freq;

    /* in kHz */
    if (pscanf("/sys/devices/system/cpu/cpu0/cpufreq/"
                "scaling_cur_freq", "%ju", &freq) != 1) {
        return NULL;
    }

    return fmt_human(freq * 1000, 1000);
}

const char *
cpu_perc(void)
{
    static long double a[7];
    long double b[7], sum;

    memcpy(b, a, sizeof(b));
    /* cpu user nice system idle iowait irq softirq */
    if (pscanf("/proc/stat", "%*s %Lf %Lf %Lf %Lf %Lf %Lf %Lf",
                &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6])
        != 7) {
        return NULL;
    }
    if (b[0] == 0) {
        return NULL;
    }

    sum = (b[0] + b[1] + b[2] + b[3] + b[4] + b[5] + b[6]) -
            (a[0] + a[1] + a[2] + a[3] + a[4] + a[5] + a[6]);

    if (sum == 0) {
        return NULL;
    }

    return bprintf("%d", (int)(100 *
                    ((b[0] + b[1] + b[2] + b[5] + b[6]) -
                    (a[0] + a[1] + a[2] + a[5] + a[6])) / sum));
}
