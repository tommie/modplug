/*
 * Copyright © 2011 Tommie Gannert
 *
 * Plugin main module.
 */
#include "common.h"


// --- External data ---
extern const struct SpotifyLFParserPlugin MODPLUG_PARSER_PLUGIN;
extern const struct SpotifyLFPlaybackPlugin MODPLUG_PLAYBACK_PLUGIN;


/*
 * libmodplug doesn't seem to have a list of file extensions.
 * In fact, it doesn't seem to know of files at all.
 *
 * I can't find any reference to the compressed files at all in libmodplug,
 * so I assume that code was never released.
 *
 * The "wav" loader only works with ADPCM, so ignore that.
 *
 * This list is from the libmodplug README, section 2 and CSoundFile::Create().
 */
static const char *KNOWN_FILE_EXTENSIONS[] = {
	// Basic formats
	"mod",
	"s3m",
	"xm",
	"it",
	"669",
	"amf",
	"ams",
	"dbm",
	"dmf",
	"dsm",
	"far",
	"mdl",
	"med",
	"mtm",
	"okt",
	"ptm",
	"stm",
	"ult",
	"umx",
	"mt2",
	"psm",
#if MPSP_HAVE_ZIP
	// ZIP versions
	"mdz",
	"s3z",
	"xmz",
	"itz",
#endif
#if MPSP_HAVE_RAR
	// RAR versions
	"mdr",
	"s3r",
	"xmr",
	"itr",
#endif
#if MPSP_HAVE_GZIP
	// GZIP versions
	"mdgz",
	"s3gz",
	"xmgz",
	"itgz",
#endif
#if MPSP_HAVE_TIMIDITY
	// Timidity patch files
	"pat",
	// MIDI files
	"mid",
	// ABC music notation
	"abc",
#endif
#if MPSP_ENABLE_WAV
	// From the CSoundFile::Create() function.
	"wav",
#endif
	NULL
};


static void setupModPlug(void)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION;
	settings.mBits = 16;
	settings.mResamplingMode = MODPLUG_RESAMPLE_SPLINE;
	settings.mLoopCount = 0;

	ModPlug_SetSettings(&settings);
}

/**
 * Entry point for the plugin.
 *
 * This function is called by Spotify while initializing the plugin.
**/
struct SpotifyLFPluginDescription* SpotifyLocalFilePlaybackPluginCreate()
{
	struct SpotifyLFPluginDescription *ret = calloc(1, sizeof(*ret));

	MPSP_DPRINTF("SpotifyLocalFilePlaybackPluginCreate\n");

	setupModPlug();

	ret->api_version = SP_LF_PLUGIN_API_VERSION;
	ret->plugin_name = "ModPlug";
	ret->plugin_version = 1;
	ret->file_extensions = KNOWN_FILE_EXTENSIONS;
	ret->plugin_context = NULL;
	ret->playback = MODPLUG_PLAYBACK_PLUGIN;
	ret->parser = MODPLUG_PARSER_PLUGIN;

	return ret;
}
