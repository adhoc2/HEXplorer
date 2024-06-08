TEMPLATE = aux

# compile project to get an installer file
# deploy repo on
# C:\Qt\Tools\QtInstallerFramework\4.8\bin\repogen -p packages C:\Users\lmbhoc2\Documents\Code\HEXplorer\HEXplorer_repo
# copy on distant distant_repo_path = https://lmbweb.liebherr.i/team/WS5/Shared/OBD/Sharepoint AC41/FaultCodeTable/OBD Calibration Tool/HEXplorer_repo

# cd C:\Users\lmbhoc2\Documents\Code\HEXplorer\HEXplorer_installer_qt6
# path\repogen.exe -p packages local_repo_path

# when update components : update version in config.xml and package.xml
# compile project to get an installer file
# update the repo locally : C:\Qt\Tools\QtInstallerFramework\4.8\bin\repogen.exe --update-new-components -p packages C:\Users\lmbhoc2\Documents\Code\HEXplorer\HEXplorer_repo

INSTALLER = HEXplorer_online_installer

INPUT = $$PWD/config/config.xml $$PWD/packages
HEXplorer_online_installer.input = INPUT
HEXplorer_online_installer.output = $$INSTALLER
HEXplorer_online_installer.commands = C:\Qt\Tools\QtInstallerFramework\4.8\bin/binarycreator --online-only -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}

QMAKE_EXTRA_COMPILERS += HEXplorer_online_installer

OTHER_FILES = README

DISTFILES += \
    packages/com.liebherr.hexplorer/meta/package.xml \
    packages/com.liebherr.hexplorer/meta/GNU_license.txt \
    packages/com.liebherr.hexplorer/meta/installscript.qs

FORMS += \
    packages/com.liebherr.hexplorer/meta/page.ui

RESOURCES += \
    toto.qrc
