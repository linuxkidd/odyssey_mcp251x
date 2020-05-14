#!/bin/bash

make -C /lib/modules/$(uname -r)/build M=$(pwd)/mcp251x-spi modules
cp mcp251x-spi/mcp251x-spi.ko /lib/modules/$(uname -r)/kernel/drivers/net/can/spi/
depmod -a
