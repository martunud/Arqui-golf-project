#include "keyboardDriver.h"
#include "interrupts.h" 

#define SC_UP     0x48
#define SC_DOWN   0x50
#define SC_LEFT   0x4B
#define SC_RIGHT  0x4D
#define SC_SPACE  0x39
#define SC_TAB    0x0F

extern uint8_t getScanCode();
extern void _refreshRegisters(void);
extern uint64_t* _getRegisters();

void refreshRegisters(void);
static int buffer_empty();
static int buffer_full();
static char buffer_pop();
static char buffer_push(char c);
static char scToAscii(uint8_t scancode);
static void updateFlags(uint8_t scancode);


// En primer indice char sin shift, en segundo indice char con shift
static const char scancode_table[KEY_COUNT][2] = {
    {0, 0}, {ESC, ESC}, {'1', '!'}, {'2', '@'}, {'3', '#'},
    {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'},
    {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'},
    {'\t', '\t'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'},
    {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'},
    {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, {0, 0},
    {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'},
    {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'},
    {'\'', '\"'}, {'`', '~'}, {0, 0}, {'\\', '|'}, {'z', 'Z'},
    {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'},
    {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, {0, 0}, {0, 0},
    {0, 0}, {' ', ' '},
    // Keypad (scancodes 0x47 - 0x53)
    [0x47] = {'7', '7'}, // Keypad 7
    [0x48] = {'8', '8'}, // Keypad 8
    [0x49] = {'9', '9'}, // Keypad 9
    [0x4B] = {'4', '4'}, // Keypad 4
    [0x4C] = {'5', '5'}, // Keypad 5
    [0x4D] = {'6', '6'}, // Keypad 6
    [0x4F] = {'1', '1'}, // Keypad 1
    [0x50] = {'2', '2'}, // Keypad 2
    [0x51] = {'3', '3'}, // Keypad 3
    [0x52] = {'0', '0'}, // Keypad 0
    [0x53] = {'.', '.'},  // Keypad .
    [0x4A] = {'-', '-'}, // Keypad -
    [0x4E] = {'+', '+'},  // Keypad +
};


typedef struct CircleBuffer{
    char buffer[BUFFER_SIZE];
    int readIndex;              //indice del proximo caracter a leer
    int writeIndex;             //indice del proximo caracter a escribir  
    int size;
} TCircleBuffer;


static TCircleBuffer buffer = {.readIndex = 0, .writeIndex = 0, .size = 0};

//Flags teclas especiales
static volatile uint8_t activeShift = 0;               //Shift presionado
static volatile uint8_t activeCapsLock = 0;            //CapsLock presionado
static volatile uint8_t activeCtrl = 0;                //Ctrl presionado

static volatile uint64_t registers[REGISTERS_CANT];   //Arreglo de registros 

static volatile uint8_t regsLoaded = 0;           //Flag para saber si se cargaron los registros


void keyboard_interrupt_handler() {
    uint8_t scancode = getScanCode();           
    updateFlags(scancode);                     

    char cAscii = scToAscii(scancode);        

    if(activeCtrl && (cAscii == 'R' || cAscii == 'r')) {
        regsLoaded = 1;
        refreshRegisters();
    } else if (cAscii != 0) {
        buffer_push(cAscii);
    }
}



static void updateFlags(uint8_t scancode) {
    if (scancode == CTRL_L) {
        activeCtrl = 1;
    }
    else if (scancode == CTRL_L_RELEASE) {
        activeCtrl = 0;
    }
    else if (scancode == SHIFT_L || scancode == SHIFT_R) {
        activeShift = 1;
    }
    else if (scancode == (SHIFT_L + RELEASE_OFFSET) || scancode == (SHIFT_R + RELEASE_OFFSET)) {
        activeShift = 0;
    }
    else if (scancode == CAPSLOCK) {
        activeCapsLock = !activeCapsLock;
    }
}


static char buffer_push(char c) {
    if (buffer_full()) return 0;

    buffer.buffer[buffer.writeIndex] = c;
    buffer.writeIndex = (buffer.writeIndex + 1) % BUFFER_SIZE;
    buffer.size++;
    return 1;
}

static char buffer_pop() {
    if (buffer_empty()) return 0;

    char c = buffer.buffer[buffer.readIndex];
    buffer.readIndex = (buffer.readIndex + 1) % BUFFER_SIZE;
    buffer.size--;
    return c;
}

static int buffer_full() {
    return buffer.size == BUFFER_SIZE;
}

static int buffer_empty() {
    return buffer.readIndex == buffer.writeIndex;
}

char keyboard_read_getchar() {
    return buffer_pop();
}


static char scToAscii(uint8_t scancode) {
    char c = 0;

    if (scancode < KEY_COUNT) {
        c = scancode_table[scancode][activeShift];
        if (activeCapsLock && c >= 'a' && c <= 'z') {
            c -= 32;
        }
    } else {
        switch (scancode) {
            case SC_UP:
                c = KEY_ARROW_UP;
                break;
            case SC_DOWN:
                c = KEY_ARROW_DOWN;
                break;
            case SC_LEFT:
                c = KEY_ARROW_LEFT;
                break;
            case SC_RIGHT:
                c = KEY_ARROW_RIGHT;
                break;
            default:
                c = 0;
                break;
        }
    }

    return c;
}


void refreshRegisters() {
    _refreshRegisters();
    uint64_t * r = _getRegisters();
    for(int i = 0; i < REGISTERS_CANT; i++) {
        registers[i] = r[i];
    }
}

uint64_t getRegisters(uint64_t * r) {
    if(!regsLoaded) {
        return 0;
    }
    for(int i = 0; i < REGISTERS_CANT; i++) {
        r[i] = registers[i];
    }
    return 1;
}
