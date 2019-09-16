vpgo 0.1
=========

Simple go engine

Copyright &copy; 2019 Ivanov Viktor

## Installation

Install dependencies

```
# apt install autoconf-archive libboost-all-dev
```

Generate autotools scripts

```
$ sh ./autogen.sh
```

Configure

```
$ mkdir -p Debug
$ cd Debug
$ ../configure
```

> The maintainer mode is **disabled by default**. That means that makefiles won't be updated when new files are added to the project.
>
> To enable maintainer mode, run configure with `--enable-maintainer-mode`. It ensures that your build scripts will always be up-to-date.

Compile

 ```
$ make
```

Install as root

```
# make install
```

For more detailed information about compiling the sources see `./configure --help`.
