PROJECT_NAME = project 
OUTPUT_DIR = build

INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/imgui -Iinclude/src
LIB_DIRS = -Llib

LIBS = -lmingw32 -lSDL2main -lSDL2

SRC = $(wildcard src/*.cpp) $(wildcard imgui/*.cpp)

default:
	g++ $(SRC) -o $(OUTPUT_DIR)/$(PROJECT_NAME) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS)