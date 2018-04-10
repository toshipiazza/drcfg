#/bin/sh
VER=32
uname -m 2> /dev/null | grep -q 64 > /dev/null 2>&1 && VER=64
HELP=yes
for i in *; do 
  test " $i" = " --" && HELP=
done
test -n "$HELP" && {
  echo "Syntax: $0 [options] -- /foo/binary -opts"
  echo Options:
  $DYNAMORIO_HOME/bin$VER/drrun -c /usr/local/lib/dynamorio/libcfg.so -h -- /bin/sh
  exit 1
}
echo "[drcfg] using DynamoRIO"
$DYNAMORIO_HOME/bin$VER/drrun -c /usr/local/lib/dynamorio/libcfg.so $*
echo "[drcfg] done"
