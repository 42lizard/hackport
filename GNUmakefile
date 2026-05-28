CC ?= cc
CFLAGS ?= -O2 -g
CFLAGS += -I. -include linux_compat.h
CPPFLAGS += -D_GNU_SOURCE
LDLIBS += -lcurses -lbsd
INSTALL ?= install
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/games
DATADIR ?= $(PREFIX)/share/games/hack
GAMESDIR ?= /var/games/hack
GAMEGROUP ?= games

PROG = hack
SHARED_PROG = hack.shared
HOSTPROG = makedefs

SRCS = alloc.c hack.Decl.c hack.apply.c hack.bones.c hack.c hack.cmd.c \
	hack.do.c hack.do_name.c hack.do_wear.c hack.dog.c hack.eat.c \
	hack.end.c hack.engrave.c hack.fight.c hack.invent.c hack.ioctl.c \
	hack.lev.c hack.main.c hack.makemon.c hack.mhitu.c hack.mklev.c \
	hack.mkmaze.c hack.mkobj.c hack.mkshop.c hack.mon.c hack.monst.c \
	hack.o_init.c hack.objnam.c hack.options.c hack.pager.c hack.potion.c \
	hack.pri.c hack.read.c hack.rip.c hack.rumors.c hack.save.c \
	hack.search.c hack.shk.c hack.shknam.c hack.steal.c hack.termcap.c \
	hack.timeout.c hack.topl.c hack.track.c hack.trap.c hack.tty.c \
	hack.u_init.c hack.unix.c hack.vault.c hack.version.c hack.wield.c \
	hack.wizard.c hack.worm.c hack.worn.c hack.zap.c rnd.c linux_shared.c

OBJS = $(SRCS:.c=.o)
SHARED_OBJS = $(SRCS:.c=.shared.o)
AUX = data help hh rumors
SHARED_CPPFLAGS = $(CPPFLAGS) -DLINUX_SHARED -DSECURE \
	-DHACKDIR=\"$(GAMESDIR)\" \
	-DHELP=\"$(DATADIR)/help\" \
	-DSHELP=\"$(DATADIR)/hh\" \
	-DRUMORFILE=\"$(DATADIR)/rumors\" \
	-DDATAFILE=\"$(DATADIR)/data\"

.PHONY: all clean run hackdir shared install-shared uninstall-shared

all: $(PROG)

shared: $(SHARED_PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(SHARED_PROG): $(SHARED_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(SHARED_OBJS) $(LDLIBS)

$(OBJS): hack.onames.h

$(SHARED_OBJS): hack.onames.h

%.shared.o: %.c
	$(CC) $(CFLAGS) $(SHARED_CPPFLAGS) -c -o $@ $<

hack.onames.h: $(HOSTPROG) def.objects.h
	./$(HOSTPROG) def.objects.h > $@.tmp
	mv $@.tmp $@

$(HOSTPROG): makedefs.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ makedefs.c -lbsd

hackdir:
	mkdir -p hackdir/save
	cp $(AUX) hackdir/
	touch hackdir/record hackdir/perm

run: $(PROG) hackdir
	./$(PROG) -d ./hackdir

install-shared: $(SHARED_PROG)
	$(INSTALL) -d -m 0755 $(DESTDIR)$(BINDIR)
	$(INSTALL) -d -m 0755 $(DESTDIR)$(DATADIR)
	$(INSTALL) -d -m 2770 -g $(GAMEGROUP) $(DESTDIR)$(GAMESDIR)
	$(INSTALL) -d -m 2770 -g $(GAMEGROUP) $(DESTDIR)$(GAMESDIR)/save
	$(INSTALL) -m 2755 -g $(GAMEGROUP) $(SHARED_PROG) $(DESTDIR)$(BINDIR)/hack
	$(INSTALL) -m 0644 $(AUX) $(DESTDIR)$(DATADIR)/
	touch $(DESTDIR)$(GAMESDIR)/record $(DESTDIR)$(GAMESDIR)/perm
	chgrp $(GAMEGROUP) $(DESTDIR)$(GAMESDIR)/record $(DESTDIR)$(GAMESDIR)/perm
	chmod 0660 $(DESTDIR)$(GAMESDIR)/record $(DESTDIR)$(GAMESDIR)/perm

uninstall-shared:
	rm -f $(DESTDIR)$(BINDIR)/hack
	rm -f $(DESTDIR)$(DATADIR)/data $(DESTDIR)$(DATADIR)/help \
	    $(DESTDIR)$(DATADIR)/hh $(DESTDIR)$(DATADIR)/rumors

clean:
	rm -f $(PROG) $(SHARED_PROG) $(HOSTPROG) $(OBJS) $(SHARED_OBJS) \
	    hack.onames.h hack.onames.h.tmp
