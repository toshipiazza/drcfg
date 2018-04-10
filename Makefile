all:
	mkdir -p build
	cd build && cmake .. -DDynamoRIO_DIR="$(DYNAMORIO_HOME)/cmake" && make

install: all
	install -d /usr/local/lib/dynamorio
	install build/libcfg.so /usr/local/lib/dynamorio
	install drcfg.sh /usr/local/bin/

save:	all
	mkdir -p bin/`uname -m`
	cp -v build/lib*.so bin/`uname -m`
	-@svn add bin/`uname -m` 2> /dev/null
	-@svn add bin/`uname -m`/*.so 2> /dev/null

clean:
	rm -rf build
