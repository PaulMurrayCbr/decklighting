#ifndef RTC2_H
#define RTC2_H

//#include <Arduino.h>
#include <Wire.h>

class DateTime2 {
  public:
    uint8_t sec;         /* seconds */
    uint8_t min;         /* minutes */
    uint8_t hour;        /* hours */
    uint8_t mday;        /* day of the month */
    uint8_t mon;         /* month */
    int16_t year;        /* year */
    uint8_t wday;        /* day of the week */
    uint8_t yday;        /* day in the year */
    uint8_t isdst;       /* daylight saving time */
    uint8_t year_s;      /* year in short notation*/
};

class RTC2 {
    TwoWire& w;
  public:
    RTC2(TwoWire& _w) : w(_w) {}

    static const int DS3231_TRANSACTION_TIMEOUT  = 100 ;// Wire NAK/Busy timeout in ms

    static const long SECONDS_FROM_1970_TO_2000 = 946684800;

    // i2c slave address of the DS3231 chip
    static const int DS3231_I2C_ADDR           =  0x68;

    // timekeeping registers
    static const int DS3231_TIME_CAL_ADDR     =   0x00;
    static const int DS3231_ALARM1_ADDR       =   0x07;
    static const int DS3231_ALARM2_ADDR       =   0x0B;
    static const int DS3231_CONTROL_ADDR      =    0x0E;
    static const int DS3231_STATUS_ADDR       =    0x0F;
    static const int DS3231_AGING_OFFSET_ADDR =   0x10;
    static const int DS3231_TEMPERATURE_ADDR  =   0x11;

    // control register bits
    static const int  DS3231_CONTROL_A1IE   = 0x01;  /* Alarm 2 Interrupt Enable */
    static const int  DS3231_CONTROL_A2IE   = 0x02;  /* Alarm 2 Interrupt Enable */
    static const int  DS3231_CONTROL_INTCN  = 0x04;  /* Interrupt Control */
    static const int  DS3231_CONTROL_RS1    = 0x08;  /* square-wave rate select 2 */
    static const int  DS3231_CONTROL_RS2    = 0x10;  /* square-wave rate select 2 */
    static const int  DS3231_CONTROL_CONV   = 0x20;  /* Convert Temperature */
    static const int  DS3231_CONTROL_BBSQW  = 0x40;  /* Battery-Backed Square-Wave Enable */
    static const int  DS3231_CONTROL_EOSC   = 0x80 ; /* not Enable Oscillator, 0 equal on */


    // status register bits
    static const int  DS3231_STATUS_A1F     = 0x01 ;  /* Alarm 1 Flag */
    static const int  DS3231_STATUS_A2F     = 0x02 ;  /* Alarm 2 Flag */
    static const int  DS3231_STATUS_BUSY    = 0x04 ;  /* device is busy executing TCXO */
    static const int  DS3231_STATUS_EN32KHZ = 0x08 ;  /* Enable 32KHz Output  */
    static const int  DS3231_STATUS_OSF     = 0x80  ; /* Oscillator Stop Flag */

    void init(const uint8_t creg);
    void set(DateTime2& t);
    void get(DateTime2& t);

    void set_addr(const uint8_t addr, const uint8_t val);
    uint8_t get_addr(const uint8_t addr);

    // control/status register
    void set_creg(const uint8_t val);
    uint8_t get_creg(void);

    void set_sreg(const uint8_t val);
    uint8_t get_sreg(void);

    // aging offset register
    void set_aging(const int8_t val);
    int8_t get_aging(void);

    // temperature register
    float get_treg(void);

    void set_32kHz_output(const uint8_t on);

    // alarms
    void set_a1(const uint8_t s, const uint8_t mi, const uint8_t h, const uint8_t d,
                       const uint8_t * flags);
    void get_a1(char *buf, const uint8_t len);
    void clear_a1f(void);
    uint8_t triggered_a1(void);

    void set_a2(const uint8_t mi, const uint8_t h, const uint8_t d, const uint8_t * flags);
    void get_a2(char *buf, const uint8_t len);
    void clear_a2f(void);
    uint8_t triggered_a2(void);
};

// helpers
uint8_t dectobcd(const uint8_t val);
uint8_t bcdtodec(const uint8_t val);
uint8_t inp2toi(char *cmd, const uint16_t seek);

#endif
