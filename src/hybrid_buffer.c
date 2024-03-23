#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "device.h"
#include "hybrid_buffer.h"
#include "log.h"

mce_HBuffer* mce_hybrid_buffer_create(mc_Device* device, uint64_t size) {
    mce_HBuffer* tBuffer = malloc(sizeof *tBuffer);

    *tBuffer = (mce_HBuffer){
        ._instance = device->_instance,
        .gpuBuffer = {0},
        .cpuBuffer = NULL,
        .copier = NULL,
    };

    DEBUG(tBuffer, "Creating hybrid buffer of size %lu", size);

    mc_Buffer* gpuBuffer = mc_buffer_create(device, MC_BUFFER_TYPE_GPU, size);
    if (!gpuBuffer) {
        mce_hybrid_buffer_destroy(tBuffer);
        return NULL;
    }

    memcpy(&tBuffer->gpuBuffer, gpuBuffer, sizeof *gpuBuffer);
    free(gpuBuffer);

    tBuffer->cpuBuffer = mc_buffer_create(device, MC_BUFFER_TYPE_CPU, size);
    if (!tBuffer->cpuBuffer) {
        mce_hybrid_buffer_destroy(tBuffer);
        return NULL;
    }

    tBuffer->copier = mc_buffer_copier_create(device);
    if (!tBuffer->copier) {
        mce_hybrid_buffer_destroy(tBuffer);
        return NULL;
    }

    return tBuffer;
}

mce_HBuffer* mce_hybrid_buffer_create_from(
    mc_Device* device,
    uint64_t size,
    void* data
) {
    struct mce_HBuffer* tBuffer = mce_hybrid_buffer_create(device, size);
    if (!tBuffer) return NULL;
    mce_hybrid_buffer_write(tBuffer, 0, size, data);
    return tBuffer;
}

void mce_hybrid_buffer_destroy(mce_HBuffer* tBuffer) {
    if (!tBuffer) return;
    DEBUG(tBuffer, "destroying hybrid buffer");

    if (tBuffer->gpuBuffer._instance) {
        mc_Buffer* gpuBuffer = malloc(sizeof *gpuBuffer);
        memcpy(gpuBuffer, &tBuffer->gpuBuffer, sizeof *gpuBuffer);
        mc_buffer_destroy(gpuBuffer);
    }
    if (tBuffer->cpuBuffer) mc_buffer_destroy(tBuffer->cpuBuffer);
    if (tBuffer->copier) mc_buffer_copier_destroy(tBuffer->copier);
    free(tBuffer);
}

uint64_t mce_hybrid_buffer_get_size(mce_HBuffer* tBuffer) {
    return mc_buffer_get_size(&tBuffer->gpuBuffer);
}

uint64_t mce_hybrid_buffer_write(
    mce_HBuffer* tBuffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!tBuffer) return 0;
    DEBUG(tBuffer, "writing %ld bytes to hybrid buffer", size);

    uint64_t res = mc_buffer_write(tBuffer->cpuBuffer, offset, size, data);
    if (res != size) return res;

    return mc_buffer_copier_copy(
        tBuffer->copier,
        tBuffer->cpuBuffer,
        &tBuffer->gpuBuffer,
        offset,
        offset,
        size
    );
}

uint64_t mce_hybrid_buffer_read(
    mce_HBuffer* tBuffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!tBuffer) return 0;
    DEBUG(tBuffer, "reading %ld bytes from hybrid buffer", size);

    uint64_t res = mc_buffer_copier_copy(
        tBuffer->copier,
        &tBuffer->gpuBuffer,
        tBuffer->cpuBuffer,
        offset,
        offset,
        size
    );
    if (res != size) return res;

    return mc_buffer_read(tBuffer->cpuBuffer, offset, size, data);
}