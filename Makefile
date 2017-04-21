CC=g++
CFLAGS=-std=c++11
EXTRAFLAGS=-lpqxx -lpq
GPBLIBS=`pkg-config --libs protobuf --cflags protobuf`

server: amazon_server.cpp server_base.cpp server_base.h msg.cpp msg.h protocal/amazon_orig_3.pb.h protocal/amazon_orig_3.pb.cc
	$(CC) -o server $(CFLAGS) amazon_server.cpp server_base.cpp msg.cpp protocal/amazon_orig_3.pb.cc $(GPBLIBS) 
clean:
	rm -f *~ *.o server          
clobber:
	rm -f *~ *.o                 
