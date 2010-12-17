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

#ifndef __FREEMS_PLUGIN_H__
#define __FREEMS_PLUGIN_H__

#include <gtk/gtk.h>
#include <defines.h>
#include <configfile.h>
#include <enums.h>
#include <threads.h>

#ifdef __FREEEMS_PLUGIN_C__
#define EXTERN
#else
#define EXTERN extern
#endif

/* Function Pointers */
EXTERN void (*error_msg_f)(const gchar *);
EXTERN gboolean (*get_symbol_f)(const gchar *,void **);
EXTERN void (*cleanup_f)(void *);
EXTERN void (*io_cmd_f)(const gchar *,void *);
EXTERN void (*dbg_func_f)(gint,gchar *);
EXTERN GList *(*get_list_f)(gchar *);
EXTERN OutputData *(*initialize_outputdata_f)(void);
EXTERN void (*set_title_f)(const gchar *);
EXTERN void (*set_widget_sensitive_f)(gpointer, gpointer);
EXTERN void (*thread_update_logbar_f)(const gchar *, const gchar *, gchar *, gboolean, gboolean);
EXTERN void (*update_logbar_f)(const gchar *, const gchar *, gchar *, gboolean, gboolean, gboolean);
EXTERN void (*process_rt_vars_f)(void * );
EXTERN void (*thread_update_widget_f)(const gchar *, WidgetType, gchar *);
EXTERN gboolean (*queue_function_f)(const gchar * );
EXTERN gboolean (*lookup_precision_f)(const gchar *, gint *);
EXTERN gboolean (*lookup_current_value_f)(const gchar *, gfloat *);
EXTERN gboolean (*lookup_current_value_f)(const gchar *, gfloat *);
EXTERN gfloat (*direct_lookup_data_f)(gchar *, gint );
EXTERN gint (*direct_reverse_lookup_f)(gchar *, gint );
EXTERN gint (*direct_reverse_lookup_f)(gchar *, gint );
EXTERN gint (*reverse_lookup_f)(gconstpointer *, gint );
EXTERN gfloat (*lookup_data_f)(gconstpointer *, gint );
EXTERN gint (*translate_string_f)(const gchar *);
EXTERN GtkWidget *(*lookup_widget_f)(const gchar *);
EXTERN void (*set_group_color_f)(GuiColor, const gchar * );
EXTERN gint (*get_multiplier_f)(DataSize);
EXTERN guint (*get_bitshift_f)(guint);
EXTERN GtkWidget *(*spin_button_handler_f)(GtkWidget *, gpointer);
EXTERN gboolean (*set_file_api_f)(ConfigFile *, gint, gint );
EXTERN gboolean (*get_file_api_f)(ConfigFile *, gint *, gint * );
EXTERN void (*stop_tickler_f)(gint);
EXTERN void (*start_tickler_f)(gint);
EXTERN gchar **(*parse_keys_f)(const gchar *, gint *, const gchar * );
EXTERN gint (*get_multiplier_f)(DataSize );
EXTERN glong (*get_extreme_from_size_f)(DataSize, Extreme);
EXTERN gfloat (*convert_after_upload_f)(GtkWidget *);
EXTERN gint (*convert_before_download_f)(GtkWidget *, gfloat);
EXTERN GdkColor (*get_colors_from_hue_f)(gfloat, gfloat, gfloat);
EXTERN void (*mem_alloc_f)(void);
EXTERN void *(*eval_create_f)(char *);
EXTERN void (*eval_destroy_f)( void *);
EXTERN double (*eval_x_f)(void *, double);
EXTERN void (*thread_widget_set_sensitive_f)(const gchar *, gboolean);
EXTERN void (*get_table_f)(gpointer, gpointer, gpointer);
EXTERN void (*free_multi_source_f)(gpointer);
EXTERN void (*flush_serial_f)(gint, gint);
EXTERN void (*_set_sized_data_f)(guint8 *, gint, DataSize, gint, gboolean);
EXTERN gint (*_get_sized_data_f)(guint8 *, gint, DataSize, gboolean);
EXTERN gboolean (*std_entry_handler_f)(GtkWidget *, gpointer);
EXTERN gboolean (*entry_changed_handler_f)(GtkWidget *, gpointer);
EXTERN gboolean (*search_model_f)(GtkTreeModel *, GtkWidget *, GtkTreeIter *);
EXTERN void (*set_reqfuel_color_f)(GuiColor, gint);
EXTERN void (*alter_widget_state_f)(gpointer, gpointer);
EXTERN void (*bind_to_lists_f)(GtkWidget *, gchar * );
EXTERN void (*warn_user_f)(const gchar *);
EXTERN gboolean (*key_event_f)(GtkWidget *, GdkEventKey *, gpointer );
EXTERN gboolean (*focus_out_handler_f)(GtkWidget *, GdkEventFocus *, gpointer );
EXTERN guint32 (*create_value_change_watch_f)(const gchar *, gboolean, const gchar *, gpointer);
EXTERN void (*remove_from_lists_f)(gchar *, gpointer);
EXTERN void (*remove_watch_f)(guint32);
EXTERN void (*thread_refresh_widget_range_f)(gint, gint, gint);
EXTERN void (*update_ve3d_if_necessary_f)(gint, gint);
EXTERN void (*thread_refresh_widget_f)(GtkWidget *);
EXTERN char *(*regex_wrapper_f)(char *, char *, int *);
EXTERN GtkWidget *(*mtx_gauge_face_new_wrapper_f)(void);
EXTERN void (*mtx_gauge_face_import_xml_wrapper_f)(GtkWidget *, gchar *);
EXTERN void (*mtx_gauge_face_set_value_wrapper_f)(GtkWidget *, gfloat);
EXTERN void (*register_widget_f)(gchar *, GtkWidget *);
EXTERN gboolean (*read_wrapper_f)(gint, void *, size_t, gint *);
EXTERN gboolean (*write_wrapper_f)(gint, const void *, size_t, gint *);
EXTERN gint (*read_data_f)(gint , void **, gboolean);

/* Function Pointers */

/* Prototypes */
void plugin_init(gconstpointer *);
void plugin_shutdown(void);
void register_common_enums(void);
/* Prototypes */

#endif
