/*
*	->Fatih Demir [ kabalak@gmx.net ]
*	Some database-search tries ...
*/

#include "msg_db.h"

int init_msg_db()
{
	FILE *db_stream;
	db_stream=fopen(msg_db,"r+");
	check_file(db_stream);
}
