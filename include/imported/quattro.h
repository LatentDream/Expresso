#ifndef QUATTRO_H
#define QUATTRO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Declare the Rust function signature
extern uint64_t add_from_rust(uint64_t left, uint64_t right);

#ifdef __cplusplus
}
#endif

#endif // QUATTRO_H
