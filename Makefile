.PHONY: all linux windows tools

all: linux windows tools

linux:
	g++ -O3 ./src/dt4dds-challenge.cpp -o ./bin/dt4dds-challenges -std=c++20 -g ./src/include/*.cpp -static

windows:
	x86_64-w64-mingw32-g++ -O3 ./src/dt4dds-challenge.cpp -o ./bin/dt4dds-challenges.exe -std=c++20 -g ./src/include/*.cpp -static

tools:
	./tools/bbmap/install.sh
	./tools/ngmerge/install.sh
