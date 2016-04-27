IDIR=-I/usr/include
CC=gcc
CFLAGS=$(IDIR) -fPIC -g -O0 -Wno-deprecated-declarations -pthread
LDFLAGS=-shared

ODIR=obj
#LDIR =../lib

$(shell [ -d "$(ODIR)" ] || mkdir -p $(ODIR))

#LIBS=-lm

#_DEPS = hellomake.h
#DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = memtool.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

memtool.so: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDFLAGS)

.PHONY: clean

clean:
		rm -f $(ODIR)/*.o core memtool.so
