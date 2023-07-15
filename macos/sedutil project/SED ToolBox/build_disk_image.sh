#!/bin/bash
#set -xv

function fail {
    code=$1 ; shift
    echo $@
    exit $code
}
export -f fail

[ -n "${TARGETROOT}" ] || fail 129 "TARGETROOT not defined"
spew "TARGETROOT=${TARGETROOT}"

# Work in temp
[ -d "${TEMP_FILES_DIR}" ] || fail 130 "TEMP_FILES_DIR not available"
spew "TEMP_FILES_DIR=${TEMP_FILES_DIR}"

image_root="${TEMP_FILES_DIR}/DTA"
[ -f "${image_root}" ] && rm -rf "${image_root}"
mkdir "${image_root}"
spew image_root="${image_root}"

# Certificates from the same directory as this script
CERTIFICATES_DIR="$(cd "${build_sh_dir}/Certificates" ; pwd)"
spew "CERTIFICATES_DIR=${CERTIFICATES_DIR}"
[ -d "${CERTIFICATES_DIR}" ] || fail 131 "Could not find Certificates directory"


# Create the macOS subfolder
spew mac_dir="${image_root}/macOS"
mac_dir="${image_root}/macOS"
spew mkdir "${mac_dir}"
mkdir "${mac_dir}"


# Fill out the macOS subfolder

# Copy in the certificates, creating the Certificates subfolder
spew cp -r "${CERTIFICATES_DIR}" "${mac_dir}"
cp -rv "${CERTIFICATES_DIR}" "${mac_dir}" \
    ||  fail 132 "Failed copying Certificates subfolder ${CERTIFICATES_DIR} to ${mac_dir}"
    
# DTATools.pkg
spew cp -v DTATools.pkg "${mac_dir}/DTATools.pkg"
cp -v DTATools.pkg "${mac_dir}/DTATools.pkg"  \
    ||  fail 133 "Failed copying DTATools.pkg to ${mac_dir}"

# .Utilites and .resources and Uninstall
spew cp -r "${TARGETROOT}/.Utilities" "${mac_dir}/.Utilities"
cp -r "${TARGETROOT}/.Utilities" "${mac_dir}/.Utilities"  \
    ||  fail 134 "Failed copying .Utilities to ${mac_dir}"

spew cp -rv "${resources_dir}" "${mac_dir}/.resources"
cp -rv "${resources_dir}" "${mac_dir}/.resources"  \
    ||  fail 135 "Failed copying .resources to ${mac_dir}"

spew cp -v "${resources_dir}/Uninstall" "${mac_dir}/Uninstall"
cp -v "${resources_dir}/Uninstall" "${mac_dir}/Uninstall"  \
    ||  fail 136 "Failed copying Uninstall script to ${mac_dir}"

spew "${resources_dir}/fileicon" set "${mac_dir}/Uninstall" "${resources_dir}/images/UninstallIcon.icns"
"${resources_dir}/fileicon" set "${mac_dir}/Uninstall" "${resources_dir}/images/UninstallIcon.icns"  \
    ||  fail 137 "${resources_dir}/fileicon failed trying to set ${mac_dir}/Uninstall icon resource to be ${resources_dir}/images/UninstallIcon.icns"




# ###################
# spew stop for debugging
# spew exit 99
# exit 99
# ###################

# Windows, Linux, misc. come from static as specified by ST_settings
static="${TARGETROOT}"

dmgbuild="$(which dmgbuild)"
[ -x "${dmgbuild}" ] || \
    dmgbuild="$(2>/dev/null find /opt/local/Library/Frameworks/Python.framework -path '*/bin/dmgbuild' -maxdepth 4 )"
[ -x "${dmgbuild}" ] || \
    dmgbuild="$(2>/dev/null find $HOME/Library/Python -path '*/bin/dmgbuild' -maxdepth 3)"
[ -x "${dmgbuild}" ] || \
    fail 171 "Can not find dmgbuild"
# Convert the macos folder to a macOS dmg

macOS_settings="${static}/macOS settings.py"
[ -f "${macOS_settings}" ] || \
    fail 172 "Can not find ${macOS_settings}"

macOS_dmg="${image_root}/macOS.dmg"
[ -f "${macOS_dmg}" ] && rm -rf "${macOS_dmg}"
spew macOS_dmg="${macOS_dmg}"

macOS_name="SED software by Drive Trust Alliance"
spew pwd = "$(pwd)"
spew "${dmgbuild}" -s "${macOS_settings}" -D static="${static}" -D build="${image_root}" "${macOS_name}" "${macOS_dmg}"
"${dmgbuild}" -s "${macOS_settings}" -D static="${static}" -D build="${image_root}" "${macOS_name}" "${macOS_dmg}" || \
    fail 173 "dmgbuild failed with exit code $?"

# DTA custom icon for the .dmg file
spew Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${macOS_dmg}"
Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${macOS_dmg}" || \
    fail 188 "Rez -append failed with exit code $?"
spew SetFile -a C "${macOS_dmg}"
SetFile -a C "${macOS_dmg}" || \
    fail 189 "SetFile -a C failed with exit code $?"




# Make the containing SED ToolBox dmg

ST_settings="${static}/SED ToolBox settings.py"
[ -f "${ST_settings}" ] || \
    fail 195 "Can not find ${ST_settings}"

ST_dmg="${BUILT_PRODUCTS_DIR}/SED ToolBox.dmg"
[ -f "${ST_dmg}" ] && rm -rf "${ST_dmg}"
spew ST_dmg="${ST_dmg}"

spew "${dmgbuild}" -s "${ST_settings}" -D static="${static}" -D build="${image_root}" "SED ToolBox" "${ST_dmg}"
"${dmgbuild}" -s "${ST_settings}" -D static="${static}" -D build="${image_root}" "SED ToolBox" "${ST_dmg}" || \
    fail 196 "dmgbuild failed with exit code $?"

# DTA custom icon for the .dmg file
# TODO: use hdiutil udifrez instead
spew Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${ST_dmg}"
Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${ST_dmg}" \
    fail 197 "Rez -append failed with exit code $?"
spew SetFile -a C "${ST_dmg}"
SetFile -a C "${ST_dmg}" || \
    fail 198 "SefFile -a C failed with exit code $?"

# Make the containing SED ToolBox iso

config="$("${mac_root}/extract_configuration_type_from_Xcode_build_environment")"
[ -n "${config}" ] || fail 199 "Can not extract configuration type from environment"
ST_iso="${BUILT_PRODUCTS_DIR}/SED ToolBox (${config}).iso"
[ -f "${ST_iso}" ] && rm -rf "${ST_iso}"
spew ST_iso="${ST_iso}"

DTASEDDev="${static}/DTA/SED Developer"
#spew hdiutil makehybrid                               \
#          -iso -joliet                                \
#          -default-volume-name 'SED ToolBox'  \
#          -hide-hfs '{Windows,*.exe}'                 \
#          -o "${ST_iso}" "${ST_dmg}"
#hdiutil makehybrid                               \
#     -iso -joliet                                \
#     -default-volume-name 'SED ToolBox'  \
#     -hide-hfs '{Windows,*.exe}'                 \
#     -o "${ST_iso}" "${ST_dmg}"
spew hdiutil makehybrid                               \
          -default-volume-name 'SED ToolBox'  \
          -joliet-volume-name  'SED_Box'              \
          -o "${ST_iso}" "${ST_dmg}"
hdiutil makehybrid                               \
     -default-volume-name 'SED ToolBox'  \
     -joliet-volume-name  'SED_Box'              \
     -o "${ST_iso}" "${ST_dmg}"
# DTA custom icon for the .iso file
# TODO: use hdiutil udifrez instead
spew Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${ST_iso}"
Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${ST_iso}" || \
    fail 200 "Rez -append failed with exit code $?"
spew SetFile -a C "${ST_iso}"
SetFile -a C "${ST_iso}" || \
    fail 201 "SetFile -a C failed with exit code $?"
echo Created ${ST_iso}
