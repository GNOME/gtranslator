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

#include "defines.include"
#include "gui.h"
#include "messages-table.h"
#include "messages.h"
#include "parse.h"
#include "preferences.h"
#include "utils.h"

#include <gal/e-paned/e-paned.h>

#include <gal/e-table/e-cell-combo.h>
#include <gal/e-table/e-cell-number.h>
#include <gal/e-table/e-cell-text.h>
#include <gal/e-table/e-table-extras.h>
#include <gal/e-table/e-table-model.h>
#include <gal/e-table/e-table-memory.h>
#include <gal/e-table/e-table-scrolled.h>
#include <gal/e-table/e-table-simple.h>

/*
 * Create the ETableExtras for our messages table.
 */
static ETableExtras *table_extras_new(void);

/*
 * Internal ETableModel functions: the names should be self-explanatory.
 *
 * Prototypes:
 */
static gint column_count_function(ETableModel *model, void *useless);
static gint row_count_function(ETableModel *model, void *useless);

static gboolean is_cell_editable_function(ETableModel *model, gint column, gint row,
	void *useless);
static gboolean is_empty_function(ETableModel *model, gint column, const void *value,
	void *useless);

static void free_value_function(ETableModel *model, gint column, void *value, void *useless);
static void set_value_at_function(ETableModel *model, gint column, gint row,
	const void *value, void *useless);

static void *duplicate_value_function(ETableModel *model, gint column, const void *value,
	void *useless);
static void *initialize_value_function(ETableModel *model, gint column, void *useless);
static void *value_at_function(ETableModel *model, gint column, gint row, void *useless);

static gchar *return_string_for_value_function(ETableModel *model, gint column, 
	const void *value, void *useless);

/* 
 * Functions:
 */
static gint column_count_function(ETableModel *model, void *useless)
{
	return 3;
}

static gint row_count_function(ETableModel *model, void *useless)
{
	if(file_opened)
	{
		return (po->length-1);
	}
	else
	{
		return 1;
	}
}

static gboolean is_cell_editable_function(ETableModel *model, gint column, gint row,
        void *useless)
{
	return TRUE;
}

static gboolean is_empty_function(ETableModel *model, gint column, const void *value,
        void *useless)
{
	if(!value || value=='\0')
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static void free_value_function(ETableModel *model, gint column, void *value, void *useless)
{
	if(value)
	{
		g_free(value);
	}
}

static void set_value_at_function(ETableModel *model, gint column, gint row,
        const void *value, void *useless)
{
	g_message("Set value for %i[%i]: %s", row, column, (gchar *) value);
}

static void *duplicate_value_function(ETableModel *model, gint column, const void *value,
        void *useless)
{
	if(value)
	{
		return g_strdup(value);
	}
	else
	{
		return NULL;
	}
}

static void *initialize_value_function(ETableModel *model, gint column, void *useless)
{
	return g_strdup("");
}

static void *value_at_function(ETableModel *model, gint column, gint row, void *useless)
{
	if(file_opened)
	{
		if(row > 0 && row <= (po->length - 1))
		{
			return g_strdup(GTR_MSG(g_list_nth_data(po->messages, row))->msgid);
		}
		else
		{
			return g_strdup("");
		}
	}
	else
	{
		return g_strdup_printf("%i[%i]", column, row);
	}
}

static gchar *return_string_for_value_function(ETableModel *model, gint column, 
        const void *value, void *useless)
{
	if(value)
	{
		return g_strdup(value);
	}
	else
	{
		return g_strdup("");
	}
}

/*
 * Creates all the nice and nifty stuff for the ETable.
 */
static ETableExtras *table_extras_new()
{
	ETableExtras 	*extras;
	ECell		*cell;
	gint		count=0;

	gchar *list_parts[] =
	{
		"number",
		"original",
		"translation",
		"comment",
		"status",
		NULL
	};

	extras=e_table_extras_new();
	
	/*
	 * Fill the table parts independently here -- just for easification
	 *  of the process, we do use this loop instead of 4, 5 different calls.
	 */
	while(list_parts[count]!=NULL)
	{
		cell=e_cell_text_new(NULL, GTK_JUSTIFY_LEFT);

		e_table_extras_add_cell(extras, list_parts[count], cell);
		count++;
	}
	
	return extras;
}

/*
 * Create the new ETable with all the stuff needed for
 *  gal/gtranslator.
 */
GtkWidget *gtranslator_messages_table_new()
{
	gchar		*statusfile;
	
	GtkWidget 	*messages_table;

	ETableExtras 	*table_extras;
	ETableModel	*table_model;

	table_extras=table_extras_new();
	
	table_model=e_table_simple_new(
		column_count_function,
		row_count_function,
		value_at_function,
		set_value_at_function,
		is_cell_editable_function,
		duplicate_value_function,
		free_value_function,
		initialize_value_function,
		is_empty_function,
		return_string_for_value_function,
		NULL);

	statusfile=gtranslator_utils_get_messages_table_state_file_name();

	messages_table=e_table_scrolled_new_from_spec_file(table_model,
		table_extras,
		ETSPECS_DIR "/messages-table.etspec", 
		statusfile);

	g_free(statusfile);
	return messages_table;
}

/*
 * Apply a very similar mechanism for hide/show/toggle-ing the
 *  messages table.
 */
gboolean gtranslator_messages_table_show()
{
	gint position=-1;

	position=e_paned_get_position(E_PANED(content_pane));

	if(position<=0)
	{
		return FALSE;
	}
	else
	{
		gtranslator_config_init();
		gtranslator_config_set_bool("toggles/show_content_pane", FALSE);
		gtranslator_config_close();

		e_paned_set_position(E_PANED(content_pane), 0);
		return TRUE;
	}
}

gboolean gtranslator_messages_table_hide()
{
	gint position=-1;

	gtranslator_config_init();
	position=gtranslator_config_get_int("interface/content_pane_position");
	gtranslator_config_close();

	if(position<=0)
	{
		return FALSE;
	}
	else
	{
		e_paned_set_position(E_PANED(content_pane), position);

		gtranslator_config_init();
		gtranslator_config_set_bool("toggles/show_content_pane", TRUE);
		gtranslator_config_close();

		return TRUE;
	}
}

void gtranslator_messages_table_toggle()
{
	if(!gtranslator_messages_table_hide())
	{
		gtranslator_messages_table_show();
	}
}
