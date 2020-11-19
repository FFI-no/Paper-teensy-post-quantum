IMPLEMENTATION_PATH_KEM := pqm4/crypto_kem/sikep751/m4
IMPLEMENTATION_PATH_SIGN := pqm4/crypto_sign/falcon-1024/m4-ct

BOARD_ID    := TEENSY36
CORE_BASE   := cores/teensy3/
GCC_BASE    :=

FLAGS_CPU   := -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant
FLAGS_OPT   := -O2 -DMUPQ_NAMESPACE=$(MUPQ_NAMESPACE)
FLAGS_COM   := -g -Wall -ffunction-sections -fdata-sections -MMD
FLAGS_LSP   :=

FLAGS_CPP   := -fno-exceptions -felide-constructors -std=gnu++14 -Wno-error=narrowing -fno-rtti
FLAGS_C     :=
FLAGS_S     := #-x assembler-with-cpp
FLAGS_LD    := -Wl,--gc-sections,--relax,--defsym=__rtc_localtime=0

LIBS        := -lm -lstdc++ # -larm_cortexM4lf_math
LD_SCRIPT   := mk66fx1m0.ld

DEFINES     := -D__MK66FX1M0__ -DTEENSYDUINO=144
DEFINES     += -DF_CPU=180000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10807

CPP_FLAGS   := $(FLAGS_CPU) $(FLAGS_OPT) $(FLAGS_COM) $(DEFINES) $(FLAGS_CPP)
C_FLAGS     := $(FLAGS_CPU) $(FLAGS_OPT) $(FLAGS_COM) $(DEFINES) $(FLAGS_C)
S_FLAGS     := $(FLAGS_CPU) $(FLAGS_OPT) $(FLAGS_COM) $(DEFINES) $(FLAGS_S)
LD_FLAGS    := $(FLAGS_CPU) $(FLAGS_OPT) $(FLAGS_LSP) $(FLAGS_LD)
AR_FLAGS    := rcs

USR_SRC     := src
PQM4_SRC    := pqm4
AES_SRC			:= arduinolibs/libraries
USR_INCLUDE := inc
CORE_SRC    := $(CORE_BASE)
COMMON_SRC  := common

BIN         := bin
USR_BIN     := $(BIN)/src
PQM4_BIN    := $(BIN)/pqm4
AES_BIN	    := $(BIN)/aes
CORE_BIN    := $(BIN)/core
COMMON_BIN  := $(BIN)/common
CORE_LIB    := $(BIN)/core.a


#******************************************************************************
# BINARIES
#******************************************************************************
CC          := $(GCC_BASE)arm-none-eabi-gcc
CXX         := $(GCC_BASE)arm-none-eabi-g++
AR          := $(GCC_BASE)arm-none-eabi-gcc-ar
OBJCOPY     := $(GCC_BASE)arm-none-eabi-objcopy
SIZE        := $(GCC_BASE)arm-none-eabi-size
OBJDUMP     := $(GCC_BASE)arm-none-eabi-objdump

#******************************************************************************
# Source and Include Files
#******************************************************************************
# Recursively create list of source and object files in USR_SRC and CORE_SRC
# and corresponding subdirectories.
# The function rwildcard is taken from http://stackoverflow.com/a/12959694)

rwildcard =$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# pqm4 Sources ----------------------------------------------------------------
COMMONINCLUDES_M4=-Icommon

COMMON_C_FILES   := common/aes.c common/sha2.c common/crypto_hashblocks_sha512.c common/fips202.c common/sp800-185.c common/nistseedexpander.c
COMMON_CPP_FILES := common/randombytes.cpp common/hal-teensy36.cpp
COMMON_SS_FILES  := common/keccakf1600.S common/aes-asm.S
COMMON_S_FILES   := common/crypto_hashblocks_sha512_inner32.s
COMMON_OBJ       := $(COMMON_SS_FILES:$(COMMON_SRC)/%.S=$(COMMON_BIN)/%.o) $(COMMON_S_FILES:$(COMMON_SRC)/%.s=$(COMMON_BIN)/%.o) $(COMMON_C_FILES:$(COMMON_SRC)/%.c=$(COMMON_BIN)/%.o) $(COMMON_CPP_FILES:$(COMMON_SRC)/%.cpp=$(COMMON_BIN)/%.o)

PQM4_C_FILES   := $(wildcard $(IMPLEMENTATION_PATH_KEM)/*.c) $(wildcard $(IMPLEMENTATION_PATH_SIGN)/*.c)
PQM4_CPP_FILES :=
PQM4_SS_FILES  := $(wildcard $(IMPLEMENTATION_PATH_KEM)/*.S) $(wildcard $(IMPLEMENTATION_PATH_SIGN)/*.S)
PQM4_S_FILES   := $(wildcard $(IMPLEMENTATION_PATH_KEM)/*.s) $(wildcard $(IMPLEMENTATION_PATH_SIGN)/*.s)
PQM4_OBJ       := $(PQM4_SS_FILES:$(PQM4_SRC)/%.S=$(PQM4_BIN)/%.o) $(PQM4_S_FILES:$(PQM4_SRC)/%.s=$(PQM4_BIN)/%.o) $(PQM4_C_FILES:$(PQM4_SRC)/%.c=$(PQM4_BIN)/%.o) $(PQM4_CPP_FILES:$(PQM4_SRC)/%.cpp=$(PQM4_BIN)/%.o)


# AES Sources  ----------------------------------------------------------------
IMPLEMENTATION_PATH_AES = $(AES_SRC)/Crypto
AES_CPP_FILES  := $(wildcard $(IMPLEMENTATION_PATH_AES)/*.cpp)
AES_OBJ				 := $(AES_CPP_FILES:$(AES_SRC)/%.cpp=$(AES_BIN)/%.o)


# User Sources ----------------------------------------------------------------
USR_C_FILES    := $(call rwildcard,$(USR_SRC)/,*.c)
USR_CPP_FILES  := $(call rwildcard,$(USR_SRC)/,*.cpp)
USR_S_FILES    := $(call rwildcard,$(USR_SRC)/,*.S)
USR_OBJ        := $(USR_S_FILES:$(USR_SRC)/%.s=$(USR_BIN)/%.o) $(USR_S_FILES:$(USR_SRC)/%.S=$(USR_BIN)/%.o) $(USR_C_FILES:$(USR_SRC)/%.c=$(USR_BIN)/%.o) $(USR_CPP_FILES:$(USR_SRC)/%.cpp=$(USR_BIN)/%.o)
initiator_OBJ  := $(filter-out %responder.o, $(USR_OBJ))
responder_OBJ  := $(filter-out %initiator.o, $(USR_OBJ))

# Core library sources --------------------------------------------------------
CORE_CPP_FILES := $(call rwildcard,$(CORE_SRC)/,*.cpp)
CORE_C_FILES   := $(call rwildcard,$(CORE_SRC)/,*.c)
CORE_S_FILES   := $(call rwildcard,$(CORE_SRC)/,*.S)
CORE_OBJ       := $(CORE_S_FILES:$(CORE_SRC)/%.S=$(CORE_BIN)/%.o) $(CORE_C_FILES:$(CORE_SRC)/%.c=$(CORE_BIN)/%.o) $(CORE_CPP_FILES:$(CORE_SRC)/%.cpp=$(CORE_BIN)/%.o)

INCLUDE        := -I./$(USR_INCLUDE) -I$(CORE_SRC) -I./arduinolibs/* $(COMMONINCLUDES_M4) #-I./pqm4/*

#******************************************************************************
# Rules:
#******************************************************************************

.PHONY: all initiator responder rebuild clean cleanUser cleanCore
.SECONDARY: $(CORE_LIB) $(USR_OBJ) $(COMMON_OBJ) $(PQM4_OBJ) $(AES_OBJ) $(BIN)/initiator.elf $(BIN)/responder.elf

all:     initiator responder
initiator: $(BIN)/initiator.lst $(BIN)/initiator.hex
responder: $(BIN)/responder.lst $(BIN)/responder.hex
rebuild: cleanUser all
clean:   cleanUser cleanCore

# Core library ----------------------------------------------------------------
$(CORE_BIN)/%.o: $(CORE_SRC)/%.S
	@echo [ASM] CORE $(notdir $<)
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(S_FLAGS) $(INCLUDE) -o $@ -c $<

$(CORE_BIN)/%.o: $(CORE_SRC)/%.c
	@echo [CC]  CORE $(notdir $<)
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(C_FLAGS) $(INCLUDE) -o $@ -c $<

$(CORE_BIN)/%.o: $(CORE_SRC)/%.cpp
	@echo [CPP] CORE $(notdir $<)
	@mkdir -p "$(dir $@)"
	@"$(CXX)" $(CPP_FLAGS) $(INCLUDE) -o $@ -c $<

$(CORE_LIB) : $(CORE_OBJ)
	@echo [AR]  $@
	@$(AR) $(AR_FLAGS) $@ $^
	@echo Teensy core built successfully

# Handle user sources ---------------------------------------------------------
$(USR_BIN)/%.o: $(USR_SRC)/%.S
	@echo [ASM] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(S_FLAGS) $(INCLUDE) -o $@ -c $<

$(USR_BIN)/%.o: $(USR_SRC)/%.cpp
	@echo [CPP] $<
	@mkdir -p "$(dir $@)"
	@"$(CXX)" $(CPP_FLAGS) $(INCLUDE) -o "$@" -c $<

$(USR_BIN)/%.o: $(USR_SRC)/%.c
	@echo [CC] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(C_FLAGS) $(INCLUDE) -o "$@" -c $<

# Handle pqm4 sources ---------------------------------------------------------
$(PQM4_BIN)/%.o: $(PQM4_SRC)/%.S
	@echo [ASM] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(S_FLAGS) $(INCLUDE) -o $@ -c $<

$(PQM4_BIN)/%.o: $(PQM4_SRC)/%.s
	@echo [ASM] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(S_FLAGS) $(INCLUDE) -o $@ -c $<

$(PQM4_BIN)/%.o: $(PQM4_SRC)/%.cpp
	@echo [CPP] $<
	@mkdir -p "$(dir $@)"
	@"$(CXX)" $(CPP_FLAGS) $(INCLUDE) -o "$@" -c $<

$(PQM4_BIN)/%.o: $(PQM4_SRC)/%.c
	@echo [CC] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(C_FLAGS) $(INCLUDE) -o "$@" -c $<

# Handle common sources ---------------------------------------------------------
$(COMMON_BIN)/%.o: $(COMMON_SRC)/%.S
	@echo [ASM] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(S_FLAGS) $(INCLUDE) -o $@ -c $<

$(COMMON_BIN)/%.o: $(COMMON_SRC)/%.s
	@echo [ASM] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(S_FLAGS) $(INCLUDE) -o $@ -c $<

$(COMMON_BIN)/%.o: $(COMMON_SRC)/%.cpp
	@echo [CPP] $<
	@mkdir -p "$(dir $@)"
	@"$(CXX)" $(CPP_FLAGS) $(INCLUDE) -o "$@" -c $<

$(COMMON_BIN)/%.o: $(COMMON_SRC)/%.c
	@echo [CC] $<
	@mkdir -p "$(dir $@)"
	@"$(CC)" $(C_FLAGS) $(INCLUDE) -o "$@" -c $<

# Handle AES sources ----------------------------------------------------------
$(AES_BIN)/%.o: $(AES_SRC)/%.cpp
	@echo [CPP] $<
	@mkdir -p "$(dir $@)"
	@"$(CXX)" $(CPP_FLAGS) $(INCLUDE) -o "$@" -c $<

# Linking ---------------------------------------------------------------------
$(BIN)/initiator.elf: $(CORE_LIB) $(initiator_OBJ) $(COMMON_OBJ) $(PQM4_OBJ) $(AES_OBJ)
	@echo [LD]  $@
	@$(CXX) $(LD_FLAGS) -T$(CORE_SRC)/$(LD_SCRIPT) -o "$@" $(PQM4_OBJ) $(initiator_OBJ) $(COMMON_OBJ) $(CORE_LIB) $(AES_OBJ) $(LIBS)
	@echo User code built and linked to core lib

$(BIN)/responder.elf: $(CORE_LIB) $(responder_OBJ) $(COMMON_OBJ) $(PQM4_OBJ) $(AES_OBJ)
	@echo [LD]  $@
	@$(CXX) $(LD_FLAGS) -T$(CORE_SRC)/$(LD_SCRIPT) -o "$@" $(PQM4_OBJ) $(responder_OBJ) $(COMMON_OBJ) $(CORE_LIB) $(AES_OBJ) $(LIBS)
	@echo User code built and linked to core lib


%.lst: %.elf
	@echo [LST] $@
	@$(OBJDUMP) -d -S --demangle --no-show-raw-insn --syms "$<"  > "$@"
	@echo Listfile generated

%.hex: %.elf
	@echo [HEX] $@
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R.eeprom "$<" "$@"
	@echo Sucessfully built project

# Cleaning --------------------------------------------------------------------
cleanUser:
	@echo Cleaning user binaries...
	@rm  -rf "$(USR_BIN)"

	@rm -f $(BIN)/*.hex
	@rm -f $(BIN)/*.elf
	@rm -f $(BIN)/*.lst

cleanCore:
	@echo Cleaning core binaries...
	@rm -rf "$(CORE_BIN)"
	@rm -f "$(CORE_LIB)"
	@rm -rf "$(PQM4_BIN)"
	@rm -rf "$(AES_BIN)"
	@rm -rf "$(COMMON_BIN)"

# compiler generated dependency info ------------------------------------------
-include $(CORE_OBJ:.o=.d)
-include $(USR_OBJ:.o=.d)
