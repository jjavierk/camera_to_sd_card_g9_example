APP = csi2_cam_sdcard_example
SRC_DIR = src
INC_DIR = inc


APP_SRCS =	$(wildcard $(SRC_DIR)/*.c) 

CONFIG_GAP_LIB_IMGIO = 1

APP_CFLAGS  += -I$(INC_DIR)
APP_CFLAGS      += -DSAVE_PIC -D OV5647
APP_CFLAGS      += -D I3C_I2C
APP_CFLAGS      += -DLOGS

APP_LDFLAGS =

# Needed for pulpos
CONFIG_I2C=1

ifeq '$(platform)' 'fpga'
# FreeRTOS uses the fast oscillator by default
# but there is none on FPGA.
CONFIG_NO_FAST_OSC=1
endif

include $(RULES_DIR)/pmsis_rules.mk
