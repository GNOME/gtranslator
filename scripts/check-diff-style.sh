#!/bin/bash

TMP_FILE=$(mktemp)
clang-format-diff -p1 -style GNU | tee $TMP_FILE
OUT=$(cat $TMP_FILE | wc -l)
rm $TMP_FILE
(( $OUT == 0 ))
