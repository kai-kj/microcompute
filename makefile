#======================================================================================================================#
# CONFIG
#======================================================================================================================#

#---- BASIC -----------------------------------------------------------------------------------------------------------#

EXECUTABLE     := micro_compute_test
LIBS           := -lgbm -lEGL -lGL -lGLEW
FLAGS          := -Wall -Wextra -Wno-missing-braces -Wno-unused-parameter -g
DEFS           := -D PRINT_DBG_LOGS

#---- PROJECT STRUCTURE -----------------------------------------------------------------------------------------------#

INCLUDE_FOLDER := include
LIB_FOLDER     := lib
BUILD_FOLDER   := build
SRC_FOLDER     := src

#======================================================================================================================#

GCC           := gcc $(FLAGS) $(DEFS) -isystem $(INCLUDE_FOLDER) -I $(SRC_FOLDER)
GPP           := g++ $(FLAGS) $(DEFS) -isystem $(INCLUDE_FOLDER) -I $(SRC_FOLDER)
DB            := gdb -q -ex "run" -iex "set confirm off" # gdb will run the binary immediately
MV            := mv
RM            := rm -rf
CP            := cp
MKDIR         := mkdir -p
SRC_FOLDERS   := $(shell find $(SRC_FOLDER)/ -type d)
BUILD_FOLDERS := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(SRC_FOLDERS))
C_FILES       := $(shell find $(SRC_FOLDER)/ -type f -name "*.c")
CPP_FILES     := $(shell find $(SRC_FOLDER)/ -type f -name "*.cpp")
C_OBJECTS     := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o, $(C_FILES)))
CPP_OBJECTS   := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .cpp,.o,$(CPP_FILES)))

export __GLX_VENDOR_LIBRARY_NAME = nvidia
export __NV_PRIME_RENDER_OFFLOAD = 1

define \n


endef

.PHONY: clean

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

debug: $(EXECUTABLE)
	$(DB) $(EXECUTABLE)

doc:
	doxygen

$(BUILD_FOLDERS):
	$(MKDIR) $(BUILD_FOLDERS)

$(C_OBJECTS): $(BUILD_FOLDERS) $(C_FILES)
	$(GCC) -c $(subst $(BUILD_FOLDER)/,$(SRC_FOLDER)/,$(subst .o,.c,$@)) -o $@

$(CPP_OBJECTS): $(BUILD_FOLDERS) $(CPP_FILES)
	$(GPP) -c $(subst $(BUILD_FOLDER)/,$(SRC_FOLDER)/,$(subst .o,.cpp,$@)) -o $@

$(EXECUTABLE): $(C_OBJECTS) $(CPP_OBJECTS)
	$(GPP) $(C_OBJECTS) $(CPP_OBJECTS) -o $(EXECUTABLE) -L$(LIB_FOLDER) $(LIBS)

clean:
	$(RM) $(EXECUTABLE) $(BUILD_FOLDER) out/*