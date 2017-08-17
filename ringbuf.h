/*
 **************************************************************************************
 *       Filename:  ringbuf.h
 *    Description:   header file
 *
 *        Version:  1.0
 *        Created:  2017-08-11 15:03:33
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#ifndef RINGBUF_H_INCLUDED
#define RINGBUF_H_INCLUDED

#include <errno.h>
struct rb_handlt_t;

struct rb_handle_t* rb_create(unsigned int size);
void rb_delete(struct rb_handle_t* h);
void rb_reset(struct rb_handle_t* h);

int rb_write(struct rb_handle_t* h, void* buf, unsigned int size);
int rb_read(struct rb_handle_t* h, void* buf, unsigned int size);

#endif /*RINGBUF_H_INCLUDED*/

/********************************** END **********************************************/

