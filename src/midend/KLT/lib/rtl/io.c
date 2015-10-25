
#include "KLT/RTL/io.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <assert.h>

char * klt_getenv(const char * variable, const char * def) {
  char * env = getenv(variable);
  char * res = NULL;
  if (env != NULL && env[0] != '\0') {
    size_t size = strlen(env) + 1;
    res = malloc(size);
    memset(res, 0, size);
    strcat(res, env);
  }
  else if (def != NULL) {
    size_t size = strlen(def) + 1;
    res = malloc(size);
    memset(res, 0, size);
    strcat(res, def);
  }
  return res;
}

char * klt_getenv_or_fail(const char * variable) {
  char * res = klt_getenv(variable, NULL);
  if (res == NULL)
    klt_fatal("Environment variable %s need to be defined.", variable);
  return res;
}

size_t klt_getenv_size_t(const char * variable, const size_t def) {
  char * env = getenv(variable);
  size_t res = def;

  if (env != NULL && env[0] != '\0')
    res = atoi(env);

  return res;
}

FILE * klt_stream_log = NULL;

FILE * klt_stream_info = NULL;
FILE * klt_stream_warning = NULL;
FILE * klt_stream_error = NULL;

size_t klt_verbose = 0;

FILE * klt_open_stream(const char * env_var, FILE * def);
FILE * klt_open_stream(const char * env_var, FILE * def) {
  char * file = klt_getenv(env_var, NULL);

  FILE * res = NULL;
       if (file == NULL)                              res = def;
  else if (file[0] == '\0')                           res = NULL;
  else if (strcmp((const char *)file, "stdout") == 0) res = stdout;
  else if (strcmp((const char *)file, "stderr") == 0) res = stderr;
  else                                                res = fopen(file, "w");

  return res;
}

void klt_init_io(void) {
  klt_stream_log = klt_open_stream("KLT_STREAM_LOG", NULL);
  klt_stream_info = klt_open_stream("KLT_STREAM_INFO", stderr);
  klt_stream_warning = klt_open_stream("KLT_STREAM_WARNING", stderr);
  klt_stream_error = klt_open_stream("KLT_STREAM_ERROR", stderr);

  klt_verbose = klt_getenv_size_t("KLT_VERBOSE", 0);

  klt_info(3, "KLT I/O initialized.");
}

void klt_exit_io(void) {
  if (klt_stream_log     != NULL && klt_stream_log     != stdout && klt_stream_log     != stderr) fclose(klt_stream_log    );
  if (klt_stream_info    != NULL && klt_stream_info    != stdout && klt_stream_info    != stderr) fclose(klt_stream_info   );
  if (klt_stream_warning != NULL && klt_stream_warning != stdout && klt_stream_warning != stderr) fclose(klt_stream_warning);
  if (klt_stream_error   != NULL && klt_stream_error   != stdout && klt_stream_error   != stderr) fclose(klt_stream_error  );  
}

void klt_vprintf(FILE * stream, const char * tag, const char * msg, va_list argp);
void klt_vprintf(FILE * stream, const char * tag, const char * msg, va_list argp) {
  fprintf(stream, tag);
  vfprintf(stream, msg, argp);
  fprintf(stream, "\n");
}

void klt_log(const char * msg, ...) {
  va_list argp;
  va_start(argp, msg);
  if (klt_stream_log != NULL)
    klt_vprintf(klt_stream_log, "[Log    ] ", msg, argp);
  va_end(argp);
}

void klt_info(size_t verbose, const char * msg, ...) {
  va_list argp;
  va_start(argp, msg);
  if (verbose <= klt_verbose && klt_stream_info != NULL)
    klt_vprintf(klt_stream_info, "[Info   ] ", msg, argp);
  if (verbose <= klt_verbose && klt_stream_log  != NULL)
    klt_vprintf(klt_stream_log , "[Info   ] ", msg, argp);
  va_end(argp);
}

void klt_warning(const char * msg, ...) {
  va_list argp;
  va_start(argp, msg);
  if (klt_stream_warning != NULL)
    klt_vprintf(klt_stream_warning, "[Warning] ", msg, argp);
  if (klt_stream_log != NULL)
    klt_vprintf(klt_stream_log, "[Warning] ", msg, argp);
  va_end(argp);
}

void klt_error(const char * msg, ...) {
  va_list argp;
  va_start(argp, msg);
  if (klt_stream_warning != NULL)
    klt_vprintf(klt_stream_error, "[Error  ] ", msg, argp);
  if (klt_stream_log != NULL)
    klt_vprintf(klt_stream_log, "[Error  ] ", msg, argp);
  va_end(argp);
}

void klt_fatal(const char * msg, ...) {
  va_list argp;
  va_start(argp, msg);
  if (klt_stream_warning != NULL)
    klt_vprintf(klt_stream_error, "[Fatal  ] ", msg, argp);
  if (klt_stream_log != NULL)
    klt_vprintf(klt_stream_log, "[Fatal  ] ", msg, argp);
  va_end(argp);
  exit(EXIT_FAILURE);
}

char * klt_read_file(const char * filename) {

   FILE * fp;
   int err;
   int size;

   char * source;

   fp = fopen(filename, "rb");
   if(fp == NULL) {
      klt_error("Could not open kernel file: %s", filename);
      return NULL;
   }
   
   err = fseek(fp, 0, SEEK_END);
   if(err != 0) {
      klt_error("Seeking to end of file: %s", filename);
      return NULL;
   }

   size = ftell(fp);
   if(size < 0) {
      klt_error("Getting position in file: %s", filename);
      return NULL;
   }

   err = fseek(fp, 0, SEEK_SET);
   if(err != 0) {
      klt_error("Seeking to start of file: %s", filename);
      return NULL;
   }

   source = (char*)malloc(size+1);
   if(source == NULL) {
      klt_fatal("Allocating %d bytes to read file: %s", size+1, filename);
   }

   err = fread(source, 1, size, fp);
   if(err != size) {
      klt_error("only read %d/%d bytes in file: %s", err, size, filename);
      return NULL;
   }

   source[size] = '\0';

   return source;
}

