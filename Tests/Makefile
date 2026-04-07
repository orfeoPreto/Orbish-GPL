# Orbish Regression Tests — Makefile
# Links against the pre-built libOrbish.a from the Xcode Debug build.
# Usage:
#   make          — build and run tests
#   make build    — build only
#   make run      — run only
#   make clean    — remove build artifacts
#   make asan     — build with AddressSanitizer

CXX := clang++
CXXFLAGS := -std=c++17 -DDEBUG=1 -D_DEBUG=1 \
    -DJUCER_XCODE_MAC_F6D2F4CF=1 \
    -DJUCE_APP_VERSION=\"0.5.0\" \
    -DJUCE_APP_VERSION_HEX=0x500 \
    -DJucePlugin_Build_VST=0 \
    -DJucePlugin_Build_VST3=0 \
    -DJucePlugin_Build_AU=0 \
    -DJucePlugin_Build_AUv3=0 \
    -DJucePlugin_Build_AAX=0 \
    -DJucePlugin_Build_Standalone=1 \
    -DJucePlugin_Build_Unity=0 \
    -DJucePlugin_Build_LV2=0

ORBISH_ROOT := $(shell cd .. && pwd)
BUILD_DIR   := $(ORBISH_ROOT)/Builds/MacOSX/build/Debug

# Include paths (match Xcode project settings)
INCLUDES := \
    -I$(ORBISH_ROOT)/JuceLibraryCode \
    -I$(ORBISH_ROOT) \
    -I$(ORBISH_ROOT)/Source \
    -I/Users/orfeopreto/Dev/JUCE/modules \
    -I/Users/orfeopreto/Dev/JUCE/modules/juce_audio_processors/format_types/VST3_SDK \
    -I$(ORBISH_ROOT)/ThirdParty/catch2

# Static library from Xcode build
LIB_ORBISH := $(BUILD_DIR)/libOrbish.a

# macOS frameworks required by JUCE
FRAMEWORKS := \
    -framework Accelerate \
    -framework AudioToolbox \
    -framework Cocoa \
    -framework CoreAudio \
    -framework CoreAudioKit \
    -framework CoreMIDI \
    -framework DiscRecording \
    -framework Foundation \
    -framework IOKit \
    -framework OpenGL \
    -framework QuartzCore \
    -framework Security \
    -framework WebKit \
    -framework Metal \
    -framework MetalKit

LDFLAGS := $(FRAMEWORKS) -lc++

# Test sources
TEST_SRCS := \
    OrbishTests.cpp \
    LeakTests.cpp \
    InitializationTests.cpp \
    AudioEngineTests.cpp \
    ThreadSafetyTests.cpp

# Catch2 amalgamated source
CATCH_SRC := $(ORBISH_ROOT)/ThirdParty/catch2/catch_amalgamated.cpp

# Output
OUT_DIR  := _build
TEST_BIN := $(OUT_DIR)/OrbishTests

# Object files
TEST_OBJS := $(TEST_SRCS:%.cpp=$(OUT_DIR)/%.o)
CATCH_OBJ := $(OUT_DIR)/catch_amalgamated.o

.PHONY: all build run clean asan

all: build run

build: $(TEST_BIN)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Check that libOrbish.a exists
$(LIB_ORBISH):
	$(error libOrbish.a not found at $(LIB_ORBISH). Build the project in Xcode first (Debug configuration).)

# Compile Catch2 amalgamated (slow, only once)
# CATCH_AMALGAMATED_CUSTOM_MAIN lets us define our own main() in OrbishTests.cpp
$(CATCH_OBJ): $(CATCH_SRC) | $(OUT_DIR)
	@echo "Compiling Catch2..."
	$(CXX) $(CXXFLAGS) -DCATCH_AMALGAMATED_CUSTOM_MAIN -I$(ORBISH_ROOT)/ThirdParty/catch2 -c $< -o $@

# Compile test files
$(OUT_DIR)/%.o: %.cpp TestFixtures.h | $(OUT_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Link
$(TEST_BIN): $(TEST_OBJS) $(CATCH_OBJ) $(LIB_ORBISH) | $(OUT_DIR)
	@echo "Linking $(TEST_BIN)..."
	$(CXX) $(TEST_OBJS) $(CATCH_OBJ) $(LIB_ORBISH) $(LDFLAGS) -o $(TEST_BIN)
	@echo "Build succeeded."

run: $(TEST_BIN)
	@echo ""
	@echo "=== Running Orbish Regression Tests ==="
	@echo ""
	./$(TEST_BIN) --reporter compact

clean:
	rm -rf _build

# AddressSanitizer build
asan: CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
asan: LDFLAGS += -fsanitize=address
asan: clean build
	@echo ""
	@echo "=== Running with AddressSanitizer ==="
	@echo ""
	./$(TEST_BIN) --reporter compact
