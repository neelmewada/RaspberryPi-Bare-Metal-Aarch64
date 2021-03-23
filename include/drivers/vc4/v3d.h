#pragma once

#include "common.h"
#include "graphics/v3d_defines.h"
#include "drivers/vc4/v3d_defines.h"

// LOW LEVEL V3D Code

static void emit_u8(u8 **list, u8 d);
static void emit_u16(u8 **list, u16 d);
static void emit_u32(u8 **list, u32 d);
static void emit_float(u8 **list, float f);


/*
* Allocates memory on GPU. Returns 0 on failure.
*/
GPU_HANDLE v3d_mem_alloc(u32 size, u32 align, V3D_MEMALLOC_FLAGS flags);


bool v3d_mem_free(GPU_HANDLE handle);

/*
* Locks the memory associated to the GPU handle to a GPU bus address.
*/
u32 v3d_mem_lock(GPU_HANDLE handle);

/*
* Unlocks the memory associated to the GPU handle from the GPU bus address.
*/
bool v3d_mem_unlock(GPU_HANDLE handle);


bool v3d_execute_code (u32 code, u32 r0, u32 r1, u32 r2, u32 r3, u32 r4, u32 r5);
bool v3d_execute_qpu (int32 num_qpus, u32 control, u32 noflush, u32 timeout);



/*
void _byte(u8 byte);
void _hword(u16 hword);
void _word(u32 word);
void _quad(u64 quad);


// Functions

void test(u32 fb_addr, u16 width, u16 height);


// MAIN Functions: Control lists, etc

void v3d_cl_start();
u32 v3d_cl_end();
void v3d_cl_align(u8 align_index);

// BINNING MODE

void v3d_cl_tile_binning_mode_config(u32 addr, u32 size, u32 base_addr, u8 width, u8 height, u8 data);
void v3d_cl_start_tile_binning();
void v3d_cl_clip_window(u16 left, u16 bottom, u16 width, u16 height);
void v3d_cl_config_bits(u8 data8, u16 data16);
void v3d_cl_viewport_offset(int16 x, int16 y);
void v3d_cl_nv_shader_state(u32 addr);
void v3d_cl_vert_array_primitives(u8 data, u32 length, u32 index);
void v3d_cl_flush();



// RENDERING MODE

void v3d_cl_clear_colors(u64 color, u32 clearzs, u32 clearvgmask, u8 clearstencil);
void v3d_cl_tile_rendering_mode_config(u32 addr, u16 width, u16 height, u16 data);
void v3d_cl_tile_coord(u8 column, u8 row);
void v3d_cl_store_tile_bfr_general(u16 data16, u32 data32, u32 addr);
void v3d_cl_branch_to_sublist(u32 addr);
void v3d_cl_store_multisample();
void v3d_cl_store_multisample_end();
*/






