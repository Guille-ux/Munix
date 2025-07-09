#ifndef STDLOG_H
#define STDLOG_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	const char* (*read_log)(size_t index);
	size_t (*get_len)(void);
	size_t (*get_max_len)(void);
	void (*clean_logs)(void);
	void (*append)(const char *str);
	void (*init)(void **base, size_t max_len, size_t max_log_len);
	size_t max_len;
	size_t len;
	size_t max_log_len;
	size_t (*get_max_log_len)(void);
	void **base;
} stdlog_t;

extern stdlog_t stdlog_interface;

void klog(const char *str);
void klog_clean();
const char *klog_read(size_t index);
size_t klog_len();
size_t klog_max_len();
size_t klog_max_log_len();
void klog_init(void **base, size_t max_len, size_t max_log_len);
void config_klog_interface();

#endif
