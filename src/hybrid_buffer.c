#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "device.h"
#include "hybrid_buffer.h"
#include "log.h"

mce_HBuffer* mce_hybrid_buffer_create(mc_Device* device, uint64_t size) {
    mce_HBuffer* hBuffer = malloc(sizeof *hBuffer);

    *hBuffer = (mce_HBuffer){
        ._instance = device->_instance,
        .gpuBuff = {0},
        .cpuBuff = NULL,
        .copier = NULL,
    };

    DEBUG(hBuffer, "Creating hybrid buffer of size %lu", size);

    mc_Buffer* gpuBuffer = mc_buffer_create(device, MC_BUFFER_TYPE_GPU, size);
    if (!gpuBuffer) {
        mce_hybrid_buffer_destroy(hBuffer);
        return NULL;
    }

    memcpy(&hBuffer->gpuBuff, gpuBuffer, sizeof *gpuBuffer);
    free(gpuBuffer);

    hBuffer->cpuBuff = mc_buffer_create(device, MC_BUFFER_TYPE_CPU, size);
    if (!hBuffer->cpuBuff) {
        mce_hybrid_buffer_destroy(hBuffer);
        return NULL;
    }

    hBuffer->copier = mc_buffer_copier_create(device);
    if (!hBuffer->copier) {
        mce_hybrid_buffer_destroy(hBuffer);
        return NULL;
    }

    return hBuffer;
}

mce_HBuffer* mce_hybrid_buffer_create_from(
    mc_Device* device,
    uint64_t size,
    void* data
) {
    struct mce_HBuffer* hBuffer = mce_hybrid_buffer_create(device, size);
    if (!hBuffer) return NULL;
    mce_hybrid_buffer_write(hBuffer, 0, size, data);
    return hBuffer;
}

void mce_hybrid_buffer_destroy(mce_HBuffer* hBuffer) {
    if (!hBuffer) return;
    DEBUG(hBuffer, "destroying hybrid buffer");

    if (hBuffer->gpuBuff._instance) {
        mc_Buffer* gpuBuffer = malloc(sizeof *gpuBuffer);
        memcpy(gpuBuffer, &hBuffer->gpuBuff, sizeof *gpuBuffer);
        mc_buffer_destroy(gpuBuffer);
    }
    if (hBuffer->cpuBuff) mc_buffer_destroy(hBuffer->cpuBuff);
    if (hBuffer->copier) mc_buffer_copier_destroy(hBuffer->copier);
    free(hBuffer);
}

uint64_t mce_hybrid_buffer_get_size(mce_HBuffer* hBuffer) {
    return mc_buffer_get_size(&hBuffer->gpuBuff);
}

uint64_t mce_hybrid_buffer_write(
    mce_HBuffer* hBuffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!hBuffer) return 0;
    DEBUG(hBuffer, "writing %ld bytes to hybrid buffer", size);

    uint64_t res = mc_buffer_write(hBuffer->cpuBuff, offset, size, data);
    if (res != size) return res;

    return mc_buffer_copier_copy(
        hBuffer->copier,
        hBuffer->cpuBuff,
        &hBuffer->gpuBuff,
        offset,
        offset,
        size
    );
}

uint64_t mce_hybrid_buffer_read(
    mce_HBuffer* hBuffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!hBuffer) return 0;
    DEBUG(hBuffer, "reading %ld bytes from hybrid buffer", size);

    uint64_t res = mc_buffer_copier_copy(
        hBuffer->copier,
        &hBuffer->gpuBuff,
        hBuffer->cpuBuff,
        offset,
        offset,
        size
    );
    if (res != size) return res;

    return mc_buffer_read(hBuffer->cpuBuff, offset, size, data);
}
