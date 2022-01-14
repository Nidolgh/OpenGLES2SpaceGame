#pragma once

////////////////////// *** TYPEDEFS *** //////////////////////

typedef unsigned char byte;

// This stores a skin name
typedef char tMd2Skin[64];

////////////////////// *** DEFINES *** //////////////////////

// These are the needed defines for the max values when loading .MD2 files
#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

// This stores the speed of the animation between each key frame
#define kAnimationSpeed			5.0f
