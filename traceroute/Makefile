CC = g++
CXXFLAGS = -Wall 
LDFLAGS =

SRCS = main.cpp icmp_send.cpp incoming_packets.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = mytraceroute

.PHONY: all clean distclean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
