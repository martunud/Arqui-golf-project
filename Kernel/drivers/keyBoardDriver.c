#include "keyboardDriver.h"
#include "keystate.h"

static int buffer_empty();
static int buffer_full();
static char buffer_pop();
static char buffer_push(char c);
static char scToAscii(uint8_t scancode);
static void updateFlags(uint8_t scancode);

//Flags teclas especiales
static volatile uint8_t activeShift = 0;               //Shift presionado
static volatile uint8_t activeCapsLock = 0;            //CapsLock presionado
static volatile uint8_t activeCtrl = 0;                //Ctrl presionado

// Arreglo para mantener el estado de las teclas
uint8_t key_states[256] = {0};

typedef struct CircleBuffer{
    char buffer[BUFFER_SIZE];
    int readIndex;              //indice del proximo caracter a leer
    int writeIndex;             //indice del proximo caracter a escribir  
    int size;
} TCircleBuffer;


static TCircleBuffer buffer = {.readIndex = 0, .writeIndex = 0, .size = 0};


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
};

void keyboard_interrupt_handler() {
    uint8_t scancode = getScanCode();           
    updateFlags(scancode);                     

    // Actualizar el estado de las teclas
    if (scancode & RELEASE_OFFSET) {
        // Tecla liberada (bit más significativo en 1)
        key_states[scancode & ~RELEASE_OFFSET] = 0;
    } else {
        // Tecla presionada
        key_states[scancode] = 1;
    }

    char cAscii = scToAscii(scancode);        

    if (activeCtrl && (cAscii == 'r' || cAscii == 'R')) {
        request_snapshot();
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
    if (buffer_empty()){
        return 0;
    }

    char c = buffer.buffer[buffer.readIndex];
    buffer.readIndex = (buffer.readIndex + 1) % BUFFER_SIZE;
    buffer.size--;
    return c;
}

static int buffer_full() {
    return buffer.size == BUFFER_SIZE;
}

static int buffer_empty() {
    return buffer.size == 0;
}

char keyboard_read_getchar() {
    return buffer_pop();
}

static char scToAscii(uint8_t scancode) {
    char c = 0;

    // Primero chequea flechas
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
            break;
    }
    if (c != 0)
        return c;

    // Si no es flecha, busca en la tabla
    if (scancode < KEY_COUNT) {
        c = scancode_table[scancode][activeShift];
        if (activeCapsLock && c >= 'a' && c <= 'z') {
            c -= 32;
        }
    }
    return c;
}

// Implementación de la función is_key_pressed
uint8_t is_key_pressed(uint8_t scancode) {
    if (scancode < 256) {
        return key_states[scancode];
    }
    return 0;
}