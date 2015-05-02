# @(#) 02 May 2015
# module for VME

# Author: Jan Musinsky

MODULE	= VME
MODDIR	= VME
FILES	= TVME TVirtualModule TModulePhTDC TModuleTDC96 TModuleTDC64V TModuleTQDC16 \
	  TVMERawData TVirtualEvent TVirtualHit \
	  TEventTdc THitTdc TEventTqdcT THitTqdcT TEventTqdcQ THitTqdcQ

VMECLEAN	:= $(OBJS) $(DICT) $(DICTH) $(DICTO)
ifeq ($(MAKECMDGOALS),distclean)
VMECLEAN	+= $(MODLIB) $(subst $(MODDIR)/,$(INCDIR)/,$(HDRS)) # '/' important
endif

$(MODLIB)EXTRA	= -O2

# used in the main Makefile
ALLLIBS		+= $(MODLIB)
ALLHDRS		+= $(HDRS)
ALLDIST		+= $(SRCS) $(HDRS) $(LNKDEF) $(MODDIR)/$(MODMAKEFILE)
ALLDEPEND	+= $(SRCS)

# local rules
$(MODDIR):	$(MODLIB)

$(MODLIB):	$(OBJS) $(DICTO)

$(DICT):	$(HDRS) $(LNKDEF)

clean-$(MODDIR):
		@rm -fv $(VMECLEAN)

clean:		clean-$(MODDIR)

distclean-$(MODDIR):
		@rm -fv $(VMECLEAN)

distclean:	distclean-$(MODDIR)
