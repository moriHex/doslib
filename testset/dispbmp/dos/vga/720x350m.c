
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

/* VGA 720x350 16-color mode with tweaks to make it a 1bpp mode.
 * Standard INT 13h mode without any tweaks. */

static const char bmpfile[] = "720350mc.bmp";
static const char bmpfile2[] = "720350m.bmp";
static const unsigned int img_width = 720;
static const unsigned int img_height = 350;
static const unsigned int img_stride = 720 / 8;

#include "dr_1bp.h"
#include "dr_aci.h"

/* CRTC mode parameters */
static const uint16_t crtcchg[] = {
	0x0011,		/* vertical retrace end 0x11: turn off protect */

	0x6B00,		/* horizontal total */
	0x5901,		/* end horizontal display */
	0x5A02,		/* start horizontal blanking */
	0x8E03,		/* end horizontal blanking */
	0x5E04,		/* start horiztonal retrace */
	0x8A05,		/* end horiztonal retrace */

	0x2D13,		/* offset register */
	0x8511		/* vertical retrace end 0x11 which also sets protect, which is why this is the last write */
};

static void display(struct BMPFILEREAD *bfr) {
	unsigned int dispw,i;

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
}

static struct BMPFILEREAD *load(const char *path) {
	struct BMPFILEREAD *bfr;

	bfr = open_bmp(path);
	if (bfr == NULL) {
		fprintf(stderr,"Failed to open BMP, errno %s\n",strerror(errno));
		close_bmp(&bfr);
		return NULL;
	}
	if (bfr->bpp != 1 || bfr->colors == 0 || bfr->colors > 2 || bfr->palette == 0) {
		fprintf(stderr,"BMP wrong format\n");
		close_bmp(&bfr);
		return NULL;
	}

	return bfr;
}

int main() {
	struct BMPFILEREAD *bfr;
	uint16_t iobase,i;

	if ((bfr=load(bmpfile)) == NULL)
		return 1;

	/* set 640x350x16 EGA, we're going to tweak it into a 1bpp mode */
	__asm {
		mov	ax,0x0010	; AH=0x00 AL=0x10
		int	0x10
	}
	/* read 0x3CC to determine color vs mono */
	iobase = (inp(0x3CC) & 1) ? 0x3D0 : 0x3B0;

	outp(0x3C2,0x67); /* misc control select 28MHz clock */

	/* CRTC mode set */
	for (i=0;i < (sizeof(crtcchg)/sizeof(crtcchg[0]));i++)
		outpw(iobase+4,crtcchg[i]);

	/* VGA may make this mode the same as 16-color but with a mono palette.
	 * To get what we want, some additional programming is needed. */
	outpw(0x3C4,0x0102); /* write only bitplane 0 (map mask) */
	outpw(0x3CE,0x0005); /* write mode 0 (read mode 0) */
	outpw(0x3CE,0xFF08); /* write all bits */

	/* enable only the first bitplane */
	inp(0x3DA);
	outp(0x3C0,0x12);
	outp(0x3C0,0x01);
	inp(0x3DA);
	outp(0x3C0,0x20); /* reenable the display */

	/* make sure AC palette is identity mapping to eliminate EGA-like default. */
	vga_ac_identity_map();

	display(bfr);
	close_bmp(&bfr);

	/* wait for ENTER */
	while (getch() != 13);

	/////

	if ((bfr=load(bmpfile2)) == NULL)
		return 1;

	display(bfr);
	close_bmp(&bfr);

	/////

	/* wait for ENTER */
	while (getch() != 13);

	/* restore text */
	__asm {
		mov	ax,0x0003	; AH=0x00 AL=0x03
		int	0x10
	}
	return 0;
}

