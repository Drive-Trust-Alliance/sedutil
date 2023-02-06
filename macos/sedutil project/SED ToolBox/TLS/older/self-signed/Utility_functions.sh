# Directories
tmp=`mktemp -d TLS_TEMP.XXXXXXXX`


function safeExit {
    if [ -d "${tmp}" ]
    then
        if [ $VERBOSE -eq 1 ]
        then
            echo "Removing temporary directory '${tmp}'"
        fi
#    rm -rf "${tmp}"   ###### TODO:  Not cleaning out tmp -- restore this
    fi

    trap - INT TERM EXIT
    exit
}
# Test output path is valid
function testPath {
    if [ ! -d "${OUTPATH}" ]; then
        echo "The specified directory \"${OUTPATH}\" does not exist"
        exit 1
    fi
}


# Prompt for variables that were not provided in arguments
function checkVariables {
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
}

function check_or_get_identity_password {
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
function showVals {
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

function generateCAKey {
    # Generate CA key & crt
    CAKEY="${FILEPATH} CA.key"
    openssl genrsa -out "${CAKEY}" 4096
}

function buildCAExtCnf {
    CAEXTCNF="${tmp}/CAv3.ext"
    cat << EOF > "${CAEXTCNF}"
[req]
prompt = no
default_md = sha1
x509_extensions = ex
distinguished_name = dn

[ex]
basicConstraints = critical,CA:TRUE
keyUsage = critical,digitalSignature,keyEncipherment,keyCertSign
extendedKeyUsage = critical,serverAuth

[dn]
EOF
    [ -n "${CN}" ] && echo "CN=${CN} CA" >> "${CAEXTCNF}"
    [ -n "${O}"  ] && echo "O=${O}" >> "${CAEXTCNF}"
    [ -n "${OU}" ] && echo "OU=${OU}" >> "${CAEXTCNF}"
    [ -n "${ST}" ] && echo "ST=${ST}" >> "${CAEXTCNF}"
    [ -n "${C}"  ] && echo "C=${C}" >> "${CAEXTCNF}"
    [ -n "${L}"  ] && echo "L=${L}" >> "${CAEXTCNF}"
    [ -n "${emailAddress}" ] && echo "emailAddress=${emailAddress}" >> "${CAEXTCNF}"
}


function generateCACert {
    CACERT="${FILEPATH} CA.pem"
    # openssl req -x509 -new -nodes \
    #         -key "${CAKEY}" \
    #         -sha256 \
    #         -days "${DURATION}" \
    #         -out "${CACERT}" \
    #         -config "${CAEXTCNF}"
    openssl req -x509 -new -nodes \
            -key "${CAKEY}" \
            -set_serial 1 \
            -sha256 \
            -days "${DURATION}" \
            -out "${CACERT}" \
            -config "${CAEXTCNF}"
}


function buildCsrCnf {
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

function buildServerExtCnf {
    SERVEREXTCNF="${tmp}/v3.ext"
    cat << EOF > "${SERVEREXTCNF}"
keyUsage=critical,digitalSignature, keyEncipherment
extendedKeyUsage=critical,serverAuth
subjectAltName=@alt_names

[alt_names]
DNS.1 = "fawkes1.magnolia-heights.com"
DNS.2 = "localhost"
DNS.3 = "Scott-Markss-MacBook-Pro.local"
IP.1 = "192.168.25.1"
IP.2 = "127.0.0.1"
EOF
}

function generateServerKeyAndCSR {
    CSR="${tmp}/tmp.csr"
    SERVERKEY="${FILEPATH}.key"
    openssl req -new -sha256 -nodes \
            -out "${CSR}" \
            -newkey rsa:2048 \
            -keyout "${SERVERKEY}" \
            -config <( cat "${CSRCNF}" )
}

function generateServerCert {
    SERVERCERT="${FILEPATH}.pem"
    # SERVERSERIAL="${FILEPATH}.srl"
    # openssl x509 -req -sha256 \
    #         -in "${tmp}/tmp.csr" \
    #         -CA "${CACERT}" \
    #         -CAkey "${CAKEY}" \
    #         -CAserial "${SERVERSERIAL}" \
    #         -CAcreateserial \
    #         -set_serial 5 \
    #         -out "${SERVERCERT}" \
    #         -days "${DURATION}" \
    #         -extfile "${SERVEREXTCNF}"
    openssl x509 -req -sha256 \
            -in "${tmp}/tmp.csr" \
            -CA "${CACERT}" \
            -CAkey "${CAKEY}" \
            -set_serial 5 \
            -out "${SERVERCERT}" \
            -days "${DURATION}" \
            -extfile "${SERVEREXTCNF}"
}

function generateServerIdentity {
    SERVERIDENTITY="${FILEPATH}.p12"
    openssl pkcs12 -export -clcerts \
            -inkey "${SERVERKEY}" \
            -in "${SERVERCERT}" \
            -out "${SERVERIDENTITY}" \
            -name "${CN}" \
            -password "pass:${SERVERIDENTITY_PASSWORD}"
}
