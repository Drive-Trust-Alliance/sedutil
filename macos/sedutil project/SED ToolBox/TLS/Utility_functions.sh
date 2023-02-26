[[ ${_UTILITY_FUNCTIONS_ALREADY_SOURCED_:-} ]] && return || readonly _UTILITY_FUNCTIONS_ALREADY_SOURCED_=1

SYSTEM_KEYCHAIN_DIR='/Library/Keychains'
SYSTEM_KEYCHAIN_PATH="${SYSTEM_KEYCHAIN_DIR}/System.keychain"

function makeTemp {
    tmp=`mktemp -d TLS_TEMP.XXXXXXXX`
}

function safeExit {
    if [ "${KEEPTEMP}" != "yes" ] && [ -d "${tmp}" ]
    then
        if [ $VERBOSE -eq 1 ]
        then
            echo "Removing temporary directory '${tmp}'"
        fi
        rm -rf "${tmp}"   ###### TODO:  Not cleaning out tmp -- restore this
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

function generate_password {
    local max_punct=3 #avoid really ugly passwords
    local charset='[:alpha:][:digit:][@#$%&;?/<>]'   # 72 chars in charset
    local exceptions='[iIlL1Oo0]'                    # take away 8 confusing characters
    local pwd=""                                     # each is 1 in 64 = 6 bits
    while ! ( [[ "${pwd}" =~ [[:alpha:]] ]]  &&  # at least one alpha
                  [[ "${pwd}" =~ [[:digit:]] ]] && # at least one digit
                  [[ "${pwd}" =~ [[:punct:]] ]] &&  # at least one punctuation
                  [ $( echo ${pwd} | tr -cd '[:punct:]' | wc -c ) -le ${max_punct} ] )  # not too many punctuations
    do
        pwd=$( dd if=/dev/urandom  2>/dev/null | LC_CTYPE=C tr -cd ${charset} | tr -d ${exceptions} | head -c20 )  # try these 20 chars
    done
    echo "${pwd}" # 120 bits of entropy except for the part lost by the selection rules (at least one alpha, etc)
}

function read_password_with_prompt {
    local password
    1>&2 echo -n "$*password:"
    read -s password
    echo "${password}"
}

function get_confirmed_password_from_user {
    local password=
    while true
    do
        password="$( read_password_with_prompt "Enter $* " )"
        local confirm="$( read_password_with_prompt "Confirm $* " )"
        [ "${password}" == "${confirm}" ] && break
        1>&2 echo "Passwords do not match, try again."
    done
    echo "${password}"
}
 
function check_or_get_CA_identity_password {
    CA_IDENTITY_PASSWORD='DTA'  # TODO!"$( get_confirmed_password_from_user "CA identity" )"
}
 
function check_or_get_server_identity_password {
    SERVER_IDENTITY_PASSWORD='De1taTang0A1fa'  # TODO! "$( get_confirmed_password_from_user "SED Server identity" )" and write it out and read it back in later
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
    echo "Server Identity Password: ${SERVER_IDENTITY_PASSWORD}"
    echo "Verbose: ${VERBOSE}"
}

function generateCAKey {
    # Generate CA key & crt
    CAKEY="${CAFILEPATH}.key"
    2>/dev/null openssl genrsa -out "${CAKEY}" 4096
}

function buildCAExtCnf {
    CAEXTCNF="${tmp}/CAv3.ext"
    << EOF > "${CAEXTCNF}" cat
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
    [ -n "${CN}"           ] && >> "${CAEXTCNF}" echo "CN=${CN}"
    [ -n "${O}"            ] && >> "${CAEXTCNF}" echo "O=${O}"
    [ -n "${OU}"           ] && >> "${CAEXTCNF}" echo "OU=${OU}"
    [ -n "${ST}"           ] && >> "${CAEXTCNF}" echo "ST=${ST}"
    [ -n "${C}"            ] && >> "${CAEXTCNF}" echo "C=${C}"
    [ -n "${L}"            ] && >> "${CAEXTCNF}" echo "L=${L}"
    [ -n "${emailAddress}" ] && >> "${CAEXTCNF}" echo "emailAddress=${emailAddress}"
}


function generateCACert {
    CACERT="${CAFILEPATH}.pem"
    openssl req -x509 -new -nodes \
            -key "${CAKEY}" \
            -set_serial 1 \
            -sha256 \
            -days "${DURATION}" \
            -out "${CACERT}" \
            -config "${CAEXTCNF}"
}


function generateCAIdentity {
    CAIDENTITY="${CAFILEPATH}.p12"
    openssl pkcs12 -export  \
            -inkey "${CAKEY}" \
            -in "${CACERT}" \
            -out "${CAIDENTITY}" \
            -name "${CN}" \
            -password "pass:${CA_IDENTITY_PASSWORD}"
}

function buildCsrCnf {
    CSRCNF="${tmp}/tmp.csr.cnf"
    << EOF > "${CSRCNF}" cat
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
[ -z "${OU}" ]           |  >> "${CSRCNF}" echo "OU=${OU}"
[ -z "${emailAddress}" ] || >> "${CSRCNF}" echo "emailAddress=${emailAddress}"
}

function buildServerExtCnf {
    SERVEREXTCNF="${tmp}/v3.ext"
    cat << EOF > "${SERVEREXTCNF}"
keyUsage=critical,digitalSignature, keyEncipherment
extendedKeyUsage=critical,serverAuth
subjectAltName=@alt_names

[alt_names]
${LOCAL_NETWORK}
EOF
}

function generateServerKeyAndCSR {
    CSR="${tmp}/tmp.csr"
    SERVERKEY="${FILEPATH}.key"
    2>/dev/null openssl req -new -sha256 -nodes \
            -out "${CSR}" \
            -newkey rsa:2048 \
            -keyout "${SERVERKEY}" \
            -config <( cat "${CSRCNF}" )
}

function generateServerCert {
    CACERT="${CAFILEPATH}.pem"
    CAKEY="${CAFILEPATH}.key"
    SERVERCERT="${FILEPATH}.pem"
    2>/dev/null openssl x509 -req -sha256 \
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
            -password "pass:${SERVER_IDENTITY_PASSWORD}"
}

function fail {
    1>&2 echo "Failed $1"
    exit $2
}


function warn {
    1>&2 echo "Failed $1"
}

function unlock_keychain {
    2>/dev/null sudo security unlock-keychain -p "${KEYCHAIN_PASSWORD}"  "${KEYCHAIN_PATH}"
}

function delete_keychain {
    2>/dev/null sudo security delete-keychain  "${KEYCHAIN_PATH}"
}

function read_keychain_password_from_system_keychain {
    2>/dev/null sudo security find-generic-password -l "${KEYCHAIN_PASSWORD_LABEL}" -w "${SYSTEM_KEYCHAIN_PATH}"
}

function delete_keychain_password_from_system_keychain {
    2>/dev/null 1>&2 sudo security delete-generic-password -l "${KEYCHAIN_PASSWORD_LABEL}"  "${SYSTEM_KEYCHAIN_PATH}"
}
