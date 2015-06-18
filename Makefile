# @(#) 18 Jun 2015
# Top level Makefile for Strela

# Author: Jan Musinsky

include $(ROOTSYS)/etc/Makefile.arch
HdrSuf		= h

ifeq ($(MAKECMDGOALS),debug)
CXXFLAGS	+= -DDEBUG
endif
CXXFLAGS	+= -Wshadow -Woverloaded-virtual
CXXFLAGS	+= -Wextra -Wformat=2 -Wreorder -Wpedantic
ifeq ($(ROOTCINT),) # ROOT6
CXXFLAGS	+= -Wdeprecated-declarations # enforce to enable
DICTCXXFLAGS	= -Wno-format-nonliteral # dict warning (enabled by format=2)
else
CXXFLAGS	+= -std=c++11
endif

MODULES		= Strela VME

MODMAKEFILE	= Module.mk
LNKFILE		= LinkDef
DICTPREFIX	= G__
MAPSUF		= rootmap
PCMSUF		= pcm
INCDIR		= include
OBJDIR		= build
LIBDIR		= lib
LIBPREFIX	= lib
DISTSRCNAME	= strela.$(shell date +%F).git$(shell git describe --always)
MAKEDEPEND	= rmkdepend
DEPENDFILE	= $(OBJDIR)/Make-depend
NODEPEND	= clean distclean distsrc showbuild

LNKDEF		= $(MODDIR)/$(LNKFILE).$(HdrSuf)
SRCS		= $(patsubst %,$(MODDIR)/%.$(SrcSuf),$(FILES))
HDRS		= $(SRCS:.$(SrcSuf)=.$(HdrSuf))
DICT		= $(OBJDIR)/$(MODDIR)/$(DICTPREFIX)$(MODULE).$(SrcSuf)
DICTH		= $(DICT:.$(SrcSuf)=.$(HdrSuf))
DICTO		= $(DICT:.$(SrcSuf)=.$(ObjSuf))
OBJS		= $(patsubst %.$(SrcSuf),$(OBJDIR)/%.$(ObjSuf),$(SRCS))
MODLIB		= $(LIBDIR)/$(LIBPREFIX)$(MODULE).$(DllSuf)

# all
ALLHDRS		:=
ALLLIBS		:=
ALLDIST		:= Makefile
ALLDEPEND	:=

# verbatim variables
DictOpt		= -s $(LIBDIR)/$1.$(DllSuf) -rml $1.$(DllSuf) -rmf $(LIBDIR)/$1.$(MAPSUF)

# define is just syntax to create (easier) variables with multi-line values
define checkdir
@if [ ! -d $@ ]; then mkdir -p $(dir $@); fi
endef

define checkdir1
@if [ ! -d $1 ]; then mkdir -p $1; fi
endef

define emptyfile
$(checkdir)
@if [ ! -e $@ ]; then > $@; fi
endef

# rules
.SUFFIXES: # delete the default suffixes
.PRECIOUS: $(INCDIR)/%.$(HdrSuf) # preserve intermediate files

$(OBJDIR)/%.$(ObjSuf) : %.$(SrcSuf)
		$(checkdir)
		@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@
		@echo -e "$@ done"

$(DICTPREFIX)%.$(SrcSuf) :
		@echo "Generating dictionary $@ ..."
		$(checkdir)
ifeq ($(ROOTCINT),)
		$(call checkdir1,$(LIBDIR)) # needed for pcm and rootmap
		@$(ROOTCLING) -f $@ $(call $@DictOpt) -c -I$(INCDIR) $^
else
		@$(ROOTCINT) -f $@ -c -I$(INCDIR) $^
endif

$(DICTPREFIX)%.$(ObjSuf) : $(DICTPREFIX)%.$(SrcSuf)
		$(checkdir)
		@$(CXX) $(CXXFLAGS) $(DICTCXXFLAGS) -I$(INCDIR) -I. -c $< -o $@
		@echo -e "$@ done"

$(LIBDIR)/$(LIBPREFIX)%.$(DllSuf) :
		$(checkdir)
		@rm -f $@
		@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(call $@Extra)
		@chmod 555 $@
		@echo "==> $@ done"

# targets
all:		libs
include	$(patsubst %,%/$(MODMAKEFILE),$(MODULES))

libs:		updateinc $(ALLLIBS)

updateinc:	$(ALLHDRS) # only for linux
		@if [ ! -d $(INCDIR) ]; then mkdir -p $(INCDIR); fi
		@cp -v -u $^ $(INCDIR)

debug:		all

clean:
		@rm -fv $(DEPENDFILE) $(DEPENDFILE).bak \
		  $(OBJDIR)/*.so $(OBJDIR)/*.d # foreign build files

distclean:	clean

distsrc:
		@rm -f $(DISTSRCNAME).tar.xz;
		@tar --ignore-failed-read -cJvf $(DISTSRCNAME).tar.xz \
		  $(ALLDIST) *.C macros/*.C macros/*.cxx sql README.md;
		@echo -e "\n$(DISTSRCNAME).tar.xz done\n"

showbuild:
		@echo "ROOTSYS        = $(realpath $(ROOTSYS))"
		@echo "ROOT VERSION   = $(shell $(RC) --version)"
		@echo "PLATFORM       = $(PLATFORM)"
		@echo "ARCH           = $(ARCH)"
		@echo ""
		@echo "CXX            = $(CXX)"
		@echo "CXX VERSION    = $(shell $(CXX) -dumpversion)"
		@echo "CXXFLAGS       = $(CXXFLAGS)"
		@echo "DICTCXXFLAGS   = $(DICTCXXFLAGS)"
		@echo ""
		@echo "LD             = $(LD)"
		@echo "SOFLAGS        = $(SOFLAGS)"
		@echo "LDFLAGS        = $(LDFLAGS)"
		@echo ""
		@echo "INCDIR         = $(INCDIR)"
ifeq ($(ROOTCINT),)
		@echo "ROOTDICT       = $(ROOTCLING)"
else
		@echo "ROOTDICT       = $(ROOTCINT)"
endif
		@echo "MAKEDEPEND     = $(MAKEDEPEND)"
		@echo ""
		@echo "The list of what to be built"
		@echo "============================"
		@echo "Modules:"
		@echo " $(word 1,$(MODULES))"
		@$(foreach mod,$(filter-out $(word 1,$(MODULES)),$(MODULES)), \
		  echo " $(mod)";)
		@echo "Libraries:"
		@echo " $(word 1,$(ALLLIBS))"
		@$(foreach lib,$(filter-out $(word 1,$(ALLLIBS)),$(ALLLIBS)), \
		  echo " $(lib)";)

$(DEPENDFILE):	$(ALLDEPEND)
		$(emptyfile)
		@$(MAKEDEPEND) -Y -f$@ -p$(dir $@) \
		  -- $(CXXFLAGS) -- $^ 2>/dev/null
		@echo "$@ done"

ifeq ($(findstring $(MAKECMDGOALS),$(NODEPEND)),)
-include $(DEPENDFILE)
endif
