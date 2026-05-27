#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 4096
#define MAX_LINE_LEN 4096
#define CONFIG_FILE "cfg.yml"

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} ConfigEntry;

char *trim(char *str);
int parse_yaml_line(const char *line, ConfigEntry *entry);
int read_config(const char *var_name, char *output, size_t output_size);
int write_config(const char *var_name, const char *value);
int read_text_value(char *output, size_t output_size);
int update_text_value(const char *new_value);

#endif
