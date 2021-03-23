#pragma once

#include "graphics/v3d_defines.h"

// High Level V3D Code (API)

bool v3d_init(void);

bool v3d_initialize_scene(RENDER_STRUCT *scene, u32 render_width, u32 render_height);

bool v3d_add_vertices_to_scene(RENDER_STRUCT *scene);

bool v3d_add_shader_to_scene(RENDER_STRUCT *scene, u32 *frag_shader, u32 frag_shader_emits);

bool v3d_setup_binning_config(RENDER_STRUCT *scene);

bool v3d_setup_render_config(RENDER_STRUCT *scene, VC4_ADDR render_buffer_addr);

void v3d_render_scene(RENDER_STRUCT *scene);


// Helper functions

u32 arm_to_gpu_address(u32 arm_addr);

u32 gpu_to_arm_address(u32 gpu_addr);

