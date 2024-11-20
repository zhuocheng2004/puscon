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


/*
 * Constant Definitions
 */

#define		HOST_PID_MAX_SHIFT	22
#define		PID_MAX_SHIFT		16

#define		LOG_BUF_SIZE		4096

#define		LOG_SOH			"\001"
#define		LOG_SOH_ASCII		'\001'

#define		LOG_EMERG		LOG_SOH "0"
#define		LOG_ALERT		LOG_SOH "1"
#define		LOG_CRIT		LOG_SOH "2"
#define		LOG_ERR			LOG_SOH "3"
#define		LOG_WARNING		LOG_SOH "4"
#define		LOG_NOTICE		LOG_SOH "5"
#define		LOG_INFO		LOG_SOH "6"
#define		LOG_DEBUG		LOG_SOH "7"



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
extern int puscon_log_level;
extern int puscon_log_use_ansi_color;

/*
 * Function Declarations
 */

int puscon_main(puscon_config* config);

int puscon_syscall_handle(puscon_task_info* task);

int skip_syscall(puscon_task_info* task);

/*
 * execute the syscall by the child task
 */
int puscon_child_syscall6(puscon_task_info* task, u64* ret,
	u64 nr, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5);

int puscon_vlog(const char *fmt, va_list args);
int puscon_log(const char *fmt, ...);

static inline void* puscon_malloc(size_t size) {
	return malloc(size);
}

static inline void* puscon_zalloc(size_t size) {
	return calloc(1, size);
}

static inline void puscon_free(void* ptr) {
	free(ptr);
}

#endif
