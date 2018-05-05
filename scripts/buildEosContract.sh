#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Command line: buildEosContract.sh <account_name> <contract_name> inside folder"
    exit 1
fi

ACCOUNT=$1
CONTRACT=$2

eosiocpp -o ${CONTRACT}.wast ${CONTRACT}.cpp &&
eosiocpp -g ${CONTRACT}.abi ${CONTRACT}.hpp &&
cleos set contract ${ACCOUNT} ../${CONTRACT}

