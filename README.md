# valery
Unix like interactive shell. Only for educational purposes and not meant to serve practical use. Everything written by and me and licensed under GPL v3.

## Installtion and set up
Dependencies: `gcc`

```shell
$ git clone git@github.com:LytixDev/valery.git
```

```shell
$ cd valery/
```

Modify, or keep the default `.valeryrc`, and move it to `$HOME`
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
- Usuable shell. POSIX compliancy would be cool, but it would probably be too much work.
- Memory efficient and safe.
- Fast.
- No third part libraries.
- No soydevery or too much voodoo.
