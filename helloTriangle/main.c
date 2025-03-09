/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
*/
#include <kernel.h>
#include <gs_psm.h>
#include <stdio.h>
#include <graph.h>
#include <draw.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>
#include <stdint.h>

// This is from the tutorial by "Tom Marks Talks Code": https://www.youtube.com/watch?v=1RYoi7yh8iU
// It is adapted for Packet2.

/** Some initialization of GS and VRAM allocation */
void init_gs(framebuffer_t *t_frame, zbuffer_t *t_z, texbuffer_t *t_texbuff)
{
	// Define a 32-bit 640x512 framebuffer.
	t_frame->width = 640;
	t_frame->height = 512;
	t_frame->mask = 0;
	t_frame->psm = GS_PSM_32;
	t_frame->address = graph_vram_allocate(t_frame->width, t_frame->height, t_frame->psm, GRAPH_ALIGN_PAGE);

	// Enable the zbuffer.
	t_z->enable = DRAW_ENABLE;
	t_z->mask = 0;
	t_z->method = ZTEST_METHOD_GREATER_EQUAL;
	t_z->zsm = GS_ZBUF_32;
	t_z->address = graph_vram_allocate(t_frame->width, t_frame->height, t_z->zsm, GRAPH_ALIGN_PAGE);

	// Allocate some vram for the texture buffer
	t_texbuff->width = 128;
	t_texbuff->psm = GS_PSM_24;
	t_texbuff->address = graph_vram_allocate(128, 128, GS_PSM_24, GRAPH_ALIGN_BLOCK);

	// Initialize the screen and tie the first framebuffer to the read circuits.
	graph_initialize(t_frame->address, t_frame->width, t_frame->height, t_frame->psm, 0, 0);
}

/** Some initialization of GS 2 */
void init_drawing_environment(framebuffer_t *t_frame, zbuffer_t *t_z)
{
    printf("Init draw environment!\n");
    
	packet2_t *packet2 = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// This will setup a default drawing environment.
	packet2_update(packet2, draw_setup_environment(packet2->next, 0, t_frame, t_z));

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	packet2_update(packet2, draw_primitive_xyoffset(packet2->next, 0, (2048 - 320), (2048 - 244)));

	// Finish setting up the environment.
	packet2_update(packet2, draw_finish(packet2->next));
    
	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();
	//draw_wait_finish();

	packet2_free(packet2);
}

#define SHIFT_AS_I64(x, b) (((int64_t)x)<<b)

// Normal offset
#define OFFSET 2048.0f

#define OFFSET_X 2048
#define OFFSET_Y 2048

#define myftoi4(x) ((x)<<4)

#define VID_W 640
#define VID_H 448

#define DRAW_TRIANGLE_NREG 6
#define DRAW_TRIANGLE_REGLIST \
		((u64)GIF_REG_RGBAQ)  <<  0 | \
		((u64)GIF_REG_XYZ2) <<  4 | \
		((u64)GIF_REG_RGBAQ)  <<  8 | \
		((u64)GIF_REG_XYZ2)  << 12 | \
		((u64)GIF_REG_RGBAQ)  << 16 | \
		((u64)GIF_REG_XYZ2)   << 20

static int tri[] = {
	10 + 320, 10 + 50, 0,
	30 + 320, 10 + 50, 0,
	20 + 320, 0 + 20, 0,
 };

void drawTriangle()
{
	uint64_t red = 0xff;
	uint64_t blue = 0x00;
	uint64_t green = 0x00;

	packet2_t* packet2 = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	qword_t qword;

	qword.dw[0] = GIF_SET_TAG(1,0,0,0,GIF_FLG_PACKED,1);
	qword.dw[1] = GIF_REG_AD; // A+D refers to the GS' general purpose registers.
	printf("%016llx-%016llx\n", qword.dw[0], qword.dw[1]);
	packet2_add_u128(packet2, qword.qw);

	qword.dw[0] = GIF_SET_PRIM(PRIM_TRIANGLE,0,0,0,0,0,0,0,0);
	qword.dw[1] = GS_REG_PRIM;
	printf("%016llx-%016llx\n", qword.dw[0], qword.dw[1]);
	packet2_add_u128(packet2, qword.qw);

	qword.dw[0] = GIF_SET_TAG(1,1,0,0,GIF_FLG_PACKED,DRAW_TRIANGLE_NREG);
	qword.dw[1] = DRAW_TRIANGLE_REGLIST;
	printf("%016llx-%016llx\n", qword.dw[0], qword.dw[1]);
	packet2_add_u128(packet2, qword.qw);

	int cx = myftoi4(2048 - (VID_W/2));
	int cy = myftoi4(2048 - (VID_H/2));

	for(int i = 0; i < 3; i++) {

    	qword.dw[0] = (red&0xff) | (green&0xff)<<32;
    	qword.dw[1] = (blue&0xff) | (SHIFT_AS_I64(0x80, 32));
    	packet2_add_u128(packet2, qword.qw);
    
		// 0xa -> 0xa0
		// fixed point format - xxxx xxxx xxxx.yyyy
		int base = i*3;
		int x = myftoi4(tri[base+0]) + cx;
		int y = myftoi4(tri[base+1]) + cy;
		int z = 0;
		qword.dw[0] = x | SHIFT_AS_I64(y, 32);
		qword.dw[1] = z;
		// printf("drawing vertex %x %x %x\n", tri[base+0], tri[base+1], tri[base+2]);
		packet2_add_u128(packet2, qword.qw);
  	}
	
	packet2_update(packet2, draw_finish(packet2->next));

	printf("packet2 qwords: %d\n", packet2->next - packet2->base);

	dma_channel_wait(DMA_CHANNEL_GIF, 0);

	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
  	dma_channel_wait(DMA_CHANNEL_GIF, 0);

  	packet2_free(packet2);
}

int main()
{
    printf("Hello world triangle!\n");
    
    // Init DMA channels.
	dma_channel_initialize(DMA_CHANNEL_GIF,NULL,0);	
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

    // The buffers to be used.
	framebuffer_t frame;
	zbuffer_t z;
	texbuffer_t texbuff;

	// Init the GS, framebuffer, zbuffer, and texture buffer.
	init_gs(&frame, &z, &texbuff);
    
    // Init the drawing environment and framebuffer.
	init_drawing_environment(&frame, &z);
       
    printf("Graphics module started! :D\n");

	printf("Draw triangle! :D\n");

	drawTriangle();
    
    SleepThread();

    return 0;
}
