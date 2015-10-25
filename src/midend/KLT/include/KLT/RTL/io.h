
#ifndef __KLT_RTL_IO_H__
#define __KLT_RTL_IO_H__

#include <stdlib.h>

char * klt_getenv(const char * variable, const char * def);
char * klt_getenv_or_fail(const char * variable);

size_t klt_getenv_size_t(const char * variable, const size_t def);

void klt_init_io(void);
void klt_exit_io(void);

void klt_info(size_t verbose, const char * msg, ...);
void klt_log(const char * msg, ...);
void klt_warning(const char * msg, ...);
void klt_error(const char * msg, ...);
void klt_fatal(const char * msg, ...);

char * klt_read_file(const char * filename);

#endif /* __KLT_RTL_IO_H__ */

