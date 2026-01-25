# Sentinel

A very work in progress game.

I will change this readme to something more sophisticated once the project
is not in a "everything is a work-in-progress prove-of-concept" state

## TODO

- [ ] map renderring & loading
  - [X] basic loading & renderring
  - [X] multilayer maps (maps with multiple tiles on top of each other)
  - [ ] layerring the map
  - [ ] loading enemys & special "tiles"
- [ ] enemys, enemy ai, enemy pathfinding
- [ ] UI
- [ ] Combat
- [ ] Menus
- [ ] Campain / Lore?
- [ ] Techtrees & upgrades

## Compiling

**Linux:** run while inside the projects root directory:

```bash
./build.sh --clean --cleanup
```

<details>
  <summary> Build.sh arguments </summary>

- [-c|--clean]           Make a clean build (Remove all old build files & binaries)
- [-p|--auto-package]    Package the compiled code using package.sh into a .tar.gz with only the important files
- [-d|--no-install-deps] (**Not Recomended!**) Don't install dependencies automatically
- [--cleanup]            Cleanup build files

</details>

**Currently, compiling & running the game has only been tested on x86_64 Arch Linux**

**Please note**: there is no official MacOS compatability. You can try to compile it for MacOS, but be warned it might not work.
