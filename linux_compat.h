/* Linux build compatibility for the OpenBSD hack sources. */

#ifndef LINUX_COMPAT_H
#define LINUX_COMPAT_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <bsd/string.h>

#ifndef __dead
#define	__dead	__attribute__((__noreturn__))
#endif

#ifndef OXTABS
#ifdef XTABS
#define	OXTABS	XTABS
#endif
#endif

#ifndef srandom_deterministic
#define	srandom_deterministic(seed)	srandom(seed)
#endif

#ifndef pledge
#define	pledge(promises, paths)		(0)
#endif

#endif /* LINUX_COMPAT_H */
