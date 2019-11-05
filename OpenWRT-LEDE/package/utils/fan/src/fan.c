#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include <pthread.h>

#define CMD_RESULT_LEN 256
#define fan_start_threshold 55
#define fan_stop_threshold 50
#define fan_start_powerful_threshold 70

/*
 * 功能：日誌
 * 注意：日誌位置：tmp/fan_log
 */

int log_count = 0; //日誌超過20行則清空，起始為0
static void show_sys_info(char *str0)
{
	
	char str1[] = "echo $(date) ";
	char str2[] = "  >> tmp/fan_log";
//	printf("%s\n", str0);
	strcat(str1, str0);
	strcat(str1, str2);
	system(str1); //echo $(date) 訊息 >> tmp/fan_log

        //日誌超過20行則清空
        log_count = log_count + 1 ;
        if (log_count >=20)
        {
                log_count=0;
                system("echo "" > tmp/fan_log");
        }
}



/*
 * 功能：執行系統命令，並獲取命令的返回數據
 * 參數：cmd 要執行的系統命令，result 為接收命令返回的數據
 * 返回值：成功返回0，失敗返回-1
 * 注意：執行的命令和返回的數據最大為CMD_RESULT_LEN
 */
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


/*
 * 功能：獲取系統cpu溫度
 * 參數：無
 * 返回值：成功返回ret，失敗返回-1
 */
static int sys_cpu_temp()
{
        char buff[CMD_RESULT_LEN] = {0};
        if (execute_cmd_result("cat /sys/class/thermal/thermal_zone0/temp", buff) < 0)
        {
                show_sys_info("failed to open thermal_zone0/temp！");
                return -1;
        }
        double ret = atoi(buff)/1000.0;
        return (int)ret;
}


/*
 * 功能：控制PWM輸出，並控制風扇
 * 參數：命令字串
 * 返回值：成功返回0，失敗返回-1
 */
static int sys_ctrl_pwm(const char *pwm_cmd)
{
        char buff[CMD_RESULT_LEN] = {0};
        if (execute_cmd_result(pwm_cmd, buff) < 0)
        {
                show_sys_info("failed to config PWM parameter in sysfs");
                return -1;
        }
        return 0;
}




static void* fan_server_th(void *arg)
{
        show_sys_info("Fan management thread started successfully！");
        sys_ctrl_pwm("echo 1 > /sys/devices/platform/980070d0.pwm/clksrcDiv0"); //重新設定clock

        /*設置狀態表示符,避免頻繁讀寫sysfs產生日誌*/
        short int state_mode_prev;
        short int state_mode_now;
        while(1)
        {        while(1)
                {
                        

                        int temp = sys_cpu_temp(); //獲取CPU溫度
                        char temp_str[10];
                        sprintf(temp_str, "%d", temp);//將獲取溫度整數轉字串
                        show_sys_info(temp_str);//將獲取溫度存入日誌
                        sleep(2);
                        
                        if (temp >= fan_start_threshold && temp <= fan_start_powerful_threshold)
                        {
                                show_sys_info("turn on fan (dutycycle=50)");
                                state_mode_now= 1;
                                if (state_mode_now == state_mode_prev)//狀態不變直接跳出第一層while
                                        break;
                                sys_ctrl_pwm("echo 1 > /sys/devices/platform/980070d0.pwm/enable0");
                                sys_ctrl_pwm("echo 50 > /sys/devices/platform/980070d0.pwm/dutyRate0");
                                state_mode_prev = 1;
                                
                        }
                        if (temp <= fan_stop_threshold)
                        {
                                show_sys_info("turn off fan");
                                state_mode_now= 2;
                                if (state_mode_now == state_mode_prev)//狀態不變直接跳出第一層while
                                        break;
                                sys_ctrl_pwm("echo 0 > /sys/devices/platform/980070d0.pwm/enable0");
                                state_mode_prev = 2;
                                
                        }
                        if (temp >= fan_start_powerful_threshold)
                        {
                                show_sys_info("turn on fan (dutycycle=100)");
                                state_mode_now= 3;
                                if (state_mode_now == state_mode_prev)//狀態不變直接跳出第一層while
                                        break;
                                sys_ctrl_pwm("echo 1 > /sys/devices/platform/980070d0.pwm/enable0");
                                sys_ctrl_pwm("echo 100 > /sys/devices/platform/980070d0.pwm/dutyRate0");
                                state_mode_prev = 3;
                        }
                }
        }

        return (void*)0;
}



int main(int argc, char* argv[])
{

        

        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_t th;
        int err_h;
        if (err_h = pthread_create(&th, &attr, fan_server_th, (void*)0) != 0)
        {
                perror("fan_server_th pthread_create error!");
                pthread_attr_destroy(&attr); //銷毀線程屬性結構體
                return ;
        }

        pthread_attr_destroy(&attr); //銷毀線程屬性結構體 





        while(1)
        {        

                sleep(2);
                
        }
        
	return 0; 
}

