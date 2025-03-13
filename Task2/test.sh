#!/bin/bash
# Create a large test file (100MB)
dd if=/dev/urandom of=large_test.txt bs=1M count=100
echo "Created large_test.txt (100MB)"
