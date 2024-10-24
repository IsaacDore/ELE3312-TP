PPATH = Projet_reference
UVPATH = MDK-ARM
BUILD_OUT = .\log\build.log
FLASH_OUT = .\log\flash.log

TP6_PATH = ./TP6/$(PPATH)
TP6_UV_PATH = $(TP6_PATH)/$(UVPATH)
TP6_PROJ_NAME = FD_STM32
TP6_PROJ_FILE_NAME = $(TP6_PROJ_NAME).uvprojx
TP6_BUILD_OUT = $(TP6_UV_PATH)\$(BUILD_OUT)
TP6_FLASH_OUT = $(TP6_UV_PATH)\$(FLASH_OUT)
TP6_PROJ_PATH = $(TP6_UV_PATH)/$(TP6_PROJ_FILE_NAME)
TP6_BUILD_ARTEFACTS = $(TP6_UV_PATH)/$(TP6_PROJ_NAME)/*

TP7_PATH = ./TP7/$(PPATH)
TP7_UV_PATH = $(TP7_PATH)/$(UVPATH)
TP7_PROJ_NAME = Laboratoire7
TP7_PROJ_FILE_NAME = $(TP7_PROJ_NAME).uvprojx
TP7_BUILD_OUT = $(TP7_UV_PATH)\$(BUILD_OUT)
TP7_FLASH_OUT = $(TP7_UV_PATH)\$(FLASH_OUT)
TP7_PROJ_PATH = $(TP7_UV_PATH)/$(TP7_PROJ_FILE_NAME)
TP7_BUILD_ARTEFACTS = $(TP6_UV_PATH)/$(TP7_PROJ_NAME)/*

$(TP6_BUILD_OUT): $(TP6_PATH)/Core/Src/* $(TP6_PROJ_PATH) 
# gotta change working-dir because ARM cant be fucked to hire a software engineer
	cd  $(TP6_UV_PATH) && \
	UV4 -b $(TP6_PROJ_FILE_NAME) -o $(BUILD_OUT) & \
	type $(BUILD_OUT) && \
	UV4 -b $(TP6_PROJ_FILE_NAME)

tp6: $(TP6_BUILD_OUT) $(TP6_PROJ_PATH)
	cd  $(TP6_UV_PATH) && \
	UV4 -f $(TP6_PROJ_FILE_NAME) -o $(FLASH_OUT) & \
	type $(FLASH_OUT) && \
	UV4 -f $(TP6_PROJ_FILE_NAME)

tp6-keil:
	cd  $(TP6_UV_PATH) && \
	UV4 $(TP6_PROJ_FILE_NAME)

$(TP7_BUILD_OUT): tp7-gen-seed $(TP7_PATH)/Src/* $(TP7_PROJ_PATH) 
# gotta change working-dir because ARM cant be fucked to hire a software engineer
	cd  $(TP7_UV_PATH) && \
	UV4 -b $(TP7_PROJ_FILE_NAME) -o $(BUILD_OUT) & \
	type $(BUILD_OUT) && \
	UV4 -b $(TP7_PROJ_FILE_NAME)

tp7-gen-seed:
	python ./TP7/gen_seed.py > $(TP7_PATH)/Inc/seed.h

tp7: $(TP7_BUILD_OUT) $(TP7_PROJ_PATH)
	cd  $(TP7_UV_PATH) && \
	UV4 -f $(TP7_PROJ_FILE_NAME) -o $(FLASH_OUT) & \
	type $(FLASH_OUT) && \
	UV4 -f $(TP7_PROJ_FILE_NAME)

tp7-keil:
	cd  $(TP7_UV_PATH) && \
	UV4 $(TP7_PROJ_FILE_NAME)