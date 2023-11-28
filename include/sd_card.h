/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_CARD_H
#define __SD_CARD_H

#define SD_INIT_TIMEOUT 2000U

typedef enum _sd_command{
    CMD_0 =0,
    CMD_1 =1,
    CMD_8 =8,
    CMD_9 =9,
    CMD_10 =10,
    CMD_12 =12,
    CMD_16 =16,
    CMD_17 =17,
    CMD_18 =18,
    CMD_23 =23,
    CMD_24 =24,
    CMD_25 =25,
    CMD_41 =41,
    CMD_55 =55,
    CMD_58 =58
}SD_Command;

#endif