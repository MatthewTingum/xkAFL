XBE_TITLE = harness
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.c
NXDK_DIR = $(CURDIR)/../..
include $(NXDK_DIR)/Makefile
