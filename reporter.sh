#!/bin/bash

find . -not -path "*/.*" -type f \( -name "*.c" -o -name "*.h" -o -name "*.asm" -o -name "Makefile" -o -name "*.md" \) | while read -r file; do
    echo "========================================"
    echo " FILE: $file"
    echo "========================================"
    cat "$file"
    echo -e "\n"
done > report.txt
