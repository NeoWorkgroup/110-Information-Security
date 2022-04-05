CC	= cc
CXX	= c++
XFLAGS	= -Og -pipe -fPIC -fPIE -I. -g3 -pedantic -Wall -Wextra -D_DEFAULT_SOURCE
#DBG	= -fsanitize=undefined,integer,nullability -fno-omit-frame-pointer
CFLAGS	= $(XFLAGS) $(DBG) -std=c99
CXXFLAGS =  $(XFLAGS) $(DBG) -std=c++17
OBJS	= encrypt.o decrypt.o

.PHONY: all clean countline test

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

countline:
	wc -l *.h *.c *.cpp *.hpp

clean:
	rm -f $(OBJS)
