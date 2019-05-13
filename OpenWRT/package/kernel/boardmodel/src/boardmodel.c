#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include "mach/gpio.h"

#define MODEL_ID_0 13
#define MODEL_ID_1 14
#define MODEL_ID_2 100


struct proc_dir_entry *proc_file_entry;
struct device_node *node;


static int boardmodel_proc_show(struct seq_file *m, void *v) {

  
  gpio_free(MODEL_ID_0);
  gpio_free(MODEL_ID_1);
  gpio_free(MODEL_ID_2);

  gpio_direction_input(MODEL_ID_0);
  gpio_direction_input(MODEL_ID_1);
  gpio_direction_input(MODEL_ID_2);
  
  // Binary -> DEC
  int MODEL_ID = gpio_get_value(MODEL_ID_2)*4 + gpio_get_value(MODEL_ID_1)*2 + gpio_get_value(MODEL_ID_0)*1;

  switch(MODEL_ID)
     {
     case 1 : //Binary 001 = DEC 1
        seq_printf(m, "HVL-RS\n");
        break;
     default :
        seq_printf(m, "unknown_boardmodel\n");
     }
    return 0; 
  }

  static int boardmodel_proc_open(struct inode *inode, struct  file *file) {
    return single_open(file, boardmodel_proc_show, NULL);
  }


static const struct file_operations boardmodel_proc_fops = {
  .owner = THIS_MODULE,
  .open = boardmodel_proc_open,
  .read = seq_read,
  //.llseek = seq_lseek,
  .release = single_release,
};

static int __init_boardmodel_proc_init(void) {
  
  proc_file_entry = proc_create("boardmodel", 0, NULL, &boardmodel_proc_fops);
  

  if(proc_file_entry == NULL)
    return -ENOMEM;
  

  /*initial gpios*/
  int n;
  int num_gpios = of_gpio_count(node);

  if(num_gpios)
  {
    for(n=0; n<num_gpios; n++){
      int gpio = of_get_gpio_flags(node,n,NULL);

      if(gpio<0){
        printk("[%s]could not get gpio from of\n",__FUNCTION__);
        return -ENODEV;
      }

      if(!gpio_is_valid(gpio)){
        printk("[%s] gpio %d is not valid\n",__FUNCTION__,gpio);
        return -ENODEV;
      }

      if(gpio_request(gpio, node->name)){
        printk("[%s] could not request gpio, %d\n",__FUNCTION__,gpio);
        return -ENODEV;
      }
    }
  }
  
  return 0;
}

static void __exit_boardmodel_proc_exit(void) {
  remove_proc_entry("boardmodel", NULL);
}


module_init(__init_boardmodel_proc_init);
module_exit(__exit_boardmodel_proc_exit);
MODULE_LICENSE("GPL");