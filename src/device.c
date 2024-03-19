#include "microcompute_internal.h"

uint32_t defaultReturn[] = {0, 0, 0};

mc_DeviceType_t mc_device_get_type(mc_Device_t* self) {
    return self ? self->type : MC_DEVICE_TYPE_OTHER;
}

uint64_t mc_device_get_total_memory_size(mc_Device_t* self) {
    return self ? self->memTot : 0;
}

uint64_t mc_device_get_used_memory_size(mc_Device_t* self) {
    return self ? self->memUse : 0;
}

uint32_t mc_device_get_max_workgroup_size_total(mc_Device_t* self) {
    return self ? self->maxWorkgroupSizeTotal : 0;
}

uint32_t* mc_device_get_max_workgroup_size_shape(mc_Device_t* self) {
    return self ? self->maxWorkgroupSizeShape : defaultReturn;
}

uint32_t* mc_device_get_max_workgroup_count(mc_Device_t* self) {
    return self ? self->maxWorkgroupCount : defaultReturn;
}

char* mc_device_get_name(mc_Device_t* self) {
    return self ? self->devName : NULL;
}