/*
* Fatih Demir [ kabalak@gmx.net ]
*
* Here are the routines which could be very
* useful if you're think of slightly auto-
* generated translations ..
*/

#include "msg_db.h"

/*
* Include the dyndata routines 
*/
#include <dyndata.h>

/*
* The db file-stream
*/
FILE *db_stream;

int init_msg_db()
{
	db_stream=fopen(msg_db,"r+");
	/*
	* Have we got a file stream ?
	*/
	check_file(db_stream,"Couldn't open the msg-db !");
	/*
	* After having checked everything create a binary
	* tree for the translation strings ...
	*/
	BT *msg_tree;
	msg_tree=create_bt();
	/*
	* And a linked list for the single message ids/strs 
	*/
	LL *msg_list;
	msg_list=create_ll();
}

void close_msg_db()
{
	/*
	* Have we lost the file stream  
	*/
	check_file(db_stream,"Couldn't close the msg-db !");
	/*
	* Go to the end of the file ..	
	*/
	fseek(db_stream,0L,SEEK_END);
	/*
	* Close the msg-db file(stream)
	*/
	fclose(db_stream);
}
