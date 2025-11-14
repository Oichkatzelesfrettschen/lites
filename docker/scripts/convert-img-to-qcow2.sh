#!/usr/bin/env bash
set -euo pipefail

##
# @file convert-img-to-qcow2.sh
# @brief Convert raw .img disk images to dynamic QCOW2 format
#
# This script converts raw disk images (.img) to QCOW2 format with
# optimal settings for performance and compression. Supports both
# sparse and dynamic allocation.
##

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Default configuration
INPUT_FILE=""
OUTPUT_FILE=""
COMPRESSION=false
COMPAT="1.1"
CLUSTER_SIZE="65536"
PREALLOCATION="off"
LAZY_REFCOUNTS="on"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

##
# @brief Print colored messages
##
log_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*" >&2
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

##
# @brief Display usage information
##
usage() {
    cat <<EOF
Usage: $0 -i INPUT -o OUTPUT [OPTIONS]

Convert raw disk images (.img) to QCOW2 format with optimal settings.

REQUIRED:
    -i, --input FILE        Input .img file (raw format)
    -o, --output FILE       Output .qcow2 file

OPTIONS:
    -c, --compress          Enable compression (slower conversion, smaller file)
    -p, --prealloc MODE     Preallocation mode: off, metadata, falloc, full (default: off)
    -s, --cluster SIZE      Cluster size in bytes (default: 65536)
        --compat VERSION    QCOW2 compatibility: 0.10, 1.1 (default: 1.1)
        --no-lazy-refcounts Disable lazy refcounts (enabled by default for speed)
    -v, --verbose           Verbose output
    -h, --help              Show this help message

PREALLOCATION MODES:
    off       - No preallocation, fully dynamic (default, slowest at runtime)
    metadata  - Preallocate metadata only (good balance)
    falloc    - Use fallocate() for space (fast, requires filesystem support)
    full      - Preallocate all data (fastest runtime, largest initial size)

EXAMPLES:
    # Basic conversion with defaults
    $0 -i lites-boot.img -o lites.qcow2

    # Compressed image (smaller size, slower conversion)
    $0 -i lites-boot.img -o lites.qcow2 --compress

    # Metadata preallocation for better performance
    $0 -i lites-boot.img -o lites.qcow2 --prealloc metadata

    # Full optimization for production
    $0 -i lites-boot.img -o lites.qcow2 --prealloc metadata --cluster 65536

NOTES:
    - Dynamic QCOW2 images grow as data is written
    - Lazy refcounts improve performance by ~5-10%
    - 64KB clusters (65536) are optimal for most workloads
    - Compression reduces size but increases CPU usage
    - Input file format is auto-detected but assumed to be raw

SEE ALSO:
    qemu-img(1), qemu-img-convert(1)
EOF
}

##
# @brief Check for required tools
##
check_dependencies() {
    if ! command -v qemu-img >/dev/null 2>&1; then
        log_error "qemu-img not found. Install qemu-utils package."
        exit 1
    fi
}

##
# @brief Detect input format
##
detect_format() {
    local file="$1"

    if ! qemu-img info "$file" >/dev/null 2>&1; then
        log_error "Cannot read input file: $file"
        exit 1
    fi

    local format
    format=$(qemu-img info "$file" | grep "file format:" | awk '{print $3}')

    log_info "Detected input format: $format"
    echo "$format"
}

##
# @brief Convert image to QCOW2
##
convert_image() {
    local input="$1"
    local output="$2"
    local input_format

    input_format=$(detect_format "$input")

    log_info "Starting conversion..."
    log_info "  Input:  $input ($input_format)"
    log_info "  Output: $output (qcow2)"
    log_info "  Compression: $([[ $COMPRESSION == true ]] && echo "enabled" || echo "disabled")"
    log_info "  Preallocation: $PREALLOCATION"
    log_info "  Cluster size: $CLUSTER_SIZE bytes"
    log_info "  Lazy refcounts: $LAZY_REFCOUNTS"
    log_info "  Compatibility: $COMPAT"

    # Build qemu-img options
    local qcow2_options="lazy_refcounts=${LAZY_REFCOUNTS},cluster_size=${CLUSTER_SIZE},compat=${COMPAT}"

    if [[ "$PREALLOCATION" != "off" ]]; then
        qcow2_options="${qcow2_options},preallocation=${PREALLOCATION}"
    fi

    # Build conversion command
    local convert_cmd=(
        qemu-img convert
        -f "$input_format"
        -O qcow2
        -o "$qcow2_options"
    )

    if [[ $COMPRESSION == true ]]; then
        convert_cmd+=(-c)
    fi

    # Show progress for large files
    convert_cmd+=(-p)

    convert_cmd+=(
        "$input"
        "$output"
    )

    log_info "Running: ${convert_cmd[*]}"

    # Execute conversion
    if "${convert_cmd[@]}"; then
        log_info "Conversion completed successfully!"
    else
        log_error "Conversion failed"
        exit 1
    fi

    # Show file sizes
    log_info ""
    log_info "File size comparison:"
    log_info "  Input:  $(du -h "$input" | cut -f1)"
    log_info "  Output: $(du -h "$output" | cut -f1)"

    # Show image info
    log_info ""
    log_info "Output image details:"
    qemu-img info "$output"
}

##
# @brief Main entry point
##
main() {
    local verbose=false

    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            -i|--input)
                INPUT_FILE="$2"
                shift 2
                ;;
            -o|--output)
                OUTPUT_FILE="$2"
                shift 2
                ;;
            -c|--compress)
                COMPRESSION=true
                shift
                ;;
            -p|--prealloc)
                PREALLOCATION="$2"
                shift 2
                ;;
            -s|--cluster)
                CLUSTER_SIZE="$2"
                shift 2
                ;;
            --compat)
                COMPAT="$2"
                shift 2
                ;;
            --no-lazy-refcounts)
                LAZY_REFCOUNTS="off"
                shift
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                usage
                exit 1
                ;;
        esac
    done

    # Validate required arguments
    if [[ -z "$INPUT_FILE" ]]; then
        log_error "Input file is required (-i/--input)"
        usage
        exit 1
    fi

    if [[ -z "$OUTPUT_FILE" ]]; then
        log_error "Output file is required (-o/--output)"
        usage
        exit 1
    fi

    # Check if input exists
    if [[ ! -f "$INPUT_FILE" ]]; then
        log_error "Input file not found: $INPUT_FILE"
        exit 1
    fi

    # Warn if output exists
    if [[ -f "$OUTPUT_FILE" ]]; then
        log_warn "Output file exists and will be overwritten: $OUTPUT_FILE"
        rm -f "$OUTPUT_FILE"
    fi

    # Check dependencies
    check_dependencies

    # Convert
    convert_image "$INPUT_FILE" "$OUTPUT_FILE"

    log_info ""
    log_info "=== Conversion complete ==="
    log_info "QCOW2 image ready: $OUTPUT_FILE"
    log_info ""
    log_info "To use with QEMU:"
    log_info "  qemu-system-i386 -drive file=$OUTPUT_FILE,format=qcow2 -m 256M"
    log_info ""
}

main "$@"
