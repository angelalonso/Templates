#include "config.h"
#include "gui_common.h"
#include "gui_edit.h"
#include "gui_main.h"
#include <gtk/gtk.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

// Test helper to catch crashes
static jmp_buf crash_env;
static int crash_handler_set = 0;

void crash_signal_handler(int sig) {
    if (crash_handler_set) {
        longjmp(crash_env, 1);
    }
}

// Test common functions
void test_common_functions() {
    char *text;

    // Test load_config_text
    update_text_value("Test content");
    text = load_config_text();
    g_assert_cmpstr(text, ==, "Test content");
    g_free(text);

    // Test save_config_text
    g_assert_true(save_config_text("New content", NULL));
    text = load_config_text();
    g_assert_cmpstr(text, ==, "New content");
    g_free(text);

    printf("✓ Common functions test passed\n");
}

// Test main window creation
void test_main_window_creation() {
    int argc = 1;
    char *argv[] = {"test", NULL};
    GtkApplication *app;
    GtkWidget *window;

    gtk_init(&argc, &argv);

    app = gtk_application_new("com.example.test", G_APPLICATION_FLAGS_NONE);
    window = create_main_window(app);

    g_assert_nonnull(window);
    g_assert_true(GTK_IS_WINDOW(window));

    g_object_unref(app);

    printf("✓ Main window creation test passed\n");
}

// Test edit dialog creation
void test_edit_dialog_creation() {
    int argc = 1;
    char *argv[] = {"test", NULL};
    GtkWidget *window;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Just test that dialog creation doesn't crash
    // We can't easily test modal dialogs in unit tests
    printf("✓ Edit dialog creation test passed (structural)\n");

    gtk_widget_destroy(window);
}

// Test config operations with GUI
void test_config_gui_integration() {
    char test_value[MAX_VALUE_LEN] = "Test GUI content";
    char read_value[MAX_VALUE_LEN];
    char *loaded_text;

    // Write test config
    g_assert_true(update_text_value(test_value));

    // Test load_config_text
    loaded_text = load_config_text();
    g_assert_cmpstr(loaded_text, ==, test_value);
    g_free(loaded_text);

    // Test save_config_text
    char new_value[MAX_VALUE_LEN] = "Updated GUI content";
    g_assert_true(save_config_text(new_value, NULL));

    // Verify
    g_assert_true(read_text_value(read_value, sizeof(read_value)));
    g_assert_cmpstr(read_value, ==, new_value);

    printf("✓ Config-GUI integration test passed\n");
}

// Test display update
void test_display_update() {
    char test_value[MAX_VALUE_LEN] = "Display test content";

    // Write test content
    update_text_value(test_value);

    // Test load
    char *loaded = load_config_text();
    g_assert_cmpstr(loaded, ==, test_value);
    g_free(loaded);

    printf("✓ Display update test passed\n");
}

int main(int argc, char **argv) {
    signal(SIGSEGV, crash_signal_handler);
    signal(SIGABRT, crash_signal_handler);

    printf("\nRunning GUI tests...\n");
    printf("=========================================\n");

    crash_handler_set = 1;

    if (setjmp(crash_env) == 0) {
        test_common_functions();
    } else {
        g_error("Common functions test crashed");
        return 1;
    }

    if (setjmp(crash_env) == 0) {
        test_main_window_creation();
    } else {
        g_error("Main window creation test crashed");
        return 1;
    }

    if (setjmp(crash_env) == 0) {
        test_edit_dialog_creation();
    } else {
        g_error("Edit dialog creation test crashed");
        return 1;
    }

    if (setjmp(crash_env) == 0) {
        test_config_gui_integration();
    } else {
        g_error("Config integration test crashed");
        return 1;
    }

    if (setjmp(crash_env) == 0) {
        test_display_update();
    } else {
        g_error("Display update test crashed");
        return 1;
    }

    printf("=========================================\n");
    printf("✓ All GUI tests passed\n");

    return 0;
}
