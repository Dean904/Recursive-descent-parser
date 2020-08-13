SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
DEPS=$(SOURCES:.cpp=.d)
BINS=$(SOURCES:.cpp=)

CFLAGS+=-MMD
CXXFLAGS+=-g -std=c++11 -w -Wall -pedantic

all: $(BINS)

.PHONY: clean

clean:
	$(RM) $(OBJECTS) $(DEPS) $(BINS)

-include $(DEPS)
