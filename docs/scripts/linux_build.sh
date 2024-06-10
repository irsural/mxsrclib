#!/bin/bash

set -e

cur_dir=$(pwd)
script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
docs_path="$(dirname "$script_dir")"

cd "$docs_path"

install_requirements=false

if [ ! -d "venv" ]; then
    install_requirements=true
    echo Create python venv...
    python3 -m venv ./venv
fi

source ./venv/bin/activate

if [ "$install_requirements" == true ]; then
    python3 -m pip install -r requirements.txt
fi

cd "$docs_path/doxygen"
doxygen Doxyfile

cd "$docs_path"
sphinx-build -b html ./ _build/html
