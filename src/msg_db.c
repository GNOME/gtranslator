/*
*	->Fatih Demir [ kabalak@gmx.net ]
*	Here are the routines which could be very
*	useful if you're think of slightly auto-
*	generated translations ..
*/

#include "msg_db.h"
/*
*	Include the dyndata routines 
*/
#include <dyndata.h>

int init_msg_db()
{
	FILE *db_stream;
	db_stream=fopen(msg_db,"r+");
	check_file(db_stream);
	/*
	* After having checked everything create a binary
	* tree for the translation strings ...
	*/
	BT *msg_tree;
	msg_tree=create_bt();
	LL *msg_list;
	msg_list=create_ll();
}
