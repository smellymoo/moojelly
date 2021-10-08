#ifdef _MSC_VER 
#include <wx/file.h>
#else
#include <wx-3.0/wx/file.h>
#endif
#include <stack>

#include "sml.h"
#include "sml2.h"
#include "sml3.h"
#include "wx/image.h"

using namespace std;

namespace ROM {
	byte* DATA;
	byte* header;

    char *rom_title;
    byte rom_ver;
    int game;
	int CL = 0, switch_state = 0;

	RGB_DATA palette[4], global_pal[4] = { {255,255,255}, {200,200,200}, {100,100,100}, {0,0,0} };


	bool load_rom(string filename) {
		DATA = new byte[ROM_SIZE];

		ifstream file(filename, ios::binary);
		if (!file) { cout << "ERROR: file.\n"; return false; }

		file.read (reinterpret_cast<char*>(DATA), ROM_SIZE);
		file.close();

		rom_title = reinterpret_cast<char*> (address(0,0x134));
		rom_ver = DATA[332];

		if (strcmp(rom_title, "SUPERMARIOLAND3")==0) game = 3;
		else if (strcmp(rom_title, "MARIOLAND2")==0) game = 2;
		else { cout << "ERROR: ROM '" << rom_title << "' not supported.\n"; return false; }

		cout << "LOADED: " << rom_title << " (v1." <<  static_cast<int>(rom_ver) << ").\n";
		return true;
	}


    void set_level(int level) {
        CL = level;

	    if (game == 2)  header = deref( 1, HEADER_SML2, CL);
	    else            header = deref(12, HEADER_SML3, CL);
	}


	void special_blocks(Bitmap &cache) {
        CALLV(special_blocks(cache));
	}


	void render_level(Bitmap &level_bitmap) {
        CALLV(render_level(level_bitmap));
	}


	void set_pal(byte pal) {
		for(byte i = 0; i < 4; ++i) palette[i] = global_pal[(pal >> i * 2) & 0x3];
	}


	void unzip_map(byte* &map_data, byte* &unzip) {
		int z = 0;

		while (z < LEVEL_SIZE) {
			if (*map_data & 128) {
				byte c = *map_data++;
				for (short r = *map_data; r >= 0; --r) unzip[z++] = c-128;
			}
			else unzip[z++] = *map_data;

			++map_data;
		}

		if (z != LEVEL_SIZE) cout << "ERROR: unzip.\n";
	}


	void skip_map(byte* &map_data) {
		int z = 0;

		do {
			if (*map_data >= 128) z += *(++map_data) +1;
			else ++z;

			++map_data;
		} while (z < LEVEL_SIZE);

		if (z != LEVEL_SIZE) cout << "ERROR: skip.\n";
	}


	byte* load_map(byte* offset, byte sub) {
		byte* unzipped = new byte[LEVEL_SIZE];

		for (byte s = 0; s <= sub; s++) {
			if (s != sub) skip_map(offset);
			else unzip_map(offset, unzipped);
		}

		return unzipped;
	}


	void render_tile(Bitmap &bitmap, uint16_t *t, uint16_t tx, uint16_t ty) {
		render_tile(bitmap, t, tx * 8, ty * 8, false, false);
	}

	void render_tile(Bitmap &bitmap, uint16_t *t, int ox, int oy, bool flipx, bool flipy, bool trans) {
		if (ox+8 > bitmap.width || oy+8 > bitmap.height) return;
		for (int y = 0; y < 8; ++y) {
			for (int x = 0; x < 8; ++x) {
				uint16_t c = (*t >> x) & 0x0101;
				c = (c & 0x1) | (c>>7 & 0x2);

				if (!trans || c) bitmap.RGB[(flipx ? x : 7-x) + ((!flipy ? y : 7-y) + oy) * bitmap.width + ox] = palette[c];
			}
			++t;
		}
	}


	void render_tiles(Bitmap &bitmap, uint16_t* cache) {
		for (uint16_t y = 0; y < bitmap.height/8; ++y) for (uint16_t x = 0; x < bitmap.width/8; ++x) {
			render_tile(bitmap, cache + y * bitmap.width + x * 8, x, y);
		}
	}


	void copy_cache_tiles(uint16_t* cache, byte* source, int o, int n) {
		memcpy (cache + o * 8, source, 8 * n * 2);
	}

	void copy_block(Bitmap &cache, Bitmap &bitmap, int b, int x, int y) {
		for (int row = 0; row < 16; ++row) {
			memcpy ( bitmap.RGB + ( (y * 16 + row) * bitmap.width + x * 16), cache.RGB + (b * 16 * 16 + row * 16), 16*3);
		}
	}


	void render_blocks(Bitmap &bitmap, Bitmap &cache) {
		for (int y = 0; y < bitmap.height/16; ++y) for (int x = 0; x < bitmap.width/16; ++x) {
			copy_block(cache, bitmap, y * (bitmap.width/16) + x, x, y);
		}
	}


	void color_tint(RGB_DATA &color, RGB_DATA tint) {
		color.r = (color.r < tint.r) ? 0 : color.r - tint.r;
		color.g = (color.g < tint.g) ? 0 : color.g - tint.g;
		color.b = (color.b < tint.b) ? 0 : color.b - tint.b;
	}


	void guess_tint(Bitmap &cache) {
		RGB_DATA color;
		for (byte b = 0; b < 128; ++b) {
			color.r = (b & 3) * 24;
			color.g = ((b>>2) & 3) * 24;
			color.b = ((b>>4) & 3) * 24;

			color_cache_block(cache, b, color);
		}
	}


	void color_cache_block(Bitmap &cache, int b, RGB_DATA color) {
		for (int p = 0; p < 16*16; ++p) color_tint(cache.RGB[b*16*16+p], color);
	}


	void copy_cache_block(Bitmap &cache, int from, int to) {
		memcpy (cache.RGB + to * 16 * 16, cache.RGB + from * 16 * 16, 16*16*3);
	}


	void tint_block(Bitmap &bitmap, int bx, int by, byte color) {
		for (int y = 0; y < 2; ++y) for (int x = 0; x < 2; ++x) {
			tint_tile(bitmap, bx*16 + x*8, by*16 + y*8, color);
		}
	}


	void tint_tile(Bitmap &bitmap, int ox, int oy, byte color) {
		if (ox < 0 || oy < 0 || ox+8 > 256*16 || oy+8 > 48*16 ) return;

		for (int y = 0; y < 8; ++y) for (int x = 0; x < 8; ++x) {
			switch (color) {
				case 0: bitmap.RGB[(oy+y)*bitmap.width + ox + x].b /= 4; break;
				case 1: bitmap.RGB[(oy+y)*bitmap.width + ox + x].g /= 4; break;
				default: bitmap.RGB[(oy+y)*bitmap.width + ox + x].r /= 4;
			}
		}
	}

	void save_png(Bitmap &bitmap, string filename) {
        wxImage image = wxImage(bitmap.width, bitmap.height,  reinterpret_cast<byte*>(bitmap.RGB), true);
		image.SaveFile(filename, wxBITMAP_TYPE_PNG);
	}

	void dump_map() {
		Bitmap bitmap(256, 48, true);
		byte* unzipped = (game == 2 ? SML2::load_map() : SML3::load_map());

		for (int y = 0; y < bitmap.height; ++y) for (int x = 0; x < bitmap.width; ++x)  {
			byte b = unzipped[x + y*bitmap.width] * 2;
			bitmap.RGB[x + y * bitmap.width].b = b;
			bitmap.RGB[x + y * bitmap.width].g = b;
			bitmap.RGB[x + y * bitmap.width].r = b;
		}

		string filename = "output/map_" + to_string(CL) + ".png";
        save_png(bitmap, filename);

		delete[] unzipped;
	}


	void dump_level() {
		Bitmap bitmap(256*16, 48*16, true);

		if (game == 3) SML3::render_level(bitmap);
		else SML2::render_level(bitmap);

		string filename = "./output/full_" + to_string(CL) + ".png";
		save_png(bitmap, filename);
	}


	void dump_blocks() {
		uint16_t* tile_cache = new uint16_t[256*8]{};
        Bitmap tile_bitmap(16 * 8, 8 * 8, true);
		Bitmap bitmap(8 * 16, 16 * 16, true);
        Bitmap cache(16, 8*16*16, true);

		set_pal(SML_HEADER(pal_bg));

		if (game == 3) {
            SML3::cache_tiles(tile_cache, header, false);
            SML3::cache_blocks(cache, tile_cache, SML3_HEADER->blocks);
            SML3::special_blocks(cache);
		} else {
		    SML2::cache_tiles(tile_cache);
            SML2::cache_blocks(cache, tile_cache);
            SML2::special_blocks(cache);
		}

        render_tiles(tile_bitmap, tile_cache);
        render_blocks(bitmap, cache);

		string filename = "./output/blocks_" + to_string(CL) + ".png";
		string tile_filename = "./output/tiles_" + to_string(CL) + ".png";
		save_png(bitmap, filename);
		save_png(tile_bitmap, tile_filename);

		delete[] tile_cache;
	}


	uint16_t xy(int x, int y) { return y * LEVEL_WIDTH + x; }

	byte get_x(uint16_t p) { return p & 0xFF; }

    byte get_y(uint16_t p) {  return p >> 8; }

    byte get_sector(uint16_t p) { return (get_y(p) >> 4) * 16 + (get_x(p) >> 4); }


	void fix_CRC() {
		unsigned short int crc = 0;
		for (int i = 0x134; i < 0x14D; i++) crc = static_cast<unsigned int short>(crc - DATA[i] - 1);
		DATA[0x14D] = static_cast<byte>((crc & 0xFF));

		crc = 0;
		for (int j = 0; j < ROM_SIZE; j++)
			if (j != 0x14E && j != 0x14F) crc += DATA[j];

		DATA[0x14E] = static_cast<byte>((crc >> 8));
		DATA[0x14F] = static_cast<byte>((crc & 0xFF));
	}


	void expand_rom() {
		DATA[0x147] = 19; DATA[0x148] = 5;
	}


	byte* bank(byte bank) {
		return DATA + bank * BANK_SIZE;
	}


	//gameboy style address
	byte* address(byte bank, uint16_t addr) {
		//if (addr >= BANK_SIZE*2) cout << "bank overflow\n";

		if (addr >= BANK_SIZE) return DATA + addr + (bank-1) * BANK_SIZE;
		else return DATA + addr;
	}


	byte* deref(byte bank, uint16_t addr, uint16_t offset) {
		uint16_t *word = reinterpret_cast<uint16_t*>(address(bank, addr));
		word += offset;
		return address(bank, *word);
	}


	uint16_t swap_bytes (uint16_t i) {
		return (i & 0xFF) << 8 | (i & 0xFF00) >> 8;
	}
}
