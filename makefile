CXX = g++
MAIN       = JDM
DLL_NAME   = JDM

SRC_DIR    = source
OBJ_DIR    = object

BIN_DIR    = Build
DLL_DIR    = Build

SRC_DIRS   = $(SRC_DIR) \
			 $(SRC_DIR)/register \
			 $(SRC_DIR)/library \
			 $(SRC_DIR)/utils \
			 $(SRC_DIR)/library/console \
			 $(SRC_DIR)/library/classes

SOURCES    = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
OBJECTS    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

INCLUDE    = -I"headers"

EXECUTABLE = $(BIN_DIR)/$(MAIN).exe
DLL        = $(DLL_DIR)/$(DLL_NAME).dll

all: clean $(EXECUTABLE) exec
comp: oclean clean $(EXECUTABLE) exec
main: main-comp exec

commit:
	@echo ---------------------------------------------------
	@echo Adding all files in git...
	@git add .
	@echo Setting commit message...
	@git commit -F message.txt
	@echo Pushing to Github...
	@git push origin private
	@git push origin private:main


main-comp:
	@echo ===================================================
	@echo - Compiling the Main executable...
	@$(CXX) main.cpp -o $(EXECUTABLE) -L$(DLL_DIR) -l$(DLL_NAME) $(INCLUDE)
	@echo - [SUCCESS] Main executable compiled: $(EXECUTABLE)

$(EXECUTABLE): $(DLL)
	@echo ===================================================
	@echo - Compiling the Main executable...
	@$(CXX) main.cpp -o $(EXECUTABLE) -L$(DLL_DIR) -l$(DLL_NAME) $(INCLUDE)
	@echo - [SUCCESS] Main executable compiled: $(EXECUTABLE)

$(DLL): $(OBJECTS)
	@echo ===================================================
	@echo - Creating DLL...
	@$(CXX) -shared -o $(DLL) $(OBJECTS) $(INCLUDE)
	@echo - [SUCCESS] DLL created: $(DLL)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo ---------------------------------------------------
	@echo - Compiling source file: $< ...
	@$(CXX) -c $< -o $@ $(INCLUDE)
	@echo - [SUCCESS] Compiled: $@

$(OBJ_DIR):
	@echo ---------------------------------------------------
	@echo - Creating Object Directories...
	@echo ---------------------------------------------------
	@echo - [INFO] Making Necessary Directories
	@mkdir $(OBJ_DIR)\library\console
	@mkdir $(OBJ_DIR)\library\classes
	@mkdir $(OBJ_DIR)\register
	@mkdir $(OBJ_DIR)\utils
	@echo - [SUCCESS] Object directories created

exec:
	@echo ===================================================
	@echo - Running the executable...
	@echo ===================================================
	@jdm

oclean:
	@echo ---------------------------------------------------
	@echo - Deleting Object Directory...
	@if exist $(OBJ_DIR)\* del /Q $(OBJ_DIR)\*
	@if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
	@echo - [SUCCESS] Object directory deleted

clean:
	@cls
	@echo ---------------------------------------------------
	@echo - Cleaning the Directory...
	@if exist $(BIN_DIR)\$(MAIN).exe del $(BIN_DIR)\$(MAIN).exe
	@if exist $(DLL_DIR)\$(DLL_NAME).dll del $(DLL_DIR)\$(DLL_NAME).dll
	
	@echo - [SUCCESS] Cleaning completed

.PHONY: all oclean clean test exec main comp commit
