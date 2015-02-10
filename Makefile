# @(#) 10 Feb 2015
# Top level Makefile for Strela

# Author: Jan Musinsky

include $(ROOTSYS)/etc/Makefile.arch
HdrSuf		= h
CXXFLAGS	+= -Wshadow -Woverloaded-virtual
CXXFLAGS	+= -std=c++11 #-Wno-deprecated-declarations
CXXFLAGS	+= -Wextra -Wformat=2 -Wreorder -Wpedantic #-Wno-vla -Wno-long-long
ifeq ($(MAKECMDGOALS),debug)
CXXFLAGS	+= -DDEBUG
endif

MODULES		= Strela VME

MODMAKEFILE	= Module.mk
LNKFILE		= LinkDef
DICTPREFIX	= G__
INCDIR		= include
OBJDIR		= build
LIBDIR		= lib
LIBPREFIX	= lib
DISTSRCNAME	= strela.$(shell date +%F).git$(shell git describe --always)
MAKEDEPEND	= rmkdepend
DEPENDFILE	= $(OBJDIR)/Make-depend
NODEPEND	= clean distclean distsrc showbuild

# all
ALLHDRS		:=
ALLLIBS		:=
ALLDIST		:= Makefile
ALLDEPEND	:=

# verbatim variables
define checkdir
@if [ ! -d $@ ]; then mkdir -p $(dir $@); fi
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
			@$(ROOTCINT) -f $@ -c -I$(INCDIR) $^

$(DICTPREFIX)%.$(ObjSuf) : $(DICTPREFIX)%.$(SrcSuf)
			$(checkdir)
			@$(CXX) $(CXXFLAGS) -I$(INCDIR) -I. -c $< -o $@
			@echo -e "$@ done"

$(LIBDIR)/$(LIBPREFIX)%.$(DllSuf) :
			$(checkdir)
			@rm -f $@
			@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@
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
		@echo "PLATFORM       = $(PLATFORM)"
		@echo "ARCH           = $(ARCH)"
		@echo ""
		@echo "CXX            = $(CXX)"
		@echo "CXXFLAGS       = $(CXXFLAGS)"
		@echo ""
		@echo "LD             = $(LD)"
		@echo "SOFLAGS        = $(SOFLAGS)"
		@echo "LDFLAGS        = $(LDFLAGS)"
		@echo ""
		@echo "INCDIR         = $(INCDIR)"
		@echo "ROOTCINT       = $(ROOTCINT)"
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
