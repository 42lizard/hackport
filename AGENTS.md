# Repository Guidelines

## Project Structure & Module Organization

This repository is a flat C source tree for the classic `hack` game. Core game logic lives in `hack.*.c` files, with shared declarations in `hack.h` and `def.*.h`. Build-time generation uses `makedefs.c` to produce `hack.onames.h` from `def.objects.h`. Runtime text assets are top-level files: `data`, `help`, `hh`, and `rumors`. Documentation and historical notes are in `READ_ME`, `Original_READ_ME`, `hack.6`, `COPYRIGHT`, and `COPYRIGHT-JF`.

## Build, Test, and Development Commands

- `make` builds the `hack` binary using the BSD-style Makefile and links against curses.
- `make clean` removes generated objects and files such as `hack.onames.h` and `makedefs`.
- `./hack -d ./hackdir` runs the game with a local game directory when configured and built.

The Makefile includes `<bsd.prog.mk>`, so on non-BSD systems you may need BSD make and compatible system makefiles. Review `config.h` before porting or changing platform behavior.

## Coding Style & Naming Conventions

Keep changes consistent with the existing K&R-era C style. Use tabs for indentation where the surrounding code does, preserve compact function bodies, and avoid broad reformatting. File names follow the established domain split, for example `hack.mkobj.c` for object creation and `hack.shk.c` for shopkeeper logic. Add declarations to existing headers rather than introducing new header layers unless the dependency boundary is clear.

## Testing Guidelines

There is no automated test suite in this tree. Validate changes by building with `make`, starting a local game, and exercising the affected flow manually. For gameplay fixes, note the commands or scenario used to reproduce and verify the behavior. For generated data changes, rebuild from clean output to ensure `makedefs` and `hack.onames.h` still work.

## Commit & Pull Request Guidelines

This checkout has no existing commit history, so use concise imperative commit subjects such as `Fix inventory display overflow` or `Update object generation table`. Keep commits focused on one behavior or subsystem. Pull requests should include a short problem statement, the files or gameplay areas touched, manual verification steps, and screenshots or terminal output when display behavior changes.

## Security & Configuration Tips

Be careful around install and runtime paths. `READ_ME` describes historical setuid and shared game-directory usage; do not enable privileged installs casually. Prefer local `-d` directories for development runs, and keep generated save, lock, and score files out of source control.
