CC=g++
CFLAGS=-std=c++11
EXTRAFLAGS=-lpqxx -lpq
GPBLIBS=`pkg-config --libs protobuf --cflags protobuf`
PTHREAD=-lpthread

server: server.cpp server_base.cpp server_base.h msg.cpp msg.h protocal/amazon_orig_3.pb.h protocal/amazon_orig_3.pb.cc thread.h thread.cpp psql.h psql.cpp protocal/AmazonUPS.pb.cc protocal/AmazonUPS.pb.h
	$(CC) $(PTHREAD) -o server $(CFLAGS) server.cpp server_base.cpp msg.cpp protocal/amazon_orig_3.pb.cc thread.cpp psql.cpp protocal/AmazonUPS.pb.cc $(GPBLIBS) $(EXTRAFLAGS)

amazon_server: amazon_server.cpp server_base.cpp server_base.h msg.cpp msg.h protocal/amazon_orig_3.pb.h protocal/amazon_orig_3.pb.cc
	$(CC) -o amazon_server $(CFLAGS) amazon_server.cpp server_base.cpp msg.cpp protocal/amazon_orig_3.pb.cc  protocal/AmazonUPS.pb.cc $(GPBLIBS) 

clean:
	rm -f *~ *.o server          
clobber:
	rm -f *~ *.o                 
