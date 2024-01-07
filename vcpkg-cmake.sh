#!/bin/bash

set -euo pipefail

# CMake wrapper for vcpkg

SCRIPT_DIR=$(dirname "$0")

function setup_vcpkg() {
  [[ -d "${SCRIPT_DIR}/.vcpkg" ]] || git clone https://github.com/microsoft/vcpkg.git --depth 1 ${SCRIPT_DIR}/.vcpkg
  [[ -f "${SCRIPT_DIR}/.vcpkg/vcpkg" ]] || "${SCRIPT_DIR}/.vcpkg/bootstrap-vcpkg.sh" -disableMetrics
  export VCPKG_ROOT=${SCRIPT_DIR}/.vcpkg
}

[[ "${VCPKG_ROOT:-}" != "" ]] || setup_vcpkg
CMAKE=$("${VCPKG_ROOT}/vcpkg" fetch cmake --x-stderr-status)

exec "${CMAKE}" "$@"
