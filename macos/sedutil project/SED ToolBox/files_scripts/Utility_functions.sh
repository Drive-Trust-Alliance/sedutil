[[ ${_UTILITY_FUNCTIONS_ALREADY_SOURCED_:-} ]] && return || readonly _UTILITY_FUNCTIONS_ALREADY_SOURCED_=1

function osVersion {
    case "${OSTYPE}" in
        darwin*)
            echo "${OSTYPE/darwin/}"
            ;;
        *)
            echo "Sorry, Operating system type ${OSTYPE} not supported\!" >2
            exit 1
            ;;
    esac
}

function findSystemApplicationsDirectory {
    local ver=$(osVersion)
    if (( ${ver} <= 17 ))
    then
        SYSTEM_APPLICATIONS_DIR='/Applications'
    elif (( 20 <= ${ver} ))
    then
        SYSTEM_APPLICATIONS_DIR='/System/Applications'
    else
        echo 'Need to check on osVersion 18 and 19' >2
        SYSTEM_APPLICATIONS_DIR='/dev/null'
        exit 1
    fi
}
findSystemApplicationsDirectory

VERBOSE=${VERBOSE:-0}
SYSTEM_KEYCHAIN_DIR='/Library/Keychains'
SYSTEM_KEYCHAIN_PATH="${SYSTEM_KEYCHAIN_DIR}/System.keychain"

function file_exists {
    [ -f "${1}" ]
}

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
    if [ ! -d "${CERTIFICATES_DIR_PATH}" ]; then
        echo "The specified directory \"${CERTIFICATES_DIR_PATH}\" does not exist"
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
    if (( $SERVER_NAME_REQUIRED )) &&  [ -z "${SERVER_NAME}" ]
    then
        echo -n "Common Name (e.g. server FQDN or YOUR name) []:"
        read SERVER_NAME
    fi

    # CA Common Name
    if (( $CACN_REQUIRED )) &&  [ -z "${CACN}" ]
    then
        echo -n "CA Common Name (e.g. Big Organization CA) []:"
        read CACN
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
    echo "${pwd}" # 120 bits of entropy
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

# popup alert to GUI
function confirm_replace {
    <<EOF  1>/dev/null 2>&1 osascript - "$@"
on run argv
    tell app (path to frontmost application as text)
        set prompt to "$1"
        if 0 < count of argv then set prompt to item 1 of argv
        display alert prompt buttons {"Cancel", "Replace"} as critical default button "Replace" cancel button "Cancel" giving up after 30
        if button returned of result is "Replace" then return
        error -128 -- osascript error
    end tell
end run
EOF
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
    echo "Common Name: ${SERVER_NAME}"
    echo "CA Common Name: ${CACN}"
    echo "Email: ${emailAddress}"
    echo "Output Path: ${CERTIFICATES_DIR_PATH}"
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
keyUsage = critical,digitalSignature,nonRepudiation,keyEncipherment,dataEncipherment,keyCertSign,cRLSign
extendedKeyUsage = critical,serverAuth

[dn]
EOF
    [ -n "${CACN}"         ] && >> "${CAEXTCNF}" echo "CN=${CACN}"
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
            -name "${SERVER_NAME}" \
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
CN=${SERVER_NAME}
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
    SERVER_KEY="${SERVER_FILEPATH}.key"
    2>/dev/null openssl req -new -sha256 -nodes \
            -out "${CSR}" \
            -newkey rsa:2048 \
            -keyout "${SERVER_KEY}" \
            -config <( cat "${CSRCNF}" )
}

function generateServerCert {
    CACERT="${CAFILEPATH}.pem"
    CAKEY="${CAFILEPATH}.key"
    SERVER_CERT="${SERVER_FILEPATH}.pem"
    2>/dev/null openssl x509 -req -sha256 \
            -in "${tmp}/tmp.csr" \
            -CA "${CACERT}" \
            -CAkey "${CAKEY}" \
            -set_serial 5 \
            -out "${SERVER_CERT}" \
            -days "${DURATION}" \
            -extfile "${SERVEREXTCNF}"
}

function generateServerIdentity {
    SERVER_IDENTITY_PATH="${SERVER_FILEPATH}.p12"
    openssl pkcs12 -export -clcerts \
            -inkey "${SERVER_KEY}" \
            -in "${SERVER_CERT}" \
            -out "${SERVER_IDENTITY_PATH}" \
            -name "${SERVER_NAME}" \
            -password "pass:${SERVER_IDENTITY_PASSWORD}"
}

function add_server_identity_to_SED_keychain {
    SERVER_FILENAME="${SERVER_NAME/\*\./}"
    SERVER_FILEPATH="${CERTIFICATES_DIR_PATH}${SERVER_FILENAME}"
    SERVER_IDENTITY_PATH="${SERVER_FILEPATH}.p12"
    SERVER_IDENTITY_PASSWORD='De1taTang0A1fa'  # TODO! "$( generate_password )" and save it and read it in later
    >/dev/null sudo security import "${SERVER_IDENTITY_PATH}" -f pkcs12 \
        -k "${KEYCHAIN_PATH}" \
        -P "${SERVER_IDENTITY_PASSWORD}" \
        -T "${DAEMON_PATH}"
}

function find_server_certificate_in_SED_keychain {
    2>/dev/null sudo security find-certificate -p ssl-server -s "${SERVER_NAME}" "${KEYCHAIN_PATH}"
}

function find_server_identity_in_SED_keychain {
    2>/dev/null sudo security find-identity -p ssl-server -s "${SERVER_NAME}" "${KEYCHAIN_PATH}"
}

function fail {
    1>&2 echo "Failed $1"
    exit $2
}


function warn {
    1>&2 echo "Failed $1"
}

function SED_keychain_exists {
    file_exists "${KEYCHAIN_PATH}"
}

function unlock_SED_keychain {
    2>/dev/null sudo security unlock-keychain -p "${KEYCHAIN_PASSWORD}"  "${KEYCHAIN_PATH}"
}

function delete_SED_keychain {
    2>/dev/null sudo security delete-keychain  "${KEYCHAIN_PATH}"
}

function read_SED_keychain_password_from_system_keychain {
   2>/dev/null sudo security find-generic-password -l "${KEYCHAIN_PASSWORD_LABEL}" -w "${SYSTEM_KEYCHAIN_PATH}"
}


function delete_SED_keychain_password_from_system_keychain {
    1>/dev/null 2>&1 sudo security delete-generic-password -l "${KEYCHAIN_PASSWORD_LABEL}"  "${SYSTEM_KEYCHAIN_PATH}"
}

function find_CA_cert {
    sudo security  find-certificate -c "${CA_NAME}" "${SYSTEM_KEYCHAIN_PATH}"
}

function add_CA_cert_to_system_keychain {
    CAFILENAME="${CA_NAME/\*\./}"
    CAFILEPATH="${CERTIFICATES_DIR_PATH}${CAFILENAME}.pem"
    sudo security add-trusted-cert -d \
         -r "trustRoot" \
         -p "basic" \
         -p "ssl" \
         -k "${SYSTEM_KEYCHAIN_PATH}" \
         "${CAFILEPATH}"
}

function remove_CA_cert_from_system_keychain {
    CAFILENAME="${CA_NAME/\*\./}"
    sudo security -v delete-certificate -t -c "Bright Plaza CA"  "${SYSTEM_KEYCHAIN_PATH}" ||
    sudo security -v delete-certificate    -c "Bright Plaza CA"  "${SYSTEM_KEYCHAIN_PATH}"
}

function save_SED_keychain_password_in_system_keychain_item {

    KEYCHAIN_PASSWORD_ACCOUNT="${KEYCHAIN_NAME}"

    sudo security -v add-generic-password  \
         -l "${KEYCHAIN_PASSWORD_LABEL}"   \
         -a "${KEYCHAIN_PASSWORD_ACCOUNT}" \
         -s "${KEYCHAIN_PASSWORD_SERVICE}" \
         -w "${KEYCHAIN_PASSWORD}"         \
         \
         -T "${SECURITY_PATH}"           \
         -T "${DAEMON_PATH}"             \
         -T "${SEDDAEMONTEST_PATH}"      \
         -T "${SEDPREFERENCESTEST_PATH}" \
         -T "${SEDACCESS_PATH}"          \
         -T "${SYSTEM_PREFERENCES_PATH}" \
         -T "${KEYCHAIN_ACCESS_PATH}"    \
         \
         "${SYSTEM_KEYCHAIN_PATH}"

}

function create_SED_keychain {
    sudo security create-keychain -p "${KEYCHAIN_PASSWORD}" "${KEYCHAIN_PATH}"
}
