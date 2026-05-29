# Hack Technical Notes

This document describes the structure and control flow of this source tree.
It is aimed at people changing the game, porting it, or trying to find the
right place for a bug fix.

## Overview

This is a flat C source tree for classic Hack. There is no module system in
the modern sense: most files include `hack.h`, share global state declared in
`hack.Decl.c`, and communicate through linked lists plus global flags.

The main runtime model is:

1. `main()` initializes options, terminal state, files, and either restores a
   saved game or generates a new level.
2. The main loop advances monsters, hunger, timeouts, healing, searching, and
   other turn-based effects when `flags.move` says time has passed.
3. Input is parsed by `parse()` and dispatched by `rhack()`.
4. Commands mutate global player, level, object, and monster state.
5. Display state is incrementally refreshed through `hack.pri.c`,
   `hack.termcap.c`, `hack.topl.c`, and `hack.tty.c`.

## Build Artifacts

`GNUmakefile` is the Linux build. It compiles `makedefs`, generates
`hack.onames.h` from `def.objects.h`, and then builds `hack`.

Important generated files:

- `makedefs`: host tool compiled from `makedefs.c`.
- `hack.onames.h`: generated object constants such as object indexes, ring
  property aliases, `CORPSE`, `LAST_RING`, and `NROFOBJECTS`.
- `hack`: local Linux game binary.
- `hack.shared`: shared-install binary built with `LINUX_SHARED` and hardened
  runtime paths.
- `hackdir/`: local development runtime directory created by `make hackdir`.

The preserved OpenBSD build is `Makefile`. It expects BSD make infrastructure
and is not the default path on Linux.

## Runtime Files

The game expects a writable game directory containing persistent state and
read-only text assets:

- `data`: symbol descriptions for the `/` command.
- `help`: long help text shown by `?` then `l`.
- `hh`: short help text shown by `?` then `s`.
- `rumors`: fortune and rumor text.
- `record`: score file.
- `perm`: lock coordination file.
- `save/`: save files named from uid and player name.
- Level lock files: temporary per-level files built from `lock` and `glo()`.

For local development, `make hackdir` copies the text assets and creates empty
`record` and `perm` files. `make run` executes `./hack -d ./hackdir`.

For shared installs, `make install-shared` separates immutable data under
`$(DATADIR)` from mutable game state under `$(GAMESDIR)`.

## Header Layout

- `hack.h`: central include file. Pulls in configuration, primary structure
  definitions, generated object names, global declarations, macros, and
  function prototypes.
- `config.h`: compile-time feature switches and default paths.
- `pathnames.h`: platform path defaults used by `config.h`.
- `linux_compat.h`: Linux compatibility shims included by `GNUmakefile` with
  `-include`.
- `linux_shared.h`: declarations for hardened shared-install file helpers.
- `def.objclass.h`: object class table structure and object display symbols.
- `def.objects.h`: object class table data; consumed by both the game and
  `makedefs`.
- `def.obj.h`: live object instance structure and inventory/worn bit masks.
- `def.permonst.h`: monster species structure and monster count constants.
- `def.monst.h`: live monster instance structure.
- `def.rm.h`: map cell structure and level terrain type macros.
- `def.mkroom.h`: generated-room metadata.
- `def.trap.h`: trap structure and trap type constants.
- `def.gold.h`: floor gold structure.
- `def.flag.h`: global option and display flags.
- `def.func_tab.h`: command dispatch table types.
- `def.edog.h`: extra data carried by tame dogs.
- `def.eshk.h`: extra data carried by shopkeepers.
- `def.wseg.h`: long worm segment structure.
- `def.gen.h`: generation constants.
- `hack.mfndpos.h`: monster movement-position flags.

## Source File Responsibilities

### Startup, Loop, And Global State

- `hack.main.c`: program entry, option parsing, player identity, game directory
  selection, locking, save restore/new-game setup, and the main turn loop.
- `hack.Decl.c`: definitions for most global variables: `u`, `flags`, `levl`,
  object/monster/trap/gold chains, player name, lock name, movement counters,
  and display coordinates.
- `hack.c`: core player movement, room visibility, picking up, trap checks,
  door interactions, carrying constraints, and movement side effects.
- `rnd.c`: random number helpers.
- `alloc.c`: allocation wrapper.
- `hack.version.c`: version command text.

### Input, Terminal, Display, And Text UI

- `hack.cmd.c`: top-level command table, extended commands, movement command
  decoding, and command dispatch.
- `hack.tty.c`: terminal mode handling, raw character reads, line input, command
  repeat prefixes, and "more" prompts.
- `hack.termcap.c`: terminal capability discovery and cursor/screen primitives.
- `hack.pri.c`: map and status display, screen symbol selection, visibility
  drawing, and bottom line formatting.
- `hack.topl.c`: top-line message buffer, repeat-last-message handling, and
  message paging.
- `hack.pager.c`: help/data/rich text paging, `/` lookup, optional shell pager,
  and optional shell escape support.
- `hack.ioctl.c`: terminal ioctl save/restore and suspend support.

### Player Commands And Item Use

- `hack.apply.c`: `a` command for tools such as keys, cameras, whistles,
  pick-axes, and ice boxes.
- `hack.do.c`: stair travel, drop/pickup helpers, rest, prayer, throw command,
  and several general object actions.
- `hack.do_name.c`: naming objects and monsters, including the cursor UI for
  selecting a monster to name.
- `hack.do_wear.c`: armor and ring wear/remove commands.
- `hack.eat.c`: eating, hunger states, corpses, food effects, fainting, and
  digestion.
- `hack.engrave.c`: engraving, wiping, and persistence of engravings.
- `hack.invent.c`: inventory letters, inventory display, object selection,
  merging, splitting, and object-chain utilities.
- `hack.objnam.c`: object description, naming, article/plural handling, and
  name parsing helpers.
- `hack.options.c`: `HACKOPTIONS` and `O` command parsing.
- `hack.potion.c`: potion effects and `#dip`.
- `hack.read.c`: scroll reading and map revelation.
- `hack.search.c`: search command and trap/door discovery.
- `hack.wield.c`: wielded weapon handling.
- `hack.worn.c`: worn-object bookkeeping and property effects.
- `hack.zap.c`: wand effects, ray movement, explosions, spell-like effects, and
  object/monster hits.

### Monsters, Combat, And AI

- `hack.monst.c`: monster species table.
- `hack.makemon.c`: monster allocation and placement.
- `hack.mon.c`: monster turn logic, movement, attacks on player, fleeing,
  item/gold pickup, and cleanup.
- `hack.mhitu.c`: monster attack effects on the player.
- `hack.fight.c`: player attacks on monsters, hit/damage calculation, monster
  death handling, and experience.
- `hack.dog.c`: dog creation, tameness, hunger, apport behavior, and dog AI.
- `hack.steal.c`: theft by nymphs and leprechauns.
- `hack.wizard.c`: Wizard of Yendor behavior.
- `hack.worm.c`: long worm segment handling.
- `hack.track.c`: player track history used by monsters.

### Level, Objects, Shops, And Special Places

- `hack.mklev.c`: standard room-and-corridor level generation, traps, vaults,
  stairs, fountains, monsters, and objects.
- `hack.mkmaze.c`: maze level generation.
- `hack.mkobj.c`: random object and gold creation.
- `hack.o_init.c`: object-class initialization, random descriptions, gem
  probabilities, and discovery bookkeeping.
- `hack.mkshop.c`: shop room selection and shop creation.
- `hack.shk.c`: shopkeeper state, billing, payment, unpaid objects, and shop
  interactions.
- `hack.shknam.c`: shopkeeper name tables.
- `hack.trap.c`: trap effects, teleportation, level teleport, and trap
  placement helpers.
- `hack.vault.c`: vault guard and fake corridor handling.
- `hack.timeout.c`: property timeout handling such as blindness, confusion,
  sickness, and stoning.

### Persistence, Scores, And System Integration

- `hack.save.c`: save-file writing and recovery.
- `hack.lev.c`: per-level serialization and deserialization.
- `hack.bones.c`: bones file save/restore.
- `hack.end.c`: death, quitting, scoring, top-ten records, and game shutdown.
- `hack.rip.c`: tombstone output.
- `hack.rumors.c`: rumor file indexing and rumor selection.
- `hack.unix.c`: Unix-specific locks, mailbox support, time/date helpers, and
  process/file utilities.
- `linux_shared.c`: safer open/unlink/name helpers for shared-install mutable
  state.

### Build-Time And Documentation Files

- `makedefs.c`: parses `def.objects.h` and emits `hack.onames.h`.
- `GNUmakefile`: Linux build, local runtime directory, shared install, and
  cleanup targets.
- `Makefile`: preserved OpenBSD build.
- `Makequest`: historical quest variant build.
- `hack.6`: manual page.
- `READ_ME`, `Original_READ_ME`, `READ_ME_LINUX`: historical and Linux build
  notes.
- `COPYRIGHT*`: license texts.
- `hack.fix`, `date.h`: historical/support files.

## Core Data Model

### Global State

Most game state is global. The central definitions live in `hack.Decl.c` and
their declarations are in `hack.h`.

Important globals:

- `struct you u`: player state: position, direction, hp, hunger, strength,
  properties, level, gold, stuck/swallowed state, and kill counts.
- `struct flag flags`: options and runtime UI/flow flags.
- `struct rm levl[COLNO][ROWNO]`: current level map cells.
- `struct monst *fmon`: linked list of monsters on the current level.
- `struct obj *fobj`: linked list of floor objects on the current level.
- `struct obj *invent`: player inventory chain.
- `struct trap *ftrap`: trap chain.
- `struct gold *fgold`: floor gold chain.
- `struct obj *uwep`, `uarm`, `uarm2`, `uarmh`, `uarms`, `uarmg`, `uleft`,
  `uright`, `uball`, `uchain`: worn or equipped object pointers.
- `long moves`: global turn counter.
- `int multi`: command repetition, occupation, and immobility counter.
- `char *save_cm`: saved command used while repeating non-movement commands.

### Map Cells

`struct rm` is intentionally small because it is stored for every coordinate
and serialized into level/save files. It contains:

- `scrsym`: the currently displayed symbol for that square.
- `typ`: terrain type such as wall, room, corridor, door, stairs.
- `new`: whether the display needs refresh.
- `seen`: whether the player has seen the square.
- `lit`: whether the square is lit.

Terrain behavior should use macros from `def.rm.h`, such as `IS_ROCK()`,
`ACCESSIBLE()`, `IS_ROOM()`, and `ZAP_POS()`, instead of numeric comparisons.

### Objects

`struct objclass objects[]` in `def.objects.h` defines object types. Live
objects are `struct obj` instances linked through `nobj`.

Object type indexes are generated by `makedefs` into `hack.onames.h`. Any
change to `def.objects.h` should be checked with a clean rebuild so generated
constants stay valid.

Key live-object fields:

- `otyp`: object type index into `objects[]`.
- `olet`: display/category symbol.
- `invlet`: inventory letter.
- `quan`, `spe`, `owt`: quantity, enchantment/charges, and weight.
- `known`, `dknown`, `cursed`, `unpaid`: discovery and shop state.
- `owornmask`: worn/equipped mask.
- `oextra`: flexible payload used for names or temporary gold amount.

### Monsters

Monster species are in `struct permonst mons[]` in `hack.monst.c`. Live
monsters are `struct monst` instances linked through `nmon`.

Important live fields:

- `data`: species pointer.
- `mx`, `my`: position.
- `mhp`, `mhpmax`: health.
- `mtrack`: short movement history.
- behavior flags such as `msleep`, `mflee`, `mtame`, `mpeaceful`, `isshk`,
  `isgd`, `mimic`, `minvis`, and `mundetected`.
- `minvent` and `mgold`: carried items and gold.
- `mextra`: flexible monster-specific data; shopkeepers use `struct eshk`,
  dogs use `struct edog`, and names follow the extra payload.

## Startup Flow

The high-level startup path in `main()` is:

1. Read `HACKDIR` or first `-d` option to choose the runtime directory.
2. Initialize options from `HACKOPTIONS`.
3. Determine player name from options, environment, login, or prompt.
4. Handle `-s` score display early.
5. Save terminal modes and initialize terminal capabilities.
6. Change to the game directory, possibly dropping privileges in secure builds.
7. Parse command-line options such as `-n`, `-u`, and role suffixes.
8. Acquire game locks.
9. Enter cbreak/no-echo terminal mode.
10. Build save-file name and try to restore it.
11. If no save is restored, initialize objects/player, generate the level,
    place the player and dog, draw the screen, and perform initial pickup.
12. Enter the infinite main loop.

## Main Turn Loop

The main loop in `hack.main.c` is the game scheduler.

When `flags.move` is true, real game time advances:

- Player track is updated.
- Monsters move on speed-dependent turns.
- New monsters may spawn.
- Slippery fingers, property timeouts, hunger, vault status, and amulet checks
  run.
- `moves` increments.
- Hit point regeneration and death checks occur.
- Automatic searching can run.

After turn effects:

- Negative `multi` counts down immobility such as paralysis or occupation
  delays.
- Armor class is recomputed.
- Objects, monsters, and screen refreshes are performed as needed.
- The bottom status line is redrawn if dirty.
- Occupations continue unless interrupted by nearby monsters.
- Repeated movement or repeated commands consume `multi`.
- Otherwise `rhack(NULL)` reads and dispatches a fresh command.

`flags.move = 0` means a command consumed input but did not spend a turn.
Command handlers return `0` when they should not consume time.

## Command And Input Flow

Input is terminal-character based, not keycode based:

1. `setftty()` disables canonical mode and echo.
2. `readchar()` reads one byte with `getchar()`.
3. `parse()` reads optional numeric repeat prefixes, then one command, with
   special handling for `f`, `F`, `m`, and `M` direction prefixes.
4. `rhack()` interprets movement first, then prefixed movement, then the
   command table.

Movement commands are hard-coded in `hack.cmd.c`:

- `h y k u l n j b`: single-step directions.
- Uppercase direction letters: run in that direction.
- `f<dir>` and control-direction: fast movement.
- `F<dir>`: fast movement with different corridor behavior.
- `m<dir>`: move without pickup.
- `M<dir>`: run without pickup.
- `<` and `>` are vertical stair commands.

Top-level commands are listed in `cmdlist[]`. Extended `#` commands are listed
in `extcmdlist[]`.

To add a simple command:

1. Implement an `int dofoo(void)` handler returning nonzero if it consumes a
   turn.
2. Add a prototype to `hack.h`.
3. Add an entry to `cmdlist[]` in `hack.cmd.c`.
4. Update `help`, `hh`, and `hack.6`.
5. Rebuild and manually test the command.

To add a `#` extended command, add an entry to `extcmdlist[]` and document it.

## Level Generation Flow

`mklev()` coordinates level creation and persistence setup. The ordinary level
builder is `makelevel()` in `hack.mklev.c`.

Normal room levels:

1. Clear `levl`.
2. Initialize level-dependent object probabilities with `oinit()`.
3. Maybe generate a maze instead of rooms.
4. Create rooms with `makerooms()`.
5. Place up/down stairs.
6. Connect rooms with corridors.
7. Add special rooms, shops, vaults, niches, traps, gold, objects, and
   monsters.

Maze levels are handled by `hack.mkmaze.c`.

The level map, room data, object chains, monster chains, traps, gold, and
engravings are serialized by `hack.lev.c`.

## Save And Level Persistence

There are two related persistence systems:

- Current and old levels are stored in temporary level files while the game is
  running.
- A save file stores the current level plus all outstanding level files and
  global player state.

`dosave0()` writes:

1. Current level via `savelev()`.
2. Inventory, contained objects, and fallen monsters.
3. uid, `flags`, `dlevel`, `maxdlevel`, `moves`, `u`, role string, genocide
   state, and object-name discovery state.
4. Each previously visited level file.

`dorecover()` reverses the process and restores global state, level state,
objects, monsters, and object-name discovery.

Because save and level files contain raw C structures, changing structure
layouts can break compatibility with existing saves and bones files.

## Display Flow

Display is incremental and terminal-capability based:

- `startup()` in `hack.termcap.c` initializes termcap data and screen size.
- `docrt()` redraws the map.
- `newsym()` and `nscr()` refresh changed map cells.
- `seeobjs()` and `seemons()` overlay visible objects and monsters.
- `bot()` draws the status line.
- `pline()` and related helpers in `hack.topl.c` manage top-line messages.
- `more()`/`xwaitforspace()` pause output.

The map square stores both terrain and last displayed symbol. When changing
level state directly, make sure the display is marked dirty through the local
patterns in surrounding code, typically by setting `levl[x][y].new` or calling
`newsym()`.

## File Access And Security Model

Local development uses the current runtime directory and direct `open()` calls.

Shared Linux builds define `LINUX_SHARED` and use wrappers in
`linux_shared.c` for mutable state. Those helpers validate simple file names,
use `O_NOFOLLOW`, and separate read/write/unlink operations for shared game
files. Shared builds also define `SECURE`, use fixed data/state directories,
and install the binary setgid `games` rather than setuid.

Be careful when touching:

- `HACKDIR`, `DATAFILE`, `HELP`, `SHELP`, `RUMORFILE`, `RECORD`, `HLOCK`.
- `chdirx()` and privilege dropping.
- `linux_shared.c` name validation.
- lock and save-file construction.

## Extension Points

### Add Or Change Objects

Primary files:

- `def.objects.h`
- `hack.o_init.c`
- `hack.mkobj.c`
- command/effect files that consume the object type
- `data`, `help`, and `hh` if user-visible

Checklist:

1. Add the object class entry in the right section of `def.objects.h`.
2. Rebuild from clean output so `hack.onames.h` regenerates.
3. Update generation probabilities or special-case creation if needed.
4. Add behavior in the relevant command/effect file.
5. Consider discovery, naming, inventory display, shop price, weight, and save
   compatibility.

### Add Or Change Monsters

Primary files:

- `hack.monst.c`
- `def.permonst.h`
- `hack.makemon.c`
- `hack.mon.c`
- `hack.mhitu.c`
- `hack.fight.c`
- corpse/object entries in `def.objects.h` if needed
- `data`

Checklist:

1. Add species data to `mons[]`.
2. Keep `CMNUM` and corpse relationships coherent.
3. Add movement, attack, death, corpse, or special behavior as needed.
4. Update symbol lookup/help text where relevant.

### Add Or Change Commands

Primary files:

- `hack.cmd.c`
- command implementation file or new local function in an existing subsystem
- `hack.h`
- `help`, `hh`, and `hack.6`

Command handlers return `0` for no turn consumed and nonzero for a turn
consumed. Commands that ask for direction should use `getdir()` so confusion,
vertical movement, and quit characters match existing behavior.

### Add Or Change Terrain Or Traps

Primary files:

- `def.rm.h`
- `def.trap.h`
- `hack.mklev.c`
- `hack.mkmaze.c`
- `hack.trap.c`
- `hack.pri.c`
- `hack.zap.c`
- save/level code in `hack.lev.c`

Keep `struct rm` small and remember that both map cells and traps are
serialized. Prefer existing terrain macros over numeric comparisons.

### Add Or Change Player Properties

Primary files:

- `hack.h` (`struct you`, property indexes, property macros)
- `hack.timeout.c`
- item files that grant/remove properties
- `hack.worn.c`
- save compatibility checks

Properties use `u.uprops[]` flags with timeout, intrinsic, and worn-ring bits.
Ring-related properties are tied to generated ring indexes from
`hack.onames.h`.

## Conditional Features

Several behaviors are compile-time switches, not runtime options:

- `SHELL`: enables `!` shell escape. Disabled by default here.
- `SUSPEND`: enables `^Z` suspend. Enabled by default here.
- `MAIL`: enables mailbox checks.
- `NEWS`: enables startup news.
- `WIZARD`: controls wizard/debug behavior.
- `QUEST`: changes the game variant and some movement/level behavior.
- `NOWORM`: replaces long worms with wumpuses.
- `LINUX_SHARED`: shared-install Linux file access and fixed paths.
- `SECURE`: privilege-dropping behavior around game directory selection.

The help files are static and do not automatically reflect these switches.
Update `help`, `hh`, and `hack.6` when changing command availability.

## Development Notes

- The code is K&R-era C style. Keep edits local and avoid broad reformatting.
- Many functions rely on globals rather than explicit parameters. Read the
  surrounding file before moving behavior.
- Save files, level files, and bones files serialize raw structures. Structure
  changes are behavioral changes.
- Object indexes are generated. Do not hard-code new object numbers by hand.
- Some help text is intentionally historical and static; keep docs and command
  tables in sync when practical.
- Use `make clean && make` after changes that touch generated object data,
  headers, build flags, or broad declarations.

## Manual Verification

There is no automated test suite in this tree. Typical verification:

```sh
make clean
make
make hackdir
./hack -d ./hackdir -s
./hack -d ./hackdir
```

For shared-install work:

```sh
make shared
make install-shared DESTDIR=/tmp/hack-install GAMEGROUP=$(id -gn)
```

For gameplay changes, record the reproduction scenario, the command sequence,
and what changed on screen or in the save/runtime files.
