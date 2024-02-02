PROJECT_NAME = project
OUTPUT_DIR = build

INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/imgui -Iinclude/src
LIB_DIRS = -Llib

LIBS = -lmingw32 -lSDL2main -lSDL2

SRC = $(wildcard src/*.cpp) $(wildcard imgui/*.cpp)
OBJ = $(SRC:%.cpp=$(OUTPUT_DIR)/%.o)

default: $(OUTPUT_DIR)/$(PROJECT_NAME)

$(OUTPUT_DIR)/$(PROJECT_NAME): $(OBJ)
	g++ $^ -o $@ $(LIB_DIRS) $(LIBS)

$(OUTPUT_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	g++ -c $< -o $@ $(INCLUDE_DIRS)

.PHONY: default
