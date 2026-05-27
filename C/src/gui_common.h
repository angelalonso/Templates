#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#include <gtk/gtk.h>
#include "config.h"

// Status update function (used by all GUI components)
void update_status_label(GtkWidget *status_label, const char *message, gboolean is_error);

// Load config and return as string (returns newly allocated string, must be freed)
char* load_config_text(void);

// Save config from string
gboolean save_config_text(const char *text, GtkWidget *status_label);

// Validation macro for widget safety
#define VALIDATE_WIDGET(widget, type, return_val) \
    do { \
        if (widget == NULL) { \
            g_printerr("ERROR: %s is NULL at %s:%d\n", #widget, __FILE__, __LINE__); \
            return return_val; \
        } \
        if (!GTK_IS_##type(widget)) { \
            g_printerr("ERROR: %s is not a valid Gtk%s at %s:%d\n", #widget, #type, __FILE__, __LINE__); \
            return return_val; \
        } \
    } while(0)

#endif
