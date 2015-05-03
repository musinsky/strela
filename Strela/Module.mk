# @(#) 03 May 2015
# module for Strela

# Author: Jan Musinsky

MODULE	= Strela
MODDIR	= Strela
FILES	= TStrela TStrelaBase TStrelaDisplay \
	  TPommeEvent TGemEvent TStrawEvent TWirePoint TWireCham \
	  TStrawTrack TStrawTube TStrawMulti TStrawLayer TStrawTracker TStrawCham

STRELADOPT	:= $(LIBPREFIX)$(MODULE)
STRELACLEAN	:= $(OBJS) $(DICT) $(DICTH) $(DICTO)
ifneq (,$(findstring distclean,$(MAKECMDGOALS)))
STRELACLEAN	+= $(MODLIB) $(subst $(MODDIR)/,$(INCDIR)/,$(HDRS)) # '/' important
STRELACLEAN	+= $(subst $(DllSuf),$(MAPSUF),$(MODLIB))
STRELACLEAN	+= $(subst .$(DllSuf),*.$(PCMSUF),$(MODLIB))
endif

# used in the main Makefile
ALLLIBS		+= $(MODLIB)
ALLHDRS		+= $(HDRS)
ALLDIST		+= $(SRCS) $(HDRS) $(LNKDEF) $(MODDIR)/$(MODMAKEFILE)
ALLDEPEND	+= $(SRCS)

$(DICT)DictOpt	= $(call DictOpt,$(STRELADOPT))
#$(MODLIB)Extra	= # nothing yet

# local rules
$(MODDIR):	$(MODLIB)

$(MODLIB):	$(OBJS) $(DICTO)

$(DICT):	$(HDRS) $(LNKDEF)

clean-$(MODDIR):
		@rm -fv $(STRELACLEAN)

clean:		clean-$(MODDIR)

distclean-$(MODDIR):
		@rm -fv $(STRELACLEAN)

distclean:	distclean-$(MODDIR)
