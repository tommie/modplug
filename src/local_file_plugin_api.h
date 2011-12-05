#ifndef _SPOTIFY_LOCAL_FILE_PLUGIN_API__H
#define _SPOTIFY_LOCAL_FILE_PLUGIN_API__H
#ifdef __cplusplus
extern "C" {
#endif



/**
	Spotify Local File Plugin API
	=============================
	This file describes the API Spotify exposes for adding support for parsing
	and playing music file formats in Spotify.

	Building
	--------
	1. Implement this API, and export the symbol SpotifyLocalFilePlaybackPluginCreate
	   (see bottom of document).
	2. Create a folder called [your plugin name].splugin
	3. Compile your source into a dynamic library for your platform.
	4. Put the compiled library into
	   [name].splugin/Contents/[MacOS|Win32|{some platform}]/{name}
	   (append .dll if the platform is Windows)
	5. Repeat 3 and 4 for each platform you wish to support.

	(You can also just rename your dynamic library to
	 [your plugin name].splugin if you only wish to support a single platform)

	Installing
	----------
	MacOS: Install the .splugin into
		   ~/Library/Application Support/Spotify/Plug-Ins/LocalFile/
		   (or any other domain)
	Windows: [TBD]
	Linux: [TBD]
*/

#include <string.h>

typedef unsigned char spbool;
enum { spfalse, sptrue };
typedef unsigned char spbyte;
typedef long sppb_offset;
typedef long sppb_ssize;

#define SPPB_API_VERSION 7

/// Spotify supports the following output sound formats.
enum sppb_sound_format {
	SPPB_SOUND_FORMAT_8BITS_PER_SAMPLE = 8,   // 8 bit per sample Native Endian PCM
	SPPB_SOUND_FORMAT_16BITS_PER_SAMPLE = 16, // 16 bit per sample Native Endian PCM
	SPPB_SOUND_FORMAT_IEEE_FLOAT = 17,        // 32 bit per sample IEEE FLOAT PCM
};

/// The different types of metadata that a parser can be asked about.
enum sppb_field_type {
	SPPB_FIELD_TYPE_INVALID,

	SPPB_FIELD_TYPE_TITLE,
	SPPB_FIELD_TYPE_ARTIST,
	SPPB_FIELD_TYPE_ALBUM,
	SPPB_FIELD_TYPE_ALBUM_ARTIST,
	SPPB_FIELD_TYPE_COMMENT,
	SPPB_FIELD_TYPE_COMPOSER,
	SPPB_FIELD_TYPE_CONTENT,
	SPPB_FIELD_TYPE_PUBLISHER,
	SPPB_FIELD_TYPE_COPYRIGHT,
	SPPB_FIELD_TYPE_URL,
	SPPB_FIELD_TYPE_ENCODED,
	SPPB_FIELD_TYPE_TRACK,
	SPPB_FIELD_TYPE_DISC,
	SPPB_FIELD_TYPE_YEAR,
	SPPB_FIELD_TYPE_ORIG_ARTIST,
	// Future versions may ask for more metadata, be prepared
	// to fail gracefully in such a case.
};

enum sppb_channel_format {
	SPPB_CHANNEL_FORMAT_INVALID,

	SPPB_CHANNEL_FORMAT_MONO,
	SPPB_CHANNEL_FORMAT_STEREO,
};

/// An argument type for sppb_byte_input.seek()
enum sppb_whence {
	SPPB_START,
	SPPB_CURRENT,
	SPPB_END,
};

struct sppb_plugin_description;

/// An abstraction of some raw data container.
struct sppb_byte_input {
	/// Close the input and free all resources used.
	void (*destroy)(struct sppb_byte_input *input);

	/// Return the length, in bytes, of the input.
	/// This may be NULL if the transport protocol doesn't support retrieving the length.
	/// Returns -1 on error.
	sppb_offset (*get_length)(struct sppb_byte_input *input);

	/// Read some bytes from the input.
	/// Returns 0 iff the end-of-input was seen.
	/// Returns -1 on error.
	sppb_ssize (*read)(struct sppb_byte_input *input, void *buf, size_t size);

	/// Move to another location in the input.
	/// Returns the new position, from start of input.
	/// Returns -1 on error.
	sppb_offset (*seek)(struct sppb_byte_input *input, sppb_offset offset, enum sppb_whence whence);
};

/// Used to extract metadata from a file in the plugin's supported formats.
struct sppb_parser_plugin {

	/// Create a metadata parsing context for the given file and song.
	/// This context will be forwarded to all metadata methods below.
	/// The caller owns the input reference and will destroy the input *after* the parser
	/// has been destroyed.
	/// @returns your internal state on success, NULL on failure.
	void *(*create)(struct sppb_plugin_description*, struct sppb_byte_input *input, int song_index);

	/// Close the file and destroy the context previously created with `create`.
	void (*destroy)(struct sppb_plugin_description*, void *context);


	/// How many songs does this single file contain? Commonly only one,
	/// but game music files (SID etc) sometimes contain several.
	unsigned int (*get_song_count)(struct sppb_plugin_description*, void *context);


	/// How many sound channels does this song have?
	enum sppb_channel_format (*get_channel_format)(struct sppb_plugin_description*, void *context);

	/// What does the metadata say that this file is in?
	unsigned int (*get_sample_rate)(struct sppb_plugin_description*, void *context);

	/// How long does the metadata claim that this file is?
	unsigned int (*get_length_in_samples)(struct sppb_plugin_description*, void *context);


	// Field extraction methods. `Type` is the 'column' of data to fetch for this row.

	/// Does this song in this file have this column of data?
	spbool (*has_field)(struct sppb_plugin_description*, void *context, enum sppb_field_type type);

	/// Read a column of data from this song in this file as a string.
	/// `read_field` will be called first with a NULL `dest` to determine the
	/// length of the string.
	/// @returns whether the operation was successful.
	spbool (*read_field)(
		struct sppb_plugin_description*, void *context,
		enum sppb_field_type type,
		char *dest,
		size_t *length
	);

	/// OPTIONAL: Write a column of data to this song in this file as a string.
	/// You may leave `write_field` as NULL if you do not support editing metadata.
	/// @returns whether the operation was successful.
	spbool (*write_field)(
		struct sppb_plugin_description*, void *context,
		enum sppb_field_type type,
		const char *src,
		size_t write_length);
};

/// Used to decode a file in the plugin's supported formats into a buffer of samples.
struct sppb_playback_plugin {

	/// Create a playback context for the given file and song.
	/// This context will be forwarded to all playback methods below.
	/// The caller owns the input reference and will destroy the input *after* the player
	/// has been destroyed.
	/// @returns your internal state on success, NULL on failure.
	void *(*create)(struct sppb_plugin_description*, struct sppb_byte_input *input, int song_index);

	/// Close the file, cease decoding and destroy the context previously created with `create`.
	void (*destroy)(struct sppb_plugin_description*, void *context);


	/// Decode at most `*destlen` bytes of audio data into the buffer `dest`.
	/// @param dest The sample buffer to deposit your samples into.
	///             If your data is stereo, left and right samples should be interlaced.
	/// @param destlen Set `*destlen`to the number of bytes you have decoded when finished
	/// @param final Set `*final = sptrue` when you have decoded all bytes in the song
	/// @returns whether decoding was successful
	spbool (*decode)(
		struct sppb_plugin_description*, void *context,
		spbyte *dest,
		size_t *destlen,
		spbool *final
	);

	/// Seek to a specific sample offset into the song, and continue decoding
	/// from there instead.
	/// @returns whether seeking was successful
	spbool (*seek)(struct sppb_plugin_description*, void *context, unsigned sample);


	/// How many bytes must the output buffer sent to `decode` be
	/// to be able to decode a frame of samples?
	size_t (*get_minimum_output_buffer_size)(struct sppb_plugin_description*, void *context);

	/// Given that you now have a decoder for this song, how long can you
	/// determine that the song *actually* is? (can be different from what
	/// the parser thought it would be)
	unsigned int (*get_length_in_samples)(struct sppb_plugin_description*, void *context);

	/// Which audio format will you output in the `decode` method?
	void (*get_audio_format)(
		struct sppb_plugin_description*, void *context,
		unsigned int *samplerate,
		enum sppb_sound_format *format,
		enum sppb_channel_format *channels
	);
};



/// This struct describes your plugin as a whole; both its parser,
/// player and metadata about the plugin.
struct sppb_plugin_description {
	/// The API version that your plugin was built for.
	/// Always set this to SP_LF_PLUGIN_API_VERSION.
	unsigned int api_version;

	/// The user-visible name of your plugin.
	char *plugin_name;
	unsigned int plugin_version;

	/// NULL-terminated list of strings of file extensions (excluding
	/// dot) this plugin adds support for.
	const char **file_extensions;

	/// Any context you wish to save that is persistent with this plugin.
	void *plugin_context;

	struct sppb_playback_plugin playback;
	struct sppb_parser_plugin parser;
};


/// Convenience method to null and initialize the basics of your plugin.
static inline void sppb_plugin_description_initialize(
	struct sppb_plugin_description *plugin,
	const char *name,
	unsigned int version
)
{
	memset(plugin, 0, sizeof(*plugin));
	plugin->plugin_name = strdup(name);
	plugin->plugin_version = version;
	plugin->api_version = SPPB_API_VERSION;
}


/// Implement and export this symbol from your dynamic library. Malloc
/// (or otherwise allocate) a plugin description, initialize it and return it.
/// @returns NULL to indicate failure.
//extern struct sppb_plugin_description *CreateSpotifyPlaybackPlugin();


#if __cplusplus
}
#endif
#endif