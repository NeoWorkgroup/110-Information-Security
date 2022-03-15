CC	= cc
CXX	= c++
XFLAGS	= -O2 -pipe -fPIC -fPIE -I. -g3 -pedantic -Wall -Wextra
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
