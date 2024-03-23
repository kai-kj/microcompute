#ifndef TRANSFER_BUFFER_H
#define TRANSFER_BUFFER_H

#include "microcompute_extra.h"

struct mce_HBuffer {
    mc_Buffer gpuBuffer; // so it can be cast to mc_Buffer
    mc_Instance* _instance;
    mc_Buffer* cpuBuffer;
    mc_BufferCopier* copier;
};

#endif // TRANSFER_BUFFER_H