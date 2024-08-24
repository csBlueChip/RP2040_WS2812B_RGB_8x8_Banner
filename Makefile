#--------------------------------------
OUT    := UF2
DEBUG  := 0

#--------------------------------------
BDIR   := b

MNT    := /home/pico/mnt

TTY    := /dev/ttyACM0
BAUD   := 115200

#--------------------------------------
CMFLAGS =

ifeq ($(PICO_SDK_PATH),)
CMFLAGS += -DPICO_SDK_PATH=/home/pico/pico-sdk
endif

ifeq ($(DEBUG), 1)
CMFLAGS += -DCMAKE_BUILD_TYPE=Debug
endif

#--------------------------------------
SHELL := /bin/bash
NAME  := ${shell sed -n '/project/{s/project( *//;s/[) ].*//;p;}' CMakeLists.txt}
UF2   := $(BDIR)/$(NAME).uf2
ELF   := $(BDIR)/$(NAME).elf
SRC   := $(wildcard *.c *.pio zxorigins/zxorigins.c)
HDR   := $(wildcard *.h       zxorigins/zxorigins.h)

#--------------------------------------
LRED = \e[1;31m
LGRN = \e[1;32m
YEL  = \e[1;33m
LWHT = \e[1;37m
NRM  = \e[0m

#--------------------------------------
# MUST be the first rule in the Makefile
# Typical default builds are "help", "all", "upload", ...
#
.PHONY: default
default: help

#--------------------------------------
# Help
#
.PHONY: help
help:
	@echo "Make options"
	@echo -e "  $(YEL)all        $(NRM)- default build parameters ($(OUT))"
	@echo -e "  $(YEL)clean      $(NRM)- erase ALL build files"
	@echo -e "  $(YEL)upload up  $(NRM)- upload UF2 to Pico-as-MSD"

	@echo -e "  $(LWHT)elf ELF    $(NRM)- build project, UF2 build not /required/"
	@echo -e "  $(LWHT)bin BIN    $(NRM)- synonym for ELF"
	@echo -e "  $(LWHT)UF2        $(NRM)- build project, and check UF2 build success"

	@echo -e "  $(LWHT)minicom mc $(NRM)- run minicom"
	@echo -e "  $(LWHT)test       $(NRM)- run test harness"
	@echo -e "  $(LWHT)MSD        $(NRM)- wait for Pico-as-MSD: $(MNT)"
	@echo -e "  $(LWHT)CDC        $(NRM)- wait for Pico-as-CDC: $(TTY) @$(BAUD)"
	@echo -e "  $(LWHT)help       $(NRM)- this text"
	@echo -e "Build directory: $(LWHT)$(BDIR)/$(NRM)"

#--------------------------------------
# Common Name to build everything
#
.PHONY: all
all: $(OUT)

#--------------------------------------
# Build the firmware - ELF required
#
.PHONY: ELF
ELF: $(ELF)

.PHONY: elf
elf: $(ELF)

$(ELF): $(SRC) $(HDR) CMakeLists.txt Makefile
	@mkdir -p $(BDIR)
	( cd $(BDIR) ; cmake $(CMFLAGS) .. ; )
	@make -C $(BDIR)

#--------------------------------------
# Build the firmware - CMake rules
#
.PHONY: BIN
BIN: $(ELF)

.PHONY: bin
bin: $(ELF)

#--------------------------------------
# Build the firmware - UF2 required
# ...controlled by the CMake rules
#
.PHONY: UF2
UF2: $(UF2)

$(UF2): ELF
	@( [[ ! -f $(UF2) ]] && \
	   { \
	     echo -e "$(LRED)! Build Failed : $(NRM)$(UF2)" ;\
	     grep '^[^#]*pico_add_extra_outputs' CMakeLists.txt >/dev/null || \
	       echo -e "  $(LWHT)pico_add_extra_outputs(\$${PROJECT_NAME}) $(NRM)not found in $(LWHT)CMakeLists.txt$(NRM)" ;\
	     false ;\
	   } ; : \
	 )

#--------------------------------------
# Erase ALL build files
#
.PHONY: clean
clean:
	rm -rf $(BDIR)

#--------------------------------------
# Wait for Pico to be mounted as MSD
#
.PHONY: MSD
MSD:
	@( [[ ! -f $(MNT)/INFO_UF2.TXT ]] && \
	   { \
	     echo -en "$(YEL)# Connect/Reset the pico while holding the BOOTSEL button$(NRM)" ;\
	     while [[ ! -f $(MNT)/INFO_UF2.TXT ]] ;\
	       do echo -n "." ; sleep 1 ; done ;\
	     echo "" ;\
	   } ; : \
	 )

#--------------------------------------
# Wait for Pico to be mounted as CDC
#
.PHONY: CDC
CDC: $(TTY)

.PHONY: $(TTY)
$(TTY):
#	@( [[ ! -e $(TTY) ]] && { echo "! $(TTY) not found"; false; }
	@( [[ ! -e $(TTY) ]] && \
	   { \
	     echo -en "$(YEL)# Waiting for $(TTY)$(NRM)" ;\
	     while [[ ! -e $(TTY) ]] ;\
	       do echo -n "." ; sleep 1 ; done ;\
	     echo "" ;\
	   } ; : \
	 )

#--------------------------------------
# Upload firmware to Pico
#
.PHONY: up
up: upload

.PHONY: upload
upload: $(UF2) MSD
	@echo -en "$(LGRN)# Uploading..."
	@cp $(UF2) $(MNT) ; sync
	@echo -e "\n$(YEL)* Finished$(NRM)"

#--------------------------------------
# Start minicom
#
.PHONY: mc
mc: minicom

.PHONY: minicom
minicom: CDC
	minicom -b 115200 -o -D $(TTY)

#--------------------------------------
# Test harness
#
.PHONY: test
test: CDC
	@echo -e "# No Test Harness - see Makefile::test"









.PHONY: nofonts
nofonts:
	rm fonts/* || true
	rm -rf fonts/tmp
