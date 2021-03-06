#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif

include	$(PSL1GHT)/ppu_rules

#---------------------------------------------------------------------------------
BUILD		:=	build
SOURCES		:=	$(wildcard $(BASEDIR)/*.cpp)

#---------------------------------------------------------------------------------
ifeq ($(strip $(PLATFORM)),)
#---------------------------------------------------------------------------------
export BASEDIR	:=	$(CURDIR)
export DEPS	:=	$(BASEDIR)/deps
export LIBS	:=	$(BASEDIR)/lib

#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
export LIBDIR	:=	$(LIBS)/$(PLATFORM)
export DEPSDIR	:=	$(DEPS)/$(PLATFORM)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

LIBRARY		:=	$(LIBDIR)/libMini2D

#---------------------------------------------------------------------------------
DEFINCS		:=	-I$(PORTLIBS)/include/freetype2 -I$(PORTLIBS)/include/ 
DEFINCS     +=  -I$(PORTLIBS)/include/Mini2D -I$(PORTLIBS)/include/
DEFINCS     +=  -I$(PSL1GHT)/ppu/include -I$(PSL1GHT)/ppu/include/rsx 
DEFINCS     +=  -I$(PSL1GHT)/ppu/include/lv2 -I$(PSL1GHT)/ppu/include/sys
DEFINCS		+=	-I$(BASEDIR)/ppu
INCLUDES	:=	$(DEFINCS)

CFLAGS		:=	-O2 -mregnames -Wall -mcpu=cell $(MACHDEP) $(INCLUDES) -Wl,-mcell
CXXFLAGS	:=	$(CFLAGS)

#---------------------------------------------------------------------------------
VPATH		:=	$(BASEDIR)

#---------------------------------------------------------------------------------
OBJS		:= 	spu_soundmodule.bin.o $(SOURCES:.cpp=.o)

#---------------------------------------------------------------------------------
all: install-headers ppu install
#---------------------------------------------------------------------------------


#---------------------------------------------------------------------------------
install-headers:
#---------------------------------------------------------------------------------
	@echo [INSTALL HEADERS]
	@[ -d $(PORTLIBS) ] || mkdir -p $(PORTLIBS)
	@[ ! -d $(PORTLIBS)/include/Mini2D ] ||  rm -rf $(PORTLIBS)/include/Mini2D
	@cp -frv Mini2D $(PORTLIBS)/include/Mini2D


#---------------------------------------------------------------------------------
ppu:
#---------------------------------------------------------------------------------
	@echo [BUILD LIB]
	@[ -d $(LIBS)/ppu ] || mkdir -p $(LIBS)/ppu
	@[ -d $(DEPS)/ppu ] || mkdir -p $(DEPS)/ppu
	@[ -d ppu ] || mkdir -p ppu
	@$(MAKE) --no-print-directory PLATFORM=ppu lib -C ppu -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
install:
#---------------------------------------------------------------------------------
	@echo [INSTALL LIB]
	@[ -d $(PORTLIBS)/lib ] || mkdir -p $(PORTLIBS)/lib
	@cp -frv $(CURDIR)/lib/ppu/*.a $(PORTLIBS)/lib

#---------------------------------------------------------------------------------
$(LIBRARY).a: $(OBJS)
#---------------------------------------------------------------------------------

.PHONY: lib ppu 

#---------------------------------------------------------------------------------
lib: $(LIBRARY).a
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
clean:
#---------------------------------------------------------------------------------
	@echo [CLEAN]
	@rm -rf ppu
	@rm -rf $(DEPS)
	@rm -rf $(LIBS)
	@rm -rf *~ */*~
	@rm -rf *.o

#---------------------------------------------------------------------------------
-include $(DEPSDIR)/*.d
#---------------------------------------------------------------------------------