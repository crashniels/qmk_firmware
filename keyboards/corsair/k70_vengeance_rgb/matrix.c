/*
 * Copyright (c) 2020 Yaotian Feng / Codetector
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

 /*
 * K70 Vengeance RGB matrix scan
 * Adapted from Kemove Snowfox matrix.c by Codetector
 * LPC11U3x: cols strobed via palClearLine/palSetLine (push-pull capable),
 * rows read via palReadPort for atomic sampling.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <hal.h>
#include "matrix.h"
#include "timer.h"
#include "quantum.h"

// Column-indexed storage (Kemove style: matrix[col] holds a row bitmask)
static pin_t        matrix[MATRIX_COLS];
static pin_t        matrix_debouncing[MATRIX_COLS];
static uint32_t     debounce_times[MATRIX_COLS];

// Pin lists — must match keyboard.json matrix_pins order
// cols: P1_0..P1_9  (10 cols, strobed)
// rows: P1_12..P1_23 (12 rows, read)
ioline_t row_list[MATRIX_ROWS] = {
    PAL_LINE(IOPORT1, 0),
    PAL_LINE(IOPORT1, 1),
    PAL_LINE(IOPORT1, 2),
    PAL_LINE(IOPORT1, 3),
    PAL_LINE(IOPORT1, 4),
    PAL_LINE(IOPORT1, 5),
    PAL_LINE(IOPORT1, 6),
    PAL_LINE(IOPORT1, 7),
    PAL_LINE(IOPORT1, 8),
    PAL_LINE(IOPORT1, 9),
};

ioline_t col_list[MATRIX_COLS] = {
    PAL_LINE(IOPORT1, 12),
    PAL_LINE(IOPORT1, 13),
    PAL_LINE(IOPORT1, 14),
    PAL_LINE(IOPORT1, 15),
    PAL_LINE(IOPORT1, 16),
    PAL_LINE(IOPORT1, 17),
    PAL_LINE(IOPORT1, 18),
    PAL_LINE(IOPORT1, 19),
    PAL_LINE(IOPORT1, 20),
    PAL_LINE(IOPORT1, 21),
    PAL_LINE(IOPORT1, 22),
    PAL_LINE(IOPORT1, 23),
};

void matrix_init(void) {
    // Configure col pins as output, idle HIGH (deselected)
    for (int c = 0; c < MATRIX_COLS; c++) {
        palSetLineMode(col_list[c], PAL_MODE_OUTPUT_PUSHPULL);
        palSetLine(col_list[c]);
    }
    // Configure row pins as input with pull-up
    for (int r = 0; r < MATRIX_ROWS; r++) {
        palSetLineMode(row_list[r], PAL_MODE_INPUT_PULLUP);
    }

    memset(matrix,            0, sizeof(matrix));
    memset(matrix_debouncing, 0, sizeof(matrix_debouncing));
    memset(debounce_times,    0, sizeof(debounce_times));

    matrix_init_kb();
}

uint8_t matrix_scan(void) {
    for (int col = 0; col < MATRIX_COLS; col++) {
        // Select column (drive LOW)
        palClearLine(col_list[col]);

        // Short settle — 5 NOP cycles ~100ns at 48MHz
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

        // Read entire port atomically — all rows are on IOPORT1
        uint32_t port1 = palReadPort(IOPORT1);

        // Deselect column (drive HIGH)
        palSetLine(col_list[col]);

        // Build row bitmask for this column
        pin_t data = 0;
        for (int row = 0; row < MATRIX_ROWS; row++) {
            uint32_t pad = PAL_PAD(row_list[row]);  // bit position in port
            // Active low: pin reads 0 when pressed, so invert
            if (!(port1 & (1u << pad))) {
                data |= (1u << row);
            }
        }

        // Debounce
        if (matrix_debouncing[col] != data) {
            matrix_debouncing[col] = data;
            debounce_times[col]    = timer_read32();
        } else if (debounce_times[col] && timer_elapsed32(debounce_times[col]) >= DEBOUNCE) {
            matrix[col]         = matrix_debouncing[col];
            debounce_times[col] = 0;
        }
    }

    matrix_scan_kb();
    return 1;
}

// QMK queries by row — reconstruct from col-indexed storage
matrix_row_t matrix_get_row(uint8_t row) {
    matrix_row_t data = 0;
    for (uint8_t c = 0; c < MATRIX_COLS; c++) {
        data |= ((matrix[c] >> row) & 1u) << c;
    }
    return data;
}

bool matrix_is_on(uint8_t row, uint8_t col) {
    return (matrix[col] & (1u << row));
}

void matrix_print(void) {}

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}