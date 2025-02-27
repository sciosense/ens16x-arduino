#ifndef SCIOSENSE_ENS160_H
#define SCIOSENSE_ENS160_H

#include "ens16x.h"

class ENS160 : public ENS16x
{
public:
    ENS160() : ENS16x()
    {
        debugPrefix = "ENS160 debug -- ";
    }

    bool isConnected() override
    {
        return Ens160_IsConnected(this);
    }
};

#endif //SCIOSENSE_ENS160_H