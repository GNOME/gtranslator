/**
* Fatih Demir [ kabalak@gmx.net ]
*
* (C) 2000 Published under GNU GPL V 2.0+
*
* The find-routines for gtranslator.
*
* -- the header.
**/

#ifndef GTR_FIND_H 
#define GTR_FIND_H 1

#include "parse.h"

/**
* The results list.
**/
GList *results;

/**
* The main search/find-function.
**/
gchar *gtr_find(GList *list,char *query);

/**
* A simple voodoo-function which always returns something, but 
*  always something not useful.
**/
gchar *gtr_find_get_hastalavista(const char *query);

#endif
