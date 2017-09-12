//
//  Parsing.cpp
//  H.264 Parser
//
//
#include<iostream>
#include "VideoParser.h"
#include "Bitstream.h"
#include <string.h>
unsigned int ruiStartCodeLen;
using namespace std;

unsigned char* VideoParser::FindSliceStart( unsigned char *pucBuffer, unsigned int  uiFrameSize, unsigned int &ruiStartCodeLen)
{
    unsigned int uiStart  = 0;
    ruiStartCodeLen = 0;
    
    
    for ( uiStart = 2; uiStart < uiFrameSize; uiStart++ )
    {
        /**
         * Verify the start code 001
         */
        if ( ( 1 == pucBuffer[uiStart] ) &&
            ( 0 == pucBuffer[uiStart-1] ) &&
            ( 0 == pucBuffer[uiStart-2] ) )
        {
            if ( uiStart > 2 )
            {
                ruiStartCodeLen = ( ( 0 == pucBuffer[uiStart-3] ) ) ? 4 : 3;
            }
            else
            {
                ruiStartCodeLen = 3;
            }
            
            uiStart++;
            
            /**
             * Return slice start
             */
            return ( pucBuffer + uiStart );
        }
    }
    /**
     * Return slice start
     */
    return ( pucBuffer + uiFrameSize );
}
int VideoParser::GetNaluType(unsigned char *pucBuffer,int size)
{
    string nal_type = "";
    int nal_unit_type = -1;
    unsigned char *pucbuff = NULL;
    pucbuff =  FindSliceStart(pucBuffer,size,ruiStartCodeLen );
    Bitstream oBitstream ;
    oBitstream.setBuff(pucbuff,size-ruiStartCodeLen);
    if (oBitstream.Readbits(1)==0) // read forbidden bits
    {
        cout<<endl<<" forbidden_zero_bit : 0"<<endl;
        //nal_ref_idc;
        int nal_ref_idc =oBitstream.Readbits(2);
        cout<<" nal_ref_idc : "<<nal_ref_idc<<endl;
        
        nal_unit_type = oBitstream.Readbits(5);
        cout<<" nal_unit_type : "<<nal_unit_type<<endl;
        
        switch(nal_unit_type)
        {
                
            case 1:
                cout << " ( Coded slice of a non IDR picture ) " << endl;
                nal_type = "IDR";
                break;
            case 5:
                cout << " ( Coded slice of an IDR picture ) " << endl;
                nal_type = "NON_IDR";
                break;
                
            case 7:
                cout << " ( Sequence parameter set ) " << endl;
                nal_type = "SPS";
                break;
                
            case 8:
                cout << " ( Picture parameter set ) " << endl;
                nal_type = "PPS";
                break;
                
                
            default :
                cout << " ( Unspecified NAL unit ) " << endl;
                
        }
    }
    
    return(nal_unit_type);
}

