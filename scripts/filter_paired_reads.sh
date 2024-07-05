#!/bin/bash 
set -e
BASE_PATH="$(dirname -- "${BASH_SOURCE[0]}")"

"$BASE_PATH"/../tools/bbmap/bbduk.sh -Xmx4g in1="$1" in2="$2" out1="$3" out2="$4" t=4 ref=adapters ktrim=r k=23 mink=11 hdist=1 tpe tbo

exit