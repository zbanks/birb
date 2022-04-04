#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

#define MIDI_CHANNEL 0

// For proper tuning, ensure PRESCALER * (2 ^ OCTAVE_OFFSET) = COUNTS_PER_PERIOD
// (COUNTS_PER_PERIOD = 16 for the lookup table below)

// This lookup table uses 16 counts per period
#define LOOKUP_COUNTS_PER_PERIOD 16
const uint16_t PERIOD_LOOKUP[] = {
38223, 36077, 34052, 32141, 30337, 28635, 27027, 25511, 24079, 22727, 21452, 20248, 19111, 18039, 17026, 16071, 15169, 14317, 13514, 12755, 12039, 11364, 10726, 10124, 9556, 9019, 8513, 8035, 7584, 7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4510, 4257, 4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389, 2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341, 1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710, 670, 633, 597, 564, 532, 502, 474, 447, 422, 399, 376, 355, 335, 316, 299, 282, 266, 251, 237, 224, 211, 199, 188, 178, 168, 158, 149, 141, 133, 126, 119, 112, 106, 100, 94, 89, 84, 79, 75, 70, 67, 63, 59, 56, 53, 50, 47, 44, 42, 40, 37, 35, 33, 31, 30, 28, 26, };

const uint8_t DOUBLE_DRIBBLE[] = {168, 52, 78, 107, 120, 120, 216, 134, 205, 50, 46, 15, 199, 226, 201, 170, 102, 156, 202, 28, 220, 134, 227, 164, 54, 149, 101, 77, 182, 212, 98, 71, 171, 106, 246, 200, 84, 77, 204, 148, 70, 143, 234, 232, 168, 85, 106, 170, 210, 38, 87, 218, 233, 31, 116, 236, 0, 14, 154, 242, 226, 171, 93, 104, 68, 145, 77, 158, 107, 201, 222, 127, 120, 120, 0, 0, 46, 252, 247, 135, 31, 0, 224, 192, 191, 15, 215, 224, 247, 143, 131, 15, 0, 224, 39, 255, 63, 192, 1, 128, 255, 124, 53, 128, 88, 255, 239, 195, 3, 0, 240, 255, 255, 14, 0, 0, 208, 255, 31, 112, 0, 226, 255, 255, 192, 0, 128, 254, 255, 255, 0, 0, 240, 255, 31, 0, 64, 166, 254, 255, 15, 0, 0, 240, 255, 255, 64, 0, 224, 255, 191, 0, 0, 126, 252, 255, 31, 0, 64, 195, 255, 255, 0, 0, 252, 255, 31, 0, 192, 7, 248, 255, 15, 0, 63, 192, 255, 127, 0, 192, 191, 255, 3, 0, 240, 3, 254, 255, 1, 128, 63, 224, 255, 15, 0, 248, 95, 224, 7, 0, 60, 254, 255, 7, 0, 254, 49, 254, 63, 0, 192, 255, 1, 240, 7, 0, 252, 255, 3, 128, 255, 19, 254, 63, 0, 224, 255, 0, 248, 15, 0, 255, 63, 0, 240, 255, 129, 255, 7, 0, 252, 63, 0, 252, 7, 240, 255, 3, 0, 255, 15, 248, 127, 0, 128, 255, 1, 224, 63, 240, 255, 3, 0, 252, 31, 188, 127, 0, 0, 255, 7, 128, 127, 252, 253, 0, 0, 252, 159, 207, 31, 0, 0, 254, 167, 0, 223, 255, 15, 0, 240, 119, 192, 255, 31, 0, 192, 63, 0, 254, 255, 127, 0, 0, 236, 23, 255, 63, 0, 0, 224, 255, 2, 236, 255, 31, 0, 224, 255, 128, 255, 63, 0, 0, 254, 3, 160, 255, 255, 1, 0, 251, 3, 213, 255, 31, 0, 0, 254, 7, 246, 255, 31, 0, 0, 255, 5, 252, 255, 1, 0, 192, 255, 72, 255, 127, 2, 0, 232, 182, 83, 171, 173, 18, 160, 111, 85, 106, 223, 130, 168, 219, 74, 66, 237, 171, 36, 86, 181, 164, 82, 171, 171, 84, 218, 86, 163, 82, 91, 43, 169, 180, 165, 34, 85, 109, 165, 202, 182, 85, 169, 180, 85, 154, 90, 173, 146, 36, 181, 170, 74, 213, 182, 165, 74, 173, 90, 201, 170, 87, 74, 168, 170, 149, 42, 173, 214, 82, 169, 214, 106, 165, 86, 173, 84, 42, 181, 82, 165, 170, 85, 150, 106, 90, 85, 171, 86, 85, 85, 169, 85, 37, 85, 85, 85, 169, 170, 90, 85, 86, 173, 85, 165, 86, 85, 89, 170, 170, 146, 170, 90, 170, 170, 86, 85, 171, 170, 150, 85, 85, 89, 170, 82, 89, 90, 85, 165, 166, 85, 85, 181, 90, 150, 86, 85, 165, 82, 85, 85, 85, 85, 90, 90, 85, 213, 170, 170, 154, 85, 165, 170, 170, 74, 85, 85, 170, 170, 170, 86, 171, 170, 170, 170, 170, 170, 170, 170, 74, 85, 153, 170, 170, 85, 213, 170, 170, 170, 170, 170, 166, 170, 42, 85, 85, 90, 106, 90, 85, 85, 171, 170, 170, 170, 85, 165, 170, 82, 85, 85, 106, 106, 85, 85, 181, 170, 170, 170, 106, 153, 170, 42, 85, 85, 169, 170, 86, 85, 213, 170, 170, 170, 170, 166, 170, 170, 74, 85, 85, 170, 102, 85, 85, 181, 170, 170, 170, 169, 101, 169, 74, 85, 85, 85, 85, 85, 85, 213, 170, 170, 170, 170, 89, 169, 84, 85, 170, 86, 85, 85, 89, 181, 90, 85, 170, 44, 45, 149, 170, 170, 213, 211, 74, 81, 233, 244, 212, 40, 73, 150, 86, 181, 237, 107, 67, 65, 82, 117, 117, 105, 82, 138, 76, 237, 253, 121, 33, 129, 84, 61, 186, 184, 84, 67, 133, 110, 255, 125, 64, 64, 86, 47, 29, 154, 213, 161, 192, 186, 255, 63, 0, 64, 235, 167, 37, 149, 173, 34, 80, 249, 255, 63, 0, 64, 222, 46, 139, 166, 207, 2, 160, 250, 255, 31, 0, 144, 191, 45, 161, 242, 181, 32, 128, 254, 255, 63, 0, 128, 111, 39, 209, 250, 179, 0, 0, 255, 255, 31, 0, 224, 239, 9, 161, 251, 107, 0, 0, 255, 255, 31, 0, 208, 175, 3, 225, 255, 39, 0, 0, 255, 255, 15, 0, 220, 175, 128, 240, 255, 39, 0, 0, 255, 255, 15, 0, 246, 71, 1, 248, 255, 15, 0, 128, 255, 255, 3, 128, 247, 67, 128, 252, 255, 1, 0, 240, 255, 255, 0, 192, 191, 129, 192, 250, 255, 1, 0, 240, 255, 127, 0, 240, 191, 0, 224, 254, 127, 0, 0, 252, 255, 31, 0, 250, 23, 0, 124, 255, 31, 0, 0, 255, 255, 11, 128, 254, 13, 0, 222, 255, 7, 0, 192, 255, 255, 1, 176, 159, 5, 0, 255, 255, 3, 0, 224, 255, 127, 0, 204, 95, 5, 192, 246, 255, 1, 0, 248, 255, 63, 0, 210, 207, 1, 224, 253, 127, 0, 0, 254, 255, 15, 0, 245, 91, 0, 120, 255, 63, 0, 0, 254, 255, 7, 64, 253, 25, 0, 238, 255, 15, 0, 128, 255, 255, 1, 80, 127, 21, 0, 235, 255, 7, 0, 224, 255, 255, 0, 224, 111, 5, 160, 252, 255, 1, 0, 248, 255, 63, 0, 240, 251, 32, 32, 255, 127, 0, 0, 254, 255, 15, 0, 250, 59, 8, 200, 255, 31, 0, 0, 255, 255, 7, 0, 255, 78, 0, 225, 255, 23, 0, 224, 255, 255, 1, 0, 255, 7, 64, 252, 251, 17, 0, 240, 255, 127, 0, 112, 255, 36, 0, 127, 127, 4, 0, 254, 255, 31, 0, 184, 63, 1, 161, 239, 79, 0, 224, 255, 255, 1, 128, 246, 87, 32, 248, 252, 41, 0, 252, 255, 31, 0, 176, 127, 69, 64, 219, 159, 2, 64, 255, 255, 3, 0, 219, 95, 66, 176, 253, 41, 0, 234, 255, 63, 0, 104, 253, 41, 8, 219, 47, 5, 144, 253, 255, 3, 1, 213, 175, 34, 104, 237, 85, 0, 181, 255, 63, 32, 176, 250, 209, 64, 213, 94, 10, 168, 253, 239, 6, 2, 151, 31, 21, 212, 244, 69, 129, 173, 127, 93, 80, 193, 211, 71, 5, 173, 94, 148, 208, 117, 183, 22, 41, 242, 234, 145, 18, 87, 27, 169, 180, 214, 170, 169, 164, 234, 173, 162, 82, 173, 150, 84, 85, 86, 213, 84, 169, 214, 86, 22, 181, 116, 101, 69, 83, 85, 85, 85, 106, 85, 171, 42, 77, 173, 86, 169, 210, 74, 75, 85, 85, 85, 85, 85, 213, 170, 170, 170, 170, 170, 170, 42, 75, 85, 170, 86, 181, 170, 170, 170, 170, 170, 170, 170, 170, 84, 169, 90, 85, 171, 170, 180, 172, 170, 170, 170, 170, 82, 85, 85, 85, 85, 85, 85, 85, 171, 154, 154, 170, 170, 82, 85, 85, 85, 85, 85, 213, 170, 170, 170, 170, 170, 170, 170, 84, 149, 170, 90, 85, 171, 170, 170, 170, 170, 170, 150, 170, 84, 169, 170, 106, 85, 85, 213, 170, 170, 170, 90, 85, 166, 42, 85, 149, 170, 170, 85, 85, 181, 170, 170, 106, 85, 85, 85, 170, 82, 165, 170, 170, 170, 90, 85, 171, 170, 102, 90, 85, 108, 114, 163, 162, 165, 170, 74, 157, 90, 213, 170, 170, 86, 85, 85, 85, 197, 70, 39, 58, 177, 82, 173, 116, 169, 75, 91, 90, 85, 85, 85, 85, 85, 170, 82, 149, 170, 84, 85, 179, 170, 85, 171, 89, 85, 85, 85, 85, 149, 170, 42, 85, 169, 170, 82, 213, 170, 86, 181, 106, 85, 53, 85, 85, 85, 165, 170, 84, 85, 38, 43, 213, 84, 171, 90, 181, 106, 85, 213, 84, 85, 85, 101, 170, 82, 85, 169, 42, 43, 181, 170, 170, 85, 171, 90, 85, 85, 85, 85, 85, 170, 42, 85, 149, 170, 170, 170, 170, 170, 106, 85, 171, 170, 85, 85, 85, 85, 85, 169, 42, 85, 85, 101, 170, 170, 170, 170, 90, 85, 85, 173, 170, 85, 85, 85, 85, 85, 169, 170, 84, 85, 85, 149, 170, 170, 169, 85, 85, 85, 213, 170, 170, 90, 85, 85, 85, 169, 170, 170, 84, 85, 149, 170, 170, 170, 85, 85, 85, 85, 171, 170, 170, 170, 149, 85, 165, 169, 170, 42, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 173, 170, 170, 170, 170, 170, 170, 170, 170, 92, 240, 193, 75, 89, 165, 141, 60, 105, 83, 45, 173, 202, 170, 170, 170, 170, 170, 170, 170, 170, 42, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 203, 170, 234, 136, 157, 92, 116, 81, 45, 109, 116, 172, 178, 240, 177, 25, 77, 102, 54, 115, 240, 104, 102, 241, 147, 201, 89, 163, 139, 142, 153, 92, 108, 84, 51, 177, 233, 96, 43, 155, 57, 217, 150, 124, 162, 151, 204, 196, 22, 53, 154, 217, 168, 236, 176, 212, 204, 180, 212, 83, 254, 224, 75, 85, 18, 87, 72, 29, 117, 113, 85, 184, 209, 216, 20, 95, 106, 63, 248, 154, 212, 192, 86, 129, 23, 173, 141, 78, 42, 45, 106, 54, 185, 253, 146, 244, 19, 69, 150, 13, 168, 61, 210, 213, 84, 137, 214, 18, 55, 243, 55, 84, 191, 80, 82, 241, 4, 234, 11, 245, 74, 85, 82, 182, 196, 122, 218, 47, 226, 87, 146, 10, 173, 130, 220, 165, 182, 138, 85, 161, 182, 100, 213, 251, 18, 249, 86, 72, 210, 150, 16, 219, 86, 85, 85, 77, 164, 186, 146, 250, 175, 130, 221, 86, 8, 214, 22, 200, 186, 181, 82, 181, 17, 177, 173, 88, 253, 43, 212, 118, 37, 160, 174, 34, 170, 222, 170, 100, 173, 132, 108, 43, 237, 183, 162, 170, 171, 4, 89, 43, 164, 218, 186, 42, 85, 21, 105, 85, 218, 191, 66, 183, 21, 5, 233, 38, 160, 219, 85, 85, 171, 69, 80, 183, 228, 123, 165, 174, 42, 21, 82, 43, 72, 219, 170, 173, 170, 138, 200, 86, 234, 191, 68, 223, 20, 9, 234, 70, 160, 111, 105, 171, 213, 4, 212, 77, 232, 255, 208, 87, 149, 4, 186, 9, 240, 110, 116, 171, 181, 0, 93, 37, 252, 127, 224, 111, 65, 2, 223, 0, 248, 23, 250, 182, 21, 64, 47, 132, 254, 63, 228, 191, 0, 69, 15, 128, 254, 33, 251, 55, 9, 84, 21, 84, 253, 63, 240, 63, 192, 130, 7, 128, 255, 80, 255, 85, 12, 84, 130, 174, 250, 63, 233, 31, 130, 3, 22, 192, 127, 240, 127, 120, 8, 152, 160, 46, 248, 255, 244, 31, 6, 0, 31, 192, 127, 224, 255, 83, 192, 16, 224, 19, 222, 255, 47, 127, 8, 0, 28, 192, 255, 197, 255, 7, 192, 1, 224, 113, 105, 255, 231, 254, 1, 0, 112, 1, 255, 31, 255, 31, 0, 2, 192, 167, 195, 247, 63, 255, 7, 0, 16, 128, 254, 251, 243, 127, 0, 0, 96, 252, 26, 252, 255, 225, 255, 0, 0, 1, 234, 191, 63, 254, 7, 0, 0, 198, 175, 193, 223, 127, 252, 15, 0, 224, 128, 252, 247, 195, 63, 1, 2, 65, 252, 147, 244, 254, 31, 126, 13, 0, 56, 224, 252, 255, 192, 175, 0, 240, 1, 252, 120, 240, 252, 127, 120, 62, 0, 112, 192, 172, 255, 193, 159, 7, 128, 7, 232, 227, 225, 232, 255, 199, 227, 65, 1, 130, 29, 248, 63, 248, 248, 1, 120, 84, 192, 63, 148, 222, 253, 195, 135, 211, 0, 130, 31, 160, 255, 28, 248, 63, 96, 184, 18, 240, 214, 10, 253, 63, 12, 252, 31, 0, 248, 7, 192, 215, 7, 234, 175, 6, 118, 131, 170, 211, 81, 37, 123, 84, 245, 85, 180, 216, 98, 81, 165, 170, 166, 170, 214, 74, 181, 42, 165, 170, 74, 91, 173, 180, 85, 181, 178, 82, 165, 170, 42, 173, 42, 86, 90, 106, 210, 170, 106, 105, 83, 85, 213, 166, 90, 170, 85, 85, 85, 85, 85, 149, 82, 85, 85, 170, 172, 165, 170, 90, 75, 85, 181, 74, 181, 180, 170, 170, 169, 106, 85, 169, 166, 154, 42, 83, 85, 165, 150, 106, 169, 149, 218, 82, 171, 180, 82, 43, 213, 212, 170, 170, 106, 85, 89, 165, 170, 170, 74, 85, 85, 85, 149, 166, 102, 165, 86, 85, 45, 173, 170, 170, 170, 170, 42, 213, 170, 170, 170, 170, 170, 170, 170, 170, 210, 84, 85, 165, 165, 170, 106, 85, 85, 85, 75, 171, 170, 170, 170, 170, 170, 170, 82, 171, 170, 170, 170, 106, 169, 170, 170, 74, 85, 85, 85, 149, 170, 149, 85, 85, 85, 173, 170, 170, 106, 170, 170, 170, 170, 82, 85, 85, 181, 170, 172, 170, 170, 170, 170, 170, 170, 84, 85, 85, 85, 169, 170, 85, 85, 85, 171, 170, 170, 170, 170, 170, 170, 42, 85, 85, 85, 85, 85, 213, 170, 170, 170, 169, 170, 170, 170, 42, 85, 85, 101, 85, 169, 169, 85, 85, 173, 170, 170, 86, 169, 170, 170, 170, 42, 85, 85, 85, 85, 85, 85, 85, 171, 170, 170, 170, 170, 170, 170, 42, 85, 85, 85, 101, 106, 90, 85, 85, 213, 170, 170, 170, 170, 85, 170, 170, 170, 170, 82, 85, 85, 85, 85, 85, 85, 85, 173, 170, 170, 170, 170, 170, 170, 170, 74, 85, 85, 85, 165, 170, 106, 85, 85, 85, 171, 170, 170, 170, 170, 169, 170, 170, 74, 83, 85, 169, 169, 85, 85, 85, 77, 85, 213, 213, 71, 76, 84, 213, 170, 168, 228, 101, 43, 83, 170, 86, 155, 42, 85, 173, 85, 85, 42, 85, 85, 85, 85, 85, 153, 102, 77, 75, 165, 238, 237, 195, 0, 4, 175, 157, 22, 122, 252, 248, 96, 144, 116, 221, 85, 35, 167, 182, 166, 66, 37, 91, 117, 210, 98, 149, 106, 116, 217, 166, 150, 148, 125, 127, 112, 0, 128, 229, 223, 7, 7, 62, 126, 20, 136, 172, 127, 253, 176, 160, 212, 90, 52, 177, 90, 55, 45, 148, 164, 86, 87, 158, 150, 37, 75, 236, 215, 15, 15, 0, 80, 252, 247, 225, 192, 227, 195, 129, 194, 219, 223, 139, 10, 20, 157, 77, 139, 206, 118, 169, 8, 73, 237, 234, 105, 201, 170, 210, 226, 208, 252, 254, 225, 0, 0, 198, 127, 63, 28, 56, 124, 56, 176, 240, 251, 119, 17, 129, 168, 181, 213, 212, 244, 122, 42, 0, 81, 125, 127, 28, 37, 153, 60, 58, 181, 88, 251, 253, 192, 0, 0, 62, 255, 63, 48, 240, 240, 194, 80, 237, 251, 171, 1, 1, 83, 251, 234, 82, 85, 91, 113, 64, 136, 119, 255, 184, 32, 81, 181, 60, 94, 213, 100, 234, 213, 7, 14, 0, 248, 249, 247, 3, 3, 14, 31, 61, 60, 249, 218, 42, 40, 176, 242, 235, 149, 74, 180, 210, 134, 14, 182, 188, 234, 146, 10, 43, 109, 182, 213, 170, 146, 146, 82, 63, 127, 112, 0, 129, 143, 63, 63, 56, 240, 240, 193, 7, 23, 223, 218, 66, 5, 13, 63, 189, 212, 200, 73, 43, 45, 178, 234, 165, 35, 11, 157, 186, 178, 82, 83, 171, 154, 170, 212, 84, 83, 187, 124, 208, 128, 5, 47, 127, 62, 49, 193, 162, 167, 171, 170, 180, 106, 75, 11, 22, 118, 124, 233, 133, 131, 22, 91, 185, 212, 201, 149, 14, 149, 184, 244, 165, 11, 71, 90, 188, 116, 201, 69, 23, 43, 57, 105, 185, 181, 67, 3, 37, 174, 190, 173, 42, 82, 178, 170, 75, 43, 45, 93, 58, 105, 97, 209, 165, 151, 27, 86, 88, 178, 86, 171, 166, 170, 170, 44, 85, 169, 85, 171, 150, 42, 90, 217, 178, 165, 38, 171, 172, 212, 84, 85, 77, 107, 217, 146, 9, 149, 90, 187, 174, 42, 50, 177, 170, 203, 150, 22, 93, 108, 114, 97, 197, 149, 157, 54, 90, 84, 169, 165, 45, 155, 154, 180, 212, 82, 149, 77, 173, 172, 180, 84, 85, 85, 171, 170, 170, 170, 170, 212, 212, 82, 75, 43, 53, 85, 173, 54, 105, 164, 82, 173, 117, 75, 11, 141, 172, 186, 106, 169, 73, 75, 43, 157, 52, 105, 229, 170, 149, 22, 85, 106, 213, 170, 165, 74, 85, 85, 181, 180, 178, 202, 74, 85, 85, 181, 172, 170, 202, 74, 75, 43, 43, 173, 170, 210, 82, 85, 85, 85, 173, 54, 89, 164, 98, 171, 171, 75, 19, 141, 90, 186, 116, 178, 209, 202, 38, 39, 45, 90, 185, 106, 165, 69, 85, 86, 181, 170, 170, 74, 75, 45, 53, 85, 213, 178, 170, 74, 83, 85, 85, 181, 170, 170, 82, 85, 85, 173, 170, 170, 82, 83, 83, 171, 44, 85, 213, 180, 170, 170, 170, 84, 169, 86, 173, 170, 170, 84, 85, 85, 181, 170, 170, 170, 170, 74, 85, 85, 213, 170, 170, 170, 84, 85, 85, 173, 170, 170, 74, 85, 85, 85, 213, 170, 170, 74, 83, 85, 213, 180, 170, 170, 84, 85, 85, 171, 170, 170, 170, 170, 170, 178, 178, 180, 170, 42, 45, 85, 85, 77, 171, 170, 42, 85, 85, 85, 173, 170, 170, 84, 85, 85, 83, 171, 170, 170, 172, 170, 84, 85, 213, 212, 170, 42, 43, 85, 85, 85, 203, 170, 74, 203, 210, 212, 178, 178, 170, 170, 172, 170, 170, 170, 42, 203, 170, 170, 210, 170, 210, 180, 52, 181, 84, 77, 77, 85, 85, 85, 85, 85, 85, 85, 77, 45, 181, 180, 84, 85, 85, 85, 85, 213, 170, 178, 170, 82, 85, 213, 84, 53, 45, 181, 172, 172, 74, 43, 171, 170, 202, 170, 202, 170, 178, 170, 170, 42, 171, 172, 42, 171, 170, 202, 170, 180, 180, 172, 52, 85, 85, 77, 85, 85, 85, 85, 85, 83, 85, 85, 85, 53, 53, 85, 77, 53, 43, 51, 83, 85, 77, 77, 173, 170, 210, 212, 84, 181, 170, 170, 172, 44, 43, 171, 170, 170, 170, 170, 210, 170, 178, 180, 52, 53, 43, 45, 203, 210, 82, 83, 85, 85};

static void note_on(uint8_t k, uint8_t v);
static void note_off(uint8_t k);

#define N_NOTES 5
#define N_OSC 3
#define N_SELECTOR_POSITIONS 12

#define NO_NOTE 255
#define NO_OSC 255

#define NO_GLIDE_INCREMENT 10000
#define WHITE_NOISE_PITCH 84

// Notes currently held down, and how they are being voiced
static uint8_t note_index[N_NOTES];
static uint8_t note_velocity[N_NOTES];
static uint8_t note_osc[N_NOTES];

// Computed from the note_* arrays:
static uint8_t osc_note_index[N_OSC];
static uint8_t osc_note_velocity[N_OSC];


enum mode {
    MODE_MONO,
    MODE_POLY,
    MODE_OCTAVE,
};

enum wave {
    WAVE_PULSE,
    WAVE_NOISE,
    WAVE_SAMPLE,
};

static uint8_t mod = 0;

struct adsr_config {
    int16_t initial_value;
    int16_t a_incr;
    int16_t a_value;
    int16_t d_incr;
    int16_t s_value;
    int16_t r_incr;
    int16_t final_value;
};

struct adsr_state {
    enum {
        ADSR_OFF,
        ADSR_ATTACK,
        ADSR_DECAY,
        ADSR_SUSTAIN,
        ADSR_RELEASE,
    } mode;
    int16_t output;
    uint8_t last_gate;
};

static void adsr_configure(struct adsr_config *config, int16_t initial_value, int16_t a_time, int16_t a_value, int16_t d_time, int16_t s_value, int16_t r_time, int16_t final_value) {
    config->initial_value = initial_value;
    if (a_time <= 0) {
        a_time = 1;
    }
    config->a_incr = (a_value - initial_value) / a_time;
    if (config->a_incr == 0) {
        config->a_incr = a_value > initial_value ? 1 : -1;
    }
    config->a_value = a_value;
    if (d_time <= 0) {
        d_time = 1;
    }
    config->d_incr = (s_value - a_value) / d_time;
    if (config->d_incr == 0) {
        config->d_incr = s_value > a_value ? 1 : -1;
    }
    config->s_value = s_value;
    if (r_time <= 0) {
        r_time = 1;
    }
    config->r_incr = (final_value - s_value) / r_time;
    if (config->r_incr == 0) {
        config->r_incr = final_value > s_value ? 1 : -1;
    }
    config->final_value = final_value;
}

static void adsr_configure_percussion(struct adsr_config *config, int16_t high_value, int16_t d_time, int16_t low_value) {
    config->initial_value = low_value;
    config->a_value = high_value;
    config->s_value = low_value;
    config->final_value = low_value;

    config->a_incr = high_value - low_value; // Instant attack
    config->d_incr = (low_value - high_value) / d_time;
    if (config->d_incr == 0) {
        config->d_incr = low_value > high_value ? 1 : -1;
    }
    config->r_incr = config->d_incr;
}

static void adsr_init(struct adsr_state *state, const struct adsr_config *config) {
    state->mode = ADSR_OFF;
    state->output = config->final_value;
    state->last_gate = 0;
}

static int16_t adsr_update(struct adsr_state *state, const struct adsr_config *config, uint8_t gate) {
    if ((state->mode == ADSR_OFF || state->mode == ADSR_RELEASE || gate != state->last_gate) && gate) {
        state->output = config->initial_value;
        state->mode = ADSR_ATTACK;
    } else if (!gate) {
        state->mode = ADSR_RELEASE;
    }
    state->last_gate = gate;
    switch (state->mode) {
        case ADSR_OFF:
            state->output = config->initial_value;
            break;
        case ADSR_SUSTAIN:
            state->output = config->s_value;
            break;
        case ADSR_ATTACK:
            if ((config->a_incr > 0 && state->output >= config->a_value - config->a_incr)
             || (config->a_incr < 0 && state->output <= config->a_value - config->a_incr)) {
                state->output = config->a_value;
                state->mode = ADSR_DECAY;
            } else {
                state->output += config->a_incr;
            }
            break;
        case ADSR_DECAY:
            if ((config->d_incr > 0 && state->output >= config->s_value - config->d_incr)
             || (config->d_incr < 0 && state->output <= config->s_value - config->d_incr)) {
                state->output = config->s_value;
                state->mode = ADSR_SUSTAIN;
            } else {
                state->output += config->d_incr;
            }
            break;
        case ADSR_RELEASE:
            if ((config->r_incr > 0 && state->output >= config->final_value - config->r_incr)
             || (config->r_incr < 0 && state->output <= config->final_value - config->r_incr)) {
                state->output = config->final_value;
                state->mode = ADSR_OFF;
            } else {
                state->output += config->r_incr;
            }
            break;
    }
    return state->output;
}

struct triangle_config {
    int16_t low;
    int16_t high;
    int16_t initial_value;
    int16_t increment;
};

struct triangle_state {
    uint8_t up;
    int16_t output;
    uint8_t last_gate;
};

static void triangle_configure(struct triangle_config *config, int16_t low, int16_t high, int16_t initial_value, int16_t period) {
    config->low = low;
    config->high = high;
    config->initial_value = initial_value;
    if (period == 0) {
        period = 1;
    }
    config->increment = ((uint16_t)high - (uint16_t)low) / period;
    if (config->increment == 0) {
        config->increment = 1;
    }
}

static void triangle_configure_constant(struct triangle_config *config, int16_t value) {
    config->low = value;
    config->high = value;
    config->initial_value = value;
    config->increment = 0;
}

static void triangle_init(struct triangle_state *state, const struct triangle_config *config) {
    state->output = config->low;
    state->up = 1;
    state->last_gate = 0;
}

static int16_t triangle_update(struct triangle_state *state, const struct triangle_config *config, uint8_t gate) {
    if (gate && gate != state->last_gate) {
        state->output = config->initial_value;
        state->up = 1;
    } else {
        if (state->up) {
            if (state->output > config->high - config->increment) {
                state->output = config->high;
                state->up = 0;
            } else {
                state->output += config->increment;
            }
        } else {
            if (state->output < config->low + config->increment) {
                state->output = config->low;
                state->up = 1;
            } else {
                state->output -= config->increment;
            }
        }
    }
    state->last_gate = gate;
    return state->output;
}

static uint8_t rand8() {
    static uint8_t x;
    static uint8_t a;
    static uint8_t b;
    static uint8_t c;

    x++;               //x is incremented every round and is not affected by any other variable
    a = a^c^x;       //note the mix of addition and XOR
    b = b+a;         //And the use of very few instructions
    c = (c+(b>>1))^a;  //the right shift is to ensure that high-order bits from b can affect  
    return c;          //low order bits of other variables
}

struct arp_config {
    const int8_t *notes;
    uint8_t n_notes;
    int16_t period;
};

struct arp_state {
    uint8_t index;
    int16_t timer;
    uint8_t last_gate;
    int8_t note;
};

static void arp_configure(struct arp_config *config, const int8_t notes[], uint8_t n_notes, int16_t period) {
    config->notes = notes;
    config->n_notes = n_notes;
    config->period = period;
}

static void arp_init(struct arp_state *state, const struct arp_config *config) {
    state->last_gate = 0;
    state->timer = 0;
    state->index = 0;
    if (config->n_notes > 0) {
        state->note = config->notes[0];
    } else {
        state->note = 0;
    }
}

static int8_t arp_update(struct arp_state *state, const struct arp_config *config, uint8_t gate) {
    if (config->n_notes == 0) {
        // No arpeggiation is configured
        state->timer = 0;
        state->index = 0;
        state->note = 0;
    } else if (gate && gate != state->last_gate) {
        // Rising edge on gate
        state->timer = 0;
        state->index = 0;
        state->note = config->notes[0];
    } else {
        state->timer++;
        if (state->timer >= config->period) {
            // Advance arp
            state->timer = 0;
            state->index++;
            if (state->index >= config->n_notes) {
                state->index = 0;
            }
            state->note = config->notes[state->index];
        }
    }
    state->last_gate = gate;
    return state->note;
}

struct glide_config {
    int16_t increment;
};

struct glide_state {
    int16_t last_pitch;
    uint8_t last_gate;
};

static void glide_configure(struct glide_config *config, int16_t increment) {
    config->increment = increment;
}

static void glide_init(struct glide_state *state, const struct glide_config *config) {
    (void)config;
    state->last_gate = 0;
}

static int16_t glide_update(struct glide_state *state, const struct glide_config *config, int16_t target_pitch, uint8_t gate) {
    if (gate && gate != state->last_gate) {
        state->last_pitch = target_pitch;
    } else {
        if (state->last_pitch < target_pitch) {
            if (state->last_pitch > target_pitch - config->increment) {
                state->last_pitch = target_pitch;
            } else {
                state->last_pitch += config->increment;
            }
        } else {
            if (state->last_pitch - config->increment < target_pitch) {
                state->last_pitch = target_pitch;
            } else {
                state->last_pitch -= config->increment;
            }
        }
    }
    state->last_gate = gate;
    return state->last_pitch;
}

static void rx(uint8_t byte) {
    static enum {
        UNKNOWN,
        WAIT_NOTE_ON_KEY,
        WAIT_NOTE_ON_VELOCITY,
        WAIT_NOTE_OFF_KEY,
        WAIT_NOTE_OFF_VELOCITY,
        WAIT_CC_CONTROL,
        WAIT_MOD_VALUE,
    } state = UNKNOWN;

    static uint8_t k;
    static uint8_t v;

    if (byte == 0xFF) {
        // "Reset" message: Issue system reset
        _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
        for(;;);
    } else if ((byte & 0xF8) == 0xF8) {
        return; // Ignore real-time messages
    }

    if (byte & 0x80) {
        // Status byte
        if (byte == (0x80 | MIDI_CHANNEL)) {
            state = WAIT_NOTE_OFF_KEY;
        } else if (byte == (0x90 | MIDI_CHANNEL)) {
            state = WAIT_NOTE_ON_KEY;
        } else if (byte == (0xB0 | MIDI_CHANNEL)) {
            state = WAIT_CC_CONTROL;
        } else {
            state = UNKNOWN;
        }
    } else {
        // Handle notes, etc
        switch (state) {
            case UNKNOWN:
                break;
            case WAIT_NOTE_ON_KEY:
                k = byte;
                state = WAIT_NOTE_ON_VELOCITY;
                break;
            case WAIT_NOTE_ON_VELOCITY:
                v = byte;
                note_on(k, v);
                state = WAIT_NOTE_ON_KEY;
                break;
            case WAIT_NOTE_OFF_KEY:
                k = byte;
                state = WAIT_NOTE_OFF_VELOCITY;
                break;
            case WAIT_NOTE_OFF_VELOCITY:
                v = byte;
                note_off(k);
                state = WAIT_NOTE_OFF_KEY;
                break;
            case WAIT_CC_CONTROL:
                if (byte == 0x01) { // mod wheel
                    state = WAIT_MOD_VALUE;
                } else {
                    state = UNKNOWN;
                }
                break;
            case WAIT_MOD_VALUE:
                mod = byte;
                state = WAIT_CC_CONTROL;
                break;
        }
    }
}

ISR (USART0_RXC_vect) 
{
    rx(USART0.RXDATAL);
}

static void update_modulation();

struct osc_state {
    // Low-level timer stuff
    uint8_t prescaler;
    uint16_t timer_period_high;
    uint16_t timer_period_low;
    uint8_t t8;
    uint16_t t16;

    bool chromatic;
    uint8_t triggered_note;
    uint8_t triggered_velocity;

    enum wave wave;

    struct adsr_config amp_env_config;
    struct triangle_config amp_lfo_config;
    struct adsr_config pitch_env_config;
    struct triangle_config pitch_lfo_config;
    struct adsr_config pwm_env_config;
    struct triangle_config pwm_lfo_config;
    struct glide_config glide_config;

    struct adsr_state amp_env;
    struct triangle_state amp_lfo;
    struct adsr_state pitch_env;
    struct triangle_state pitch_lfo;
    struct adsr_state pwm_env;
    struct triangle_state pwm_lfo;
    struct glide_state glide;

    uint16_t sample_length;
    const uint8_t *sample;

    // Output stuff
    int8_t amplitude;
    int8_t out;
};

#define MIN_PER 300

static struct global_config {
    enum mode mode;
    struct arp_config arp_config;
    int16_t note_offset;
    uint8_t prescaler;
} global_config;

static struct global_state {
    struct osc_state osc[N_OSC];
    int8_t mixer_inputs[N_OSC];
    struct arp_state arp;
} global;

// Adjust the range of the instrument to include more low notes or more high notes
static void global_config_adjust_range(int8_t octaves) {
    global_config.note_offset = (int16_t)octaves * 12;

    if (octaves >= 0) {
        global_config.prescaler = LOOKUP_COUNTS_PER_PERIOD >> octaves;
    } else {
        global_config.prescaler = LOOKUP_COUNTS_PER_PERIOD << (-octaves);
    }
}

static bool osc_handle_timer_pulse(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // For a pulse wave, this function
    // returns the output signal level
    // and updates next_period with the time until this function should be called next

    state->prescaler++;
    if (state->prescaler >= global_config.prescaler) {
        state->prescaler = 0;
    } else {
        return false;
    }

    *next_period = (state->t8 & 1) ? state->timer_period_high : state->timer_period_low;
    *out = (state->t8 & 1) ? state->amplitude : -state->amplitude;
    state->t8++;
    return true;
}

static bool osc_handle_timer_noise(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // For a noise wave, this function
    // returns the output signal level
    // and updates next_period with the time until this function should be called next

    *next_period = (state->t8 & 1) ? state->timer_period_high : state->timer_period_low;
    *out = (rand8() & 1) ? state->amplitude : -state->amplitude;
    state->t8++;
    return true;
}

static bool osc_handle_timer_sample(struct osc_state *state, int8_t *out, volatile uint16_t *next_period) {
    // For a pulse wave, this function
    // returns the output signal level
    // and updates next_period with the time until this function should be called next

    uint16_t byte_index = state->t16 >> 3;
    uint16_t bit_index = state->t16 & 7;

    if (state->t16 >= state->sample_length) {
        *out = 0;
        *next_period = 1250;
        return true;
    }

    if (state->sample[byte_index] & (1 << bit_index)) {
        state->out += 2;
    } else {
        state->out -= 2;
    }

    state->t16++;

    *out = state->out;
    *next_period = state->timer_period_high;
    return true;
}

static uint16_t period(int16_t note) {
    int16_t adj_note = note - (global_config.note_offset << 8);
    // Set osc as per new note
    if (adj_note < 0) {
        adj_note = 0;
    }
    if (adj_note > (72 << 8)) {
        adj_note = (72 << 8);
    }
    uint8_t ix = adj_note >> 8;
    uint16_t per_low = PERIOD_LOOKUP[ix];
    uint16_t per_high = PERIOD_LOOKUP[ix + 1];
    uint8_t frac = adj_note & 0xFF;
    uint16_t per = ((uint32_t)per_low * (256 - frac) + (uint32_t)per_high * frac) >> 8;
    return per;
}

static void global_osc_init() {
    for (int i = 0; i < N_OSC; i++) {
        global.osc[i].wave = WAVE_PULSE;
        global.osc[i].chromatic = true;
        global.osc[i].prescaler = 0;
        global.osc[i].timer_period_high = 1250;
        global.osc[i].timer_period_low = 1250;
        global.osc[i].t8 = 0;
        global.osc[i].t16 = 0;
        global.osc[i].out = 0;
        global.osc[i].amplitude = 0;
        global.osc[i].sample_length = 0;
        global.osc[i].sample = NULL;
        adsr_init(&global.osc[i].amp_env, &global.osc[i].amp_env_config);
        triangle_init(&global.osc[i].amp_lfo, &global.osc[i].amp_lfo_config);
        adsr_init(&global.osc[i].pitch_env, &global.osc[i].pitch_env_config);
        triangle_init(&global.osc[i].pitch_lfo, &global.osc[i].pitch_lfo_config);
        adsr_init(&global.osc[i].pwm_env, &global.osc[i].pwm_env_config);
        triangle_init(&global.osc[i].pwm_lfo, &global.osc[i].pwm_lfo_config);
        glide_init(&global.osc[i].glide, &global.osc[i].glide_config);
    }
}

static void patch_trigger();

static void note_off_mono(uint8_t note_index_to_remove) {
    note_velocity[note_index_to_remove] = 0;

    // See if there's another note playing
    for (int i = 0; i < N_NOTES; i++) {
        if (note_velocity[i] > 0) {
            osc_note_index[0] = note_index[i];
            osc_note_velocity[0] = note_velocity[i];
            return;
        }
    }

    // No notes left playing
    osc_note_velocity[0] = 0;
}

static void note_on_mono(uint8_t k, uint8_t v) {
    note_index[0] = k;
    note_velocity[0] = v;

    osc_note_index[0] = k;
    osc_note_velocity[0] = v;
}

static void note_off_poly(uint8_t note_index_to_remove) {
    uint8_t osc_index = note_osc[note_index_to_remove];
    if (osc_index != NO_OSC) {
        osc_note_velocity[osc_index] = 0;
    }
}

static void assign_osc_poly() {
    // Finds a free oscillator to use and return its index.

    uint8_t osc_index = 0;

    // First, look for osctillators matching the note
    for (int i = 0; i < N_OSC; i++) {
        if (osc_note_index[i] == note_index[0]) {
            osc_index = i;
            goto found;
        }
    }

    // First, look for completely quiet oscillators
    for (int i = 0; i < N_OSC; i++) {
        if (osc_note_velocity[i] == 0 && global.osc[i].amplitude == 0) {
            osc_index = i;
            goto found;
        }
    }

    // Failing that, look for a releasing oscillator
    for (int i = 0; i < N_OSC; i++) {
        if (osc_note_velocity[i] == 0) {
            osc_index = i;
            goto found;
        }
    }

    // Failing that, steal the oldest note's oscillator
    for (int i = N_NOTES - 1; i >= 0; i--) {
        if (note_osc[i] != NO_OSC) {
            osc_index = note_osc[i];
            goto found;
        }
    }

    found:

    // Perform the oscillator (re)assignment
    note_osc[0] = osc_index;
    for (int i = 1; i < N_NOTES; i++) {
        if (note_osc[i] == osc_index) {
            note_velocity[i] = 0;
            note_osc[i] = NO_OSC; // Stolen!
            break;
        }
    }
}

static void note_on_poly(uint8_t k, uint8_t v) {
    // Find a suitable oscillator to use for this new note
    note_index[0] = k;
    note_velocity[0] = v;
    assign_osc_poly();

    uint8_t osc_index = note_osc[0];
    osc_note_index[osc_index] = k;
    osc_note_velocity[osc_index] = v;
}

static void note_off_octave(uint8_t note_index_to_remove) {
    note_velocity[note_index_to_remove] = 0;

    // See if there's another note playing
    for (int i = 0; i < N_NOTES; i++) {
        if (note_velocity[i] > 0) {
            osc_note_index[0] = note_index[i];
            osc_note_velocity[0] = note_velocity[i];
            osc_note_index[1] = note_index[i] - 12;
            osc_note_velocity[1] = note_velocity[i];
            return;
        }
    }

    // No notes left playing
    osc_note_velocity[0] = 0;
    osc_note_velocity[1] = 0;
}

static void note_on_octave(uint8_t k, uint8_t v) {
    note_index[0] = k;
    note_velocity[0] = v;

    osc_note_index[0] = k;
    osc_note_velocity[0] = v;
    osc_note_index[1] = k - 12;
    osc_note_velocity[1] = v;
}

static void all_notes_off() {
    for (int i = 0; i < N_NOTES; i++) {
        note_velocity[i] = 0;
        note_osc[i] = NO_OSC;
    }
    for (int i = 0; i < N_OSC; i++) {
        osc_note_velocity[i] = 0;
    }
}

static void note_on(uint8_t k, uint8_t v) {
    if (v == 0) {
        note_off(k);
        return;
    }

    // Figure out which note entry to replace with the new note.
    // If the note is already in our list, use that entry.
    // If not, default to the oldest slot.
    uint8_t replace_index = N_NOTES - 1;
    for (int i = 0; i < N_NOTES - 1; i++) {
        if (note_index[i] == k) {
            replace_index = i;
            break;
        }
    }

    uint8_t replaced_k = note_index[replace_index];
    uint8_t replaced_v = note_velocity[replace_index];
    uint8_t replaced_osc = note_osc[replace_index];

    // Add new note to beginning and shift everything right by one
    for (int i = replace_index; i >= 1; i--) {
        note_index[i] = note_index[i - 1];
        note_velocity[i] = note_velocity[i - 1];
        note_osc[i] = note_osc[i - 1];
    }

    note_index[0] = replaced_k;
    note_velocity[0] = replaced_v;
    note_osc[0] = replaced_osc;

    // Mode-specific behavior
    switch (global_config.mode) {
        case MODE_MONO:
            note_on_mono(k, v);
            break;
        case MODE_POLY:
            note_on_poly(k, v);
            break;
        case MODE_OCTAVE:
            note_on_octave(k, v);
            break;
    }

    patch_trigger(note_index[0], note_osc[0]);
}

static void note_off(uint8_t k) {
    uint8_t note_index_to_remove = NO_NOTE;
    for (int i = 0; i < N_NOTES; i++) {
        if (note_index[i] == k && note_velocity[i] > 0) {
            note_index_to_remove = i;
            break;
        }
    }

    // If no note was sounding, do nothing
    if (note_index_to_remove == NO_NOTE) {
        return;
    }

    // Mode-specific behavior
    switch (global_config.mode) {
        case MODE_MONO:
            note_off_mono(note_index_to_remove);
            break;
        case MODE_POLY:
            note_off_poly(note_index_to_remove);
            break;
        case MODE_OCTAVE:
            note_off_octave(note_index_to_remove);
            break;
    }

    // Remove from list by shifting left
    for (int i = note_index_to_remove; i < N_NOTES - 1; i++) {
        note_index[i] = note_index[i + 1];
        note_velocity[i] = note_velocity[i + 1];
        note_osc[i] = note_osc[i + 1];
    }
    // Clear out top entry in list
    note_velocity[N_NOTES - 1] = 0;
    note_osc[N_NOTES - 1] = NO_OSC;
}

int
main (void)
{
    // Set up external clock input
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA |= CLKCTRL_CLKSEL_EXTCLK_gc;
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB &= ~CLKCTRL_PEN_bm;

    // Set up TCA0 as first voice oscillator
    TCA0.SINGLE.PER = 1250; // 8 KHz update rate
    TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;

    // Set up TCB0 as second voice oscillator
    TCB0.CCMP = 1250; // Just to get the audio loop going
    TCB0.INTCTRL |= TCB_CAPT_bm;
    TCB0.CTRLA |= TCB_ENABLE_bm;

    // Set up TCB1 as third voice oscillator
    TCB1.CCMP = 1250; // Just to get the audio loop going
    TCB1.INTCTRL |= TCB_CAPT_bm;
    TCB1.CTRLA |= TCB_ENABLE_bm;

    // Set up modulation timer
    TCD0.CMPBCLR = 1250; // 8 KHz update rate
    TCD0.CTRLA |= TCD_ENABLE_bm;

    PORTB.DIRSET = 1 << 0; // Set PB0 as output

    VREF_CTRLA |= VREF_DAC0REFSEL_2V5_gc; // VREF = 2.5V for DAC
    VREF_CTRLB |= VREF_DAC0REFEN_bm; // Force enable VREF

    // Disable everything else on PA6
    PORTA.PIN6CTRL &= ~PORT_ISC_gm;
    PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm;

    // Enable DAC output
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm | DAC_RUNSTDBY_bm;

    // ADC
    ADC0.CTRLA |= ADC_RESSEL_8BIT_gc; // 8 bit resolution
    ADC0.CTRLB |= ADC_PRESC_DIV8_gc; // 1.25MHz ADC clock rate
    ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc; // Use full VDD for reference

    // Disable everything else on PA1
    PORTA.PIN1CTRL &= ~PORT_ISC_gm;
    PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN1CTRL &= ~PORT_PULLUPEN_bm;

    // Disable everything else on PA2
    PORTA.PIN2CTRL &= ~PORT_ISC_gm;
    PORTA.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN2CTRL &= ~PORT_PULLUPEN_bm;

    // Disable everything else on PA4
    PORTA.PIN4CTRL &= ~PORT_ISC_gm;
    PORTA.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN4CTRL &= ~PORT_PULLUPEN_bm;

    ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC

    // Enable UART TX and RX
    // Set PB2 as output
	PORTB.DIRSET = 1 << 2;

    // Invert PB3 input because this transceiver is weird
    PORTB.PIN3CTRL |= PORT_INVEN_bm;

    // Set PB3 pullup
    PORTB.PIN3CTRL |= PORT_PULLUPEN_bm;

    USART0.BAUD = 1280; // 31250 baud @ 10 MHz clock
    USART0.CTRLA |= USART_RXCIE_bm; // Receive data interrupt enable
	USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;	// Enable rx and tx

    // Call this before enabling the timer interrupts
    // because otherwise the global prescaler will be 0
    // which will deadlock the timer ISRs
    update_modulation();

    sei();

    for(;;) {
        asm volatile("": : :"memory");
        if (TCD0.INTFLAGS & TCD_OVF_bm) {
            TCD0.INTFLAGS |= TCD_OVF_bm;
            update_modulation();
        }
    }
}

static struct knobs {
    uint8_t select;
    uint8_t depth;
    uint8_t freq;
} knobs;

static void init_empty() {
    global_config.mode = MODE_POLY;

    global_config_adjust_range(2); // Two octaves up

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        osc->wave = WAVE_PULSE;

        glide_configure(&osc->glide_config, NO_GLIDE_INCREMENT);
        adsr_configure(&osc->pwm_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure_constant(&osc->pwm_lfo_config, 0);

        adsr_configure(&osc->amp_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure_constant(&osc->amp_lfo_config, 0);

        adsr_configure(&osc->pitch_env_config, 0, 1, 0, 1, 0, 1, 0);
        triangle_configure_constant(&osc->pitch_lfo_config, 0);
    }

    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_empty() {
}

static void trigger_empty(uint8_t k, uint8_t osc_index) {
    (void)k;
    (void)osc_index;
}


static void init_basic() {
    global_config.mode = MODE_POLY;

    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 100, 0);
        osc->amp_env_config.s_value = 0;

        // Add some vibrato
        const int16_t et = 6000;
        const int16_t ev = 6000;
        adsr_configure(&osc->pitch_env_config, 0, et, ev, 1, ev, 1, 0);
        const int16_t ot = 400;
        const int16_t ov = 200;
        triangle_configure(&osc->pitch_lfo_config, -ov, ov, 0, ot);
    }
}

static void mod_basic() {
    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Decay based on depth knob
        osc->amp_env_config.d_incr = -(knobs.depth < 127 ? ((int16_t)(127 - knobs.depth) * (127 - knobs.depth) >> 6) + 16 : (255 - knobs.depth) >> 3);
        osc->amp_env_config.s_value = knobs.depth < 255 ? 0 : 64 << 8;

        // Vibrato speed and amount based on freq knob
        int16_t vibrato_amt = knobs.freq << 4;
        osc->pitch_lfo_config.low = -vibrato_amt;
        osc->pitch_lfo_config.high = vibrato_amt;
        osc->pitch_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 11;
    }
}

static void init_chorus() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Basic amplitude envelope
        adsr_configure(&osc->amp_env_config, 0, 1, 127 << 8, 1000, 63 << 8, 100, 0);
    }
}

static void mod_chorus() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // PWM based on depth knob
        int16_t pwm_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
        osc->pwm_lfo_config.low = -pwm_amt;
        osc->pwm_lfo_config.high = pwm_amt;
        osc->pwm_lfo_config.increment = knobs.depth >> 1;

        osc->amp_env_config.a_value = (64 + (knobs.depth >> 2)) << 8;

        // Tremolo speed and amount based on freq knob
        int16_t tremolo_amt = (knobs.freq < 127 ? knobs.freq : 127) << 7;
        osc->amp_lfo_config.low = -tremolo_amt;
        osc->amp_lfo_config.high = tremolo_amt;
        osc->amp_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 8;
    }
}

static void init_wave() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Constant PWM
        triangle_configure(&osc->pwm_lfo_config, -120 << 8, 120 << 8, -120 << 8, 800);

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 100, 64 << 8, 1, 64 << 8, 100, 0);
    }
}

static void mod_wave() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Release based on depth knob
        //osc->amp_env_config.a_incr = (knobs.depth < 127 ? knobs.depth : 127) << 2;
        uint16_t inv_knob = 255 - knobs.freq;
        osc->amp_env_config.r_incr = -(((inv_knob * inv_knob) >> 11) + 1);

        osc->pwm_lfo_config.high = (((int16_t)-120 << 7) + (int16_t)knobs.depth * (240 >> 1)) << 1;
        osc->pwm_lfo_config.initial_value = (osc->pwm_lfo_config.low + osc->pwm_lfo_config.high) >> 1;
        //osc->pwm_lfo_config.increment = ((uint16_t)knobs.depth * knobs.depth) >> 11;
        osc->pwm_lfo_config.increment = knobs.depth >> 3;
    }
}

static void init_bass() {
    global_config_adjust_range(1);
    global_config.mode = MODE_MONO;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 100, 0);

        // Glide set by freq knob
        glide_configure(&osc->glide_config, 1);
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_bass() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Pulse width based on depth knob
        if (i == 0) {
            osc->pwm_env_config.initial_value = (int16_t)(255 - knobs.depth) << 7;
            osc->pwm_env_config.a_value = osc->pwm_env_config.initial_value;
            osc->pwm_env_config.s_value = osc->pwm_env_config.initial_value;
            osc->pwm_env_config.final_value = osc->pwm_env_config.initial_value;
        }

        osc->glide_config.increment = 64 - (knobs.freq >> 2);
    }
}

static void init_wobble_bass() {
    global_config_adjust_range(1);
    global_config.mode = MODE_OCTAVE;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // No glide
        glide_configure(&osc->glide_config, NO_GLIDE_INCREMENT);

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 64 << 8, 1, 64 << 8, 300, 0);

        // No noise channel
        if (i == 2) {
            adsr_configure(&osc->amp_env_config, 0, 1, 0, 1, 0, 1, 0);
        }
    }

    // No arp
    arp_configure(&global_config.arp_config, NULL, 0, 0);
}

static void mod_wobble_bass() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // No glide
        glide_configure(&osc->glide_config, NO_GLIDE_INCREMENT);

        if (i == 0 || i == 1) {
            // Vibrato speed and amount based on freq knob
            int16_t vibrato_amt = knobs.freq << 4;
            osc->pitch_lfo_config.low = -vibrato_amt;
            osc->pitch_lfo_config.high = vibrato_amt;
            osc->pitch_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 11;
        }

        if (i == 0) {
            // PWM based on depth knob
            int16_t pwm_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
            osc->pwm_lfo_config.low = -pwm_amt;
            osc->pwm_lfo_config.high = pwm_amt;
            osc->pwm_lfo_config.increment = knobs.depth >> 1;
        }

        if (i == 1) {
            // Bring in lower octave with depth knob
            uint16_t vol = knobs.depth < 64 ? knobs.depth : 64;
            osc->amp_env_config.a_value = vol << 8;
            osc->amp_env_config.s_value = vol << 8;
        }

        // Tremolo (wobble)
        int16_t tremolo_amt = (knobs.freq < 127 ? knobs.freq : 127) << 7;
        osc->amp_lfo_config.low = -tremolo_amt;
        osc->amp_lfo_config.high = tremolo_amt;
        osc->amp_lfo_config.increment = ((uint16_t)knobs.freq * knobs.freq) >> 8;

        if (i == 2) {
            // Noise channel
        }
    }
}

static void init_pluck_bass() {
    global_config_adjust_range(1);
    global_config.mode = MODE_OCTAVE;

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // Basic amplitude envelope, no amplitude LFO
        adsr_configure(&osc->amp_env_config, 0, 1, 127 << 8, 300, 64 << 8, 100, 0);
        osc->amp_env_config.s_value = 0;

        // Pluck PWM
        if (i == 0) {
            adsr_configure(&osc->pwm_env_config, 127 << 8, 1, 127 << 8, 300, 64 << 8, 100, 0);
            osc->pwm_env_config.s_value = 0;
        }
    }
}

static void mod_pluck_bass() {
    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        // PWM depth based on depth knob
        if (i == 0) {
            osc->pwm_env_config.initial_value = (int16_t)(knobs.depth) << 7;
            osc->pwm_env_config.a_value = osc->pwm_env_config.initial_value;
        }

        // Decay based on freq knob
        uint16_t base_amt = 255 - knobs.freq;
        uint16_t amp_amt = (base_amt * 255) >> 10;
        uint16_t pwm_amt = (base_amt * knobs.depth) >> 10;
        osc->amp_env_config.d_incr = -(int16_t)amp_amt - 1;
        osc->amp_env_config.r_incr = -(int16_t)amp_amt - 1;
        osc->pwm_env_config.d_incr = -(int16_t)pwm_amt - 1;
        osc->pwm_env_config.r_incr = -(int16_t)pwm_amt - 1;
    }
}

static void init_drums() {
    global_config_adjust_range(2);

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        osc->wave = WAVE_NOISE;
        osc->chromatic = false;

        // Turn on the pitch triangle wave at a constant 127
        // since this is multiplied by the pitch envelope.
        // This allows the pitch envelope to do its thing
        triangle_configure_constant(&osc->pitch_lfo_config, 127 << 8);
    }
}

static void trigger_drums(uint8_t k, uint8_t osc_index) {
    struct osc_state *osc = &global.osc[osc_index];

    switch (k) {
        case 35:
        case 36:
            // KICK DRUM
            // Basic amplitude envelope
            // and weep down the pitch of the noise very quickly
            adsr_configure_percussion(&osc->amp_env_config, 127 << 8, 400, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 100 << 8, 50, 0);
            break;
        case 38:
        case 40:
            // SNARE DRUM
            // Basic amplitude envelope
            // and sweep down the pitch
            adsr_configure_percussion(&osc->amp_env_config, 127 << 8, 400, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 100 << 8, 50, 75 << 8);
            break;
        case 42:
            // CLOSED HI-HAT
            // Basic amplitude envelope
            adsr_configure_percussion(&osc->amp_env_config, 127 << 8, 30, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 100 << 8, 1, 100 << 8);
            break;
        default:
            // No sound
            adsr_configure_percussion(&osc->amp_env_config, 0, 1, 0);
            adsr_configure_percussion(&osc->pitch_env_config, 0, 1, 0);
            break;
    }

    adsr_init(&osc->amp_env, &osc->amp_env_config);
    adsr_init(&osc->pitch_env, &osc->pitch_env_config);
}

static void init_sample() {
    global_config_adjust_range(2);

    for (int i = 0; i < N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];

        osc->wave = WAVE_SAMPLE;
    }
}

static void trigger_sample(uint8_t k, uint8_t osc_index) {
    struct osc_state *osc = &global.osc[osc_index];

    // Begin playing sample
    osc->timer_period_high = 350;
    osc->sample_length = sizeof (DOUBLE_DRIBBLE) * 8;
    osc->sample = DOUBLE_DRIBBLE;
    osc->out = 0;
    osc->t16 = 0;
    osc->amplitude = 1; // Mark sample as playing
}

/*

const int8_t arp_octave_notes[] = {-12, 0};
    //adsr_init(&amp_env, 0, 1, 127, 20, 100, 32, 0);
    //adsr_init(&amp_env, 1, 255, 127, 255);
    //triangle_init(&amp_lfo, -100, 100, 30);

    //adsr_init(&pitch_env, 0, 1, 0, 1, 0, 100, -127);
    //triangle_init(&pitch_lfo, -60, 60, 30);
    //adsr_init(&pitch_lfo_env, 0, 100, 100, 1, 100, 1, 100);

    //adsr_init(&mod_env, 127, 100, 64 + 32, 1, 64 + 32, 100, 127);
    //triangle_init(&mod_lfo, 20, 100, 100);
    //mod_lfo.increment = 1;
    //triangle_init(&mod_lfo, 0, 0, 10); // XXX

    // Detune
    triangle_configure(&global_config.pwm_lfo_config, 0, 0, 0, 0);
    triangle_init(&global.pwm_lfo, &global_config.pwm_lfo_config);

    // Amplitude envelope and LFO
    //adsr_configure(&global_amp_env_config, 0, 100, 127 << 8, 2000, 100 << 8, 3200, 0);
    adsr_configure(&global_config.amp_env_config, 0, 0, 127 << 8, 1, 127 << 8, 100, 0);
    //triangle_configure(&global_amp_lfo_config, 10000, 32767, 32767, 100);
    triangle_configure(&global_config.amp_lfo_config, 0, 0, 0, 100);
    triangle_init(&global.amp_lfo, &global_config.amp_lfo_config);

    // Pitch envelope and LFO
    //adsr_configure(&global_pitch_env_config, -256 * 2, 300, 0, 1, 0, 0, 0);
    const int16_t et = 6000;
    const int16_t ev = 6000;
    adsr_configure(&global_config.pitch_env_config, 0, et, ev, 0, ev, 0, ev);
    const int16_t ot = 400;
    const int16_t ov = 200;
    triangle_configure(&global_config.pitch_lfo_config, -ov, ov, 0, ot);
    triangle_init(&global.pitch_lfo, &global_config.pitch_lfo_config);

    //arp_configure(&global_config.arp_config, arp_octave_notes, sizeof (arp_octave_notes), 0);
    //arp_init(&global.arp, &global_config.arp_config);
    arp_configure(&global_config.arp_config, NULL, 0, 0);
    arp_init(&global.arp, &global_config.arp_config);

static void mod_basic() {
    // Detune based on depth knob
    int16_t pwm_amt = (knobs.depth < 12 ? knobs.depth : 12) << 10;
    global_config.pwm_lfo_config.low = -pwm_amt;
    global_config.pwm_lfo_config.high = pwm_amt;
    global_config.pwm_lfo_config.increment = knobs.depth >> 1;

    // Arp speed based on freq knob
    global_config.arp_config.period = (int16_t)knobs.freq * 10;
}
*/

static const struct patches {
    void (*init_fn)();
    void (*mod_fn)();
    void (*trigger_fn)(uint8_t k, uint8_t osc_index);
} patches[N_SELECTOR_POSITIONS] = {
    {
        .init_fn = init_basic,
        .mod_fn = mod_basic,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_chorus,
        .mod_fn = mod_chorus,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_wave,
        .mod_fn = mod_wave,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_bass,
        .mod_fn = mod_bass,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_wobble_bass,
        .mod_fn = mod_wobble_bass,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_empty, // XXX hardware bug: skip this slot
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_pluck_bass,
        .mod_fn = mod_pluck_bass,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_drums,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_drums,
    },
    {
        .init_fn = init_sample,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_sample,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
    {
        .init_fn = init_empty,
        .mod_fn = mod_empty,
        .trigger_fn = trigger_empty,
    },
};

static void patch_init() {
    init_empty();
    patches[knobs.select].init_fn();
}

static void patch_mod() {
    patches[knobs.select].mod_fn();
}

static void patch_trigger() {
    patches[knobs.select].trigger_fn(note_index[0], note_osc[0]);
}

static void read_knobs() {
    static uint8_t last_select = 255;
    static uint8_t last_depth;
    static uint8_t last_freq;

    ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc; // Read from PA1
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    uint8_t read_depth = ADC0.RES >> 2;
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc; // Read from PA2
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    uint8_t read_freq = ADC0.RES >> 2;
    ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc; // Read from PA4
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND & ADC_STCONV_bm);
    uint8_t read_select = ADC0.RES >> 6;
    if (read_select >= N_SELECTOR_POSITIONS) read_select = N_SELECTOR_POSITIONS - 1;

    if (read_depth != last_depth) {
        last_depth = read_depth;
        knobs.depth = read_depth;
    }
    if (read_freq != last_freq) {
        last_freq = read_freq;
        knobs.freq = read_freq;
    }
    if (read_select != last_select) {
        last_select = read_select;
        knobs.select = read_select;
        all_notes_off();
        arp_init(&global.arp, &global_config.arp_config);
        global_osc_init();
        patch_init();
    }
}

// Update modulation
static void update_modulation() {
    read_knobs();

    patch_mod();

    bool notes_held = false;
    for (int i=0; i<N_OSC; i++) {
        notes_held = notes_held || osc_note_velocity[i] > 0;
    }

    // Advance arp
    int8_t arp_note = arp_update(&global.arp, &global_config.arp_config, notes_held);

    // Advance each oscillator
    for (int i=0; i<N_OSC; i++) {
        struct osc_state *osc = &global.osc[i];
        uint8_t note = osc->chromatic ? osc_note_index[i] : 0;
        uint8_t velocity = osc_note_velocity[i];

        // Store triggered velocity
        uint8_t gate = 0;
        if (velocity > 0) {
            osc->triggered_note = note;
            osc->triggered_velocity = velocity;
            gate = note + 1;
        }

        if (osc->wave == WAVE_SAMPLE) {
            if (osc->t16 >= osc->sample_length) {
                // Mark sample as finished
                osc->sample_length = 0;
                osc->amplitude = 0;
            }
        } else {
            // Advance pwm LFO
            uint16_t pwm_lfo_value = triangle_update(&osc->pwm_lfo, &global.osc[i].pwm_lfo_config, notes_held);

            // Advance amplitude LFO
            uint16_t amp_lfo_value = 32768 + (uint16_t)triangle_update(&osc->amp_lfo, &global.osc[i].amp_lfo_config, notes_held);

            // Advance PWM envelope generator
            int16_t pwm_env_value = adsr_update(&osc->pwm_env, &osc->pwm_env_config, gate);

            // Advance amplitude envelope generator
            int16_t amp_env_value = adsr_update(&osc->amp_env, &osc->amp_env_config, gate);

            // Advance pitch LFO
            int16_t pitch_lfo_value = triangle_update(&osc->pitch_lfo, &global.osc[i].pitch_lfo_config, notes_held);

            // Advance pitch envelope generator
            int16_t pitch_env_value = adsr_update(&osc->pitch_env, &osc->pitch_env_config, gate);

            // Apply pitch glide
            int16_t glided_pitch = glide_update(&osc->glide, &osc->glide_config, (osc->triggered_note + arp_note) << 8, notes_held);
            int16_t pitch_adjust = ((int32_t)pitch_env_value * pitch_lfo_value) >> 15;

            // Calculate timer periods
            uint16_t output_period = period(glided_pitch + pitch_adjust);

            if (output_period < 2 * MIN_PER) {
                output_period = 2 * MIN_PER;
            }
            uint16_t pulse_width_multiplier = 32768 + (uint16_t)(pwm_env_value + pwm_lfo_value);
            uint16_t per1 = ((uint32_t)output_period * pulse_width_multiplier) >> 16;
            uint16_t per2 = output_period - per1;
            if (per1 < MIN_PER) {
                per1 = MIN_PER;
                per2 = output_period - MIN_PER;
            } else if (per2 < MIN_PER) {
                per2 = MIN_PER;
                per1 = output_period - MIN_PER;
            }

            int8_t adjusted_velocity = 64 + (osc->triggered_velocity >> 1);

            // Update low-level osc state
            int16_t amp_multiplier = ((int32_t)amp_env_value * amp_lfo_value) >> 16;
            osc->timer_period_high = per1;
            osc->timer_period_low = per2;
            osc->amplitude = ((int32_t)amp_multiplier * adjusted_velocity) >> 15;
        }
    }

    // See if any oscillators are outputting
    bool led_on = false;
    for (int i=0; i<N_OSC; i++) {
        led_on = led_on || global.osc[i].amplitude > 0;
    }

    if (led_on) {
        PORTB.OUT |= 1 << 0;
    } else {
        PORTB.OUT &= ~(1 << 0);
    }

}

static void update_output(void) {
    int16_t out = 0;
    for (int i = 0; i < N_OSC; i++) {
        out += global.mixer_inputs[i];
    }
    DAC0.DATA = 127 + (out >> 2);
}

ISR (TCA0_OVF_vect) 
{
    TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
    switch (global.osc[0].wave) {
        case WAVE_PULSE:
            if (osc_handle_timer_pulse(&global.osc[0], &global.mixer_inputs[0], &TCA0.SINGLE.PER)) {
                update_output();
            }
            break;
        case WAVE_NOISE:
            if (osc_handle_timer_noise(&global.osc[0], &global.mixer_inputs[0], &TCA0.SINGLE.PER)) {
                update_output();
            }
            break;
        case WAVE_SAMPLE:
            if (osc_handle_timer_sample(&global.osc[0], &global.mixer_inputs[0], &TCA0.SINGLE.PER)) {
                update_output();
            }
            break;
    }
}

ISR (TCB0_INT_vect) 
{
    TCB0.INTFLAGS |= TCB_CAPT_bm;
    switch (global.osc[1].wave) {
        case WAVE_PULSE:
            if (osc_handle_timer_pulse(&global.osc[1], &global.mixer_inputs[1], &TCB0.CCMP)) {
                update_output();
            }
            break;
        case WAVE_NOISE:
            if (osc_handle_timer_noise(&global.osc[1], &global.mixer_inputs[1], &TCB0.CCMP)) {
                update_output();
            }
            break;
        case WAVE_SAMPLE:
            if (osc_handle_timer_sample(&global.osc[1], &global.mixer_inputs[1], &TCB0.CCMP)) {
                update_output();
            }
            break;
    }
}

ISR (TCB1_INT_vect) 
{
    TCB1.INTFLAGS |= TCB_CAPT_bm;
    switch (global.osc[2].wave) {
        case WAVE_PULSE:
            if (osc_handle_timer_pulse(&global.osc[2], &global.mixer_inputs[2], &TCB1.CCMP)) {
                update_output();
            }
            break;
        case WAVE_NOISE:
            if (osc_handle_timer_noise(&global.osc[2], &global.mixer_inputs[2], &TCB1.CCMP)) {
                update_output();
            }
            break;
        case WAVE_SAMPLE:
            if (osc_handle_timer_sample(&global.osc[2], &global.mixer_inputs[2], &TCB1.CCMP)) {
                update_output();
            }
            break;
    }
}
