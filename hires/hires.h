#ifndef __HIRES_H__
#define __HIRES_H__

// Configuration
#define ROWS                192 // number of scanlines
#define ROW_FIRST           0
#define ROW_LAST            ROWS - 1
#define ROW_SECOND_LAST     ROW_LAST - 1
#define COLUMNS             40            // Number of columns/bytes per row
#define COLUMN_FIRST        0
#define COLUMN_LAST         COLUMNS - 1
#define WHITE               0x7F           // White hires byte
#define BLACK               0x00
#define SPRITE_BUFFER_SIZE  45
#define HGR1SCRN            0x2000
#define HGR_SCRN_LEN        0x2000

#define TXTCLR              0xC050         // graphics mode
#define HIRES               0xC057         // hires mode

extern const uint8_t lklo[];
extern const uint8_t lkhi[];

void hires_clear(void);

#endif /* __HIRES_H__ */
