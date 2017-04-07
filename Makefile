PROGRAM = spv
SRC_DIR = .

INCLUDEDIRS = \
	-I$(ARCENGINEHOME)/include \
	-I/usr/X11R6/include \
        $(shell pkg-config --cflags gtk+-3.0 poppler-glib pango)

LIBDIRS = \
	-L$(ARCENGINEHOME)/bin \
	-L/usr/X11R6/lib

LIBS = \
	$(shell pkg-config --libs gtk+-3.0 poppler-glib pango)

CCFLAGS = -DESRI_UNIX $(INCLUDEDIRS)
CC = gcc

OBJS	+= search.o 
OBJS	+= gui.o 
OBJS	+= highlight.o 
OBJS	+= main.o 
OBJS	+= note.o 
OBJS	+= pdf.o 
OBJS	+= zoom.o 
OBJS	+= page.o 

LDFLAGS = $(LIBDIRS) $(LIBS)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS):
%.o: %.c
	$(CC) -c  $(CCFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJS) $(PROGRAM) 
