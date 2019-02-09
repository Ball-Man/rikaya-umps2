# Makefile for umps2 projects

# User settings
# Prefix of your installed mipsel cross compiler
XT_PRG_PREFIX = mipsel-linux-gnu-
# Name of the compiler
CC = $(XT_PRG_PREFIX)gcc
# Name of the linker
LD = $(XT_PRG_PREFIX)ld
# Extension of the used sources and binaries
SRC_EXTENSION = c
OBJ_EXTENSION = o
# Project directories
SRC_DIR = C
INCLUDE_DIR = H 
EXTERNAL_DIR = E
# Static libraries
LIB_DIR = LIB
# Built binaries
BUILD_DIR = BUILD

KERNEL = kernel
CORE_EXTENSION = core.umps

# Location of your umps2 installation(if installed in a standard way leave as it is)
UMPS2_DIR_PREFIX = /usr/local

#
# Implementation
#

# Recursive wildcard function
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Calculated sources and targets
SRCS = $(call rwildcard,$(SRC_DIR)/,*.$(SRC_EXTENSION))
OBJS = $(SRCS:$(SRC_DIR)/%.$(SRC_EXTENSION)=$(BUILD_DIR)/%.$(OBJ_EXTENSION))
LIBS = $(call rwildcard,$(LIB_DIR)/,*.$(OBJ_EXTENSION))

# Umps related paths
UMPS2_DATA_DIR = $(UMPS2_DIR_PREFIX)/share/umps2
UMPS2_INCLUDE_DIR = $(UMPS2_DIR_PREFIX)/include/umps2

# Compiler options
CFLAGS_LANG = -ffreestanding -ansi
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS2_INCLUDE_DIR) -I$(INCLUDE_DIR) -I$(EXTERNAL_DIR) -Wall -Wno-unused-function -O0

# Linker options
LDFLAGS = -G 0 -nostdlib -T $(UMPS2_DATA_DIR)/umpscore.ldscript

# Add the location of crt*.S to the search path
VPATH = $(UMPS2_DATA_DIR)

.PHONY : all clean

all : $(dir $(OBJS)) $(BUILD_DIR)/$(KERNEL).$(CORE_EXTENSION)

$(BUILD_DIR)/$(KERNEL).$(CORE_EXTENSION) : $(BUILD_DIR)/$(KERNEL)
	umps2-elf2umps -k $<

# Create missing directories
%/:
	@mkdir $@

$(BUILD_DIR)/$(KERNEL) : $(OBJS) $(LIBS)
	$(LD) -o $@ $^ $(LDFLAGS)

# Pattern rule for assembly modules
$(BUILD_DIR)/%.$(OBJ_EXTENSION) : $(SRC_DIR)/%.$(SRC_EXTENSION)
	$(CC) $(CFLAGS) -c -o $@ $<
	@$(CC) $(CFLAGS) $< -MM -MT$@ > $@.d

# Clean everything
clean :
	-rm -rf $(BUILD_DIR)

-include $(call rwildcard,$(BUILD_DIR)/,*.d)
