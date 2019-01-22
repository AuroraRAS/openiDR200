#include <iostream>
#include "idrusb.h"

int main()
{
    IDRUSB idrusb;

    idrusb.samStatus();
    idrusb.findCard();
    idrusb.selectCard();
    idrusb.readCard();

    return 0;
}
