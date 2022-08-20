void io_outb(unsigned short port, unsigned char val) {
	__asm__("out %%al, %%dx" : : "a"(val), "d"(port));
}

char io_inb(unsigned short port) {
	unsigned short val;
	__asm__("in %%dx, %%ax" : "=a"(val) : "d"(port));

	return val;
}