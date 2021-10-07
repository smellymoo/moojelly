//#include <wx-3.0/wx/file.h>
//#include <stack>

#include "sml.h"
#include "sml2.h"
#include "wx/image.h"

using namespace ROM;
using namespace std;

namespace SML2 {

    void render_level(Bitmap &level_bitmap) {
		cout << "load level " << CL << ".\n";

		set_pal(HEADER(pal_bg));

		uint16_t* tile_cache = new uint16_t[8*16*16]{};
		cache_tiles(tile_cache);

		Bitmap cache(16, 8*16*16, true);
		cache_blocks(cache, tile_cache);
		special_blocks(cache);

		byte* unzipped = load_map();

		for (int y = 0; y < LEVEL_HEIGHT; ++y) for (int x = 0; x < LEVEL_WIDTH; ++x)  {
			byte b = unzipped[x + y * LEVEL_WIDTH];
			copy_block(cache, level_bitmap, b, x, y);
		}

//		for (int y = 0; y < 2; ++y) for (int x = 0; x < 16; ++x) {
//			byte scroll = *(bank(0xC) + CL*32 + y*16+x);
//
//			uint16_t *warp_id = (uint16_t*) (bank(0xC) + 0x0F30 + CL*64 + (y*16+x)*2);
//		}

		render_sprites(level_bitmap, tile_cache);

		point16 player = HEADER(player);
		tint_block(level_bitmap, player.x/16, player.y/16, 2);

		delete[] unzipped;
		delete[] tile_cache;
	}

    byte* load_map() {
	    return ROM::load_map(bank(HEADER(map_bank)) + 0x500, HEADER(map_sub));
	}

	void cache_tiles(uint16_t* cache) {
        copy_cache_tiles(cache, address(0x1B, 0x7000), 0, 48);
        copy_cache_tiles(cache, address(7, 0x6A00), 104, 56);

        auto tiles = reinterpret_cast<header2_struct*> (address(0, HEADER_TILES));
        copy_cache_tiles(cache, address(tiles[CL].bank, tiles[CL].addr), 160, 96);
	}


	void cache_blocks(Bitmap &cache, uint16_t* tile_cache) {
		for (uint16_t b = 0; b < 8*16; ++b) for (uint16_t ty = 0; ty < 2; ++ty) for (uint16_t tx = 0; tx < 2; ++tx) {
			byte t = *(bank(7) + HEADER(blocks) + b*4 + tx+ty*2);
			t -= 128;

			render_tile(cache, tile_cache + t * 8, tx, b*2+ty);
		}
	}


	void special_blocks(Bitmap &cache) {
		const RGB_DATA red = {0,96,96}, blue = {96,96,0}, purple = {0,96,0}, solid = {0,16,32}, sea = {48,32,0},
			green = {64,0,64}, yellow = {0,0,64}; //grey = {64,64,64}

		struct b_list { int start; int length; RGB_DATA color; };

        //copy_cache_block(cache, 7, 2); //solid
        copy_cache_block(cache, 1, 126); //?brick

        b_list blocks[] = {
            {2,1,solid}, {126,1,purple}, {4,2,blue}, {8,8,green}, {16,8,red}, {80,2,red},
            {1,1,yellow}, {24,32,solid}, {82,4,sea}, {76,4,purple}
        };

        for (auto& c : blocks) for (int r = 0; r < c.length; ++r) color_cache_block(cache, c.start + r, c.color);
	}


	void unpack_sprites(byte *data, byte** store) {
		int pos = 0, start = 0;

		for (int l = 0; l < LEVELS; ++l) {
			while (data[pos] != 0xFF) pos += 3;
			++pos;

			store[l] = new byte[pos - start];
			memcpy(store[l], data + start, pos - start);
			start = pos;
		}
	}


	void unzip_sprites(byte *data, vector<sprite> &sprites) {
		int p = 0;

		while (data[p] != 0xFF) {
            byte a = data[p++];
            byte b = data[p++];
            byte c = data[p++];

            sprites.emplace_back(
                (a & 0x0F) * 32 + (b & 0x1F), (c & 0x7F), //X, Y
                (a & 0x10) << 2 | (a & 0xE0) >> 2 | (b & 0xE0) >> 5	//TYPE
            );
        }
	}


	void render_sprite(Bitmap &bitmap, uint16_t* tile_cache, byte type, int tx, int ty) {
        type = *address(2, (CL<10 ? 0x4B61 : 0x4BC1) + type);
        byte* t = deref(3, (CL<10 ? 0x40B1 : 0x4F11), type);

        int bail = 32;
		while(t[0] != 0x80 && --bail) {
			int x = tx*8 + char(t[1]), y = ty*8 + char(t[0]);

			if (x < 0 || y < 0 || x+8 > 256*16 || y+8 > 48*16 || (abs(char(t[1])) + abs(char(t[0])) > 64)) { cout << "sprite mess.\n"; break; }

			render_tile(bitmap, tile_cache + (int(t[2]) - 128) * 8, x, y, t[3] & 0x20, t[3] & 0x40, true);

			t += 4;

			//tint_tile(bitmap, x, y);
		}

		for (int y = -1; y < 1; ++y) for (int x = -1; x < 1; ++x) tint_tile(bitmap, (tx+x)*8, (ty+y-1)*8);
	}


	void render_sprites(Bitmap &bitmap, uint16_t* cache) {
		vector<sprite> sprites;

		set_pal(HEADER(pal_sprite1));

        byte* store[LEVELS];
        unpack_sprites(DATA + 0xE077, store);
        unzip_sprites(store[CL], sprites);

        byte *p = address(0x1B, SPRITES_TILES + CL*4);
        copy_cache_tiles(cache, address(p[0], *(uint16_t*)(p+2)), 48, 56);


		for(auto sprite: sprites) render_sprite(bitmap, cache, sprite.type, sprite.x, sprite.y);
	}
}
