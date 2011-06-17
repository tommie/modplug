/*
 * Copyright © 2011 Tommie Gannert
 *
 * Common routines for the ModPlug Spotify plugin.
 */
#ifndef __MODPLUG_SPOTIFY_COMMON_H__
#define __MODPLUG_SPOTIFY_COMMON_H__

#include <stdio.h>
#include <unistd.h>
#include <libmodplug/modplug.h>
#include "local_file_plugin_api.h"


// --- Macros ---
#if MPSP_ENABLE_DEBUG
	/**
	 * Print some debug information.
	**/
#	define MPSP_DPRINTF(...) fprintf(stderr, "MODPLUG: " __VA_ARGS__)
#else
#	define MPSP_DPRINTF(...) do {} while (0)
#endif

/**
 * Report an error.
**/
#define MPSP_EPRINTF(...) fprintf(stderr, "MODPLUG: " __VA_ARGS__)

/**
 * Simple casting of the context argument present in most functions
 * below to the correct type for us.
**/
#define self ((ModPlugFile*) (context))


// --- Functions ---
/**
 * Load a MOD from the named file.
 *
 * @param path a valid file name.
 * @return NULL on error, a valid pointer on success.
**/
extern ModPlugFile* loadModPlugFile(const char *path);

/**
 * Return the sampling rate as reported by libmodplug.
 *
 * @return the sampling rate, in samples per second.
**/
extern int getSamplingRate(void);

/**
 * Copy the string from src to dest, updating length aftwards.
 *
 * After the call, \c length is the number of bytes occupied by
 * the string, not including the terminating zero.
 *
 * If \c dest is NULL, only the \c length value is updated.
 *
 * @param src the string to copy from, may be NULL.
 * @param dest the buffer to copy to, may be NULL.
 * @param length a pointer to the initial length of the buffer.
 * @return zero if \c src was NULL, non-zero otherwise.
**/
spbool copyString(const char *src, char *dest, size_t *length);

#endif /* __MODPLUG_SPOTIFY_COMMON_H__ */
