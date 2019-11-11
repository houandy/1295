#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>

#define CMD_RESULT_LEN 256

static int execute_cmd_result(const char *cmd, char *result)
{       
        if (cmd == NULL || result == NULL || strlen(cmd) <= 0)
                return -1;
        char buf_ps[CMD_RESULT_LEN] = {0};
        char ps[CMD_RESULT_LEN] = {0};
        FILE *ptr; 
        strcpy(ps, cmd);
        if((ptr=popen(ps, "r"))!=NULL)
        {       
                while(fgets(buf_ps, CMD_RESULT_LEN, ptr)!=NULL)
                {       
                        strcat(result, buf_ps);
                        if(strlen(result)>CMD_RESULT_LEN)
                                break;
                }
                pclose(ptr);
                ptr = NULL;
        }
        else
        {       
                printf("error: [sys.c]popen %s error\n", ps);
                return -1;
        }
        return 0;
}

int main(int argc, char* argv[])
{
        char buff[CMD_RESULT_LEN] = {0};
        while(1){
                execute_cmd_result("/sbin/fan.sh", buff);
                sleep(5);
        }
	return 0; 
}

