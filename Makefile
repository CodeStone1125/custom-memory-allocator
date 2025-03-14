SRC = src/memory_allocator.c
OBJ = build/memory_allocator.o
TARGET = build/memory_allocator

$(shell mkdir -p build)

CFLAGS = -g

all: $(TARGET)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

clean:
	rm -rf build/

run:
	make
	./build/memory_allocator
