#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>

int usage(void)
{
	printf("\n");	
	printf(" Usage: usbctl [-xrcdh] [DEVPATH] \n\n");
	printf("    -x     reset\n");
	printf("    -r     read loop\n");
	printf("    -c     connect\n");
	printf("    -d     disconnect\n");
	printf("    -h     usage\n");
	printf("\n");
	return 0;
}

void usbctl_read(int dev)
{
	fd_set fds, fde;
	char buf[512];
	int ret;

	while (1) {
		FD_ZERO(&fds);
		FD_ZERO(&fde);
		FD_SET(dev, &fds);
		FD_SET(dev, &fde);
		if ((ret = select(dev + 1, &fds, NULL, &fde, NULL)) > 0) {
			if (FD_ISSET(dev, &fde)) {
				printf("except\n");
				return;
			}
			if (FD_ISSET(dev, &fds))
				read(dev, buf, sizeof(buf));
		} else {
			printf("error=%d\n",ret);
			return;
		}
	}
}

int main(int argc, char *argv[])
{
	int ret, dev, c;
	char *path;
	int ioctl_no;
	void *arg = 0;
	struct usbdevfs_ioctl data;

	if (argc <= 1)
		return usage();

	while ((c = getopt(argc, argv, "d:c:r:x:h")) != -1) {
		switch (c) {
		case 'x':
			ioctl_no = USBDEVFS_RESET;
			path = optarg;
			break;
		case 'c':
			//ioctl_no = USBDEVFS_CONNECT;
			ioctl_no = USBDEVFS_IOCTL;
			arg = (void *)&data;
			data.ifno = 0;
			data.ioctl_code = USBDEVFS_CONNECT;
			data.data = (void *) NULL;
			path = optarg;
			break;
		case 'd':
			//ioctl_no = USBDEVFS_CONNECT;
			ioctl_no = USBDEVFS_IOCTL;
			arg = (void *)&data;
			data.ifno = 0;
			data.ioctl_code = USBDEVFS_DISCONNECT;
			data.data = (void *) NULL;
			path = optarg;
			break;
		case 'r':
			ioctl_no = 0;
			path = optarg;
			break;
	
		case 'h':
		case '?':
			return usage();
		default:
			break;
		}
	}

	dev = open(path, O_RDWR);

	if (dev < 0) {
		printf("dev open failed\n");
		return -ENODEV;
	}

	if (!ioctl_no) {
		printf("READ %s forever ...\n", path);
		usbctl_read(dev);
	}

	ret = ioctl(dev, ioctl_no, arg);
	printf("usbdev '%s' ioctl=%x ret=%d ... ", path, ioctl_no, ret);

	if (ret >= 0)
		printf("OK\n");
	else
		printf("FAILED\n");

	close(dev);

	return 0;
}
