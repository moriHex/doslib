
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <conio.h>
#include <stdio.h>
#include <errno.h>
#include <i86.h>

#include "libbmp.h"

#define DSCOLOR
#define PVGA1A

static const char bmpfile[] = "800600_4.bmp";
static const unsigned int img_width = 800;
static const unsigned int img_height = 600;
static const unsigned int img_stride = 800 / 8;

#include "dr_bnk4.h"
#include "dr_aci.h"
#include "detect.h"

static draw_scanline_func_t draw_scanline;

int main() {
	struct BMPFILEREAD *bfr;
	unsigned int dispw,i;

	draw_scanline = draw_scanline_pvga1a;

	bfr = open_bmp(bmpfile);
	if (bfr == NULL) {
		fprintf(stderr,"Failed to open BMP, errno %s\n",strerror(errno));
		return 1;
	}
	if (bfr->bpp != 4 || bfr->colors == 0 || bfr->colors > 16 || bfr->palette == 0) {
		fprintf(stderr,"BMP wrong format\n");
		return 1;
	}

	__asm {
		mov	ax,0x0058	; AH=0x00 AL=0x58
		int	0x10
	}
	pvga1a_unlock();

	/* AC needs to be an identity map to make palette programming easier */
	vga_ac_identity_map();

	/* set palette */
	outp(0x3C8,0);
	for (i=0;i < bfr->colors;i++) {
		outp(0x3C9,bfr->palette[i].rgbRed >> 2);
		outp(0x3C9,bfr->palette[i].rgbGreen >> 2);
		outp(0x3C9,bfr->palette[i].rgbBlue >> 2);
	}

	/* load and render */
	dispw = bfr->width;
	if (dispw > img_width) dispw = img_width;
	while (read_bmp_line(bfr) == 0)
		draw_scanline((unsigned int)bfr->current_line,bfr->scanline,dispw);

	/* done reading */
	close_bmp(&bfr);

	/* wait for ENTER */
	while (getch() != 13);

	/* restore text */
	__asm {
		mov	ax,0x0003	; AH=0x00 AL=0x03
		int	0x10
	}
	return 0;
}

