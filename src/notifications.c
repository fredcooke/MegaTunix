/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
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

#include <defines.h>
#include <protos.h>
#include <constants.h>
#include <globals.h>
#include <gtk/gtk.h>


extern GdkColor red;
extern GdkColor black;
extern struct Buttons buttons;


void set_store_red()
{
	/* Let user know to burn vars byt turnign button text red */
	gtk_widget_modify_fg(GTK_BIN(buttons.const_store_but)->child,
			GTK_STATE_NORMAL,&red);
	gtk_widget_modify_fg(GTK_BIN(buttons.const_store_but)->child,
			GTK_STATE_PRELIGHT,&red);
	gtk_widget_modify_fg(GTK_BIN(buttons.enrich_store_but)->child,
			GTK_STATE_NORMAL,&red);
	gtk_widget_modify_fg(GTK_BIN(buttons.enrich_store_but)->child,
			GTK_STATE_PRELIGHT,&red);
	gtk_widget_modify_fg(GTK_BIN(buttons.vetable_store_but)->child,
			GTK_STATE_NORMAL,&red);
	gtk_widget_modify_fg(GTK_BIN(buttons.vetable_store_but)->child,
			GTK_STATE_PRELIGHT,&red);
}

void set_store_black()
{
	gtk_widget_modify_fg(GTK_BIN(buttons.const_store_but)->child,
			GTK_STATE_NORMAL,&black);
	gtk_widget_modify_fg(GTK_BIN(buttons.const_store_but)->child,
			GTK_STATE_PRELIGHT,&black);
	gtk_widget_modify_fg(GTK_BIN(buttons.enrich_store_but)->child,
			GTK_STATE_NORMAL,&black);
	gtk_widget_modify_fg(GTK_BIN(buttons.enrich_store_but)->child,
			GTK_STATE_PRELIGHT,&black);
	gtk_widget_modify_fg(GTK_BIN(buttons.vetable_store_but)->child,
			GTK_STATE_NORMAL,&black);
	gtk_widget_modify_fg(GTK_BIN(buttons.vetable_store_but)->child,
			GTK_STATE_PRELIGHT,&black);
}


void update_statusbar(GtkWidget *status_bar,int context_id, gchar * message)
{
        /* takes 3 args, 
         * the GtkWidget pointer to the statusbar,
         * the context_id of the statusbar in arg[0],
         * and the string to be sent to that bar
         *
         * Fairly generic, works for multiple statusbars
         */

        gtk_statusbar_pop(GTK_STATUSBAR(status_bar),
                        context_id);
        gtk_statusbar_push(GTK_STATUSBAR(status_bar),
                        context_id,
                        message);
}

void no_ms_connection(void)
{
	gchar *buff;
	buff = g_strdup("The MegaSquirt ECU seems to be currently disconnected.  This means that either one of the following occurred:\n   1. Wrong Comm port is selected on the Communications Tab\n   2. The MS serial link is not plugged in\n   3. The MS ECU does not have adequate power.\n\nSuggest checking the serial settings on the Communications page first, and then check the Serial Statusbar at the bottom of that page.");
	warn_user(buff);
	g_free(buff);
}

void warn_user(gchar *message)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(NULL,0,GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE,message);
	g_signal_connect (G_OBJECT(dialog),
			"delete_event",
			G_CALLBACK (gtk_widget_destroy),
			dialog);
	g_signal_connect (G_OBJECT(dialog),
			"destroy_event",
			G_CALLBACK (gtk_widget_destroy),
			dialog);
	g_signal_connect (G_OBJECT(dialog),
			"destroy_event",
			G_CALLBACK (gtk_widget_destroy),
			dialog);
	g_signal_connect_swapped (dialog,
			"response",
			G_CALLBACK (gtk_widget_destroy),
			dialog);

	gtk_widget_show_all(dialog);

}

