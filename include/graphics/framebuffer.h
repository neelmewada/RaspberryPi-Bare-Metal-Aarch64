#pragma once

#include "common.h"

void fb_init(u32 width, u32 height);

void fb_showpicture();
void fb_print(int x, int y, char *s);
void fb_proprint(int x, int y, char *s);

void fb_clear();
u64 get_fb_address();
