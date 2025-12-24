/*
 * PC814_ThreePhase.c
 *
 * PC814 Three-Phase System Support Implementation
 * Detects phase sequence and phase relationships for three-phase AC systems
 *
 * Author: Ehsan Zehni
 * Created: 2025
 * 
 * Description: Complete implementation of three-phase system support
 */

#include "PC814_ThreePhase.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

/* Default tolerance for sequence detection (degrees) */
#define PC814_DEFAULT_SEQUENCE_TOLERANCE 10.0f

/* Expected phase angle for correct sequence (degrees) */
#define PC814_EXPECTED_PHASE_ANGLE 120.0f

/* Calculate phase angle between two timestamps */
static float calculate_phase_angle(uint32_t time1, uint32_t time2, uint32_t period_us)
{
    if (period_us == 0) {
        return 0.0f;
    }
    
    uint32_t time_diff;
    
    /* Calculate time difference, handling wrap-around */
    if (time2 >= time1) {
        time_diff = time2 - time1;
    } else {
        /* Handle wrap-around (assuming 32-bit timestamp) */
        time_diff = (0xFFFFFFFF - time1) + time2 + 1;
    }
    
    /* Normalize to period (handle multiple periods) */
    time_diff = time_diff % period_us;
    
    /* Calculate angle: (time_diff / period) * 360 */
    float angle = ((float)time_diff / (float)period_us) * 360.0f;
    
    /* Normalize to 0-360 range */
    if (angle >= 360.0f) {
        angle = fmodf(angle, 360.0f);
    }
    if (angle < 0.0f) {
        angle += 360.0f;
    }
    
    return angle;
}

/* Check if angle is approximately 120 degrees */
static bool is_angle_120(float angle, float tolerance)
{
    float diff_120 = fabsf(angle - 120.0f);
    
    /* Also check 240 degrees (which is -120 in three-phase) */
    float diff_240 = fabsf(angle - 240.0f);
    if (diff_240 > 180.0f) {
        diff_240 = 360.0f - diff_240;
    }
    
    return (diff_120 <= tolerance) || (diff_240 <= tolerance);
}

/* Check if angle is approximately 240 degrees (reverse sequence) */
static bool is_angle_240(float angle, float tolerance)
{
    float diff_240 = fabsf(angle - 240.0f);
    if (diff_240 > 180.0f) {
        diff_240 = 360.0f - diff_240;
    }
    
    float diff_120 = fabsf(angle - 120.0f);
    
    /* For reverse sequence, we expect ~240 degrees, not 120 */
    return (diff_240 <= tolerance) && (diff_120 > tolerance);
}

/* Initialize three-phase system */
pc814_status_t pc814_threephase_init(pc814_threephase_t *threephase,
                                     pc814_handle_t *phase_a,
                                     pc814_handle_t *phase_b,
                                     pc814_handle_t *phase_c)
{
    if (threephase == NULL || phase_a == NULL || phase_b == NULL || phase_c == NULL) {
        return PC814_ERROR;
    }
    
    memset(threephase, 0, sizeof(pc814_threephase_t));
    threephase->phase_a = phase_a;
    threephase->phase_b = phase_b;
    threephase->phase_c = phase_c;
    threephase->sequence = PC814_SEQUENCE_UNKNOWN;
    threephase->sequence_tolerance = PC814_DEFAULT_SEQUENCE_TOLERANCE;
    threephase->initialized = true;
    
    return PC814_OK;
}

/* Process three-phase system */
pc814_status_t pc814_threephase_process(pc814_threephase_t *threephase)
{
    if (threephase == NULL || !threephase->initialized) {
        return PC814_ERROR;
    }
    
    pc814_data_t data_a, data_b, data_c;
    
    /* Read data from all three phases */
    if (pc814_read_data(threephase->phase_a, &data_a) != PC814_OK || !data_a.valid) {
        return PC814_ERROR;
    }
    if (pc814_read_data(threephase->phase_b, &data_b) != PC814_OK || !data_b.valid) {
        return PC814_ERROR;
    }
    if (pc814_read_data(threephase->phase_c, &data_c) != PC814_OK || !data_c.valid) {
        return PC814_ERROR;
    }
    
    /* Update relationship data */
    threephase->relationship.phase_a_zc_time = data_a.timestamp_us;
    threephase->relationship.phase_b_zc_time = data_b.timestamp_us;
    threephase->relationship.phase_c_zc_time = data_c.timestamp_us;
    threephase->relationship.phase_a_freq = data_a.frequency_hz;
    threephase->relationship.phase_b_freq = data_b.frequency_hz;
    threephase->relationship.phase_c_freq = data_c.frequency_hz;
    
    /* Calculate average period for angle calculations */
    uint32_t avg_period = (data_a.period_us + data_b.period_us + data_c.period_us) / 3;
    
    /* Calculate phase angles */
    threephase->relationship.phase_ab_angle = calculate_phase_angle(
        data_a.timestamp_us, data_b.timestamp_us, avg_period);
    threephase->relationship.phase_bc_angle = calculate_phase_angle(
        data_b.timestamp_us, data_c.timestamp_us, avg_period);
    threephase->relationship.phase_ca_angle = calculate_phase_angle(
        data_c.timestamp_us, data_a.timestamp_us, avg_period);
    
    /* Detect sequence */
    threephase->sequence = pc814_threephase_detect_sequence(threephase);
    
    threephase->relationship.valid = true;
    threephase->last_update_time = data_a.timestamp_us;
    
    return PC814_OK;
}

/* Detect phase sequence */
pc814_sequence_t pc814_threephase_detect_sequence(pc814_threephase_t *threephase)
{
    if (threephase == NULL || !threephase->relationship.valid) {
        return PC814_SEQUENCE_ERROR;
    }
    
    float ab_angle = threephase->relationship.phase_ab_angle;
    float bc_angle = threephase->relationship.phase_bc_angle;
    float ca_angle = threephase->relationship.phase_ca_angle;
    float tolerance = threephase->sequence_tolerance;
    
    /* Check for ABC sequence (A->B->C: 120° each) */
    /* In correct sequence: A->B = 120°, B->C = 120°, C->A = 120° */
    bool ab_120 = is_angle_120(ab_angle, tolerance);
    bool bc_120 = is_angle_120(bc_angle, tolerance);
    bool ca_120 = is_angle_120(ca_angle, tolerance);
    
    if (ab_120 && bc_120 && ca_120) {
        return PC814_SEQUENCE_ABC;  /* Correct sequence */
    }
    
    /* Check for ACB sequence (reverse) */
    /* In reverse sequence: A->C = 120°, C->B = 120°, B->A = 120° */
    /* This means: A->B = 240° (or -120°), B->C = 240°, C->A = 120° */
    bool ab_240 = is_angle_240(ab_angle, tolerance);
    bool bc_240 = is_angle_240(bc_angle, tolerance);
    
    if ((ab_240 || bc_240) && ca_120) {
        return PC814_SEQUENCE_ACB;  /* Reverse sequence */
    }
    
    /* Alternative: Check if angles suggest reverse sequence */
    /* If A->B is close to 240 and B->C is close to 240, it's reverse */
    if (ab_240 && bc_240) {
        return PC814_SEQUENCE_ACB;  /* Reverse sequence */
    }
    
    /* If angles don't match expected values, return error */
    return PC814_SEQUENCE_ERROR;
}

/* Get current phase sequence */
pc814_sequence_t pc814_threephase_get_sequence(pc814_threephase_t *threephase)
{
    if (threephase == NULL) {
        return PC814_SEQUENCE_ERROR;
    }
    return threephase->sequence;
}

/* Check if sequence is correct */
bool pc814_threephase_is_sequence_correct(pc814_threephase_t *threephase)
{
    if (threephase == NULL) {
        return false;
    }
    return threephase->sequence == PC814_SEQUENCE_ABC;
}

/* Get phase relationship data */
pc814_status_t pc814_threephase_get_relationship(pc814_threephase_t *threephase,
                                                  pc814_phase_relationship_t *relationship)
{
    if (threephase == NULL || relationship == NULL) {
        return PC814_ERROR;
    }
    
    memcpy(relationship, &threephase->relationship, sizeof(pc814_phase_relationship_t));
    return PC814_OK;
}

/* Get phase angle between two phases */
float pc814_threephase_get_phase_angle(pc814_threephase_t *threephase,
                                       pc814_phase_id_t phase1,
                                       pc814_phase_id_t phase2)
{
    if (threephase == NULL || !threephase->relationship.valid) {
        return 0.0f;
    }
    
    if (phase1 == phase2) {
        return 0.0f;
    }
    
    /* Get angle based on phase combination */
    if (phase1 == PC814_PHASE_A && phase2 == PC814_PHASE_B) {
        return threephase->relationship.phase_ab_angle;
    } else if (phase1 == PC814_PHASE_B && phase2 == PC814_PHASE_C) {
        return threephase->relationship.phase_bc_angle;
    } else if (phase1 == PC814_PHASE_C && phase2 == PC814_PHASE_A) {
        return threephase->relationship.phase_ca_angle;
    } else if (phase1 == PC814_PHASE_B && phase2 == PC814_PHASE_A) {
        /* Reverse: 360 - angle */
        float angle = threephase->relationship.phase_ab_angle;
        return (angle > 0) ? (360.0f - angle) : 0.0f;
    } else if (phase1 == PC814_PHASE_C && phase2 == PC814_PHASE_B) {
        float angle = threephase->relationship.phase_bc_angle;
        return (angle > 0) ? (360.0f - angle) : 0.0f;
    } else if (phase1 == PC814_PHASE_A && phase2 == PC814_PHASE_C) {
        float angle = threephase->relationship.phase_ca_angle;
        return (angle > 0) ? (360.0f - angle) : 0.0f;
    }
    
    return 0.0f;
}

/* Get frequency of specific phase */
uint32_t pc814_threephase_get_phase_frequency(pc814_threephase_t *threephase,
                                              pc814_phase_id_t phase)
{
    if (threephase == NULL || !threephase->relationship.valid) {
        return 0;
    }
    
    switch (phase) {
        case PC814_PHASE_A:
            return threephase->relationship.phase_a_freq;
        case PC814_PHASE_B:
            return threephase->relationship.phase_b_freq;
        case PC814_PHASE_C:
            return threephase->relationship.phase_c_freq;
        default:
            return 0;
    }
}

/* Get which phases need to be swapped */
pc814_status_t pc814_threephase_get_swap_recommendation(pc814_threephase_t *threephase,
                                                         bool *swap_ab,
                                                         bool *swap_bc,
                                                         bool *swap_ca)
{
    if (threephase == NULL || swap_ab == NULL || swap_bc == NULL || swap_ca == NULL) {
        return PC814_ERROR;
    }
    
    *swap_ab = false;
    *swap_bc = false;
    *swap_ca = false;
    
    if (!threephase->relationship.valid) {
        return PC814_ERROR;
    }
    
    /* If sequence is ACB (reverse), recommend swapping B and C */
    /* This is the most common correction for reverse sequence */
    if (threephase->sequence == PC814_SEQUENCE_ACB) {
        *swap_bc = true;
        return PC814_OK;
    }
    
    if (threephase->sequence == PC814_SEQUENCE_ERROR) {
        /* Analyze angles to determine what needs to be swapped */
        float ab_angle = threephase->relationship.phase_ab_angle;
        float bc_angle = threephase->relationship.phase_bc_angle;
        float ca_angle = threephase->relationship.phase_ca_angle;
        float tolerance = threephase->sequence_tolerance;
        
        /* Analyze angles to determine what needs to be swapped */
        /* If A->B is 120 and C->A is 120, swapping B and C should fix it */
        if (is_angle_120(ab_angle, tolerance) && is_angle_120(ca_angle, tolerance)) {
            *swap_bc = true;
        }
        /* If B->C is 120 and C->A is 120, swapping A and B should fix it */
        else if (is_angle_120(bc_angle, tolerance) && is_angle_120(ca_angle, tolerance)) {
            *swap_ab = true;
        }
        /* If A->B is 120 and B->C is 120, but sequence is wrong, swap C and A */
        else if (is_angle_120(ab_angle, tolerance) && is_angle_120(bc_angle, tolerance)) {
            *swap_ca = true;
        }
    }
    
    return PC814_OK;
}

/* Get phase order correction message */
pc814_status_t pc814_threephase_get_correction_message(pc814_threephase_t *threephase,
                                                        char *message,
                                                        uint32_t max_len)
{
    if (threephase == NULL || message == NULL || max_len < 64) {
        return PC814_ERROR;
    }
    
    bool swap_ab, swap_bc, swap_ca;
    if (pc814_threephase_get_swap_recommendation(threephase, &swap_ab, &swap_bc, &swap_ca) != PC814_OK) {
        strncpy(message, "Error: Cannot determine phase correction", max_len - 1);
        message[max_len - 1] = '\0';
        return PC814_ERROR;
    }
    
    if (threephase->sequence == PC814_SEQUENCE_ABC) {
        strncpy(message, "Phase sequence is CORRECT (ABC)", max_len - 1);
        message[max_len - 1] = '\0';
        return PC814_OK;
    }
    
    if (swap_ab && swap_bc && swap_ca) {
        strncpy(message, "Error: All phases need correction - check connections", max_len - 1);
    } else if (swap_ab) {
        strncpy(message, "SWAP phases A and B to correct sequence", max_len - 1);
    } else if (swap_bc) {
        strncpy(message, "SWAP phases B and C to correct sequence", max_len - 1);
    } else if (swap_ca) {
        strncpy(message, "SWAP phases C and A to correct sequence", max_len - 1);
    } else {
        strncpy(message, "Phase sequence error - check all connections", max_len - 1);
    }
    
    message[max_len - 1] = '\0';
    return PC814_OK;
}

/* Set sequence tolerance */
void pc814_threephase_set_tolerance(pc814_threephase_t *threephase, float tolerance)
{
    if (threephase != NULL && tolerance > 0.0f && tolerance <= 30.0f) {
        threephase->sequence_tolerance = tolerance;
    }
}

/* Check if all phases are synchronized */
bool pc814_threephase_is_synchronized(pc814_threephase_t *threephase)
{
    if (threephase == NULL || !threephase->relationship.valid) {
        return false;
    }
    
    /* Check if all frequencies are similar */
    uint32_t freq_a = threephase->relationship.phase_a_freq;
    uint32_t freq_b = threephase->relationship.phase_b_freq;
    uint32_t freq_c = threephase->relationship.phase_c_freq;
    
    uint32_t max_freq = (freq_a > freq_b) ? freq_a : freq_b;
    max_freq = (max_freq > freq_c) ? max_freq : freq_c;
    
    uint32_t min_freq = (freq_a < freq_b) ? freq_a : freq_b;
    min_freq = (min_freq < freq_c) ? min_freq : freq_c;
    
    /* Allow 1 Hz difference */
    return (max_freq - min_freq) <= 1;
}

/* Get phase imbalance percentage */
float pc814_threephase_get_imbalance(pc814_threephase_t *threephase)
{
    if (threephase == NULL || !threephase->relationship.valid) {
        return -1.0f;
    }
    
    float ab_angle = threephase->relationship.phase_ab_angle;
    float bc_angle = threephase->relationship.phase_bc_angle;
    float ca_angle = threephase->relationship.phase_ca_angle;
    
    /* Calculate deviation from 120 degrees */
    float ab_dev = fabsf(ab_angle - 120.0f);
    float bc_dev = fabsf(bc_angle - 120.0f);
    float ca_dev = fabsf(ca_angle - 120.0f);
    
    /* Average deviation */
    float avg_dev = (ab_dev + bc_dev + ca_dev) / 3.0f;
    
    /* Convert to percentage */
    float imbalance = (avg_dev / 120.0f) * 100.0f;
    
    return imbalance;
}

/* Reset three-phase system */
void pc814_threephase_reset(pc814_threephase_t *threephase)
{
    if (threephase == NULL) {
        return;
    }
    
    threephase->sequence = PC814_SEQUENCE_UNKNOWN;
    threephase->relationship.valid = false;
    memset(&threephase->relationship, 0, sizeof(pc814_phase_relationship_t));
}

