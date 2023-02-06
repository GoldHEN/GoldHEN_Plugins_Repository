DEBUG ?= 0
LOG ?= KLOG

ifeq ($(DEBUG),1)
	TYPE=debug
	DEBUG_FLAG=DEBUG=1
else
	TYPE=final
	DEBUG_FLAG=DEBUG=0
endif

ifeq ($(LOG), PRINTF)
	LOG_TYPE=PRINTF=1
	LOG_MSG=libc:printf
else
	LOG_TYPE=
	LOG_MSG=GoldHEN:SDK:KernelLog
endif

all: build hashes

build_sdk:
	@echo "[+] Rebuilding SDK with Log Type: $(LOG_MSG)"
	make -C $(GOLDHEN_SDK) $(LOG_TYPE) DEBUGFLAGS=1

BUILD_PRX=bin/plugins/prx_$(TYPE)
BUILD_ELF=bin/plugins/elf_$(TYPE)

$(BUILD_PRX) $(BUILD_ELF): build_sdk
	@rm -rf $(BUILD_PRX) $(BUILD_ELF)
	@mkdir -p $(BUILD_PRX) $(BUILD_ELF)
	@echo "[+] building plugins"
	@for dir in plugin_src/*; do \
		if [ -f "$$dir/Makefile" ]; then \
			echo "[+] building dir: $$dir"; \
			make -C "$$dir" clean || exit 1; \
			make -C "$$dir" $(DEBUG_FLAG) $(LOG_TYPE) $(O_FLAG) || exit 1; \
		fi; \
	done

build: $(BUILD_PRX) $(BUILD_ELF)

hashes:
	@echo "MD5:" > $(BUILD_PRX)/md5.txt
	@echo "SHA256:" > $(BUILD_PRX)/sha256.txt
	@for f in $(BUILD_PRX)/*.prx; do \
		echo "$$(sha256sum $$f)" >> $(BUILD_PRX)/sha256.txt; \
		echo "$$(md5sum $$f)" >> $(BUILD_PRX)/md5.txt; \
	done
