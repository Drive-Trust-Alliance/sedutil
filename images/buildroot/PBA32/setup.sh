#!/bin/bash
rm PBA32/target/etc/init.d/S*
rm PBA32/target/etc/init.d/rcK
sed -i '1,1s/\*//' PBA32/target/etc/shadow
exit 0
