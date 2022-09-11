
$(info COMPILING FOR $(ARCH))
BUILD:=$(ROOT)/build$(SUFFIX)/$(ARCH)
JSON_CONF:=$(ROOT)/ads1115.conf.default.json
JSON_CONF_OBJ:=$(BUILD)/conf_json.o

$(shell mkdir -p $(BUILD))
EXECUTABLE:=$(BUILD)/ads1115d
SRC:=$(wildcard *.cc)
OBJS:=$(patsubst %.cc, $(BUILD)/%.o, $(SRC))
TEST_SRC:=$(wildcard tests/*.cc)
TEST_OBJS:=$(patsubst %.cc, $(BUILD)/%.o, $(TEST_SRC))
TEST_EXE:=$(patsubst %.cc, $(BUILD)/%, $(TEST_SRC))
TEST_EXTRA:=$(filter-out $(BUILD)/ads1115d.o,$(OBJS))

H:=$(wildcard *.h)
$(info Sources: $(SRC))
$(info Headers: $(H))
$(info Objects: $(OBJS))
$(info Executable: $(EXECUTABLE))

$(info Test Sources: $(TEST_SRC))
$(info Test Objects: $(TEST_OBJS))
$(info Test Exec: $(TEST_EXE))
$(info Test Extra: $(TEST_EXTRA))


SYSROOT:=$(ROOT)/sysroot/$(ARCH)
LDFLAGS:=--sysroot=$(SYSROOT)
CXXFLAGS:=$(OPT) \
	-I$(ROOT)/src \
	-Wall -fmessage-length=0 -Wno-pessimizing-move -Wno-unused-result \
	--sysroot=$(SYSROOT) \
	-I$(SYSROOT)/usr/include \
	-D_REENTRANT \
	-D_POSIX_C_SOURCE=202001L -D_XOPEN_SOURCE=600 -fPIC -pthread \
	-std=c++17

ifeq "$(ARCH)" "armhf"
	CXX:=arm-linux-gnueabihf-g++
	LDFLAGS:=$(LDFLAGS) -L$(SYSROOT)/lib/arm-linux-gnueabihf -L$(SYSROOT)/usr/lib/arm-linux-gnueabihf
	LDFLAGS:=$(LDFLAGS) -static-libstdc++ -static-libgcc -Wl,-Bstatic -ljansson -Wl,-Bdynamic
	OBJCOPY:=/usr/bin/arm-linux-gnueabihf-objcopy
	OBJCOPY_ARGS:= -v --input-target binary --output-target elf32-littlearm
	TESTER:= qemu-arm
	STRIP:=arm-linux-gnueabihf-strip
else ifeq "$(ARCH)" "armel"
	CXX:=arm-linux-gnueabi-g++
	LDFLAGS:=$(LDFLAGS) -L /usr/lib/arm-linux-gnueabi -L$(SYSROOT)/lib/arm-linux-gnueabihf
	LDFLAGS:=$(LDFLAGS) -Wl,-Bstatic -ljansson -Wl,-Bdynamic	
	OBJCOPY:=/usr/bin/arm-linux-gnueabi-objcopy
	OBJCOPY_ARGS:= --input-target binary --output-target elf32-littlearm
	TESTER:= qemu-arm
	STRIP:=arm-linux-gnueabi-strip
endif
$(info CXX $(CXX))
$(info LDFLAGS $(LDFLAGS))
$(info CXXFLAGS $(CXXFLAGS))

$(JSON_CONF_OBJ) : $(JSON_CONF)
	sh -c "cp $(JSON_CONF) $(BUILD)/conf_json && cd $(BUILD) && $(OBJCOPY) $(OBJCOPY_ARGS) conf_json $@ && rm conf_json"

$(BUILD)/%.o : %.cc
	echo "Processing " $< " into " $@
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/tests/%.o : tests/%.cc
	echo "Processing " $< " into " $@
	mkdir -p $(BUILD)/tests
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/tests/% : $(BUILD)/tests/%.o
	echo "Processing " $< " into " $@
	$(CXX) -o $@ $(TEST_EXTRA) $(JSON_CONF_OBJ) $< $(LDFLAGS)
	@echo "Built: " $@
	@$(TESTER) $@ || { echo Test $@ Failed!!! ; exit 1; }

#mkdir -p $(@D)
	

$(BUILD)/tests/% : $(OBJ) 

$(OBJS) : $(SRC) $(H) Makefile arch.mk

$(TEST_OBJS) : $(TEST_SRC)  $(SRC) $(H) Makefile arch.mk


$(EXECUTABLE): $(OBJS) $(JSON_CONF_OBJ)
	$(CXX) -o $@ $(OBJS) $(JSON_CONF_OBJ) $(LDFLAGS)
ifeq "$(SUFFIX)" ""
	$(STRIP) $@
endif	

$(TEST_EXE) : $(EXECUTABLE)

all: $(EXECUTABLE) $(TEST_EXE) 

clean:
	rm -rf $(BUILD)/*
	
.PHONY: clean
