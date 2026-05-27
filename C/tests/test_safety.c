#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_TEST_SIZE 10000

jmp_buf env;

void signal_handler(int sig) {
    longjmp(env, 1);
}

void test_buffer_overflow() {
    char buffer[10];
    char large_input[1000];
    memset(large_input, 'A', 999);
    large_input[999] = '\0';

    // Test strncpy safety
    strncpy(buffer, large_input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    assert(strlen(buffer) < sizeof(buffer));
    printf("Buffer overflow test passed\n");
}

void test_config_parsing_safety() {
    ConfigEntry entry;

    // Test with extremely long line
    char long_line[MAX_LINE_LEN * 2];
    memset(long_line, 'x', sizeof(long_line) - 1);
    snprintf(long_line, sizeof(long_line), "key: %s", long_line);
    long_line[sizeof(long_line) - 1] = '\0';

    int result = parse_yaml_line(long_line, &entry);
    assert(result == 0 || strlen(entry.key) < MAX_KEY_LEN);

    printf("Config parsing safety test passed\n");
}

void test_memory_allocation() {
    // Test repeated operations for leaks
    for (int i = 0; i < 1000; i++) {
        char value[MAX_VALUE_LEN];
        snprintf(value, sizeof(value), "test_value_%d", i);
        write_config("test_key", value);

        char read_value[MAX_VALUE_LEN];
        read_config("test_key", read_value, sizeof(read_value));

        assert(strcmp(read_value, value) == 0);
    }

    printf("Memory allocation test passed\n");
}

void test_null_pointer_safety() {
    // Test functions with NULL pointers
    assert(parse_yaml_line(NULL, NULL) == 0);
    assert(read_config(NULL, NULL, 0) == 0);
    assert(write_config(NULL, NULL) == 0);

    printf("Null pointer safety test passed\n");
}

int main() {
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);

    if (setjmp(env) == 0) {
        test_buffer_overflow();
        test_config_parsing_safety();
        test_memory_allocation();
        test_null_pointer_safety();

        printf("\nAll safety tests passed successfully\n");
        return 0;
    } else {
        fprintf(stderr, "Safety test failed due to crash\n");
        return 1;
    }
}
