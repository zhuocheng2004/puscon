#ifndef PUSCON_TYPES_H
#define PUSCON_TYPES_H

typedef char			bool;
#ifndef true
#define true	1
#endif
#ifndef false
#define false	0
#endif

typedef unsigned char		u8;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef char			s8;
typedef int			s32;
typedef long long		s64;


typedef struct list_head {
	struct list_head *next, *prev;
} list_head ;

typedef struct hlist_head {
	struct hlist_node *first;
} hlist_head;

typedef struct hlist_node {
	struct hlist_node *next, **pprev;
} hlist_node;

#endif
