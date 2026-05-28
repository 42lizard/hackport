CC ?= cc
CFLAGS ?= -O2 -g
CFLAGS += -I. -include linux_compat.h
CPPFLAGS += -D_GNU_SOURCE
LDLIBS += -lcurses -lbsd

PROG = hack
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
	hack.wizard.c hack.worm.c hack.worn.c hack.zap.c rnd.c

OBJS = $(SRCS:.c=.o)
AUX = data help hh rumors

.PHONY: all clean run hackdir

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(OBJS): hack.onames.h

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

clean:
	rm -f $(PROG) $(HOSTPROG) $(OBJS) hack.onames.h hack.onames.h.tmp
