#include <Diversinet/DiversinetInterface.h>

int main()
{
    Diversinet::Interface::DiversinetInterface model;
    model.setLambda(1.0);
    model.setMu(0.1);
    model.setKMaxInt(16);
    return 0;
}
