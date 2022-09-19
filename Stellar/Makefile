LIB_NAME = 
DBG_OPT_FLAGS=-g
ASAN_FLAGS=-fsanitize=address

# Compiler and linker
CXX      = 
INCLUDES = -I./vendor/glfw/include -I./vendor/spdlog/include -I./vendor/imgui -I./src -I./vendor/glm
CXXFLAGS = -std=c++17 -DSTLR_BUILD_DLL -fPIC -Wall
LDFLAGS  = 
LDLIBS   =

# PCH
PCH_SRC  	= ./src/stlrpch.h
PCH_OUT  	= ./src/stlrpch.h.gch
PCH_FLAGS = -x c++-header -std=c++17 -DSTLR_BUILD_DLL -fPIC

# Other
RM       	= rm -f
OUT_DIR   = release

ifeq "$(findstring debug, $(MAKECMDGOALS))" ""
	LIB_NAME = Stellar
	OUT_DIR = release
 	CXXFLAGS += -O3 -DNDEBUG
else
	LIB_NAME = Stellar_debug
 	OUT_DIR = debug
 	CXXFLAGS += -g -fsanitize=address
endif

# Target OS detection
ifeq ($(OS),Windows_NT) # OS is a preexisting environment variable on Windows
	OS = windows
	LIB_EXT = .dll
	GLFW_LIB = glfw3.dll

	CXX = g++
	LDFLAGS += -L./vendor/glfw/libs -L./vendor/vulkan/win
	INCLUDES += -I ./vendor/vulkan/win/Include
	LDLIBS += -lglfw3 -lopengl32 -lgdi32 -lvulkan-1 -Wl,--out-implib,lib/libStellar.a
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		OS = macos
		LIB_EXT = .dylib
		GLFW_LIB = libglfw.3.dylib

		CXX = clang++
		INCLUDES += -I./vendor/metal-cpp -I/usr/local/include
		LDFLAGS += -L./vendor/glfw/libs -L./vendor/vulkan
		LDLIBS += -lglfw.3 -lvulkan -framework QuartzCore -framework Metal -framework Foundation -framework Cocoa -framework CoreGraphics -framework MetalKit
	else 
		ifeq ($(UNAME),Linux)
			OS = linux
			LIB_EXT = .so
			GLFW_LIB = libglfw.so.3

			CXX = g++
			INCLUDES += -I ./vendor/vulkan/linux/include
			LDFLAGS += -L./vendor/glfw/libs -L./vendor/vulkan/linux/lib
			LDLIBS += -lglfw -lvulkan -Wl,--out-implib,lib/libStellar.a
		else
			$(error OS not supported by this Makefile)
		endif
	endif
endif

.SECONDARY:

include $(wildcard ../PrettyPrint.inc)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

OBJC_FILES := $(call rwildcard,src/Stellar/Platform/Metal/,*.mm)
IMGUI_DIR = vendor/imgui
IMGUI_SRC_FILES := $(wildcard $(IMGUI_DIR)/*.cpp)
SRC_FILES := $(call rwildcard,src/,*.cpp) \
	$(IMGUI_SRC_FILES)

OBJS = 

ifeq ($(OS),macos)
	CXX = clang++
	SRC_FILES := $(filter-out $(call rwildcard,src/Stellar/Platform/Vulkan/,*.cpp), $(SRC_FILES))
	OBJS = $(filter %.o,$(patsubst %.cpp,$(OUT_DIR)/build/%.o,$(SRC_FILES)) \
    $(patsubst %.mm,$(OUT_DIR)/build/%.o,$(OBJC_FILES)))
else
	CXX = g++
	SRC_FILES := $(filter-out $(call rwildcard,src/Stellar/Platform/Metal/,*.cpp), $(SRC_FILES))
	SRC_FILES := $(filter-out $(call rwildcard,src/Stellar/Platform/Metal/,*.mm), $(SRC_FILES))
	OBJS = $(filter %.o,$(patsubst %.cpp,$(OUT_DIR)/build/%.o,$(SRC_FILES)))
endif

debug: all
release: all

all: $(OUT_DIR)/lib/lib$(LIB_NAME)$(LIB_EXT)
	@mkdir -p ../Sandbox/bin
	@cp $(OUT_DIR)/lib/lib$(LIB_NAME)$(LIB_EXT) ../Sandbox/bin/
	@cp ./vendor/glfw/libs/$(GLFW_LIB) ../Sandbox/bin/

# Update dependencies with each compilation
-include $(shell find build -name "*.d" 2> /dev/null)

# Compiles PCH
$(PCH_OUT): $(PCH_SRC) 
	@$(call run_and_test,$(CXX) $(INCLUDES) $(CXXFLAGS) $(PCH_FLAGS) -o $@ -c $<)

$(OUT_DIR)/lib/lib$(LIB_NAME)$(LIB_EXT): $(OBJS)
	@mkdir -p $(@D)
	@$(call run_and_test, ($(CXX) $(CXXFLAGS) -std=c++17 -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)))

$(OUT_DIR)/build/%.o: %.cpp $(PCH_OUT)
	@mkdir -p $(@D)
	@$(call run_and_test, ($(CXX) -c $(INCLUDES)  $(CXXFLAGS) -include $(PCH_SRC) $< -o $@))

$(OUT_DIR)/build/%.o: %.mm
	@mkdir -p $(@D)
	@$(call run_and_test, ($(CXX) -c $(INCLUDES) $(CXXFLAGS) -ObjC++ -fobjc-arc $< -o $@))

clean:
	@echo "Cleaning"
	@$(RM) -r debug/ release/ **/*.gch **/*.d *.o *.a *.def *.s **/.o