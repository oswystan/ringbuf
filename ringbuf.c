/*
 **************************************************************************************
 *       Filename:  ringbuf.c
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2017-08-11 15:03:38
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "ringbuf.h"

#ifndef NULL
#define NULL 0
#endif

struct rb_handle_t {
    void* ptr;
    unsigned int size;
    unsigned int owned;
    uint64_t wp;
    uint64_t rp;
    pthread_mutex_t mutex;
};

struct rb_handle_t* rb_create(unsigned int size) {
    if (0 == size) {
        return NULL;
    }

    struct rb_handle_t* h = (struct rb_handle_t*)malloc(sizeof(struct rb_handle_t));
    void* ptr = malloc(size);
    if (!h || !ptr) {
        goto EXIT;
    }
    memset(h, 0x00, sizeof(*h));
    memset(ptr, 0x00, size);
    h->ptr = ptr;
    h->size = size;
    h->owned = 1;
    pthread_mutex_init(&h->mutex, NULL);
    return h;

EXIT:
    if(h) free(h);
    if(ptr) free(ptr);
    return NULL;
}

void rb_delete(struct rb_handle_t* h) {
    if (h) {
        pthread_mutex_lock(&h->mutex);
        if (h->ptr) {
            free(h->ptr);
        }
        pthread_mutex_unlock(&h->mutex);
        pthread_mutex_destroy(&h->mutex);
        free(h);
    }
}

void rb_reset(struct rb_handle_t* h) {
    if (h) {
        pthread_mutex_lock(&h->mutex);
        if (h->ptr) {
            memset(h->ptr, 0x00, h->size);
        }
        h->wp = h->rp = 0;
        pthread_mutex_unlock(&h->mutex);
    }
}

int rb_usebuf(struct rb_handle_t* h, void* buf, unsigned int size) {
    if (!h || !buf || size == 0) {
        return -EINVAL;
    }

    pthread_mutex_lock(&h->mutex);
    if (h->ptr && h->owned) {
        free(h->ptr);
    }
    h->ptr = buf;
    h->owned = 0;
    h->wp = h->rp = 0;
    pthread_mutex_unlock(&h->mutex);
    return 0;
}

void* rb_getbuf(struct rb_handle_t* h) {
    void* ptr = NULL;
    if (h) {
        pthread_mutex_lock(&h->mutex);
        ptr = h->ptr;
        pthread_mutex_unlock(&h->mutex);
    }
    return ptr;
}

int rb_write(struct rb_handle_t* h, void* buf, unsigned int size) {
    if (!h || !buf || size > h->size || size == 0) {
        return -EINVAL;
    }

    int ret = 0;
    pthread_mutex_lock(&h->mutex);
    if (h->wp-h->rp + size > h->size) {
        ret = -EOVERFLOW;
    } else {
        void* p = h->ptr + (h->wp % h->size);
        if (p - h->ptr + size < h->size) {
            /* case 1: enough space to store data after wp */
            memcpy(p, buf, size);
        } else {
            /* case 2: NOT enough space to store data after wp */
            unsigned int sz = h->size - (p - h->ptr);
            memcpy(p, buf, sz);
            memcpy(h->ptr, buf+sz, size-sz);
        }
        h->wp += size;
    }
    pthread_mutex_unlock(&h->mutex);
    return ret;
}

int rb_read(struct rb_handle_t* h, void* buf, unsigned int size) {
    if (!h || !buf || size > h->size || size == 0) {
        return -EINVAL;
    }

    int ret = 0;
    pthread_mutex_lock(&h->mutex);
    if (h->wp - h->rp < size) {
        ret = -EPERM;
    } else {
        void* p = h->ptr + (h->rp % h->size);
        if (p - h->ptr + size < h->size) {
            memcpy(buf, p, size);
        } else {
            unsigned int sz = h->size - (p - h->ptr);
            memcpy(buf, p, sz);
            memcpy(buf+sz, h->ptr, size-sz);
        }
        h->rp += size;
    }
    pthread_mutex_unlock(&h->mutex);

    return ret;
}

int rb_getpos(struct rb_handle_t* h, uint64_t* r, uint64_t* w) {
    if (!h) return -EINVAL;
    if (r) *r = h->rp;
    if (w) *w = h->wp;
    return 0;
}

/********************************** END **********************************************/

