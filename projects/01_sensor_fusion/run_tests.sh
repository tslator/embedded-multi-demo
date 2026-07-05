#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

LOCAL_RUBY_BIN="$WORKSPACE_ROOT/ruby/installation/bin"

export PATH="$LOCAL_RUBY_BIN:$PATH"

echo "===================================================="
echo "Initializing isolated Ceedling environment (Bash)..."
echo "Using Ruby at: $LOCAL_RUBY_BIN"
echo "===================================================="

bundle exec ceedling test:all