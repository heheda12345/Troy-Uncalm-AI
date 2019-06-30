all: main.exe

main.exe: makefile main.o Entry.o Entry.h Const.h Logic.o Logic.h PlayerAI.cpp Record.o Record.h jsoncpp/json/json-forwards.h jsoncpp/json/json.h jsoncpp/jsoncpp.cpp
ifeq ($(OS),Windows_NT)
	g++64 main.o Entry.o Logic.o PlayerAI.cpp Record.o jsoncpp/jsoncpp.cpp -o main.exe -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -static-libstdc++ -lwsock32
else
	g++64 main.o Entry.o Logic.o PlayerAI.cpp Record.o jsoncpp/jsoncpp.cpp -o main.exe -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -static-libstdc++
endif

.PHONY: clean
clean:
	-rm -r *.exe
	-del -r *.exe
