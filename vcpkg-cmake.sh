#!/bin/bash

set -euo pipefail

# CMake wrapper for vcpkg

SCRIPT_DIR=$(dirname "$0")
VCPKG_BASELINE=98aa6396292d57e737a6ef999d4225ca488859d5

function clone_vcpkg() {
  mkdir -p "${SCRIPT_DIR}/.vcpkg"
  pushd "${SCRIPT_DIR}/.vcpkg" >/dev/null
  git init -q
  git remote add origin https://github.com/microsoft/vcpkg.git
  git fetch -q origin "${VCPKG_BASELINE}" --depth 1
  git checkout -q FETCH_HEAD
  popd >/dev/null
}

function setup_vcpkg() {
  [[ -d "${SCRIPT_DIR}/.vcpkg" ]] || clone_vcpkg
  [[ -f "${SCRIPT_DIR}/.vcpkg/vcpkg" ]] || "${SCRIPT_DIR}/.vcpkg/bootstrap-vcpkg.sh" -disableMetrics
  export VCPKG_ROOT=${SCRIPT_DIR}/.vcpkg
}

[[ "${VCPKG_ROOT:-}" != "" ]] || setup_vcpkg
CMAKE=$("${VCPKG_ROOT}/vcpkg" fetch cmake --x-stderr-status)

exec "${CMAKE}" "$@"
