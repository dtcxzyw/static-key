#!/usr/bin/bash
set -euo pipefail
shopt -s inherit_errexit

$@ | grep -Pz "key1 disabled.\nkey2 disabled.\n" > /dev/null
KEY1=1 $@ | grep -Pz "key1 enabled.\nkey2 disabled.\n" > /dev/null
KEY2=1 $@ | grep -Pz "key1 disabled.\nkey2 enabled.\n" > /dev/null
KEY1=1 KEY2=1 $@ | grep -Pz "key1 enabled.\nkey2 enabled.\n" > /dev/null
