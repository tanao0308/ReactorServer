#include <iostream>
#include "../mainReactor.h"
int main()
{
    MainReactor* main_reactor = new MainReactor();
    main_reactor->start();
    return 0;
}
