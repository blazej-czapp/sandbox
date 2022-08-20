C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard *.h kernel/*.h drivers/*.h)
OBJ_FILES = ${C_SOURCES:.c=.o}

all: os-image

run: os-image
	bochs -q

os-image: boot/boot.bin kernel.bin
	cat $^ > $@

kernel.bin: kernel/kernel_entry.o ${OBJ_FILES}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat=binary

# all object files are built from .c files with the corresponding name
%.o: %.c ${HEADERS}
	gcc -m32 -ffreestanding -c $< -o $@

kernel/kernel_entry.o: kernel/kernel_entry.asm
	nasm $< -f elf -o $@

# whenever a .bin file is needed, it is built from the corresponding .asm file
%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.o os-image
	rm -rf kernel/*.o kernel/*.bin boot/*.bin drivers/*.o