#include "gui_main.h"
#include <gtk/gtk.h>

// Application activation callback
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = create_main_window(app);
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status = 0;

    app = gtk_application_new("com.example.configapp", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
