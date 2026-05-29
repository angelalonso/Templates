// main.c
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include "backend/config_manager.h"
#include "backend/webkit_handler.h"

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Configuration Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));
    
    // Setup custom URI scheme handler
    setup_webkit_handler(web_view);
    
    // Load the app from custom scheme
    webkit_web_view_load_uri(web_view, "app:///index.html");
    
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    // Initialize configuration
    config_init();
    
    GtkApplication *app = gtk_application_new("com.example.configapp", 0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
