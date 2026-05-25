
set -euo pipefail
cd "$(dirname "$0")"
BASE="linux_practice"
DOCS="${BASE}/docs"
BACKUP="${BASE}/backup"
mkdir -p "$DOCS" "$BACKUP"
touch "$DOCS/readme.txt" "$DOCS/notes.log" "$DOCS/temp.tmp"
rm -f "$DOCS/temp.tmp"
mv "$DOCS/notes.log" "$DOCS/daily_report.txt"
echo "Project Status: Active" > "$DOCS/daily_report.txt"
date >> "$DOCS/daily_report.txt"
cp "$DOCS"/*.txt "$BACKUP"/
shopt -s nullglob
for f in "$BACKUP"/*; do
  if [[ -f "$f" ]]; then
    chmod 444 "$f"
    echo "Archive Complete. File $(basename "$f") is now read-only."
  fi
done
