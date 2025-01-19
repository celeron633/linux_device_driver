#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#define DEV_FILE_PATH "/dev/spi_max7219"

int main(int argc, char *argv[])
{
    int fp;
    int brightness;
    int ret;

    fp = open(DEV_FILE_PATH, O_WRONLY);
    if (fp < 0) {
        perror("open file failed\r\n");
        return -1;
    }

    while (1)
    {
        printf("enter brightness:");
        scanf("%d", &brightness);

        ret = ioctl(fp, 10, brightness);
        if (ret < 0) {
            perror("ioctl failed!\r\n");
            return -1;
        } else {
            printf("ret code: [%d], op ok!\r\n", ret);
        }
    }
    
    return 0;
}