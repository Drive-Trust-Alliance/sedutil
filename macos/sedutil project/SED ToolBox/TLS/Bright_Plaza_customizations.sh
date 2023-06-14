[[ ${_BRIGHT_PLAZA_CUSTOMIZATIONS_ALREADY_SOURCED_:-} ]] && return \
        || readonly _BRIGHT_PLAZA_CUSTOMIZATIONS_ALREADY_SOURCED_=1

. "${cur}/Utility_functions.sh"

CA_NAME="Bright Plaza CA"

KEYCHAIN_NAME="Bright Plaza SED"
KEYCHAIN_PASSWORD_SERVICE="com.BrightPlaza.SED"
SERVER_NAME="Bright Plaza SED Server"

KEYCHAIN_PATH="${SYSTEM_KEYCHAIN_DIR}/${KEYCHAIN_NAME}.keychain"
KEYCHAIN_PASSWORD_LABEL="${KEYCHAIN_NAME} Keychain"



# Applications authorized to access the system keychain item that contains the password of the SED Keychain
TLSTOOL_PATH="/Users/scott/Library/Developer/Xcode/DerivedData/DTA-emaykawkwetfggaxqqnpsfomwgcp/Build/Products/Debug/TLSTool" # TODO: DEBUGGING
SECURITY_PATH="/usr/bin/security"
DAEMON_PATH="/usr/local/libexec/sedd"
SEDDAEMONTEST_PATH="/Applications/Utilities/SEDDaemonTest.app"
SEDPREFERENCESTEST_PATH="/Applications/Utilities/SEDPreferencesTest.app"
SEDACCESS_PATH="/usr/local/libexec/SEDAccess.app"
# SYSTEM_PREFERENCES_PATH="${SYSTEM_APPLICATIONS_DIR}/System Preferences.app"
SYSTEM_PREFERENCES_PATH="${SYSTEM_APPLICATIONS_DIR}/System Settings.app"
