# @(#) 09 Jun 2008
# module for Strela

# Author: Jan Musinsky

MODULE	= Strela
MODDIR	= Strela
FILES	= TStrela TStrelaBase TStrelaDisplay \
	  TPommeEvent TGemEvent TStrawEvent \
	  TWirePoint TWireCham \
	  TStrawTrack TStrawTube TStrawMulti TStrawLayer TStrawTracker \
	  TStrawCham

LNKDEF	= $(MODDIR)/$(LNKFILE).$(HdrSuf)
SRCS	= $(patsubst %,$(MODDIR)/%.$(SrcSuf),$(FILES))
HDRS	= $(SRCS:.$(SrcSuf)=.$(HdrSuf))
DICT	= $(TMPDIR)/$(MODDIR)/$(DICTPREFIX)$(MODULE).$(SrcSuf)
DICTH	= $(DICT:.$(SrcSuf)=.$(HdrSuf))
DICTO	= $(DICT:.$(SrcSuf)=.$(ObjSuf))
OBJS	= $(patsubst %.$(SrcSuf),$(TMPDIR)/%.$(ObjSuf),$(SRCS))
MODLIB	= $(LIBDIR)/$(LIBPREFIX)$(MODULE).$(DllSuf)

STRELACLEAN := $(OBJS) $(DICTO)
ifeq (distclean,$(findstring distclean,$(MAKECMDGOALS)))
STRELACLEAN += $(DICT) $(DICTH) $(MODLIB)
endif

# used in the main Makefile
ALLLIBS	+= $(MODLIB)
ALLHDRS	+= $(HDRS)
ALLDIST	+= $(SRCS) $(HDRS) $(LNKDEF) $(MODDIR)/$(MODMAKEFILE)
ALLDEPEND += $(SRCS)

$(MODDIR):	$(MODLIB)

$(MODLIB):	$(OBJS) $(DICTO)

$(DICT):	$(HDRS) $(LNKDEF)

clean-$(MODDIR):
		@rm -fv $(STRELACLEAN)

clean:		clean-$(MODDIR)

distclean-$(MODDIR):
		@rm -fv $(STRELACLEAN)

distclean:	distclean-$(MODDIR)
