#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int write_offset(const char *device, long offset, void *data, int size)
{
    int fd = open(device, O_RDWR);

    if (fd < 0)
    {
        printf("Disk open failed\n");
        return -1;
    }

    if (pwrite(fd, data, size, offset) < 0)
    {
        printf("Write failed\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
