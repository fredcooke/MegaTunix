/*
 * Copyright (C) 2002-2011 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <init.h>
#include <listmgmt.h>

static GHashTable *lists_hash = NULL;
extern gconstpointer *global_data;

/*!
 \brief get_list returns the list referenced by name
 \param key, Text name of list to return a pointer to
 \returns pointer to GList
 \see store_list
 */
G_MODULE_EXPORT GList * get_list(const gchar * key)
{
	if (!lists_hash)
	{
		lists_hash = g_hash_table_new_full(g_str_hash,g_str_equal,g_free,NULL);
		DATA_SET_FULL(global_data,"lists_hash",lists_hash,dealloc_lists_hash);
	}
	return (GList *)g_hash_table_lookup(lists_hash,key);
}


/*!
 \brief store_list stores a list by a textual name
 \param key, Text name of list to store
 \param list pointer to list to store
 \see get_list
 */
G_MODULE_EXPORT void store_list(const gchar * key, GList * list)
{
	if (!lists_hash)
	{
		lists_hash = g_hash_table_new_full(g_str_hash,g_str_equal,g_free,NULL);
		DATA_SET_FULL(global_data,"lists_hash",lists_hash,dealloc_lists_hash);
	}
	g_hash_table_replace(lists_hash,g_strdup(key),(gpointer)list);
	return;
}


/*!
 \brief remove_list removes a list from the hashtable
 \param key, Text name of list to remove
 \see get_list
 */
G_MODULE_EXPORT void remove_list(const gchar *key)
{
	if (!lists_hash)
		return;
	g_hash_table_remove(lists_hash,key);
}


/*!
  \brief Does a string comparison on two list elements 'name' members
  \param a, pointer to ListElement structure
  \param b, pointer to ListElement structure
  */
G_MODULE_EXPORT gint list_sort(gconstpointer a, gconstpointer b)
{
	ListElement *a1 = (ListElement *)a;
	ListElement *b1 = (ListElement *)b;
	return g_ascii_strcasecmp(a1->name,b1->name);
}


/*!
  \brief does a object string comparison on the key passed
  \param a, pointer to gconstpointer object
  \param b, pointer to gconstpointer object
  \param data, pointer to char string of the key name to each object passed
  */
G_MODULE_EXPORT gint list_object_sort(gconstpointer a, gconstpointer b, gpointer data)
{
	const gchar *key = (const gchar *)data;
	return g_ascii_strcasecmp((gchar *)DATA_GET(a,key),(gchar *)DATA_GET(b,key));
}


/*!
  \brief free's a ListElement Structure
  \param data, pointer to ListElement structure to be freed
  \param user_data, unused
  */
G_MODULE_EXPORT void free_element(gpointer data, gpointer user_data)
{
	ListElement *a = (ListElement *)data;
	g_free(a->filename);
	g_free(a->name);
	g_free(a);
}
