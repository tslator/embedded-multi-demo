#ifndef NULLPTR_COMPAT_H
#define NULLPTR_COMPAT_H

#if !defined(__cplusplus)
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L)
#include <stddef.h>
#ifndef nullptr
#define nullptr NULL
#endif
#endif
#endif

#endif