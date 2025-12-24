/*
 * PC814_ThreePhase_Example.c
 *
 * Complete usage example for PC814 three-phase system
 * Demonstrates phase sequence detection and correction
 *
 * Author: Ehsan Zehni
 * Created: 2025
 */

#include "PC814_ThreePhase.h"
#include "PC814.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

/* Three PC814 handles for three phases */
static pc814_handle_t pc814_phase_a;
static pc814_handle_t pc814_phase_b;
static pc814_handle_t pc814_phase_c;

/* Three-phase system handle */
static pc814_threephase_t threephase_system;

/* Port functions for each phase (can be same or different) */
extern pc814_port_t pc814_port_a;
extern pc814_port_t pc814_port_b;
extern pc814_port_t pc814_port_c;

/* ========== Example Functions ========== */

/**
 * Initialize three-phase system
 */
void PC814_ThreePhase_Init(void)
{
    pc814_status_t status;
    
    /* Initialize each phase */
    status = pc814_init(&pc814_phase_a, &pc814_port_a, 
                       PC814_PULL_UP, PC814_EDGE_RISING);
    if (status != PC814_OK) {
        printf("Phase A init failed!\r\n");
        return;
    }
    
    status = pc814_init(&pc814_phase_b, &pc814_port_b, 
                       PC814_PULL_UP, PC814_EDGE_RISING);
    if (status != PC814_OK) {
        printf("Phase B init failed!\r\n");
        return;
    }
    
    status = pc814_init(&pc814_phase_c, &pc814_port_c, 
                       PC814_PULL_UP, PC814_EDGE_RISING);
    if (status != PC814_OK) {
        printf("Phase C init failed!\r\n");
        return;
    }
    
    /* Set expected frequency for all phases */
    pc814_set_expected_frequency(&pc814_phase_a, 50);
    pc814_set_expected_frequency(&pc814_phase_b, 50);
    pc814_set_expected_frequency(&pc814_phase_c, 50);
    
    /* Start all phases */
    pc814_start(&pc814_phase_a);
    pc814_start(&pc814_phase_b);
    pc814_start(&pc814_phase_c);
    
    /* Initialize three-phase system */
    status = pc814_threephase_init(&threephase_system,
                                   &pc814_phase_a,
                                   &pc814_phase_b,
                                   &pc814_phase_c);
    if (status != PC814_OK) {
        printf("Three-phase system init failed!\r\n");
        return;
    }
    
    /* Set tolerance for sequence detection */
    pc814_threephase_set_tolerance(&threephase_system, 10.0f);
    
    printf("Three-phase system initialized\r\n");
}

/**
 * Process three-phase system and detect sequence
 */
void PC814_ThreePhase_Process(void)
{
    pc814_status_t status;
    
    /* Process three-phase system */
    status = pc814_threephase_process(&threephase_system);
    
    if (status == PC814_OK) {
        /* Get detected sequence */
        pc814_sequence_t sequence = pc814_threephase_get_sequence(&threephase_system);
        
        switch (sequence) {
            case PC814_SEQUENCE_ABC:
                printf("Sequence: ABC (CORRECT)\r\n");
                break;
            case PC814_SEQUENCE_ACB:
                printf("Sequence: ACB (REVERSE - needs correction)\r\n");
                break;
            case PC814_SEQUENCE_UNKNOWN:
                printf("Sequence: UNKNOWN (waiting for data)\r\n");
                break;
            case PC814_SEQUENCE_ERROR:
                printf("Sequence: ERROR (check connections)\r\n");
                break;
        }
    }
}

/**
 * Display phase relationships
 */
void PC814_ThreePhase_DisplayRelationships(void)
{
    pc814_phase_relationship_t rel;
    
    if (pc814_threephase_get_relationship(&threephase_system, &rel) == PC814_OK && rel.valid) {
        printf("=== Phase Relationships ===\r\n");
        printf("Phase A-B angle: %.2f degrees\r\n", rel.phase_ab_angle);
        printf("Phase B-C angle: %.2f degrees\r\n", rel.phase_bc_angle);
        printf("Phase C-A angle: %.2f degrees\r\n", rel.phase_ca_angle);
        printf("Phase A frequency: %lu Hz\r\n", rel.phase_a_freq);
        printf("Phase B frequency: %lu Hz\r\n", rel.phase_b_freq);
        printf("Phase C frequency: %lu Hz\r\n", rel.phase_c_freq);
        printf("==========================\r\n");
    }
}

/**
 * Check phase sequence and get correction recommendation
 */
void PC814_ThreePhase_CheckSequence(void)
{
    bool swap_ab, swap_bc, swap_ca;
    char message[128];
    
    /* Get swap recommendations */
    if (pc814_threephase_get_swap_recommendation(&threephase_system, 
                                                  &swap_ab, &swap_bc, &swap_ca) == PC814_OK) {
        printf("=== Phase Correction ===\r\n");
        printf("Swap A-B: %s\r\n", swap_ab ? "YES" : "NO");
        printf("Swap B-C: %s\r\n", swap_bc ? "YES" : "NO");
        printf("Swap C-A: %s\r\n", swap_ca ? "YES" : "NO");
        printf("=======================\r\n");
    }
    
    /* Get correction message */
    if (pc814_threephase_get_correction_message(&threephase_system, message, sizeof(message)) == PC814_OK) {
        printf("Correction: %s\r\n", message);
    }
}

/**
 * Display all phase information
 */
void PC814_ThreePhase_DisplayAllInfo(void)
{
    pc814_phase_relationship_t rel;
    pc814_sequence_t sequence;
    float imbalance;
    bool synchronized;
    
    /* Get relationship data */
    if (pc814_threephase_get_relationship(&threephase_system, &rel) != PC814_OK || !rel.valid) {
        printf("Phase data not available\r\n");
        return;
    }
    
    sequence = pc814_threephase_get_sequence(&threephase_system);
    imbalance = pc814_threephase_get_imbalance(&threephase_system);
    synchronized = pc814_threephase_is_synchronized(&threephase_system);
    
    printf("=== Three-Phase System Status ===\r\n");
    
    /* Sequence */
    printf("Sequence: ");
    switch (sequence) {
        case PC814_SEQUENCE_ABC:
            printf("ABC (CORRECT)\r\n");
            break;
        case PC814_SEQUENCE_ACB:
            printf("ACB (REVERSE)\r\n");
            break;
        case PC814_SEQUENCE_UNKNOWN:
            printf("UNKNOWN\r\n");
            break;
        case PC814_SEQUENCE_ERROR:
            printf("ERROR\r\n");
            break;
    }
    
    /* Frequencies */
    printf("Frequencies: A=%lu Hz, B=%lu Hz, C=%lu Hz\r\n",
           rel.phase_a_freq, rel.phase_b_freq, rel.phase_c_freq);
    
    /* Phase angles */
    printf("Phase Angles: A-B=%.2f°, B-C=%.2f°, C-A=%.2f°\r\n",
           rel.phase_ab_angle, rel.phase_bc_angle, rel.phase_ca_angle);
    
    /* Synchronization */
    printf("Synchronized: %s\r\n", synchronized ? "YES" : "NO");
    
    /* Imbalance */
    if (imbalance >= 0) {
        printf("Imbalance: %.2f%%\r\n", imbalance);
    }
    
    /* Correction recommendation */
    PC814_ThreePhase_CheckSequence();
    
    printf("==================================\r\n");
}

/**
 * Get individual phase data
 */
void PC814_ThreePhase_GetIndividualPhases(void)
{
    pc814_data_t data_a, data_b, data_c;
    
    /* Read data from each phase */
    if (pc814_read_data(&pc814_phase_a, &data_a) == PC814_OK && data_a.valid) {
        printf("Phase A: Freq=%lu Hz, Period=%lu us, Count=%lu\r\n",
               data_a.frequency_hz, data_a.period_us, data_a.count);
    }
    
    if (pc814_read_data(&pc814_phase_b, &data_b) == PC814_OK && data_b.valid) {
        printf("Phase B: Freq=%lu Hz, Period=%lu us, Count=%lu\r\n",
               data_b.frequency_hz, data_b.period_us, data_b.count);
    }
    
    if (pc814_read_data(&pc814_phase_c, &data_c) == PC814_OK && data_c.valid) {
        printf("Phase C: Freq=%lu Hz, Period=%lu us, Count=%lu\r\n",
               data_c.frequency_hz, data_c.period_us, data_c.count);
    }
}

/**
 * Monitor phase sequence continuously
 */
void PC814_ThreePhase_MonitorSequence(void)
{
    static pc814_sequence_t last_sequence = PC814_SEQUENCE_UNKNOWN;
    
    /* Process system */
    pc814_threephase_process(&threephase_system);
    
    /* Check if sequence changed */
    pc814_sequence_t current_sequence = pc814_threephase_get_sequence(&threephase_system);
    
    if (current_sequence != last_sequence) {
        printf("Sequence changed: ");
        switch (current_sequence) {
            case PC814_SEQUENCE_ABC:
                printf("ABC (CORRECT)\r\n");
                break;
            case PC814_SEQUENCE_ACB:
                printf("ACB (REVERSE - SWAP B and C)\r\n");
                break;
            case PC814_SEQUENCE_ERROR:
                printf("ERROR - Check connections\r\n");
                break;
            default:
                break;
        }
        last_sequence = current_sequence;
    }
    
    /* Display correction if needed */
    if (current_sequence == PC814_SEQUENCE_ACB || current_sequence == PC814_SEQUENCE_ERROR) {
        PC814_ThreePhase_CheckSequence();
    }
}

/* ========== Main Usage Example ========== */
/*
void main(void)
{
    // Initialize system
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();   // Timer for Phase A
    MX_TIM3_Init();   // Timer for Phase B
    MX_TIM4_Init();   // Timer for Phase C
    
    // Initialize three-phase system
    PC814_ThreePhase_Init();
    
    // Main loop
    while (1) {
        // Process three-phase system
        PC814_ThreePhase_Process();
        
        // Display all information
        PC814_ThreePhase_DisplayAllInfo();
        
        // Or monitor sequence continuously
        // PC814_ThreePhase_MonitorSequence();
        
        HAL_Delay(1000);  // Wait 1 second
    }
}
*/

