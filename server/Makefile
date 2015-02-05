RM=rm -f
COMPILE_CPP=g++ -Wall -g

all: build/server

build/server: main.cpp
	$(COMPILE_CPP) main.cpp -o build/server

clean:
	$(RM) build/server
