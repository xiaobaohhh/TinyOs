#ifndef KBD_H
#define KBD_H

#include "cpu/irq.h"

#define KEY_RSHIFT  0x36
#define KEY_LSHIFT  0x2a

#define KEY_CAPS    0x3a
#define KEY_CTRL    0x1d

#define KEY_E0      0xe0
#define KEY_E1      0xe1

#define KEY_ALT 		0x38		// E0, 38或38

#define	KEY_FUNC		 0x8000
#define KEY_F1			(0x3B)
#define KEY_F2			(0x3C)
#define KEY_F3			(0x3D)
#define KEY_F4			(0x3E)
#define KEY_F5			(0x3F)
#define KEY_F6			(0x40)
#define KEY_F7			(0x41)
#define KEY_F8			(0x42)
#define KEY_F9			(0x43)
#define KEY_F10			(0x44)
#define KEY_F11			(0x57)
#define KEY_F12			(0x58)

#define	KEY_SCROLL_LOCK		(0x46)
#define KEY_HOME			(0x47)
#define KEY_END				(0x4F)
#define KEY_PAGE_UP			(0x49)
#define KEY_PAGE_DOWN		(0x51)
#define KEY_CURSOR_UP		(0x48)
#define KEY_CURSOR_DOWN		(0x50)
#define KEY_CURSOR_RIGHT	(0x4D)
#define KEY_CURSOR_LEFT		(0x4B)
#define KEY_INSERT			(0x52)
#define KEY_DELETE			(0x53)

#define KEY_BACKSPACE		0xE
typedef struct _key_map_t
{
    uint8_t normal;
    uint8_t func;
}key_map_t;


typedef struct _kbd_state_t
{
    uint8_t lshift_press : 1;
    uint8_t rshift_press : 1;
    uint8_t caps_lock_press : 1;
    uint8_t lctrl_press : 1;
    uint8_t lalt_press : 1;
    uint8_t ralt_press : 1;
    uint8_t rctrl_press : 1;
}kbd_state_t;

#define KBD_PORT_DATA 0x60
#define KBD_PORT_STAT 0x64
#define KBD_PORT_CMD  0x64 
#define KBD_STAT_RECV_READY (1 << 0)
void kbd_init(void);
void exception_handler_kbd(void);
void do_handler_kbd(exception_frame_t *frame);

#endif
