#ifndef __UTILS_H__
#define __UTILS_H__

static inline void barrier() {
    __asm__ volatile ("" : : : "memory");
}

#endif /* __UTILS_H__ */