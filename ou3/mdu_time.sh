#!/bin/bash

# Shell script for OU3, C Programming and Unix - Umea University Autumn 2021
#
# Systematically record execution time of a program with a varying 
# number of threads, and save the formatted output to a file. 
#
# Author: Elias Olofsson (tfy17eon@cs.umu.se)
# Version information:
#   2021-10-15: v1.0, first public version.

max_threads=100 # Vary the number of threads from 1 to max_threads. 
num_samples=10  # Number of samples to record at each thread count.
exec=mdu        # Executable to run
dir=/pkg/       # Directory to estimate disk usage.

filename=time.txt # File to save program timings.
stdout=stdout.txt # File to save stdout stream. 
stderr=stderr.txt # File to save stderr stream.
 
# Compile the program.
make

# Set locale such that points, not commas, are used for floats.
export LC_NUMERIC="en_US.UTF-8"

# Set time formatting for bash time.
TIMEFORMAT='%3R' # Real (wall clock) time elapsed, in seconds.

# Create files or truncate if already existing.
printf "" > $filename
printf "" > $stdout
printf "" > $stderr

# Print info to terminal.
printf "\nSettings: max_threads=%d, num_samples=%d, dir=%s\n\n" "$max_threads" "$num_samples" "$dir" 
printf "Starting tests:\n"

# For each thread count
for ((i = 1; i <= $max_threads; i++))
do
        # Perform a number of samples
        printf "threads = %-3d: Running" "$i"
        printf "%-3d " "$i" >> $filename
        for ((j = 1; j <= $num_samples; j++))
        do
                printf "%s " `{ time ./$exec -j$i $dir >> $stdout 2>> $stderr ; } 2>&1` >> $filename
                printf "."
        done
        printf "\n" >> $filename
        printf " completed.\n"
done
printf "Done.\n"
