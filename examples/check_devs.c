#include <stdio.h>
#include <stdlib.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

int main(void) {
    mc_Instance_t* instance = mc_instance_create(mc_default_debug_cb, NULL);
    mc_Device_t** devs = mc_instance_get_devices(instance);

    printf(
        "\n%d supported device(s) found\n\n",
        mc_instance_get_device_count(instance)
    );

    for (uint32_t i = 0; i < mc_instance_get_device_count(instance); i++) {
        mc_Device_t* dev = devs[i];
        printf("=== %s ===\n", mc_device_get_name(dev));
        printf("- memory: %ldGB\n", mc_device_get_total_memory_size(dev) >> 30);
        printf("- type: %s\n", mc_device_type_to_str(mc_device_get_type(dev)));
        printf("- testing (values should be doubled every iteration):\n");

        float arr[] = {0.0, 1.0, 2.0, 3.0, 4.0};

        mc_Buffer_t* buff = mc_buffer_create_from(dev, sizeof arr, arr);

        mc_Program_t* prog = mc_program_create(dev, "check_devs.spv", "main");

        for (uint32_t i = 0; i < 3; i++) {
            mc_program_run(prog, 5, 1, 1, buff);
            mc_buffer_read(buff, 0, sizeof arr, arr);

            printf("  - iteration %d: {", i + 1);
            for (uint32_t i = 0; i < 5; i++)
                printf("%f%s", arr[i], i != 4 ? ", " : "");
            printf("}\n");
        }

        printf("\n");

        mc_buffer_destroy(buff);
        mc_program_destroy(prog);
    }

    mc_instance_destroy(instance);
}
