#ifndef VGA_HEADERS
#define VGA_HEADERS

#define VGA_TEXT_MODE_STANDARD 80*25
#define VGA_TEXT_MODE_BEGIN (size_t)0xB8000
#define VGA_TEXT_MODE_COLS 80
#define VGA_TEXT_MODE_LINES 25

// ports
// Miscellaneous
#define VGA_MISC_READ 0x3CC
#define VGA_MISC_WRITE 0x3C2

/*
 * 0 IOAS (controls base direction of CRTC)
 * 1 RESERVED (Always 1)
 * 2 RAM_ENABLE (enable or disable RAM on video)
 * 3 CLKSEL (selects pixel clock)
 * 4 HPOL (¿?) idk actually
 * 5 VPOL (¿?) idk actually
 * 6 CSYNC (syncronization composed signal)
 * 7 VSYNC (syncronization vertical signal)
 */

// Sequencer Registers
#define VGA_SEQ_INDEX_PORT 0x3C4
#define VGA_SEQ_READ_WRITE_INDEX 0x3C5
/*
 * 0x00 Reset Registers
 * 0x01 Clocking Mode Register
 * 0x02 Map Mask Register
 * 0x03 Character Map Select
 * 0x04 Memory Mode Register
 */

// Atributte Controller Register
#define VGA_ACR_INDEX_WRITE_PORT 0x3C0
#define VGA_ACR_RESET 0x3DA
#define VGA_ACR_READ_INDEX 0x3C1
/*
 * 0x00 - 0x0F Palette Registers
 * 0x10 Mode Control Register
 * 0x11 Overscan Color Register
 * 0x12 Color Select Register
 * 0x13 Horizontal Pixel Pan
 * 0x14 Color Select Register
 */

// Graphic Controller Registers
#define VGA_GCR_INDEX_PORT 0x3CE
#define VGA_GCR_READ_WRITE_INDEX 0x3CF

/*
 * 0x00 Set/Reset Register
 * 0x01 Enable/Disable Set/Reset
 * 0x02 Color Compare Register
 * 0x03 Data Rotate Register
 * 0x04 Read Map Select Reg.
 * 0x05 Mode Register
 * 0x06 Miscellaneous Register
 * 0x07 Color don't Care register
 * 0x08 Bit Mask Register
 */

// CRT Controller

/*
 * to select between 0x3D0 and 0x3B0 set to 1 or 0 de IOAS bit from Miscellaneous
 */

#define VGA_CRTC_INDEX_BASE_A 0x3D0
#define VGA_CRTC_INDEX_BASE_B 0x3B0
#define VGA_CRTC_INDEX_PORT 4
#define VGA_CRTC_READ_WRITE 5

/*
 * 0x00 Horizontal Total
 * 0x01 Horizontal Display End
 * 0x02 Start Horizontal Blanking
 * 0x03 End Horizontal Blanking
 * 0x04 Start Horizontal Retrace
 * 0x05 End Horizontal Retrace
 * 0x06 Vertical Total
 * 0x07 Overflow Register
 * 0x08 Reset Row Scan
 * 0x09 Maximum Scan Line
 * 0x0A Cursor Start Register
 * 0x0B Cursor End Register
 * 0x0C Start Address High
 * 0x0D Start Address Low
 * 0x0E Cursor Location High
 * 0x0F Cursor Location Low
 * 0x10 Vertical Retrace Start
 * 0x11 Vertical Retrace End
 * 0x12 Vertical Display End
 * 0x13 Offset Register
 * 0x14 Underline Location
 * 0x15 Start Vertical Blanking
 * 0x16 End Vertical Blanking
 * 0x17 Mode Control Register
 * 0x18 Line Compare Register
 */

#endif
