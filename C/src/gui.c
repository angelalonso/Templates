#include "config.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    GtkWidget *text_view;
    GtkWidget *status_label;
} AppWidgets;

// Function to update status label
void update_status(GtkWidget *status_label, const char *message, gboolean is_error) {
    GdkRGBA color;
    if (is_error) {
        gdk_rgba_parse(&color, "red");
    } else {
        gdk_rgba_parse(&color, "green");
    }
    gtk_widget_override_color(status_label, GTK_STATE_FLAG_NORMAL, &color);
    gtk_label_set_text(GTK_LABEL(status_label), message);
}

// Load text from config file
void load_text(GtkWidget *button, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    char buffer[MAX_VALUE_LEN];
    
    if (read_text_value(buffer, sizeof(buffer))) {
        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->text_view));
        gtk_text_buffer_set_text(text_buffer, buffer, -1);
        update_status(widgets->status_label, "Loaded successfully", FALSE);
    } else {
        update_status(widgets->status_label, "Failed to load config. Creating new...", TRUE);
        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->text_view));
        gtk_text_buffer_set_text(text_buffer, "", -1);
    }
}

// Save text to config file
void save_text(GtkWidget *button, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);
    
    if (update_text_value(text)) {
        update_status(widgets->status_label, "Saved successfully!", FALSE);
    } else {
        update_status(widgets->status_label, "Error saving configuration!", TRUE);
    }
    
    g_free(text);
}

// Create the main window
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *scrolled_window;
    GtkWidget *button_box;
    GtkWidget *load_button;
    GtkWidget *save_button;
    GtkWidget *status_label;
    GtkWidget *text_view;
    
    // Allocate widgets structure
    AppWidgets *widgets = g_malloc(sizeof(AppWidgets));
    
    // Create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Configuration Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    
    // Create vertical box container
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Create scrolled window for text view
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Create text view
    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Create button box
    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);
    
    // Create buttons
    load_button = gtk_button_new_with_label("Load");
    save_button = gtk_button_new_with_label("Save");
    gtk_container_add(GTK_CONTAINER(button_box), load_button);
    gtk_container_add(GTK_CONTAINER(button_box), save_button);
    
    // Create status label
    status_label = gtk_label_new("Ready");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);
    
    // Store widgets in structure
    widgets->text_view = text_view;
    widgets->status_label = status_label;
    
    // Connect signals
    g_signal_connect(load_button, "clicked", G_CALLBACK(load_text), widgets);
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_text), widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Load initial text
    char buffer[MAX_VALUE_LEN];
    if (read_text_value(buffer, sizeof(buffer))) {
        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        gtk_text_buffer_set_text(text_buffer, buffer, -1);
        update_status(status_label, "Loaded successfully", FALSE);
    } else {
        update_status(status_label, "No config found. Create text and click Save.", TRUE);
    }
    
    // Show all widgets
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("com.example.configapp", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
