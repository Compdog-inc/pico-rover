#ifndef _KERNEL_H
#define _KERNEL_H

#ifndef __ASSEMBLER__

#ifdef __cplusplus
extern "C" void rtos_panic(const char *fmt, ...);
#else
extern void rtos_panic(const char *fmt, ...);
#endif

#endif

#endif