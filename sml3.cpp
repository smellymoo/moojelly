//#include <wx-3.0/wx/file.h>
//#include <stack>

#include "wx/image.h"

#include "sml.h"
#include "sml3.h"
#include "emu.h"

using namespace std;
using namespace ROM;

namespace SML3 {

    void render_level(Bitmap &level_bitmap) {
		uint16_t* tile_cache = new uint16_t[8*16*16]{};
		Bitmap cache(16, 8*16*16, true);

        cout << "load level " << CL << ".\n";

        set_pal(HEADER(pal_bg));

		byte* unzipped = load_map();
        byte* sectors = new byte[48]{};
        byte* trace = new byte[LEVEL_SIZE]{};

        header_struct* savepoint = reinterpret_cast<header_struct*> (deref(12, HEADER_SML3, CL + 43));

        point16 player = decode_player(HEADER(player));
        point16 player2 = decode_player(savepoint->player);

        trace_fill(unzipped, trace, sectors, xy(player.x>>4, player.y>>4), 1);
        trace_fill(unzipped, trace, sectors, xy(player2.x>>4, player2.y>>4), 2);

        trace_fill_warps(unzipped, trace, sectors);

        load_cache(cache, tile_cache, header, false);

        vector<sprite> sprites;
        unzip_sprites(deref(7, 0x4199, CL), sprites);

        byte current = 255;
        for (int sx = 0; sx != 16; ++sx) for (int sy = 2; sy >= 0; --sy) {
            byte s = sx + sy * 16;
            uint16_t enemy_table;

            if (sectors[s] != current && sectors[s] != 0) {
                current = sectors[s];
                //cout << int(current) << " " << flush;

                if (current <= 2) {
                    byte* h = (current == 2 ? reinterpret_cast<byte*>(savepoint) : header);
                    enemy_table = WARP(h, enemies, false);
                    load_cache(cache, tile_cache, h, false);
                } else {
                    byte* h = reinterpret_cast<byte*>(get_warp(current - 16));
                    load_cache(cache, tile_cache, h, true);
                    enemy_table = WARP(h, enemies, true);
                }
            }

            render_map(level_bitmap, cache, unzipped, sx, sy);

            render_sprites(level_bitmap, tile_cache, enemy_table, sprites, sx, sy);
        }

		tint_block(level_bitmap, player.x/16, player.y/16, 2);
        //trace_debug(level_bitmap, trace);

		delete[] unzipped;
		delete[] tile_cache;
		delete[] sectors;
		delete[] trace;
	}

    byte* load_map() {
	    return ROM::load_map(bank(HEADER(map_bank)) + 0x40, HEADER(map_sub));
	}

    void cache_tiles(uint16_t* cache, byte* addr, bool warp) {
        copy_cache_tiles(cache, address(17, WARP(addr, tiles2, warp)), 0, 32);
        copy_cache_tiles(cache, address(WARP(addr, tile_bank, warp), WARP(addr, tiles, warp)), 32, 96);

        if (WARP(addr, animated_tilemask, warp) != 0) copy_cache_tiles(cache, address(17, WARP(addr, tiles_animated, warp)), 32, 4);
	}


    void cache_blocks(Bitmap &cache, uint16_t* tile_cache, uint16_t addr) {
		for (uint16_t b = 0; b != 8*16; ++b) for (uint16_t ty = 0; ty != 2; ++ty) for (uint16_t tx = 0; tx != 2; ++tx) {
			byte t = *(bank(10) + addr + b*4 + tx+ty*2);

			render_tile(cache, tile_cache + t * 8, tx, b*2+ty);
		}
	}


	void special_blocks(Bitmap &cache) {
		const RGB_DATA red = {0,96,96}, blue = {96,96,0}, purple = {0,96,0}, solid = {0,16,32}, sea = {48,32,0},
			yellow = {0,0,64}, aqua = {64,0,0}, grey = {64,64,64};//, green = {64,0,64};

		struct b_list { int start; int length; RGB_DATA color; };


        copy_cache_block(cache, 40, 73); //? brick
        copy_cache_block(cache, 40, 74); //? brick

        b_list blocks[] = {
            {74,1,blue}, {76,6,sea}, {83,1,sea}, {88,1,sea},
            {0,40,solid}, {73,1,purple}, {82,1,purple}, {84,1,purple}, {72,1,purple}, {75,1,purple}, {46,1,purple}, {45,1,purple},
            {89,2,red}, {86,2,red}, {92,4,red},
            {41,2,grey}, {52,4,aqua}, {40,1,yellow},
        };

        for (auto& c : blocks) for (int r = 0; r < c.length; ++r) color_cache_block(cache, c.start + r, c.color);

	}


	constexpr byte switch_block(byte block, byte mode) {
		switch(mode){
			case 1:
				switch(block){
					case 0x7C: return 0x27;
					case 0x27: return 0x7C;
					case 0x7A: return 0x44;
					case 0x79: return 0x45;
				}
			case 2:
				switch(block) {
					case 0x7C: return 0x55;
					case 0x55: return 0x7C;
					case 0x7A: return 0x7B;
					case 0x7B: return 0x7A;
					case 0x59: return 0x5D;
					case 0x5D: return 0x59;
				}
			default: return block;
		}
	}


	void render_map(Bitmap &bitmap, Bitmap &cache, byte* unzipped, int sx, int sy) {
        for (int y = 0; y != 16; ++y) for (int x = 0; x != 16; ++x)  {
            int lx = sx * 16 + x, ly = sy * 16 + y;
			byte b = unzipped[lx + ly * LEVEL_WIDTH];
			b = switch_block(b, switch_state);
			copy_block(cache, bitmap, b, lx, ly);
		}
	}


    uint16_t find_door(byte* unzipped, uint16_t s) {
        for (int y = 0; y != 16; ++y) for (int x = 0; x != 16; ++x)  {
            int lx = (s & 0xF) * 16 + x, ly = (s >> 4) * 16 + y;
			byte b = unzipped[lx + ly * LEVEL_WIDTH];

			if (fill_type(b) == door) return lx + ly * LEVEL_WIDTH;
		}

		return 0xFFFF;
	}

	warp_struct* get_warp(byte s) {
        uint16_t warp = *(uint16_t*) address(0xC, 0x4F30 + CL*64 + s*2);
        return reinterpret_cast<warp_struct*>(address(0xC, warp));
	}


	void load_cache(Bitmap &cache, uint16_t* tile_cache, byte* header, bool warp) {
	    memset(tile_cache, 0, 16*16*16);
	    set_pal(WARP(header, pal_bg, warp));

        cache_tiles(tile_cache, header, warp);
        cache_blocks(cache, tile_cache, WARP(header, blocks, warp));

        special_blocks(cache);
	}


	point16 decode_player(point16 player) {
	    point16 ret;
        ret.x = swap_bytes(player.x);
        ret.y = swap_bytes(player.y) + 14*16;
        return ret;
	}


	void unzip_sprites(byte *data, vector<sprite> &sprites) {
		int p = 0;

        while(p < 0x2000) {
            byte b = *(data++);
            byte l = b >> 4, r = b & 0xF;

            if(l) {
				sprites.emplace_back(p, l);
			} 
			++p;
            
			if(r) {
				sprites.emplace_back(p, r); 
			}
			++p;
            p += *(data++) * 2;
        }
	}

	void render_sprite(Bitmap &bitmap, uint16_t* tile_cache, uint16_t enemy_table, byte type, int tx, int ty) {
		byte *t;
        int bail = 32;

        uint16_t level_sprites, tile_map = 0;
        byte* tile_table;

        lookup_sprites(enemy_table, level_sprites, tile_table);

        auto sprite = deref(7, level_sprites + (type) * 2); // loc(16); byte; bank;
        tile_map = lookup_sprite_tilemap(*(uint16_t*)sprite);

        tile_table += 4 * (type-1);

        if (type > 6) {
            int fixed_sprites[] = { 0x4ACB, 0x4AEB, 0x4ADB, 0x4AFB, 0x4B0B, 0x4B1B, 0x4B2B, 0x4B4B, 0x4B5B };
            tile_map = fixed_sprites[type-7];
            t = deref(0xF, tile_map);

            copy_cache_tiles(tile_cache, address(0x05, 0x4C81), 0, 23);
            copy_cache_tiles(tile_cache, address(0x11, 0x4C81), 23, 16);
        } else {
            if (tile_map == 0) { cout << "null tile_map\n"; tint_tile(bitmap, tx*8, ty*8); return; }

            t = deref(sprite[3], tile_map);

            if (sprite[2] & 0x42) bail = 1; //invisible?

            copy_cache_tiles(tile_cache, address(tile_table[0], *(uint16_t*) (tile_table+1)), 0, tile_table[3]);
        }

        set_pal((*(t++) & 0x10) != 0 ? 0xD1 : 0x1E);


		while(t[0] != 0x80 && --bail) {
			int x = tx*8 + char(t[1]), y = ty*8 + char(t[0]);

			if (x < 0 || y < 0 || x+8 > 256*16 || y+8 > 48*16 || (abs(char(t[1])) + abs(char(t[0])) > 64)) { cout << "sprite mess.\n"; break; }

			render_tile(bitmap, tile_cache + (t[2] & 0x3F) * 8, x, y, t[2] & 0x40, t[2] & 0x80, true);

			t += 3;

			//tint_tile(bitmap, x, y);
		}

		for (int y = -1; y < 1; ++y) for (int x = -1; x < 1; ++x) tint_tile(bitmap, (tx+x)*8, (ty+y-1)*8);
	}


	void render_sprites(Bitmap &bitmap, uint16_t* cache) {
		vector<sprite> sprites;


        unzip_sprites(deref(7, 0x4199, CL), sprites);

//			for (int e = 0; e < 6; ++e) {
//				uint16_t sprite = *(uint16_t*)address(7, id + (e + 1) * 2);
//				if (sprite != 0x530f) {
//					uint16_t sprite2 = *(uint16_t*)address(7, sprite);
//					cout << hex << e << ":" << sprite << " => " << sprite2 << "(" << int(*address(7, sprite+3)) << ")\n";
//
//					uint16_t tile_map = lookup_sprite_tilemap(sprite2);
//				}
//			}

        //copy_cache_tiles(cache, address(0x5, 0x42F1), 0, 64); //wario

		for(auto sprite: sprites) render_sprite(bitmap, cache, HEADER(enemies), sprite.type, sprite.x, sprite.y);
	}


    void render_sprites(Bitmap &bitmap, uint16_t* cache, uint16_t enemy_table, vector<sprite> &sprites, int sx, int sy) {
		for(auto sprite: sprites) {
            if (sprite.y > (sy+1) * 32) return;
            if (sprite.x > (sx+1) * 32 || sprite.x < sx * 32 || sprite.y < sy * 32) continue;

            render_sprite(bitmap, cache, enemy_table, sprite.type, sprite.x, sprite.y);
		}
	}


	void lookup_sprites(uint16_t enemy_table, uint16_t &id, byte* &tile_table) {
		byte* p = address(7, enemy_table);
		byte a;
		uint16_t addr;

		while(p[0] != 0xC9) {	//ret
			switch (p[0]) {
			case 0x3E: //ld a, byte;
				a = p[1];
				break;
			case 0xEA:  // ld (x), a
				addr = *(uint16_t*)(p+1);
				if (addr == 0xA32F) id = a;
				else if (addr == 0xA330) id |= a << 8;
				break;
			case 0x01: //ld bc, u16
				tile_table = address(7, *(uint16_t*)(p+1));
				break;
			}

			p += EMU::instr_length[p[0]];
		}
	}

    void init(byte bank, uint16_t run) {
		//debug = false; home_exit = true;
		EMU::DATA = ROM::DATA;
		EMU::reg.bank = bank;
		*EMU::address(0xA8C5, true) = bank; //SML3

		EMU::reg.PC = run;
		EMU::reg.SP = 0xAFFF;
    }


	uint16_t lookup_sprite_tilemap(uint16_t code) {
		EMU::set_D16(0xA112, code);

//		cout << "\nlookup = " << hex << EMU::get_D16(0xA112) << std::flush << "\n";

		int bail = 1000000;
		init(2, 0x40B5); //02:40B5
		*EMU::address(0xA100, true) = 1; //load
		*EMU::address(0xA3AD, true) = 0xFF; //wait until not drawing screen

		while (--bail) {
			if (!EMU::emu_step()) break;
			if (EMU::reg.PC <= 100) break;
		}

//		cout << EMU::get_D16(0xA112) << " >> " << EMU::get_D16(0xA10C) << "\n";
//
//		init(2, 0x40B5);
//		*EMU::address(0xA100, true) = 2; //load
//		*EMU::address(0xbf00, true) = 0x65; //tile?
//
//		while (bail--) {
//			if (!EMU::emu_step()) break;
//			if (EMU::reg.PC <= 100) break;
//		}

//		cout << "result = " << EMU::get_D16(0xA112) << " >> " << EMU::get_D16(0xA10C) << "\n";

		return EMU::get_D16(0xA10C);
	}


	constexpr fill_t fill_type(byte block) {

		if ((block >= 0) && (block <= 40) || (block == 60) || (block == 73) || (block == 74) || ((block >= 50) && (block <= 55)))
		{
			return solid;
		}
		if ((block >= 64) && (block <= 67))
		{
			return platform;
		}

		if ((block >= 41) && (block <= 46))
		{
			return smash;

		}

		if ((block >= 76 && block <= 82) || (block == 84) || (block == 88))
		{
			return sea;
		}

		if((block>=89 && block<=90) || (block>=86 && block<=87) || (block>=92 && block<= 95))
		{
			return spike;
		}

		switch (block) {
			/*case 0 ... 40:
			case 60: 
			case 73 ... 74: case 50 ... 55: return solid;*/
			case 47: return bounce;
			/*case 64 ... 67: return platform;*/
			/*case 41 ... 46: return smash;*/
			/*case 76 ... 82: case 84: case 88: return sea;*/
			case 68:
			case 69: return follow;
			/*case 89 ... 90: case 86 ... 87: case 92 ... 95: return spike;*/
            case 72: case 75: return door;
			default: return sky;
		}
		return sky;
	}

	void trace_fill(byte* blockmap, byte* mark, byte* mark_sector, uint16_t from, byte filler) {
	    if (mark[from]) return;

		std::stack<fill_todo> todo;

		todo.emplace(from, 0xF, 0);
		mark[from] = filler;

		while (!todo.empty()) {
			fill_todo c = todo.top(); todo.pop();
			int s = (get_x(c.p) >> 4) + (get_y(c.p) >> 4) * 16;

			if (mark_sector[s] && mark_sector[s] != filler) cout << "fill collision (" << int(s) << ").\n";
            if (!mark_sector[s]) mark_sector[s] = filler;

			int up = c.p - LEVEL_WIDTH;
			int dn = c.p + LEVEL_WIDTH;
			int lt = c.p - 1;
			int rt = c.p + 1;

			byte jump = c.jump ? c.jump-1 : 0;

			if (get_y(c.p) != LEVEL_HEIGHT-1) {
                fill_t dn_type = fill_type(blockmap[dn]);

				if (c.flags & 0x8 && dn_type >= smash) FILL(dn, 0xF ^ 0x1, 0);

				switch (dn_type) {
                    case bounce: jump = 9; break;
                    case spike: jump = 0; break;
                    case sky: break;
                    default: jump = 3;
                }
			}

			if (get_y(c.p)) {
                if (fill_type(blockmap[c.p]) == sea) jump = 2;

                if (c.flags & 0x1 && fill_type(blockmap[up]) == follow) {
                    FILL(up, 0xF ^ 0x8, 3);
                } else {
                    int jp = c.p;
                    byte j = jump;

                    while (get_y(jp)) {
                        jp -= LEVEL_WIDTH;
                        if (!j-- || !fill_check_border(blockmap, s, jp)) break;

                        FILL(jp, 0xF ^ 0x8, j);
                    }
                }
			}

            if (c.flags & 0x2 && fill_check_border(blockmap, s, lt) && get_x(c.p)) FILL(lt, 0xF ^ 0x4, jump);

			if (c.flags & 0x4 && fill_check_border(blockmap, s, rt) && get_x(c.p) != LEVEL_WIDTH-1) FILL(rt, 0xF ^ 0x2, jump);
		}
	}


	bool fill_check_border(byte* blockmap, byte s, uint16_t p) {
        if (fill_type(blockmap[p]) < platform) return false;

        byte s2 = get_sector(p);

        if (s2 != s) {
            if (s2 < 16) return false;

            if      (s2 - s == 1 && scroll_border(s-16) & 1) return false;
            else if (s - s2 == 1 && scroll_border(s-16) & 2) return false;
        }

        return true;
	}


	byte scroll_border(byte s) {
        return *(address(0xC, 0x4000 + CL*32 + s));
	}


	void trace_fill_warps(byte* blockmap, byte* mark, byte* mark_sectors) {
        for (int i = 0; i != LEVEL_SIZE; ++i) {
            byte b = blockmap[i];

            if (b == 84 || b == 72 || b == 75) { //doors
                uint16_t s = (get_y(i) >> 4) * 16 + (get_x(i) >> 4);
                warp_struct* warp = get_warp(s - 16);

                if (warp->sector < 32) {
                    uint16_t x = (warp->sector & 0xF) * 16 + (warp->player_x >> 4);
                    uint16_t y = (warp->sector >> 4) * 16 + (warp->player_y >> 4) + 16 - 1;

                    trace_fill(blockmap, mark, mark_sectors, xy(x, y), s);

                    cout << int(s - 16) << ">" << int(warp->sector) << ": " << int(warp->player_x >> 4) << "," << int(warp->player_y >> 4) << " " << flush;
                }
            }
        }
	}

	void trace_debug(Bitmap &level_bitmap, byte* trace) {
        for (int y = 0; y < LEVEL_HEIGHT; ++y) for (int x = 0; x < LEVEL_WIDTH; ++x)  {
			if (trace[x + y * LEVEL_WIDTH]) tint_block(level_bitmap, x, y, trace[x + y * LEVEL_WIDTH]-1);
		}
	}
}
