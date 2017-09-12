//
//  Parsing.hpp
//  H.264 Parser
//
//  Created by Ashmita Chatterjee on 6/8/17.
//  Copyright © 2017 Ashmita Chatterjee. All rights reserved.
//

#ifndef VideoParser_hpp
#define VideoParser_hpp


using namespace std;

class VideoParser{
public:
    int nal_ref_idc;
public:
    int GetNaluType(unsigned char *pucBuf,int size);
    
    unsigned char *FindSliceStart( unsigned char  *pucBuffer, unsigned int  uiFrameSize, unsigned int& ruiStartCodeLen );
    
};

#endif /* VideoParser_hpp */
