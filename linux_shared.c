#include <sys/file.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hack.h"
#include "linux_shared.h"

static int safe_name(const char *);
static int checked_open(const char *, int, int);

int
hack_drop_privs(void)
{
	gid_t gid;
	uid_t uid;

	gid = getgid();
	uid = getuid();
	if(setresgid(gid, gid, gid) == -1)
		return(-1);
	if(setresuid(uid, uid, uid) == -1)
		return(-1);
	return(0);
}

int
hack_open_read(const char *name)
{
	return(checked_open(name, O_RDONLY, 0));
}

int
hack_open_write(const char *name, int mode)
{
	return(checked_open(name, O_CREAT | O_TRUNC | O_WRONLY, mode));
}

int
hack_open_excl(const char *name, int mode)
{
	return(checked_open(name, O_CREAT | O_EXCL | O_WRONLY, mode));
}

int
hack_open_rw_create(const char *name, int mode)
{
	return(checked_open(name, O_RDWR | O_CREAT, mode));
}

FILE *
hack_record_lock(int *fdp)
{
	int fd;
	FILE *fp;

	fd = checked_open(RECORD, O_RDWR | O_CREAT, FMASK);
	if(fd == -1)
		return(NULL);
	if(flock(fd, LOCK_EX) == -1) {
		(void) close(fd);
		return(NULL);
	}
	fp = fdopen(fd, "r+");
	if(fp == NULL) {
		(void) flock(fd, LOCK_UN);
		(void) close(fd);
		return(NULL);
	}
	*fdp = fd;
	return(fp);
}

FILE *
hack_record_rewrite(int fd, const char *name)
{
	if(ftruncate(fd, 0) == -1)
		return(NULL);
	if(lseek(fd, 0, SEEK_SET) == -1)
		return(NULL);
	return(fdopen(dup(fd), "w"));
}

void
hack_record_unlock(int fd)
{
	if(fd != -1) {
		(void) flock(fd, LOCK_UN);
		(void) close(fd);
	}
}

static int
safe_name(const char *name)
{
	const char *p;

	if(name == NULL || *name == '\0' || *name == '/')
		return(0);
	for(p = name; *p; p++) {
		if((p[0] == '.' && p[1] == '.' &&
		    (p == name || p[-1] == '/') &&
		    (p[2] == '\0' || p[2] == '/')))
			return(0);
	}
	return(1);
}

static int
checked_open(const char *name, int flags, int mode)
{
	struct stat st;
	int fd;

	if(!safe_name(name)) {
		errno = EINVAL;
		return(-1);
	}
	fd = open(name, flags | O_CLOEXEC | O_NOFOLLOW, mode);
	if(fd == -1)
		return(-1);
	if(fstat(fd, &st) == -1 || !S_ISREG(st.st_mode)) {
		(void) close(fd);
		errno = EINVAL;
		return(-1);
	}
	return(fd);
}
