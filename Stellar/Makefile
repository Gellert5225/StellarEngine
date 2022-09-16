LIB_NAME = Stellar

# Commen flags for all platforms
CXX      = g++
CPPFLAGS = -I./vendor/glfw/include -I./vendor/spdlog/include -I./vendor/imgui -I./src -I./vendor/glm
CXXFLAGS = -std=c++17 -g -O2 -DSTLR_BUILD_DLL -fPIC -Wall
LDFLAGS  = 
LDLIBS   =
RM       = rm -f
PCH_SRC  = ./src/stlrpch.h
PCH_OUT  = ./src/stlrpch.h.gch
PCH_FLAGS = -x c++-header -std=c++17 -g -O3 -DSTLR_BUILD_DLL -fPIC

# Target OS detection
ifeq ($(OS),Windows_NT) # OS is a preexisting environment variable on Windows
	OS = windows
	LIB_EXT = .dll
	GLFW_LIB = glfw3.dll
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		OS = macos
		LIB_EXT = .dylib
		GLFW_LIB = libglfw.3.dylib
	else 
		ifeq ($(UNAME),Linux)
			OS = linux
			LIB_EXT = .so
			GLFW_LIB = libglfw.so.3
		else
			$(error OS not supported by this Makefile)
		endif
	endif
endif
# OS-specific settings
ifeq ($(OS),windows)
	LDFLAGS += -L./vendor/glfw/libs -L./vendor/vulkan/win
	CPPFLAGS += -I ./vendor/vulkan/win/Include
	LDLIBS += -lglfw3 -lopengl32 -lgdi32 -lvulkan-1 -Wl,--out-implib,lib/libStellar.a

	ifeq ($(win32),1)
	$(error Stellar Engine does not support 32 bit os)
	endif

	else ifeq ($(OS),macos)
		CPPFLAGS += -I./vendor/metal-cpp
		LDFLAGS += -L./vendor/glfw/libs
		LDLIBS += -lglfw.3 -lvulkan -framework QuartzCore -framework Metal -framework Foundation -framework Cocoa -framework CoreGraphics -framework MetalKit
	else ifeq ($(OS),linux)
		CPPFLAGS += -I ./vendor/vulkan/linux/include
		LDFLAGS += -L./vendor/glfw/libs -L./vendor/vulkan/linux/lib
		LDLIBS += -lglfw -lvulkan -Wl,--out-implib,lib/libStellar.a
endif

# EVERYTHING PAST HERE SHOULD WORK AUTOMATICALLY

.SECONDARY:

include $(wildcard ../PrettyPrint.inc)

# Find the source files that will be used.
# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# How to recursively find all files that match a pattern
OBJC_FILES := $(call rwildcard,src/Stellar/Platform/Metal/,*.mm)
IMGUI_DIR = vendor/imgui
IMGUI_SRC_FILES := $(wildcard $(IMGUI_DIR)/*.cpp)
SRC_FILES := $(call rwildcard,src/,*.cpp) \
	$(IMGUI_SRC_FILES)

OBJS = $(filter %.o,$(patsubst %.cpp,build/%.o,$(SRC_FILES)) \
    $(patsubst %.mm,build/%.o,$(OBJC_FILES)))

all: lib/lib$(LIB_NAME)$(LIB_EXT)
	@mkdir -p ../Sandbox/bin
	@cp ./lib/libStellar$(LIB_EXT) ../Sandbox/bin/
	@cp ./vendor/glfw/libs/$(GLFW_LIB) ../Sandbox/bin/

# Update dependencies with each compilation
override CPPFLAGS += -MMD
-include $(shell find build -name "*.d" 2> /dev/null)

# Compiles PCH
$(PCH_OUT): $(PCH_SRC) 
	@$(call run_and_test,$(CXX) $(CPPFLAGS) $(PCH_FLAGS) -o $@ -c $<)

lib/lib$(LIB_NAME)$(LIB_EXT): $(OBJS)
	@mkdir -p $(@D)
	@$(call run_and_test, ($(CXX) -std=c++17 -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)))

build/%.o: %.cpp $(PCH_OUT)
	@mkdir -p $(@D)
	@$(call run_and_test, ($(CXX) -c $(CPPFLAGS)  $(CXXFLAGS) -include $(PCH_SRC) $< -o $@))

build/%.o: %.mm
	@mkdir -p $(@D)
	@$(call run_and_test, ($(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -ObjC++  $< -o $@))

clean:
	@echo "Cleaning"
	@$(RM) -r bin build lib **/*.gch **/*.d *.o *.a *.def *.s **/.o