/* gnome-regex.c - Implementation of regex cache object.

   Copyright (C) 1998 Tom Tromey

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <config.h>

#include <string.h>
#include <glib.h>

#include "gnome-regex.h"

#define DEFAULT_SIZE 96

/**
 * gnome_regex_cache_new:
 * 
 * Creates a new regular expression cache object with default size of items
 * 
 * Return value: the new cache object.
 **/
GnomeRegexCache *
gnome_regex_cache_new (void)
{
	return gnome_regex_cache_new_with_size (DEFAULT_SIZE);
}

/**
 * gnome_regex_cache_new:
 * @size: the number of cache items
 * 
 * Creates a new regular expression cache object.
 * 
 * Return value: the new cache object.
 **/
GnomeRegexCache *
gnome_regex_cache_new_with_size (int size)
{
	GnomeRegexCache *rxc = g_new (GnomeRegexCache, 1);
	rxc->size = size;
	rxc->next = 0;
	rxc->regexs = g_new0 (char *, rxc->size);
	rxc->patterns = g_new (regex_t, rxc->size);
	rxc->flags = g_new0 (int, rxc->size);
	return rxc;
}

static void
free_element (GnomeRegexCache *rxc, int elt)
{
	if (rxc->regexs[elt]) {
		g_free (rxc->regexs[elt]);

		/* We only want to try to free a pattern if we know it
		   has been allocated.  Hence this is inside the
		   `if'. */
		regfree (&rxc->patterns[elt]);
	}
}

/**
 * gnome_regex_cache_destroy:
 * @rxc: A regular expression cache object
 * 
 * Destroys a regular expression cache object.
 **/
void
gnome_regex_cache_destroy (GnomeRegexCache *rxc)
{
	int i;

	for (i = 0; i < rxc->size; ++i) {
		free_element (rxc, i);
	}

	g_free (rxc->regexs);
	g_free (rxc->patterns);
	g_free (rxc->flags);
	g_free (rxc);
}

/**
 * gnome_regex_cache_set_size:
 * @rxc: A regular expression cache object
 * @new_size: new size of cache
 * 
 * Sets the maxiumum number of regular expressions the cache can
 * hold.  If this is less than the number of currently cached
 * expressions, then the oldest expressions are deleted.
 **/
void
gnome_regex_cache_set_size (GnomeRegexCache *rxc, int new_size)
{
	if (new_size == rxc->size)
		return;

	if (new_size < rxc->size) {
		int i;
		/* FIXME This deletes not the oldest elements in cache,
		 * but truncates the end of it instead  */
		for (i = new_size; i < rxc->size; i++) {
			free_element (rxc, i);
		}
	}
	rxc->regexs = (char **) g_realloc (rxc->regexs,
					   new_size * sizeof (char *));
	rxc->patterns = (regex_t *) g_realloc (rxc->patterns,
					       new_size * sizeof (regex_t));
	if (new_size > rxc->size) {
		memset (&rxc->regexs[rxc->size + 1], 0,
			(new_size - rxc->size) * sizeof (char *));
	}
	rxc->size = new_size;
	if (rxc->next >= new_size) {
		rxc->next = 0;
	}
}

/**
 * gnome_regex_cache_compile:
 * @rxc: A regular expression cache object
 * @pattern: A string representing a regular expression
 * @flags: Flags to pass to regcomp()
 * 
 * This compiles a regular expression.  If the expression is cached,
 * the previously computed value is returned.  Otherwise, the
 * expression is compiled, cached, and then returned.
 * 
 * Return value: a compiled regular expression, or %NULL on error.
 **/
regex_t *
gnome_regex_cache_compile (GnomeRegexCache *rxc, const char *pattern,
			   int flags)
{
	int i;
	regex_t rx;

	for (i = 0; i < rxc->size; i++) {
		if (! rxc->regexs[i])
			break;
		if ((rxc->flags[i] == flags)
		    && (! strcmp (rxc->regexs[i], pattern))) {
			return &rxc->patterns[i];
		}
	}

	free_element (rxc, rxc->next);

	/* FIXME: use GNU regex call here?  */
	if (regcomp (&rx, pattern, flags)) {
		/* Failure.  */
		return NULL;
	}

	rxc->regexs[rxc->next] = g_strdup (pattern);
	memcpy (&rxc->patterns[rxc->next], &rx, sizeof (regex_t));
	rxc->flags[rxc->next] = flags;

	i = rxc->next;
	if (++rxc->next >= rxc->size)
		rxc->next = 0;

	return &rxc->patterns[i];
}

