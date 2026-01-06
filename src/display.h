#ifndef DISPLAY_H
#define DISPLAY_H

namespace display
{
    struct display_mode
    {
        int width;
        int height;
        float height_scale;
    };

    extern display_mode mode;

    void init();
    float scaled(float value);
    float scaled_font_height(float height);
}

#endif

