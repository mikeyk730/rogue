//setup: Get starting setup for all games
void setup();

void clock_on();

void no_clock();

//flush_type: Flush typeahead for traps, etc.
void flush_type();

void credits();

int getkey();

//readchar: Return the next input character, from the macro or from the keyboard.
int readchar();

int bdos(int fnum, int dxval);

int swint(int intno, struct sw_regs *rp);

void one_tick();