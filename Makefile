PREFIX ?= /usr/local

CFLAGS ?= -O2 -I include/
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic

CPPFLAGS += -D_GNU_SOURCE 

.PHONY: all clean install

all: binaries

binaries: prep pexec execelf client tartest

prep:
	mkdir bin||true

test:
	@bash ./test.sh

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o bin/$@

%: %.c  
	echo staticx --strip bin/$@ bin/$@.static >> strip.sh

strip:
	bash -x strip.sh

install: binaries
	cp -v ./bin/* "$(PREFIX)/bin"

clean:
	rm -rf bin
	mkdir bin||true

