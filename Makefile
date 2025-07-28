# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror=vla -MMD -I./src -I./assets -I/opt/X11/include
LDFLAGS = -L/opt/X11/lib -lX11

# Executable name and folders
EXEC = chess
BIN = bin
SRC = src
BUILD = build

# Source and object files
SOURCES = $(wildcard $(SRC)/*.cc) main.cc
OBJECTS = $(patsubst %.cc,$(BUILD)/%.o,$(notdir $(SOURCES)))
DEPENDS = $(OBJECTS:.o=.d)

# Default target
all: $(BIN)/$(EXEC)

# Linking final binary
$(BIN)/$(EXEC): $(OBJECTS)
	mkdir -p $(BIN)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compiling .cc to .o
$(BUILD)/%.o: $(SRC)/%.cc
	mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/main.o: main.cc
	mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD) $(BIN)

# Run the program
run: $(BIN)/$(EXEC)
	./$(BIN)/$(EXEC)

# Include dependency files
-include $(DEPENDS)
