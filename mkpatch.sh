#!/bin/bash
ORIG=/tmp/orig/linux-7.2
SRC=$HOME/Linux/linux-7.2
OUT=$HOME/Linux/sdi2400171_submission/hmwk1.patch

cd "$SRC" || exit 1
# every source file that mentions k22tree or k22info (generated files and hidden files are skipped)
files=$(grep -rIlE 'k22tree|k22info' --exclude-dir=generated --exclude='.*' \
        arch include kernel scripts)

rm -rf /tmp/a /tmp/b && mkdir -p /tmp/a /tmp/b
for f in $files; do
  mkdir -p "/tmp/b/$(dirname "$f")" && cp "$f" "/tmp/b/$f"
  if [ -f "$ORIG/$f" ]; then
    mkdir -p "/tmp/a/$(dirname "$f")" && cp "$ORIG/$f" "/tmp/a/$f"
  fi
done

(cd /tmp && diff -ruN a b) > "$OUT"
grep '^diff ' "$OUT"
