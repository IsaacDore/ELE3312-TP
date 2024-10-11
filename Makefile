TP6_PRE_PATH = ./TP6/Projet_reference
TP6_PRE_UV_PATH = $(TP6_PRE_PATH)/MDK-ARM
TP6_PRE_PROJ_NAME = FD_STM32.uvprojx
BUILD_OUT = .\log\build.log
FLASH_OUT = .\log\flash.log
TP6_PRE_BUILD_OUT = $(TP6_PRE_UV_PATH)\$(BUILD_OUT)
TP6_PRE_FLASH_OUT = $(TP6_PRE_UV_PATH)\$(FLASH_OUT)
TP6_PRE_PROJ_PATH = $(TP6_PRE_UV_PATH)/$(TP6_PRE_PROJ_NAME)
TP6_PRE_BUILD_ARTEFACTS = $(TP6_PRE_UV_PATH)/FD_STM32/*

$(TP6_PRE_BUILD_OUT): $(TP6_PRE_PATH)/Core/* $(TP6_PRE_PROJ_PATH)
# gotta change working-dir because ARM cant be fucked to hire a software engineer
	cd  $(TP6_PRE_UV_PATH) && \
	UV4 -b $(TP6_PRE_PROJ_NAME) -o $(BUILD_OUT) & \
	type $(BUILD_OUT) && \
	UV4 -b $(TP6_PRE_PROJ_NAME)

pretp6: $(TP6_PRE_BUILD_OUT) $(TP6_PRE_PROJ_PATH)
	cd  $(TP6_PRE_UV_PATH) && \
	UV4 -f $(TP6_PRE_PROJ_NAME) -o $(FLASH_OUT) & \
	type $(FLASH_OUT) && \
	UV4 -f $(TP6_PRE_PROJ_NAME)

pretp6-keil:
	cd  $(TP6_PRE_UV_PATH) && \
	UV4 $(TP6_PRE_PROJ_NAME)