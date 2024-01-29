#include <linux/module.h> // acceso a llamadas del kernel
#include <linux/kernel.h> // mensajes debug
#include <linux/init.h>  // para facilitar con macros
#include <linux/moduleparam.h> // esto se importa para tener acceso a los modulos de
                                // parametro (sus funciones)
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


int init_module(void);
int device_open(struct inode * inode, struct file *file);
int device_release(struct inode * inode, struct file *file);
ssize_t device_read(struct file *file, char * buffer, size_t length, loff_t *offset);
ssize_t device_write(struct file * file, const char * buffer, size_t length, loff_t *offset);

#define EXITO 0
#define DEVICE_NAME "mirootkit"
#define BUF_LEN 80
extern int Major;