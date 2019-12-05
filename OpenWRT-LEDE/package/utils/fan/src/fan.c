#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
        while(1){
                system("/sbin/fan.sh");
                sleep(10);
        }
        return 0;
}
