/*
 * Copyright © 2011 Tommie Gannert
 *
 * Common routines for the ModPlug Spotify plugin.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#if MPSP_HAVE_LIBZIP
#	include <zip.h>
#endif
#include "common.h"


#if MPSP_HAVE_LIBZIP
/**
 * Read the contents of the first file of the ZIP file and load
 * it into a ModPlugFile.
**/
static ModPlugFile* loadZipFile(const char *path)
{
	ModPlugFile *self_ = NULL;
	int err = 0;
	struct zip *zf = zip_open(path, 0, &err);

	if (!zf) return NULL;

	struct zip_stat sb;

	// FIXME: Assumes the first file is the mod-file
	if (zip_stat_index(zf, 0, 0, &sb)) {
		MPSP_EPRINTF("failed to stat ZIP member: %s\n", zip_strerror(zf));
		goto exit1;
	}

	// FIXME: Assumes the first file is the mod-file
	struct zip_file *file = zip_fopen_index(zf, 0, 0);

	if (!file) {
		MPSP_EPRINTF("failed to open ZIP member: %s\n", zip_strerror(zf));
		goto exit1;
	}

	void *data = malloc(sb.size);

	if (!data) {
		MPSP_EPRINTF("failed to allocate memory: %s\n", strerror(errno));
		goto exit2;
	}

	if (zip_fread(file, data, sb.size) != sb.size) {
		MPSP_EPRINTF("failed to read ZIP member: %s\n", zip_file_strerror(file));
		goto exit3;
	}

	self_ = ModPlug_Load(data, sb.size);

exit3:
	free(data);

exit2:
	(void) zip_fclose(file);

exit1:
	(void) zip_close(zf);

	return self_;
}
#endif

ModPlugFile* load_mod_plug(struct sppb_byte_input *input)
{
	ModPlugFile *self_;

	if (!input->get_length || !input->seek)
		return NULL;

	ssize_t len = input->get_length(input);

	if (len < 0) return NULL;

	void *data = malloc(len);

	if (!data) return NULL;

	if (input->read(input, data, len) != len) {
		MPSP_EPRINTF("failed to read from input\n");
		return NULL;
	}

	self_ = ModPlug_Load(data, len);

	if (self_) {
		// The default volume of 127 is lower than the GMES
		// volume, so set to maximum.
		ModPlug_SetMasterVolume(self_, 512);
	} else {
		MPSP_EPRINTF("failed to load file\n");
	}

	return self_;
}

int get_sampling_rate(void)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	return settings.mFrequency;
}

spbool copy_string(const char *src, char *dest, size_t *length)
{
	if (!src) {
		*length = 0;
		return spfalse;
	}

	size_t n = strlen(src);

	if (n < *length) *length = n;

	if (dest) {
		strncpy(dest, src, *length);
	}

	return sptrue;
}
