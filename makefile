BIN:= Build
INCLUDE:= -I"headers"

all: compile exec

compile:
	@g++ main.cpp ${INCLUDE} -o ${BIN}/main.exe

exec:
	@${BIN}/main.exe

test:
	@g++ test.cpp -o ${BIN}/test.exe
	@${BIN}/test.exe
