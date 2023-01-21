#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_BUILD 2
#define VERSION_SUFFIX "Beta"

#define STRINGIFY_VERSION(A, B, C) CONCAT(A, B, C )
#define CONCAT(A, B, C ) STRINGIFY( A##.##B##.##C )
#define STRINGIFY(A) #A

#define STR_VERSION STRINGIFY_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD) "-" VERSION_SUFFIX

#define VER_FILEVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,0
#define STR_FILEVERSION STR_VERSION

// Keep the product version as fixed
#define VER_PRODUCTVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,0
#define STR_PRODUCTVERSION STR_VERSION

#define STR_COMPANYNAME "RxLaboratory"
#define STR_FILEDESCRIPTION "DuBuilder"
#define STR_INTERNALNAME "DuBuilder"
#define STR_LEGALCOPYRIGHT "Copyright (c) 2020-2022 RxLaboratory, Nicolas Dufresne and contributors"
#define STR_LEGALTRADEMARKS1 "All Rights Reserved"
#define STR_ORIGINALFILENAME "DuBuilder.exe"
#define STR_PRODUCTNAME "DuBuilder - JSX(inc) scripts builder"

#define STR_COMPANYDOMAIN "rxlaboratory.org"

#define URL_CHAT "http://chat.rxlab.info"
#define URL_DOC "https://dubuilder.rxlab.guide"
#define URL_BUGREPORT "https://github.com/Rainbox-dev/DuAEF_DuBuilder/issues/new/choose"

#endif // VERSION_H
