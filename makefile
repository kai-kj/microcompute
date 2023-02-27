#======================================================================================================================#
# CONFIG
#======================================================================================================================#

#---- BASIC -----------------------------------------------------------------------------------------------------------#

LIBRARY        := microcompute
LIBS           := -lmicrocompute -lgbm -lEGL -lGL -lGLEW
FLAGS          := -Wall -Wextra -Wno-missing-braces -Wno-unused-parameter
DEFS           := 

#---- PROJECT STRUCTURE -----------------------------------------------------------------------------------------------#

INCLUDE_FOLDER := include
LIB_FOLDER     := out
BUILD_FOLDER   := build
SRC_FOLDER     := src
OUT_FOLDER     := out
EXAMPLE_FOLDER := example

#======================================================================================================================#

CC                := gcc $(FLAGS) $(DEFS) -isystem $(INCLUDE_FOLDER) -I $(SRC_FOLDER)
AR                := ar rcs
RM                := rm -rf
CP                := cp
MKDIR             := mkdir -p
SRC_FOLDERS       := $(shell find $(SRC_FOLDER)/ -type d)
BUILD_SUB_FOLDERS := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(SRC_FOLDERS))
C_FILES           := $(shell find $(SRC_FOLDER)/ -type f -name "*.c")
C_OBJECTS         := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(C_FILES)))
EXAMPLE_C_FILES   := $(shell find $(EXAMPLE_FOLDER)/ -type f -name "*.c")
EXAMPLES          := $(subst $(EXAMPLE_FOLDER)/,$(OUT_FOLDER)/,$(subst .c,,$(EXAMPLE_C_FILES)))
STATIC_LIB        := $(OUT_FOLDER)/lib$(LIBRARY).a

export __GLX_VENDOR_LIBRARY_NAME = nvidia
export __NV_PRIME_RENDER_OFFLOAD = 1

.PHONY: all doc clean

all: $(EXAMPLES) $(STATIC_LIB)

lib: $(STATIC_LIB)

$(BUILD_FOLDER):
	$(MKDIR) $(BUILD_FOLDER)

$(OUT_FOLDER):
	$(MKDIR) $(OUT_FOLDER)

$(BUILD_SUB_FOLDERS): $(BUILD_FOLDER)
	$(MKDIR) $(BUILD_SUB_FOLDERS)

$(C_OBJECTS): $(BUILD_SUB_FOLDERS) $(C_FILES)
	$(CC) -c $(subst $(BUILD_FOLDER)/,$(SRC_FOLDER)/,$(subst .o,.c,$@)) -o $@

$(STATIC_LIB): $(OUT_FOLDER) $(C_OBJECTS)
	$(AR) $(STATIC_LIB) $(C_OBJECTS)
	$(CP) $(SRC_FOLDER)/$(LIBRARY).h $(OUT_FOLDER)/$(LIBRARY).h

$(EXAMPLES): $(STATIC_LIB)
	$(CC) -g $(subst $(OUT_FOLDER)/,$(EXAMPLE_FOLDER)/,$@).c -o $@ -L$(LIB_FOLDER) $(LIBS)

doc:
	python3 microdoc/doc_generator.py $(SRC_FOLDER)/$(LIBRARY).h doc.md

clean:
	$(RM) $(BUILD_FOLDER) $(OUT_FOLDER)