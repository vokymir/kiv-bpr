#!/usr/bin/env bash

set -euo pipefail

if [ $# -lt 1 ]; then
  echo "Usage: $0 <file> [--breakdown] [--verbose] [--work|--calendar]"
  exit 1
fi

file="$1"
shift

SHOW_BREAKDOWN=false
SHOW_VERBOSE=false
MODE="calendar"   # default: calendar (24h/7d)

while [ $# -gt 0 ]; do
  case "$1" in
    --breakdown) SHOW_BREAKDOWN=true ;;
    --verbose)   SHOW_VERBOSE=true ;;
    --work)      MODE="work" ;;
    --calendar)  MODE="calendar" ;;
    *) echo "Unknown option: $1" >&2; exit 1 ;;
  esac
  shift
done

# Default: show both if no flag given
if ! $SHOW_BREAKDOWN && ! $SHOW_VERBOSE; then
  SHOW_BREAKDOWN=true
  SHOW_VERBOSE=true
fi

# Configuration based on mode
MINUTES_PER_HOUR=60
# average calendar month length (365.2425 / 12)
MONTH_DAYS=30.436875

if [ "$MODE" = "work" ]; then
  HOURS_PER_DAY=8
  DAYS_PER_WEEK=5
elif [ "$MODE" = "calendar" ]; then
  HOURS_PER_DAY=24
  DAYS_PER_WEEK=7
else
  echo "Unknown mode: $MODE" >&2
  exit 1
fi

MINUTES_PER_DAY=$((HOURS_PER_DAY * MINUTES_PER_HOUR))
MINUTES_PER_WEEK=$((DAYS_PER_WEEK * MINUTES_PER_DAY))

# Sum minutes (safe awk)
total_minutes=$(awk -F'|' '
NR > 1 {
  gsub(/^[ \t]+|[ \t]+$/, "", $2)
  if ($2 ~ /^[0-9]+$/) sum += $2
}
END { print (sum==""?0:sum) }
' "$file")

# Hierarchical integer breakdown (weeks/days/hours/minutes)
weeks=$(( total_minutes / MINUTES_PER_WEEK ))
rem=$(( total_minutes % MINUTES_PER_WEEK ))

days=$(( rem / MINUTES_PER_DAY ))
rem=$(( rem % MINUTES_PER_DAY ))

hours=$(( rem / MINUTES_PER_HOUR ))
minutes=$(( rem % MINUTES_PER_HOUR ))

if $SHOW_BREAKDOWN; then
  echo "Breakdown (${MODE} mode):"
  printf "  %dw %dd %dh %dm\n" "$weeks" "$days" "$hours" "$minutes"
  echo
fi

if $SHOW_VERBOSE; then
  echo "Total: $total_minutes minutes"
  echo

  # Equivalent to: decimal values for hours / days / weeks / months (formatted inside awk to avoid locale issues)
  LC_NUMERIC=C awk \
    -v tm="$total_minutes" \
    -v mpw="$MINUTES_PER_WEEK" \
    -v mpd="$MINUTES_PER_DAY" \
    -v hpd="$HOURS_PER_DAY" \
    -v dpw="$DAYS_PER_WEEK" \
    -v md="$MONTH_DAYS" '
  BEGIN {
    months_minutes = md * hpd * 60
    printf "Equivalent to (decimal):\n"
    printf "  %.2f hours\n", tm/60
    printf "  %.2f days   (1 day = %dh)\n", tm/mpd, hpd
    printf "  %.2f weeks  (1 week = %d days)\n", tm/mpw, dpw
    printf "  %.2f months (1 month = %.2f days, using average calendar month)\n", tm/months_minutes, md
  }'

  echo
  echo "Structured breakdown (integer):"
  printf "  %d week(s)\n" "$weeks"
  printf "  %d day(s)\n"  "$days"
  printf "  %d hour(s)\n" "$hours"
  printf "  %d minute(s)\n" "$minutes"
  echo
fi
