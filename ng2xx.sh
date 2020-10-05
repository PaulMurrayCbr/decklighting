#!/bin/bash



for f in ng/dist/ng/*
do
  echo "$f"
  ff=${f//\./_}
  echo "$ff"
  bb=`basename "$ff"`
#  gzip < "$f" | xxd -i -

  echo "$bb"

	cat > "$bb".h << FOO
extern const unsigned char ${bb}_DATA[];
extern const int ${bb}_LEN[];
FOO

	cat > "$bb".c << FOO1
#include <Arduino.h>

const unsigned char ${bb}_DATA[] PROGMEM = {
FOO1

  gzip < "$f" | xxd -i >> "$bb".c

	cat >> "$bb".c << FOO2
};

const int ${bb}_LEN = sizeof(${bb}_DATA);
FOO2

done
