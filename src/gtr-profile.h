/*
 * (C) 2008 	Pablo Sanxiao <psanxiao@gmail.com>
 *		Igalia
 *	
 * gtranslator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or   
 * (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *   Pablo Sanxiao <psanxiao@gmail.com>
 */

#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

/*
 * Utility Macros
 */

#define GTR_TYPE_PROFILE		(gtranslator_profile_get_type ())
#define GTR_PROFILE(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_PROFILE, GtrProfile))
#define GTR_PROFILE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_PROFILE, GtrProfileClass))
#define GTR_IS_PROFILE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_PROFILE))
#define GTR_IS_PROFILE_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_PROFILE))
#define GTR_PROFILE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_PROFILE, GtrProfileClass))

/* Private structure type */
typedef struct _GtrProfilePrivate GtrProfilePrivate;

/*
 * Main object structure
 */
typedef struct _GtrProfile GtrProfile;

struct _GtrProfile
{
  GObject parent_instance;
  /*< private > */
  GtrProfilePrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtrProfileClass GtrProfileClass;

struct _GtrProfileClass
{
  GObjectClass parent_class;
};

/*
 * Public methods
 */
GType
gtranslator_profile_get_type (void)
  G_GNUC_CONST;

     GType gtranslator_profile_register_type (GTypeModule * module);

GtrProfile * gtranslator_profile_new (void);

     const gchar *
     gtranslator_profile_get_name (GtrProfile * profile);
     void
     gtranslator_profile_set_name (GtrProfile * profile,
				   const gchar * data);

     const gchar *
     gtranslator_profile_get_author_name (GtrProfile * profile);
     void
     gtranslator_profile_set_author_name (GtrProfile * profile,
					  const gchar * data);

     const gchar *
     gtranslator_profile_get_author_email (GtrProfile * profile);
     void
     gtranslator_profile_set_author_email (GtrProfile * profile,
					   const gchar * data);

     const gchar *
     gtranslator_profile_get_language_name (GtrProfile * profile);
     void
     gtranslator_profile_set_language_name (GtrProfile * profile,
					    const gchar * data);

     const gchar *
     gtranslator_profile_get_language_code (GtrProfile * profile);
     void
     gtranslator_profile_set_language_code (GtrProfile * profile,
					    const gchar * data);

     const gchar *
     gtranslator_profile_get_charset (GtrProfile * profile);
     void
     gtranslator_profile_set_charset (GtrProfile * profile,
				      const gchar * data);

     const gchar *
     gtranslator_profile_get_encoding (GtrProfile * profile);
     void
     gtranslator_profile_set_encoding (GtrProfile * profile,
				       const gchar * data);

     const gchar *
     gtranslator_profile_get_group_email (GtrProfile * profile);
     void
     gtranslator_profile_set_group_email (GtrProfile * profile,
					  const gchar * data);

     const gchar *
     gtranslator_profile_get_plurals (GtrProfile * profile);
     void
     gtranslator_profile_set_plurals (GtrProfile * profile,
				      const gchar * data);

     void
     gtranslator_profile_xml_new_entry (xmlDocPtr doc,
					GtrProfile * profile);
GtrProfile * gtranslator_profile_xml_get_entry (xmlNodePtr child);

     GList *
     gtranslator_profile_get_profiles_from_xml_file (gchar * filename);

     gint
     gtranslator_profile_save_profiles_in_xml (gchar * filename);

#endif /* __PROFILE_H__ */
