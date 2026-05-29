# Hack

This repository contains a Linux-portable source tree for the classic
terminal game Hack. The original OpenBSD-style build files are preserved, and
the Linux workflow uses `GNUmakefile` so plain `make` builds the local game
binary.

Hack is a display-oriented dungeon game in the Rogue family. The game is
implemented as a flat C source tree with shared declarations in `hack.h`,
game state in `hack.Decl.c`, and subsystem files named `hack.*.c`.

## Requirements

On Linux you need:

- C compiler
- GNU make
- curses development headers
- libbsd development headers

On Debian or Ubuntu-like systems:

```sh
sudo apt install build-essential libncurses-dev libbsd-dev
```

Package names may differ on other distributions.

## Build

Build the local binary:

```sh
make
```

The build first compiles `makedefs`, generates `hack.onames.h` from
`def.objects.h`, and then links `./hack` against curses and libbsd.

Remove generated build output:

```sh
make clean
```

## Run Locally

For development runs, use a local runtime directory instead of system-wide game
paths:

```sh
make hackdir
```

This creates `./hackdir`, copies `data`, `help`, `hh`, and `rumors`, creates
empty `record` and `perm` files, and creates `hackdir/save` for saved games.

Run the game:

```sh
make run
```

Equivalent manual command:

```sh
./hack -d ./hackdir
```

Show the score list without starting an interactive game:

```sh
./hack -d ./hackdir -s
```

## Shared Install

The shared install mode is intended for a local system install under
`/usr/local`. It builds a separate `hack.shared` binary with shared runtime
paths and Linux shared-install hardening enabled.

Build it:

```sh
make shared
```

Install into the live system:

```sh
make install-shared
```

By default this installs:

- `/usr/local/games/hack`
- `/usr/local/share/games/hack/data`
- `/usr/local/share/games/hack/help`
- `/usr/local/share/games/hack/hh`
- `/usr/local/share/games/hack/rumors`
- `/var/games/hack`

The installed binary is setgid to the `games` group by default. Mutable state
lives under `/var/games/hack`; read-only text assets live under
`/usr/local/share/games/hack`.

Useful install variables:

```sh
DESTDIR=...
PREFIX=/usr/local
GAMESDIR=/var/games/hack
GAMEGROUP=games
```

Stage a package image:

```sh
make install-shared DESTDIR=/tmp/hack-install
```

If staging in an environment without a `games` group, use your current group:

```sh
make install-shared DESTDIR=/tmp/hack-install GAMEGROUP=$(id -gn)
```

Do not install this program setuid. The supported shared model is setgid
`games`.

## Runtime Files

The game uses these text assets:

- `data`: descriptions for the `/` command.
- `help`: long in-game help.
- `hh`: short in-game help.
- `rumors`: fortune cookie and rumor text.

The game also needs mutable state files:

- `record`: score file.
- `perm`: lock coordination file.
- `save/`: saved games.
- temporary level and lock files created while a game is running.

For normal development, keep these under `./hackdir` by running with
`-d ./hackdir`.

## Generated And Local Files

These files are generated locally and should not be committed:

- `hack`
- `hack.shared`
- `makedefs`
- `hack.onames.h`
- `*.o`
- `*.shared.o`
- `hackdir/`
- `record`
- `perm`
- `record_lock`
- `safelock`
- `save/`

Top-level `record` or `perm` files can appear if you run `./hack` without
`-d ./hackdir`.

## Documentation

- `TECHNICAL.md`: source layout, control flow, data model, and extension
  points.
- `READ_ME_LINUX`: original Linux build notes used as the basis for this
  README.
- `READ_ME`: historical upstream notes.
- `Original_READ_ME`: Jay Fenlason's original notes.
- `hack.6`: manual page.

## Security Notes

Linux-only compatibility is collected in `linux_compat.h`. Prefer adding small
Linux shims there instead of spreading platform conditionals through gameplay
code.

Shared builds define `LINUX_SHARED` and `SECURE`. That enables checked opens
for runtime files, kernel locking for the score file, and full uid/gid
privilege dropping before child execs or user-selected directories.

Be careful when changing install paths, runtime state paths, locking, save
files, or command features that execute external programs.
