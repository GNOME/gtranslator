/*
 * (C) 2001-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 * 			Ignacio Casal Quinteiro <nacho.resa@gmail.com>
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

#include <glib/gi18n.h>

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
		'\\', '<', '>', 1
	};

	g_return_val_if_fail(str!=NULL, NULL);
	
	/*
	 * First strip out all the 'Ž' characters from the string.
	 */
	stripped_string=nautilus_str_strip_chr(str, 'Ž');

	/*
	 * Now strip out all these special characters to get a more raw
	 *  string (could result in better results for matching).
	 */
	while((gint) punctuation_characters[index]!=1)
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
	const gchar	*contents=NULL;
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
		else if(!g_ascii_strncasecmp(languages[i].locale, locale_code, 2))
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
			gtranslator_config_set_string("language/plural_string", languages[i].plural);
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
	if(g_file_test(gtranslator_runtime_config->temp_filename, G_FILE_TEST_EXISTS))
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
		if(gtranslator_mkdir_hier(dirname, 0755)==-1)
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
		g_free(dirname);
		dirname=g_strdup_printf("%s/.gtranslator/%s", g_get_home_dir(), subdirectories[i]);

		if(!g_file_test(dirname, G_FILE_TEST_EXISTS))
		{
			if(gtranslator_mkdir_hier(dirname, 0755)==-1)
			{
				g_warning(_("Can't create directory `%s'!"), dirname);
				exit(1);
			}
		}

		i++;
	}

	g_free(dirname);
}

/*
 * The GSourceFunc for the periodic autosaving.
 */
gboolean gtranslator_utils_autosave(gpointer data)
{
	GList	*pagelist;
	GtrPage	*page;

	/*
	 * Call autosave on any currently open pages
	 */
	/*pagelist = pages;
	while(pagelist) {
		page = (GtrPage*)pagelist->data;
		gtranslator_page_autosave(page);
		pagelist = pagelist->next;
	}*/
	
	
	return TRUE;
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
				file=entry->d_name;
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

			g_free(file);
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
				g_free(list->data);
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
	if(strlen(a) > strlen(b))
	{
		if(!g_ascii_strncasecmp(a, b, (strlen(a) - 1)))
		{
			return 100.0;
		}

		one_char_percentage=(100 / (strlen(a) - 1));
	}
	else
	{
		if(!g_ascii_strncasecmp(b, a, (strlen(b) - 1)))
		{
			return 100.0;
		}

		one_char_percentage=(100 / (strlen(b) - 1));
	}

	/*
	 * Now we do check the characters of the two given strings..
	 *  Voodoo, Magic!
	 */
	while(a[i] && b[i] && a[i]!='\0' && b[i]!='\0')
	{
		if((a[i] <= 32) || (b[i] <= 32))
		{
			i++;
			continue;
		}
		else if(a[i]==b[i])
		{
			similarity+=one_char_percentage;
		}
		else if((tolower(a[i]))==(tolower(b[i])))
		{
			similarity+=(one_char_percentage / 2);
		}
		
		i++;
	}

	if(similarity >= 100.0)
	{
		return 100.0;
	}
	else
	{
		return similarity;
	}
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
		g_free (ret);
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
			language=g_string_ascii_down(language);
			taillanguage=g_string_ascii_up(taillanguage);

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

gint gtranslator_xml_get_integer_prop_by_name_with_default (const xmlNode *parent,
	const xmlChar *prop_name, gint default_int)
{
	xmlChar *prop;
	gint ret_val=default_int;

	g_return_val_if_fail(parent!=NULL, 0);
	g_return_val_if_fail(prop_name!=NULL, 0);

	prop=xmlGetProp((xmlNode *) parent, prop_name);
	
	if(prop!=NULL)
	{
		(void) sscanf((gchar *) prop, "%d", &ret_val);
		xmlFree (prop);
	}

	return ret_val;
}

void gtranslator_xml_set_integer_prop_by_name (xmlNode *parent, const xmlChar *prop_name,
	gint value)
{
	gchar *valuestr;

	g_return_if_fail(parent!=NULL);
	g_return_if_fail(prop_name!=NULL);

	valuestr=g_strdup_printf("%d", value);
	xmlSetProp(parent, prop_name, (xmlChar *) valuestr);
	g_free(valuestr);
}

gchar *gtranslator_xml_get_string_prop_by_name_with_default(const xmlNode *parent,
	const xmlChar *prop_name, const gchar *default_string)
{
	xmlChar *prop;
	gchar *ret_val;

	g_return_val_if_fail(parent!=NULL, 0);
	g_return_val_if_fail(prop_name!=NULL, 0);

	prop=xmlGetProp((xmlNode *) parent, prop_name);
	
	if(prop!=NULL)
	{
		ret_val=g_strdup((gchar *) prop);
		xmlFree(prop);
	}
	else
	{
		g_return_val_if_fail(default_string!=NULL, NULL);
		ret_val=g_strdup (default_string);
	}

	return ret_val;
}

void gtranslator_xml_set_string_prop_by_name(xmlNode *parent, const xmlChar *prop_name,
	const gchar *value)
{
	g_return_if_fail(parent!=NULL);
	g_return_if_fail(prop_name!=NULL);

	if(value!=NULL)
	{
		xmlSetProp(parent, prop_name, (xmlChar *) value);
	}
}

int gtranslator_mkdir_hier(const char *path, mode_t mode)
{
        char *copy, *p;

        p=copy=g_strdup (path);
	
        do
	{
                p=strchr(p + 1, '/');
                if(p)
		{
                        *p='\0';
		}
		
                if(access (copy, F_OK)==-1)
		{
                        if(mkdir(copy, mode)==-1)
			{
                                g_free(copy);
                                return -1;
                        }
                }
                if(p)
		{
                        *p='/';
		}
        } while(p);

        g_free(copy);
        return 0;
}

GdkPixbuf*
gtranslator_pixbuf_from_file(gchar *file)
{
    GdkPixbuf    *pixbuf;
    GError       *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(file, &error);

    if (error){
		g_critical ("Could not load pixbuf: %s\n", error->message);
		g_error_free(error);
		return NULL;
    }
    return pixbuf;
}

gchar*
gtranslator_get_plural_form_string(gchar *lang)
{

#define BUF_SIZE 2048
	
	gchar	*cmd, 
			*po_test_file,
			**pair,
			buf[BUF_SIZE],
			*plural_forms = NULL;
	FILE 	*fs;
		
	g_return_val_if_fail(lang!=NULL,NULL);
	
	/*
	 * Check if msginit is available on the system.
	 */
	if(!g_find_program_in_path("msginit")) {
		gtranslator_show_message(_("Sorry, msginit isn't available on your system!"), NULL);
		return NULL;
	}	
	po_test_file = g_strconcat ("# SOME DESCRIPTIVE TITLE.\n",
				"# Copyright (C) YEAR Free Software Foundation, Inc.\n",
				"# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n",
				"#\n",
				"#, fuzzy\n",
				"msgid \"\"\n",
				"msgstr \"\"\n",
				"\"Project-Id-Version: PACKAGE VERSION\\n\"\n",
				"\"POT-Creation-Date: 2002-06-25 03:23+0200\\n\"\n",
				"\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n",
				"\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n",
				"\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n",
				"\"MIME-Version: 1.0\\n\"\n",
				"\"Content-Type: text/plain; charset=CHARSET\\n\"\n",
				"\"Content-Transfer-Encoding: ENCODING\\n\"\n",
				NULL);
	
	cmd = g_strdup_printf("echo '%s' | msginit -i- -l %s -o- --no-translator --no-wrap 2>/dev/null",
							po_test_file, lang);
	
	fs = popen(cmd,"r");

#define if_key_is(str) if (pair[0] && !strcmp(pair[0],str))
	while(fgets(buf, BUF_SIZE, fs) != NULL)
	{
		pair = g_strsplit(buf, ": ", 2);		
		if_key_is("\"Plural-Forms")
		{
		    plural_forms = g_strdup(*g_strsplit(pair[1], "\\n\"", 0));						
		}
		g_strfreev(pair);
	}
	
	pclose(fs);
	return plural_forms;
}
