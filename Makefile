all:
	mkdir -p build
	cd build && cmake .. -DDynamoRIO_DIR="$(DYNAMORIO_HOME)/cmake" && make

install: all
	install -d /usr/local/lib/dynamorio
	install build/libcfg.so /usr/local/lib/dynamorio
	install drcfg.sh /usr/local/bin/

clean:
	rm -rf build
