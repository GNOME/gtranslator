/*
* Fatih Demir [ kabalak@gmx.net ]
*
* Here will be the db-type stuff. 
* I'm thinking of a pure gzipped
* text file as msg[id][str]-base
*/

#include "header_stuff.h"
#include "parse.h"
#include "callbacks.h"
#include "interface.h"

/*
* The default msg_db location .
*/
const char *msg_db=MSG_DB_DIR "msg.db";

/*
* Init the msg-db ( open & 
* check the file )
*/
int init_msg_db();

/*
* Close the msg-db ( file )
* & gzip it 
*/
void close_msg_db();

/*
* Put this to the db
*/
int put_to_msg_db(const char *);

/*
* Get a similar entry from 
* the msg-db & return a ´char *´
*/
char *get_from_msg_db(const char *);

/*
* Returns the length of the msg-db
* in bytes :
*/
unsigned int get_msg_db_size();
