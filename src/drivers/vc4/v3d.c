#include "common.h"
#include "printf.h"
#include "utils.h"

#include "drivers/vc4/v3d.h"
#include "graphics/v3d.h"
#include "drivers/mbox.h"
#include "peripherals/vc4.h"


//-------------------------------------------
// High Level API

bool v3d_init() {
    if (mbox_is_processing())
        return false;
    
    mbox_property_init();
    mbox_property_tag(MBOX_TAG_SET_CLKRATE, 3, MBOX_CLOCK_ID_V3D, 250*1000*1000, 0);
    mbox_property_tag(MBOX_TAG_GPU_SET_ENABLEQPU, 1, 1);
    mbox_property_end();
    if (mbox_property_exec()) {
        if (*V3D_IDENT0 == 0x2443356) {
            printf("InitV3D Success\n");
            return true;
        }
    }
    printf("InitV3D Fail\n");
    return false;
}

bool v3d_initialize_scene(RENDER_STRUCT *scene, u32 render_width, u32 render_height) {
    if (scene) {
        scene->renderer_handle = v3d_mem_alloc(0x40000, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);
        if (!scene->renderer_handle) {
            printf("Init Scene failed at handle!\n");
            return false;
        }
        scene->renderer_data_vc4 = v3d_mem_lock(scene->renderer_handle);
        scene->loadpos = scene->renderer_data_vc4;   // VC4 load from start of memory

        scene->render_width = render_width;
        scene->render_height = render_height;
        scene->binning_width = (render_width + 63) / 64;
        scene->binning_height = (render_height + 63) / 64;

        scene->tile_mem_size = 0x40000;
        scene->tile_handle = v3d_mem_alloc(scene->tile_mem_size + 0x40000, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);
        scene->tile_state_data_vc4 = v3d_mem_lock(scene->tile_handle);
        scene->tile_data_buffer_vc4 = scene->tile_state_data_vc4 + 0x40000;

        scene->binning_handle = v3d_mem_alloc(0x40000, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);
        scene->binning_data_vc4 = v3d_mem_lock(scene->binning_handle);
        printf("Binning Handle addr(gpu): 0x%X\n", scene->binning_handle);
        printf("Binning Data addr(gpu): 0x%X\n", scene->binning_data_vc4);
        return true;
    }
    printf("Init Scene fail\n");
    return false;
}

bool v3d_add_vertices_to_scene(RENDER_STRUCT *scene) {
    if (scene) {
        scene->vertex_vc4 = (scene->loadpos + 127) & ALIGN_128BIT_MASK; // Hold vertex start address aligned to 128 bits
        u8 *p = (u8 *)(uintptr_t)gpu_to_arm_address(scene->vertex_vc4);
        u8 *q = p;

        u32 centre_x = scene->render_width / 2;
        u32 centre_y = (u32)(0.4f * (scene->render_height / 2));
        u32 half_shape_w = (u32)(0.4f * (scene->render_width / 2));
        u32 half_shape_h = (u32)(0.3f * (scene->render_height / 2));

        // Vertex Data

        // Vertex: Top, red
        emit_u16(&p, centre_x << 4);                   // X in 12.4 fixed point
        emit_u16(&p, (centre_y - half_shape_h) << 4);  // Y in 12.4 fixed point
        emit_float(&p, 1.0f);                          // Z
        emit_float(&p, 1.0f);                          // 1/W
        emit_float(&p, 1.0f);                          // Varying 0 R
        emit_float(&p, 0.0f);                          // Varying 1 G
        emit_float(&p, 0.0f);                          // Varying 2 B

        // Vertex: bottom left, blue
        emit_u16(&p, (centre_x - half_shape_w) << 4);  // X in 12.4 fixed point
        emit_u16(&p, (centre_y + half_shape_h) << 4);  // Y in 12.4 fixed point
        emit_float(&p, 1.0f);                          // Z
        emit_float(&p, 1.0f);                          // 1/W
        emit_float(&p, 0.0f);                          // Varying 0 R
        emit_float(&p, 0.0f);                          // Varying 1 G
        emit_float(&p, 1.0f);                          // Varying 2 B

        // Vertex: bottom right, green
        emit_u16(&p, (centre_x + half_shape_w) << 4);  // X in 12.4 fixed point
        emit_u16(&p, (centre_y + half_shape_h) << 4);  // Y in 12.4 fixed point
        emit_float(&p, 1.0f);                          // Z
        emit_float(&p, 1.0f);                          // 1/W
        emit_float(&p, 0.0f);                          // Varying 0 R
        emit_float(&p, 1.0f);                          // Varying 1 G
        emit_float(&p, 0.0f);                          // Varying 2 B

        scene->vertex_count = 3;
        scene->loadpos = scene->vertex_vc4 + (p - q);

        scene->index_vertex_vc4 = (scene->loadpos + 127) & ALIGN_128BIT_MASK;
        p = (u8 *)(uintptr_t)gpu_to_arm_address(scene->index_vertex_vc4);
        q = p;

        emit_u8(&p, 0); // tri - top
        emit_u8(&p, 1); // tri - bottom left
        emit_u8(&p, 2); // tri - bottom right

        scene->index_vertex_count = 3;
        scene->max_index_vertex = 2;

        scene->loadpos = scene->index_vertex_vc4 + (p - q); // update loadpos to new position
        printf("Vert Add success\n");
        return true;
    }
    //printf("Vert Add fail\n");
    return false;
}

bool v3d_add_shader_to_scene(RENDER_STRUCT *scene, u32 *frag_shader, u32 frag_shader_emits) {
    if (scene) {
        scene->shader_start = (scene->loadpos + 127) & ALIGN_128BIT_MASK;
        u8 *p = (u8 *)(uintptr_t)gpu_to_arm_address(scene->shader_start);
        u8 *q = p;

        for (u32 i = 0; i < frag_shader_emits; i++) {
            emit_u32(&p, frag_shader[i]);
        }

        scene->loadpos = scene->shader_start + (p - q);

        scene->frag_shader_rec_start = (scene->loadpos + 127) & ALIGN_128BIT_MASK;
        p = (u8 *)(uintptr_t)gpu_to_arm_address(scene->frag_shader_rec_start);
        q = p;

        // add shader record to buffer
        emit_u8(&p, 0x01);   // flags
        emit_u8(&p, 6 * 4);  // stride
        emit_u8(&p, 0xCC);   // num uniforms (not used)
        emit_u8(&p, 3);      // num varyings
        emit_u32(&p, scene->shader_start); // address of shader start location
        emit_u32(&p, 0);
        emit_u32(&p, scene->vertex_vc4); // address of vertices list

        scene->loadpos = scene->frag_shader_rec_start + (p - q);
        //printf("Shader Add success\n");
        return true;
    }
    printf("Shader Add fail\n");
    return false;
}

bool v3d_setup_binning_config(RENDER_STRUCT *scene) {
    if (scene) {
        u8 *p = (u8 *)(uintptr_t)gpu_to_arm_address(scene->binning_data_vc4);
        u8 *list = p;

        emit_u8(&p, GL_TILE_BINNING_CONFIG);
        emit_u32(&p, scene->tile_data_buffer_vc4);
        emit_u32(&p, scene->tile_mem_size);
        emit_u32(&p, scene->tile_state_data_vc4);
        emit_u8(&p, scene->binning_width);
        emit_u8(&p, scene->binning_height);
        emit_u8(&p, 0x04);

        //Start tile binning
        emit_u8(&p, GL_START_TILE_BINNING);

        emit_u8(&p, GL_PRIMITIVE_LIST_FORMAT);
        emit_u8(&p, 0x32); // 16-bit triangle

        emit_u8(&p, GL_CLIP_WINDOW);
        emit_u16(&p, 0);
        emit_u16(&p, 0);
        emit_u16(&p, scene->render_width);
        emit_u16(&p, scene->render_height);

        emit_u8(&p, GL_CONFIG_STATE);
        emit_u8(&p, 0x03);    // enable double sided polygons
        emit_u8(&p, 0x00);    // depth testing disabled
        emit_u8(&p, 0x02);    // enable early depth write

        emit_u8(&p, GL_VIEWPORT_OFFSET);
        emit_u16(&p, 0);
        emit_u16(&p, 0);

        emit_u8(&p, GL_NV_SHADER_STATE);
        emit_u32(&p, scene->frag_shader_rec_start);  // frag shader record address

        // primitive index list
        emit_u8(&p, GL_INDEXED_PRIMITIVE_LIST);
        emit_u8(&p, PRIM_TRIANGLE);              // 8-bit index, triangles
        emit_u32(&p, scene->index_vertex_count); // number of indices
        emit_u32(&p, scene->index_vertex_vc4);   // address of index data
        emit_u32(&p, scene->max_index_vertex);   // max index value

        // End of Bin List
        // Flush
        emit_u8(&p, GL_FLUSH_ALL_STATE);
        emit_u8(&p, GL_NOP);
        emit_u8(&p, GL_NOP);
        scene->loadpos = scene->binning_data_vc4 + (p - list) + 2;
        scene->binning_cfg_end = scene->binning_data_vc4 + (p - list);
        emit_u8(&p, GL_NOP);
        emit_u8(&p, GL_NOP);
        //printf("Binning setup success\n");
        return true;
    }
    printf("Binning setup fail\n");
    return false;
}

bool v3d_setup_render_config(RENDER_STRUCT *scene, VC4_ADDR render_buffer_addr) {
    if (scene) {
        printf("Framebuffer addr(GPU): 0x%X\n", render_buffer_addr);
        scene->render_control_vc4 = (scene->loadpos + 127) & ALIGN_128BIT_MASK;
        u8 *p = (u8 *)(uintptr_t)gpu_to_arm_address(scene->render_control_vc4);
        u8 *q = p;

        emit_u8(&p, GL_CLEAR_COLORS);
        emit_u32(&p, 0xFF000000);
        emit_u32(&p, 0xFF000000);
        emit_u32(&p, 0);

        emit_u8(&p, GL_TILE_RENDER_CONFIG);
        emit_u32(&p, render_buffer_addr);  // framebuffer address
        emit_u16(&p, scene->render_width);
        emit_u16(&p, scene->render_height);
        emit_u8(&p, 0x04);
        emit_u8(&p, 0x00);

        emit_u8(&p, GL_TILE_COORDINATES);
        emit_u8(&p, 0);
        emit_u8(&p, 0);

        // store buffer general
        emit_u8(&p, GL_STORE_TILE_BUFFER);
        emit_u16(&p, 0);  // store nothing, just clear
        emit_u32(&p, 0);  // no address needed

        // Link all binned lists together
        for (int x = 0; x < scene->binning_width; x++) {
            for (int y = 0; y < scene->binning_height; y++) {
                emit_u8(&p, GL_TILE_COORDINATES);
                emit_u8(&p, x);
                emit_u8(&p, y);

                // call tile sublist
                emit_u8(&p, GL_BRANCH_TO_SUBLIST);
                emit_u32(&p, scene->tile_data_buffer_vc4 + (y * scene->binning_width + x) * 32);

                if (x == (scene->binning_width - 1) && y == (scene->binning_height - 1)) {
                    emit_u8(&p, GL_STORE_MULTISAMPLE_END);
                } else {
                    emit_u8(&p, GL_STORE_MULTISAMPLE);
                }
            }
        }
        scene->loadpos = scene->render_control_vc4 + (p - q);
        scene->render_control_end_vc4 = scene->render_control_vc4 + (p - q);
        printf("Render setup success\n");
        return true;
    }
    printf("Render setup fail\n");
    return false;
}

void v3d_render_scene(RENDER_STRUCT *scene) {
    if (scene) {
        // clear caches
        *V3D_L2CACTL = 4;
        *V3D_SLCACTL = 0x0F0F0F0F;

        // stop the binning thread
        *V3D_CT0CS = 0x20;
        // wait for it to stop
        while (*V3D_CT0CS & 0x20);
        printf("Binning thread stopped!\n");        
        // run our binning control list
        *V3D_BFC = 1;
        *V3D_CT0CA = scene->binning_data_vc4;
        *V3D_CT0EA = scene->binning_cfg_end;

        printf("\nWaiting for Binning to Finish!\n");
        // wait for binning to finish
        while (*V3D_BFC == 0);
        printf("V3D_CT0CS Value is: 0x%X\n", *V3D_CT0CS);
        printf("Binning Done! V3D_BFC = 0x%X\n", *V3D_BFC);        

        // stop the rendering thread
        *V3D_CT1CS = 0x20;
        // wait for it to stop
        while (*V3D_CT1CS & 0x20);
        printf("Render thread stopped!\n");
        printf("V3D_CT1CS Value is: 0x%X\n", *V3D_CT1CS);

        // run our render control list
        *V3D_RFC = 1;
        *V3D_CT1CA = scene->render_control_vc4;
        *V3D_CT1EA = scene->render_control_end_vc4;

        *V3D_CT1CS = 0x10; // clear halt state

        printf("Waiting for Rendering to finish!\n");
        printf("V3D_CT1CS Value is: 0x%X\n", *V3D_CT1CS);
        printf("V3D_CT1CA Value is: 0x%X\n", *V3D_CT1CA);

        delay_micros(1 * 1000000);
        printf("V3D_CT1CS Value is: 0x%X\n", *V3D_CT1CS);
        printf("V3D_CT1CA Value is: 0x%X\n", *V3D_CT1CA);
        printf("V3D_CT1EA Value is: 0x%X\n", *V3D_CT1EA);
        printf("Value stored at (gpu addr)0x%X is: 0x%X\n", *V3D_CT1CA, *(u8 *)(uintptr_t)(gpu_to_arm_address(*V3D_CT1CA)));

        printf("Error Status: 0x%X\n", *V3D_ERRSTAT);
        printf("V3D_FDBGS: 0x%X\n", *V3D_FDBGS);
        printf("V3D_INTCTL: 0x%X\n", *V3D_INTCTL);

        // wait for render to finish
        while (*V3D_RFC == 0);

        printf("Rendering finished!\n");
    }
}


u32 arm_to_gpu_address(u32 arm_addr) {
    return arm_addr  |  GPU_RAM_START_ADDR;
}

u32 gpu_to_arm_address(u32 gpu_addr) {
    return gpu_addr & (~GPU_RAM_START_ADDR);
}


//-------------------------------------------
// Low Level Driver Code

struct __attribute__((__packed__, aligned(1))) EMITDATA {
	u8 byte1;
	u8 byte2;
	u8 byte3;
	u8 byte4;
};

static void emit_u8(u8 **list, u8 d) {
    *((*list)++) = d;
}

static void emit_u16(u8 **list, u16 d) {
    struct EMITDATA *data = (struct EMITDATA *)&d;
    *((*list)++) = data->byte1;
    *((*list)++) = data->byte2;
}

static void emit_u32(u8 **list, u32 d) {
    struct EMITDATA *data = (struct EMITDATA *)&d;
    *((*list)++) = data->byte1;
    *((*list)++) = data->byte2;
    *((*list)++) = data->byte3;
    *((*list)++) = data->byte4;
}

static void emit_float(u8 **list, float f) {
    struct EMITDATA *data = (struct EMITDATA *)&f;
    *((*list)++) = (*data).byte1;
    *((*list)++) = (*data).byte2;
    *((*list)++) = (*data).byte3;
    *((*list)++) = (*data).byte4;
}

GPU_HANDLE v3d_mem_alloc(u32 size, u32 align, V3D_MEMALLOC_FLAGS flags) {
    mbox_property_init();
    u8 i = mbox_property_tag(MBOX_TAG_GPU_ALLOC_MEMORY, 3, size, align, flags);
    mbox_property_end();

    if (mbox_property_exec()) {
        return get_mbox(i);
    }
    printf("FAILED to allocate v3d memory!");
    return 0;
}

bool v3d_mem_free(GPU_HANDLE handle) {
    mbox_property_init();
    u8 i = mbox_property_tag(MBOX_TAG_GPU_RELEASE_MEMORY, 1, handle);
    mbox_property_end();

    if (mbox_property_exec()) {
        if (get_mbox(i) == 0) return true;
    }
    return false;
}

u32 v3d_mem_lock(GPU_HANDLE handle) {
    mbox_property_init();
    u8 i = mbox_property_tag(MBOX_TAG_GPU_LOCK_MEMORY, 1, handle);
    mbox_property_end();

    if (mbox_property_exec()) {
        return get_mbox(i);
    }
    printf("FAILED to lock v3d memory!");
    return 0;
}

bool v3d_mem_unlock(GPU_HANDLE handle) {
    mbox_property_init();
    u8 i = mbox_property_tag(MBOX_TAG_GPU_UNLOCK_MEMORY, 1, handle);
    mbox_property_end();

    if (mbox_property_exec()) {
        if (get_mbox(i) == 0) return true;
    }
    return false;
}

bool v3d_execute_code (u32 code, u32 r0, u32 r1, u32 r2, u32 r3, u32 r4, u32 r5) {
    mbox_property_init();
    mbox_property_tag(MBOX_TAG_GPU_EXEC_CODE, 7, code, r0, r1, r2, r3, r4, r5);
    mbox_property_end();

    return mbox_property_exec();
}

bool v3d_execute_qpu (int32 num_qpus, u32 control, u32 noflush, u32 timeout) {
    mbox_property_init();
    mbox_property_tag(MBOX_TAG_GPU_EXEC_QPU_CODE, 4, num_qpus, control, noflush, timeout);
    mbox_property_end();

    return mbox_property_exec();
}



