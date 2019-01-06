#ifndef __MV_INCLUDE_H
#define __MV_INCLUDE_H
typedef enum image_bpp
{
	IMAGE_BPP_BINARY,       // BPP = 0
	IMAGE_BPP_GRAYSCALE,    // BPP = 1
	IMAGE_BPP_RGB565,       // BPP = 2
	IMAGE_BPP_BAYER,        // BPP = 3
	IMAGE_BPP_JPEG          // BPP > 3
}image_bpp_t;
#include "MV_Core/MV_Image.h"
#include "MV_Support/MV_FrameBuffer.h"


#endif
