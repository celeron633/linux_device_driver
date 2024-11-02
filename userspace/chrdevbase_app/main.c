#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <string.h>

enum CHRDEVBASE_IOCTL_CMD {
    TEST_CMD_1 = 1,
    TEST_CMD_2,
    TEST_CMD_3,
    TEST_CMD_4,
    TEST_CMD_5
};

#define CHRDEVBASE_NAME "chrdevbase"
#define DEV_PATH "/dev/"CHRDEVBASE_NAME

int main(int argc, char *argv[])
{
    int ret = 0;
    int ioctl_cmd = -1;

    if (argc < 2) {
        printf("%s <cmd>\r\n", argv[0]);
        return -1;
    }
    ioctl_cmd = atoi(argv[1]);
    printf("cmd: %d\r\n", ioctl_cmd);

    int ioctl_cmd_para = -1;
    if (argc >= 3) {
        ioctl_cmd_para = atoi(argv[2]);
    }
    printf("ioctl cmd para: %d\r\n", ioctl_cmd_para);

    int fd = open(DEV_PATH, O_RDWR);
    if (fd < 0) {
        printf("open %s failed!\r\n", DEV_PATH);
        return -1;
    }

    if (ioctl(fd, ioctl_cmd, ioctl_cmd_para) < 0) {
        printf("ioctl failed\r\n");
        return -1;
    }

    return 0;
}