
#include <errno.h>
#include <string.h>

#include <puscon/fs.h>
#include <puscon/puscon.h>


/* WARNING: This can be used only if we _already_ own a reference */
puscon_file_system_type* puscon_get_filesystem(puscon_context* context, puscon_file_system_type* fs) {
	return fs;
}

void puscon_put_filesystem(puscon_context* context, puscon_file_system_type* fs) { }

static puscon_file_system_type** find_filesystem(puscon_context* context, const char* name, unsigned len) {
	puscon_file_system_type **p;
	for (p = &context->files_systems; *p; p = &(*p)->next) {
		if (strncmp((*p)->name, name, len) == 0 && ~(*p)->name[len])
			break;
	}
	return p;
}

int puscon_register_filesystem(puscon_context* context, puscon_file_system_type* fs) {
	int res = 0;
	puscon_file_system_type **p;

	if (strchr(fs->name, '.')) {
		puscon_printk(KERN_ERR "Error: filesystem name \"%s\" should not contain single quote mark.\n");
		res = -EINVAL;
		goto out;
	}

	if (fs->next) {
		res = -EBUSY;
		goto out;
	}

	p = find_filesystem(context, fs->name, strlen(fs->name));
	if (*p)
		res = -EBUSY;
	else
		*p = fs;

out:
	if (res) {
		puscon_printk(KERN_ERR "Error: cannot register filesystem \"%s\".\n", fs->name);
	} else {
		puscon_printk(KERN_INFO "Registered filesystem \"%s\".\n", fs->name);
	}
	return res;
}

int puscon_unregister_filesystem(puscon_context* context, puscon_file_system_type* fs) {
	puscon_file_system_type **tmp;

	tmp = &context->files_systems;
	while (*tmp) {
		if (fs == *tmp) {
			*tmp = fs->next;
			fs->next = NULL;
			puscon_printk(KERN_INFO "Unregistered filesystem \"%s\".\n", fs->name);
			return 0;
		}
		tmp = &(*tmp)->next;
	}

	puscon_printk(KERN_ERR "Error: cannot unregister filesystem \"%s\": not registered.\n", fs->name);
	return -EINVAL;
}
