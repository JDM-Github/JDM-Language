CXX = g++
MAIN       = JDM
DLL_NAME   = JDM

SRC_DIR    = source
OBJ_DIR    = object

BIN_DIR    = Build
DLL_DIR    = Build

OTHER_SRC  = tokenizerSource parserSource
SOURCES    = $(wildcard $(SRC_DIR)/*.cpp) $(foreach D, $(OTHER_SRC), $(wildcard $(SRC_DIR)\$(D)/*.cpp))
OBJECTS    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
INCLUDE    = -I"headers"

EXECUTABLE = $(BIN_DIR)/$(MAIN).exe
DLL        = $(DLL_DIR)/$(DLL_NAME).dll

all: clean $(EXECUTABLE) exec

$(EXECUTABLE): $(DLL)
	$(CXX) main.cpp -o $(EXECUTABLE) -L$(DLL_DIR) -l$(DLL_NAME) $(INCLUDE)

$(DLL): $(OBJECTS)
	$(CXX) -shared -o $(DLL) $(OBJECTS) $(INCLUDE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(INCLUDE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

exec:
	@$(EXECUTABLE)

oclean:
	del $(OBJ_DIR)\*

clean:
	del $(BIN_DIR)\$(MAIN).exe
	del $(DLL_DIR)\$(DLL_NAME).dll

test:
	echo $(OBJECTS)

.PHONY: all oclean clean test exec
