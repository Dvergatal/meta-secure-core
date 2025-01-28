# Set a default root specifier.
inherit user-key-store

SRC_URI:append:class-target = "\
    file://efi-secure-boot.inc \
    file://password.inc \
"

do_install:append:class-target() {
    install -m 0600 "${WORKDIR}/efi-secure-boot.inc" "${D}${EFI_FILES_PATH}"
    install -m 0600 "${WORKDIR}/password.inc" "${D}${EFI_FILES_PATH}"
}

python do_sign:prepend:class-target() {
    bb.build.exec_func("check_deploy_keys", d)
    if d.getVar('GRUB_SIGN_VERIFY') == '1':
        bb.build.exec_func("check_boot_public_key", d)
}

fakeroot python do_sign:class-target() {
    image_dir = d.getVar('D')
    efi_files_path = d.getVar('EFI_FILES_PATH')
    dir = image_dir + efi_files_path + '/'

    uks_bl_sign(dir + 'grub.cfg', d)
    uks_bl_sign(dir + 'boot-menu.inc', d)
    uks_bl_sign(dir + 'efi-secure-boot.inc', d)
    uks_bl_sign(dir + 'password.inc', d)
}

python do_sign() {
}

addtask sign after do_install before do_deploy do_package

fakeroot do_chownboot() {
    chown root:root -R ${D}${EFI_FILES_PATH}/grub.cfg${SB_FILE_EXT}
    chown root:root -R ${D}${EFI_FILES_PATH}/boot-menu.inc${SB_FILE_EXT}
    chown root:root -R ${D}${EFI_FILES_PATH}/efi-secure-boot.inc${SB_FILE_EXT}
    chown root:root -R ${D}${EFI_FILES_PATH}/password.inc${SB_FILE_EXT}
}

addtask chownboot after do_deploy before do_package

do_deploy:append:class-target() {
    # Deploy the stacked grub configs.
    install -m 0600 ${D}${EFI_FILES_PATH}/grub.cfg${SB_FILE_EXT} ${DEPLOYDIR}
    install -m 0600 ${D}${EFI_FILES_PATH}/boot-menu.inc${SB_FILE_EXT} ${DEPLOYDIR}
    install -m 0600 ${D}${EFI_FILES_PATH}/efi-secure-boot.inc ${DEPLOYDIR}
    install -m 0600 ${D}${EFI_FILES_PATH}/password.inc ${DEPLOYDIR}
    install -m 0600 ${D}${EFI_FILES_PATH}/efi-secure-boot.inc${SB_FILE_EXT} ${DEPLOYDIR}
    install -m 0600 ${D}${EFI_FILES_PATH}/password.inc${SB_FILE_EXT} ${DEPLOYDIR}
}

FILES:${PN}:append = "\
    ${EFI_FILES_PATH}/grub.cfg${SB_FILE_EXT} \
    ${EFI_FILES_PATH}/boot-menu.inc${SB_FILE_EXT} \
    ${EFI_FILES_PATH}/efi-secure-boot.inc \
    ${EFI_FILES_PATH}/efi-secure-boot.inc${SB_FILE_EXT} \
    ${EFI_FILES_PATH}/password.inc \
    ${EFI_FILES_PATH}/password.inc${SB_FILE_EXT} \
"
