#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "include/videoDriver.h"
#include "include/keyboardDriver.h"
#include "include/idtLoader.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

void test_keyboard() {
    char c;
    video_clearScreen();
    video_putString("Test de teclado - Presione ESC para salir\n", 0xFFFFFF, 0x000000);

    while(1) {
        c = keyboard_read_getchar();
        if(c != 0) {
            if(c == 27) {  // ESC
                video_putString("\nSaliendo...\n", 0xFFFFFF, 0x000000);
                break;
            }
            char str[2] = {c, 0};
            video_putString(str, 0xFFFFFF, 0x000000);
        }
    }
}

int main()
{	
	load_idt();

	//test_keyboard();
	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncPrint("  Calling the sample code module returned: ");
	ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	ncNewline();
	ncNewline();

	ncPrint("  Sample data module at 0x");
	ncPrintHex((uint64_t)sampleDataModuleAddress);
	ncNewline();
	ncPrint("  Sample data module contents: ");
	ncPrint((char*)sampleDataModuleAddress);
	ncNewline();

	// video_putChar('s', 0xFFFFFF, 0x000000);
	// video_putChar('e', 0xFFFFFF, 0x000000);
	// video_putChar(' ', 0xFFFFFF, 0x000000);
	// video_putString("Hola mundo", 0xFFFFFF, 0x000000);
	// video_putChar(' ', 0xFFFFFF, 0x000000);
	// video_putChar(' ', 0xFFFFFF, 0x000000);
	// video_putChar(' ', 0xFFFFFF, 0x000000);
	// video_putChar(' ', 0xFFFFFF, 0x000000);
	// video_putChar(' ', 0xFFFFFF, 0x000000);
	// video_putChar(' ', 0xFFFFFF, 0x000000);
	// video_putString("Prueba de video ", 0xFFFFFF, 0x000000);
	// video_putString("tab:\t y ahora un enter\n y ahora un backspace\b", 0xFFFFFF, 0x000000);

	// video_putString("\nHola\tcomo\testas\n", 0xFFFFFF, 0x000000);
	// video_putString("\blinea de abajo", 0xFFFFFF, 0x000000);
	
	ncPrint("[Finished]");
	return 0;
}
