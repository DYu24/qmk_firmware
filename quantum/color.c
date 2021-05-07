/* Copyright 2017 Jason Williams
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

#include "color.h"
#include "led_tables.h"
#include "progmem.h"
#include <math.h>

RGB hsv_to_rgb_impl(HSV hsv, bool use_cie) {
    RGB      rgb;
    uint8_t  region, remainder, p, q, t;
    uint16_t h, s, v;

    if (hsv.s == 0) {
#ifdef USE_CIE1931_CURVE
        if (use_cie) {
            rgb.r = rgb.g = rgb.b = pgm_read_byte(&CIE1931_CURVE[hsv.v]);
        } else {
            rgb.r = hsv.v;
            rgb.g = hsv.v;
            rgb.b = hsv.v;
        }
#else
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
#endif
        return rgb;
    }

    h = hsv.h;
    s = hsv.s;
#ifdef USE_CIE1931_CURVE
    if (use_cie) {
        v = pgm_read_byte(&CIE1931_CURVE[hsv.v]);
    } else {
        v = hsv.v;
    }
#else
    v = hsv.v;
#endif

    region    = h * 6 / 255;
    remainder = (h * 2 - region * 85) * 3;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 6:
        case 0:
            rgb.r = v;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = v;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = v;
            rgb.b = t;
            break;
        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = v;
            break;
        case 4:
            rgb.r = t;
            rgb.g = p;
            rgb.b = v;
            break;
        default:
            rgb.r = v;
            rgb.g = p;
            rgb.b = q;
            break;
    }

    return rgb;
}

RGB hsv_to_rgb(HSV hsv) {
#ifdef USE_CIE1931_CURVE
    return hsv_to_rgb_impl(hsv, true);
#else
    return hsv_to_rgb_impl(hsv, false);
#endif
}

RGB hsv_to_rgb_nocie(HSV hsv) { return hsv_to_rgb_impl(hsv, false); }

#ifdef RGBW
#    ifndef MIN
#        define MIN(a, b) ((a) < (b) ? (a) : (b))
#    endif
void convert_rgb_to_rgbw(LED_TYPE *led) {
    // Determine lowest value in all three colors, put that into
    // the white channel and then shift all colors by that amount
    led->w = MIN(led->r, MIN(led->g, led->b));
    led->r -= led->w;
    led->g -= led->w;
    led->b -= led->w;
}
#endif

HSV createUsableHSV(float h, float s, float v) {
    uint8_t h8 = (h / 360) * 255;
    uint8_t s8 = (s / 100) * 255;
    uint8_t v8 = (v / 100) * 255;

    HSV hsv = {h8, s8, v8};
    return hsv;
}

// Convert RGB to HSV
// Adapted from https://www.tutorialspoint.com/c-program-to-change-rgb-color-model-to-hsv-color-model
float max(float a, float b, float c) {
   return ((a > b)? (a > c ? a : c) : (b > c ? b : c));
}

float min(float a, float b, float c) {
   return ((a < b)? (a < c ? a : c) : (b < c ? b : c));
}

HSV rgb_to_hsv(float r, float g, float b) {
    // R, G, B values are divided by 255
    // to change the range from 0..255 to 0..1:
    float h, s, v;
    r /= 255;
    g /= 255;
    b /= 255;
    float cmax = max(r, g, b); // maximum of r, g, b
    float cmin = min(r, g, b); // minimum of r, g, b
    float diff = cmax-cmin; // diff of cmax and cmin.
    if (cmax == cmin)
      h = 0;
    else if (cmax == r)
      h = fmod((60 * ((g - b) / diff) + 360), 360.0);
    else if (cmax == g)
      h = fmod((60 * ((b - r) / diff) + 120), 360.0);
    else if (cmax == b)
      h = fmod((60 * ((r - g) / diff) + 240), 360.0);
    else
      h = 0;

    // if cmax equal zero
    if (cmax == 0)
        s = 0;
    else
        s = (diff / cmax) * 100;

    // compute v
    v = cmax * 100;

    return createUsableHSV(h, s, v);
}
