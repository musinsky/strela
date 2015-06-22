CC		= gcc
GCC_MAJOR	:= $(shell $(CC) -dumpversion | cut -f1 -d '.')
GCC_OLD		:= $(shell [ $(GCC_MAJOR) -lt 5 ] && echo old)
ifeq ($(GCC_OLD),old)
CFLAGS		= -std=gnu99
endif
CFLAGS		+= -pedantic -Wall -Wextra -Wformat=2 -Wmissing-prototypes

SOURCES		= $(wildcard *.c)
HEADERS		= $(patsubst %.c, %.h, $(SOURCES))
OBJECTS		= $(patsubst %.c, %.o, $(SOURCES))
PROGRAM		= vmemonitor
DISTSRCNAME	= $(PROGRAM).$(shell date +%F).tar.xz

all:		default

default:	$(PROGRAM)

%.o:		%.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM):	$(OBJECTS)
		$(CC) $^ -o $@
		@echo "==> $@ done"

distsrc:
		@rm -f $(DISTSRCNAME)
		@tar --ignore-failed-read -cJvf $(DISTSRCNAME) \
		  $(SOURCES) $(HEADERS) Makefile
		@echo -e "\n$(DISTSRCNAME) done\n"

clean:
		@rm -fv $(OBJECTS)

distclean:
		@rm -fv $(PROGRAM) $(OBJECTS)
