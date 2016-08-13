#!/bin/bash
rm PBA64/target/etc/init.d/S*
rm PBA64/target/etc/init.d/rcK
sed -i '1,1s/\*//' PBA64/target/etc/shadow
exit 0
