# @(#) 02 May 2015
# module for Strela

# Author: Jan Musinsky

MODULE	= Strela
MODDIR	= Strela
FILES	= TStrela TStrelaBase TStrelaDisplay \
	  TPommeEvent TGemEvent TStrawEvent TWirePoint TWireCham \
	  TStrawTrack TStrawTube TStrawMulti TStrawLayer TStrawTracker TStrawCham

STRELACLEAN	:= $(OBJS) $(DICT) $(DICTH) $(DICTO)
ifeq ($(MAKECMDGOALS),distclean)
STRELACLEAN	+= $(MODLIB) $(subst $(MODDIR)/,$(INCDIR)/,$(HDRS)) # '/' important
endif

$(MODLIB)EXTRA	= -O1

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
		@rm -fv $(STRELACLEAN)

clean:		clean-$(MODDIR)

distclean-$(MODDIR):
		@rm -fv $(STRELACLEAN)

distclean:	distclean-$(MODDIR)
