#!/bin/bash
set -xv

function fail {
    code=$1 ; shift
    echo $@
    exit $code
}

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
CERTIFICATES_DIR="$(cd "${cur}/Certificates" ; pwd)"
spew "CERTIFICATES_DIR=${CERTIFICATES_DIR}"
[ -d "${CERTIFICATES_DIR}" ] || fail 131 "Couldn't find Certificates directory"


# Copy in the certificates, creating the Certificates subfolder
spew cp -r "${CERTIFICATES_DIR}" "${image_root}"
cp -r "${CERTIFICATES_DIR}" "${image_root}" \
    ||  fail 132 "Failed copying Certificates subfolder ${CERTIFICATES_DIR}"
certificates="${image_root}/${CERTIFICATES_DIR}"

# Create the macOS subfolder
spew mac_dir="${image_root}/macOS"
mac_dir="${image_root}/macOS"
spew mkdir "${mac_dir}"
mkdir "${mac_dir}"


# Fill out the macOS subfolder

# DTATools.pkg
spew cp -v DTATools.pkg "${mac_dir}/DTA.pkg"
cp -v DTATools.pkg "${mac_dir}/DTA.pkg"  \
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



# iSED builds for Debugging
[ -n "${BUILD_ROOT}" ] || ( echo "BUILD_ROOT not defined" ; exit 1 )
ised="${BUILD_ROOT}/Release-iphoneos/iSED"
spew "ised=${ised}"
if [ -d "${ised}" ]
then
    if [ $ISED_AD_HOC_DISTRIBUTION -eq 1 ]  ## TODO -- WRONG! Need the .ipa
    then
        spew cp -r "${ised}" "${mac_dir}"
        cp -r "${ised}" "${mac_dir}"   \
            ||  fail 161 "Failed copying ${ised} to ${mac_dir}"
    else
        spew rm -rf "${HOME}/Desktop/iSED"
        rm -rf "${HOME}/Desktop/iSED"   \
            ||  fail 162 "Failed removing old ${HOME}/Desktop/iSED"
        spew mv "${ised}" "${HOME}/Desktop/"
        mv "${ised}" "${HOME}/Desktop/"   \
            ||  fail 163 "Failed copying ${ised} to ${HOME}/Desktop/iSED"
    fi
fi

# ###################
# spew stop for debugging
# spew exit 99
# exit 99
# ###################

# Windows, Linux, misc. come from static as specified by SEB_settings
static="${TARGETROOT}"

dmgbuild="$(which dmgbuild)"
[ -x "${dmgbuild}" ] || \
    dmgbuild="$(find /opt/local/Library/Frameworks/Python.framework -path '*/bin/dmgbuild' -maxdepth 4 )"
[ -x "${dmgbuild}" ] || \
    dmgbuild="$(find $HOME/Library/Python -path '*/bin/dmgbuild' -maxdepth 3)"
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

SEB_settings="${static}/SED ToolBox settings.py"
[ -f "${SEB_settings}" ] || \
    fail 195 "Can not find ${SEB_settings}"

SEB_dmg="${BUILT_PRODUCTS_DIR}/SED ToolBox.dmg"
[ -f "${SEB_dmg}" ] && rm -rf "${SEB_dmg}"
spew SEB_dmg="${SEB_dmg}"

spew "${dmgbuild}" -s "${SEB_settings}" -D static="${static}" -D build="${image_root}" "SED ToolBox" "${SEB_dmg}"
"${dmgbuild}" -s "${SEB_settings}" -D static="${static}" -D build="${image_root}" "SED ToolBox" "${SEB_dmg}" || \
    fail 196 "dmgbuild failed with exit code $?"

# DTA custom icon for the .dmg file
# TODO: use hdiutil udifrez instead
spew Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${SEB_dmg}"
Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${SEB_dmg}" \
    fail 197 "Rez -append failed with exit code $?"
spew SetFile -a C "${SEB_dmg}"
SetFile -a C "${SEB_dmg}" || \
    fail 198 "SefFile -a C failed with exit code $?"

# Make the containing SED ToolBox iso

SEB_iso="${BUILT_PRODUCTS_DIR}/SED ToolBox.iso"
[ -f "${SEB_iso}" ] && rm -rf "${SEB_iso}"
spew SEB_iso="${SEB_iso}"

DTASEDDev="${static}/DTA/SED Developer"
#spew hdiutil makehybrid                               \
#          -iso -joliet                                \
#          -default-volume-name 'SED ToolBox'  \
#          -hide-hfs '{Windows,*.exe}'                 \
#          -o "${SEB_iso}" "${SEB_dmg}"
#hdiutil makehybrid                               \
#     -iso -joliet                                \
#     -default-volume-name 'SED ToolBox'  \
#     -hide-hfs '{Windows,*.exe}'                 \
#     -o "${SEB_iso}" "${SEB_dmg}"
spew hdiutil makehybrid                               \
          -default-volume-name 'SED ToolBox'  \
          -joliet-volume-name  'SED_Box'              \
          -hide-hfs '{Windows,*.exe}'                 \
          -o "${SEB_iso}" "${SEB_dmg}"
hdiutil makehybrid                               \
     -default-volume-name 'SED ToolBox'  \
     -joliet-volume-name  'SED_Box'              \
     -hide-hfs '{Windows,*.exe}'                 \
     -o "${SEB_iso}" "${SEB_dmg}"
# DTA custom icon for the .iso file
# TODO: use hdiutil udifrez instead
spew Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${SEB_iso}"
Rez -append "${resources_dir}/images/DTA.VolumeIcon.icns.rsrc" -o "${SEB_iso}" || \
    fail 199 "Rez -append failed with exit code $?"
spew SetFile -a C "${SEB_iso}"
SetFile -a C "${SEB_iso}" || \
    fail 200 "SetFile -a C failed with exit code $?"
echo Created ${SEB_iso}
