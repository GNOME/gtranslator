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

/**
* This happens to be a test-programm for the libgtranslator routines.
**/

#include <handle-db.h>

int main(int argc,char *argv[])
{
	xmlDocPtr ptr;
	ptr=xmlParseFile("./foo.xml");
	if(!ptr)
	{
		g_error("`foo.xml' is missing!\n");
	}
	else
	{
		add_node(ptr, "nodename", "nodecontent");
	}
	return 0;
}	
