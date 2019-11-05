#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h> 
#include <pthread.h>
#include <math.h>

#define CMD_RESULT_LEN 256
#define fan_start_threshold 55
#define fan_stop_threshold 50
#define fan_start_powerful_threshold 70



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
 * 功能：LSADC數值換為溫度
 * 參數：lsadc 十進位數值
 * 返回值：成功返回0，失敗返回-1
 */
static double lsadc_formula(double lsadc_value)
{
        //printf("%lf-2\n",lsadc_value);
        double Voltage = lsadc_value/64*3.3;
        //printf("%lf-3\n",Voltage);
        double Resistor_Val = (4700*Voltage)/(3.3-Voltage);
        //printf("%lf-4\n",Resistor_Val);
        double thermal_val = pow(Resistor_Val,0.1605224);
        //double thermal_val = ((7846547/(Resistor_Val+1995))-1)/26;
        thermal_val =-140.834 + 22849.554/(1 + (thermal_val)/0.0320614851731001);
        //printf("%lf-5\n",thermal_val);
     
        return thermal_val;
}


/*
 * 功能：獲取系統lsadc溫度
 * 參數：無
 * 返回值：0
 */
static int sys_lsadc_temp()
{
        char buff[CMD_RESULT_LEN] = {0};
        if (execute_cmd_result("cat /sys/devices/platform/9801b00c.rtk-lsadc/info0 | grep pad0_adc | cut -c 13-14", buff) < 0)
        {
                //show_sys_info("failed to get rtk-lsadc value！");
                return -1;
        }
        int lsadc_value = strtol(buff,NULL,16);
        
        printf("lsadc_value=%d\n",lsadc_value);
        printf("Temperature=%lf\n",lsadc_formula(lsadc_value));
        //return lsadc_formula(lsadc_value);
        return 0;
}


/*
 * 功能：獲取系統soc溫度
 * 參數：無
 * 返回值：成功返回ret，失敗返回-1
 */
static int sys_soc_temp()
{
        char buff[CMD_RESULT_LEN] = {0};
        if (execute_cmd_result("cat /sys/class/thermal/thermal_zone0/temp", buff) < 0)
        {
                //show_sys_info("failed to open thermal_zone0/temp！");
                return -1;
        }
        double ret = atoi(buff)/1000.0;
        printf("Temperature=%lf\n", ret);
        return (int)ret;
}

/*
 * 功能：獲取系統soc溫度
 * 參數：無
 * 返回值：成功返回0，失敗返回-1
 */
static int sys_hdd_temp()
{
        char buff[CMD_RESULT_LEN] = {0};
        if (execute_cmd_result("smartctl -a /dev/sataa1 | grep Tem | grep 194 | cut -d _ -f 3 | cut -d - -f 2 | cut -d m -f 1", buff) < 0)
        {
                //show_sys_info("");
                return -1;
        }

        printf("Temperature=%s\n", buff);
        return 0;
}


/*
 * 功能：控制PWM輸出，設置lsadc閥值
 * 參數：命令字串
 * 返回值：成功返回0，失敗返回-1
 */
static int sys_ctrl_cmd(const char *pwm_cmd)
{
        char buff[CMD_RESULT_LEN] = {0};
        if (execute_cmd_result(pwm_cmd, buff) < 0)
        {
                //show_sys_info("failed to config PWM parameter in sysfs");
                return -1;
        }
        return 0;
}




static void* fan_server_th(void *arg)
{
        //show_sys_info("Fan management thread started successfully！");
        sys_ctrl_cmd("echo 1 > /sys/devices/platform/980070d0.pwm/clksrcDiv0"); //重新設定clock

        /*設置狀態表示符,避免頻繁讀寫sysfs產生日誌*/
        short int state_mode_prev;
        short int state_mode_now;

                        

                        int temp = sys_lsadc_temp(); //獲取CPU溫度
                        char temp_str[10];
                        sprintf(temp_str, "%d", temp);//將獲取溫度整數轉字串
                        //show_sys_info(temp_str);//將獲取溫度存入日誌
                        sleep(2);
                        
                        if (temp >= fan_start_threshold && temp <= fan_start_powerful_threshold)
                        {
                                //show_sys_info("turn on fan (dutycycle=50)");
                                state_mode_now= 1;
                                if (state_mode_now == state_mode_prev)//狀態不變直接跳出第一層while
                                        
                                sys_ctrl_cmd("echo 1 > /sys/devices/platform/980070d0.pwm/enable0");
                                sys_ctrl_cmd("echo 50 > /sys/devices/platform/980070d0.pwm/dutyRate0");
                                state_mode_prev = 1;
                                
                        }
                        if (temp <= fan_stop_threshold)
                        {
                                //show_sys_info("turn off fan");
                                state_mode_now= 2;
                                if (state_mode_now == state_mode_prev)//狀態不變直接跳出第一層while
                                        
                                sys_ctrl_cmd("echo 0 > /sys/devices/platform/980070d0.pwm/enable0");
                                state_mode_prev = 2;
                                
                        }
                        if (temp >= fan_start_powerful_threshold)
                        {
                                //show_sys_info("turn on fan (dutycycle=100)");
                                state_mode_now= 3;
                                if (state_mode_now == state_mode_prev)//狀態不變直接跳出第一層while
                                        
                                sys_ctrl_cmd("echo 1 > /sys/devices/platform/980070d0.pwm/enable0");
                                sys_ctrl_cmd("echo 100 > /sys/devices/platform/980070d0.pwm/dutyRate0");
                                state_mode_prev = 3;
                        }
                
        

        return (void*)0;
}




int main(int argc, char* argv[])
{


        if (argc < 2)
        {
		printf("Enter soc or hdd parameter \n");
		return 0;                  
        }
	/*
        if (strcmp(argv[1], "lsadc") == 0)
        {
            printf("lsadc\n");
            sys_ctrl_cmd("echo 1 > /sys/devices/platform/9801b00c.rtk-lsadc/threshold00");
        	int temp = sys_lsadc_temp();                    
        }

        else */ if (strcmp(argv[1], "soc") == 0) 
   		{
      		printf("soc\n");
      		int temp = sys_soc_temp();   
  		}
  		else if (strcmp(argv[1], "hdd") == 0) 
   		{
      		printf("hdd\n");
      		int temp = sys_hdd_temp();

  		}
   		else 
   		{
      		printf("1.soc 2.hdd\n");
   		}




/*        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_t th;
        int err_h;
        if (err_h = pthread_create(&th, &attr, fan_server_th, (void*)0) != 0)
        {
                perror("fan_server_th pthread_create error!");
                pthread_attr_destroy(&attr); //銷毀線程屬性結構體
                return -1;
        }

        pthread_attr_destroy(&attr); //銷毀線程屬性結構體 
*/
	return 0; 
}
