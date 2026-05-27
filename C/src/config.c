#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim(char *str) {
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';
    return str;
}

int parse_yaml_line(const char *line, ConfigEntry *entry) {
    char buffer[MAX_LINE_LEN];
    char *colon_pos;
    char *key_start;
    char *value_start;

    if (!line || !entry)
        return 0;

    strncpy(buffer, line, MAX_LINE_LEN - 1);
    buffer[MAX_LINE_LEN - 1] = '\0';

    colon_pos = strchr(buffer, ':');
    if (!colon_pos)
        return 0;

    *colon_pos = '\0';
    key_start = trim(buffer);
    value_start = trim(colon_pos + 1);

    strncpy(entry->key, key_start, MAX_KEY_LEN - 1);
    entry->key[MAX_KEY_LEN - 1] = '\0';
    strncpy(entry->value, value_start, MAX_VALUE_LEN - 1);
    entry->value[MAX_VALUE_LEN - 1] = '\0';

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
                strncpy(output, entry.value, output_size - 1);
                output[output_size - 1] = '\0';
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
    char lines[100][MAX_LINE_LEN];
    int line_count = 0;
    int var_found = 0;

    if (file) {
        while (fgets(lines[line_count], MAX_LINE_LEN, file) && line_count < 100) {
            char temp_line[MAX_LINE_LEN];
            strncpy(temp_line, lines[line_count], MAX_LINE_LEN - 1);
            temp_line[MAX_LINE_LEN - 1] = '\0';

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
