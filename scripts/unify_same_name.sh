#!/usr/bin/env bash

#
# unify_same_name.sh -- unify duplicated filenames within a Git tree
#
# This utility searches the working tree for basenames that occur more than
# once. It examines each group of files sharing a basename and either marks
# byte-for-byte duplicates for deletion or launches meld to merge divergent
# copies.
#
# Usage: ./unify_same_name.sh
#
# Dependencies: fd (fd-find), fdupes, meld, diffutils

set -euo pipefail

## \brief Ensure all required external commands are available.
check_dependencies() {
    local missing=0 # track if any tool is unavailable
    for cmd in fd fdupes meld diff3; do
        if ! command -v "$cmd" >/dev/null 2>&1; then
            echo "Error: required command '$cmd' not found" >&2
            missing=1
        fi
    done
    return $missing
}

## \brief Collect basenames that appear multiple times.
## \return Null-delimited list of repeated basenames on stdout.
get_repeated_basenames() {
    fd --type f -0 | xargs -0 -n1 basename -a | sort -z | uniq -dz
}

## \brief Retrieve every file path that has the given basename.
## \param base The basename to search for.
## \param paths[out] Name of the array to populate via nameref.
get_paths_for_basename() {
    local base="$1"       # basename to locate
    local -n paths_ref=$2 # reference to caller-provided array

    # Gather all paths matching the basename
    mapfile -d '' -t paths_ref < <(
        fd --type f -0 | grep -z "/${base}$"
    )
}

## \brief Determine whether a set of paths are all byte-identical.
## \param paths Name of the array containing file paths.
## \retval 0 if all paths are identical, non-zero otherwise.
are_all_identical() {
    local -n paths_ref=$1
    fdupes -r1 -q "${paths_ref[@]}" >/dev/null
}

## \brief Launch meld to merge a list of paths.
## \param paths Name of the array containing file paths.
launch_meld() {
    local -n paths_ref=$1

    # Meld handles up to three paths comfortably.
    if ((${#paths_ref[@]} <= 3)); then
        meld "${paths_ref[@]}"
    else
        # Process in batches of three for larger groups.
        local i=0
        while ((i < ${#paths_ref[@]})); do
            meld "${paths_ref[@]:i:3}" &
            ((i += 3))
        done
    fi
}

## \brief Process a single basename group.
## \param base The basename being processed.
process_basename() {
    local base="$1"
    local paths=()
    get_paths_for_basename "$base" paths

    echo "\n\033[1;36m» Group: $base\033[0m"

    if are_all_identical paths; then
        echo "  ✔ All duplicates are binary-identical – you may safely delete extras:"
        printf "    %s\n" "${paths[@]:1}"
        return
    fi

    echo "  ⚡ Divergent copies detected – launching meld for synthesis…"
    launch_meld paths
}

## \brief Main entry point.
unify_same_name_main() {
    check_dependencies || exit 1
    local repeated_basenames=()
    mapfile -d '' -t repeated_basenames < <(get_repeated_basenames)

    for base in "${repeated_basenames[@]}"; do
        process_basename "$base"
    done
}

unify_same_name_main "$@"
