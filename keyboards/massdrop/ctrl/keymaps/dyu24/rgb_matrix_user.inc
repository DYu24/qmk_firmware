RGB_MATRIX_EFFECT(TEAL_SALMON)
RGB_MATRIX_EFFECT(WHITE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS    
    uint8_t BLEND_WIDTH = 33;

    RGB calculateBlendByXPosition(RGB c1, RGB c2, uint8_t x_pos, uint8_t scale) {
        // As a percentage out of 100
        float firstStop = (float)scale / 64.f * 100;
        if (firstStop > 100 - BLEND_WIDTH)
            firstStop = 100 - BLEND_WIDTH;

        float secondStop = firstStop + BLEND_WIDTH;

        float pos = (float)x_pos / 224.f * 100;
        if (pos < firstStop)
            return c1;

        if (pos >= secondStop)
            return c2;

        RGB rgb;
        float blend = (pos - firstStop) / (secondStop - firstStop);
        rgb.r = (blend * (c2.r - c1.r)) + c1.r;
        rgb.g = (blend * (c2.g - c1.g)) + c1.g;
        rgb.b = (blend * (c2.b - c1.b)) + c1.b;
        return rgb;
    }

    static bool TEAL_SALMON(effect_params_t* params) {
        RGB_MATRIX_USE_LIMITS(led_min, led_max);

        HSV tealHsv = createUsableHSV(177, 88.8, 84.3); //"Teal" rgb(24, 215, 204)
        HSV salmonHsv = createUsableHSV(358, 55.3, 100.0); //"Salmon" rgb(255, 114, 118)

        tealHsv.v = rgb_matrix_config.hsv.v;
        salmonHsv.v = rgb_matrix_config.hsv.v;

        RGB tealRgb = hsv_to_rgb(tealHsv);
        RGB salmonRgb = hsv_to_rgb(salmonHsv);

        uint8_t scale = scale8(64, rgb_matrix_config.speed);
        for (uint8_t i = led_min; i < led_max; i++) {
            RGB_MATRIX_TEST_LED_FLAGS();
            RGB rgb = calculateBlendByXPosition(tealRgb, salmonRgb, g_led_config.point[i].x, scale);
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }

        return led_max < DRIVER_LED_TOTAL;
    }

    static bool WHITE(effect_params_t* params) {
        RGB_MATRIX_USE_LIMITS(led_min, led_max);

        HSV whiteHsv = createUsableHSV(62, 30, 100); // White on the slightly warmer side
        whiteHsv.v = rgb_matrix_config.hsv.v;
        RGB whiteRgb = hsv_to_rgb(whiteHsv);

        for (uint8_t i = led_min; i < led_max; i++) {
            RGB_MATRIX_TEST_LED_FLAGS();
            rgb_matrix_set_color(i, whiteRgb.r, whiteRgb.g, whiteRgb.b);
        }

        return led_max < DRIVER_LED_TOTAL;
    }
#endif