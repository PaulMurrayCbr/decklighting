#ifndef RGB_H
#define RGB_H

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

class RGB {
  public:
    unsigned char r, g, b;
    RGB(int rr, int gg, int bb) : r(rr), g(gg), b(bb) {}
    RGB() : RGB(64, 64, 192) { }
    char *write(char *p);
    void read(const String &s);
    rgb torgb() {
      rgb out = {r,g,b};
      return out;
    }
};


#endif
