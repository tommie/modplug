/*
 * Copyright © 2011 Tommie Gannert
 *
 * Module handling playback of the MOD files through libmodplug.
 */
#include <limits.h>
#include "common.h"


static void* create(struct SpotifyLFPluginDescription *plugin, const char *path, int song_index)
{
	MPSP_DPRINTF("playback: create(%s, %d)\n", path, song_index);

	// We only support a single song.
	if (song_index) return NULL;

	return loadModPlugFile(path);
}

static void destroy(struct SpotifyLFPluginDescription *plugin, void *context)
{
	MPSP_DPRINTF("playback: destroy(%p)\n", context);

	ModPlug_Unload(self);
}

static spbool decode(struct SpotifyLFPluginDescription *plugin, void *context, spbyte *dest, size_t *destlen, spbool *final)
{
	int n = ModPlug_Read(self, dest, *destlen);

	MPSP_DPRINTF("playback: decode(%p, %zu): %d\n", dest, *destlen, n);

	*destlen = (size_t) n;
	if (!n) *final = sptrue;

	return sptrue;
}

static spbool seek(struct SpotifyLFPluginDescription *plugin, void *context, unsigned int sample)
{
	MPSP_DPRINTF("playback: seek(%u)\n", sample);

	ModPlug_Seek(self, (sample / getSamplingRate()) * 1000);

	return sptrue;
}

static size_t getMinimumOutputBufferSize(struct SpotifyLFPluginDescription *plugin, void *context)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	// This is just a ballpark figure, one tenth of a second.
	return settings.mBits / 8 * settings.mChannels * settings.mFrequency / 10;
}

static unsigned int getLengthInSamples(struct SpotifyLFPluginDescription *plugin, void *context)
{
	return (ModPlug_GetLength(self) + 500) / 1000 * getSamplingRate();
}

static void getAudioFormat(struct SpotifyLFPluginDescription *plugin, void *context, unsigned int *samplerate, enum SpotifyLFSoundFormat *format, enum SPChannelFormat *channels)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	*samplerate = settings.mFrequency;

	switch (settings.mBits) {
	case  8: *format = kSoundFormat8BitsPerSample; break;
	case 16: *format = kSoundFormat16BitsPerSample; break;
	default: *format = INT_MAX; break; // TODO(tommie): Report error.
	}

	switch (settings.mChannels) {
	case 1: *channels = kSPMono; break;
	case 2: *channels = kSPStereo; break;
	default: *channels = INT_MAX; break; // TODO(tommie): Report error.
	}
}


/**
 * The playback plugin description, as required by the plugin API.
**/
const struct SpotifyLFPlaybackPlugin MODPLUG_PLAYBACK_PLUGIN = {
	.create = create,
	.destroy = destroy,
	.decode = decode,
	.seek = seek,
	.getMinimumOutputBufferSize = getMinimumOutputBufferSize,
	.getLengthInSamples = getLengthInSamples,
	.getAudioFormat = getAudioFormat,
};
