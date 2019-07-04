#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include "../test_ioctl.h"

#define DEVFILE "/proc/rtl8117-cmac/cmac_enabled"

struct rtl_dash_ioctl_struct {
        unsigned int   type;
        unsigned int   offset;
        unsigned int   len;
        char data_buffer[100];
	unsigned char  *data_buffer2;
};


#define RTLOOB_IOC_MAGIC 0x96

/* ▒~B▒▒~A▒~Z~D▒~K~U▒~\ */
#define IOCTL_SEND      _IOW(RTLOOB_IOC_MAGIC, 0, struct rtl_dash_ioctl_struct)
#define IOCTL_RECV      _IOR(RTLOOB_IOC_MAGIC, 1, struct rtl_dash_ioctl_struct)



int main(void)
{
 struct rtl_dash_ioctl_struct cmd;
        int fd,i;
 long ret;
 int num = 0;

unsigned char ttt;

        fd = open(DEVFILE, O_RDWR);
        if (fd == -1)
                perror("open");

	memset(&cmd, 0, sizeof(cmd));
	cmd.data_buffer2 =(unsigned char*) malloc(100+1);
	
	if(cmd.data_buffer2 == NULL)
		return;


	printf("%x %x",cmd.data_buffer,cmd.data_buffer2);

	//ttt = cmd.data_buffer[6];


        ret = ioctl(fd, IOCTL_RECV, &cmd);
	if (ret == -1) {
                printf("errno %d\n", errno);
                perror("ioctl");
        }
	
        printf("val %x\n", cmd.len);
	for (i=0; i<cmd.len;i++)
		printf("%x ",*(cmd.data_buffer2+i));	
	
	ttt=*(cmd.data_buffer2+4);

	printf("\n");
	
	sleep(1);

        cmd.len = 0x8;
	//for (i=0; i<cmd.len;i++)
	//	cmd.data_buffer[i]=i;
	 cmd.data_buffer[0]=0x00;
	 cmd.data_buffer[1]=0x00;
	cmd.data_buffer[2]=0x00;
	cmd.data_buffer[3]=0x00;
	cmd.data_buffer[4]=0x02;
	cmd.data_buffer[5]=0x00;
	cmd.data_buffer[6]=0x92;
	cmd.data_buffer[7]=0x00;

        cmd.data_buffer[8]=0x12;
        cmd.data_buffer[9]=0x00;
        cmd.data_buffer[10]=0x00;
        cmd.data_buffer[11]=0x00;
	
       cmd.data_buffer[12]=0x34;
        cmd.data_buffer[13]=0x00;
        cmd.data_buffer[14]=0x00;
        cmd.data_buffer[15]=0x00;

       cmd.data_buffer[16]=0x56;
        cmd.data_buffer[17]=0x00;
        cmd.data_buffer[18]=0x00;
        cmd.data_buffer[19]=0x00;

       cmd.data_buffer[20]=0x78;
        cmd.data_buffer[21]=0x00;
        cmd.data_buffer[22]=0x00;
        cmd.data_buffer[23]=0x00;

       cmd.data_buffer[24]=0xab;
        cmd.data_buffer[25]=0x00;
        cmd.data_buffer[26]=0x00;
        cmd.data_buffer[27]=0x00;
	

	memcpy(cmd.data_buffer2,cmd.data_buffer,cmd.len);
        ret = ioctl(fd, IOCTL_SEND, &cmd);
        if (ret == -1) {
                printf("errno %d\n", errno);
                perror("ioctl");
        }

	//sleep(1);
      cmd.len = 0x9;
        //for (i=0; i<cmd.len;i++)
        //      cmd.data_buffer[i]=i;
         cmd.data_buffer[0]=0x01;
         cmd.data_buffer[1]=0x00;
        cmd.data_buffer[2]=0x00;
        cmd.data_buffer[3]=0x00;
        cmd.data_buffer[4]=ttt;
        cmd.data_buffer[5]=0x00;
        cmd.data_buffer[6]=0x91;
        cmd.data_buffer[7]=0x00;

        cmd.data_buffer[8]=0x00;

memcpy(cmd.data_buffer2,cmd.data_buffer,cmd.len);

	 ret = ioctl(fd, IOCTL_SEND, &cmd);

        if (ret == -1) {
                printf("errno %d\n", errno);
                perror("ioctl");
        }
        printf("val %x\n", cmd.len);

//        for (i=0; i<cmd.len;i++)
 //               printf("%x",cmd.data_buffer2+i);

        printf("\n");

        if (close(fd) != 0)
                perror("close");

        return 0;
}
