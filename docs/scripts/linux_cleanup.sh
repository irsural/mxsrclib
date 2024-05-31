#!/bin/bash

set -e

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
docs_path="$(dirname "$script_dir")"

rm -rf "$docs_path/doxygen/out"
rm -rf "$docs_path/_build"
