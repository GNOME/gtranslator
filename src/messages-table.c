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
#include "utils.h"

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
	return 4;
}

static gint row_count_function(ETableModel *model, void *useless)
{
	return 0;
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
	g_message("Set value in %i:%i => %s", column, row, (gchar *)value);
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
	/*
	 * FIXME: Is foo'sh of course.. 
	 */
	return g_strdup("FIXME!");
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
		NULL
	};

	extras=e_table_extras_new();
	
	while(list_parts[count]!=NULL)
	{
		if(!g_strcasecmp(list_parts[count], "number"))
		{
			cell=e_cell_number_new(NULL, GTK_JUSTIFY_LEFT);
		}
		else
		{
			cell=e_cell_text_new(NULL, GTK_JUSTIFY_LEFT);
		}

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

	messages_table=e_table_scrolled_new_from_spec_file(table_model,
		table_extras,
		ETSPECS_DIR "/messages-table.etspec", 
		NULL);

	return messages_table;
}
