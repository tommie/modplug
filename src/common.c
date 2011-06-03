/*
 * Copyright © 2011 Tommie Gannert
 *
 * Common routines for the ModPlug Spotify plugin.
 */
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "common.h"


ModPlugFile* loadModPlugFile(const char *path)
{
	ssize_t size = getFileSize(path);
	ModPlugFile *self_ = NULL;

	if (size < 0) return NULL;

	void *data = malloc(size);

	if (!data) return NULL;

	if (readFile(path, data, size) != size) goto exit1;

	self_ = ModPlug_Load(data, size);

	if (self_) {
		// The default volume of 127 is lower than the GMES
		// volume, so set to maximum.
		ModPlug_SetMasterVolume(self_, 512);
	} else {
		MPSP_EPRINTF("failed to load file \"%s\"\n", path);
	}

exit1:
	free(data);

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
