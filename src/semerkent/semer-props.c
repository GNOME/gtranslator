/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
 *
 * semerkent is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Library General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *    option) any later version.
 *    
 * semerkent is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *    License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * semerkent is being developed around gtranslator, so that any wishes or own
 *  requests should be mailed to me, as I will naturally keep semerkent very
 *   close to gtranslator. This shouldn't avoid any use of semerkent from other
 *    applications/packages.
 *
 * The homepage for semerkent is: http://semerkent.sourceforge.net
 */

#include "semer-props.h"
#include "semer-utils.h"

G_BEGIN_DECLS

/*
 * The new/creation-method for our SemerProps -- 
 *  plain and in node connection.
 */
SemerProps *semer_props_new(void)
{
	SemerProps *props=g_new0(SemerProps, 1);
	
	props->id=NULL;
	props->changedate=semer_utils_tmx_date_new();
	props->type=NULL;
	props->client=NULL;
	props->domain=NULL;
	props->sourcefile=NULL;
	props->translator_name=NULL;
	props->translator_email=NULL;
	
	return props;
}

/*
 * Return the translator name + EMail in a usual form.
 */
gchar *semer_props_get_translator(SemerProps *props)
{
	gchar	*translator_string;
	
	g_return_val_if_fail(SEMER_PROPS(props)!=NULL, NULL);

	/*
	 * Check if the given SemerProps do own an own translator attribute
	 *  at all.
	 */
	if(SEMER_PROPS(props)->translator_name)
	{
		/*
		 * This should be the normal case: the "translator name <EMail>"
		 *  string is returned.
		 */
		if(SEMER_PROPS(props)->translator_email)
		{
			translator_string=g_strdup_printf("%s <%s>",
				SEMER_PROPS(props)->translator_name,
				SEMER_PROPS(props)->translator_email);
		}
		else
		{
			/*
			 * Return at least the translator name.
			 */
			translator_string=g_strdup(
				SEMER_PROPS(props)->translator_name);
		}

		return translator_string;
	}
	else
	{
		return NULL;
	}
}

/*
 * Sets the translator's name/EMail according to the arguments.
 */
void semer_props_set_translator(SemerProps *props,
	const gchar *translator_name,
	const gchar *translator_email)
{
	g_return_if_fail(SEMER_PROPS(props)!=NULL);

	/*
	 * Set the translatorname + EMail from the arguments -- use the
	 *  previously owned values as fallback values.
	 */
	semer_utils_strset(&SEMER_PROPS(props)->translator_name,
		translator_name, SEMER_PROPS(props)->translator_name);
	
	semer_utils_strset(&SEMER_PROPS(props)->translator_email,
		translator_email, SEMER_PROPS(props)->translator_email);
}

/*
 * Free the given Props.
 */
void semer_props_free(SemerProps *props)
{
	g_return_if_fail(SEMER_PROPS(props)!=NULL);
	
	semer_utils_free(SEMER_PROPS(props)->id);
	semer_utils_free(SEMER_PROPS(props)->changedate);
	semer_utils_free(SEMER_PROPS(props)->type);
	semer_utils_free(SEMER_PROPS(props)->client);
	semer_utils_free(SEMER_PROPS(props)->domain);
	semer_utils_free(SEMER_PROPS(props)->sourcefile);
	semer_utils_free(SEMER_PROPS(props)->translator_name);
	semer_utils_free(SEMER_PROPS(props)->translator_email);
	
	semer_utils_free(SEMER_PROPS(props));
}

/*
 * Copy the SemerProps safely -- NULL's are kept!
 */
SemerProps *semer_props_copy(SemerProps *props)
{
	SemerProps *copy=semer_props_new();
	
	g_return_val_if_fail(SEMER_PROPS(props)!=NULL, NULL);

	copy->id=semer_utils_strcopy(SEMER_PROPS(props)->id);
	
	semer_utils_free(copy->changedate);
	copy->changedate=semer_utils_strcopy(SEMER_PROPS(props)->changedate);
	copy->type=semer_utils_strcopy(SEMER_PROPS(props)->type);
	copy->client=semer_utils_strcopy(SEMER_PROPS(props)->client);
	copy->domain=semer_utils_strcopy(SEMER_PROPS(props)->domain);
	copy->sourcefile=semer_utils_strcopy(SEMER_PROPS(props)->sourcefile);
	copy->translator_name=semer_utils_strcopy(SEMER_PROPS(props)->translator_name);
	copy->translator_email=semer_utils_strcopy(SEMER_PROPS(props)->translator_email);

	return copy;
}

/*
 * Create an "advanced" and filles SemerProps out of the given arguments.
 */
SemerProps *semer_props_new_with_specs(const gchar *id, 
	const gchar *changedate, 
	const gchar *type,
	const gchar *client, 
	const gchar *domain,
	const gchar *sourcefile,
	const gchar *translator_name,
	const gchar *translator_email)
{
	SemerProps *props=g_new0(SemerProps, 1);
	
	props->id=semer_utils_strcopy(id);
	props->changedate=semer_utils_strcopy(changedate);
	props->type=semer_utils_strcopy(type);
	props->client=semer_utils_strcopy(client);
	props->domain=semer_utils_strcopy(domain);
	props->sourcefile=semer_utils_strcopy(sourcefile);
	props->translator_name=semer_utils_strcopy(translator_name);
	props->translator_email=semer_utils_strcopy(translator_email);
	
	return props;
}

G_END_DECLS
