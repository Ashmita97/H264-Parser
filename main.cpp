#include<iostream>
#include<fstream>
#include<string.h>
#include<cstdio>
#include<stdio.h>

#include "Bitstream.h"
#include "VideoParser.h"
#include "H264.h"

using namespace std;

int main()
{
    cout<<"Main"<<endl;
    unsigned char buf[10000]={0};
    unsigned char *pbuf= buf;
    int pos = 0;
    bool bIsGotFirstNaluOfTheStream=true;
    FILE *Fp;
    
    Fp = fopen("/Users/Ashmita/Code/Parser/Parser/strh.264","rb");
    unsigned int start_code=0;
    
    if( NULL == Fp )
    {
        printf("File open failed, exiting the application\n");
    }
    else
    {
        printf("File open successfully \n");
    }
    int size=0;
    
    while (!feof(Fp))
    {
        size++;
        if(feof(Fp))
        break;
        
        fread(pbuf+pos,1,1,Fp);
    
        if( pos >= 2)
            
        {
            if((*(pbuf+pos) == 1) && (*(pbuf+pos-1) == 0) && (*(pbuf+pos-2) == 0))
            {
                if ( bIsGotFirstNaluOfTheStream)
                {
                    if( (pos >= 3) && (0 == *(pbuf+pos-3) ))
                    {
                        start_code = 3;
                    }
                    else
                    {
                        start_code = 2;
                    }
                    
                    
                    // ignore for the first time
                    bIsGotFirstNaluOfTheStream = false;
                }
                else
                {
                    
                    unsigned int uiBufSize = pos - start_code ;
                    H264 oh264;
                    oh264.FindSliceStart(pbuf, uiBufSize,start_code);
                    int nal_type = oh264.GetNaluType(pbuf,uiBufSize);
                    
                    
                    if( 7  == nal_type )
                    {
                        oh264.ProcessSPS(pbuf,uiBufSize);
                        cout << "SPS processed"<<endl;
                    }
                    
                    if( 8 == nal_type)
                    {
                        oh264.ProcessPPS(pbuf,uiBufSize);
                        cout<< "PPS processed"<<endl;
                    }
                    if( 5 == nal_type|| 1 == nal_type||20 == nal_type||21 == nal_type)
                    {
                        oh264.GetSliceType(pbuf,uiBufSize,nal_type,oh264.nal_ref_idc );
                        cout<< "IDR/NON_IDR"<<endl;
                    }

                    
                    // Reset the buffer here again
                    memset(buf,0,10000);
                    *(pbuf+0)=0;
                    *(pbuf+1)=0;
                    if ( 4 == start_code )
                    {
                        *(pbuf+2)=0;
                        *(pbuf+3)=1;
                    }
                    else if ( 3 == start_code )
                    {
                        *(pbuf+2)=1;
                    }
                    
                    pos = start_code - 1 ;
                    
                }
                
                
                
                
            }
            
            
        }
        pos++;
    }
    
    fclose(Fp);
}
