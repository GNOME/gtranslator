/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *		Gediminas Paulauskas <menesis@gtranslator.org>
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

#include "dialogs.h"
#include "gui.h"
#include "languages.h"
#include "nautilus-string.h"
#include "prefs.h"
#include "runtime-config.h"
#include "translator.h"
#include "utils.h"

#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

#include <libgnome/gnome-i18n.h>
#include <libgnome/gnome-util.h>

#include <gal/util/e-util.h>

/*
 * The used ref' count for the language lists.
 */
static gint list_ref=0; 

/*
 * The language data lists -- declare and initialize them here locally.
 */
GList *languages_list=NULL;
GList *encodings_list=NULL;
GList *lcodes_list=NULL;
GList *group_emails_list=NULL;
GList *bits_list=NULL;

/*
 * Strip the filename to get a "raw" enough filename.
 */ 
gchar *gtranslator_utils_get_raw_file_name(gchar *filename)
{
	GString *o;
	gint 	count=0;

	g_return_val_if_fail(filename!=NULL, g_strdup(""));

	o=g_string_new("");
	filename=g_basename(filename);

	/*
	 * Strip all extensions after the _first_ point.
	 */ 
	while(filename[count] && filename[count]!='.')
	{
		o=g_string_append_c(o, filename[count]);
		count++;
	}

	if(o->len > 0)
	{
		return o->str;
	}
	else
	{
		return NULL;
	}

	g_string_free(o, FALSE);
}

/*
 * Strip all punctuation characters out.
 */
gchar *gtranslator_utils_strip_all_punctuation_chars(const gchar *str)
{
	gchar	*stripped_string;
	gint	 index=0;
	
	/*
	 * Strip out all common punctuation characters which are defined
	 *  here locally in this array.
	 */
	const gchar punctuation_characters[] = 
	{ 
		',', '.', ':', ';',
		'?', '!', '(', ')',
		'[', ']', '{', '}',
		'`', '\'', '^', '/',
		'\\', '<', '>', -1
	};

	g_return_val_if_fail(str!=NULL, NULL);
	
	/*
	 * First strip out all the '´' characters from the string.
	 */
	stripped_string=nautilus_str_strip_chr(str, '´');

	/*
	 * Now strip out all these special characters to get a more raw
	 *  string (could result in better results for matching).
	 */
	while(punctuation_characters[index]!=-1)
	{
		stripped_string=nautilus_str_strip_chr(stripped_string,
			punctuation_characters[index]);
		
		index++;
	}

	return stripped_string;
}

/*
 * Cruise through the "envpath" string and set the "value" if anything
 *  could be found.
 */
void gtranslator_utils_get_environment_value(const gchar *envpath, gchar **value)
{
	gint	i=0;
	gchar	*contents=NULL;
	gchar	**array=NULL;
	
	g_return_if_fail(envpath!=NULL);

	/*
	 * Now split the envpath string into it's original parts.
	 */
	array=g_strsplit(envpath, ":", 0);
	g_return_if_fail(array[0]!=NULL);

	/*
	 * Now cruise through the array and search for a value.
	 */
	while(array[i]!=NULL)
	{
		contents=g_getenv(array[i]);

		if(contents)
		{
			*value=g_strdup(contents);
				break;
		}
		
		i++;
	}

	g_strfreev(array);
}

/*
 * Get the non-localized name for the language, if available.
 */ 
const gchar *gtranslator_utils_get_english_language_name(const gchar *lang)
{
	if(lang)
	{
		gint c;
		
		for(c=0; languages[c].name!=NULL; c++)
		{
			if(!nautilus_strcmp(_(languages[c].name), lang))
			{
				return languages[c].name;
			}
		}

		/*
		 * Return the original language if no conversion could be made.
		 */
		return lang;
	}
	else
	{
		/*
		 * Return the language in the preferences it no language was
		 *  given/defined.
		 */
		return gtranslator_translator->language->name;
	}
}

/*
 * Return the corresponding language name (e.g. "Turkish") for the given locale code (e.g. "tr").
 */
gchar *gtranslator_utils_get_language_name_by_locale_code(const gchar *locale_code)
{
	gint 	i;
	
	g_return_val_if_fail(locale_code!=NULL, NULL);

	/*
	 * Cruise through the list and return the matching language entries' name.
	 */
	for(i=0; languages[i].locale!=NULL; i++)
	{
		/*
		 * First check for full equality, then for a 2 char-prefix equality.
		 */
		if(!nautilus_strcasecmp(languages[i].locale, locale_code))
		{
			return _(languages[i].name);
		}
		else if(!g_strncasecmp(languages[i].locale, locale_code, 2))
		{
			return _(languages[i].name);
		}
	}

	return NULL;
}

/*
 * Set the prefs values for the given language.
 */
void gtranslator_utils_set_language_values_by_language(const gchar *language)
{
	gint	i;
	
	g_return_if_fail(language!=NULL);

	for(i=0; languages[i].name!=NULL; i++)
	{
		/*
		 * Search for our language and if found write the corresponding
		 *  language values into the prefs and return.
		 */
		if(!nautilus_strcasecmp(languages[i].name, language) ||
			!nautilus_strcasecmp(_(languages[i].name), language))
		{
			gtranslator_config_init();
			gtranslator_config_set_string("language/name", languages[i].name);
			gtranslator_config_set_string("language/language_code", languages[i].locale);
			
			/*
			 * Unfortunately not all languages in our languages list have
			 *  got a group EMail address so that we need to be safety-first checking
			 *   for a group EMail address before writing it into the prefs.
			 */
			if(languages[i].group_email)
			{
				gtranslator_config_set_string("language/team_email", languages[i].group_email);
			}
			
			gtranslator_config_set_string("language/mime_type", languages[i].encoding);
			gtranslator_config_set_string("language/encoding", languages[i].bits);
			gtranslator_config_close();

			return;
		}
	}
}

/*
 * Remove the gtranslator-generated temp.-files.
 */
void gtranslator_utils_remove_temp_files()
{
	/*
	 * Check for any lungering 'round file rests of any temporary action
	 *  and remove these rests if any had been found.
	 */
	if(g_file_exists(gtranslator_runtime_config->temp_filename))
	{
		remove(gtranslator_runtime_config->temp_filename);
	}
}

/*
 * Create the ".gtranslator" directory in the users home directory if necessary.
 */
void gtranslator_utils_create_gtranslator_directory()
{
	gchar *dirname;
	gint   i=0;

	const	gchar	*subdirectories[] =
	{
		"colorschemes",
		"etstates",
		"files",
		"umtf",
		NULL
	};

	dirname=g_strdup_printf("%s/.gtranslator", g_get_home_dir());

	/*
	 * First create the ~/.gtranslator main node/directory if needed.
	 */
	if(!g_file_test(dirname, G_FILE_TEST_EXISTS))
	{
		if(e_mkdir_hier(dirname, 0755)==-1)
		{
			g_warning(_("Can't create directory `%s'!"), dirname);
			exit(1);
		}
	}

	/*
	 * Now create all the subdirectories under ~/.gtranslator.
	 */
	while(subdirectories[i]!=NULL)
	{
		GTR_FREE(dirname);
		dirname=g_strdup_printf("%s/.gtranslator/%s", g_get_home_dir(), subdirectories[i]);

		if(!g_file_test(dirname, G_FILE_TEST_EXISTS))
		{
			if(e_mkdir_hier(dirname, 0755)==-1)
			{
				g_warning(_("Can't create directory `%s'!"), dirname);
				exit(1);
			}
		}

		i++;
	}

	GTR_FREE(dirname);
}

/*
 * The GSourceFunc for the periodic autosaving.
 */
gboolean gtranslator_utils_autosave(gpointer foo_me_or_die)
{
	/*
	 * As long as no file is opened, don't perform any autosave.
	 */
	if(!file_opened || !po)
	{
		return FALSE;
	}
	else if(!po->file_changed)
	{
		/*
		 * As the file didn't change, we don't need to autosave it, but
		 *  the timeout function must still return TRUE for getting it's
		 *   periodic sense.
		 */
		 return TRUE;
	}
	else
	{
		/*
		 * Should we use a different suffix for the autosaved files?
		 *  (Should help differing the own-saved/autosaved po files.)
		 */
		if(GtrPreferences.autosave_with_suffix) 
		{
			gchar *autosave_filename;
		
			/*
			 * Take "autosave" as the fallback autosave suffix.
			 */
			if(GtrPreferences.autosave_suffix)
			{
				autosave_filename=g_strdup_printf("%s.%s",
					po->filename, 
					GtrPreferences.autosave_suffix);
			}
			else
			{
				autosave_filename=g_strdup_printf("%s.autosave",
					po->filename);
			}

			gtranslator_save_file(autosave_filename);
			GTR_FREE(autosave_filename);
		}
		else
		{
			gtranslator_save_file(po->filename);
		}
		
		return TRUE;
	}
}

/*
 * Set up and assign the test file names for the compile process.
 */
void gtranslator_utils_get_compile_file_names(gchar **test_file, 
	gchar **output_file, gchar **result_file)
{
	*test_file=g_strdup_printf("%s/.gtranslator/gtranslator-temp-compile-file",
		g_get_home_dir());

	*result_file=g_strdup_printf("%s/.gtranslator/gtranslator-compile-result-file",
		g_get_home_dir());

	*output_file=g_strdup_printf("%s/%s.gmo",
		g_get_current_dir(), po->header->prj_name);
}

/*
 * Clean up after the test compile run.
 */
void gtranslator_utils_remove_compile_files(gchar **test_file,
	gchar **output_file, gchar **result_file)
{
	if(*test_file)
	{
		remove(*test_file);
		GTR_FREE(*test_file);
	}

	if(*result_file)
	{
		remove(*result_file);
		GTR_FREE(*result_file);
	}
	
	if(*output_file)
	{
		/*
		 * Only cleanup if this is wished by the user.
		 */
		if(GtrPreferences.sweep_compile_file)
		{
			remove(*output_file);
		}
		
		GTR_FREE(*output_file);
	}
}

/*
 * Test the prefixes of the given file_uri for support.
 */
gboolean gtranslator_utils_uri_supported(const gchar *file_uri)
{
	if(nautilus_istr_has_prefix(file_uri, "http://") ||
		nautilus_istr_has_prefix(file_uri, "https://") ||
		nautilus_istr_has_prefix(file_uri, "ftp://") ||
		nautilus_istr_has_prefix(file_uri, "file:/") ||
		nautilus_istr_has_prefix(file_uri, "www.") ||
		nautilus_istr_has_prefix(file_uri, "ftp.") ||
		nautilus_istr_has_prefix(file_uri, "nfs:/"))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * Return a good matching language name from the environment variables
 *  concerning locales.
 */
gchar *gtranslator_utils_get_environment_locale()
{
	gchar		*localename=NULL;

	gtranslator_utils_get_environment_value(
		"GTRANSLATOR_LANGUAGE:LANGUAGE:LC_ALL:LC_MESSAGES:LANG",
		&localename);

	return localename;
}

/*
 * Subsequently filter out all extension containing filename from the directory.
 * 
 * Should be useful for many cases.
 */
GList *gtranslator_utils_file_names_from_directory(const gchar *directory,
	const gchar *extension, gboolean sort, gboolean strip_extension,
	gboolean with_full_path)
{
	GList		*files=NULL;
	DIR 		*dir;
	struct dirent 	*entry;

	/*
	 * Filter out all bad cases ,-)
	 */
	g_return_val_if_fail(directory!=NULL, NULL);
	g_return_val_if_fail(extension!=NULL, NULL);
	g_return_val_if_fail(strlen(directory) > strlen(extension), NULL);

	/*
	 * Operate on/in the given directory and search for out pattern.
	 */
	dir=opendir(directory);
	g_return_val_if_fail(dir!=NULL, NULL);

	while((entry=readdir(dir)) != NULL)
	{
		if(entry->d_name &&
			nautilus_strcmp(entry->d_name, ".") && 
			nautilus_strcmp(entry->d_name, "..") &&
			nautilus_istr_has_suffix(entry->d_name, extension))
		{
			gchar *file;
			
			if(strip_extension && !with_full_path)
			{
				file=gtranslator_utils_get_raw_file_name(
					entry->d_name);
			}
			else if(with_full_path)
			{
				file=g_strdup_printf("%s/%s", directory,
					entry->d_name);
			}
			else
			{
				file=entry->d_name;
			}

			files=g_list_prepend(files, g_strdup(file));

			GTR_FREE(file);
		}
	}

	files=g_list_reverse(files);

	/*
	 * If the according argument is given, then sort the filenames list.
	 */
	if(sort && files)
	{
		files=g_list_sort(files, (GCompareFunc) nautilus_strcmp);
	}

	closedir(dir);
	return files;
}

/*
 * Free the list and it's data safely -- NULL cases should be catched here.
 */
void gtranslator_utils_free_list(GList *list, gboolean free_contents)
{
	if(!list)
	{
		/*
		 * When the list is NULL, don't crash or act, simply
		 *  return.
		 */
		return;
	}
	else
	{
		/*
		 * Free the list data as long as possible and desired.
		 */
		if(free_contents)
		{
			while(list!=NULL)
			{
				GTR_FREE(list->data);
				GTR_ITER(list);
			}
		}

		g_list_free(list);
		list=NULL;
	}
}

/*
 * Some voodoo-alike functions have been missing in gtranslator, so here we
 *  go: calculates a similarity percentage based on really insane logic.
 */
gfloat gtranslator_utils_calculate_similarity(const gchar *a, const gchar *b)
{
	gfloat	similarity;
	gfloat	one_char_percentage;
	
	gint	i=0;

	similarity=one_char_percentage=0.0;

	/*
	 * Eh, check for these quite useless situations in the two gchars.
	 */
	if(!a || !b || !nautilus_strcmp(a, "") || !nautilus_strcmp(b, ""))
	{
		return 0.0;
	}

	/*
	 * Calculate the similarity value a single char is representing.
	 */
	one_char_percentage=(100 / (strlen(a) - 1));

	/*
	 * Now we do check the characters of the two given strings..
	 *  Voodoo, Magic!
	 */
	while(a[i] && b[i] && a[i]!='\0' && b[i]!='\0')
	{
		if(a[i]==b[i])
		{
			similarity+=one_char_percentage;
		}
		else if(tolower(a[i])==tolower(b[i]))
		{
			similarity+=(one_char_percentage / 2);
		}
		
		i++;
	}

	return similarity;
}

/*
 * Check for matching of an entry of the list entry and the string -- returns
 *  '-1' on non-matching, else the position in the list.
 */
gint gtranslator_utils_stringlist_strcasecmp(GList *list, const gchar *string)
{
	gint pos=0;
	
	g_return_val_if_fail(list!=NULL, -1);
	g_return_val_if_fail(string!=NULL, -1);

	/*
	 * The list should only consist out of gchar's..
	 */
	g_return_val_if_fail(sizeof(gchar *) == sizeof(list->data), -1);

	while(list!=NULL)
	{
		if(!nautilus_strcasecmp(list->data, string))
		{
			return pos;
		}

		GTR_ITER(list);
		pos++;

		if(!list)
		{
			return -1;
		}
	}

	return -1;
}

/*
 * Get the right localename for the language.
 */
gchar *gtranslator_utils_get_locale_name(void)
{
	if((!file_opened) || !(po->header->language))
	{
		return NULL;
	}
	else
	{
		gint c;

		for(c=0; languages[c].name!=NULL; c++)
		{
			/*
			 * Check for the language in the header -- may it be in 
			 *  English or in the current locale..
			 */
			if(!nautilus_strcasecmp(languages[c].name, 
					po->header->language) ||
			   !nautilus_strcasecmp(_(languages[c].name),
					po->header->language))
			{
				return languages[c].locale;
			}
		}

		return NULL;
	}
}

/*
 * Get the right charset/encoding for the language.
 */
gchar *gtranslator_utils_get_locale_charset(void)
{
	if((!file_opened) || !(po->header->language))
	{
		return NULL;
	}
	else if(po->header->charset && 
		(nautilus_strcasecmp(po->header->charset, "utf-8") && 
		 nautilus_strcasecmp(po->header->charset, "utf8")))
	{
		return g_strdup(po->header->charset);
	}
	else
	{
		gint c;

		for(c=0; languages[c].name!=NULL; c++)
		{
			if(!nautilus_strcmp(languages[c].name, po->header->language) ||
				!nautilus_strcmp(_(languages[c].name), po->header->language))
			{
				return g_strdup(languages[c].encoding);
			}
		}

		return NULL;
	}
}

/*
 * Set up the lists to use within the combo boxes.
 */
void gtranslator_utils_language_lists_create(void)
{
	gint c = 0;
	list_ref++;
	/*
	 * Create only if it's the first call.
	 */
	if (list_ref > 1) 
		return;
	languages_list = encodings_list = lcodes_list = group_emails_list = 
		bits_list = NULL;
	while (languages[c].name != NULL) {
		languages_list =
		    g_list_prepend(languages_list,
				   (gpointer) _(languages[c].name));
		lcodes_list =
		    g_list_prepend(lcodes_list,
				   (gpointer) languages[c].locale);
		if (g_list_find_custom
		    (encodings_list, (gpointer) languages[c].encoding,
		     (GCompareFunc) strcmp) == NULL)
			encodings_list =
			    g_list_prepend(encodings_list,
					   (gpointer) languages[c].encoding);
		if (g_list_find_custom
		    (group_emails_list, (gpointer) languages[c].group_email,
		     (GCompareFunc) strcmp) == NULL)
			group_emails_list =
			    g_list_prepend(group_emails_list,
					   (gpointer) languages[c].group_email);
		if (g_list_find_custom
		    (bits_list, GINT_TO_POINTER(languages[c].bits),
		     (GCompareFunc) strcmp) == NULL)
			bits_list =
			    g_list_prepend(bits_list,
					   GINT_TO_POINTER(languages[c].bits));
		c++;
	}
	/*
	 * Arrange the resulting lists.
	 */
	languages_list = g_list_sort(languages_list, (GCompareFunc) strcoll);
	lcodes_list = g_list_reverse(lcodes_list);
	group_emails_list =
	    g_list_sort(group_emails_list, (GCompareFunc) strcmp);
	encodings_list =
	    g_list_sort(encodings_list, (GCompareFunc) strcmp);
	bits_list = g_list_sort(bits_list, (GCompareFunc) strcmp);
}

/*
 * Frees the language list.
 */
gboolean gtranslator_utils_language_lists_free(GtkWidget  * widget, gpointer useless)
{
	list_ref--;
	
	/*
	 * If something needs them, leave.
	 */
	if (list_ref != 0) 
	{
		return FALSE;
	}
	
	gtranslator_utils_free_list(languages_list, FALSE);
	gtranslator_utils_free_list(lcodes_list, FALSE);
	gtranslator_utils_free_list(group_emails_list, FALSE);
	gtranslator_utils_free_list(encodings_list, FALSE);
	gtranslator_utils_free_list(bits_list, FALSE);
	
	return FALSE;
}

/*
 * Try to convert the < 0.37 color preferences to their new location.
 */
void gtranslator_utils_old_colors_to_new_location()
{
	gchar *value;
	gboolean converted=FALSE;
	
	gtranslator_config_init();

	value=gtranslator_config_get_string("colors/fg");

	if(value && value[0]=='#')
	{
		gtranslator_config_set_string("colors/own_fg", value);
		converted=TRUE;
	}

	value=gtranslator_config_get_string("colors/bg");

	if(value && value[0]=='#')
	{
		gtranslator_config_set_string("colors/own_bg", value);
		converted=TRUE;
	}

	if(converted)
	{
		gtranslator_config_set_string("colors/fg", "black");
		gtranslator_config_set_string("colors/bg", "white");
	}

	gtranslator_config_close();
}

/* gtranslator_utils_getline
 *
 * reads newline (or CR) or EOF terminated line from stream, allocating the
 * return buffer as appropriate. do not free the returning value!
 *
 * copied form nautilus/src/nautilus-first-time-druid.c and modified to return
 * internal buffer
 */
gchar * 
gtranslator_utils_getline (FILE* stream)
{
	static char *ret = NULL;
	static size_t ret_size = 0;
	size_t ret_used;
	int char_read;

	if (ret == NULL) {
		ret = g_malloc (80);
		ret_size = 80;
	}
	
	/* used for terminating 0 */
	ret_used = 0;

	while ((char_read = fgetc (stream)) != EOF)
	{
		if (ret_size == (ret_used + 1)) {
			ret_size *= 2;
			ret = g_realloc (ret, ret_size); 
		}
		ret [ret_used++] = char_read;
		if ('\n' == char_read || '\r' == char_read ) {
			int next_char = fgetc(stream);
			if (EOF == next_char || 
				('\n' == char_read && '\r' == next_char) ||
				('\r' == char_read && '\n' == next_char)) 
				break;

			ungetc(next_char, stream);
			break;
		}
	}

	if (ret_used == 0) {
		GTR_FREE (ret);
		ret = NULL;
	} else {
		ret [ret_used] = '\0';
	}

	return ret;
}

/*
 * Just accomplish the given language name to it's full beautifulness.
 */
gchar *gtranslator_utils_get_full_language_name(gchar *lang)
{
	g_return_val_if_fail(lang!=NULL, NULL);

	/*
	 * If the language name does already include an underscore it will
	 *  be surely a complete language name.
	 */  
	if(strchr(lang, '_'))
	{
		return lang;
	}
	else
	{
		/*
		 * Longer language names should also be Ok.
		 */ 
		if(strlen(lang) > 2)
		{
			return lang;
		}
		else
		{
			GString *taillanguage=g_string_new(lang);
			GString *language=g_string_new(lang);
			
			/*
			 * The first two characters are converted to small
			 *  characters, and the last two are capitalized.
			 */
			language=g_string_down(language);
			taillanguage=g_string_up(taillanguage);

			/*
			 * Append the capilized letters after a '_'.
			 */
			language=g_string_append_c(language, '_');
			language=g_string_append(language, taillanguage->str);

			if(language->len > 0)
			{
				return language->str;
			}
			else
			{
				return NULL;
			}

			g_string_free(language, FALSE);
			g_string_free(taillanguage, FALSE);
		}
	}
}
