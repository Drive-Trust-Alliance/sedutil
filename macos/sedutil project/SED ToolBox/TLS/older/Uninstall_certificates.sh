#!/bin/bash -xv
sudo security remove-trusted-cert -d './Bright Plaza, Inc. SED Server_CA.pem'
sudo security delete-identity -c 'Bright Plaza, Inc. SED Server' /Library/Keychains/System.keychain
