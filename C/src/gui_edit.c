#include "gui_edit.h"
#include "gui_common.h"
#include <string.h>
#include <stdlib.h>

// Structure for edit dialog data
typedef struct {
    GtkWidget *dialog;
    GtkWidget *text_view;
    MainWindowWidgets *main_widgets;
} EditDialogData;

// Save and close dialog
void on_edit_save_clicked(GtkWidget *button, gpointer data) {
    EditDialogData *dialog_data = (EditDialogData *)data;
    GtkTextBuffer *text_buffer;
    GtkTextIter start, end;
    char *new_text;
    
    if (!dialog_data || !dialog_data->text_view) {
        g_printerr("ERROR: Invalid dialog data in on_edit_save_clicked\n");
        return;
    }
    
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dialog_data->text_view));
    if (!text_buffer) {
        g_printerr("ERROR: Could not get text buffer\n");
        return;
    }
    
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    new_text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);
    
    if (new_text) {
        // Update the config file
        if (dialog_data->main_widgets) {
            if (save_config_text(new_text, dialog_data->main_widgets->status_label)) {
                // Update main window display
                update_main_display(dialog_data->main_widgets);
            }
        } else {
            // No main window reference, just save
            update_text_value(new_text);
        }
        g_free(new_text);
    }
    
    // Destroy the dialog (this will trigger on_edit_dialog_destroy)
    if (dialog_data->dialog) {
        gtk_widget_destroy(dialog_data->dialog);
    }
}

// Cancel and close dialog
void on_edit_cancel_clicked(GtkWidget *button, gpointer data) {
    EditDialogData *dialog_data = (EditDialogData *)data;
    
    if (dialog_data && dialog_data->dialog) {
        gtk_widget_destroy(dialog_data->dialog);
    }
}

// Clean up dialog data when dialog is destroyed
void on_edit_dialog_destroy(GtkWidget *dialog, gpointer data) {
    EditDialogData *dialog_data = (EditDialogData *)data;
    
    if (dialog_data) {
        // main_widgets is owned by main window, don't free it here
        g_free(dialog_data);
    }
}

// Create and show edit dialog
void create_edit_dialog(GtkWidget *parent_window, MainWindowWidgets *main_widgets) {
    EditDialogData *dialog_data;
    GtkWidget *dialog;
    GtkWidget *vbox;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;
    GtkWidget *button_box;
    GtkWidget *save_button;
    GtkWidget *cancel_button;
    GtkWidget *label;
    GtkWidget *content_area;
    GtkWidget *separator;
    char *current_text;
    
    if (!parent_window) return;
    
    // Create dialog
    dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Edit Configuration");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 450);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
    
    // Create dialog data structure
    dialog_data = g_new0(EditDialogData, 1);
    dialog_data->dialog = dialog;
    dialog_data->main_widgets = main_widgets;
    
    // Get current text using common function
    current_text = load_config_text();
    
    // Get dialog content area and remove default button box
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);
    
    // Create vertical box (this will fill the content area)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);
    gtk_widget_set_vexpand(vbox, TRUE);
    gtk_widget_set_hexpand(vbox, TRUE);
    
    // Add instruction label at top
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), 
                         "<span weight='bold' size='large'>Edit Configuration</span>\n\n"
                         "<span>Modify the text below and click Save when done.</span>");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    
    // Add separator
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 5);
    
    // Create scrolled window for text view (THIS MUST EXPAND)
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);
    
    // Set expansion - THIS IS THE KEY FIX
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Create text view
    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 5);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Set current text
    if (current_text) {
        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        gtk_text_buffer_set_text(text_buffer, current_text, -1);
        g_free(current_text);
    }
    
    dialog_data->text_view = text_view;
    
    // Add separator above buttons
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 5);
    
    // Create button box at bottom (centered like main window)
    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);
    
    // Create buttons
    save_button = gtk_button_new_with_label("Save");
    cancel_button = gtk_button_new_with_label("Cancel");
    gtk_container_add(GTK_CONTAINER(button_box), save_button);
    gtk_container_add(GTK_CONTAINER(button_box), cancel_button);
    
    // Style buttons to match main window
    gtk_widget_set_size_request(save_button, 100, 35);
    gtk_widget_set_size_request(cancel_button, 100, 35);
    
    // Set button spacing
    gtk_box_set_spacing(GTK_BOX(button_box), 20);
    
    // Connect button signals
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_edit_save_clicked), dialog_data);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_edit_cancel_clicked), dialog_data);
    g_signal_connect(dialog, "destroy", G_CALLBACK(on_edit_dialog_destroy), dialog_data);
    
    // Show all widgets
    gtk_widget_show_all(dialog);
}
