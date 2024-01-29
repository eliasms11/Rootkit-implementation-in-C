#include </home/elias/Desktop/rootkit/modulo04/funciones.h>


 static void UnloadDriver(void){

    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_INFO "ROOTKITDEV_DEBUG: Driver unloaded \n");
    

 }


module_exit(UnloadDriver);