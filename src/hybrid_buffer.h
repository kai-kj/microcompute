#ifndef TRANSFER_BUFFER_H
#define TRANSFER_BUFFER_H

#include "microcompute_extra.h"

struct mc_HBuffer {
    mc_Buffer gpuBuff; // "superclass"
    mc_Instance* _instance;
    mc_Buffer* cpuBuff;
    mc_BufferCopier* copier;
};

#endif // TRANSFER_BUFFER_H