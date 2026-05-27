#include "gui_main.h"
#include "config.h"
#include "gui_common.h"
#include "gui_edit.h"
#include <stdlib.h>
#include <string.h>

#define DISPLAY_EXTRA 100

// Update the main window display
void update_main_display(MainWindowWidgets *widgets) {
    char buffer[MAX_VALUE_LEN];

    if (!widgets) {
        return;
    }

    if (read_text_value(buffer, sizeof(buffer))) {
        char display_text[MAX_VALUE_LEN + DISPLAY_EXTRA];
        snprintf(display_text, sizeof(display_text),
                 "<span font='14' weight='bold'>Current Configuration:</span>\n\n"
                 "<span font='11'>%s</span>",
                 buffer);
        gtk_label_set_markup(GTK_LABEL(widgets->content_label), display_text);
        update_status_label(widgets->status_label, "Loaded successfully", FALSE);
    } else {
        gtk_label_set_markup(GTK_LABEL(widgets->content_label),
                             "<span font='14' weight='bold'>No configuration found</span>\n\n"
                             "<span>Click Edit to create new content.</span>");
        update_status_label(widgets->status_label, "No configuration found", TRUE);
    }
}

// Refresh button callback
void on_refresh_clicked(GtkWidget *button, gpointer data) {
    MainWindowWidgets *widgets = (MainWindowWidgets *)data;
    if (widgets) {
        update_main_display(widgets);
        update_status_label(widgets->status_label, "Refreshed", FALSE);
    }
}

// Edit button callback
void on_edit_clicked(GtkWidget *button, gpointer data) {
    MainWindowWidgets *widgets = (MainWindowWidgets *)data;
    if (widgets) {
        create_edit_dialog(widgets->window, widgets);
    }
}

// Create the main window
GtkWidget *create_main_window(GtkApplication *app) {
    MainWindowWidgets *widgets = NULL;
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *content_box;
    GtkWidget *scrolled_window;
    GtkWidget *content_label;
    GtkWidget *button_box;
    GtkWidget *edit_button;
    GtkWidget *refresh_button;
    GtkWidget *status_label;
    GtkWidget *title_label;
    GtkWidget *separator;

    // Allocate widgets structure
    widgets = g_new0(MainWindowWidgets, 1);

    // Create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Configuration Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Store widgets pointer as user data for callbacks
    g_object_set_data_full(G_OBJECT(window), "main_widgets", widgets, g_free);

    // Create vertical box container
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create title label
    title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label),
                         "<span size='x-large' weight='bold'>Configuration Editor</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 0);

    // Create separator
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 5);

    // Create content box
    content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), content_box, TRUE, TRUE, 0);

    // Create scrolled window for content
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(content_box), scrolled_window, TRUE, TRUE, 0);

    // Create content label (displays the current config)
    content_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(content_label),
                         "<span font='14' weight='bold'>Loading...</span>");
    gtk_label_set_justify(GTK_LABEL(content_label), GTK_JUSTIFY_LEFT);
    gtk_label_set_xalign(GTK_LABEL(content_label), 0.0);
    gtk_label_set_yalign(GTK_LABEL(content_label), 0.0);
    gtk_widget_set_margin_start(content_label, 10);
    gtk_widget_set_margin_end(content_label, 10);
    gtk_widget_set_margin_top(content_label, 10);
    gtk_widget_set_margin_bottom(content_label, 10);
    gtk_container_add(GTK_CONTAINER(scrolled_window), content_label);

    // Create button box
    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    // Create buttons
    edit_button = gtk_button_new_with_label("Edit");
    refresh_button = gtk_button_new_with_label("Refresh");
    gtk_container_add(GTK_CONTAINER(button_box), edit_button);
    gtk_container_add(GTK_CONTAINER(button_box), refresh_button);

    // Style buttons
    gtk_widget_set_size_request(edit_button, 100, 35);
    gtk_widget_set_size_request(refresh_button, 100, 35);

    // Create status label
    status_label = gtk_label_new("Ready");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 5);

    // Store widgets in structure
    widgets->window = window;
    widgets->content_label = content_label;
    widgets->status_label = status_label;

    // Connect signals
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_clicked), widgets);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_clicked), widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Load initial display
    update_main_display(widgets);

    return window;
}
