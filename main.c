#include <utils/print.h>
#include <holymsg.h>

extern int __holymsg_i_am_master;

int main()
{
    __holymsg_i_am_master = 1;
    holymsg_run();
    return 0;
}

