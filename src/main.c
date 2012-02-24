#include "textmode.h"
#include "dt.h"
#include "paging.h"
#include "timer.h"
#include "kb.h"
#include "heap.h"
#include "system.h"
#include "vfs.h"
#include "multiboot.h"
#include "initrd.h"
#include "panic.h"
#include "task.h"
#include "syscall.h"
#include "speaker.h"

extern uint32_t placement_address;
uint32_t initial_esp;

void proc_a(void);
void proc_b(void);

int main(struct multiboot *mboot_ptr, uint32_t initial_stack)
{
    initial_esp = initial_stack;

    /* Setup all the ISRs and segmentation */
    init_dt();

    /* Setup the screen (by clearing it) */
    init_textmode();
    textmode_color(LIGHTGREEN, BLACK);
    puts("# GDT.................. OK\n");
    puts("# IDT.................. OK\n");
    puts("# Screen (text mode)... OK\n");

    /* Init the system timer */
    init_timer();
    puts("# Timer/Clock.......... OK\n");

    /* Init the keyboard */
    init_keyboard();
    puts("# Keyboard (US)........ OK\n");

    /* Find the location of our initial ramdisk which should have
    *  been loaded as a module by the boot loader */
    ASSERT(mboot_ptr->mods_count > 0);
    uint32_t initrd_location = *((uint32_t *)mboot_ptr->mods_addr);
    uint32_t initrd_end = *(uint32_t *)(mboot_ptr->mods_addr + 4);
    /* Don't trample our module with placement accesses, please! */
    placement_address = initrd_end;

    /* Initialise the initial ramdisk, and set it as the filesystem root */
    fs_root = init_initrd(initrd_location);

    /* Setup paging */
    init_paging();
    puts("# Paging............... OK\n");

    /* Setup multitasking */
/*    init_tasking();
    puts("# Multitasking......... OK\n");
*/
    textmode_color(LIGHTRED, BLACK);

    /* Check if the system really works */
    puts("# Testing interrupts...\n");
    __asm__ __volatile__("int $0x0");
    __asm__ __volatile__("int $0x3");
    /* Important! Re-enable interrupt requests */
    sti();

    /* Check if something went wrong with paging and heap setup */
    puts("# Testing the heap.....\n");
    void *a = (void *) kmalloc(8);
    void *b = (void *) kmalloc(8);
    void *c = (void *) kmalloc(8);
    kprintf("a: 0x%x\n", a);
    kprintf("b: 0x%x\n", b);
    kprintf("c: 0x%x\n", c);
    kfree(c);
    kfree(b);
    void *d = (void *) kmalloc(12);
    kprintf("d: 0x%x\n", d);
    kfree(a);
    kfree(d);

    /* Try to read files from the ramdisk image */
    puts("# Testing initrd.img...\n");
    /* list the contents of / */
    int32_t i = 0;
    struct dirent *node = 0;
    while ( (node = readdir_fs(fs_root, i)) != 0)
    {
	kprintf("Found file %s", node->name);
	fs_node_t *fsnode = finddir_fs(fs_root, node->name);
	if ((fsnode->flags & 0x7) == FS_DIRECTORY)
	    puts("\n\t(directory)\n");
	else
	{
	    puts("\n\tcontents: \"");
	    uint8_t buf[256];
	    uint32_t sz = read_fs(fsnode, 0, 256, buf);
	    uint32_t j;
	    for (j = 0; j < sz; j++)
		putc(buf[j]);
	    puts("\"\n");
	}
	i++;
    }

    textmode_color(LIGHTGREEN, BLACK);

    /* Setup syscalls */
    init_syscalls();
    puts("# Syscalls............. OK\n");

    /* Switch to user mode */
/*    init_usermode();
    syscall_puts("# Usermode..............OK\n");
*/
    init_shell();

    return 0;
}


/* Execute these functions with int32_t child_pid = task_init(&proc_a); */
void proc_a() {
    for(;;) putc('>');
}

void proc_b() {
    for(;;) putc('<');
}
