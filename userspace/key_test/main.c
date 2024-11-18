#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <string.h>

int key_fd = 0;

void close_key_file(void)
{
    if (key_fd > 0)
        close(key_fd);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char buf[12];

    atexit(close_key_file);
    
    key_fd = open("/dev/ebf-key", O_RDONLY);
    if (key_fd < 0) {
        perror("open ebf-key failed\r\n");
        return -1;
    }

    while (1)
    {
        ret = read(key_fd, buf, sizeof(buf));
        if (ret < 0) {
            printf("read failed\r\n");
            return -1;
        }
        printf("%s\r\n", buf);
    }
    

    return 0;
}