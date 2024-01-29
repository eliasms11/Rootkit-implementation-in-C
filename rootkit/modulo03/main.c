#include <linux/module.h> // acceso a llamadas del kernel
#include <linux/kernel.h> // mensajes debug
#include <linux/init.h>  // para facilitar con macros

#include <linux/moduleparam.h> // esto se importa para tener acceso a los módulos de
                                // parámetro (sus funciones)
#include <linux/stat.h>

MODULE_LICENSE("GPL");


#define DRIVER_AUTHOR "Elias" 
#define DRIVER_DESC "Driver hola mundo"
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);



// MODULE_SUPPORTED_DEVICE("dispositivo de prueba") // A lo mejor no hace falta



static char *MiCadena = "";
module_param(MiCadena, charp, 0000); // parámetro del modulo, charc pointer, permisos
MODULE_PARM_DESC(MiCadena, "Esto es una cadena que sale tras echo.");
// La descripción es útil para informar en caso de que el parámetro fuera una dirección MAC, IP o 
// muchas otras cosas

static int HelloInit(void) {

    // printk(KERN_INFO 'ROOTKITDEV_DEBUG: HELLO WORLD! \n');
    printk(KERN_INFO "ROOTKITDEV_DEBUG: %s \n", MiCadena);
    return 0;
}


module_init(HelloInit);



/* EN EL TERMINAL TRAS COMPILAR CON gcc helloworld.c -o helloworld.o:
insmod -> insertar driver (usar archivo .ko tal que insmod -> insmod driver.ko)
o con parámetro: 

insmod helloworld.ko MiCadena="aqui_esta_mi_cadena" (Hay que usar "_" porque en bash los 
parámetros acaban cuando llegan al espacio)

lsmod  -> ver info del driver (lsmod | grep driver)  
rmmod  -> borrar driver (rmmod driver)

Los mensajes generados por el driver se solían guardar en var/log, pero en ubuntu 
se sitúan en /var/log/syslog
Para verlos ejecutamos  "tail -f /var/log/syslog | grep DEBUG"  */
