/*
 * Copyright © 2011 Tommie Gannert
 *
 * Module handling metadata parsing of the MOD files through libmodplug.
 */
#include <limits.h>
#include "common.h"


static void* create(struct SpotifyLFPluginDescription *plugin, const char *path, int song_index)
{
	MPSP_DPRINTF("parser: create(%s, %d)\n", path, song_index);

	// We only support a single song.
	if (song_index) return NULL;

	return loadModPlugFile(path);
}

static void destroy(struct SpotifyLFPluginDescription *plugin, void *context)
{
	MPSP_DPRINTF("parser: destroy(%p)\n", context);

	ModPlug_Unload(self);
}

static unsigned int getSongCount(struct SpotifyLFPluginDescription *plugin, void *context)
{
	return 1;
}

static enum SPChannelFormat getChannelFormat(struct SpotifyLFPluginDescription *plugin, void *context)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	MPSP_DPRINTF("parser: getChannelFormat(): %d\n", settings.mChannels);

	switch (settings.mChannels) {
	case 1: return kSPMono;
	case 2: return kSPStereo;
	default: return INT_MAX; // TODO(tommie): Report error.
	}
}

static unsigned int getSampleRate(struct SpotifyLFPluginDescription *plugin, void *context)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	MPSP_DPRINTF("parser: getSampleRate(): %d\n", settings.mFrequency);

	return settings.mFrequency;
}

static unsigned int getLengthInSamples(struct SpotifyLFPluginDescription *plugin, void *context)
{
	MPSP_DPRINTF("parser: getLengthInSamples(): %u\n", (ModPlug_GetLength(self) + 500) / 1000 * getSamplingRate());

	return (ModPlug_GetLength(self) + 500) / 1000 * getSamplingRate();
}

static spbool hasField(struct SpotifyLFPluginDescription *plugin, void *context, enum SPFieldType type)
{
	MPSP_DPRINTF("parser: hasField(%d)\n", type);

	switch (type) {
	case kSPFieldTypeTitle: return sptrue;
	default: return spfalse;
	}
}

static spbool readField(struct SpotifyLFPluginDescription *plugin, void *context, enum SPFieldType type, char *dest, size_t *length)
{
	MPSP_DPRINTF("parser: readField(%d, %p, %zu)\n", type, dest, *length);

	switch (type) {
	case kSPFieldTypeTitle:
		return copyString(ModPlug_GetName(self), dest, length);

	default:
		return spfalse;
	}
}


/**
 * The playback plugin description, as required by the plugin API.
**/
const struct SpotifyLFParserPlugin MODPLUG_PARSER_PLUGIN = {
	.create = create,
	.destroy = destroy,
	.getSongCount = getSongCount,
	.getChannelFormat = getChannelFormat,
	.getSampleRate = getSampleRate,
	.getLengthInSamples = getLengthInSamples,
	.hasField = hasField,
	.readField = readField,
	.writeField = NULL,
};
