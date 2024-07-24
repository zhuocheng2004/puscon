
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <puscon/puscon.h>

int puscon_printk_level;

int puscon_printk_use_ansi_color = 0;

static char log_buf[PRINTK_BUF_SIZE];


int puscon_vprintk(const char *fmt, va_list args) {
	int n = vsnprintf(log_buf, PRINTK_BUF_SIZE - 1, fmt, args);
	log_buf[n] = '\0';

	char *buf = log_buf;
	int log_level = -1;
	if (log_buf[0] == KERN_SOH_ASCII) {
		buf++;
		if ('0' <= log_buf[1] && log_buf[1] <= '7') {
			log_level = log_buf[1] - '0';
			buf++;
		}
	}

	if (log_level <= puscon_printk_level) {
		if (puscon_printk_use_ansi_color) {
			switch (log_level) {
				case 0:
				case 1:
				case 2:
				case 3:
					fputs("\033[91m", stderr); break;
					break;
				case 4:
					fputs("\033[93m", stderr); break;
					break;
				case 5:
				case 6:
					fputs("\033[36m", stderr); break;
					break;
				case 7:
					fputs("\033[34m", stderr); break;
					break;
				default:
					fputs("\033[0m", stderr); break;
					break;
			}
		}

		fputs(buf, stderr);

		if (puscon_printk_use_ansi_color) {
			fputs("\033[0m", stderr);
		}
	}
}

int puscon_printk(const char *fmt, ...) {
	va_list args;
	int n;

	va_start(args, fmt);
	n = puscon_vprintk(fmt, args);
	va_end(args);

	return n;
}

