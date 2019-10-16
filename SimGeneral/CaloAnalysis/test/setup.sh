#!/usr/bin/env bash

action() {
    # determine the directory of this file
    if [ ! -z "$ZSH_VERSION" ]; then
        local this_file="${(%):-%x}"
    else
        local this_file="${BASH_SOURCE[0]}"
    fi
    local this_dir="$( cd "$( dirname "$this_file" )" && pwd )"

    # ensure that plotlib there
    if [ ! -d "$this_dir/plotlib" ]; then
        ( cd "$this_dir" && git clone https://github.com/riga/plotlib.git )
    fi

    # setup plotlib
    source "$this_dir/plotlib/setup.sh" ""
}
action "$@"
