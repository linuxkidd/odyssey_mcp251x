#!/bin/bash

make -C /lib/modules/$(uname -r)/build M=$(pwd)/mcp251x-spi clean

