# Test output path is valid
testPath() {
    if [ ! -d "${OUTPATH}" ]; then
        echo "The specified directory \"${OUTPATH}\" does not exist"
        exit 1
    fi
}


# Prompt for variables that were not provided in arguments
checkVariables() {
    # Country
    if (( $C_REQUIRED )) && [ -z "${C}" ]
    then
        echo -n "Country Name (2 letter code) []:"
        read C
    fi

    # State
    if (( $ST_REQUIRED )) && [ -z "${ST}" ]
    then
        echo -n "State or Province Name (full name) []:"
        read ST
    fi

    # Locality
    if (( $L_REQUIRED )) && [ -z "${L}" ]
    then
        echo -n "Locality Name (eg, city) []:"
        read L
    fi

    # Organization
    if (( $O_REQUIRED )) && [ -z "${O}" ]
    then
        echo -n "Organization Name (eg, company) []:"
        read O
    fi

    # Organizational Unit
    if (( $OU_REQUIRED )) && [ -z "${OU}" ]
    then
        echo -n "Organizational Unit Name (eg, section) []:"
        read OU
    fi

    # Common Name
    if (( $CN_REQUIRED )) &&  [ -z "${CN}" ]
    then
        echo -n "Common Name (e.g. server FQDN or YOUR name) []:"
        read CN
    fi

    # email address
    if (( emailAddress_REQUIRED )) && [ -z "${emailAddress}" ]
    then
        echo -n "Email Address []:"
        read emailAddress
    fi

    # identity password
    while [ -z "${SERVERIDENTITY_PASSWORD}" ]
    do
        local PASSWORD CONFIRM
        echo -n "Identity password []:"
        read -s PASSWORD
        echo
        echo -n "Confirm identity password []:"
        read -s CONFIRM
        echo
        if [ "${PASSWORD}" != "${CONFIRM}" ]
        then
            echo "Passwords do not match, try again."
        else
            SERVERIDENTITY_PASSWORD="${PASSWORD}"
        fi
    done
}

# Show variable values
showVals() {
    echo "Country: ${C}"
    echo "State: ${ST}"
    echo "Locality: ${L}"
    echo "Organization: ${O}"
    echo "Organization Unit: ${OU}"
    echo "Common Name: ${CN}"
    echo "Email: ${emailAddress}"
    echo "Output Path: ${OUTPATH}"
    echo "Certificate Duration (Days): ${DURATION}"
    echo "Identity Password: ${SERVERIDENTITY_PASSWORD}"
    echo "Verbose: ${VERBOSE}"
}

buildCsrCnf() {
    CSRCNF="${tmp}/tmp.csr.cnf"
    cat << EOF > "${CSRCNF}"
[req]
default_bits = 2048
prompt = no
default_md = sha256
distinguished_name = dn

[dn]
C=${C}
ST=${ST}
L=${L}
O=${O}
CN=${CN}
EOF
[ -z "${OU}" ] || echo "OU=${OU}" >> "${CSRCNF}"
[ -z "${emailAddress}" ] || echo "emailAddress=${emailAddress}" >> "${CSRCNF}"
}

buildExtCnf() {
    EXTCNF="${tmp}/v3.ext"
    cat << EOF > "${EXTCNF}"
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage=digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName=@alt_names
extendedKeyUsage=serverAuth,clientAuth

[alt_names]
DNS.1 = ${CN}
EOF
}

generateCAKey() {
    # Generate CA key & crt
    CAKEY="${tmp}/tmp.key"
    openssl genrsa -out "${CAKEY}" -3 4096
}

generateCACert() {
    CACERT="${OUTPATH}${FILENAME}_CA.pem"
    subj=""
    [ -n "${C}"  ] && subj="${subj}/C=${C}"
    [ -n "${ST}" ] && subj="${subj}/ST=${ST}"
    [ -n "${L}"  ] && subj="${subj}/L=${L}"
    [ -n "${O}"  ] && subj="${subj}/O=${O}"
    [ -n "${OU}" ] && subj="${subj}/OU=${OU}"
    [ -n "${CN}" ] && subj="${subj}/CN=${CN} CA"
    [ -n "${emailAddress}" ] && subj="${subj}/emailAddress=${emailAddress}"
    openssl req -x509 -new -nodes \
            -key "${CAKEY}" \
            -sha256 \
            -days "${DURATION}" \
            -out "${CACERT}" \
            -subj "${subj}"
}

generateServerKeyAndCSR() {
    CSR="${tmp}/tmp.csr"
    SERVERKEY="${OUTPATH}${FILENAME}.key"
    openssl req -new -sha256 -nodes \
            -out "${CSR}" \
            -newkey rsa:2048 \
            -keyout "${SERVERKEY}" \
            -config <( cat "${CSRCNF}" )
}

generateServerCert() {
    SERVERCERT="${OUTPATH}${FILENAME}.crt"
    SERVERSERIAL="${OUTPATH}${FILENAME}.srl"
    openssl x509 -req \
            -in "${tmp}/tmp.csr" \
            -CA "${CACERT}" \
            -CAkey "${CAKEY}" \
            -CAserial "${SERVERSERIAL}" \
            -CAcreateserial \
            -out "${SERVERCERT}" \
            -days "${DURATION}" \
            -sha256 \
            -extfile "${EXTCNF}"
}

generateServerIdentity() {
    SERVERIDENTITY="${OUTPATH}${FILENAME}.p12"
    openssl pkcs12 -export -clcerts \
            -inkey "${SERVERKEY}" \
            -in "${SERVERCERT}" \
            -out "${SERVERIDENTITY}" \
            -name "${CN}" \
            -password "pass:${SERVERIDENTITY_PASSWORD}"
}
