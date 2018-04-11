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
 -only_from_app       [ false]  Only count app, not lib, instructions
 -instrument_ret      [ false]  Count return instructions as control flow instructions
 -racy                [ false]  Perform racy hashtable insertion
 -no_cbr              [ false]  Don't count conditional branch instructions
 -no_cti              [ false]  Don't count control transfer instructions
 -output              [    ""]  Output results to file
```

# How to Build

```
$ mkdir -p build && cd build
$ cmake .. -DDynamoRIO_DIR=<path/to/dr/cmake/>
$ make -j4
```
