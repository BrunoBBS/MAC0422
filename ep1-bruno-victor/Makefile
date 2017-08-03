CC:=gcc
CFLAGS=-Wall -std=c11
DEBUGF:=-g
RELEASEF:=-O2

LIBS:=
CEXT:=c
HEXT:=h

ASTDIR:=assets
INCDIR:=include
SRCDIR:=src
OBJDIR:=obj
BINDIR:=bin
DEPDIR:=deps

EXECSRC:=execsrc
TESTSRC:=testsrc
TESTBIN:=testbin

DEBUG_POST:=-d
RELEASE_POST:=

# Auto dependency, executable sources and tests sources finder

OBJS:=$(patsubst $(SRCDIR)/%.$(CEXT), $(OBJDIR)/%.o, $(shell find $(SRCDIR) -type f -name '*.$(CEXT)'))
TESTS:=$(patsubst $(TESTSRC)/%.$(CEXT), $(TESTBIN)/%, $(shell find $(TESTSRC) -type f -name '*.$(CEXT)'))
EXECS:=$(patsubst $(EXECSRC)/%.$(CEXT), $(BINDIR)/%, $(shell find $(EXECSRC) -type f -name '*.$(CEXT)'))

SRCS:=$(shell find $(SRCDIR) -type f -name '*.$(CEXT)')
SRCS+=$(shell find $(TESTDIR) -type f -name '*.$(CEXT)')
SRCS+=$(shell find $(EXECDIR) -type f -name '*.$(CEXT)')

# Don't erase dependency and object files

.PRECIOUS: $(OBJDIR)/%$(RELEASE_POST).o
.PRECIOUS: $(OBJDIR)/%$(DEBUG_POST).o

.PRECIOUS: $(DEPDIR)/%$(RELEASE_POST).d
.PRECIOUS: $(DEPDIR)/%$(DEBUG_POST).d

# Make rules

release: CFLAGS+=$(RELEASEF)
release: organize
release: dependencies
release: execs$(RELEASE_POST)
release: tests$(RELEASE_POST)

debug: CFLAGS+=$(DEBUGF)
debug: organize
debug: dependencies
debug: execs$(DEBUG_POST)
debug: tests$(DEBUG_POST)

# Generate directory trees on objects folder, delete unnecessary stuff, etc.
.PHONY: organize
organize: delete configure
	@echo "Organizing..."
	@mkdir -p $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(shell find $(SRCDIR)/ -type d -not -empty)) .
	@mkdir -p $(patsubst $(SRCDIR)/%, $(DEPDIR)/%, $(shell find $(SRCDIR)/ -type d -not -empty)) .
	@mkdir -p $(patsubst $(EXECSRC)/%, $(BINDIR)/%, $(shell find $(EXECSRC)/ -type d -not -empty)) .
	@mkdir -p $(patsubst $(TESTSRC)/%, $(TESTBIN)/%, $(shell find $(TESTSRC)/ -type d -not -empty)) .

.PHONY: delete
delete:
	@echo "Deleting empty dirs..."
	@find $(ASTDIR)/ -type d -empty -delete
	@find $(INCDIR)/ -type d -empty -delete
	@find $(SRCDIR)/ -type d -empty -delete
	@find $(OBJDIR)/ -type d -empty -delete
	@find $(BINDIR)/ -type d -empty -delete
	@find $(DEPDIR)/ -type d -empty -delete
	@find $(EXECSRC)/ -type d -empty -delete
	@find $(TESTSRC)/ -type d -empty -delete
	@find $(TESTBIN)/ -type d -empty -delete

.PHONY: configure
configure: clang
	@echo "Configuring directories..."
	@mkdir -p $(INCDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p $(EXECSRC)
	@mkdir -p $(TESTSRC)
	@mkdir -p $(TESTBIN)


clang:
	@echo "-I$(INCDIR)" > .clang_complete

# Make executables

execs$(RELEASE_POST): $(patsubst %, %$(RELEASE_POST), $(EXECS))
execs$(DEBUG_POST): $(patsubst %, %$(DEBUG_POST), $(EXECS))

$(BINDIR)/%$(RELEASE_POST): $(OBJDIR)/%$(RELEASE_POST).o $(patsubst %, %$(RELEASE_POST), $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BINDIR)/%$(DEBUG_POST): $(OBJDIR)/%$(DEBUG_POST).o $(patsubst %, %$(RELEASE_POST), $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Make tests

tests$(RELEASE_POST): $(patsubst %, %$(RELEASE_POST), $(TESTS))
tests$(DEBUG_POST): $(patsubst %, %$(DEBUG_POST), $(TESTS))

$(TESTBIN)/%$(RELEASE_POST): $(OBJDIR)/%$(RELEASE_POST).o $(patsubst %, %$(RELEASE_POST), $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TESTBIN)/%$(DEBUG_POST): $(OBJDIR)/%$(DEBUG_POST).o $(patsubst %, %$(RELEASE_POST), $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Make dependencies

dependencies: $(DEPS)

# General rules

# For release

$(OBJDIR)/%$(RELEASE_POST).o: $(SRCDIR)/%.$(CEXT) $(INCDIR)/%.$(HEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

$(OBJDIR)/%$(RELEASE_POST).o: $(SRCDIR)/%.$(CEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

$(OBJDIR)/%$(RELEASE_POST).o: $(TESTSRC)/%.$(CEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

$(OBJDIR)/%$(RELEASE_POST).o: $(EXECSRC)/%.$(CEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

# For debug

$(OBJDIR)/%$(DEBUG_POST).o: $(SRCDIR)/%.$(CEXT) $(INCDIR)/%.$(HEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

$(OBJDIR)/%$(DEBUG_POST).o: $(SRCDIR)/%.$(CEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

$(OBJDIR)/%$(DEBUG_POST).o: $(TESTSRC)/%.$(CEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

$(OBJDIR)/%$(DEBUG_POST).o: $(EXECSRC)/%.$(CEXT)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

# For dependencies

$(DEPDIR)/%.d: $(SRCDIR)/%.$(CEXT)
	$(CC) $(CFLAGS) $< -MM -MT $(OBJDIR)/$(patsubst $(DEPDIR)/%.d,%,$@).o -o $@ -I$(INCDIR)

$(DEPDIR)/%.d: $(TESTSRC)/%.$(CEXT)
	$(CC) $(CFLAGS) $< -MM -MT $(OBJDIR)/$(patsubst $(DEPDIR)/%.d,%,$@).o -o $@ -I$(INCDIR)

$(DEPDIR)/%.d: $(EXECSRC)/%.$(CEXT)
	$(CC) $(CFLAGS) $< -MM -MT $(OBJDIR)/$(patsubst $(DEPDIR)/%.d,%,$@).o -o $@ -I$(INCDIR)

# Including dependencies

# Helpers

# Upload to git

upload: clean
	git add --all
	git commit
	git push

# Clean binaries

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(BINDIR)/*
	rm -rf $(TESTBIN)/*
