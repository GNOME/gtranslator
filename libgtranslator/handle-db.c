/**
*
* (C) 2000 Fatih Demir -- kabalak / kabalak@gmx.net
*
* This is distributed under the GNU GPL V 2.0 or higher which can be
*  found in the file COPYING for further studies.
*
* Enjoy this piece of software, brain-crack and other nice things.
*
* WARNING: Trying to decode the source-code may be hazardous for all your
*	future development in direction to better IQ-Test rankings!
*
* PSC: This has been completely written with vim; the best editor of all.
*
**/

#include <libgtranslator/handle-db.h>

/**
* This function saves the given database.
**/
void gtranslator_save_db(GtranslatorDatabase database)
{
	/**
	* First a check.
	**/
	if(!database)
	{
		g_warning(_("No translation database given to store."));
		return;
	}
}
