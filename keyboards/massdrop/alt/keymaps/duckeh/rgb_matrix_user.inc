RGB_MATRIX_EFFECT(RAINBOW_REACTIVE_DIM)
RGB_MATRIX_EFFECT(RAINBOW_REACTIVE_WHITE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool effect_runner_rainbow_reactive(effect_params_t* params, reactive_f effect_func) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t max_tick = 65535 / rgb_matrix_config.speed;
    uint8_t scale = scale8(64, rgb_matrix_config.hsv.h);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        HSV hsv = rgb_matrix_config.hsv;
        hsv.h = hsv.h + (scale * g_led_config.point[i].x >> 5);
        uint16_t tick = max_tick;
        // Reverse search to find most recent key hit
        for (int8_t j = g_last_hit_tracker.count - 1; j >= 0; j--) {
            if (g_last_hit_tracker.index[j] == i && g_last_hit_tracker.tick[j] < tick) {
                tick = g_last_hit_tracker.tick[j];
                break;
            }
        }

        uint16_t offset = scale16by8(tick, rgb_matrix_config.speed);
        RGB      rgb    = hsv_to_rgb(effect_func(hsv, offset));
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < DRIVER_LED_TOTAL;
}

// Rainbow Reactive (Dim)

static HSV RAINBOW_REACTIVE_DIM_math(HSV hsv, uint16_t offset) {
    hsv.v = qsub8(hsv.v, 255 - offset);
    return hsv;
}

static bool RAINBOW_REACTIVE_DIM(effect_params_t* params) {
    return effect_runner_rainbow_reactive(params, &RAINBOW_REACTIVE_DIM_math);
}

// Rainbow Reactive (White)

static HSV RAINBOW_REACTIVE_WHITE_math(HSV hsv, uint16_t offset) {
    hsv.s = qsub8(hsv.s, 255 - offset);
    return hsv;
}

static bool RAINBOW_REACTIVE_WHITE(effect_params_t* params) {
    return effect_runner_rainbow_reactive(params, &RAINBOW_REACTIVE_WHITE_math);
}




// HSV RAINBOW_REACTIVE_math(HSV hsv, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick) {
//     uint16_t effect = tick + dist * 5;
//     if (effect > 255) effect = 255;
//     //hsv.v -= qadd8(hsv.v, 255 - effect);
//     hsv.v -= qsub8(255, effect);
//     //hsv.s -= qsub8(hsv.s, effect);
//     return hsv;
// }

// bool effect_runner_rainbow_reactive_splash(uint8_t start, effect_params_t* params, reactive_splash_f effect_func) {
//     RGB_MATRIX_USE_LIMITS(led_min, led_max);

//     uint8_t scale = scale8(64, rgb_matrix_config.speed);

//     uint8_t count = g_last_hit_tracker.count;
//     for (uint8_t i = led_min; i < led_max; i++) {
//         RGB_MATRIX_TEST_LED_FLAGS();
//         HSV hsv = rgb_matrix_config.hsv;
//         hsv.h = hsv.h + (scale * g_led_config.point[i].x >> 5);
//         //hsv.v   = 0;
//         for (uint8_t j = start; j < count; j++) {
//             int16_t  dx   = g_led_config.point[i].x - g_last_hit_tracker.x[j];
//             int16_t  dy   = g_led_config.point[i].y - g_last_hit_tracker.y[j];
//             uint8_t  dist = sqrt16(dx * dx + dy * dy);
//             uint16_t tick = scale16by8(g_last_hit_tracker.tick[j], rgb_matrix_config.speed);
//             hsv           = effect_func(hsv, dx, dy, dist, tick);
//         }
//         //hsv.v   = scale8(hsv.v, rgb_matrix_config.hsv.v);
//         RGB rgb = hsv_to_rgb(hsv);
//         rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
//     }
//     return led_max < DRIVER_LED_TOTAL;
// }

// // e.g: A simple effect, self-contained within a single method
// static bool RAINBOW_REACTIVE(effect_params_t* params) {
//     //return effect_runner_rainbow_reactive_splash(0, params, &RAINBOW_REACTIVE_math);
//     return effect_runner_rainbow_reactive(params, &RAINBOW_REACTIVE_math);
//     // RGB_MATRIX_USE_LIMITS(led_min, led_max);

//     // HSV     hsv   = rgb_matrix_config.hsv;
//     // uint8_t scale = scale8(64, rgb_matrix_config.speed);
//     // for (uint8_t i = led_min; i < led_max; i++) {
//     //     RGB_MATRIX_TEST_LED_FLAGS();
//     //     // The x range will be 0..224, map this to 0..7
//     //     // Relies on hue being 8-bit and wrapping
//     //     hsv.h   = rgb_matrix_config.hsv.h + (scale * g_led_config.point[i].x >> 5);
//     //     RGB rgb = hsv_to_rgb(hsv);
//     //     rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
//     // }
//     // return led_max < DRIVER_LED_TOTAL;
// }

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
