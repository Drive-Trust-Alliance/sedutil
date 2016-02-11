#!/bin/bash
rm PBA64/target/etc/init.d/*
sed -i '1,1s/\*//' PBA64/target/etc/shadow
exit 0
