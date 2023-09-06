BIN:= Build
INCLUDE:= -I"headers"


all: compile exec

compile:
	@g++ main.cpp ${INCLUDE} -o ${BIN}/main.exe

exec:
	@${BIN}/main.exe
