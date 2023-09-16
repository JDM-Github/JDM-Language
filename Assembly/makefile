CXX = g++

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DLL_DIR = dll

# Source files and object files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Output files
EXECUTABLE = $(BIN_DIR)/main.exe
DLL = $(DLL_DIR)/mydll.dll

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(DLL) $(OBJECTS)
	$(CXX) main.cpp $(OBJECTS) -o $(EXECUTABLE) -L$(DLL_DIR) -lmydll

$(DLL): $(SOURCES)
	$(CXX) -shared -o $(DLL) $(SOURCES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(DLL_DIR):
	mkdir -p $(DLL_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DLL_DIR)

.PHONY: all clean