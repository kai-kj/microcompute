#include <stdio.h>
#include <stdlib.h>

#include "microcompute.h"

int main(void) {
    mc_Instance* instance = mc_instance_create(NULL, NULL);
    mc_Device** devs = mc_instance_get_devices(instance);

    printf(
        "\n%d supported devi(s) found\n\n",
        mc_instance_get_device_count(instance)
    );

    for (uint32_t i = 0; i < mc_instance_get_device_count(instance); i++) {
        mc_Device* dev = devs[i];
        printf("=== %s ===\n", mc_device_get_name(dev));
        printf("- type: %s\n", MC_DEVICE_TYPE_to_str(mc_device_get_type(dev)));
        printf("- testing (values should be doubled every iteration):\n");

        float arr[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f};

        mc_Buffer* buff = mc_buffer_create(dev, MC_BUFFER_TYPE_CPU, sizeof arr);
        mc_buffer_write(buff, 0, sizeof arr, arr);

        FILE* fp = fopen("check_devs.spv", "rb");
        fseek(fp, 0, SEEK_END);
        size_t codeLen = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char* code = malloc(codeLen);
        fread(code, 1, codeLen, fp);
        fclose(fp);

        mc_Program* prog = mc_program_create(dev, codeLen, code, "main");

        for (uint32_t j = 0; j < 3; j++) {
            mc_program_run(prog, 5, 1, 1, buff);
            mc_buffer_read(buff, 0, sizeof arr, arr);

            printf("  - iteration %d: {", j + 1);
            for (uint32_t k = 0; k < 5; k++)
                printf("%f%s", arr[k], k != 4 ? ", " : "");
            printf("}\n");
        }

        printf("\n");

        mc_buffer_destroy(buff);
        mc_program_destroy(prog);
    }

    mc_instance_destroy(instance);
}
