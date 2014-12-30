CC=g++ -I. -I.. -I../logger/third_party/glog/src/
CFLAGS=-O2 -Wall -g -fno-default-inline  -DTIXML_USE_STL=1 -DOS_POSIX

OBJS =	base_luabind_pb.o \
		base_pb_luabind.o \
		luabind_engine_register.o \
		lua_util.o \
		script_engine.o

%.o: %.cpp
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<
%.o: %.cc
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<

all:	$(OBJS)
	ar rv  ../../lib/libscriptengine.a $(OBJS)
clean:
	rm -f *.o *~
	ar d   ../../lib/libscriptengine.a $(OBJS)
