TEMPLATE = aux

# C:\Qt\Tools\QtInstallerFramework\4.8\bin\repogen -p path\to\spackages path\HEXplorer_repo
# C:\Qt\Tools\QtInstallerFramework\4.8\bin\repogen.exe --update-new-components -p path\to\packages  path\to\HEXplorer_repo

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
