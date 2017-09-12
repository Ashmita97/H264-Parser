//
//  Bitstream.hpp
//  
//
//  Created by Ashmita Chatterjee on 6/5/17.
//
//

#ifndef Bitstream_h
#define Bitstream_h
class Bitstream {
public:
    void setBuff(unsigned char *buff,int size);
    Bitstream();
    ~Bitstream();
    Bitstream operator [] (const Bitstream& obj);
    int Readbits(int n);
    unsigned int uGolombcode();
    signed int sGolombcode();
    unsigned char operator[]( unsigned int uioffset);
    
private:
    unsigned char *m_puc;
    int m_size;
    int m_bitCount;
    unsigned int m_byteCount;
    bool m_bIsEmulPreventByte;
    
};

#endif /* Bitstream_h */
