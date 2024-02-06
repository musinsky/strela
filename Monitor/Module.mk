# @(#) 27 Aug 2015
# module for Monitor

# Author: Jan Musinsky

MODULE	= Monitor
MODDIR	= Monitor
FILES	= TVMEServer

MONITORDOPT	:= $(LIBPREFIX)$(MODULE)
MONITORCLEAN	:= $(OBJS) $(DICT) $(DICTH) $(DICTO)
ifneq (,$(findstring distclean,$(MAKECMDGOALS)))
MONITORCLEAN	+= $(MODLIB) $(subst $(MODDIR)/,$(INCDIR)/,$(HDRS)) # '/' important
MONITORCLEAN	+= $(subst $(DllSuf),$(MAPSUF),$(MODLIB))
MONITORCLEAN	+= $(subst .$(DllSuf),*.$(PCMSUF),$(MODLIB))
endif

# used in the main Makefile
ALLLIBS		+= $(MODLIB)
ALLHDRS		+= $(HDRS)
ALLDIST		+= $(SRCS) $(HDRS) $(LNKDEF) $(MODDIR)/$(MODMAKEFILE)
ALLDEPEND	+= $(SRCS)

$(DICT)DictOpt	= $(call DictOpt,$(MONITORDOPT))
#$(MODLIB)Extra	= # nothing yet

# local rules
$(MODDIR):	$(MODLIB)

$(MODLIB):	$(OBJS) $(DICTO)

$(DICT):	$(HDRS) $(LNKDEF)

clean-$(MODDIR):
		@rm -fv $(MONITORCLEAN)

clean:		clean-$(MODDIR)

distclean-$(MODDIR):
		@rm -fv $(MONITORCLEAN)

distclean:	distclean-$(MODDIR)
