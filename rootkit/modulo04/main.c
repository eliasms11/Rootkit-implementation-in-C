#include </home/elias/Desktop/rootkit/modulo04/funciones.h>

MODULE_LICENSE("GPL");


#define DRIVER_AUTHOR "Elias" 
#define DRIVER_DESC "Driver"
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

int Major; //numero de dispositivo al que llamamos
static int Device_Open = 0; // indicador de si el dispositivo esta activo o no
static char msg[BUF_LEN] = {0}; // puntero
static char *msg_punt; //puntero del mensaje


static struct  file_operations fops =  {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

// FUNCIONES MODULO_INIT

int init_module(void) { // Esta funcion nos informara de si el dispositivo se ha registrado correctamente
                        // y como crear un dispositivo con el que podamos "hablar" con nuestro driver

Major = register_chrdev(0, DEVICE_NAME, &fops);

if(Major < 0) {// si error
    printk(KERN_ALERT "Ha fallado la carga \n");
    return Major;
}

printk(KERN_ALERT "Se me ha asignado un driver, major numero %d\n", Major);
printk(KERN_ALERT "Por favor, crea un dispositivo con nombre \n mknod /dev/%s c %d 0 \n", DEVICE_NAME, Major); //mknod porque es como se crea un dispositivo (fichero especial)
                 // Una vez creamos nuestro dispositivo e interactuamos con el, cuando nuestro driver reciba input (read, open, write) es cuando trabajaremos con el
return 0;

}




int device_open(struct inode * inode, struct file * file) {
    static int contador = 1;
    if (Device_Open) {
        return -EBUSY;

    }
    
    sprintf(msg, "Se ha abierto %d veces\n", contador++); // mensaje de aviso de que se ha abierto el dipositivo
    msg_punt = msg;
    try_module_get(THIS_MODULE);
    return 0;

}
int device_release(struct inode * inode, struct file * file) { //una vez nuestro dispositivo haya acabado de leer se notificara que lo podemos volver a usar (similar a close)
    Device_Open--;

    module_put(THIS_MODULE);
    return 0;

}

//ssize se usa para representar el tamano de un bloque de memoria y representar el valor -1 (error)
ssize_t device_read(struct file * file, char * buffer, size_t length, loff_t *offset) {
    int bytes_read = 0;
    if (*msg_punt == 0) { //  si el mensaje es nada se devuelve nada (no hace nada)
        return 0;
    }
    
    while(length && *msg_punt) {
        put_user(* (msg_punt++), buffer++); // El buffer necesita ser enviado entre el proceso actual (el dispositivo) y el kernel, el kernel recibe un puntero del buffer 
                                            // que esta en el segmento del proceso. put_user se encarga de escribir un valor simple en el espacio del usuario
        length--;
        bytes_read++;
    }
    return bytes_read;
}
ssize_t device_write(struct file * file, const char * buffer, size_t length, loff_t *offset) {
    int contador = 0;
    memset(msg, 0, BUF_LEN);
    while(length > 0) {
        copy_from_user(msg, buffer, BUF_LEN - 1);
        contador++;
        length--;
        msg[BUF_LEN - 1] = 0x00; // necesario que la terminacion del caracter que se envia sea NULL para que no de problemas


    }
    
    printk(KERN_ALERT "HE ESCRITO \n");
    return contador; // SIEMPRE debe devolver algo
}

// al ejecutar insmod mirootkit.ko y luego echo -en "123456789\0" > /dev/mirootkit, si hacemos un cat de /dev/mirootkit obtendremos 1234567890

/* EN EL TERMINAL TRAS COMPILAR CON make all:
insmod -> insertar driver (usar archivo .ko tal que insmod -> insmod driver.ko)
o con parámetro: 

insmod mirootkit.ko 
lsmod  -> ver info del driver (lsmod | grep driver)  
rmmod  -> borrar driver (rmmod driver)

Los mensajes generados por el driver se solían guardar en var/log, pero en ubuntu 
se sitúan en /var/log/syslog
Para verlos ejecutamos  "tail -f /var/log/syslog"
sudo mknod /dev/mirootkit c 238 0
cat dev/mirootkit */