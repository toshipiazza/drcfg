drcfg
=====

Constructs a CFG of the target program, without source. For motivation, see this [blog
post](https://tpiazza.me/posts/2016-11-04-dynamorio_cfg.html). This plugin implements the
following:

- intercepts control branch instructions (`cbr's`)
- intercepts control transfer instructions (`cti's`)
- dumps output in json format

# Usage

```
$ drrun -c ./libcfg.so -- target-program <args>
```

<script type="text/javascript" src="https://asciinema.org/a/7ciqzh7rkmprinrv9q7vd0m6s.js"
                                id="asciicast-7ciqzh7rkmprinrv9q7vd0m6s" async></script>

# How to Build

```
$ mkdir -p build && cd build
$ cmake .. -DDynamoRIO_DIR=<path/to/dr/cmake/>
$ make -j4
```

# TODO

- [ ] Dump json to a file
- [ ] Optionally dump YAML
- [ ] Optionally intercept only branches in main module (i.e. `-only_from_app`)
- [ ] Implement cache flushing as per `cbr.c`, to remove instrumentation once a branch has
      or has not been taken. This will hopefully speed up applications considerably.
