#################################################################
# Title: Makefile
# Author: Sahaj Sarup
# Copyright (c) 2019 Linaro Limited
#################################################################

CC=gcc
CFLAGS=-lmraa -lpthread -lncurses
FPGA_LIB=fpga_mezz_lib
INCLUDE=$(FPGA_LIB)/include
FPGA_LIB_C=$(FPGA_LIB)/src/fpga_mezz.c
SRC=src

all:
	@$(CC) $(CFLAGS) -I$(INCLUDE) $(SRC)/binjuke.c $(FPGA_LIB_C) -o binjuke

clean:
	rm -rf main
