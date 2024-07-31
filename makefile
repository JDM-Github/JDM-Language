CXX = g++
MAIN         = JDM
DLL_NAME     = JDM
DLL_LEXER    = JDMLexer
DLL_PARSER   = JDMParser
DLL_COMP     = JDMCompiler

DLL_CONSOLE  = JDMConsole
DLL_LIB      = JDMLibrary

ALL_DLL = -l$(DLL_NAME) -l$(DLL_LEXER) -l$(DLL_PARSER) -l$(DLL_COMP)

SRC_DIR    = source
OBJ_DIR    = object

BIN_DIR    = Build
DLL_DIR    = Build

SRC_DIRS   = $(SRC_DIR)/register \
			 $(SRC_DIR)/utils 
SOURCES    = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
OBJECTS    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# REGISTER_SRC    = $(SRC_DIR)/register
# REGISTER_SOURCE =  $(foreach dir, $(REGISTER_SRC), $(wildcard $(dir)/*.cpp))
# REGISTER_OBJECT = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(REGISTER_SOURCE))

LEXER_SRC    = $(SRC_DIR)/lexer
LEXER_SOURCE =  $(foreach dir, $(LEXER_SRC), $(wildcard $(dir)/*.cpp))
LEXER_OBJECT = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(LEXER_SOURCE))

PARSER_SRC    = $(SRC_DIR)/parser
PARSER_SOURCE =  $(foreach dir, $(PARSER_SRC), $(wildcard $(dir)/*.cpp))
PARSER_OBJECT = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(PARSER_SOURCE))

COMPILER_SRC    = $(SRC_DIR)/compiler
COMPILER_SOURCE =  $(foreach dir, $(COMPILER_SRC), $(wildcard $(dir)/*.cpp))
COMPILER_OBJECT = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMPILER_SOURCE))

CONSOLE_SRC    = $(SRC_DIR)/library/console
CONSOLE_SOURCE =  $(foreach dir, $(CONSOLE_SRC), $(wildcard $(dir)/*.cpp))
CONSOLE_OBJECT = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CONSOLE_SOURCE))

LIBRARY_SRC    = $(SRC_DIR)/library \
			     $(SRC_DIR)/library/classes
LIBRARY_SOURCE =  $(foreach dir, $(LIBRARY_SRC), $(wildcard $(dir)/*.cpp))
LIBRARY_OBJECT = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(LIBRARY_SOURCE))

INCLUDE      = -I"headers"
EXECUTABLE   = $(BIN_DIR)/$(MAIN).exe
DLL          = $(DLL_DIR)/$(DLL_NAME).dll
LEXER_DLL    = $(DLL_DIR)/$(DLL_LEXER).dll
PARSER_DLL   = $(DLL_DIR)/$(DLL_PARSER).dll
COMPILER_DLL = $(DLL_DIR)/$(DLL_COMP).dll

CONSOLE_DLL  = $(DLL_DIR)/$(DLL_CONSOLE).dll
LIBRARY_DLL  = $(DLL_DIR)/$(DLL_LIB).dll

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
	@$(CXX) main.cpp -o $(EXECUTABLE) -L$(DLL_DIR) $(ALL_DLL) $(INCLUDE)
	@echo - [SUCCESS] Main executable compiled: $(EXECUTABLE)

$(EXECUTABLE): $(DLL) $(CONSOLE_DLL) $(LIBRARY_DLL) $(LEXER_DLL) $(PARSER_DLL) $(COMPILER_DLL)
	@echo ===================================================
	@echo - Compiling the Main executable...
	@if not exist $(BIN_DIR)\$(MAIN).exe $(CXX) main.cpp -o $(EXECUTABLE) -L$(DLL_DIR) $(ALL_DLL) $(INCLUDE)
	@echo - [SUCCESS] Main executable compiled: $(EXECUTABLE)

$(DLL): $(OBJECTS)
	@echo ===================================================
	@echo - Creating DLL...
	@$(CXX) -shared -o $(DLL) $(OBJECTS) $(INCLUDE)
	@echo - [SUCCESS] DLL created: $(DLL)

$(CONSOLE_DLL): $(CONSOLE_OBJECT)
	@echo ===================================================
	@echo - Creating CONSOLE DLL...
	@$(CXX) -shared -o $(CONSOLE_DLL) $(CONSOLE_OBJECT) $(INCLUDE) -L$(DLL_DIR) -l$(DLL_NAME)
	@echo - [SUCCESS] CONSOLE DLL created: $(CONSOLE_DLL)

$(LIBRARY_DLL): $(LIBRARY_OBJECT)
	@echo ===================================================
	@echo - Creating LIBRARY DLL...
	@$(CXX) -shared -o $(LIBRARY_DLL) $(LIBRARY_OBJECT) $(INCLUDE) -L$(DLL_DIR) -l$(DLL_NAME)
	@echo - [SUCCESS] LIBRARY DLL created: $(LIBRARY_DLL)

$(LEXER_DLL): $(LEXER_OBJECT)
	@echo ===================================================
	@echo - Creating LEXER DLL...
	@$(CXX) -shared -o $(LEXER_DLL) $(LEXER_OBJECT) $(INCLUDE) -L$(DLL_DIR) -l$(DLL_NAME) 
	@echo - [SUCCESS] LEXER DLL created: $(LEXER_DLL)

$(PARSER_DLL): $(PARSER_OBJECT)
	@echo ===================================================
	@echo - Creating PARSER DLL...
	@$(CXX) -shared -o $(PARSER_DLL) $(PARSER_OBJECT) $(INCLUDE) -L$(DLL_DIR) -l$(DLL_NAME)
	@echo - [SUCCESS] PARSER DLL created: $(PARSER_DLL)

$(COMPILER_DLL): $(COMPILER_OBJECT)
	@echo ===================================================
	@echo - Creating COMPILER DLL...
	@$(CXX) -shared -o $(COMPILER_DLL) $(COMPILER_OBJECT) $(INCLUDE) -L$(DLL_DIR) -l$(DLL_NAME) -l$(DLL_LIB) -l$(DLL_CONSOLE)
	@echo - [SUCCESS] COMPILER DLL created: $(COMPILER_DLL)


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
	@mkdir $(OBJ_DIR)\compiler
	@mkdir $(OBJ_DIR)\lexer
	@mkdir $(OBJ_DIR)\parser
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
# 	@if exist $(BIN_DIR)\$(MAIN).exe del $(BIN_DIR)\$(MAIN).exe
# 	@if exist $(DLL_DIR)\$(DLL_LIB).dll del $(DLL_DIR)\$(DLL_LIB).dll
# 	@if exist $(DLL_DIR)\$(DLL_LIB).dll del $(DLL_DIR)\$(DLL_LIB).dll
# 	@if exist $(DLL_DIR)\$(DLL_LEXER).dll del $(DLL_DIR)\$(DLL_LEXER).dll
# 	@if exist $(DLL_DIR)\$(DLL_PARSER).dll del $(DLL_DIR)\$(DLL_PARSER).dll
# 	@if exist $(DLL_DIR)\$(DLL_COMP).dll del $(DLL_DIR)\$(DLL_COMP).dll
	
	@echo - [SUCCESS] Cleaning completed

.PHONY: all oclean clean test exec main comp commit
