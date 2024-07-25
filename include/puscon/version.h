#ifndef PUSCON_VERSION_H
#define PUSCON_VERSION_H

#define PUSCON_VERSION_MAJOR	0
#define PUSCON_VERSION_MINOR	0
#define PUSCON_VERSION_PATCH	0

#define _STRINGIFY(x)	#x
#define STRINGIFY(x)	_STRINGIFY(x)

#define PUSCON_VERSION_STRING	STRINGIFY(PUSCON_VERSION_MAJOR) "." STRINGIFY(PUSCON_VERSION_MINOR) "." STRINGIFY(PUSCON_VERSION_PATCH)

#endif
