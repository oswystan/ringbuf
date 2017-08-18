## WHY

This is a ring buffer implementation. It can be used in multi-thread environment.

## HOWTO

```
int total = 1024;
struct rb_handle_t* h = rb_create(total);
char* p = malloc(total);
rb_write(h, p, total);
rb_read(h, p, total);

rb_delete(h);
```
