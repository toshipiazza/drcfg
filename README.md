drcfg
=====

Constructs a CFG of the target program, without source. For motivation, see this [blog
post](https://tpiazza.me/posts/2016-11-04-dynamorio_cfg.html). This plugin implements the
following:

- intercepts conditional branch instructions (`cbr's`)
- intercepts control transfer instructions (`cti's`)
- dumps output in json format

# Usage

```
$ drrun -c ./libcfg.so -- ../test/call.out
{
  "branches": [
    <snip>
    {
      "address": 4194497,
      "targets": [
        4194500
      ]
    },
    <snip>
  ]
}
```

Options for `drcfg` are shown below:

```
 -no_cbr              [ false]  Don't count conditional branch instructions
 -no_cti              [ false]  Don't count control transfer instructions
 -racy                [ false]  Perform racy hashtable insertion
 -only_from_app       [ false]  Only count app, not lib, instructions
```

# How to Build

```
$ mkdir -p build && cd build
$ cmake .. -DDynamoRIO_DIR=<path/to/dr/cmake/>
$ make -j4
```

# TODO

Listed in relative order of importance

- [x] Implement cache flushing as per `cbr.c`, to remove instrumentation once a branch has
      or has not been taken. This will hopefully speed up applications considerably.
- [x] Optionally intercept only branches in main module (i.e. `-only_from_app`)
- [ ] Dump json to a file
- [ ] Optionally dump YAML
- [ ] Optionally instrument return cti's
