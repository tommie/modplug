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


static ModPlugFile* loadRawFile(const char *path)
{
	ssize_t size = getFileSize(path);
	ModPlugFile *self_ = NULL;

	if (size < 0) return NULL;

	void *data = malloc(size);

	if (!data) return NULL;

	if (readFile(path, data, size) != size)
		goto exit1;

	self_ = ModPlug_Load(data, size);

exit1:
	free(data);

	return self_;
}

#if MPSP_HAVE_LIBZIP
static ModPlugFile* loadZipFile(const char *path)
{
	int err = 0;
	struct zip *zf = zip_open(path, 0, &err);

	if (!zf) return NULL;

	struct zip_stat sb;

	// FIXME: Assumes the first file is the mod-file
	if (zip_stat_index(zf, 0, 0, &sb)) {
		MPSP_EPRINTF("failed to stat ZIP member: %s\n", zip_strerror(zf));
		goto err1;
	}

	// FIXME: Assumes the first file is the mod-file
	struct zip_file *file = zip_fopen_index(zf, 0, 0);

	if (!file) {
		MPSP_EPRINTF("failed to open ZIP member: %s\n", zip_strerror(zf));
		goto err1;
	}

	void *data = malloc(sb.size);

	if (!data) {
		MPSP_EPRINTF("failed to allocate memory: %s\n", strerror(errno));
		goto err2;
	}

	if (zip_fread(file, data, sb.size) != sb.size) {
		MPSP_EPRINTF("failed to read ZIP member: %s\n", zip_file_strerror(file));
		goto err3;
	}

	(void) zip_fclose(file);

	ModPlugFile *self_ = ModPlug_Load(data, sb.size);

	free(data);
	(void) zip_close(zf);

	return self_;

err3:
	free(data);

err2:
	(void) zip_fclose(file);

err1:
	(void) zip_close(zf);

	return NULL;
}
#endif

ModPlugFile* loadModPlugFile(const char *path)
{
	ModPlugFile *self_;
#if MPSP_HAVE_ZIP
	size_t plen = strlen(path);
#endif

#define MATCH(ext) (!strcasecmp(path + plen - (sizeof(ext) - 1), ext))
#if MPSP_HAVE_ZIP
	if (MATCH(".mdz") || MATCH(".s3z") || MATCH(".xmz") || MATCH(".itz"))
		self_ = loadZipFile(path);
	else
#endif
		self_ = loadRawFile(path);
#undef MATCH

	if (self_) {
		// The default volume of 127 is lower than the GMES
		// volume, so set to maximum.
		ModPlug_SetMasterVolume(self_, 512);
	} else {
		MPSP_EPRINTF("failed to load file \"%s\"\n", path);
	}

	return self_;
}

ssize_t getFileSize(const char *path)
{
	struct stat sb;

	if (stat(path, &sb)) return -1;

	return sb.st_size;
}

ssize_t readFile(const char *path, void *buffer, size_t size)
{
	int fd = open(path, O_RDONLY);

	if (fd < 0) return -1;

	ssize_t ret = read(fd, buffer, size);

	close(fd);

	return ret;
}

int getSamplingRate(void)
{
	ModPlug_Settings settings;

	ModPlug_GetSettings(&settings);

	return settings.mFrequency;
}

spbool copyString(const char *src, char *dest, size_t *length)
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
