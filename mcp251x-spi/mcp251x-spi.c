#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/can/platform/mcp251x.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

int busnum = 1;
int chip_select = 0;
int gpio_int = 390;

module_param(busnum, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(busnum, "busnum of spi bus to use");

module_param(gpio_int, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(gpio_int, "linux gpio number of INT gpio");

module_param(chip_select, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(chip_select, "spi chip select");

int gpio_requested = 0;

struct spi_device * dev1;
static struct mcp251x_platform_data mcp251x_info = {
  .oscillator_frequency = 16000000,
};

static struct spi_board_info spi_device_info = {
  .modalias = "mcp2515",
  .platform_data = & mcp251x_info,
  .irq = -1,
  .max_speed_hz = 10 * 1000 * 1000,
};

static int __init mcp2515_init(void) {
  int ret;
  struct spi_master * master;

  printk("mcp2515: init\n");

  ret = gpio_request(gpio_int, "sysfs");

  if (ret) {
    printk("mcp2515: could not request gpio %d\n", gpio_int);
    gpio_free(gpio_int);
    return ret;
  }
  gpio_requested = 1;

  gpio_direction_input(gpio_int);

  ret = gpio_to_irq(gpio_int);
  printk("mcp2515: irq for pin %d is %d\n", gpio_int, ret);
  spi_device_info.irq = ret;
  spi_device_info.bus_num = busnum;
  spi_device_info.chip_select = chip_select;

  master = spi_busnum_to_master(spi_device_info.bus_num);
  if (!master) {
    printk("mcp2515: MASTER not found.\n");
    ret = -ENODEV;
    goto error_postgpio;
  }

  // create a new slave device, given the master and device info
  dev1 = spi_new_device(master, & spi_device_info);

  if (!dev1) {
    printk("mcp2515: FAILED to create slave.\n");
    ret = -ENODEV;
    goto error_postgpio;
  }

  printk("mcp2515: device created!\n");

  return 0;

  error_postgpio:
    gpio_free(gpio_int);
  return ret;
}

static void __exit mcp2515_exit(void) {
  printk("mcp2515: exit\n");

  if (dev1) {
    spi_unregister_device(dev1);
  }
  if (gpio_requested)
    gpio_free(gpio_int);
}

module_init(mcp2515_init);
module_exit(mcp2515_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("linuxkidd");
MODULE_DESCRIPTION("MCP2515 for Odyssey");
