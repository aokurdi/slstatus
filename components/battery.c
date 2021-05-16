/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>

#include "../util.h"

#include <limits.h>
#include <stdint.h>
#include <unistd.h>

static const char *
get_symbol(unsigned int perc)
{
    const char *bat_symbol[]  = { "", "", "", "", "", "🔋" };
    return bat_symbol[perc];

}

static const char *
pick(const char *bat, const char *f1, const char *f2, char *path,
        size_t length)
{
    if (esnprintf(path, length, f1, bat) > 0 &&
        access(path, R_OK) == 0) {
        return f1;
    }

    if (esnprintf(path, length, f2, bat) > 0 &&
        access(path, R_OK) == 0) {
        return f2;
    }

    return NULL;
}

const char *
battery_state(const char *bat)
{
    char path[PATH_MAX], state[12];

    if (esnprintf(path, sizeof(path),
                    "/sys/class/power_supply/%s/status", bat) < 0) {
        return NULL;
    }
    if (pscanf(path, "%12s", state) != 1) {
        return NULL;
    }

    return ((strcmp("Charging", state) == 0) ? "" : "");
}

const char *
battery_perc(const char *bat)
{
    int perc;
    char path[PATH_MAX];
    const char *symbol;

    if (esnprintf(path, sizeof(path),
                    "/sys/class/power_supply/%s/capacity", bat) < 0) {
        return NULL;
    }

    if (pscanf(path, "%d", &perc) != 1) {
        return NULL;
    }

    symbol = get_symbol(perc/20);
    return bprintf("%s%s %d", symbol, battery_state(bat), perc);
}

const char *
battery_remaining(const char *bat)
{
    uintmax_t charge_now, current_now, m, h;
    double timeleft;
    char path[PATH_MAX], state[12];

    if (esnprintf(path, sizeof(path),
                    "/sys/class/power_supply/%s/status", bat) < 0) {
        return NULL;
    }
    if (pscanf(path, "%12s", state) != 1) {
        return NULL;
    }

    if (!pick(bat, "/sys/class/power_supply/%s/charge_now",
                "/sys/class/power_supply/%s/energy_now", path,
                sizeof(path)) ||
        pscanf(path, "%ju", &charge_now) < 0) {
        return NULL;
    }

    if (!strcmp(state, "Discharging")) {
        if (!pick(bat, "/sys/class/power_supply/%s/current_now",
                    "/sys/class/power_supply/%s/power_now", path,
                    sizeof(path)) ||
            pscanf(path, "%ju", &current_now) < 0) {
            return NULL;
        }

        if (current_now == 0) {
            return NULL;
        }

        timeleft = (double)charge_now / (double)current_now;
        h = timeleft;
        m = (timeleft - (double)h) * 60;

        return bprintf("%juh %jum", h, m);
    }

    return "";
}
