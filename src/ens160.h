#ifndef SCIOSENSE_ENS160_H
#define SCIOSENSE_ENS160_H

#include "ens16x.h"

namespace ScioSense
{
    class ENS160 : public ENS16x
    {
    public:
        ENS160() : ENS16x()
        {
            debugPrefix = "ENS160 debug -- ";
        }

        bool isConnected() override
        {
            debug("part id:", partId);
            return partId == 0x0160;
        }
    };
}

#endif //SCIOSENSE_ENS160_H