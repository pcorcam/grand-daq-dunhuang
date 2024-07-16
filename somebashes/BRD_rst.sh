#!/bin/bash

cd /sys/class/gpio/

echo 60 > export

echo out > gpio60/direction

echo "[xx] The FEB power will be reset."

echo 1 > gpio60/value

