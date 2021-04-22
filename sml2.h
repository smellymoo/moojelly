#pragma once

#define HEADER_SML2     0x55CB

#define HEADER_TILES	(ROM::rom_ver == 2 ? 0x36AD : 0x36AA)
#define SPRITES_TILES	(ROM::rom_ver == 2 ? 0x362D : 0x362A)

namespace SML2 {
    struct __attribute__((packed)) header_struct {
        point16 player, viewport;
        byte unknown1; //focus_shift 00/80
        byte unknown2; //castle?
        byte level;
        uint16_t blocks;
        byte map_bank;
        byte music;
        byte pal_bg; byte pal_sprite1; byte pal_sprite2;
        byte map_sub;
        byte time; // Multiply by 0x64
    };

    struct __attribute__((packed)) header2_struct { //map tiles
        byte bank;
        byte unknown;
        uint16_t addr;
    };


    void cache_tiles(uint16_t* cache);
	void cache_blocks(Bitmap &cache, uint16_t* tile_cache);

	void special_blocks(Bitmap &cache);
	void render_level(Bitmap &level_bitmap);

	void unpack_sprites(byte *data, byte** store);
	void unzip_sprites(byte *data, std::vector<sprite> &sprites);
	void render_sprite(Bitmap &bitmap, uint16_t* tile_cache, byte type, int tx, int ty);
	void render_sprites(Bitmap &bitmap, uint16_t* cache);

    byte* load_map();
};
