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
#include "utils.h"

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
 * Remove the gtranslator-generated temp.-files.
 */
void gtranslator_utils_remove_temp_files()
{
	gchar *tempfile=gtranslator_utils_get_temp_file_name();

	/*
	 * Check and clean up our "namespace" in ~/.gtranslator.
	 */
	 
	if(g_file_exists(tempfile))
	{
		remove(tempfile);
	}

	GTR_FREE(tempfile);
	tempfile=gtranslator_utils_get_save_differently_file_name();

	if(g_file_exists(tempfile))
	{
		remove(tempfile);
	}

	GTR_FREE(tempfile);
}

/*
 * Create the ".gtranslator" directory in the users home directory if necessary.
 */
void gtranslator_utils_create_gtranslator_directory()
{
	gchar *dirname;

	dirname=g_strdup_printf("%s/.gtranslator", g_get_home_dir());

	/*
	 * Create the directory if needed.
	 */
	if(!g_file_test(dirname, G_FILE_TEST_EXISTS))
	{
		if(e_mkdir_hier(dirname, 0755)==-1)
		{
			g_warning(_("Can't create directory `%s'!"), dirname);
			exit(1);
		}
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
 * Return the generally used filename of our temp. file.
 */
gchar *gtranslator_utils_get_temp_file_name()
{
	gchar *tempfilename;

	tempfilename=g_strdup_printf("%s/.gtranslator/gtranslator-temp-po-file",
		g_get_home_dir());

	return tempfilename;
}

/*
 * Return the filename of the crash-file.
 */
gchar *gtranslator_utils_get_crash_file_name()
{
	gchar *crashfile;

	crashfile=g_strdup_printf("%s/.gtranslator/gtranslator-crash-po-file", 
		g_get_home_dir());

	return crashfile;
}

/*
 * Get the different filename for the save-differently.c/.h routines.
 */
gchar *gtranslator_utils_get_save_differently_file_name()
{
	gchar *sd_file;

	sd_file=g_strdup_printf(
		"%s/.gtranslator/gtranslator-save-differently-temp-po-file",
		g_get_home_dir());

	return sd_file;
}

/*
 * Get the ETable state specification filename for our messages table.
 */
gchar *gtranslator_utils_get_messages_table_state_file_name()
{
	gchar *state_file;
	
	state_file=g_strdup_printf(
		"%s/.gtranslator/gtranslator-ui-messages-table-state",
		g_get_home_dir());
	
	return state_file;
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
			strcmp(entry->d_name, ".") && 
			strcmp(entry->d_name, "..") &&
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

	/*
	 * Test the files list.
	 */
	g_return_val_if_fail(files!=NULL, NULL);

	files=g_list_reverse(files);

	/*
	 * If the according argument is given, then sort the filenames list.
	 */
	if(sort)
	{
		files=g_list_sort(files, (GCompareFunc) nautilus_strcmp);
	}

	closedir(dir);
	return files;
}

/*
 * Free the list and it's data safely -- NULL cases should be catched here.
 */
void gtranslator_utils_free_list(GList *list)
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
		 * Free the list data as long as possible.
		 */
		while(list!=NULL)
		{
			GTR_FREE(list->data);
			GTR_ITER(list);
		}

		g_list_free(list);
		list=NULL;
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

		list=list->next;
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
				return languages[c].lcode;
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
	else
	{
		gint c;

		for(c=0; languages[c].name!=NULL; c++)
		{
			if(!strcmp(languages[c].name, po->header->language) ||
				!strcmp(_(languages[c].name), po->header->language))
			{
				return languages[c].enc;
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
				   (gpointer) languages[c].lcode);
		if (g_list_find_custom
		    (encodings_list, (gpointer) languages[c].enc,
		     (GCompareFunc) strcmp) == NULL)
			encodings_list =
			    g_list_prepend(encodings_list,
					   (gpointer) languages[c].enc);
		if (g_list_find_custom
		    (group_emails_list, (gpointer) languages[c].group,
		     (GCompareFunc) strcmp) == NULL)
			group_emails_list =
			    g_list_prepend(group_emails_list,
					   (gpointer) languages[c].group);
		if (g_list_find_custom
		    (bits_list, (gpointer) languages[c].bits,
		     (GCompareFunc) strcmp) == NULL)
			bits_list =
			    g_list_prepend(bits_list,
					   (gpointer) languages[c].bits);
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
	
	gtranslator_utils_free_list(languages_list);
	gtranslator_utils_free_list(lcodes_list);
	gtranslator_utils_free_list(group_emails_list);
	gtranslator_utils_free_list(encodings_list);
	gtranslator_utils_free_list(bits_list);
	
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
		/* Translators: DO NOT translate these */
		g_warning("Converting old fg color..");
		gtranslator_config_set_string("colors/own_fg", value);
		converted=TRUE;
	}

	value=gtranslator_config_get_string("colors/bg");

	if(value && value[0]=='#')
	{
		/* Translators: DO NOT translate these. */
		g_warning("Converting old bg color");
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
char * 
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
