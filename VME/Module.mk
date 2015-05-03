# @(#) 03 May 2015
# module for VME

# Author: Jan Musinsky

MODULE	= VME
MODDIR	= VME
FILES	= TVME TVirtualModule TModulePhTDC TModuleTDC96 TModuleTDC64V TModuleTQDC16 \
	  TVMERawData TVirtualEvent TVirtualHit \
	  TEventTdc THitTdc TEventTqdcT THitTqdcT TEventTqdcQ THitTqdcQ

VMEDOPT		:= $(LIBPREFIX)$(MODULE)
VMECLEAN	:= $(OBJS) $(DICT) $(DICTH) $(DICTO)
ifneq (,$(findstring distclean,$(MAKECMDGOALS)))
VMECLEAN	+= $(MODLIB) $(subst $(MODDIR)/,$(INCDIR)/,$(HDRS)) # '/' important
VMECLEAN	+= $(subst $(DllSuf),$(MAPSUF),$(MODLIB))
VMECLEAN	+= $(subst .$(DllSuf),*.$(PCMSUF),$(MODLIB))
endif

# used in the main Makefile
ALLLIBS		+= $(MODLIB)
ALLHDRS		+= $(HDRS)
ALLDIST		+= $(SRCS) $(HDRS) $(LNKDEF) $(MODDIR)/$(MODMAKEFILE)
ALLDEPEND	+= $(SRCS)

$(DICT)DictOpt	= $(call DictOpt,$(VMEDOPT))
#$(MODLIB)Extra	= # nothing yet

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
