#include "gui_common.h"
#include <string.h>
#include <stdlib.h>

void update_status_label(GtkWidget *status_label, const char *message, gboolean is_error) {
    GdkRGBA color;
    
    if (!status_label) return;
    
    VALIDATE_WIDGET(status_label, LABEL, );
    
    if (is_error) {
        gdk_rgba_parse(&color, "red");
    } else {
        gdk_rgba_parse(&color, "green");
    }
    gtk_widget_override_color(status_label, GTK_STATE_FLAG_NORMAL, &color);
    gtk_label_set_text(GTK_LABEL(status_label), message);
}

char* load_config_text(void) {
    char buffer[MAX_VALUE_LEN];
    char *result = NULL;
    
    if (read_text_value(buffer, sizeof(buffer))) {
        result = g_strdup(buffer);
    } else {
        result = g_strdup("");
    }
    
    return result;
}

gboolean save_config_text(const char *text, GtkWidget *status_label) {
    if (!text) return FALSE;
    
    if (update_text_value(text)) {
        if (status_label) {
            update_status_label(status_label, "Saved successfully!", FALSE);
        }
        return TRUE;
    } else {
        if (status_label) {
            update_status_label(status_label, "Error saving configuration!", TRUE);
        }
        return FALSE;
    }
}
