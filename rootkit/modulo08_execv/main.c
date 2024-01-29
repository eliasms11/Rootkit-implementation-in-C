#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/kern_levels.h>
#include <linux/gfp.h>
#include <asm/unistd.h>
#include <asm/paravirt.h>


#include <linux/binfmts.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Elias");
MODULE_DESCRIPTION("Enganche (hook) de la llamada al sistema execv");
MODULE_VERSION("1.0");


unsigned long **SYS_CALL_TABLE;


/* RESULTA QUE LINUX KERNEL 5.3 SOLUCIONA LA VULNERABILIDAD
DE LA ESCRITURA DE LA ORDEN CR0 HACIENDO QUE SE REGISTRE SOLO LECTURA.
EL CODIGO QUE APARECE ABAJO FUNCIONA ESTE PROBLEMA ESCRIBIENDO DIRECTAMENTE
EN LAS PAGINAS DE MEMORIA USANDO SUS PUNTEROS PARA ACCEDER A ELLAS */
void EnablePageWriting(unsigned long address){
	write_cr0(read_cr0() & (~0x10000));
	/* unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	if(pte->pte &~ _PAGE_RW) {
		pte->pte != _PAGE_RW;
	}*/

} 
void DisablePageWriting(unsigned long address){
	write_cr0(read_cr0() | 0x10000);
	/*unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	pte->pte = pte->pte &~ _PAGE_RW; */
} 


char char_buffer[255] = {0};
// NO LLAMAR A LAS VARIABLES DE FORMA SIMILAR, SOBRETODO LAS GLOBALES
// argc <-> argz <-> argv se diferencian en un caracter.
char argz[255][255] = {0};
// conteo de los argumenos
size_t argc = 0;

char CharBuffer [255] = {'\0'};
char Argz       [255] = {'\0'};;


unsigned int RealCount = 0;

/* desde: /usr/src/linux-headers-$(uname -r)/include/linux/syscalls.h */




asmlinkage int (*original_execve)(const char *filename, char *const argv[], char *const envp[]);
asmlinkage int HookExecve(const char *filename, char *const argv[], char *const envp[]) {

      copy_from_user(&CharBuffer , filename , strnlen_user(filename , sizeof(CharBuffer) - 1  ) );
      printk( KERN_INFO "Nombre ejecutable %s  \n", CharBuffer );

			char * ptr = 0xF00D; 

      // Como no sabemos la cantidad de args iremos hasta 0 y tendremos 20 args como maximo

		for (int i = 0 ; i < 20 ; i++){ 
        if(ptr){
	
         int success =  copy_from_user(&ptr, &argv[i], sizeof(ptr));
         // Comprobacion de que ptr sea 0x000 
         if(success == 0 && ptr){
			RealCount ++;
			
            //printk( KERN_INFO "Nombre del puntero %px  \n", ptr );
            strncpy_from_user(Argz, ptr , sizeof(Argz));
            printk( KERN_INFO "Args  %s  \n", Argz );
            memset(Argz, 0 ,sizeof(Argz));



         }
    }
}
		printk("RealCount %d\n", RealCount);
		RealCount = 0;
		argc = RealCount + 1;


		// for(int i = 0 ; i < argc; i++){
			// Insertar ARGS aqui

		// }




  return (*original_execve)(filename, argv, envp);
}


asmlinkage int (*original_read)(unsigned int, void __user*, size_t);
asmlinkage int  HookRead(unsigned int fd, void __user* buf, size_t count) {
	return (*original_read)(fd, buf, count);
}


static int __init SetHooks(void) {
	// Conseguimos el puntero de la tabla de llamadas al sistema = Syscall Table 
 	//SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table"); 
	SYS_CALL_TABLE = (unsigned long**)__symbol_get("sys_call_table"); 

	printk(KERN_INFO "Los ganchos estaran listos.\n");
	printk(KERN_INFO "System call table en %p\n", SYS_CALL_TABLE);


	EnablePageWriting( ( unsigned long )SYS_CALL_TABLE );

	
	original_read = (void*)SYS_CALL_TABLE[__NR_read];
	SYS_CALL_TABLE[__NR_read] = (unsigned long*)HookRead;

	original_execve = (void*)SYS_CALL_TABLE[__NR_execve];
	SYS_CALL_TABLE[__NR_execve] = (unsigned long*)HookExecve;
	
	DisablePageWriting( ( unsigned long )SYS_CALL_TABLE );

	return 0;
}

static void __exit HookCleanup(void) {

	// Limpieza de hooks
	EnablePageWriting( ( unsigned long )SYS_CALL_TABLE );
	SYS_CALL_TABLE[__NR_read]   = (unsigned long*)original_read;
	SYS_CALL_TABLE[__NR_execve] = (unsigned long*)original_execve;
	DisablePageWriting( ( unsigned long )SYS_CALL_TABLE );

	printk(KERN_INFO "Ganchos limpios!");
}

module_init(SetHooks);
module_exit(HookCleanup);