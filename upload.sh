#!/bin/bash
PORT=$(arduino-cli board list | grep Leonardo | awk '{print $1}')
echo "Kompilacja programu..."
arduino-cli compile --fqbn arduino:avr:leonardo big_foot_midi_controller.ino
echo "Wgrywanie programu na port $PORT..."
arduino-cli upload -p $PORT --fqbn arduino:avr:leonardo big_foot_midi_controller.ino 