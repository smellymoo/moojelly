#pragma once

typedef unsigned char byte;

#define OP(N)       *address(reg.PC+N)

#define ROM_SIZE    512*1024
#define BANK_SIZE   0x4000

/* ############################################ */

namespace EMU {
    enum instr_type : byte { X, G, L, M, J, U, };

    enum instr_result { NORM, FAIL, IGNORE, NO_JUMP, JUMP, CALL, RET, EXIT, };

    void init();
    void emu_run();
    bool emu_step();

    bool jump_op();
    void general_op();
    void unique_op();
    void stack_op();
    void math_op(byte op, byte rval);

    void interrupt();

    void add_sp(bool which);
    void carry(uint16_t change);
    void inc_dec(byte *loc, char delta);
    void bitmath(byte instr);
    void daa();

    uint16_t* reg16();
    byte* L_target8(byte instr);
    byte* R_target8(byte instr, bool write = false);

    uint16_t get_D16(uint16_t addr);
    void set_D16(uint16_t addr, uint16_t data);

    byte* address(uint16_t addr, bool write = false);
    uint16_t gb_address(byte* p);

    void print_addr();
    void print_addr(uint16_t addr);
    void print_result(instr_result it);
    void print_reg();
    void print_masks();

    bool load_rom(std::string filename);
    byte* address_banked(byte bank, uint16_t address);

    /* ############################################ */


    const byte mask[256] = {
        X,G,G,G,G,G,G,G, U,G,G,G,G,G,G,G,
        U,G,G,G,G,G,G,G, J,G,G,G,G,G,G,G,
        J,G,G,G,G,G,G,U, J,G,G,G,G,G,G,U,
        J,G,G,G,G,G,G,U, J,G,G,G,G,G,G,U,
        L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,L,
        L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,L,
        L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,L,
        L,L,L,L,L,L,U,L, L,L,L,L,L,L,L,L,
        M,M,M,M,M,M,M,M, M,M,M,M,M,M,M,M,
        M,M,M,M,M,M,M,M, M,M,M,M,M,M,M,M,
        M,M,M,M,M,M,M,M, M,M,M,M,M,M,M,M,
        M,M,M,M,M,M,M,M, M,M,M,M,M,M,M,M,
        J,G,J,J,J,G,G,J, J,J,J,U,J,J,G,J,
        J,G,J,X,J,G,G,J, J,J,J,X,J,X,G,J,
        G,G,G,X,X,G,G,J, U,J,G,X,X,X,G,J,
        G,G,G,U,X,G,G,J, U,U,G,U,X,X,G,J,
    };

    const byte instr_length[256] = {
        1,3,1,1,1,1,2,1, 3,1,1,1,1,1,2,1,
        1,3,1,1,1,1,2,1, 2,1,1,1,1,1,2,1,
        2,3,1,1,1,1,2,1, 2,1,1,1,1,1,2,1,
        2,3,1,1,1,1,2,1, 2,1,1,1,1,1,2,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,3,3,3,1,2,1, 1,1,3,2,3,3,2,1,
        1,1,3,1,3,1,2,1, 1,1,3,1,3,1,2,1,
        2,1,1,1,1,1,2,1, 2,1,3,1,1,1,2,1,
        2,1,1,1,1,1,2,1, 2,1,3,1,1,1,2,1,
    };

    struct gb_registers {
        union {
            struct {
                union {
                    uint16_t AF;
                    struct {

                         struct __attribute__ ((packed)) {
                            int F_pad:4;
                            bool FC:1; bool HC:1; bool N:1; bool Z:1;
                        };
                        byte A;
                    };
                };
                union {
                    uint16_t BC;
                    struct { byte C; byte B; };
                };
                union {
                    uint16_t DE;
                    struct { byte E; byte D; };
                };
                union {
                    uint16_t HL;
                    struct { byte L; byte H; };
                };
            };

            byte RAW[8]{};
            uint16_t RAW16[4];
        };

        uint16_t SP, PC;


        byte memory[0x6000]{};
        byte bank = 0;

        bool IME = false;
        byte IE = 0, IF = 0;

        uint16_t pop() { uint16_t ret = get_D16(SP); SP += 2; return ret;  }

        void push(uint16_t val) { SP -= 2; set_D16(SP, val); }
    };

    extern gb_registers reg;
    extern byte *DATA;
}
