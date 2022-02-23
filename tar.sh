#!/bin/bash
set -eou pipefail
ARGS="-l"

TAR=$(pwd)/.test-read-1.tar

get_files(){
  (find bin -type f) && (find sb -type f)

}
create_tar_file(){
  cmd="tar -cf $TAR $(get_files|tr \"\\\n\" ' ')"
  echo -e "$cmd"
  eval "$cmd"
}

untar_file(){
  cmd="tar xf $TAR $1 -O"
  eval "$cmd"
}

create_tar_file
