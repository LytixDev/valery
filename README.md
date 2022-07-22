# valery
Unix like interactive shell. Only for educational purposes and not meant to serve practical use. Everything written by me (Nicolai Brand, https://lytix.dev) and licensed under GPL v3.

## Installation and set up
Dependencies: `gcc`

```shell
$ git clone git@github.com:LytixDev/valery.git
```

```shell
$ cd valery/
```

Modify or keep the default `.valeryrc`, and move it to `$HOME`
```shell
$ cp .valeryrc ~/
```

```shell
$ ./build.sh
```

The executable is now made and can be ran by doing
```shell
$ ./valery
```

## Goals
- Usuable shell without aiming for POSIX compliancy.
- Memory efficient and memory safe.
- Fast.
- No third party libraries.
- No soydevery or too much voodoo.
