#pragma once

namespace geni::platform {

    enum OperatingSystem {
        OSX,
        Windows,
        GNULinux,
        GNUHurd,
        GNUFreeBSD,
        Android,
        Unix
    };

    using OS = OperatingSystem;

    enum Architecture {
        x86,
        ARM
    };

    using Arch = Architecture;

    enum ProcessorSize {
        bit16,
        bit32,
        bit64,
    };


    #if defined(__linux__)
            #if defined(__gnu_linux__)
                constexpr OS os{OS::GNULinux};
            #elif defined(__ANDROID__)
                constexpr OS os{OS::Android};
            #endif
    #else

        #if defined(_WIN16)
            constexpr OS os{OS::Windows};
            constexpr ProcessorSize processorSize{bit16};
        #elif defined(_WIN32) || defined(__WIN32__)
            constexpr OS os{OS::Windows};
            constexpr ProcessorSize processorSize{bit32};
        #elif defined(_WIN64)
            constexpr OS os{OS::Windows};
            constexpr ProcessorSize processorSize{bit64};
        #endif
        
    #endif

    constexpr OS os{  }

}