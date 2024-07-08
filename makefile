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
COMPILER   = $(wildcard $(SRC_DIR)/compiler/*.cpp)
CONSOLE    = $(wildcard $(SRC_DIR)/library/console/*.cpp)
OBJECTS    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES)) \
			 $(patsubst $(SRC_DIR)/library/%.cpp, $(OBJ_DIR)/library/%.o, $(LIBRARY)) \
			 $(patsubst $(SRC_DIR)/utils/%.cpp, $(OBJ_DIR)/utils/%.o, $(UTILITY)) \
			 $(patsubst $(SRC_DIR)/compiler/%.cpp, $(OBJ_DIR)/compiler/%.o, $(COMPILER)) \
			 $(patsubst $(SRC_DIR)/library/console/%.cpp, $(OBJ_DIR)/library/console/%.o, $(CONSOLE))

INCLUDE    = -I"headers"

EXECUTABLE = $(BIN_DIR)/$(MAIN).exe
DLL        = $(DLL_DIR)/$(DLL_NAME).dll

all: clean $(EXECUTABLE) exec
comp: oclean clean $(EXECUTABLE) exec

$(EXECUTABLE): $(DLL)
	@echo ===================================================
	@echo Compiling the Main executable...
	@$(CXX) main.cpp -o $(EXECUTABLE) -L$(DLL_DIR) -l$(DLL_NAME) $(INCLUDE)
	@echo [SUCCESS] Main executable compiled: $(EXECUTABLE)

$(DLL): $(OBJECTS)
	@echo ===================================================
	@echo Creating DLL...
	@$(CXX) -shared -o $(DLL) $(OBJECTS) $(INCLUDE)
	@echo [SUCCESS] DLL created: $(DLL)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo ---------------------------------------------------
	@echo Compiling source file: $< ...
	@$(CXX) -c $< -o $@ $(INCLUDE)
	@echo [SUCCESS] Compiled: $< to $@

$(OBJ_DIR):
	@echo ---------------------------------------------------
	@echo Creating Object Directories...
	@echo ---------------------------------------------------
	@echo [INFO] Making Directory: $(OBJ_DIR)\library\console...
	@mkdir $(OBJ_DIR)\library\console
	@echo [INFO] Making Directory: $(OBJ_DIR)\compiler...
	@mkdir $(OBJ_DIR)\compiler
	@echo [INFO] Making Directory: $(OBJ_DIR)\utils...
	@mkdir $(OBJ_DIR)\utils
	@echo [SUCCESS] Object directories created

exec:
	@echo ===================================================
	@echo Running the executable...
	@echo ===================================================
	@jdm

oclean:
	@echo ---------------------------------------------------
	@echo Deleting Object Directory...
	@del /Q $(OBJ_DIR)\*
	@rmdir /S /Q $(OBJ_DIR)
	@echo [SUCCESS] Object directory deleted

clean:
	@cls
	@echo ---------------------------------------------------
	@echo Cleaning the Directory...
	@if exist $(BIN_DIR)\$(MAIN).exe del $(BIN_DIR)\$(MAIN).exe
	@if exist $(DLL_DIR)\$(DLL_NAME).dll del $(DLL_DIR)\$(DLL_NAME).dll
	
	@echo [SUCCESS] Cleaning completed


.PHONY: all oclean clean test exec
