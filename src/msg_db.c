/*
*	->Fatih Demir [ kabalak@gmx.net ]
*	Some database-search tries ...
*/

#include "msg_db.h"
#include <db.h>

int init_msg_db()
{
	FILE *db_stream;
	db_stream=fopen(msg_db,"r+");
	check_file(db_stream);
	fclose(db_stream);
	dbopen(msg_db,O_RDWR,S_IRWXU,DB_HASH,NULL);
}
