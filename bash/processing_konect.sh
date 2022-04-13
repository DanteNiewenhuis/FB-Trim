#!/bin/bash

while IFS= read -r file; do
    ./executables/info "KONECT_500-10000/$file" "KONECT"
done < "bash/konect_500-10000.txt"

while IFS= read -r file; 
do
    for i in {0..5}
    do
        # echo "$file"
        ./executables/main "KONECT_500-10000/$file" 4 -NO -Both -log
        ./executables/main "KONECT_500-10000/$file" 4 -NI -Both -log
        ./executables/main "KONECT_500-10000/$file" 4 -A -Both -log
        ./executables/main "KONECT_500-10000/$file" 4 -I -Both -log
        ./executables/main "KONECT_500-10000/$file" 4 -AI -Both -log
    done
done < "bash/konect_500-10000.txt"
