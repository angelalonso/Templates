#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stddef.h>

int config_init(void);
int config_read(const char *key, char *output, size_t output_size);
int config_write(const char *key, const char *value);
int config_read_text(char *output, size_t output_size);
int config_write_text(const char *value);

#endif
