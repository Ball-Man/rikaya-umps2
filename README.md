# umps2-project
umps2 kernel related project from Unibo


## Project structure
The project is structured as a collection of phases. Each different phase is considered a 'subproject' with its own folder and subproject structure(as well as makefile etc.).

## Build and run
Be sure to have [uMPS2](https://github.com/tjonjic/umps) and a mipsel cross compiler installed(e.g. gcc-mispel-linux-gnu from apt repos) as well as make. You can't build all the phases at once due to their heterogeneity so you have to choose which one you want to build. Once you are inside the phase directory, just run:
```
make
```
and the binaries will be created in a BUILD subdirectory.

To run the program you have to create a umps machine from the umps GUI and configure it accordingly. A fast way is creating it directly inside the BUILD directory.

## Wiki
Check the [wiki](https://github.com/Ball-Man/umps2-project/wiki) for more information about how the project is structured.
