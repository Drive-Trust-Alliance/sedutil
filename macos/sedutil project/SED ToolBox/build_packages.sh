#!/bin/bash

version=${VERSION:-2.0}
spew "version=${version}"
prodcert=${PRODCERT:-"Developer ID Installer: Bright Plaza, Inc. (UES63D9K7E)"}
spew "prodcert=${prodcert}"

spew cd "${BUILT_PRODUCTS_DIR}"
cd "${BUILT_PRODUCTS_DIR}"

spew "pwd=>$(pwd)"

verbose=0

(( verbose )) && ( cd .. ; find . )

verbose=1

TARGETROOT="${SRCROOT}/${TARGETNAME}"
bundles_dir="./bundles"
spew "bundles_dir=${bundles_dir}"
bundles_scripts_dir="${TARGETROOT}/bundles_scripts"
spew "bundles_scripts_dir=${bundles_scripts_dir}"
spew pkgbuild --root "${bundles_dir}"                                           \
              --scripts "${bundles_scripts_dir}"                                \
              --component-plist "${TARGETROOT}/DTABundles.plist"                \
              --identifier com.brightplaza.dtabundles                           \
              --version "${version}"                                            \
              --install-location /                                              \
              --ownership recommended                                           \
              DTAbundles.pkg
pkgbuild --root "${bundles_dir}"                                                \
         --scripts "${bundles_scripts_dir}"                                     \
         --component-plist "${TARGETROOT}/DTABundles.plist"                     \
         --identifier com.brightplaza.dtabundles                                \
         --version "${version}"                                                 \
         --install-location /                                                   \
         --ownership recommended                                                \
         DTAbundles.pkg  ||  exit 1

files_dir="./files"
spew "files_dir=${files_dir}"
files_scripts_dir="${TARGETROOT}/files_scripts"
spew "files_scripts_dir=${files_scripts_dir}"
spew pkgbuild --root "${files_dir}"                                             \
              --scripts "${files_scripts_dir}"                                  \
              --identifier com.brightplaza.dtafiles                             \
              --version "${version}"                                            \
              --install-location /                                              \
              --ownership recommended                                           \
              DTAfiles.pkg
pkgbuild --root "${files_dir}"                                                  \
         --scripts "${files_scripts_dir}"                                       \
         --identifier com.brightplaza.dtafiles                                  \
         --version "${version}"                                                 \
         --install-location /                                                   \
         --ownership recommended                                                \
         DTAfiles.pkg  ||  exit 1


# certs_dir="./certs"
# spew "certs_dir=${certs_dir}"
# certs_scripts_dir="${TARGETROOT}/certs_scripts"
# spew "certs_scripts_dir=${certs_scripts_dir}"
# spew pkgbuild --root "${certs_dir}"                                             \
#               --scripts "${certs_scripts_dir}"                                  \
#               --identifier com.brightplaza.dtacerts                             \
#               --version "${version}"                                            \
#               --install-location /                                              \
#               --ownership recommended                                           \
#               DTAcerts.pkg
# pkgbuild --root "${certs_dir}"                                                  \
#          --scripts "${certs_scripts_dir}"                                       \
#          --identifier com.brightplaza.dtacerts                                  \
#          --version "${version}"                                                 \
#          --install-location /                                                   \
#          --ownership recommended                                                \
#          DTAcerts.pkg  ||  exit 1

resources_dir="${TARGETROOT}/resources"
spew "resources_dir=${resources_dir}"
distribution_XML="${TARGETROOT}/distribution.xml"
spew "distribution_XML=${distribution_XML}"

spew productbuild --distribution "${distribution_XML}"                          \
                  --resources "${resources_dir}"                                \
                  --version "${version}"                                        \
                  --sign "${prodcert}"                                          \
                  --timestamp                                                   \
                  DTA.pkg
productbuild --distribution "${distribution_XML}"                               \
             --resources "${resources_dir}"                                     \
             --version "${version}"                                             \
             --sign "${prodcert}"                                               \
             --timestamp                                                        \
             DTA.pkg  ||  exit 1
