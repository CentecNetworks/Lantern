#ifndef _CTC8247_H
#define _CTC8247_H

/*must the same order with glb_hw_define.h*/
enum glb_board_series_e
{
    GLB_BOARD_SERIES_E300 = 0,
    GLB_BOARD_SERIES_E600,
    GLB_BOARD_SERIES_E800,
    GLB_BOARD_SERIES_EX2000,
    GLB_BOARD_SERIES_EX3000,
    GLB_BOARD_SERIES_E310,
    GLB_BOARD_SERIES_E810,
    GLB_BOARD_SERIES_CPCI,
    GLB_BOARD_SERIES_SEOUL = 0x8,
    GLB_BOARD_SERIES_HUMBER_DEMO = 0xf,
    GLB_BOARD_SERIES_MAX
};
typedef enum glb_board_series_e glb_board_series_t;

enum seoul_board_type_e
{
    BOARD_SEOUL_G24EU = 1,   
    BOARD_SEOUL_MAX
} ;
typedef enum seoul_board_type_e seoul_board_type_T;

#endif
