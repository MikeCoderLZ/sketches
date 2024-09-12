
namespace parse {

    class Base64 {
        public:
            static decode( std::string_view encoded );
            static encode( std::string_view encoded );
        private:
            static unsigned int bitValueStandard( char encodedByte );
    };

    inline
    Base64::encode( std::string_view encoded ) {

        struct Buffer {
            unsigned char bytes[3];
            unsigned char hextet0() { return bytes[0] & 64U;  }
            unsigned char hextet1() { return (bytes[0] >> 6) | (bytes[1] << 2) & 64U };
            unsigned char hextet2() { return (bytes[1] >> 4) | (bytes[2] << 4) & 64U };
            unsigned char hextet3() { return bytes[3] >> 2 };
        } buffer = { 0U, 0U, 0U };

        enum Frames {
            HEX0, HEX1, HEX2, HEX3
        } frame{ HEX3 };

        for( char ch : encoded ) {

            switch( frame ) {
                case HEX0: buffer.sextets.hex0 = Base64::bitvalue( ch ); break;
                case HEX1: buffer.sextets.hex1 = Base64::bitvalue( ch ); break;
                case HEX2: buffer.sextets.hex2 = Base64::bitvalue( ch ); break;
                case HEX3: buffer.sextets.hex3 = Base64::bitvalue( ch ); break;
            }

            if( frame == HEX0 ) {
                // print value of buffer ot output
            }

            --frame;
            frame &= ~3U;
        }        

    }

    inline
    Base64::bitValueStandard( char encodedByte ) {
        switch( ch ) {
                case 'A':return 0U;
                case 'B':return 1U;
                case 'C':return 2U;
                case 'D':return 3U;
                case 'E':return 4U;
                case 'F':return 5U;
                case 'G':return 6U;
                case 'H':return 7U;
                case 'I':return 8U;
                case 'J':return 9U;
                case 'K':return 10U;
                case 'L':return 11U;
                case 'M':return 12U;
                case 'N':return 13U;
                case 'O':return 14U;
                case 'P':return 15U;
                case 'Q':return 16U;
                case 'R':return 17U;
                case 'S':return 18U;
                case 'T':return 19U;
                case 'U':return 20U;
                case 'V':return 21U;
                case 'W':return 22U;
                case 'X':return 23U;
                case 'Y':return 24U;
                case 'Z':return 25U;

                case 'a':
                    break;
            }
    }

} // namespace parse