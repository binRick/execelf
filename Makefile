PREFIX ?= /usr/local

CFLAGS ?= -O2 -I include/
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic

CPPFLAGS += -D_GNU_SOURCE 

.PHONY: all clean install

all: binaries

binaries: prep pexec-tar pexec execelf client dostrip dotar tartest

prep:
	mkdir bin||true
	echo '' > strip.sh
	chmod +x strip.sh

test:
	@bash ./test.sh

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o bin/$@
	echo 'staticx --strip bin/$@ bin/$@.static &' >> strip.sh

#%: %.c  

dostrip:
	echo 'wait' >> strip.sh
	@bash strip.sh

dotest:
	@bash test.sh 2>.err

dotar:
	@bash tar.sh

install: binaries
	cp -v ./bin/* "$(PREFIX)/bin"

clean:
	rm -rf bin
	mkdir bin||true

