#include "graphics/framebuffer.h"
#include "drivers/mbox.h"
#include "io/uart.h"
#include "homer.h"
#include "printf.h"

/* PC Screen Font as used by Linux Console */
typedef struct {
    u32 magic;
    u32 version;
    u32 headersize;
    u32 flags;
    u32 numglyph;
    u32 bytesperglyph;
    u32 height;
    u32 width;
    u8 glyphs;
} __attribute__((packed)) psf_t;
extern volatile u8 _binary_src_font_psf_start;

/* Scalable Screen Font */
typedef struct {
    u8    magic[4];
    u32   size;
    u8    type;
    u8    features;
    u8    width;
    u8    height;
    u8    baseline;
    u8    underline;
    u16   fragments_offs;
    u32   characters_offs;
    u32   ligature_offs;
    u32   kerning_offs;
    u32   cmap_offs;
} __attribute__((packed)) sfn_t;
extern volatile u8 _binary_src_font_sfn_start;


u32 _width, _height, _pitch, _isrgb;   /* dimensions and channel order */
u8 *fb;                         /* raw frame buffer address */

void fb_clear() {
    u8 *ptr = fb;
    for(u32 y = 0; y < _height; y++)
    {
        for(u32 x = 0; x < _width; x++)
        {
            *((u32 *)ptr) = 0x0;
            ptr += 4;
        }
        ptr += _pitch - _width * 4;
    }
}

u64 get_fb_address() {
    printf("Value of fb pointer is: 0x%X\n", fb);
    return (u64)fb;
}

void fb_init(u32 width, u32 height) {
    u32 _indices[7];

    //while (mbox_is_processing());// wait till processing

    mbox_property_init();
    _indices[0] = mbox_property_tag(MBOX_TAG_FB_SET_PHYS_WH, 2, width, height);
    _indices[1] = mbox_property_tag(MBOX_TAG_FB_SET_VIRT_WH, 2, width, height);
    _indices[2] = mbox_property_tag(MBOX_TAG_FB_SET_VIRT_OFFSET, 2, 0, 0);
    _indices[3] = mbox_property_tag(MBOX_TAG_FB_SET_DEPTH, 1, 32);
    _indices[4] = mbox_property_tag(MBOX_TAG_FB_SET_PX_ORDER, 1, 1);
    _indices[5] = mbox_property_tag(MBOX_TAG_FB_ALLOCATE, 2, 4096, 0);
    _indices[6] = mbox_property_tag(MBOX_TAG_FB_GET_PITCH, 1, 0);
    mbox_property_end();

    if(mbox_property_exec() && get_mbox(20)==32 && get_mbox(28)!=0) {
        set_mbox(_indices[5], get_mbox(_indices[5]) & 0x3FFFFFFF); //convert GPU address to ARM address
        _width=get_mbox(_indices[0]);          //get actual physical width
        _height=get_mbox(_indices[0]+1);         //get actual physical height
        _isrgb=get_mbox(_indices[4]);         //get the actual channel order
        _pitch=get_mbox(_indices[6]);         //get number of bytes per line
        fb=(void*)((u64)get_mbox(_indices[5]));
        printf("Screen resolution is now set to %dx%dx32\n", _width, _height);
    } else {
        printf("Unable to set screen resolution to %dx%dx32\n", width, height);
    }
}

void fb_showpicture() {
    int x, y;
    u8 *ptr = fb;
    char *data = homer_data, pixel[4];

    ptr += (_height - homer_height) / 2 * _pitch + (_width - homer_width) * 2;
    for (y = 0; y < homer_height; y++) {
        for (x = 0; x < homer_width; x++) {
            HEADER_PIXEL(data, pixel);

            *((u32 *)ptr) = _isrgb ? *((u32 *)&pixel) : (u32)(pixel[0] << 16 | pixel[1] << 8 | pixel[2]);
            ptr += 4;
        }
        ptr += _pitch - homer_width * 4;
    } 
}

void fb_print(int x, int y, char *s) {
    psf_t *font = (psf_t*)&_binary_src_font_psf_start;
    // draw next character if it's not zero
    while(*s) {
        // get the offset of the glyph. Need to adjust this to support unicode table
        unsigned char *glyph = (unsigned char*)&_binary_src_font_psf_start + font->headersize + (*((unsigned char*)s)<font->numglyph?*s:0)*font->bytesperglyph;
        // calculate the offset on screen
        int offs = (y * _pitch) + (x * 4);
        // variables
        int i,j, line,mask, bytesperline=(font->width+7)/8;
        // handle carrige return
        if(*s == '\r') {
            x = 0;
        } else
        // new line
        if(*s == '\n') {
            x = 0; y += font->height;
        } else {
            // display a character
            for(j=0;j<font->height;j++){
                // display one row
                line=offs;
                mask=1<<(font->width-1);
                for(i=0;i<font->width;i++){
                    // if bit set, we use white color, otherwise black
                    *((unsigned int*)(fb + line))=((int)*glyph) & mask?0xFFFFFF:0;
                    mask>>=1;
                    line+=4;
                }
                // adjust to next line
                glyph+=bytesperline;
                offs+=_pitch;
            }
            x += (font->width+1);
        }
        // next character
        s++;
    }
}

void fb_proprint(int x, int y, char *s) {
    // get our font
    sfn_t *font = (sfn_t*)&_binary_src_font_sfn_start;
    unsigned char *ptr, *chr, *frg;
    unsigned int c;
    unsigned long o, p;
    int i, j, k, l, m, n;

    while(*s) {
        // UTF-8 to UNICODE code point
        if((*s & 128) != 0) {
            if(!(*s & 32)) { c = ((*s & 0x1F)<<6)|(*(s+1) & 0x3F); s += 1; } else
            if(!(*s & 16)) { c = ((*s & 0xF)<<12)|((*(s+1) & 0x3F)<<6)|(*(s+2) & 0x3F); s += 2; } else
            if(!(*s & 8)) { c = ((*s & 0x7)<<18)|((*(s+1) & 0x3F)<<12)|((*(s+2) & 0x3F)<<6)|(*(s+3) & 0x3F); s += 3; }
            else c = 0;
        } else c = *s;
        s++;
        // handle carrige return
        if(c == '\r') {
            x = 0; continue;
        } else
        // new line
        if(c == '\n') {
            x = 0; y += font->height; continue;
        }
        // find glyph, look up "c" in Character Table
        for(ptr = (unsigned char*)font + font->characters_offs, chr = 0, i = 0; i < 0x110000; i++) {
            if(ptr[0] == 0xFF) { i += 65535; ptr++; }
            else if((ptr[0] & 0xC0) == 0xC0) { j = (((ptr[0] & 0x3F) << 8) | ptr[1]); i += j; ptr += 2; }
            else if((ptr[0] & 0xC0) == 0x80) { j = (ptr[0] & 0x3F); i += j; ptr++; }
            else { if((unsigned int)i == c) { chr = ptr; break; } ptr += 6 + ptr[1] * (ptr[0] & 0x40 ? 6 : 5); }
        }
        if(!chr) continue;
        // uncompress and display fragments
        ptr = chr + 6; o = (unsigned long)fb + y * _pitch + x * 4;
        for(i = n = 0; i < chr[1]; i++, ptr += chr[0] & 0x40 ? 6 : 5) {
            if(ptr[0] == 255 && ptr[1] == 255) continue;
            frg = (unsigned char*)font + (chr[0] & 0x40 ? ((ptr[5] << 24) | (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]) :
                ((ptr[4] << 16) | (ptr[3] << 8) | ptr[2]));
            if((frg[0] & 0xE0) != 0x80) continue;
            o += (int)(ptr[1] - n) * _pitch; n = ptr[1];
            k = ((frg[0] & 0x1F) + 1) << 3; j = frg[1] + 1; frg += 2;
            for(m = 1; j; j--, n++, o += _pitch)
                for(p = o, l = 0; l < k; l++, p += 4, m <<= 1) {
                    if(m > 0x80) { frg++; m = 1; }
                    if(*frg & m) *((unsigned int*)p) = 0xFFFFFF;
                }
        }
        // add advances
        x += chr[4]+1; y += chr[5];
    }
}

