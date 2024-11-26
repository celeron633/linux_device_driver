#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <string.h>

int list1_fd = 0;

void close_list1_file(void)
{
    if (list1_fd > 0)
        close(list1_fd);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char buf[12];

    atexit(close_list1_file);
    
    list1_fd = open("/dev/list1", O_RDONLY);
    if (list1_fd < 0) {
        perror("open list1 failed\r\n");
        return -1;
    }

    ioctl(list1_fd, 3, 100);
    ioctl(list1_fd, 4, 200);

    return 0;
}