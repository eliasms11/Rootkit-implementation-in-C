#include <linux/module.h> // acceso a llamadas del kernel
#include <linux/kernel.h> // mensajes debug
#include <linux/init.h>  // para facilitar con macros

#include <linux/moduleparam.h> // esto se importa para tener acceso a los módulos de
                                // parámetro (sus funciones)
#include <linux/stat.h>


 static void HelloExit(void){

    printk(KERN_INFO "ROOTKITDEV_DEBUG : GOODBYE WORLD \n");
    

 }


module_exit(HelloExit);

