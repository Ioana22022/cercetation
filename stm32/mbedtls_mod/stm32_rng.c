#include <stdlib.h>
#include <string.h>
#include <mbedtls/entropy.h>

#include <libopencm3/stm32/rng.h>


// initializes the RNG
void STM32RNG_Init() {
    rng_enable();
}

/**
 * Entropy poll callback for a hardware source.
 *
 * Note: This must accept NULL as its first argument.
 */
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len,
        size_t *olen)
{
  (void)data; // use this so compiler doesn't complain
  uint32_t aRandom32bit;
  uint32_t offset = 0;
  *olen = 0;
  while (*olen < len) {
    if (!rng_get_random(&aRandom32bit)) {
      /* Random number generation error */
      return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
    // append the random word to the output buffer
    uint32_t cplen = 4;
    if (cplen > (len - *olen)) { // when the length is not a multiple of 4
      cplen = (len - *olen);
    }
    memcpy(output + offset, &aRandom32bit, cplen);
    *olen += cplen;
  }
  return 0;
}

