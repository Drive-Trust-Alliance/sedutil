#!/bin/bash -xv
sudo -v security remove-trusted-cert -d './Bright Plaza, Inc. SED Server_CA.pem'
sudo -v security delete-identity -c 'Bright Plaza, Inc. SED Server' /Library/Keychains/System.keychain
