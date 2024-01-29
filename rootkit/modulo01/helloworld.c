#include <linux/module.h>
#include <linux/kernel.h> // Debug messages
#include <linux/init.h>   //macros

 static int HelloInit(void){

    printk(KERN_INFO "ROOTKITDEV_DEBUG: HELLO WORLD! \n");
    return 0; 
 }
 

 static void HelloExit(void){

    printk(KERN_INFO "ROOTKITDEV_DEBUG : GOODBYE WORLD \n");
    

 }


module_init(HelloInit);
module_exit(HelloExit);

MODULE_LICENSE("GPL");

/* make all
sudo insmod helloworld.ko
sudo lsmod | grep hello: tamaño y permisos
sudo rmmod helloworld.ko

tail -f /var/log/syslog | grep DEBUG */