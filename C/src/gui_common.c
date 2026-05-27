#include "gui_common.h"
#include <stdlib.h>
#include <string.h>

void update_status_label(GtkWidget *status_label, const char *message, gboolean is_error) {
    GdkRGBA color;

    if (!status_label) {
        return;
    }

    VALIDATE_WIDGET(status_label, LABEL, );

    if (is_error) {
        gdk_rgba_parse(&color, "red");
        gtk_widget_override_color(status_label, GTK_STATE_FLAG_NORMAL, &color);
        gtk_label_set_text(GTK_LABEL(status_label), message);
        return;
    }

    gdk_rgba_parse(&color, "green");
    gtk_widget_override_color(status_label, GTK_STATE_FLAG_NORMAL, &color);
    gtk_label_set_text(GTK_LABEL(status_label), message);
}

char *load_config_text(void) {
    char buffer[MAX_VALUE_LEN];
    char *result;

    if (read_text_value(buffer, sizeof(buffer))) {
        // Allocate exactly what we need
        // NOLINTNEXTLINE(performance-no-int-to-ptr)
        result = (char *)g_malloc(strlen(buffer) + 1); //
        if (result) {
            strcpy(result, buffer);
        }
    } else {
        // Empty string
        // NOLINTNEXTLINE(performance-no-int-to-ptr)
        result = (char *)g_malloc(1);
        if (result) {
            result[0] = '\0';
        }
    }

    return result;
}

gboolean save_config_text(const char *text, GtkWidget *status_label) {
    if (!text) {
        if (status_label) {
            update_status_label(status_label, "Error: No text to save", TRUE);
        }
        return FALSE;
    }

    if (update_text_value(text)) {
        if (status_label) {
            update_status_label(status_label, "Saved successfully!", FALSE);
        }
        return TRUE;
    }

    if (status_label) {
        update_status_label(status_label, "Error saving configuration!", TRUE);
    }
    return FALSE;
}
