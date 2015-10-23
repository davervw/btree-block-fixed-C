INC=-Isrc/cache -Isrc/core
OPT=-O3

all: lib/libbtree.a bin/test1 bin/test2 bin/test3 bin/test4 bin/test5

clean:
	rm -rf lib 2>/dev/null
	rm -rf obj 2>/dev/null
	rm -rf bin 2>/dev/null

bin/test1: lib/libbtree.a src/test/test1.cpp
	mkdir bin 2>/dev/null || true
	g++ $(OPT) -Llib $(INC) src/test/test1.cpp -lbtree -o bin/test1

bin/test2: lib/libbtree.a src/test/permute.cpp
	mkdir bin 2>/dev/null || true
	g++ $(OPT) -Llib $(INC) src/test/permute.cpp -lbtree -o bin/test2

bin/test3:
	mkdir bin 2>/dev/null || true
	g++ $(OPT) -Llib $(INC) src/test/test_bulk.cpp -lbtree -o bin/test3

bin/test4: lib/libbtree.a src/test/test4.cpp
	mkdir bin 2>/dev/null || true
	g++ $(OPT) -Llib $(INC) src/test/test4.cpp -lbtree -o bin/test4

bin/test5:
	mkdir bin 2>/dev/null || true
	g++ $(OPT) -Llib $(INC) src/test/test_load.cpp -lbtree -o bin/test5

lib/libbtree.a: obj/btbulk.o obj/btiter.o obj/btree.o \
 obj/throwexception.o obj/btcache.o obj/btcacherw.o obj/btcacherw2.o
	mkdir lib 2>/dev/null || true
	ar rcs lib/libbtree.a obj/btcache.o obj/btcacherw.o obj/btcacherw2.o \
               obj/btbulk.o obj/btiter.o obj/btree.o obj/throwexception.o

obj/btbulk.o: src/core/btbulk.cpp src/core/btbulk.h src/core/btree.h
	mkdir obj 2>/dev/null || true
	g++ $(OPT) $(INC) -c src/core/btbulk.cpp -o obj/btbulk.o

obj/btiter.o: src/core/btiter.cpp src/core/btiter.h src/core/btree.h
	mkdir obj 2>/dev/null || true
	g++ $(OPT) $(INC) -c src/core/btiter.cpp -o obj/btiter.o

obj/btree.o: src/core/btree.cpp src/core/btree.h src/core/throwexception.cpp \
             src/core/btbulk.h src/core/btiter.h
	mkdir obj 2>/dev/null || true
	g++ $(OPT) $(INC) -c src/core/btree.cpp -o obj/btree.o

obj/throwexception.o: src/core/throwexception.cpp
	mkdir obj 2>/dev/null || true
	g++ $(OPT) $(INC) -c src/core/throwexception.cpp -o obj/throwexception.o

obj/btcache.o: src/cache/btcache.cpp src/cache/btcache.h src/core/btree.h
	mkdir obj 2>/dev/null || true
	g++ $(OPT) $(INC) -c src/cache/btcache.cpp -o obj/btcache.o

obj/btcacherw.o: src/cache/btcacherw.cpp src/cache/btcacherw.h src/core/btree.h
	mkdir obj 2>/dev/null || true
	g++ $(OPT) $(INC) -c src/cache/btcacherw.cpp -o obj/btcacherw.o

obj/btcacherw2.o: src/cache/btcacherw2.cpp src/cache/btcacherw2.h src/core/btree.h
	mkdir obj 2>/dev/null || true
	g++ $(OPT) $(INC) -c src/cache/btcacherw2.cpp -o obj/btcacherw2.o
