const char *DEBUG_OUT = (const char *)0x10000;

static void debug_write(const char* str)
{
	while(*str) {
		*((volatile char *)DEBUG_OUT) = *str++;
	}
}

int main() {
	debug_write("Hello, world!\n");
}

