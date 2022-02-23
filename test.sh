#!/bin/bash
set -eou pipefail
ARGS="-l"

while read -r f; do
	for b in pexec pexec.static; do
		cmd="./bin/$b < $f $ARGS |grep sessions -A 999 -B 999 && ansi --green --bold --bg-black --underline \"$b $f OK\""
		echo >&2 -e "$(ansi --yellow --bg-black --italic "$cmd")"
		eval "$cmd" || true
	done
	for b in execelf execelf.static; do
		cmd="./bin/$b $f $ARGS |grep sessions -A 999 -B 999 && ansi --green --bold --bg-black --underline \"$b $f OK\""
		echo >&2 -e "$(ansi --yellow --bg-black --italic "$cmd")"
		eval "$cmd" || true
	done
done < <(
	find sb -type f
	find bin -type f | grep client
)
