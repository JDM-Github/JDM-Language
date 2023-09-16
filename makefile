CXX = g++
MAIN       = main
DLL_NAME   = JDM

SRC_DIR    = source
BIN_DIR    = Build
DLL_DIR    = Build
OBJ_DIR    = object

SOURCES    = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
INCLUDE    = -I"headers"

EXECUTABLE = $(BIN_DIR)/$(MAIN).exe
DLL        = $(DLL_DIR)/$(DLL_NAME).dll

all: clean $(EXECUTABLE) exec

$(EXECUTABLE): $(DLL)
	$(CXX) $(MAIN).cpp -o $(EXECUTABLE) -L$(DLL_DIR) -l$(DLL_NAME)

$(DLL): $(OBJECTS)
	$(CXX) -shared -o $(DLL) $(OBJECTS) $(INCLUDE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(INCLUDE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

exec:
	@$(EXECUTABLE)

oclean:
	rm -rf $(OBJ_DIR)

clean:	
	del $(BIN_DIR)\$(MAIN).exe
	del $(DLL_DIR)\$(DLL_NAME).dll

test:
	@g++ test.cpp -o ${BIN_DIR}/test.exe
	@${BIN_DIR}/test.exe

.PHONY: all clean oclean test exec
