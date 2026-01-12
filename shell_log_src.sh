#!/bin/bash

LOG_FILE="/home/yasmine/Desktop/Linux_Project/FileLogging.txt"

DURATION=250         
INTERVAL=0.5        

: > "$LOG_FILE"      

START_TIME=$(date +%s)

while (( $(date +%s) - START_TIME < DURATION )); do
    VALUE=$(awk -v r=$RANDOM 'BEGIN { printf "%.2f", r/32767*100 }')

	echo "$VALUE"
    echo "$VALUE" >> "$LOG_FILE"
    sleep "$INTERVAL"
done