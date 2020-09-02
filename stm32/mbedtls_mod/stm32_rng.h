#ifndef _LIB_STM32_RNG_IMPL_H
#define _LIB_STM32_RNG_IMPL_H

void STM32RNG_Init();

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len,
        size_t *olen);

#endif


