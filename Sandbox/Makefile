DBG_OPT_FLAGS=-g
ASAN_FLAGS=-fsanitize=address

CXX = 
CXXFLAGS = -std=c++17 -Wall -MD
CPPFLAGS = -I../Stellar/src/ -I../Stellar/vendor/spdlog/include -I../Stellar/vendor/glfw/include -I../Stellar/vendor/glm -I../Stellar/vendor/imgui
LFLAGS = 
LIBS = 
PWD = $(shell pwd)
EXPORT_FLAG =
SHADER_FLAG = glslc

# Other
BUILT_MSG = Sandbox has been built to bin
INSTALL_NAME_TOOL_STELLAR =
INSTALL_NAME_TOOL_GLFW =
CP_LIBS =
CP_SHADERS = 
RM       	= rm -f
OUT_DIR   = 
BUILD_MODE = 
BIN_DIR = bin

ifeq ($(BUILD),release)
	BUILD_MODE = release
	LIB_NAME = Stellar
	OUT_DIR = release
	BIN_DIR = $(OUT_DIR)/bin
 	CXXFLAGS += -O3 -DNDEBUG
	LFLAGS = -L$(OUT_DIR)/bin
	LIBS = -lStellar
else
	BUILD_MODE = debug
	LIB_NAME = Stellar_debug
 	OUT_DIR = debug
	BIN_DIR = $(OUT_DIR)/bin
 	CXXFLAGS += -g -O1
	LFLAGS = -L$(OUT_DIR)/bin
	LIBS = -lStellar_debug
endif

# OS check
ifeq ($(OS),Windows_NT)
	OS = windows
	EXE_EXT = .exe
	CPPFLAGS += -I../Stellar/vendor/vulkan/win/Include
	CXX = g++
	LIBS += -lglfw3
else
	UNAME := $(shell uname -s)
	EXPORT = export
	ifeq ($(UNAME),Darwin)
		ifeq ($(BUILD_MODE),release)
			BIN_DIR = ../macOS/StellarEngine.app/Contents/MacOS
			INSTALL_NAME_TOOL_STELLAR = $(shell install_name_tool -change $(OUT_DIR)/bin/libStellar.dylib  "@executable_path/../Frameworks/libStellar.dylib" ../macOS/StellarEngine.app/Contents/MacOS/Sandbox)
			INSTALL_NAME_TOOL_GLFW = $(shell install_name_tool -add_rpath "@executable_path/../Frameworks" ../macOS/StellarEngine.app/Contents/MacOS/Sandbox)
			CP_LIBS = $(shell mkdir -p ../macOS/StellarEngine.app/Contents/Frameworks &&  cp $(OUT_DIR)/bin/libStellar.dylib ../macOS/StellarEngine.app/Contents/Frameworks && cp $(OUT_DIR)/bin/libglfw.3.dylib ../macOS/StellarEngine.app/Contents/Frameworks)
			CP_SHADERS = $(shell cp -r ./Resources/Shader ../macOS/StellarEngine.app/Contents/Resources)
		endif
		OS = macos
		EXPORT_FLAG = DYLD_LIBRARY_PATH
		LIBS += -lglfw.3
		CPPFLAGS += -I../Stellar/vendor/metal-cpp
		CXX = clang++
	else 
		ifeq ($(UNAME),Linux)
			OS = linux
			EXPORT_FLAG = LD_LIBRARY_PATH
			CPPFLAGS += -I../Stellar/vendor/vulkan/linux/include
			CXX = g++
		else
			$(error OS not supported by this Makefile)
		endif
	endif
endif

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
SRC_FILES := $(call rwildcard,src/,*.cpp)
# Source files
SRC_DIR = src
SHADER_DIR = resource/Shader
SRCS := $(call rwildcard,src/,*.cpp)
OBJS = $(patsubst %.cpp,$(OUT_DIR)/build/%.o,$(SRCS))
VERT_SRCS := $(call rwildcard,$(SHADER_DIR)/,*.vert)
FRAG_SRCS := $(call rwildcard,$(SHADER_DIR)/,*.frag)
VERT_SPIRV = $(VERT_SRCS:%.vert=$(SHADER_DIR)/%.spv)
FRAG_SPIRV = $(FRAG_SRCS:%.frag=$(SHADER_DIR)/%.spv)

COMPILE_SHADERS = $(shell chmod +x Resources/Shader/compile.sh && ./Resources/Shader/compile.sh)

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
	$(CP_SHADERS)
	@echo Compiling shaders
	$(COMPILE_SHADERS)
	@echo Sandbox has been built to bin

$(BIN_DIR)/$(MAIN): $(OBJS)
	@mkdir -p '$(@D)'
	@$(call run_and_test,$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(BIN_DIR)/$(MAIN) $(OBJS) $(LFLAGS) $(LIBS))

$(OUT_DIR)/build/%.o: %.cpp
	@mkdir -p '$(@D)'
	@$(call run_and_test, ($(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<  -o $@))

run:
	@export $(EXPORT_FLAG)="${PWD}/$(OUT_DIR)/bin"; \
		$(BIN_DIR)/Sandbox$(EXE_EXT)

run_mac:
	$(shell open ../macOS/StellarEngine.app)

clean:
	@$(RM) -r release debug bin
	@$(RM) ../macOS/StellarEngine.app/Contents/Frameworks/* ../macOS/StellarEngine.app/Contents/MacOS/*

depend: $(SRCS)
	makedepend $(INCLUDES) $^

-include $(OBJS:.o=.d)