MAKEFILE_DIR=$(dir $(lastword $(MAKEFILE_LIST)))
INCLUDE_DIR=$(MAKEFILE_DIR)include
SRC_DIR=$(MAKEFILE_DIR)src
OUTPUT_DIR=$(MAKEFILE_DIR)lib

CC=gcc
CFLAGS=-shared -fPIC -DKXVER=3 -Wall -I$(INCLUDE_DIR)

XML_SOURCE=$(SRC_DIR)/xml.c
XML_LIB=-lxml2

all: xml64

xml32: mkdir
	$(CC) $(CFLAGS) -m32 -o $(OUTPUT_DIR)/xml.l32.so $(XML_SOURCE) $(XML_LIB)

xml64: mkdir
	$(CC) $(CFLAGS) -m64 -o $(OUTPUT_DIR)/xml.l64.so $(XML_SOURCE) $(XML_LIB)

mkdir:
	mkdir -p $(OUTPUT_DIR)
	rm -rf $(OUTPUT_DIR)/*
