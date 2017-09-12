//
//  H.hpp
//  H.264 Parser
//
//  Created by Ashmita Chatterjee on 6/15/17.
//  Copyright © 2017 Ashmita Chatterjee. All rights reserved.
//

#ifndef H264_h
#define H264_h

class H264:public VideoParser {
public:
    void ProcessSPS(unsigned char *pucBuf,unsigned int size);
    void ProcessPPS(unsigned char *pucBuf,unsigned int size);
    void GetSliceType(unsigned char *pucBuf,unsigned int size, int nal_unit_type, int nal_ref_idc);
};
#endif /* H_hpp */
