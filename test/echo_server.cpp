#include <iostream>
#include "../include/MainReactor.h"
int main()
{
    MainReactor* main_reactor = new MainReactor(5);
    main_reactor->start();
    return 0;
}
