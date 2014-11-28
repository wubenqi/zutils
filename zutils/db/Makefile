CC=g++ -I. -I.. -I/usr/local/include -I/usr/include/mysql
CFLAGS=-O2 -Wall -g -fno-default-inline  -DTIXML_USE_STL=1 -DOS_POSIX

OBJS = conn_pool_manager.o database_util.o mysql/mysql_database.o mysql/mysql_dll.o

%.o: %.cpp
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<
%.o: %.cc
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<

all:	$(OBJS)
	ar rv  ../../build/linux/lib/libdb_conn_pool.a $(OBJS)

clean:
	rm -f *.o mysql/*.o mysql/*~
	ar d   ../../build/linux/lib/libdb_conn_pool.a $(OBJS)
