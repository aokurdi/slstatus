/* See LICENSE file for copyright and license details. */
#include <fcntl.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#include "../util.h"


    int
vol_perc(const char *card)
{
    long v;
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    snd_mixer_selem_id_t* sid;

    static const char* mix_name = "Master";
    static int mix_index = 0;

    snd_mixer_selem_id_alloca(&sid);

    //sets simple-mixer index and name
    snd_mixer_selem_id_set_index(sid, mix_index);
    snd_mixer_selem_id_set_name(sid, mix_name);

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);

    snd_mixer_load(handle);
    elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume(elem, 0, &v);
    snd_mixer_close(handle);

    long minv, maxv;
    snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv);
    /* make the value bound to 100 */
    v -= minv;
    maxv -= minv;
    minv = 0;
    v = 100 * v / maxv; // make the value bound from 0 to 100

    return v;
}
