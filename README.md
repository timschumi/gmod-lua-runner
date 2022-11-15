# gmod-lua-runner

This project aims to provide a headless implementation of the Garry's Mod Lua
environment, intended to run scripts, addons, and binary modules without having to
boot up the full game.

As of now, the tool only provides a subset of the Lua standard library and the
binary module interface (in particular the parts that are needed by [GLuaTest](https://github.com/CFC-Servers/GLuaTest),
a testing framework). Nonetheless, the project should already provide a solid basis
for trying to run a script and pinpointing the part that explodes.

In case you find anything to be missing or something doesn't behave as expected,
feel free to report the problem in an [issue](https://github.com/timschumi/gmod-lua-runner/issues).

If you like my work (or otherwise profited from this tool), consider [buying me some
iced tea](https://paypal.me/schumact)!

## Usage

### Building

**Note:** Automatic builds for the `linux64` architecture are done by GitHub for each
commit on an Ubuntu 22.04 host. Since no tagged releases exist yet, those builds can
only be downloaded via the artifacts package on [one of the CI runs](https://github.com/timschumi/gmod-lua-runner/actions).

The project is contained within one big CMake project, and can be built as such.
However, attention has to be paid to the fact that the repository features submodules,
which need to be cloned by passing `--recursive` to the clone command or by running
`git submodule update --init --recursive` after the fact.

Building the project also requires selecting the correct toolchain file by passing
`-DCMAKE_TOOLCHAIN_FILE=toolchain-<arch>.cmake` to the configure command, with
`toolchain-<arch>.cmake` being one of the files that are found in the repository root.
Note that support for 32-bit Linux is incomplete at the moment (as the resulting
LuaJIT binary is not actually a 32-bit executable), and support for Windows is missing
entirely.

### Running scripts

After building, arbitrary scripts can be run using the `runner` binary:

```
./runner /path/to/file.lua
```

Note that, when not running from the distribution package (which roughly resembles a
Garry's Mod installation directory), that directory structure will have to be created
manually and the program will have to be pointed to it using the `GMOD_DIR` environment
variable:

```
GMOD_DIR=/path/to/directory ./runner /path/to/file.lua
```

In this example, `/path/to/directory` would then contain the usually known `garrysmod` directory.

## Limitations

### Client/Server architecture

The "engine" doesn't currently have a concept of being a server or client. By default,
neither the `SERVER` nor the `CLIENT` variable is set. In case one of your scripts or modules
specifically relies on being on a client or server, those settings have to be dialed in manually
in the main script or in one of the autorun scripts (depending on where that setting is required).

### Halting problem

As it's one of the harder problems to solve, the Lua implementation is not able to tell whether
the script will eventually finish "running", including all of its side effects. The way that this
is currently handled is that, after the main script has finished running, a loop will continue
processing the `Tick` and `Think` hooks (as well as any timers) as long as there still is a yielding
coroutine present or a timer with a non-infinite number of repetitions remains.

## Extensions

### Exit code

The runner converts the first returned value from the main script to a number and relays
it back to the operating system as the exit code. Note that this does not account for wanting
to change the exit code using anything that runs after the main script has already stopped.

If no value has been returned by the main script, the tool will instead return the number that
is stored in `runner.return_value` in the Lua environment. This value only gets evaluated after
all the [background tasks](#halting-problem) have stopped running, so the stored value may be
changed from there as well.

### Environment

The global `runner` table also contains other values that relate to the environment outside of Lua.
At this point, this includes the following:

- `runner.root`: The path where the `garrysmod` directory can be found. This is either the full path
  to the directory that was set through `GMOD_DIR` or (if the former is not set) the path to the `runner`
  executable itself.
