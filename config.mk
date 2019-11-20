export BUILD_ROOT = $(shell pwd)
export INCLUDE_PATH = $(BUILD_ROOT)/_include
export DEBUG = true

BUILD_DIR = $(BUILD_ROOT)/judgingcore/ \
			$(BUILD_ROOT)/test/ \
			$(BUILD_ROOT)/app/ \
