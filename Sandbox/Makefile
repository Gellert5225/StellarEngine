# Directory setup
BUILD_DIR = build
BIN_DIR = bin
RM = rm -f

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -O3
CPPFLAGS = -I../Stellar/src/ -I../Stellar/vendor/spdlog/include -I../Stellar/vendor/glfw/include -I../Stellar/vendor/glm -I../Stellar/vendor/imgui
LFLAGS = -L./bin
LIBS = -lStellar -lglfw
PWD = $(shell pwd)
EXPORT_FLAG =
SHADER_FLAG = glslc

# Other
BUILT_MSG = Sandbox has been built to bin
INSTALL_NAME_TOOL_STELLAR =
INSTALL_NAME_TOOL_GLFW =
CP_LIBS =

# OS check
ifeq ($(OS),Windows_NT) # OS is a preexisting environment variable on Windows
	OS = windows
	EXE_EXT = .exe
	CPPFLAGS += -I../Stellar/vendor/vulkan/win/Include
else
	UNAME := $(shell uname -s)
	EXPORT = export
	ifeq ($(UNAME),Darwin)
		BIN_DIR = ../macOS/StellarEngine.app/Contents/MacOS
		OS = macos
		EXPORT_FLAG = DYLD_LIBRARY_PATH

		INSTALL_NAME_TOOL_STELLAR = $(shell install_name_tool -change lib/libStellar.dylib  "@executable_path/../Frameworks/libStellar.dylib" ../macOS/StellarEngine.app/Contents/MacOS/Sandbox)
        INSTALL_NAME_TOOL_GLFW = $(shell install_name_tool -add_rpath "@executable_path/../Frameworks" ../macOS/StellarEngine.app/Contents/MacOS/Sandbox)
        CP_LIBS = $(shell cp ./bin/libStellar.dylib ../macOS/StellarEngine.app/Contents/Frameworks && cp ./bin/libglfw.3.dylib ../macOS/StellarEngine.app/Contents/Frameworks)
	else 
		ifeq ($(UNAME),Linux)
			OS = linux
			EXPORT_FLAG = LD_LIBRARY_PATH
			CPPFLAGS += -I../Stellar/vendor/vulkan/linux/include
		else
			$(error OS not supported by this Makefile)
		endif
	endif
endif

# Source files
SRC_DIR = src
SHADER_DIR = resource/Shader
SRCS := $(sort $(shell find $(SRC_DIR) -name '*.cpp'))
OBJS = $(patsubst %.cpp,build/%.o,$(SRCS))
VERT_SRCS := $(sort $(shell find $(SHADER_DIR) -name '*.vert'))
FRAG_SRCS := $(sort $(shell find $(SHADER_DIR) -name '*.frag'))
VERT_SPIRV = $(VERT_SRCS:%.vert=$(SHADER_DIR)/%.spv)
FRAG_SPIRV = $(FRAG_SRCS:%.frag=$(SHADER_DIR)/%.spv)

COMPILE_SHADERS = $(shell chmod +x resource/Shader/compile.sh && ./resource/Shader/compile.sh)

ifeq ($(OS),linux)
	LIBS += -lglfw -lvulkan
endif

# define the executable file 
MAIN = Sandbox

.SECONDARY:

include $(wildcard ../PrettyPrint.inc)

.PHONY: depend clean

all: $(BIN_DIR)/$(MAIN)
	$(CP_LIBS)
	$(INSTALL_NAME_TOOL_STELLAR)
	$(INSTALL_NAME_TOOL_GLFW)
	@echo Compiling shaders
	$(COMPILE_SHADERS)
	@echo Sandbox has been built to bin

override CPPFLAGS += -MMD
-include $(shell find build -name "*.d" 2> /dev/null)

$(BIN_DIR)/$(MAIN): $(OBJS)
	@mkdir -p '$(@D)'
	@$(call run_and_test,$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BIN_DIR)/$(MAIN) $(OBJS) $(LFLAGS) $(LIBS))

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	@$(call run_and_test, ($(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<  -o $@))

run:
	export $(EXPORT_FLAG)="${PWD}/bin"; \
	$(BIN_DIR)/Sandbox$(EXE_EXT)

run_mac:
	$(shell open ../macOS/StellarEngine.app)

clean:
	@echo "Cleaning"
	@$(RM) -r bin build lib
	@$(RM) ../macOS/StellarEngine.app/Contents/Frameworks/* ../macOS/StellarEngine.app/Contents/MacOS/*

depend: $(SRCS)
	makedepend $(INCLUDES) $^