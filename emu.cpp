#include <cstdint>

#include <iostream>		// cstd::cout
#include <iomanip>		// cstd::cout.setfill
#include <fstream>      // file io
#include <string>       // strings

//#define SYMBOL_PRINTER
//#define SYMBOL_DEBUG
//#define SYMBOL_SKIP
//#define SYMBOL_GBIT

#include "emu.h"

namespace EMU {

byte sink_hole[2]{};
int call_stack = 0; //debug
gb_registers reg;
bool debug = false, home_exit = false;
byte *DATA;
instr_result decode;

void init() {
    reg.bank = 1;
    reg.PC = 0x0100;
    reg.SP = 0xFFFE;
    reg.AF = 0x1180;
    reg.BC = 0x0000;
    reg.DE = 0xFF56;
    reg.HL = 0x000D;
}

bool load_rom(std::string filename) {
    DATA = new byte[ROM_SIZE];

    std::ifstream file(filename, std::ios::binary);
    if (!file) { std::cout << "ERROR: file.\n"; return false; }

    file.read (reinterpret_cast<char*>(DATA), ROM_SIZE);
    file.close();

    return true;
}

void emu_run() {
    int bail = 0;

    EMU::init();

    while (--bail) {
        emu_step();

#ifdef SYMBOL_PRINTER
        if (*address(0xFF02) & 0x80) {
            std::cstd::cout << *address(0xFF01) << std::flush;
            *address(0xFF02, true) = 0x01;
        }
#endif
    }

    std::cout << (!bail ? "\n[BAIL]" : "\n[exit]") << "\n";
}

bool emu_step() {
    bool it = true;
    byte op = OP(0);
    decode = NORM;

    if (debug) print_addr();

    switch(mask[op]) {
        case L: *L_target8(OP(0)) = *R_target8(OP(0)); break;
        case J: if (jump_op()) it = false; else decode = NO_JUMP; break;
        case U: unique_op(); break;
        case M: math_op((op >> 3) & 7, *R_target8(OP(0))); break;
        case X: break;
        case G: general_op(); break;
    }

    if (debug) { print_result(decode); print_reg(); std::cout << "\n"; }

    if (decode == FAIL) std::cout << "FAIL=" << std::hex << int(op) << "\n";

    if (it) reg.PC += instr_length[op];

#ifdef SYMBOL_SKIP
    static uint16_t last1, last2;

    if (decode == JUMP || decode == NO_JUMP) {
        if (reg.PC == last1 || reg.PC == last2) {
            if (debug) { std::cout << "SKIP... "; print_addr(last1); print_addr(last2); std::cout << "\n"; }
            debug = false;
        }
        else { last2 = last1; last1 = reg.PC; debug = true; }
    }
#endif

    //if(reg.IME && reg.IE & reg.IF) interrupt();

    return decode != EXIT;
}

bool jump_op() {
    byte op = OP(0);
    byte high = (op & 0xF0) >> 4, low = op & 0x0F;
    decode = JUMP;

    if ( (1 << (0xF - low) ) & 0xA8A8) {
        switch (high) {
            case 0x2: case 0xC: if (low > 0x7 ? !reg.Z  : reg.Z ) return false; break;
            case 0x3: case 0xD: if (low > 0x7 ? !reg.FC : reg.FC) return false; break;
        }
    }

    switch(op) {
        case 0xE9: //JP HL
            reg.PC = reg.HL;
            break;
        case 0xC2: case 0xC3: case 0xCA: case 0xD2: case 0xDA: //JP
            reg.PC = get_D16(reg.PC+1);
            break;
        case 0x20: case 0x30: case 0x18: case 0x28: case 0x38: //JR
            reg.PC += char(OP(1) + 2);
            break;
        case 0xD9: reg.IME = true; //RETI
        case 0xC0: case 0xD0: case 0xC8: case 0xC9: case 0xD8: //RET
            reg.PC = reg.pop();
            decode = RET;
            //if (home_exit && --call_stack == 0) decode = EXIT;
            break;
        case 0xC4: case 0xD4: case 0xCC: case 0xCD: case 0xDC: //CALL
            reg.push(reg.PC + instr_length[op]);
            reg.PC = get_D16(reg.PC+1);
            decode = CALL; ++call_stack;
            break;
        case 0xC7: case 0xD7: case 0xE7: case 0xF7:
        case 0xCF: case 0xDF: case 0xEF: case 0xFF: //RST
            reg.push(reg.PC + instr_length[op]);
            reg.PC = (high - 0xC) * 0x10 | (low == 0xF ? 0x8 : 0);
            decode = CALL;
            break;
        default: decode = FAIL;
    }

    return true;
}

void general_op() {
    byte op = OP(0);
    byte high = (op & 0xF0) >> 4;
    byte comb = ((op >> 3) & 0x10) | (op & 0x0F);

    switch(comb) {
        case 0x1: *reg16() = get_D16(reg.PC+1); break; // LD reg,d16
        case 0x9: { uint32_t result = reg.HL + *reg16(); carry((reg.HL ^ result ^ *reg16())>>8); reg.HL = result; reg.N = false; break; } // ADD HL,reg16
        case 0x3: ++(*reg16()); break; // INC reg16
        case 0xB: --(*reg16()); break; // DEC reg16
        case 0x2: *address((high & 0x2) ? reg.HL : *reg16(), true) = reg.A;  if (high == 0x2) ++reg.HL; else if (high == 0x3) --reg.HL; break; // LD (reg),a
        case 0xA: reg.A = *address((high & 0x2) ? reg.HL : *reg16(), false); if (high == 0x2) ++reg.HL; else if (high == 0x3) --reg.HL; break; // LD a,(reg)
        case 0x4: case 0xC: inc_dec(L_target8(OP(0)), 1); break; //INC reg
        case 0x5: case 0xD: inc_dec(L_target8(OP(0)),-1); break; //DEC reg
        case 0x6: case 0xE: *L_target8(OP(0)) = OP(1); break; //LD reg,d8
        case 0x7: case 0xF: bitmath(op); reg.Z = reg.N = reg.HC = false; break;

        case 0x11: *reg16() = reg.pop(); reg.F_pad = 0; break; //POP
        case 0x15: reg.push(*reg16()); break; //PUSH
        case 0x16: case 0x1E: math_op((op >> 3) & 7, OP(1)); break; //MATH
        case 0x10: //LDH
            if (high == 0xE) *address(0xFF00 + OP(1), true) = reg.A;
            else reg.A = *address(0xFF00 + OP(1), false);
            break;
        case 0x12: //LD (C),A
            if (high == 0xE) *address(0xFF00 + reg.C, true) = reg.A;
            else reg.A = *address(0xFF00 + reg.C, false);
            break;
        case 0x1A: //LD (a16),A
            if (high == 0xE) *address(get_D16(reg.PC+1), true) = reg.A;
            else reg.A = *address(get_D16(reg.PC+1), false);
            break;
        default: decode = FAIL;
    }
}

void unique_op() {
    switch(OP(0)) {
        case 0x37: reg.FC = true;    reg.HC = false; reg.N = false; break;
        case 0x3F: reg.FC = !reg.FC; reg.HC = false; reg.N = false; break;
        case 0x2F: reg.A = ~reg.A;   reg.HC = true;  reg.N = true;  break;
        case 0xFE: math_op(2, OP(1)); break;
        case 0xCB: bitmath(OP(1)); break;
        case 0xF3: reg.IME = false; break;
        case 0xFB: reg.IME = true;  break;
        case 0x27: daa(); break;
        case 0x08: set_D16(get_D16(reg.PC+1), reg.SP); break;
        case 0xF9: reg.SP = reg.HL; break;
        case 0xE8: add_sp(false); break;
        case 0xF8: add_sp(true); break;
        case 0x10: case 0x76: decode = /*reg.IME ? IGNORE :*/ EXIT; break;
        default: decode = FAIL;
    }
}

void math_op(byte op, byte rval) {
    uint16_t temp = reg.A;
    reg.N = false;
    bool hc = true, save = true;

    switch (op) {
        case 0: temp += rval; break;
        case 1: temp += (rval + reg.FC); break;
        case 2: temp -= rval;            reg.N = true; break;
        case 3: temp -= (rval + reg.FC); reg.N = true; break;
        case 4: temp &= rval; reg.HC = true;  hc = false; break;
        case 5: temp ^= rval; reg.HC = false; hc = false; break;
        case 6: temp |= rval; reg.HC = false; hc = false; break;
        case 7: temp -= rval; reg.N = true; save = false; break;
    }

    if (hc) reg.HC = ((temp ^ rval ^ reg.A) & 0x10);

    byte result = temp & 0xFF;
    reg.Z = !result;
    reg.FC = temp != result;

    if (save) reg.A = result;
}

void bitmath(byte instr) {
    byte high = (instr & 0xF0) >> 4;
    byte* v = R_target8(instr, true);
    byte t = (instr >> 3) & 7;

    switch(high >> 2) {
        case 0:
            if (t == 6) { *v = ((*v & 0xF) << 4) | ((*v & 0xF0) >> 4); reg.FC = false; }
            else {
                uint16_t temp = t & 0x1 ? *v << 7  : *v << 1;

                switch(t) {
                    case 0: temp |= (temp & 0x100) >> 8; break; //RLC
                    case 1: temp |= (temp & 0x80) << 8; break; //RRC
                    case 2: temp |= reg.FC; break; //RL
                    case 3: temp |= reg.FC << 15; break; //RR
                    case 5: temp |= (*v & 0x80) << 8; break; //SRA
                    default: break; //SLA SRL
                }

                if (t & 0x1) { *v = temp >> 8; reg.FC = temp & 0x80; }
                else { *v = temp; reg.FC = temp & 0x100; }
            }

            reg.Z = (*v == 0);
            reg.N = false; reg.HC = false;
            break;
        case 1: reg.Z = !((0x1 << t) & *v); reg.N = false; reg.HC = true; return;
        case 2: *v &= ~(0x1 << t); break;
        case 3: *v |= (0x1 << t); break;
    }
}

inline void interrupt() {
    for (byte i = 0; i != 4; ++i) {
        byte flag = 1 << i;
        if (reg.IF & flag) {
            reg.push(reg.PC);
            reg.PC = 0x40 * i;
        }
    }
}

inline void daa() {
    char add = 0;
    if ((!reg.N && (reg.A & 0xf) > 0x9) || reg.HC) add |= 0x6;
    if ((!reg.N && reg.A > 0x99) || reg.FC) { add |= 0x60; reg.FC = true; }
    reg.A += reg.N ? -add : add;
    reg.Z = !reg.A;
    reg.HC = false;
}

inline void add_sp(bool set_hl) {
    uint16_t temp = reg.SP + char(OP(1));

    carry(reg.SP ^ temp ^ uint16_t(char(OP(1))));
    if (set_hl) reg.HL = temp; else reg.SP = temp;
    reg.Z = reg.N = false;
}

inline void carry(uint16_t change) {
    reg.HC = bool(change & 0x10);
    reg.FC = bool(change & 0x100);
}

inline void inc_dec(byte *loc, char delta) {
    byte before = *loc;
    *loc += delta;

    reg.HC = (*loc ^ before) & 0x10;
    reg.N = delta != 1;
    reg.Z = !(*loc);
}

byte* L_target8(byte instr) {
    byte t = (instr >> 3) & 0x7;

    switch (t) {
        case 0x6: return address(reg.HL, true);
        case 0x7: return &reg.A;
        default: return &reg.RAW[(t ^ 0x1) +2]; //^1 = endian
    }
}

byte* R_target8(byte instr, bool write) {
    byte low = instr & 0x0F;

    switch(low) {
        case 0x6: case 0xE: return address(reg.HL, write);
        case 0x7: case 0xF: return &reg.A;
        default: return &reg.RAW[ ((low & 0x7) ^ 0x1) + 2 ]; //^1 = endian
    }
}

uint16_t* reg16() {
    byte high = (OP(0) & 0xF0) >> 4;

    switch (high) {
        case 0x3: return &reg.SP;
        case 0xF: return &reg.AF;
        default: return &reg.RAW16[(high & 0x3) +1];
    }
}

uint16_t get_D16(uint16_t addr) {
    return (*address(addr+1)<<8) | *address(addr);
    //return *(uint16_t*)address(addr);
}

void set_D16(uint16_t addr, uint16_t data) {
    *address(addr+1, true) = byte(data >> 8);
    *address(addr, true) = byte(data & 0xFF);
    //*(uint16_t*)address(addr) = data;
}

byte* address_banked(byte bank, uint16_t addr) {
    //if (addr >= BANK_SIZE*2) std::cout << "bank overflow\n"; //debug

    if (addr >= BANK_SIZE) return DATA + addr + (bank-1) * BANK_SIZE;
    else return DATA + addr;
}

byte* address(uint16_t addr, bool write) {
    #ifdef SYMBOL_GBIT
        if (addr > instruction_mem_size) {
            for (int i = 60; i != 80; ++i) reg.memory[i] = 0xAA;
            return &reg.memory[69];
        }
    #endif

    if (addr == 0xFF44) ++reg.memory[0x5F44]; //LY 0xA3AD

    if (!write && addr < 0x8000) return address_banked(reg.bank, addr);
    else if (addr >= 0xA000) return &reg.memory[addr-0xA000];
    else if (write && addr >= 0x2000 && addr < 0x4000) return &reg.bank;
    else return sink_hole;
}

uint16_t gb_address(byte* p) {
    if (p >= DATA && p < DATA + ROM_SIZE) {
        int addr = p - DATA;
        if (addr < BANK_SIZE) return addr;
        else return addr % BANK_SIZE + BANK_SIZE;
    }
    else return p - reg.memory + 0xA000;
}

void print_addr() {
    std::cout << std::setfill('0') << std::hex
        << (reg.PC < BANK_SIZE ? 0 : int(reg.bank)) << ":" << std::setw(4) << reg.PC << ": "
        << std::setw(2) << int(OP(0)) << ".. "
        << std::setw(2) << int(OP(1))
        << std::setw(2) << int(OP(2)) << " " << std::flush;
}

void print_addr(uint16_t addr) {
    std::cout << std::setfill('0') << std::hex
        << "<" << (addr < BANK_SIZE ? 0 : int(reg.bank)) << ":" << std::setw(4) << addr << ">"
        << std::flush;
}

void print_reg() {
    std::cout  << std::hex
        << " HL=" << std::setw(4) << int(reg.HL)
        << " BC=" << std::setw(4) << int(reg.BC)
        << " DE=" << std::setw(4) << int(reg.DE)
        << " AF=" << std::setw(4) << int(reg.AF)
        << " SP=" << reg.SP
        << " " << (reg.Z ? "Z" : ".") << (reg.HC ? "H" : ".") << (reg.N ? "N" : ".") << (reg.FC ? "C" : ".");
}

void print_result(instr_result it) {
    switch(it) {
        case FAIL:   std::cout << "[FAIL]"; break;
        case IGNORE: std::cout << "[ -- ]"; break;
        case NO_JUMP:std::cout << "[NOJP]"; break;
        case JUMP:   std::cout << "[JUMP]"; break;
        case CALL:   std::cout << "[CALL]"; break;
        case RET:    std::cout << "[RET ]"; break;
        case EXIT:   std::cout << "[EXIT]"; break;
        default:	 std::cout << "[    ]";
    }
}

/*void print_masks() {
    for (int i = 0; i < 256; ++i) {
        if ((i & 0x0F) == 0) std::cout << "\n";

        if (instr_mask(i, mask_jump))  std::cout << "J";
        else if (instr_mask(i, mask_unique)) std::cout << "U";
        else std::cout << "-";
    }

    std::cout << "\n\n";
}*/

}
