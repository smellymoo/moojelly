#pragma once

#include <iostream>	// cout
#include <fstream>  // file io
#include <cstring>  // strcomp
#include <string>   // strings
#include <iomanip>	// cout.setfill

#include "common.h"

#define SML(A,B)        (ROM::game == 2 ? A : B)
#define SML2_HEADER     ((SML2::header_struct*)header)
#define SML3_HEADER     ((SML3::header_struct*)header)
#define SML_HEADER(X)   (game == 2 ? SML2_HEADER->X : SML3_HEADER->X)
#define HEADER(X)       (reinterpret_cast<header_struct*>(header))->X

namespace ROM {

    extern int   game;
    extern byte  rom_ver;
    extern int   CL;
    extern int   switch_state;

    extern byte* DATA;
    extern byte* header;


	bool load_rom(std::string filename);

    void set_level(int level);
    //void cache_tiles(uint16_t* cache);
	//void cache_blocks(Bitmap &cache, uint16_t* tile_cache);
	void special_blocks(Bitmap &cache);
	void render_level(Bitmap &level_bitmap);

	void set_pal(byte pal);

	void unzip_map(byte* &map_data, byte* &unzip);
	void skip_map(byte* &map_data);

	byte* load_map(byte* offset, byte sub);

	void render_tile(Bitmap &bitmap, uint16_t *t, uint16_t tx, uint16_t ty);
	void render_tile(Bitmap &bitmap, uint16_t *t, int ox, int oy, bool flipx, bool flipy, bool trans = false);
	void render_tiles(Bitmap &bitmap, uint16_t* cache);
    void render_blocks(Bitmap &bitmap, Bitmap &cache);

	void copy_cache_tiles(uint16_t* cache, byte* source, int o, int n);
	void copy_block(Bitmap &cache, Bitmap &bitmap, int b, int x, int y);

	void color_tint(RGB_DATA &color, RGB_DATA tint);
	void guess_tint(Bitmap &cache);
	void color_cache_block(Bitmap &cache, int b, RGB_DATA color);
	void copy_cache_block(Bitmap &cache, int from, int to);
	void tint_block(Bitmap &bitmap, int bx, int by, byte color = 0);
	void tint_tile(Bitmap &bitmap, int ox, int oy, byte color = 0);

	void save_png(Bitmap &bitmap, std::string filename);
	void dump_map();
	void dump_level();
	void dump_blocks();

    uint16_t xy(int x, int y);
	byte get_x(uint16_t p);
    byte get_y(uint16_t p);
    byte get_sector(uint16_t p);

	void fix_CRC();
	void expand_rom();
	byte* bank(byte bank);
	byte* address(byte bank, uint16_t addr);
	byte* deref(byte bank, uint16_t addr, uint16_t offset = 0);
	uint16_t swap_bytes (uint16_t i);
};
