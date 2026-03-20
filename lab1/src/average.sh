#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Количество: 0"
  echo "Среднее: 0"
  exit 0
fi

count=$#
sum=0

for num in "$@"
do
  sum=$((sum + num))
done

average=$(echo "$sum $count" | awk '{printf "%.2f", $1 / $2}')

echo "Количество: $count"
echo "Среднее арифметическое: $average"