#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 100

char *trim(char *str) {
    char *end = NULL;

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    end[1] = '\0';
    return str;
}

int parse_yaml_line(const char *line, ConfigEntry *entry) {
    char buffer[MAX_LINE_LEN];
    char *colon_pos = NULL;
    char *key_start = NULL;
    char *value_start = NULL;

    if (!line || !entry) {
        return 0;
    }

    // Use memcpy with explicit bounds check instead of strncpy
    size_t len = strlen(line);
    if (len >= MAX_LINE_LEN) {
        len = MAX_LINE_LEN - 1;
    }
    memcpy(buffer, line, len);
    buffer[len] = '\0';

    colon_pos = strchr(buffer, ':');
    if (!colon_pos) {
        return 0;
    }

    *colon_pos = '\0';
    key_start = trim(buffer);
    value_start = trim(colon_pos + 1);

    // Use memcpy with bounds check
    len = strlen(key_start);
    if (len >= MAX_KEY_LEN) {
        len = MAX_KEY_LEN - 1;
    }
    memcpy(entry->key, key_start, len);
    entry->key[len] = '\0';

    len = strlen(value_start);
    if (len >= MAX_VALUE_LEN) {
        len = MAX_VALUE_LEN - 1;
    }
    memcpy(entry->value, value_start, len);
    entry->value[len] = '\0';

    return 1;
}

int read_config(const char *var_name, char *output, size_t output_size) {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) {
        return 0;
    }

    char line[MAX_LINE_LEN];
    ConfigEntry entry;
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        if (parse_yaml_line(line, &entry)) {
            if (strcmp(entry.key, var_name) == 0) {
                size_t len = strlen(entry.value);
                if (len >= output_size) {
                    len = output_size - 1;
                }
                memcpy(output, entry.value, len);
                output[len] = '\0';
                found = 1;
                break;
            }
        }
    }

    fclose(file);
    return found;
}

int write_config(const char *var_name, const char *value) {
    FILE *file = fopen(CONFIG_FILE, "r");
    char lines[MAX_LINES][MAX_LINE_LEN];
    int line_count = 0;
    int var_found = 0;

    if (file) {
        while (fgets(lines[line_count], MAX_LINE_LEN, file) && line_count < MAX_LINES - 1) {
            char temp_line[MAX_LINE_LEN];
            size_t len = strlen(lines[line_count]);
            if (len >= MAX_LINE_LEN) {
                len = MAX_LINE_LEN - 1;
            }
            memcpy(temp_line, lines[line_count], len);
            temp_line[len] = '\0';

            ConfigEntry entry;
            if (parse_yaml_line(temp_line, &entry)) {
                if (strcmp(entry.key, var_name) == 0) {
                    snprintf(lines[line_count], MAX_LINE_LEN, "%s: %s\n", var_name, value);
                    var_found = 1;
                }
            }
            line_count++;
        }
        fclose(file);
    }

    if (!var_found) {
        snprintf(lines[line_count], MAX_LINE_LEN, "%s: %s\n", var_name, value);
        line_count++;
    }

    file = fopen(CONFIG_FILE, "w");
    if (!file) {
        return 0;
    }

    for (int i = 0; i < line_count; i++) {
        fprintf(file, "%s", lines[i]);
    }

    fclose(file);
    return 1;
}

int read_text_value(char *output, size_t output_size) {
    return read_config("text", output, output_size);
}

int update_text_value(const char *new_value) {
    return write_config("text", new_value);
}
