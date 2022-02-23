PREFIX ?= /usr/local

CFLAGS ?= -O2
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic

CPPFLAGS += -D_GNU_SOURCE

.PHONY: all clean install

all: build

build: prep pexec execelf client

prep:
	mkdir bin||true

test: build
	@bash ./test.sh

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o bin/$@
	staticx --strip bin/$@ bin/$@.static

install: build
	cp -v ./bin/* "$(PREFIX)/bin"

clean:
	rm -rf bin
	mkdir bin||true

