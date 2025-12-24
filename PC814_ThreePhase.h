/*
 * PC814_ThreePhase.h
 *
 * PC814 Three-Phase System Support
 * Detects phase sequence and phase relationships for three-phase AC systems
 *
 * Author: Ehsan Zehni
 * Created: 2025
 * 
 * Description: Three-phase system support for PC814 library
 *              with phase sequence detection and phase relationship analysis
 */

#ifndef PC814_THREEPHASE_H
#define PC814_THREEPHASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "PC814.h"
#include <stdint.h>
#include <stdbool.h>

/* Phase sequence types */
typedef enum {
    PC814_SEQUENCE_ABC = 0,      /* Correct sequence: A-B-C (120° apart) */
    PC814_SEQUENCE_ACB = 1,      /* Reverse sequence: A-C-B (-120° apart) */
    PC814_SEQUENCE_UNKNOWN = 2,  /* Sequence not yet determined */
    PC814_SEQUENCE_ERROR = 3     /* Error in phase detection */
} pc814_sequence_t;

/* Phase identification */
typedef enum {
    PC814_PHASE_A = 0,
    PC814_PHASE_B = 1,
    PC814_PHASE_C = 2
} pc814_phase_id_t;

/* Phase relationship structure */
typedef struct {
    uint32_t phase_a_zc_time;    /* Zero-crossing time of phase A */
    uint32_t phase_b_zc_time;    /* Zero-crossing time of phase B */
    uint32_t phase_c_zc_time;    /* Zero-crossing time of phase C */
    float phase_ab_angle;        /* Phase angle between A and B (degrees) */
    float phase_bc_angle;        /* Phase angle between B and C (degrees) */
    float phase_ca_angle;        /* Phase angle between C and A (degrees) */
    uint32_t phase_a_freq;       /* Frequency of phase A (Hz) */
    uint32_t phase_b_freq;       /* Frequency of phase B (Hz) */
    uint32_t phase_c_freq;       /* Frequency of phase C (Hz) */
    bool valid;                  /* Data validity flag */
} pc814_phase_relationship_t;

/* Three-phase system handle */
typedef struct {
    pc814_handle_t *phase_a;     /* Handle for phase A */
    pc814_handle_t *phase_b;     /* Handle for phase B */
    pc814_handle_t *phase_c;     /* Handle for phase C */
    pc814_sequence_t sequence;   /* Detected phase sequence */
    pc814_phase_relationship_t relationship; /* Phase relationships */
    uint32_t last_update_time;  /* Last update timestamp */
    float sequence_tolerance;    /* Tolerance for sequence detection (degrees) */
    bool initialized;            /* Initialization flag */
} pc814_threephase_t;

/**
 * Initialize three-phase system
 * @param threephase Pointer to three-phase handle
 * @param phase_a Handle for phase A
 * @param phase_b Handle for phase B
 * @param phase_c Handle for phase C
 * @return PC814_OK on success
 */
pc814_status_t pc814_threephase_init(pc814_threephase_t *threephase,
                                     pc814_handle_t *phase_a,
                                     pc814_handle_t *phase_b,
                                     pc814_handle_t *phase_c);

/**
 * Process three-phase system (call periodically)
 * @param threephase Pointer to three-phase handle
 * @return PC814_OK on success
 */
pc814_status_t pc814_threephase_process(pc814_threephase_t *threephase);

/**
 * Detect phase sequence
 * @param threephase Pointer to three-phase handle
 * @return Detected sequence (ABC, ACB, or ERROR)
 */
pc814_sequence_t pc814_threephase_detect_sequence(pc814_threephase_t *threephase);

/**
 * Get current phase sequence
 * @param threephase Pointer to three-phase handle
 * @return Current sequence
 */
pc814_sequence_t pc814_threephase_get_sequence(pc814_threephase_t *threephase);

/**
 * Check if phase sequence is correct
 * @param threephase Pointer to three-phase handle
 * @return true if sequence is ABC (correct)
 */
bool pc814_threephase_is_sequence_correct(pc814_threephase_t *threephase);

/**
 * Get phase relationship data
 * @param threephase Pointer to three-phase handle
 * @param relationship Pointer to relationship structure
 * @return PC814_OK on success
 */
pc814_status_t pc814_threephase_get_relationship(pc814_threephase_t *threephase,
                                                  pc814_phase_relationship_t *relationship);

/**
 * Get phase angle between two phases
 * @param threephase Pointer to three-phase handle
 * @param phase1 First phase (A, B, or C)
 * @param phase2 Second phase (A, B, or C)
 * @return Phase angle in degrees, 0 on error
 */
float pc814_threephase_get_phase_angle(pc814_threephase_t *threephase,
                                       pc814_phase_id_t phase1,
                                       pc814_phase_id_t phase2);

/**
 * Get frequency of specific phase
 * @param threephase Pointer to three-phase handle
 * @param phase Phase ID (A, B, or C)
 * @return Frequency in Hz, 0 on error
 */
uint32_t pc814_threephase_get_phase_frequency(pc814_threephase_t *threephase,
                                              pc814_phase_id_t phase);

/**
 * Get which phases need to be swapped
 * @param threephase Pointer to three-phase handle
 * @param swap_ab Pointer to flag: true if A and B should be swapped
 * @param swap_bc Pointer to flag: true if B and C should be swapped
 * @param swap_ca Pointer to flag: true if C and A should be swapped
 * @return PC814_OK on success
 */
pc814_status_t pc814_threephase_get_swap_recommendation(pc814_threephase_t *threephase,
                                                         bool *swap_ab,
                                                         bool *swap_bc,
                                                         bool *swap_ca);

/**
 * Get phase order correction message
 * @param threephase Pointer to three-phase handle
 * @param message Buffer to store message (at least 128 bytes)
 * @param max_len Maximum message length
 * @return PC814_OK on success
 */
pc814_status_t pc814_threephase_get_correction_message(pc814_threephase_t *threephase,
                                                        char *message,
                                                        uint32_t max_len);

/**
 * Set sequence tolerance
 * @param threephase Pointer to three-phase handle
 * @param tolerance Tolerance in degrees (default: 10.0)
 */
void pc814_threephase_set_tolerance(pc814_threephase_t *threephase, float tolerance);

/**
 * Check if all phases are synchronized
 * @param threephase Pointer to three-phase handle
 * @return true if all phases are synchronized
 */
bool pc814_threephase_is_synchronized(pc814_threephase_t *threephase);

/**
 * Get phase imbalance percentage
 * @param threephase Pointer to three-phase handle
 * @return Imbalance percentage (0-100), negative on error
 */
float pc814_threephase_get_imbalance(pc814_threephase_t *threephase);

/**
 * Reset three-phase system
 * @param threephase Pointer to three-phase handle
 */
void pc814_threephase_reset(pc814_threephase_t *threephase);

#ifdef __cplusplus
}
#endif

#endif /* PC814_THREEPHASE_H */

