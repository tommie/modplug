/*
 * Copyright © 2011 Tommie Gannert
 *
 * Module handling playback of the MOD files through libmodplug.
 */
#include <limits.h>
#include "common.h"


static void* create(struct sppb_plugin_description *plugin, struct sppb_byte_input *input, int song_index)
{
	MPSP_DPRINTF("playback: create(%s, %d)\n", path, song_index);

	// We only support a single song.
	if (song_index) return NULL;

	return load_mod_plug(input);
}

static void destroy(struct sppb_plugin_description *plugin, void *context)
{
	MPSP_DPRINTF("playback: destroy(%p)\n", context);

	ModPlug_Unload(self);
}

static spbool decode(struct sppb_plugin_description *plugin, void *context, spbyte *dest, size_t *destlen, spbool *final)
{
	int n = ModPlug_Read(self, dest, *destlen);

	MPSP_DPRINTF("playback: decode(%p, %zu): %d\n", dest, *destlen, n);

	*destlen = (size_t) n;
	if (!n) *final = sptrue;

	return sptrue;
}

static spbool seek(struct sppb_plugin_description *plugin, void *context, unsigned int sample)
{
	MPSP_DPRINTF("playback: seek(%u)\n", sample);

	ModPlug_Seek(self, (sample / get_sampling_rate()) * 1000);

	return sptrue;
}

static size_t get_minimum_output_buffer_size(struct sppb_plugin_description *plugin, void *context)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	// This is just a ballpark figure, one tenth of a second.
	return settings.mBits / 8 * settings.mChannels * settings.mFrequency / 10;
}

static unsigned int get_length_in_samples(struct sppb_plugin_description *plugin, void *context)
{
	return (ModPlug_GetLength(self) + 500) / 1000 * get_sampling_rate();
}

static void get_audio_format(struct sppb_plugin_description *plugin, void *context, unsigned int *samplerate, enum sppb_sound_format *format, enum sppb_channel_format *channels)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	*samplerate = settings.mFrequency;

	switch (settings.mBits) {
	case  8: *format = SPPB_SOUND_FORMAT_8BITS_PER_SAMPLE; break;
	case 16: *format = SPPB_SOUND_FORMAT_16BITS_PER_SAMPLE; break;
	default: *format = INT_MAX; break; // TODO(tommie): Report error.
	}

	switch (settings.mChannels) {
	case 1: *channels = SPPB_CHANNEL_FORMAT_MONO; break;
	case 2: *channels = SPPB_CHANNEL_FORMAT_STEREO; break;
	default: *channels = SPPB_CHANNEL_FORMAT_INVALID; break;
	}
}


/**
 * The playback plugin description, as required by the plugin API.
**/
const struct sppb_playback_plugin MODPLUG_PLAYBACK_PLUGIN = {
	.create = create,
	.destroy = destroy,
	.decode = decode,
	.seek = seek,
	.get_minimum_output_buffer_size = get_minimum_output_buffer_size,
	.get_length_in_samples = get_length_in_samples,
	.get_audio_format = get_audio_format,
};
