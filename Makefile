# if you dont specify PREFIX .a and .so files as well as include headers will go
# libcerberus directory and this directory will be copied to /usr/local/lib/

CFLAGS = -g -O2 -Wall -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LIBS = -ldl $(OPTLIBS)
PREFIX ?= /usr/local

SOURCES = $(wildcard src/**/*.c src/*.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

TEST_SRC = $(wildcard tests/*_tests.c)
TESTS = $(patsubst %.c, %, $(TEST_SRC))

TARGET = build/libcerberus/lib/libcerberus.a
SO_TARGET = $(patsubst %.a, %.so, $(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) tests

dev: CFLAGS = -g -Wall -Wextra -Isrc -DNDEBUG
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

.PHONY: build
build:
	@mkdir -p build/libcerberus/lib
	@mkdir -p build/libcerberus/include
	@cp src/*.h build/libcerberus/include
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS)
	@touch tests/tests.log
	/bin/bash ./tests/runtests.sh

$(TESTS): $(TARGET)
	$(CC) $(TEST_SRC) $(CFLAGS) -o $@

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -rf bin
	rm -f tests/tests.log
	find . -name "*.gc" -exec rm {} \;
	rm -rf `find . -name ".dSYM" -print`

# The Installer
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	cp -r build/libcerberus $(DESTDIR)/$(PREFIX)/lib/

# The checker
check:
	/bin/bash ./tests/runchecks.sh $(SOURCES)
