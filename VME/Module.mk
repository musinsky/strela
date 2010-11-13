# @(#) 09 Jun 2008
# module for VME

# Author: Jan Musinsky

MODULE	= VME
MODDIR	= VME
FILES	= TVME TVirtualModule TModulePhTDC TModuleTDC96

LNKDEF	= $(MODDIR)/$(LNKFILE).$(HdrSuf)
SRCS	= $(patsubst %,$(MODDIR)/%.$(SrcSuf),$(FILES))
HDRS	= $(SRCS:.$(SrcSuf)=.$(HdrSuf))
DICT	= $(TMPDIR)/$(MODDIR)/$(DICTPREFIX)$(MODULE).$(SrcSuf)
DICTH	= $(DICT:.$(SrcSuf)=.$(HdrSuf))
DICTO	= $(DICT:.$(SrcSuf)=.$(ObjSuf))
OBJS	= $(patsubst %.$(SrcSuf),$(TMPDIR)/%.$(ObjSuf),$(SRCS))
MODLIB	= $(LIBDIR)/$(LIBPREFIX)$(MODULE).$(DllSuf)

VMECLEAN := $(OBJS) $(DICTO)
ifeq (distclean,$(findstring distclean,$(MAKECMDGOALS)))
VMECLEAN += $(DICT) $(DICTH) $(MODLIB)
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
		@rm -fv $(VMECLEAN)

clean:		clean-$(MODDIR)

distclean-$(MODDIR):
		@rm -fv $(VMECLEAN)

distclean:	distclean-$(MODDIR)
