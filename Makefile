SRC = src/memory_allocator.c src/main.c
OBJ = build/memory_allocator.o build/main.o
TARGET = build/memory_allocator

$(shell mkdir -p build)

all: $(TARGET)

build/%.o: src/%.c
	$(CC) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -rf build/

run:
	./build/memory_allocator