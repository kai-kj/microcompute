#======================================================================================================================#
# CONFIG
#======================================================================================================================#

#---- BASIC -----------------------------------------------------------------------------------------------------------#

LIBRARY        := microcompute
FLAGS          := -std=c11 -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wno-unused-parameter -Wno-cast-qual -O3
DEFS           := $(CFLAGS)

ifeq ($(strip $(STANDALONE)),true)
DEFS += -DMC_STANDALONE_MODE
endif

#---- PROJECT STRUCTURE -----------------------------------------------------------------------------------------------#

INCLUDE_FOLDER := include
LIB_FOLDER     := lib
BUILD_FOLDER   := build
OUT_FOLDER     := out
SRC_FOLDER     := src

#======================================================================================================================#

CC                := gcc $(FLAGS) -I $(INCLUDE_FOLDER) -I $(SRC_FOLDER)
AR                := ar rcs
RM                := rm -rf
CP                := cp
MKDIR             := mkdir -p
SRC_SUB_FOLDERS   := $(shell find $(SRC_FOLDER)/ -type d)
BUILD_SUB_FOLDERS := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(SRC_SUB_FOLDERS))
C_FILES           := $(shell find $(SRC_FOLDER)/ -type f -name "*.c")
C_OBJECTS         := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(C_FILES)))
STATIC_LIB        := $(OUT_FOLDER)/lib$(LIBRARY).a

.PHONY: all library doc clean run_examples

all: library doc 

library: $(STATIC_LIB)

doc:
	lua include/k_tools/k_doc.lua $(SRC_FOLDER)/$(LIBRARY).h doc.md

run_examples: DEFS += -DMC_STANDALONE_MODE
run_examples: library
	cd examples; \
	make

$(OUT_FOLDER):
	$(MKDIR) $(OUT_FOLDER)

$(BUILD_SUB_FOLDERS):
	$(MKDIR) $(BUILD_SUB_FOLDERS)

$(C_OBJECTS): $(BUILD_SUB_FOLDERS) $(C_FILES)
	$(CC) $(DEFS) -c $(subst $(BUILD_FOLDER)/,$(SRC_FOLDER)/,$(subst .o,.c,$@)) -o $@

$(STATIC_LIB): $(OUT_FOLDER) $(C_OBJECTS)
	$(AR) $(STATIC_LIB) $(C_OBJECTS)
	$(CP) $(SRC_FOLDER)/$(LIBRARY).h $(OUT_FOLDER)/$(LIBRARY).h
	$(MKDIR) $(OUT_FOLDER)/k_tools
	$(CP) $(INCLUDE_FOLDER)/k_tools/*.h $(OUT_FOLDER)/k_tools

clean:
	$(RM) $(BUILD_FOLDER) $(OUT_FOLDER)