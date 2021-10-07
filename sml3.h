#pragma once

#include <stack>

#define HEADER_SML3	    0x4560 //BANK12
#define WARP(H,TAG,W)   (W ? ((warp_struct*)H)->TAG : ((header_struct*)H)->TAG)

#define FILL(P, F, J)  if (!mark[P]) { todo.emplace(P, F, J); mark[P] = filler; }

namespace SML3 {
    struct __attribute__((packed)) header_struct {
        byte tile_bank;
        uint16_t tiles;
        uint16_t tiles2;
        uint16_t tiles_hud;
        uint16_t tiles_animated;
        byte map_bank;
        byte map_sub;
        uint16_t blocks;
        point16 player;
        byte unknown_b1;    //17 NOP
        byte player_flags;  // 8=invisible 10=inverted 20=facing 40=upsidedown 80=behind
        point16 viewport;
        byte focus_shift;
        byte camera_mode;   // 0=norm,1=train,10=floor,11=camera tracks, 20+=scrolling FF=fixed camera
        byte player_state;  // 00=stand, **=swim
        byte animated_tilemask;
        byte pal_bg;
        uint16_t enemies;
    };

    struct __attribute__((packed)) warp_struct {
        byte sector;
        byte player_y;
        byte player_x;
        byte unknown;
        point16 viewport;
        byte camera_mode;
        byte animated_tilemask;
        byte pal_bg;
        byte tile_bank;
        uint16_t tiles;
        uint16_t tiles2;
        uint16_t tiles_hud;
        uint16_t tiles_animated;
        uint16_t blocks;
        uint16_t enemies;
    };

    struct fill_todo {
        uint16_t p;
        byte flags;
        int jump;

        fill_todo (uint16_t pP, byte pFlags, byte pJump) : p(pP), flags(pFlags), jump(pJump) { }
    };

    enum fill_t { spike, solid, bounce, platform, smash, sea, follow, sky, door, };

    void cache_tiles(uint16_t* cache, byte* addr, bool warp);
    void cache_blocks(Bitmap &cache, uint16_t* tile_cache, uint16_t addr);
    void load_cache(Bitmap &cache, uint16_t* tile_cache, byte* header, bool warp);

	void special_blocks(Bitmap &cache);
	constexpr byte switch_block(byte block, byte mode);

	void render_level(Bitmap &level_bitmap);
	void render_map(Bitmap &bitmap, Bitmap &cache, byte* unzipped, int sx, int sy);

	uint16_t lookup_sprite_tilemap(uint16_t code);
	void lookup_sprites(uint16_t enemy_table, uint16_t &id, byte* &tile_table);
	void unzip_sprites(byte *data, std::vector<sprite> &sprites);
	void render_sprite(Bitmap &bitmap, uint16_t* tile_cache, uint16_t enemy_table, byte type, int tx, int ty);
	void render_sprites(Bitmap &bitmap, uint16_t* cache);
    void render_sprites(Bitmap &bitmap, uint16_t* cache, uint16_t enemy_table, std::vector<sprite> &sprites, int sx, int sy);

	void test_sprite_lookup();
    void init(byte bank, uint16_t run);
	constexpr fill_t fill_type(byte block);
    void trace_fill(byte* unzipped, byte* trace, byte* sector, uint16_t from, byte fill_val);
    void trace_fill_warps(byte* blockmap, byte* mark, byte* mark_sectors);
    bool fill_check_border(byte* blockmap, byte s, uint16_t p);
    void trace_debug(Bitmap &bitmap, byte* trace);

    uint16_t find_door(byte* unzipped, uint16_t s);
	warp_struct* get_warp(byte s);
	byte scroll_border(byte s);

    point16 decode_player(point16 player);

	byte* load_map();
};
