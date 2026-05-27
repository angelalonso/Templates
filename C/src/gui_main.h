#ifndef GUI_MAIN_H
#define GUI_MAIN_H

#include <gtk/gtk.h>

// Main window widgets structure
typedef struct {
    GtkWidget *window;
    GtkWidget *content_label;
    GtkWidget *status_label;
} MainWindowWidgets;

// Create and show main window
GtkWidget *create_main_window(GtkApplication *app);

// Update main window display
void update_main_display(MainWindowWidgets *widgets);

#endif
