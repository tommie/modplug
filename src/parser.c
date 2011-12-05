/*
 * Copyright © 2011 Tommie Gannert
 *
 * Module handling metadata parsing of the MOD files through libmodplug.
 */
#include <limits.h>
#include "common.h"


static void* create(struct sppb_plugin_description *plugin, struct sppb_byte_input *input, int song_index)
{
	MPSP_DPRINTF("parser: create(%p, %d)\n", input, song_index);

	// We only support a single song.
	if (song_index) return NULL;

	return load_mod_plug(input);
}

static void destroy(struct sppb_plugin_description *plugin, void *context)
{
	MPSP_DPRINTF("parser: destroy(%p)\n", context);

	ModPlug_Unload(self);
}

static unsigned int get_song_count(struct sppb_plugin_description *plugin, void *context)
{
	return 1;
}

static enum sppb_channel_format get_channel_format(struct sppb_plugin_description *plugin, void *context)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	MPSP_DPRINTF("parser: get_channel_format(): %d\n", settings.mChannels);

	switch (settings.mChannels) {
	case 1: return SPPB_CHANNEL_FORMAT_MONO;
	case 2: return SPPB_CHANNEL_FORMAT_STEREO;
	default: return SPPB_CHANNEL_FORMAT_INVALID;
	}
}

static unsigned int get_sample_rate(struct sppb_plugin_description *plugin, void *context)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	MPSP_DPRINTF("parser: get_sample_rate(): %d\n", settings.mFrequency);

	return settings.mFrequency;
}

static unsigned int get_length_in_samples(struct sppb_plugin_description *plugin, void *context)
{
	MPSP_DPRINTF("parser: get_length_in_samples(): %u\n", (ModPlug_GetLength(self) + 500) / 1000 * getSamplingRate());

	return (ModPlug_GetLength(self) + 500) / 1000 * get_sampling_rate();
}

static spbool has_field(struct sppb_plugin_description *plugin, void *context, enum sppb_field_type type)
{
	MPSP_DPRINTF("parser: has_field(%d)\n", type);

	switch (type) {
	case SPPB_FIELD_TYPE_TITLE: return sptrue;
	default: return spfalse;
	}
}

static spbool read_field(struct sppb_plugin_description *plugin, void *context, enum sppb_field_type type, char *dest, size_t *length)
{
	MPSP_DPRINTF("parser: read_field(%d, %p, %zu)\n", type, dest, *length);

	switch (type) {
	case SPPB_FIELD_TYPE_TITLE:
		return copy_string(ModPlug_GetName(self), dest, length);

	default:
		return spfalse;
	}
}


/**
 * The playback plugin description, as required by the plugin API.
**/
const struct sppb_parser_plugin MODPLUG_PARSER_PLUGIN = {
	.create = create,
	.destroy = destroy,
	.get_song_count = get_song_count,
	.get_channel_format = get_channel_format,
	.get_sample_rate = get_sample_rate,
	.get_length_in_samples = get_length_in_samples,
	.has_field = has_field,
	.read_field = read_field,
	.write_field = NULL,
};
