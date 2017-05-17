#----------------------------
# Makefile
# Startup modules and LibLoad library setup
#----------------------------
CSTARTUP_ASM  := cstartup.asm
LIBHEADER_ASM := libheader.asm
#----------------------------

VERSION := 7.0

#----------------------------
# Try not to edit anything below these lines unless you know what you are doing
#----------------------------

#----------------------------

# define some common makefile things
empty :=
space := $(empty) $(empty)
comma := $(empty),$(empty)

ifeq ($(OS),Windows_NT)
NATIVEPATH = $(subst /,\,$(1))
WINPATH    = $(NATIVEPATH)
WINRELPATH = $(subst /,\,$(1))
WINCHKDIR  = if not exist
RM         = del /q /f 2>nul
CEDEV     ?= $(call NATIVEPATH,$(realpath ..\..))
BIN       ?= $(call NATIVEPATH,$(CEDEV)/bin)
AS         = $(call NATIVEPATH,$(BIN)/ez80asm.exe)
LD         = $(call NATIVEPATH,$(BIN)/ez80link.exe)
CC         = $(call NATIVEPATH,$(BIN)/ez80cc.exe)
CD         = cd
CP         = copy /y
CV         = $(call NATIVEPATH,$(BIN)/convhex.exe)
PG         = $(call NATIVEPATH,$(BIN)/convpng.exe)
NULL       = >nul 2>&1
RMDIR      = rmdir /s /q
MKDIR      = mkdir
chain     := &&
else
NATIVEPATH = $(subst \,/,$(1))
WINPATH    = $(subst \,\\,$(shell winepath --windows $(1)))
WINRELPATH = $(subst /,\,$(1))
RM         = rm -f
CEDEV     ?= $(call NATIVEPATH,$(realpath ..\..))
BIN       ?= $(call NATIVEPATH,$(CEDEV)/bin)
AS         = $(call NATIVEPATH,wine $(BIN)/ez80asm.exe)
LD         = $(call NATIVEPATH,wine $(BIN)/ez80link.exe)
CC         = $(call NATIVEPATH,wine $(BIN)/ez80cc.exe)
CD         = cd
CP         = cp
CV         = $(call NATIVEPATH,$(BIN)/convhex)
PG         = $(call NATIVEPATH,$(BIN)/convpng)
NULL       =
RMDIR      = rm -rf
MKDIR      = mkdir -p
chain     := ;
TESTDIR    = test -d $(1)
endif

# generate the default names for input and object files
TARGETHEX     := $(TARGET).hex
TARGETMAP     := $(TARGET).map
TARGETTYPE    := $(TARGET).8xp
ICON_ASM      := iconc.asm

# locations of the startup modules and LibLoad modules
LIBHEADER_LOC := $(call NATIVEPATH,$(CEDEV)/lib/asm/$(LIBHEADER_ASM))
CSTARTUP_LOC  := $(call NATIVEPATH,$(CEDEV)/lib/asm/$(CSTARTUP_ASM))

# default objects
ICON_OBJ      := $(ICON_ASM:%.asm=%.obj)
CSTARTUP_OBJ  := $(CSTARTUP_ASM:%.asm=%.obj)
LIBHEADER_OBJ := $(LIBHEADER_ASM:%.asm=%.obj)


#Find all the directories in the source input
ALLDIRS       := $(sort $(dir $(wildcard $(SRCDIR)/*/)))

#Determine if output should be archived or compressed
ifeq ($(ARCHIVED),YES)
CVFLAGS       := -a
endif
ifeq ($(COMPRESSED),YES)
CVFLAGS       += -x
endif

#Find all of the available C and ASM files (Remember, you can create C <-> assembly routines easily this way)
SOURCES       := $(call NATIVEPATH,$(foreach dir,$(ALLDIRS),$(wildcard $(dir)*.c)))
ASMSOURCES    := $(call NATIVEPATH,$(foreach dir,$(ALLDIRS),$(wildcard $(dir)*.asm)))

#Figure out what the names of the sources will become once made into objects
OBJECTS       := $(addprefix $(OBJDIR)/,$(notdir $(SOURCES:%.c=%.obj)))
OBJECTS       += $(addprefix $(OBJDIR)/,$(notdir $(ASMSOURCES:%.asm=%.obj)))
CSTARTUPOBJ   := $(OBJDIR)/$(CSTARTUP_OBJ)

#Check if there is an icon present that we can convert; if so, generate a recipe to build it properly
ifneq ("$(wildcard $(ICONPNG))","")
ICON_CONV     := $(PG) -c $(ICONPNG)$(comma)$(DESCRIPTION) $(chain) \
                 $(CP) $(ICON_ASM) $(OBJDIR) $(NULL) $(chain) \
	             $(RM) $(ICON_ASM) $(NULL)
OBJECTS       += $(OBJDIR)/$(ICON_OBJ)
ICONC         := ICON
else
ICON_CONV     :=
endif

#Are there any shared libraries that we need to find? If so, add in the proper objects and sources and header files
ifdef L
LIBS          := $(call NATIVEPATH,$(strip $(foreach var,$(L),$(space)$(CEDEV)/lib/asm/$(var)_header.asm)))
LOBJECTS      := $(call NATIVEPATH,$(addprefix $(OBJDIR)/,$(notdir $(LIBS:%.asm=%.obj))))
LIBHOBJ       := $(call NATIVEPATH,$(OBJDIR)/$(LIBHEADER_OBJ))
LIBS          := $(call NATIVEPATH,$(foreach var,$(L),$(CEDEV)/lib/asm/$(var)_header.asm$(space)))
endif

#Define the nesassary headers, along with any the user may have defined, where modification should just trigger a build
USERHEADERS   := $(call NATIVEPATH,$(foreach dir,$(ALLDIRS),$(wildcard $(dir)*.h)))
HEADERS       := $(subst $(space),;,$(call WINPATH,$(ALLDIRS) $(addprefix $(CEDEV)/,include include/compat include/ce include/std)))
HEADERS       := $(subst \;,;,$(HEADERS))
HEADERS       := $(subst \;,;,$(HEADERS))
HEADERS       := $(subst /;,;,$(HEADERS))

ifeq ($(USE_FLASH_FUNCTIONS),YES)
LINKED_OS_FUN := clinked.lib
else
LINKED_OS_FUN := cstatic.lib
endif

LIBRARIES     := $(call NATIVEPATH,$(addprefix $(CEDEV)/lib/,cce.lib cshared.lib $(LINKED_OS_FUN)))

ifdef L
LIBRARIES     += $(call NATIVEPATH,$(foreach var,$(L),$(CEDEV)/lib/$(var).lib))
endif

#Define the assembler flags used by the Zilog assembler
ASM_FLAGS       := \
	-define:_EZ80 -define:$(DEBUGMODE) -define:$(ICONC) -NOlist -NOlistmac \
	-pagelen:250 -pagewidth:130 -quiet -sdiopt -warn -NOdebug -NOigcase -cpu:EZ80F91

#Define the C flags used by the Zilog compiler
CFLAGS := \
	-define:_EZ80 -define:$(DEBUGMODE) -define:_EZ80F91 -NOlistinc -NOmodsect -cpu:EZ80F91 -keepasm \
	-optspeed -NOreduceopt -NOgenprintf -quiet -stdinc:"$(HEADERS)" -NOdebug -asmsw:"$(ASM_FLAGS)"

#This is a giant mess, but basically it is just repeated code to properly align all of the necessary libraries and
#their header information. Kind of annoying, but this is the only certain way ZDS is able to organize things properly
#Currently 11 libraries are supported; more can be added later if needed (probably not)
ifdef L
LIBNUM := $(words $(L))
LDLIBS := locate .$(word 1,$(L))_header at (top of .libs)+1
LDLIBS += locate .$(word 1,$(L)) at (top of .$(word 1,$(L))_header)+1
ifneq ($(LIBNUM),1)
LDLIBS += locate .$(word 2,$(L))_header at (top of .$(word 1,$(L))+1)
LDLIBS += locate .$(word 2,$(L)) at (top of .$(word 2,$(L))_header)+1
ifneq ($(LIBNUM),2)
LDLIBS += locate .$(word 3,$(L))_header at (top of .$(word 2,$(L))+1)
LDLIBS += locate .$(word 3,$(L)) at (top of .$(word 3,$(L))_header)+1
ifneq ($(LIBNUM),3)
LDLIBS += locate .$(word 4,$(L))_header at (top of .$(word 3,$(L))+1)
LDLIBS += locate .$(word 4,$(L)) at (top of .$(word 4,$(L))_header)+1
ifneq ($(LIBNUM),4)
LDLIBS += locate .$(word 5,$(L))_header at (top of .$(word 4,$(L))+1)
LDLIBS += locate .$(word 5,$(L)) at (top of .$(word 5,$(L))_header)+1
ifneq ($(LIBNUM),5)
LDLIBS += locate .$(word 6,$(L))_header at (top of .$(word 5,$(L))+1)
LDLIBS += locate .$(word 6,$(L)) at (top of .$(word 6,$(L))_header)+1
ifneq ($(LIBNUM),6)
LDLIBS += locate .$(word 7,$(L))_header at (top of .$(word 6,$(L))+1)
LDLIBS += locate .$(word 7,$(L)) at (top of .$(word 7,$(L))_header)+1
ifneq ($(LIBNUM),7)
LDLIBS += locate .$(word 8,$(L))_header at (top of .$(word 7,$(L))+1)
LDLIBS += locate .$(word 8,$(L)) at (top of .$(word 8,$(L))_header)+1
ifneq ($(LIBNUM),8)
LDLIBS += locate .$(word 9,$(L))_header at (top of .$(word 8,$(L))+1)
LDLIBS += locate .$(word 9,$(L)) at (top of .$(word 9,$(L))_header)+1
ifneq ($(LIBNUM),9)
LDLIBS += locate .$(word 10,$(L))_header at (top of .$(word 9,$(L))+1)
LDLIBS += locate .$(word 10,$(L)) at (top of .$(word 10,$(L))_header)+1
ifneq ($(LIBNUM),10)
LDLIBS += locate .$(word 11,$(L))_header at (top of .$(word 10,$(L))+1)
LDLIBS += locate .$(word 11,$(L)) at (top of .$(word 11,$(L))_header)+1
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
LDLAST := .$(word $(words $(L)),$(L))
else
LDLAST := .libs
endif

#These are the linker flags, basically organized to properly set up the environment
LDFLAGS := \
	-FORMAT=INTEL32 \
	-map -maxhexlen=64 -quiet -warnoverlap -xref -unresolved=fatal \
	-sort ADDRESS=ascending -warn -NOdebug -NOigcase \
	define __copy_code_to_ram = 0 \
	range rom $$000000 : $$3FFFFF \
	range ram $$D00000 : $$FFFFFF \
	range bss $$$(BSSHEAP_LOW) : $$$(BSSHEAP_HIGH) \
	change code is ram \
	change data is ram \
	change text is ram \
	change strsect is text \
	define __low_bss = base of bss \
	define __len_bss = length of bss \
	define __heaptop = (highaddr of bss) \
	define __heapbot = (top of bss)+1 \
	define __stack = $$$(STACK_HIGH) \
	locate .header at $$$(INIT_LOC) \
	locate .icon at (top of .header)+1 \
	locate .launcher at (top of .icon)+1 \
	locate .libs at (top of .launcher)+1 \
	$(LDLIBS) \
	locate .startup at (top of $(LDLAST))+1 \
	locate code at (top of .startup)+1 \
	locate data at (top of code)+1 \
	locate text at (top of data)+1

ifneq ($(OS),Windows_NT)
	LDFLAGS := '$(LDFLAGS)'
else
	WINCHKBINDIR := $(WINCHKDIR) $(BINDIR)
	WINCHKOBJDIR := $(WINCHKDIR) $(OBJDIR)
endif

#This rule is trigged to build everything
all: add_dirs $(BINDIR)/$(TARGET8XP)
	@echo Finished.

add_dirs:
	$(WINCHKBINDIR) $(MKDIR) $(BINDIR)
	$(WINCHKOBJDIR) $(MKDIR) $(OBJDIR)
	@echo C CE SDK Version $(VERSION)
	@echo Looking for changes...

$(BINDIR)/$(TARGET8XP): $(BINDIR)/$(TARGETHEX)
	@$(CD) $(BINDIR) $(chain) \
	$(CV) $(CVFLAGS) $(notdir $<)

$(BINDIR)/$(TARGETHEX): $(CSTARTUPOBJ) $(LIBHOBJ) $(OBJECTS)
	@$(LD) $(LDFLAGS) $@ = "$(subst $(space),$(comma),$(strip $(call NATIVEPATH,$^) $(LOBJECTS) $(LIBRARIES)))"

#This rule handles conversion of the icon, if it is ever updated
$(OBJDIR)/$(ICON_OBJ): $(ICONPNG)
	@$(ICON_CONV) $(chain) \
	$(CD) $(OBJDIR) $(chain) \
	$(AS) $(ASM_FLAGS) $(ICON_ASM)

#This rule builds the assembly files and places them in the object directory
$(OBJDIR)/%.obj: $(SRCDIR)/%.asm
	@$(CD) $(OBJDIR) $(chain) \
	$(AS) $(ASM_FLAGS) $(call WINPATH,$(addprefix $(CURDIR)/,$<))

#These rules compile the source files into object files
$(OBJDIR)/%.obj: $(SRCDIR)/%.c $(USERHEADERS)
	@$(CD) $(OBJDIR) $(chain) \
	$(CC) $(CFLAGS) $(call WINPATH,$(addprefix $(CURDIR)/,$<))

$(OBJDIR)/%.obj: $(GFXDIR)/%.c $(USERHEADERS)
	@$(CD) $(OBJDIR) $(chain) \
	$(CC) $(CFLAGS) $(call WINPATH,$(addprefix $(CURDIR)/,$<))

$(CSTARTUPOBJ): $(CSTARTUP_LOC)
	@$(CD) $(OBJDIR) $(chain) \
	$(AS) $(ASM_FLAGS) $(call WINPATH,$<)

$(LIBHOBJ): $(LIBHEADER_LOC) $(LIBS)
	@$(CD) $(OBJDIR) $(chain) \
	$(AS) $(ASM_FLAGS) $(call WINPATH,$<)

$(LIBS):
	@$(CD) $(OBJDIR) $(chain) \
	$(AS) $(ASM_FLAGS) $(call WINPATH,$@)

clean:
	@$(RM) $(call NATIVEPATH,$(BINDIR)/$(TARGETHEX) $(BINDIR)/$(TARGETTYPE) $(BINDIR)/$(TARGETMAP) $(CSTARTUPOBJ) $(LIBHOBJ) $(OBJDIR)/*.obj $(OBJDIR)/*.src $(OBJDIR)/*.asm)
	@echo Cleaned build files.

version:
	@echo C SDK Version $(VERSION)

.PHONY: all clean version add_dirs $(LIBS)

