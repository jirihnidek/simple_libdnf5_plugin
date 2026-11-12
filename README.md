# simple_libdnf5_plugin

A minimal example [libdnf5](https://github.com/rpm-software-management/dnf5)
plugin written in C++. It does nothing more than report an example progress
(a counter from 0 to 10) through the libdnf5 progress API when a DNF5
transaction starts.

It is intended as a starting point / reference for writing your own libdnf5
plugins. It was inspired by the
[rhsm-dnf5-plugins](https://github.com/candlepin/rhsm-dnf5-plugins).

## How it works

The plugin implements the mandatory `libdnf5::plugin::IPlugin` interface and the
C-linkage entry points that libdnf5 uses to load the plugin from disk. The
progress is reported from the `post_base_setup()` hook, which runs after the
`Base` object has been set up.

The hook starts a new progress entry via `get_base().progress(...)` and then
counts from 0 to 10, reporting the current value on each step (with a short
pause between increments so the progress is noticeable) and finally marking the
progress as finished. The messages are provided to the API through small
`Message` subclasses (`SimpleMessage` for messages without arguments and
`ParamMessage` for messages with a single argument).

## Requirements

* A C++ compiler with C++20 support (e.g. [GCC](https://gcc.gnu.org/))
* [CMake](https://cmake.org/) (minimum version 3.21)
* [libdnf5](https://github.com/rpm-software-management/dnf5) (development package)
* [PkgConf](http://pkgconf.org/)

On Fedora, the build dependencies can be installed with:

```console
$ sudo dnf install gcc-c++ cmake libdnf5-devel pkgconf-pkg-config
```

## Building

```console
$ mkdir build
$ cd build
$ cmake ../
$ make
```

This produces `build/simple/simple.so`.

## Installing

```console
$ sudo make install
```

This installs:

* the plugin `simple.so` into `<libdir>/libdnf5/plugins/`
* the configuration file `simple.conf` into `/etc/dnf/libdnf5-plugins/`

## Trying it out

After installing, run any DNF5 command and the plugin will report its example
progress counting from 0 to 10:

```console
$ dnf5 repolist
...
```

To disable the plugin, set `enabled = no` in
`/etc/dnf/libdnf5-plugins/simple.conf`.
