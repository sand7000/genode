INC_DIR += $(call select_from_ports,curl)/include

INC_DIR += $(REP_DIR)/src/lib/curl/spec/32bit
INC_DIR += $(REP_DIR)/src/lib/curl/spec/32bit/curl
INC_DIR += $(REP_DIR)/include/python
ifeq ($(filter-out $(SPECS),x86_32),)
  SPEC = x86_32
else ifeq ($(filter-out $(SPECS),x86_64),)
  SPEC = x86_64
else
  SPEC = arm
endif
INC_DIR += $(REP_DIR)/src/lib/openssl/spec/$(SPEC)

include $(REP_DIR)/lib/mk/curl.inc

CC_CXX_WARN_STRICT =
