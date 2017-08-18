/*
 **************************************************************************************
 *       Filename:  main.c
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2017-08-11 15:04:01
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ringbuf.h"

#define rb_assert(x) \
    if (!(x)) { \
        printf("(%s,%d)ERROR: %s failed\n", __FILE__, __LINE__, #x); \
        return 1; \
    }
int test_rb() {
    int total = 4;
    uint64_t r = 0;
    uint64_t w = 0;
    struct rb_handle_t* h = rb_create(total);
    char* p = malloc(total+1);
    char* c = malloc(total+1);
    char* t = malloc(total+1);
    rb_assert(h != NULL);
    rb_assert(p != NULL);
    rb_assert(c != NULL);
    rb_assert(t != NULL);

    memset(p, 0x00, total+1);
    memset(c, 0x00, total+1);
    memset(p, 'A', total);
    p[total] = '\0';

    /* usebuf and getbuf */
    rb_assert(NULL == rb_getbuf(NULL));
    rb_assert(NULL != rb_getbuf(h));
    rb_assert(-EINVAL == rb_usebuf(h, NULL, 0));
    rb_assert(-EINVAL == rb_usebuf(h, c, 0));
    rb_assert(0 == rb_usebuf(h, c, total));
    rb_assert(0 == rb_usebuf(h, t, total));

    /* write and read error case */
    rb_assert(-EINVAL == rb_write(NULL, NULL, 1));
    rb_assert(-EINVAL == rb_write(h, NULL, 1));
    rb_assert(-EINVAL == rb_write(h, p, total+1));
    rb_assert(-EINVAL == rb_write(h, p, 0));
    rb_assert(-EINVAL == rb_read(NULL, p, 1));
    rb_assert(-EINVAL == rb_read(h, NULL, 1));
    rb_assert(-EINVAL == rb_read(h, p, total+1));
    rb_assert(-EINVAL == rb_read(h, p, 0));
    rb_assert(-EINVAL == rb_getpos(NULL, NULL, NULL));

    rb_assert(0 == rb_write(h, p, total-1));
    rb_assert(0 == rb_write(h, p, 1));
    rb_assert(-EOVERFLOW == rb_write(h, p, 1));

    rb_assert(0 == rb_read(h, p, total-1));
    rb_assert(0 == rb_read(h, p, 1));
    rb_assert(-EPERM == rb_read(h, p, 1));

    rb_assert(0 == rb_getpos(h, NULL, NULL));
    rb_assert(0 == rb_getpos(h, &r, NULL));
    rb_assert(0 == rb_getpos(h, &r, &w));

    /* check rp and wp */
    rb_reset(h);
    rb_assert(0 == rb_getpos(h, &r, &w));
    rb_assert(0 == r && 0 == w);
    rb_assert(-EPERM == rb_read(h, p, 1));
    rb_assert(0 == rb_write(h, p, total));
    rb_assert(0 == rb_getpos(h, &r, &w));
    rb_assert(0 == r && w == total);
    rb_assert(0 == rb_read(h, p, 2));
    rb_assert(0 == rb_getpos(h, &r, &w));
    rb_assert(2 == r && w == total);
    rb_assert(0 == rb_write(h, p, 1));
    rb_assert(0 == rb_getpos(h, &r, &w));
    rb_assert(2 == r && w == total+1);
    rb_assert(0 == rb_read(h, p, 3));
    rb_assert(0 == rb_getpos(h, &r, &w));
    rb_assert(r == w && r == (total+1));

    /* check content */
    rb_reset(h);
    sprintf(p, "1234");
    memset(c, 0x00, total+1);
    rb_assert(0 == rb_write(h, p, total));
    rb_assert(0 == rb_read(h, c, total));
    rb_assert(0 == strcmp(p, c));
    sprintf(p, "567");
    memset(c, 0x00, total+1);
    rb_assert(0 == rb_write(h, p, 3));
    rb_assert(0 == rb_read(h, c, 3));
    rb_assert(0 == strcmp(p, c));
    sprintf(p, "89");
    memset(c, 0x00, total+1);
    rb_assert(0 == rb_write(h, p, 2));
    rb_assert(0 == rb_read(h, c, 2));
    rb_assert(0 == strcmp(p, c));
    sprintf(p, "0");
    memset(c, 0x00, total+1);
    rb_assert(0 == rb_write(h, p, 1));
    rb_assert(0 == rb_read(h, c, 1));
    rb_assert(0 == strcmp(p, c));

    rb_delete(h);
    return 0;
}

int main(int argc, const char *argv[]) {
    if (0 != test_rb()) {
        printf("fail to test rb\n");
        return -1;
    } else {
        printf("pass\n");
        return 0;
    }
}

/********************************** END **********************************************/

