#ifndef _X509DEC_H_
#define _X509DEC_H_

#include <sys/queue.h>

TAILQ_HEAD(tailq, name);

struct name {
	int name_type;
	int name_len;
	unsigned char *name_buf;
	TAILQ_ENTRY(name) names;
};
#endif	/*  _X509DEC_H_*/
