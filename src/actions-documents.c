/*
 * actions-documents.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2009 - Ignacio Casal Quinteiro
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "actions.h"

void
gtranslator_actions_documents_next_document (GtkAction * action,
					     GtranslatorWindow * window)
{
  GtranslatorNotebook *notebook;
  notebook = gtranslator_window_get_notebook (window);
  gtk_notebook_next_page (GTK_NOTEBOOK (notebook));
}

void
gtranslator_actions_documents_previous_document (GtkAction * action,
						 GtranslatorWindow * window)
{
  GtranslatorNotebook *notebook;
  notebook = gtranslator_window_get_notebook (window);
  gtk_notebook_prev_page (GTK_NOTEBOOK (notebook));
}
