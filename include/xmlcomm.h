/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef __XMLCOMM_H__
#define __XMLCOMM_H__

#include <defines.h>
#include <enums.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


typedef struct _PotentialArg PotentialArg;
typedef struct _Command Command;
typedef struct _PostFunction PostFunction;

/*!
 * \brief _PotentialArgs struct holds information read from the 
 * communications XML, one PotentialArg per command
 */
struct _PotentialArg
{
	gchar *name;		/* Potential arg name */
	gchar *desc;		/* Description */
	gchar *internal_name;	/* Internal name used for linking */
	gint count;		/* Number of elements to xfer */
	DataSize size;		/* Size of data */
};


/*!
 * \brief _Command struct holds information read from the 
 * communications XML, describing each possible command. Their names
 * are used as internal keys to link firmware stuf to the XML definitions.
 */
struct _Command
{
	gchar *name;		/* Command Name */
	gchar *desc;		/* Command Description */
	gchar *base;		/* Base command charactor(s) */
	CmdType type;		/* Command type enumeration */
	GArray *args;		/* Argument list array of PotentialArgs */
	GArray *post_functions;	/* Argument list array of PostFunctions */
	gchar *return_data_func;/* Return data function name */
	XmlCmdType return_data_arg;/* Return data arg (ENUM) */
	gchar *func_call_name;	/* FUNC_CALL function name */
	void (*function) (XmlCmdType); /* Function call pointer */
	XmlCmdType func_call_arg;/* Enum arg to function call */
	gchar *worker_func;	/* Actual processing function */
};


struct _PostFunction
{
	gchar *name;		/* Function name */
	void (*function) (XmlCmdType); /* Pointer to function */
};
/* Prototypes */
void load_comm_xml(gchar *, gpointer );
void load_xmlcomm_elements(xmlNode *);
void load_potential_args(GHashTable *, xmlNode *);
void load_commands(GHashTable *, xmlNode *);
void load_arg_details(PotentialArg *, xmlNode *);
void load_cmd_details(Command *, xmlNode *);
void load_cmd_args(Command *, xmlNode *);
void load_cmd_post_functions(Command *, xmlNode *);
void load_cmd_func_call_details(Command *,xmlNode *);
/* Prototypes */

#endif
