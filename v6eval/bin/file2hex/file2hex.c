#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char *prog = NULL;

static void file2hex_dump(const unsigned char *const, int);
static int file2hex_return(const int, int, unsigned char *);
static void usage(void);

int
main(int argc, char **argv, char **envp)
{
	struct stat st;

	unsigned char *buf = NULL;
	int fd	= 0;

	memset(&st, 0, sizeof(st));

	if(!(prog = basename(argv[0]))) {
		fprintf(stderr,
			"basename: %s -- %s\n", strerror(errno), argv[0]);

		return(file2hex_return(-1, fd, buf));
		/* NOTREACHED */
	}

	{
		int ch = 0;

		while((ch = getopt(argc, argv, "")) != -1) {
			switch (ch) {
				default:
					usage();
					/* NOTREACHED */

					break;
			}
		}

		argc -= optind;
		argv += optind;
	}

	if(!argc) {
		usage();
		/* NOTREACHED */
	}

	if(stat(argv[0], &st)) {
		fprintf(stderr,
			"err: stat: %s -- %s\n", argv[0], strerror(errno));

		return(file2hex_return(-1, fd, buf));
		/* NOTREACHED */
	}

	if((fd = open(argv[0], O_RDONLY)) < 0) {
		fprintf(stderr,
			"err: open: %s -- %s\n", argv[0], strerror(errno));

		return(file2hex_return(-1, fd, buf));
		/* NOTREACHED */
	}

	if(!(buf = (unsigned char *)malloc(st.st_size))) {
		fprintf(stderr, "err: malloc: %s\n", strerror(errno));
		return(file2hex_return(-1, fd, buf));
		/* NOTREACHED */
	}

	if(read(fd, buf, st.st_size) != st.st_size) {
		fprintf(stderr, "err: read: %s\n", strerror(errno));
		return(file2hex_return(-1, fd, buf));
		/* NOTREACHED */
	}

	file2hex_dump(buf, st.st_size);

	return(file2hex_return(0, fd, buf));
}



static void
file2hex_dump(const unsigned char *const buf, int buflen)
{
	int d = 0;

        printf("log:File2Hex_Results                (length:%d)\n", buflen);
        printf("log:| Data                            (length:%d)\n", buflen);
        printf("log:| | data                             = ");

	for(d = 0; d < buflen; d ++) {
		printf("%02x", buf[d]);
		/* NOTREACHED */
	}

	printf("\n");

	return;
}



static int
file2hex_return(const int code, int fd, unsigned char *buf)
{
	if(buf) {
		free(buf); buf = NULL;
	}

	if(fd) {
		close(fd); fd = 0;
	}

	return(code);
}



static void
usage(void)
{
	fprintf(stderr, "err: usage: %s file\n", prog);

	exit(-1);
	/* NOTREACHED */

	return;
}
