/* See LICENSE file for copyright and license details. */
#include <stdio.h>

#include "../util.h"
#include <stdint.h>

const char *
ram_free(void)
{
    uintmax_t free;

    if (pscanf("/proc/meminfo",
                "MemTotal: %ju kB\n"
                "MemFree: %ju kB\n"
                "MemAvailable: %ju kB\n",
                &free, &free, &free) != 3) {
        return NULL;
    }

    return fmt_human(free * 1024, 1024);
}

const char *
ram_perc(void)
{
    uintmax_t total, free, buffers, cached;

    if (pscanf("/proc/meminfo",
                "MemTotal: %ju kB\n"
                "MemFree: %ju kB\n"
                "MemAvailable: %ju kB\n"
                "Buffers: %ju kB\n"
                "Cached: %ju kB\n",
                &total, &free, &buffers, &buffers, &cached) != 5) {
        return NULL;
    }

    if (total == 0) {
        return NULL;
    }

    return bprintf("%d", 100 * ((total - free) - (buffers + cached))
                            / total);
}

const char *
ram_total(void)
{
    uintmax_t total;

    if (pscanf("/proc/meminfo", "MemTotal: %ju kB\n", &total)
        != 1) {
        return NULL;
    }

    return fmt_human(total * 1024, 1024);
}

const char *
ram_used(void)
{
    uintmax_t total, free, buffers, cached;

    if (pscanf("/proc/meminfo",
                "MemTotal: %ju kB\n"
                "MemFree: %ju kB\n"
                "MemAvailable: %ju kB\n"
                "Buffers: %ju kB\n"
                "Cached: %ju kB\n",
                &total, &free, &buffers, &buffers, &cached) != 5) {
        return NULL;
    }

    return fmt_human((total - free - buffers - cached) * 1024,
                        1024);
}
