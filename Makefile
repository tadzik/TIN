CC=clang++
#CC=g++
CFLAGS=-Wall -Wextra -pedantic
COMPILE= $(CC) $(CFLAGS)

app: csgi.o app.cpp
	$(COMPILE) app.cpp csgi.o -o app

csgi.o: csgi.hpp csgi.cpp
	$(COMPILE) -c csgi.cpp

clean:
	rm -f app csgi.o
