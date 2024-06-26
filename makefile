CXX = g++
MAIN       = JDM
DLL_NAME   = JDM

SRC_DIR    = source
OBJ_DIR    = object

BIN_DIR    = Build
DLL_DIR    = Build

OTHER_SRC  = library/console
SOURCES    = $(wildcard $(SRC_DIR)/*.cpp)
LIBRARY    = $(wildcard $(SRC_DIR)/library/*.cpp)
UTILITY    = $(wildcard $(SRC_DIR)/utils/*.cpp)
CONSOLE    = $(wildcard $(SRC_DIR)/library/console/*.cpp)
OBJECTS    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES)) \
			 $(patsubst $(SRC_DIR)/library/%.cpp, $(OBJ_DIR)/library/%.o, $(LIBRARY)) \
			 $(patsubst $(SRC_DIR)/utils/%.cpp, $(OBJ_DIR)/utils/%.o, $(UTILITY)) \
			 $(patsubst $(SRC_DIR)/library/console/%.cpp, $(OBJ_DIR)/library/console/%.o, $(CONSOLE))

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
	mkdir $(OBJ_DIR)\library\console
	mkdir $(OBJ_DIR)\utils

exec:
	@$(EXECUTABLE)

oclean:
	del $(OBJ_DIR)\* y

clean:
	del $(BIN_DIR)\$(MAIN).exe
	del $(DLL_DIR)\$(DLL_NAME).dll

test:
	@echo $(OBJECTS)

.PHONY: all oclean clean test exec
