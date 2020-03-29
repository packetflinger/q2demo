-include .config

CC ?= gcc
LD ?= ld
RM ?= rm -f
STRIP ?= strip
WINDRES ?= windres

CFLAGS ?= --std=c99 -D_POSIX_C_SOURCE=200112L

HEADERS := demo.h

OBJS :=	main.o \
		msg.o \
		parse.o \
		util.o \
		write.o

TARGET ?= q2demo
	
all: $(TARGET)

default: all

.PHONY: all default clean strip

# Define V=1 to show command line.
ifdef V
    Q :=
    E := @true
else
    Q := @
    E := @echo
endif

-include $(OBJS:.o=.d)

%.o: %.c $(HEADERS)
	$(E) [CC] $@
	$(Q)$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.rc
	$(E) [RC] $@
	$(Q)$(WINDRES) $(RCFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	$(E) [LD] $@
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	$(E) [CLEAN]
	$(Q)$(RM) *.o *.d $(TARGET)

strip: $(TARGET)
	$(E) [STRIP]
	$(Q)$(STRIP) $(TARGET)
