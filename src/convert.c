/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "convert.h"

#include <iconv.h>

/*
 * Convert the given string from_encoding -> to_encoding using iconv and
 *  return the converted string.
 */
gchar *gtranslator_convert_string(const gchar *string,
	const gchar *from_encoding, const gchar *to_encoding)
{
	iconv_t	iconv_base;
	size_t	input_length, output_length;
	gchar	*input_string, *input_string_pointer;
	gchar	*output_string, *output_string_pointer;
	
	g_return_val_if_fail(string!=NULL, NULL);
	g_return_val_if_fail(to_encoding!=NULL, NULL);

	/*
	 * Assume the from_encoding to be "iso-8859-1" per default.
	 */
	if(!from_encoding)
	{
		from_encoding="iso-8859-1";
	}
	else if(!g_strcasecmp(to_encoding, from_encoding))
	{
		/*
		 * In the case of the same enodings as to/from targets, return
		 *  the given string argument.
		 */
		return g_strdup(string);
	}

	iconv_base=iconv_open(to_encoding, from_encoding);

	/*
	 * Fallback to the pure string if we cannot perform any conversion.
	 */
	g_return_val_if_fail(iconv_base != ((iconv_t) -1), g_strdup(string));

	input_string=input_string_pointer=g_strdup(string);
	input_length=(strlen(input_string) + 1);
	
	output_length=(input_length << 1);
	output_string=output_string_pointer=g_malloc(output_length);

	#if defined __GLIBC__ && __GLIBC_MINOR__ <= 1
	iconv(iconv_base, (const gchar **) &input_string, &input_length,
		&output_string, &output_length);
	#else
	iconv(iconv_base, &input_string, &input_length, 
		&output_string, &output_length);
	#endif

	g_free(input_string_pointer);
	
	iconv_close(iconv_base);

	return output_string_pointer;
}

/*
 * Typo-saver functions for doing the most-casual conversions.
 */
gchar *gtranslator_convert_string_to_utf8(const gchar *string,
	const gchar *from_encoding)
{
	return (gtranslator_convert_string(string, from_encoding, "UTF-8"));
}

gchar *gtranslator_convert_string_from_utf8(const gchar *string,
	const gchar *to_encoding)
{
	return (gtranslator_convert_string(string, "UTF-8", to_encoding));
}
