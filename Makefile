CXX?=clang++
CFLAGS=-Wall -Wextra -pedantic -ggdb
LDFLAGS=-lssl -lcrypto
COMPILE= $(CXX) $(CFLAGS)

all: app skunk_test

test: dispatcher_test

app: csgi.o app.cpp
	$(COMPILE) app.cpp csgi.o $(LDFLAGS) -o app

dispatcher_test: csgi.o dispatcher.o dispatcher_test.cpp
	$(COMPILE) dispatcher_test.cpp csgi.o dispatcher.o $(LDFLAGS) -o dispatcher_test
	./dispatcher_test

csgi.o: csgi.hpp csgi.cpp
	$(COMPILE) -c csgi.cpp

dispatcher.o: dispatcher.hpp dispatcher.cpp
	$(COMPILE) -c dispatcher.cpp

Skunk.o: Skunk.cpp Skunk.hpp csgi.hpp
	$(COMPILE) -c Skunk.cpp

skunk_test: skunk_test.cpp Skunk.o csgi.o
	$(COMPILE) skunk_test.cpp Skunk.o csgi.o $(LDFLAGS) -o skunk_test

test: skunk_test Selenium_test.py
	./skunk_test &
	sst-run -r html -s Selenium_test
	killall skunk_test

clean:
	rm -f *.o app dispatcher_test
