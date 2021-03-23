#pragma once

#include "common.h"

// ARM & GPU have different address space
typedef u32 GPU_HANDLE;
typedef u32 VC4_ADDR;

// Render Structure
typedef struct render_t
{
	VC4_ADDR loadpos;

    GPU_HANDLE renderer_handle;  // renderer memory handle
    VC4_ADDR renderer_data_vc4;  // renderer data VC4 locked address

    u16 render_width;
    u16 render_height;

    VC4_ADDR shader_start;
    VC4_ADDR frag_shader_rec_start;

    u32 binning_width;
    u32 binning_height;

    VC4_ADDR render_control_vc4;
    VC4_ADDR render_control_end_vc4;

    VC4_ADDR vertex_vc4;
    u32 vertex_count;

    VC4_ADDR index_vertex_vc4;
    u32 index_vertex_count;
    u32 max_index_vertex;

    // Tile Data Memory. Has to be 4K align
    GPU_HANDLE tile_handle;
    u32 tile_mem_size;
    VC4_ADDR tile_state_data_vc4;
    VC4_ADDR tile_data_buffer_vc4;

    GPU_HANDLE binning_handle;
    VC4_ADDR binning_data_vc4;
    VC4_ADDR binning_cfg_end;

} RENDER_STRUCT;

