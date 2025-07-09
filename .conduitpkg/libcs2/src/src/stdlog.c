#include "../include/stdlog.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/printf.h"

stdlog_t stdlog_interface;

void config_klog_interface() {
	stdlog_interface.get_max_len = klog_max_len;
	stdlog_interface.get_len = klog_len;
	stdlog_interface.read_log = klog_read;
	stdlog_interface.clean_logs = klog_clean;
	stdlog_interface.append = klog;
	stdlog_interface.init = klog_init;
	stdlog_interface.get_max_log_len = klog_max_log_len;
}

void klog(const char *str) {
	if (strlen(str) > stdlog_interface.max_log_len) return;
	if (stdlog_interface.len > stdlog_interface.max_len) return;
	strcpy((char*)stdlog_interface.base[stdlog_interface.len], str);
	stdlog_interface.len++;
}

void klog_clean() {
	stdlog_interface.len = 0;
	memset(stdlog_interface.base[0], 32, stdlog_interface.max_log_len);
}

const char *klog_read(size_t index) {
	return (const char*)stdlog_interface.base[index];
}

size_t klog_len() {
	return stdlog_interface.len;
}

size_t klog_max_len() {
	return stdlog_interface.max_len;
}

size_t klog_max_log_len() {
	return stdlog_interface.max_log_len;
}

void klog_init(void **base, size_t max_len, size_t max_log_len) {
	stdlog_interface.base = base;
	stdlog_interface.max_len = max_len;
	stdlog_interface.max_log_len = max_log_len;
	stdlog_interface.len = 0;
}
