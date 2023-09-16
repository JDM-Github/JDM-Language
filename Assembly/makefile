CXX = g++

# Directories
SRC_DIR = src
BIN_DIR = bin
DLL_DIR = dll

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

EXECUTABLE = $(BIN_DIR)/main.exe
DLL = $(DLL_DIR)/mydll.dll

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(DLL)
	$(CXX) main.cpp -o $(EXECUTABLE) -L$(DLL_DIR) -lmydll

$(DLL): $(SOURCES)
	$(CXX) -shared -o $(DLL) $(SOURCES) -I.

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(DLL_DIR):
	mkdir -p $(DLL_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DLL_DIR)

.PHONY: all clean