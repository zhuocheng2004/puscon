#ifndef PUSCON_PUSCON_H
#define PUSCON_PUSCON_H

#include <stdarg.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

#include <puscon/defs.h>
#include <puscon/task.h>
#include <puscon/types.h>


struct puscon_file_system_type;


/*
 * Constant Definitions
 */

/* custom syscalls */
#define		SYS_puscon_base			0x1000
#define		SYS_puscon_nop			SYS_puscon_base + 0
#define		SYS_puscon_kernel_enter		SYS_puscon_base + 1
#define		SYS_puscon_kernel_exit		SYS_puscon_base + 2
#define		SYS_puscon_bypass_enable	SYS_puscon_base + 3
#define		SYS_puscon_bypass_disable	SYS_puscon_base + 4
#define		SYS_puscon_set_syscall_entry	SYS_puscon_base + 5

#define		HOST_PID_MAX_SHIFT	22
#define		PID_MAX_SHIFT		16

#define		PRINTK_BUF_SIZE		4096

#define		KERN_SOH		"\001"
#define		KERN_SOH_ASCII		'\001'

#define		KERN_EMERG		KERN_SOH "0"
#define		KERN_ALERT		KERN_SOH "1"
#define		KERN_CRIT		KERN_SOH "2"
#define		KERN_ERR		KERN_SOH "3"
#define		KERN_WARNING		KERN_SOH "4"
#define		KERN_NOTICE		KERN_SOH "5"
#define		KERN_INFO		KERN_SOH "6"
#define		KERN_DEBUG		KERN_SOH "7"


#define	S_IFMT		0170000	/* These bits determine file type.  */

/* File types.  */
#define	S_IFDIR		0040000	/* Directory.  */
#define	S_IFCHR		0020000	/* Character device.  */
#define	S_IFBLK		0060000	/* Block device.  */
#define	S_IFREG		0100000	/* Regular file.  */
#define	S_IFIFO		0010000	/* FIFO.  */
#define	S_IFLNK		0120000	/* Symbolic link.  */
#define	S_IFSOCK	0140000	/* Socket.  */

#define PATH_MAX	4096


/*
 * Type Definitions
 */

/*
 * config to run puscon main function
 */
typedef struct puscon_config {
	/* filename of the helper kernel */
	char*		kernel_filename;

	/* filename of the entry program */
	char*		entry_filename;

	int		entry_argc;

	char**		entry_argv;
} puscon_config;

/*
 * an instance of puscon simulation environment
 */
typedef struct puscon_context {
	puscon_config*	config;

	puscon_task_context	task_context;

	int		should_stop;
}  puscon_context;

/*
 * Global Variables
 */
extern int puscon_printk_level;
extern int puscon_printk_use_ansi_color;

/*
 * Function Declarations
 */

int puscon_main(puscon_config* config);

int puscon_syscall_handle(puscon_context* context);

int skip_syscall(puscon_context* context);

/*
 * execute the syscall by the child task
 */
int puscon_child_syscall6(puscon_context* context, u64* ret,
	u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5);

int puscon_vprintk(const char *fmt, va_list args);
int puscon_printk(const char *fmt, ...);

static inline void* puscon_kmalloc(size_t size) {
	return malloc(size);
}

static inline void* puscon_kzalloc(size_t size) {
	return calloc(1, size);
}

static inline void puscon_kfree(void* ptr) {
	free(ptr);
}

#endif
