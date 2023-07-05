ifeq ($(DEBUG), 1)
	TYPE = debug
	DEBUG_FLAG = DEBUG=1
else
	TYPE = final
	DEBUG_FLAG = DEBUG=0
endif

LOG_TYPE = PRINTF=1
LOG_MSG = libc:printf

BUILD_DIR = bin/plugins
BUILD_PRX = $(BUILD_DIR)/prx_$(TYPE)
BUILD_ELF = $(BUILD_DIR)/elf_$(TYPE)

.PHONY: all build_sdk build hashes clean

all: build hashes

build_sdk:
	@echo "[+] Rebuilding SDK with Log Type: $(LOG_MSG)"
	make -C $(GOLDHEN_SDK) $(LOG_TYPE) DEBUGFLAGS=1

$(BUILD_PRX): build_sdk
	@rm -rf $(BUILD_PRX) $(BUILD_ELF)
	@mkdir -p $(BUILD_PRX) $(BUILD_ELF)
	@echo "[+] Building plugins"
	@for dir in plugin_src/*; do \
		if [ -f "$$dir/Makefile" ]; then \
			@echo "[+] Building dir: $$dir"; \
			make -C "$$dir" clean || exit 1; \
			make -C "$$dir" $(DEBUG_FLAG) $(LOG_TYPE) || exit 1; \
		fi; \
	done

build: $(BUILD_PRX)

hashes: $(BUILD_PRX)
	@echo "MD5:" > $(BUILD_PRX)/md5.txt
	@echo "SHA256:" > $(BUILD_PRX)/sha256.txt
	@for f in $(BUILD_PRX)/*.prx; do \
		echo "$$(sha256sum $$f)" >> $(BUILD_PRX)/sha256.txt; \
		echo "$$(md5sum $$f)" >> $(BUILD_PRX)/md5.txt; \
	done

clean:
	@echo "[+] Cleaning build files"
	@rm -rf $(BUILD_DIR)
