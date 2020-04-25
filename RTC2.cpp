#include <Arduino.h>

#include "RTC2.h"

void RTC2::set(DateTime2& t)
{
  uint8_t i, century;

  if (t.year >= 2000) {
    century = 0x80;
    t.year_s = t.year - 2000;
  } else {
    century = 0;
    t.year_s = t.year - 1900;
  }

  uint8_t TimeDate[7] = { t.sec, t.min, t.hour, t.wday, t.mday, t.mon, t.year_s };

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(DS3231_TIME_CAL_ADDR);
  for (i = 0; i <= 6; i++) {
    TimeDate[i] = dectobcd(TimeDate[i]);
    if (i == 5)
      TimeDate[5] += century;
    w.write(TimeDate[i]);
  }
  w.endTransmission();
}

void RTC2::get(DateTime2& t)
{
  uint8_t TimeDate[7];        //second,minute,hour,dow,day,month,year
  uint8_t century = 0;
  uint8_t i, n;
  uint16_t year_full;

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(DS3231_TIME_CAL_ADDR);
  w.endTransmission();

  uint8_t gotData = false;
  uint32_t start = millis(); // start timeout
  while (millis() - start < DS3231_TRANSACTION_TIMEOUT) {
    if (w.requestFrom(DS3231_I2C_ADDR, 7) == 7) {
      gotData = true;
      break;
    }
    delay(2);
  }
  if (!gotData)
    return; // error timeout

  for (i = 0; i <= 6; i++) {
    n = w.read();
    if (i == 5) {
      TimeDate[5] = bcdtodec(n & 0x1F);
      century = (n & 0x80) >> 7;
    } else
      TimeDate[i] = bcdtodec(n);
  }

  if (century == 1) {
    year_full = 2000 + TimeDate[6];
  } else {
    year_full = 1900 + TimeDate[6];
  }

  t.sec = TimeDate[0];
  t.min = TimeDate[1];
  t.hour = TimeDate[2];
  t.mday = TimeDate[4];
  t.mon = TimeDate[5];
  t.year = year_full;
  t.wday = TimeDate[3];
  t.year_s = TimeDate[6];
}

void RTC2::set_addr(const uint8_t addr, const uint8_t val)
{
  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(addr);
  w.write(val);
  w.endTransmission();
}

uint8_t RTC2::get_addr(const uint8_t addr)
{
  uint8_t rv;

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(addr);
  w.endTransmission();

  uint8_t gotData = false;
  uint32_t start = millis(); // start timeout
  while (millis() - start < DS3231_TRANSACTION_TIMEOUT) {
    if (w.requestFrom(DS3231_I2C_ADDR, 1) == 1) {
      gotData = true;
      break;
    }
    delay(2);
  }
  if (!gotData)
    return 0; // error timeout

  rv = w.read();

  return rv;
}



// control register

void RTC2::set_creg(const uint8_t val)
{
  set_addr(DS3231_CONTROL_ADDR, val);
}

uint8_t RTC2::get_creg(void)
{
  uint8_t rv;
  rv = get_addr(DS3231_CONTROL_ADDR);
  return rv;
}

// status register 0Fh/8Fh

/*
  bit7 OSF      Oscillator Stop Flag (if 1 then oscillator has stopped and date might be innacurate)
  bit3 EN32kHz  Enable 32kHz output (1 if needed)
  bit2 BSY      Busy with TCXO functions
  bit1 A2F      Alarm 2 Flag - (1 if alarm2 was triggered)
  bit0 A1F      Alarm 1 Flag - (1 if alarm1 was triggered)
*/

void RTC2::set_sreg(const uint8_t val)
{
  set_addr(DS3231_STATUS_ADDR, val);
}

uint8_t RTC2::get_sreg(void)
{
  uint8_t rv;
  rv = get_addr(DS3231_STATUS_ADDR);
  return rv;
}

// aging register

void RTC2::set_aging(const int8_t val)
{
  uint8_t reg;

  if (val >= 0)
    reg = val;
  else
    reg = ~(-val) + 1;      // 2C

  /*
     At 25°C the aging change of:
     +1 means -0.1ppm
     -1 means -0.1ppm
  */
  set_addr(DS3231_AGING_OFFSET_ADDR, reg);
  /*
     A conversion mut be done to forace the new aging value.
  */
  set_creg(get_creg() | DS3231_CONTROL_CONV);
}

int8_t RTC2::get_aging(void)
{
  uint8_t reg;
  int8_t rv;

  reg = get_addr(DS3231_AGING_OFFSET_ADDR);

  if ((reg & 0x80) != 0)
    rv = reg | ~((1 << 8) - 1);     // if negative get two's complement
  else
    rv = reg;

  return rv;
}

// temperature register

float RTC2::get_treg()
{
  float rv;
  uint8_t temp_msb, temp_lsb;
  int8_t nint;

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(DS3231_TEMPERATURE_ADDR);
  w.endTransmission();

  uint8_t gotData = false;
  uint32_t start = millis(); // start timeout
  while (millis() - start < DS3231_TRANSACTION_TIMEOUT) {
    if (w.requestFrom(DS3231_I2C_ADDR, 2) == 2) {
      gotData = true;
      break;
    }
    delay(2);
  }
  if (!gotData)
    return 0; // error timeout

  temp_msb = w.read();
  temp_lsb = w.read() >> 6;

  if ((temp_msb & 0x80) != 0)
    nint = temp_msb | ~((1 << 8) - 1);      // if negative get two's complement
  else
    nint = temp_msb;

  rv = 0.25 * temp_lsb + nint;

  return rv;
}

void RTC2::set_32kHz_output(const uint8_t on)
{
  /*
     Note, the pin1 is an open drain pin, therfore a pullup
     resitor is required to use the output.
  */
  if (on) {
    uint8_t sreg = get_sreg();
    sreg &= ~DS3231_STATUS_OSF;
    sreg |= DS3231_STATUS_EN32KHZ;
    set_sreg(sreg);
  } else {
    uint8_t sreg = get_sreg();
    sreg &= ~DS3231_STATUS_EN32KHZ;
    set_sreg(sreg);
  }
}

// alarms

// flags are: A1M1 (seconds), A1M2 (minutes), A1M3 (hour),
// A1M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0)
void RTC2::set_a1(const uint8_t s, const uint8_t mi, const uint8_t h, const uint8_t d, const uint8_t * flags)
{
  uint8_t t[4] = { s, mi, h, d };
  uint8_t i;

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(DS3231_ALARM1_ADDR);

  for (i = 0; i <= 3; i++) {
    if (i == 3) {
      w.write(dectobcd(t[3]) | (flags[3] << 7) | (flags[4] << 6));
    } else
      w.write(dectobcd(t[i]) | (flags[i] << 7));
  }

  w.endTransmission();
}

void RTC2::get_a1(char *buf, const uint8_t len)
{
  uint8_t n[4];
  uint8_t t[4];               //second,minute,hour,day
  uint8_t f[5];               // flags
  uint8_t i;

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(DS3231_ALARM1_ADDR);
  w.endTransmission();

  uint8_t gotData = false;
  uint32_t start = millis(); // start timeout
  while (millis() - start < DS3231_TRANSACTION_TIMEOUT) {
    if (w.requestFrom(DS3231_I2C_ADDR, 4) == 4) {
      gotData = true;
      break;
    }
    delay(2);
  }
  if (!gotData)
    return; // error timeout

  for (i = 0; i <= 3; i++) {
    n[i] = w.read();
    f[i] = (n[i] & 0x80) >> 7;
    t[i] = bcdtodec(n[i] & 0x7F);
  }

  f[4] = (n[3] & 0x40) >> 6;
  t[3] = bcdtodec(n[3] & 0x3F);

  snprintf(buf, len,
           "s%02d m%02d h%02d d%02d fs%d m%d h%d d%d wm%d %d %d %d %d",
           t[0], t[1], t[2], t[3], f[0], f[1], f[2], f[3], f[4], n[0],
           n[1], n[2], n[3]);

}

// when the alarm flag is cleared the pulldown on INT is also released
void RTC2::clear_a1f(void)
{
  uint8_t reg_val;

  reg_val = get_sreg() & ~DS3231_STATUS_A1F;
  set_sreg(reg_val);
}

uint8_t RTC2::triggered_a1(void)
{
  return  get_sreg() & DS3231_STATUS_A1F;
}

// flags are: A2M2 (minutes), A2M3 (hour), A2M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0) -
void RTC2::set_a2(const uint8_t mi, const uint8_t h, const uint8_t d, const uint8_t * flags)
{
  uint8_t t[3] = { mi, h, d };
  uint8_t i;

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(DS3231_ALARM2_ADDR);

  for (i = 0; i <= 2; i++) {
    if (i == 2) {
      w.write(dectobcd(t[2]) | (flags[2] << 7) | (flags[3] << 6));
    } else
      w.write(dectobcd(t[i]) | (flags[i] << 7));
  }

  w.endTransmission();
}

void RTC2::get_a2(char *buf, const uint8_t len)
{
  uint8_t n[3];
  uint8_t t[3];               //second,minute,hour,day
  uint8_t f[4];               // flags
  uint8_t i;

  w.beginTransmission(DS3231_I2C_ADDR);
  w.write(DS3231_ALARM2_ADDR);
  w.endTransmission();

  w.requestFrom(DS3231_I2C_ADDR, 4);
  uint8_t gotData = false;
  uint32_t start = millis(); // start timeout
  while (millis() - start < DS3231_TRANSACTION_TIMEOUT) {
    if (w.requestFrom(DS3231_I2C_ADDR, 3) == 3) {
      gotData = true;
      break;
    }
    delay(2);
  }
  if (!gotData)
    return; // error timeout



  for (i = 0; i <= 2; i++) {
    n[i] = w.read();
    f[i] = (n[i] & 0x80) >> 7;
    t[i] = bcdtodec(n[i] & 0x7F);
  }

  f[3] = (n[2] & 0x40) >> 6;
  t[2] = bcdtodec(n[2] & 0x3F);

  snprintf(buf, len, "m%02d h%02d d%02d fm%d h%d d%d wm%d %d %d %d", t[0],
           t[1], t[2], f[0], f[1], f[2], f[3], n[0], n[1], n[2]);

}

// when the alarm flag is cleared the pulldown on INT is also released
void RTC2::clear_a2f(void)
{
  uint8_t reg_val;

  reg_val = get_sreg() & ~DS3231_STATUS_A2F;
  set_sreg(reg_val);
}

uint8_t RTC2::triggered_a2(void)
{
  return  get_sreg() & DS3231_STATUS_A2F;
}

// helpers

uint8_t dectobcd(const uint8_t val)
{
  return ((val / 10 * 16) + (val % 10));
}

uint8_t bcdtodec(const uint8_t val)
{
  return ((val / 16 * 10) + (val % 16));
}

uint8_t inp2toi(char *cmd, const uint16_t seek)
{
  uint8_t rv;
  rv = (cmd[seek] - 48) * 10 + cmd[seek + 1] - 48;
  return rv;
}
