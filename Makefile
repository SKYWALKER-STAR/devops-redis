###################################################
# Authtor:	ming
# 创建日期:	2024/2/20
# 最后修改日期:	2024/2/20
# 作用:		redis-dump Makefile
###################################################

HOME_DIR=$(shell pwd)
HIREDIS_DIR=$(HOME_DIR)/hiredis-1.1.0

SOURCE_NAME=redis-dump.c
TARGET_NAME=redis-dump
CFLAG= -g -o

.PHONY: all
all:
	make -C $(HIREDIS_DIR)
	$(CC) $(CFLAG) $(TARGET_NAME) $(SOURCE_NAME) -I $(HIREDIS_DIR) -L$(HIREDIS_DIR) -lhiredis 

clean:
	rm -rf ./redis-dump
	make -C $(HIREDIS_DIR) clean
