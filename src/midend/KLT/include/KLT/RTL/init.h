
#ifndef __KLT_RTL_INIT_H__
#define __KLT_RTL_INIT_H__

#include "KLT/RTL/definition.h"

void klt_init() __attribute__((constructor));

void klt_exit() __attribute__((destructor));

#endif /* __KLT_RTL_INIT_H__ */

