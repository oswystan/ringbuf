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
        exit(1); \
    }
int test_rb() {
    int total = 4;
    struct rb_handle_t* h = rb_create(total);
    char* p = malloc(total+1);
    memset(p, 'A', total);
    p[total] = '\0';

    rb_assert(h != NULL);
    rb_assert(p != NULL);

    /* write and read error case */
    rb_assert( -EINVAL == rb_write(NULL, NULL, 1));
    rb_assert( -EINVAL == rb_write(h, NULL, 1));
    rb_assert( -EINVAL == rb_write(h, p, total+1));
    rb_assert( -EINVAL == rb_write(h, p, 0));
    rb_assert( -EINVAL == rb_read(NULL, p, 1));
    rb_assert( -EINVAL == rb_read(h, NULL, 1));
    rb_assert( -EINVAL == rb_read(h, p, total+1));
    rb_assert( -EINVAL == rb_read(h, p, 0));

    rb_assert( 0 == rb_write(h, p, total-1));
    rb_assert( 0 == rb_write(h, p, 1));
    rb_assert( -EOVERFLOW == rb_write(h, p, 1));

    rb_assert( 0 == rb_read(h, p, total-1));
    rb_assert( 0 == rb_read(h, p, 1));
    rb_assert( -EPERM == rb_read(h, p, 1));
    rb_reset(h);

    rb_assert( 0 == rb_write(h, p, 2));
    memset(p, 0x00, total+1);
    rb_assert( 0 == rb_read(h, p, 2));
    printf("%s\n", p);

    rb_reset(h);
    rb_delete(h);
    return 0;
}

int main(int argc, const char *argv[]) {
    if (0 != test_rb()) {
        printf("fail to test rb\n");
        return -1;
    } else {
        printf("pass\n");
    }
    return 0;
}

/********************************** END **********************************************/

