#!/bin/bash

sudo dd if=stage2/build/stage2.bin of=/dev/sda1 bs=1024 count=1 conv=notrunc
