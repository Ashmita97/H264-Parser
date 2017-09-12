 
 ===================================================
 Bitstream.cpp
 ===================================================
 ===================================================
 
 */

#include "Bitstream.hpp"
#include <math.h>



//This is the default constructor to initialise the variables //

Bitstream::Bitstream()
{
    m_puc=nullptr;
    m_size=0;
    m_bitCount=0;
    m_byteCount=0;
    m_bIsEmulPreventByte=false;
}


//This is the destructor that deletes the pointer pointing to the buffer //

Bitstream::~Bitstream()
{
    delete []m_puc;
}



/*
 *This function initialises buffer and the buffer size:
 
 *  @param buff [in] - Pointer to the character array to extract bits from
 *  @param size [in] - Size of the buffer array
 */

void Bitstream::setBuff( unsigned char *buff,
                        int            size  )
{
    m_puc = buff;
    m_size = size;
}



/*
 *  This function checks how many bits remain to be read from the current byte:
 
 *  @param prebits  [in] - The bits to be skipped before the bits to be extracted
 *  @param n        [in] - Number of bits to be extracted
 *  @param bits_read[in] - Bits to be read from the current byte
 
 *  @return bits_read IF BITS TO BE READ ARE WITHIN THE SAME BYTE
 
 */

int Availablebits( int prebits,
                   int n,
                   int bits_read )
{
    if( bits_read < 8 && ((bits_read+prebits) <= 8) )
        return bits_read;
    else
        return (8 - prebits);
}
/*
 *  This operator overloaded function finds emulation prevention byte code and updates the byte position to read remaining data
 
 * @param uioffset [in] - Position of the current byte
 * @retvalue       [out] - current byte or updated byte position
 */

unsigned char Bitstream::operator[](unsigned int uioffset)
{
    
    if( true == m_bIsEmulPreventByte)
    {
        while(( 3 ==  m_puc[uioffset]) &&  ( 0==  m_puc[uioffset - 1]) &&  ( 0 ==  m_puc[uioffset-2]))
              {
                  uioffset++;
              }
    }
              
    return *(m_puc+ uioffset);
}
              

/*
 *  This function finds the extracted bits in integer form
 
 *  @param n  [in] - Number of bits to be extracted
 *  @retvalue [out]- Extracted bits in integer form
 */

int Bitstream::Readbits(int n)
{
    if((m_byteCount== m_size-1)&&(n>(8-m_bitCount)))
        
        // Error thrown when the bits to be extracted exceeds the number of bits left in the buffer
    {
        return -9999;
    }
    
    unsigned int bits_read=n;          // The number of bits remaining to be read
    int final_value=0;
    unsigned char add_value;           // The value in the buffer element
    unsigned int current_bits;         // The current number of bits being read
    
    while(bits_read>0)
    {
        m_bIsEmulPreventByte=false;
        current_bits= Availablebits(m_bitCount,n,bits_read);
        
        if (int(m_puc[m_byteCount])==3)
            m_bIsEmulPreventByte=true;
        
        add_value = m_puc [m_byteCount];
       
        // Checking if ALL the bits from the current byte have to be read //
        
        if(current_bits!=8)
        {
            add_value = add_value << m_bitCount;// Left shifting by the number of preceding bits
            add_value >>=(8-current_bits);
        }
        
        final_value <<= current_bits;
        final_value |= add_value;
        bits_read   -= current_bits;   // This updates the number of bits read
        m_bitCount +=current_bits;        //  This updates the preceding bits (prebits)
        
        if(m_bitCount>= 8)
        {
            m_bitCount %= 8;
            m_byteCount++;                   // Increments position of the current byte to the next byte if prebits is greater than 8
        }
    }
    return final_value;
    
}

/* This function returns the required unsigned value of the Golumb code */

unsigned int Bitstream::uGolombcode()
{
    int b=0;
    int counter = -1;   //counter to count leading zeros in Golomb code
    while(b==0)
    {
        b=Readbits(1);
        counter++;
    }
    int value = Readbits(counter);     //extracting value of the given bits
    unsigned int codeNum= (1<<counter)-1+ value;
    return codeNum;
}




/* This function returns the required signed value of the Golumb code */

signed int Bitstream::sGolombcode()
{
    unsigned int k= uGolombcode();               //calling uGolombcode function to find required value
    signed int codeNum= pow(-1, k+1)*ceil(k/2);  //calculating sign of given Golomb code value;
    return codeNum;
}



