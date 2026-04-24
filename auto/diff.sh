#!/usr/bin/env bash

set -e

n_days=3

if [ -n "$1" ]; then
  n_days=$1
else
  read -p "How many days back? [default: 3] " input
  if [ -n "$input" ]; then
    n_days=$input
  fi
fi

commit=$(git rev-list -n 1 --before="$n_days days ago" HEAD)

if [ -z "$commit" ]; then
  echo "No commit found before $n_days days ago"
  exit 1
fi

echo "Using commit: $commit"

echo "Preparing diff/ directory"

mkdir -p "diff"
rm -rf "diff/*"

echo "Using git archive and tar"

git archive "$commit" . | tar -x -C diff

echo "Using typdiff"

typdiff diff/doc.typ doc.typ -o diff/diff.typ

echo "Editing diff functions"

tmp=$(mktemp)

{ 
  cat ../auto/diff.txt
  tail -n +9 diff/diff.typ
} > "$tmp"

mv "$tmp" diff/diff.typ

echo "Compiling pdf"

typst compile diff/diff.typ diff/diff.pdf

echo "Done: diff/diff.pdf"
