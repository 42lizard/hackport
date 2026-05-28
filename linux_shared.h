#ifndef LINUX_SHARED_H
#define LINUX_SHARED_H

#include <stdio.h>

int  hack_drop_privs(void);
int  hack_open_read(const char *);
int  hack_open_write(const char *, int);
int  hack_open_excl(const char *, int);
int  hack_open_rw_create(const char *, int);
FILE *hack_record_lock(int *);
FILE *hack_record_rewrite(int, const char *);
void hack_record_unlock(int);

#endif /* LINUX_SHARED_H */
