CC=g++
CFLAGS=-std=c++11
EXTRAFLAGS=-lpqxx -lpq
GPBFLAG=`pkg-config --cflags --libs protobuf`

server: amazon_server.cpp server_base.h server_base.cpp  protocal/amazon.pb.h protocal/amazon.pb.cc
	$(CC) $(CFLAGS) -o server amazon_server.cpp server_base.cpp protocal/amazon.pb.cc  $(GPBFLAG)

clean:
	rm -f *~ *.o server          
clobber:
	rm -f *~ *.o                 
