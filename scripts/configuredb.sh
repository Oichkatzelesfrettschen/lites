#!/usr/bin/env bash
set -euo pipefail

CONFIG_FILE="configuredb.conf"
DB_FILE="configuredb.sqlite"

if [ ! -f "$CONFIG_FILE" ]; then
  cat > "$CONFIG_FILE" <<EOC
[database]
path=$DB_FILE
EOC
fi

if [ ! -f "$DB_FILE" ]; then
  sqlite3 "$DB_FILE" "CREATE TABLE IF NOT EXISTS settings (key TEXT PRIMARY KEY, value TEXT);"
fi

exit 0
