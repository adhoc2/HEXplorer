TEMPLATE = aux

# C:\Qt\6.7.1\msvc2019_64\bin\windeployqt.exe C:\Users\lmbhoc2\Documents\Code\HEXplorer\HEXplorer_qt6_github\build\Desktop_Qt_6_7_1_MSVC2019_64bit-Release\src\HEXplorer.exe
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
    packages/com.adhoc2.hexplorer/meta/package.xml \
    packages/com.adhoc2.hexplorer/meta/GNU_license.txt \
    packages/com.adhoc2.hexplorer/meta/installscript.qs

FORMS += \
    packages/com.adhoc2.hexplorer/meta/page.ui

RESOURCES += \
    toto.qrc
