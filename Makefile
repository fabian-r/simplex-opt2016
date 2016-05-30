BUILDDIR ?= build
CFG      ?= debug
NAME     ?= simplex
SRCDIR   ?= src

# Use clang by default.
CC  = gcc
CXX = g++

# Be verbose about the build.
Q ?= @

BINDIR := $(BUILDDIR)/$(CFG)
BIN    := $(BINDIR)/$(NAME)
SRC    := $(sort $(wildcard $(SRCDIR)/*.cpp))
OBJ    := $(SRC:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)
DEP    := $(OBJ:%.o=%.d)

# Show compiler warnings and treat them seriously!
CXXFLAGS += -Wall -W

# Standard compile time flags for C++/CXX projects.
CXXFLAGS += -std=c++11
CXXFLAGS += -I./include

-include config/$(CFG).cfg

DUMMY := $(shell mkdir -p $(sort $(dir $(OBJ))))

.PHONY: all clean

all: $(BIN)

-include $(DEP)

clean:
	@echo "===> CLEAN"
	$(Q)rm -fr $(BINDIR)

$(BIN): $(OBJ)
	@echo "===> LD $@"
	$(Q)$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "===> CXX $<"
	$(Q)$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<
