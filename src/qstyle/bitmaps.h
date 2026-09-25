#ifndef __BITMAPS_H
#define __BITMAPS_H

/* Image bits processed by KPixmap2Bitmaps */
static const unsigned char radiooff_light_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x10, 0x00, 0x10,
    0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x08, 0x00, 0x08, 0x0c, 0x06,
    0xf0, 0x01};

static const unsigned char radiooff_gray_bits[] = {
    0xf0, 0x01, 0x0c, 0x06, 0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00};

static const unsigned char radiooff_dgray_bits[] = {
    0x00, 0x00, 0xf0, 0x01, 0x0c, 0x02, 0x04, 0x00, 0x02, 0x00, 0x02, 0x00,
    0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00};

static const unsigned char radiooff_center_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xf8, 0x03, 0xfc, 0x07, 0xfc, 0x07,
   0xfc, 0x07, 0xfc, 0x07, 0xfc, 0x07, 0xf8, 0x03, 0xf0, 0x01, 0x00, 0x00,
   0x00, 0x00};

static const unsigned char radiomask_bits[] = {
   0xf0, 0x01, 0xfc, 0x07, 0xfe, 0x0b, 0xfe, 0x0b, 0xff, 0x17, 0xff, 0x17,
   0xff, 0x17, 0xff, 0x17, 0xff, 0x17, 0xfe, 0x0b, 0xf2, 0x09, 0x0c, 0x06,
   0xf0, 0x01};

// Checkbox "checked" bitmap
static const unsigned char x_bits[] = {0x63, 0x77, 0x3e, 0x1c, 0x3e, 0x77, 0x63};
 
// Arrow bitmaps (NB: QCOORD is deprecated, use a qint32 here)
static const qint32 u_arrow[]={-1,-3, 0,-3, -2,-2, 1,-2, -3,-1, 2,-1, -4,0, 3,0, -4,1, 3,1};
static const qint32 d_arrow[]={-4,-2, 3,-2, -4,-1, 3,-1, -3,0, 2,0, -2,1, 1,1, -1,2, 0,2};
static const qint32 l_arrow[]={-3,-1, -3,0, -2,-2, -2,1, -1,-3, -1,2, 0,-4, 0,3, 1,-4, 1,3};
static const qint32 r_arrow[]={-2,-4, -2,3, -1,-4, -1,3, 0,-3, 0,2, 1,-2, 1,1, 2,-1, 2,0};

namespace B3 {
	const qint32 u_arrow[]={ 0,-2, 0,-2, -1,-1, 1,-1, -2,0, 2,0, -3,1, 3,1 };
	const qint32 d_arrow[]={ -3,-2, 3,-2, -2,-1, 2,-1, -1,0, 1,0, 0,1, 0,1 };
	const qint32 l_arrow[]={ 1,-3, 1,-3, 0,-2, 1,-2, -1,-1, 1,-1, -2,0, 1,0, -1,1, 1,1, 0,2, 1,2, 1,3, 1,3 };
	const qint32 r_arrow[]={ -2,-3, -2,-3, -2,-2, -1,-2, -2,-1, 0,-1, -2,0, 1,0, -2,1, 0,1, -2,2, -1,2, -2,3, -2,3 };
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(qint32)*2)

// Fix Qt's wacky image positions
static const char * const hc_minimize_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"...######...",
"...######...",
"............",
"............"}; 

static const char * const hc_close_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"..##....##..",
"...##..##...",
"....####....",
".....##.....",
"....####....",
"...##..##...",
"..##....##..",
"............",
"............"};

#endif
