#include <stdio.h>

#include "microcompute.h"
#include "microcompute_extra.h"

#define SPV_PATH "check_devs.spv"

int main(void) {
    mc_Instance* instance = mc_instance_create(NULL, NULL);
    mc_Device** devs = mc_instance_get_devices(instance);

    printf(
        "\n%d supported device(s) found\n\n",
        mc_instance_get_device_count(instance)
    );

    for (uint32_t i = 0; i < mc_instance_get_device_count(instance); i++) {
        mc_Device* dev = devs[i];
        printf("=== %s ===\n", mc_device_get_name(dev));
        printf("- type: %s\n", MC_DEVICE_TYPE_to_str(mc_device_get_type(dev)));
        printf("- testing (values should be doubled every iteration):\n");

        float arr[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
        mce_HBuffer* buff = mce_hybrid_buffer_create_from(dev, sizeof arr, arr);

        mc_Program* prog = mce_program_create_from_file(dev, SPV_PATH, "main");

        for (uint32_t j = 0; j < 3; j++) {
            mc_program_run(prog, 5, 1, 1, buff);
            mce_hybrid_buffer_read(buff, 0, sizeof arr, arr);

            printf("  - iteration %d: {", j + 1);
            for (uint32_t k = 0; k < 5; k++)
                printf("%f%s", arr[k], k != 4 ? ", " : "");
            printf("}\n");
        }

        printf("\n");

        mce_hybrid_buffer_destroy(buff);
        mc_program_destroy(prog);
    }

    mc_instance_destroy(instance);
}
