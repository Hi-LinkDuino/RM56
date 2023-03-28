#!/bin/bash

#out: out/xxx.asm

chip=""

case "x$1" in
  xb) chip="ota_boot1" ;;
  xb2) chip="ota_boot2a" ;;
  xa) chip="a7_dsp" ;;
  x|xm) chip="best2600w_aiot" ;;
  *)  echo "Syntax error. Usage: $0 [a|m]"; exit ;;
esac
echo objdump ${chip} to out/${chip}.asm ......

arm-none-eabi-objdump -l -S out/${chip}/${chip}.elf > out/${chip}.asm

echo Done