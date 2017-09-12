//  H.cpp
//  H.264 Parser

//


#include <iostream>
#include "VideoParser.h"
#include "Bitstream.h"
#include "H264.h"

using namespace std;

int chroma_format_idc=0;
int frame_mbs_only_flag=0;
int pic_order_cnt_type=0;
int separate_colour_plane_flag=0;
int bottom_field_pic_order_in_frame_present_flag=0;
int delta_pic_order_always_zero_flag=0;
int redundant_pic_cnt_present_flag=0;
int weighted_pred_flag=0;
int weighted_bipred_idc=0;
int entropy_coding_mode_flag=0;
int deblocking_filter_control_present_flag=0;
int slice_group_map_type=0;
int num_slice_groups_minus1=0;
int slice_type=0;
void ref_pic_list_modification(Bitstream oBitstream);
void ref_pic_list_mvc_modification(Bitstream oBitstream);
void hrd_parameters(Bitstream oBitstream);
void vui_parameters(Bitstream oBitstream );

//  H.cpp
//  H.264 Parser


void H264::ProcessSPS(unsigned char *pucBuf,unsigned int size)
{
    unsigned int startoffset=0;
    VideoParser ovp;
    ovp.FindSliceStart(pucBuf,size,startoffset);
    Bitstream oBitstream;
    oBitstream.setBuff(pucBuf+startoffset, size);
    oBitstream.setEmulPrevByte();
    // Parse the Nalu header
    //Forbidden zerobit
    oBitstream.Readbits(1);
    //Nal_ref_idc
    oBitstream.Readbits(2);
    //Nal_unit_type
    oBitstream.Readbits(5);
    cout << "======= SPS ======="<<endl;
    int profile_idc= oBitstream.Readbits(8);
    cout<< "profile_idc : "<<profile_idc<<endl; // Nalu header will be 8 bits only
    // Start parsing sps data from here
    cout<< "constraint_set0_flag : "<<oBitstream.Readbits(1)<<endl;
    cout<< "constraint_set1_flag : "<<oBitstream.Readbits(1)<<endl;
    cout<< "constraint_set2_flag : "<<oBitstream.Readbits(1)<<endl;
    cout<< "constraint_set3_flag : "<<oBitstream.Readbits(1)<<endl;
    cout<< "constraint_set4_flag : "<<oBitstream.Readbits(1)<<endl;
    cout<< "constraint_set5_flag : "<<oBitstream.Readbits(1)<<endl;
    cout<< "reserved_zero_2bits : "<<oBitstream.Readbits(2)<<endl;
    cout<< "level_idc : "<<oBitstream.Readbits(8)<<endl;
    cout<< "seq_parameter_set_id : "<<oBitstream.uGolombcode()<<endl;
    if( profile_idc == 100 || profile_idc == 110 ||
       profile_idc == 122 || profile_idc == 244 || profile_idc == 44 || profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128 || profile_idc == 138 || profile_idc == 139 || profile_idc == 134 || profile_idc == 135 )
    {
        chroma_format_idc = oBitstream.uGolombcode();
        cout<< "chroma_format_idc : "<<chroma_format_idc<<endl;
        if(chroma_format_idc == 3)
            cout<< "separate_colour_transform_flag : "<<oBitstream.Readbits(1)<<endl;
        cout<< "bit_depth_luma_minus8 : "<<oBitstream.uGolombcode()<<endl;
        cout<< "bit_depth_chroma_minus8 : "<<oBitstream.uGolombcode()<<endl;
        cout<< "qpprime_y_zero_transform_bypass_flag : "<< oBitstream.Readbits(1)<<endl;
        int seq_scaling_matrix_present_flag = oBitstream.Readbits(1);
        cout<< "seq_scaling_matrix_present_flag : "<<seq_scaling_matrix_present_flag<<endl;
        
        if(1 == seq_scaling_matrix_present_flag)
        {
            /**
             * Loop for seq_scaling_list
             */
            for ( unsigned char ucIndex = 0; ucIndex < ( ( chroma_format_idc != 3 ) ? 8 : 12 ); ucIndex++ )
            {
                unsigned char ucScalingListIndex = 0;
                
                /**
                 * Read the seq_scaling_list_present_flag [u(1)] and ignore
                 */
                oBitstream.Readbits(1);
                
                /**
                 * If the index is less than '6'
                 */
                if ( ucIndex < 6 )
                {
                    /**
                     * Loop for seq_scaling_list
                     */
                    for ( ucScalingListIndex = 0; ucScalingListIndex < 16;
                         ucScalingListIndex++ )
                    {
                        unsigned char ucDeltaScale = 0;
                        
                        /**
                         * Read the delta_scale [se(v)] and ignore
                         */
                        ucDeltaScale = oBitstream.uGolombcode();
                    }
                }
                else
                {
                    /**
                     * Loop for seq_scaling_list
                     */
                    for ( ucScalingListIndex = 0; ucScalingListIndex < 64;
                         ucScalingListIndex++ )
                    {
                        unsigned char ucDeltaScale = 0;
                        
                        /**
                         * Read the delta_scale [se(v)] and ignore
                         */
                        ucDeltaScale = oBitstream.uGolombcode();
                    }
                }
            }
        }
    }
    
    cout<< "log2_max_frame_num_minus4 : "<<oBitstream.uGolombcode()<<endl;
    int pic_order_cnt_type =oBitstream.uGolombcode();
    cout<< "pic_order_cnt_type : "<<pic_order_cnt_type<<endl;
    
    if(pic_order_cnt_type==0)
        cout<< "log2_max_pic_order_cnt_lsb_minus4 : "<<oBitstream.uGolombcode()<<endl;
    
    else if(pic_order_cnt_type==1)
    {
        cout<< "delta_pic_order_always_zero_flag : "<<oBitstream.Readbits(1)<<endl;
        cout<< "offset_for_non_ref_pic : "<<oBitstream.sGolombcode()<<endl;
        cout<< "offset_for_top_to_bottom_field : "<<oBitstream.sGolombcode()<<endl;
        int num_ref_frames_in_pic_order_cnt_cycle = oBitstream.uGolombcode();
        cout<< "num_ref_frames_in_pic_order_cnt_cycle : "<<num_ref_frames_in_pic_order_cnt_cycle <<endl;
        int offset_for_ref_frame[num_ref_frames_in_pic_order_cnt_cycle];
        for(int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ ){
            offset_for_ref_frame[i]=oBitstream.sGolombcode();
            cout<<" offset_for_ref_frame[i] : "<<offset_for_ref_frame[i]<<endl;
        }
    }
    cout<< "max_num_ref_frames : "<<oBitstream.uGolombcode()<<endl;
    cout<< "gaps_in_frame_num_value_allowed_flag : "<< oBitstream.Readbits(1)<<endl;
    int pic_width_in_mbs_minus1=oBitstream.uGolombcode();
    cout<< "pic_width_in_mbs_minus1 : "<< pic_width_in_mbs_minus1<<endl;
    int pic_height_in_map_units_minus1 = oBitstream.uGolombcode();
    cout<< "pic_height_in_map_units_minus1 : "<< pic_height_in_map_units_minus1<<endl;
    int frame_mbs_only_flag =oBitstream.Readbits(1);
    cout<< "frame_mbs_only_flag : "<<frame_mbs_only_flag<<endl;
    
    if( !frame_mbs_only_flag )
        cout<< "mb_adaptive_frame_field_flag : "<<oBitstream.Readbits(1)<<endl;
    
    cout<< "direct_8x8_inference_flag : "<<oBitstream.Readbits(1)<<endl;
    
    int frame_cropping_flag= oBitstream.Readbits(1);
    cout<< "frame_cropping_flag : "<< frame_cropping_flag<<endl;
    
    int frame_crop_left_offset=0;
    int frame_crop_right_offset=0;
    int frame_crop_top_offset=0;
    int frame_crop_bottom_offset=0;
    
    if(frame_cropping_flag)
    {
        frame_crop_left_offset = oBitstream.uGolombcode();
        cout<< "frame_crop_left_offset : "<<frame_crop_left_offset<<endl;
        frame_crop_right_offset= oBitstream.uGolombcode();
        cout<< "frame_crop_right_offset : "<<frame_crop_right_offset<<endl;
        frame_crop_top_offset=oBitstream.uGolombcode();
        cout<< "frame_crop_top_offset : "<<frame_crop_top_offset<<endl;
        frame_crop_bottom_offset=oBitstream.uGolombcode();
        cout<< "frame_crop_bottom_offset : "<<frame_crop_bottom_offset <<endl;
    }
    
    int vui_parameters_present_flag=oBitstream.Readbits(1);
    cout<< "vui_parameters_present_flag : "<<  vui_parameters_present_flag<<endl;
    
    if( vui_parameters_present_flag )
    vui_parameters(oBitstream);
    int Width = (((pic_width_in_mbs_minus1 +1)*16) - (frame_crop_bottom_offset*2) - frame_crop_top_offset*2);
    int Height = (((2 - frame_mbs_only_flag)*(pic_height_in_map_units_minus1 +1) *16) - (frame_crop_right_offset *2) - (frame_crop_left_offset *2));
    
    cout<<" WxH = "<<Width<<" x "<<Height<<endl;
}










void H264::ProcessPPS(unsigned char *pucBuf,unsigned int size)
{
    unsigned int startoffset=0;
    FindSliceStart(pucBuf,size,startoffset);
    Bitstream oBitstream;
    oBitstream.setBuff(pucBuf+startoffset, size);
    // Parse the Nalu header
    //Forbidden zerobit
    oBitstream.Readbits(1);
    //Nal_ref_idc
    oBitstream.Readbits(2);
    //Nal_unit_type
    oBitstream.Readbits(5);
    
    
    cout << "======= PPS ======="<<endl;
    
    cout<<" picture parameter set ID : "<<oBitstream.uGolombcode()<<endl;
    cout<<" sequence parameter set ID : "<<oBitstream.uGolombcode()<<endl;
    entropy_coding_mode_flag=oBitstream.Readbits(1);
    cout<< "entropy coding mode flag : "<<entropy_coding_mode_flag<<endl;
    bottom_field_pic_order_in_frame_present_flag=oBitstream.Readbits(1);
    cout<< "bottom_field_pic_order_in_frame_present_flag : "<<bottom_field_pic_order_in_frame_present_flag<<endl;
    
    num_slice_groups_minus1=oBitstream.uGolombcode();
    cout<<"num slice groups minus 1 : "<<num_slice_groups_minus1<<endl;
    int run_length_minus1[num_slice_groups_minus1];
    int top_left[num_slice_groups_minus1];
    int bottom_right[num_slice_groups_minus1];
    
    if( num_slice_groups_minus1 > 0 )
    {
        slice_group_map_type=oBitstream.uGolombcode();
        cout<<"slice_group_map_type : "<<slice_group_map_type<<endl;
        
        if( slice_group_map_type == 0 )
        {
            for( int iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++ )
                run_length_minus1[ iGroup ] = oBitstream.uGolombcode();
            cout<<"slice_group_map_type : "<<slice_group_map_type<<endl;
        }
        
        else if(slice_group_map_type == 2)
        {
            for( int iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++ )
            {
                top_left[ iGroup ]=oBitstream.uGolombcode();
                bottom_right[ iGroup ]=oBitstream.uGolombcode();
            }
        }
        else if( slice_group_map_type == 3 || slice_group_map_type == 4 || slice_group_map_type == 5 )
        {
            cout<<"slice_group_change_direction_flag : "<<oBitstream.Readbits(1)<<endl;
            cout<<"slice_group_change_rate_minus1 : "<<oBitstream.uGolombcode()<<endl;
        }
        else if( slice_group_map_type == 6 )
        {
            int pic_size_in_map_units_minus1=oBitstream.uGolombcode();
            int slice_group_id[pic_size_in_map_units_minus1];
            for( int i = 0; i <= pic_size_in_map_units_minus1; i++ )
                slice_group_id[ i ]= oBitstream.Readbits(1);
        }
    }
    
    cout<<" num_ref_idx_l0_default_active_minus1 : "<<oBitstream.uGolombcode()<<endl;
    cout<<" num_ref_idx_l1_default_active_minus1: "<<oBitstream.uGolombcode()<<endl;
    
    weighted_pred_flag=oBitstream.Readbits(1);
    cout<<"weighted_pred_flag : "<<weighted_pred_flag<<endl;
    weighted_bipred_idc=oBitstream.Readbits(2);
    cout<<"weighted_bipred_flag : "<<weighted_bipred_idc<<endl;
    cout<<"pic_init_qp_minus26 : "<<oBitstream.sGolombcode()<<endl;
    // relative to 26 /
    cout<<"pic_init_qs_minus26 : "<<oBitstream.sGolombcode()<<endl;
    // relative to 26 /
    cout<<"chroma_qp_index_offset: "<<oBitstream.uGolombcode()<<endl;
    deblocking_filter_control_present_flag=oBitstream.Readbits(1);
    cout<<"deblocking_filter_control_present_flag : "<<deblocking_filter_control_present_flag<<endl;
    cout<<"constrained_intra_pred_flag: "<<oBitstream.Readbits(1)<<endl;
    redundant_pic_cnt_present_flag= oBitstream.Readbits(1);
    cout<<"redundant_pic_cnt_present_flag: "<<redundant_pic_cnt_present_flag<<endl;
    
    /*if( more_rbsp_data() )
    {
        int  transform_8x8_mode_flag=oBitstream.uGolombcode();
        cout<<"  transform_8x8_mode_flag : "<<transform_8x8_mode_flag<<endl;
        int pic_scaling_matrix_present_flag= oBitstream.uGolombcode();
        cout<<" pic_scaling_matrix_present_flag: "<<pic_scaling_matrix_present_flag<<endl;
        cout<<"  transform_8x8_mode_flag : "<<oBitstream.uGolombcode()<<endl;
        int pic_scaling_list_present_flag[6+((chroma_format_idc != 3?2:6)*transform_8x8_mode_flag)];
        
        if( pic_scaling_matrix_present_flag )
            
            for( int i = 0; i<6+(( chroma_format_idc != 3 ) ? 2 : 6 )*transform_8x8_mode_flag;i++ )
                
            {
                
                pic_scaling_list_present_flag[ i ] = oBitstream.Readbits(1);
                if( pic_scaling_list_present_flag[ i ] )
                    if( i < 6 )
                    {
                        scaling_list( ScalingList4x4[ i ], 16, UseDefaultScalingMatrix4x4Flag[ i ]);
                    }
                
                    else
                    {
                        scaling_list(ScalingList8x8[i-6],64,UseDefaultScalingMatrix8x8Flag[i-6]);
                    }
            
                
                cout<<"second_chroma_qp_index_offset: "<<oBitstream.sGolombcode()<<endl;
            }
        //rbsp_trailing_bits( )
    }*/
    
}

void H264::GetSliceType(unsigned char *pucBuf,unsigned int size, int nal_unit_type, int nal_ref_idc)
{
    unsigned int startoffset=0;
    FindSliceStart(pucBuf,size,startoffset);
    Bitstream oBitstream;
    oBitstream.setBuff(pucBuf+startoffset, size);
    cout<<" first_mb_in_slice : "<<oBitstream.uGolombcode()<<endl;
    
    slice_type=oBitstream.uGolombcode();
    cout<<" slice_type : "<<slice_type<<endl;
    cout<<" pic_parameter_set_id : "<<oBitstream.uGolombcode()<<endl;
    if( separate_colour_plane_flag == 1 )
        cout<<" colour_plane_id : "<<oBitstream.Readbits(2)<<endl;
    cout<<" frame_num : "<<oBitstream.Readbits(1)<<endl;
    int field_pic_flag=0;
    if( !frame_mbs_only_flag ) {
        field_pic_flag=oBitstream.Readbits(1);
        cout<<" field_pic_flag : "<<field_pic_flag<<endl;
        if( field_pic_flag )
            cout<<" bottom_field_flag : "<<oBitstream.Readbits(1)<<endl;
    }
    if( nal_unit_type == 5 )
        cout<<" idr_pic_id : "<<oBitstream.uGolombcode()<<endl;
    if( pic_order_cnt_type == 0 ) {
        cout<<" pic_order_cnt_lsb: "<<oBitstream.Readbits(1)<<endl;
        if( bottom_field_pic_order_in_frame_present_flag && !field_pic_flag )
            cout<< "delta_pic_order_cnt_bottom : "<<oBitstream.sGolombcode()<<endl;
    }
    int delta_pic_order_cnt[2];
    if( pic_order_cnt_type == 1 && !delta_pic_order_always_zero_flag ) {
        delta_pic_order_cnt[0]=oBitstream.sGolombcode();
        cout<<" delta_pic_order_cnt[0] : "<<delta_pic_order_cnt[0]<<endl;
        if( bottom_field_pic_order_in_frame_present_flag && !field_pic_flag ){
            delta_pic_order_cnt[1]=oBitstream.sGolombcode();
            cout<<" delta_pic_order_cnt[1] : "<<delta_pic_order_cnt[1]<<endl;
        }
    }
    int num_ref_idx_active_override_flag;
    if( redundant_pic_cnt_present_flag )
        cout<<" redundant_pic_cnt : "<<oBitstream.uGolombcode()<<endl;
    if( slice_type ==0 ||slice_type == 5)//P slice
        cout<<" direct_spatial_mv_pred_flag : "<<oBitstream.Readbits(1)<<endl;
    if(slice_type == 0 || slice_type ==5 || slice_type == 1||slice_type ==6||slice_type ==3||slice_type ==8)
    {
        num_ref_idx_active_override_flag=oBitstream.Readbits(1);
        cout<<" num_ref_idx_active_override_flag : "<<num_ref_idx_active_override_flag<<endl;
        if( num_ref_idx_active_override_flag )
        {
            cout<<" num_ref_idx_l0_active_minus1 : "<<oBitstream.uGolombcode()<<endl;
            if( slice_type == 1|| slice_type == 6)
                cout<<" num_ref_idx_l1_active_minus1 : "<<oBitstream.uGolombcode()<<endl;
        }
    }
    if( nal_unit_type == 20 || nal_unit_type == 21 )
        ref_pic_list_mvc_modification(oBitstream); /* specified in Annex H */
   
    else
       ref_pic_list_modification(oBitstream);
    //if( ( weighted_pred_flag && ( slice_type == 0||slice_type == 5||slice_type == 3||slice_type == 8))||(weighted_bipred_idc == 1 && (slice_type == 1||slice_type == 0)))
        //pred_weight_table( );
    //if( nal_ref_idc != 0 )
        //dec_ref_pic_marking( );
    if( entropy_coding_mode_flag && (slice_type != 2 && slice_type != 7))
        cout<<" cabac_init_idc : "<<oBitstream.uGolombcode()<<endl;
    
    cout<<" slice_qp_delta : "<<oBitstream.sGolombcode()<<endl;
    if(slice_type==3||slice_type==8||slice_type==4||slice_type==9){
        if( slice_type == 3||slice_type==8)
            cout<<" sp_for_switch_flag slice_qs_delta : "<<oBitstream.Readbits(1)<<endl;
        cout<<" slice_qs_delta : "<<oBitstream.sGolombcode()<<endl;
    }
    int disable_deblocking_filter_idc;
    if( deblocking_filter_control_present_flag ) {
        disable_deblocking_filter_idc=oBitstream.uGolombcode();
        cout<<" disable_deblocking_filter_idc : "<<disable_deblocking_filter_idc<<endl;
        if( disable_deblocking_filter_idc != 1 ) {
            cout<<" slice_alpha_c0_offset_div2 : "<<oBitstream.sGolombcode();
            cout<<" slice_beta_offset_div2 : "<<oBitstream.sGolombcode();
        }
    }
    if( num_slice_groups_minus1 > 0 && slice_group_map_type >= 3 && slice_group_map_type <= 5)
        cout<<" slice_group_change_cycle : "<<oBitstream.Readbits(1)<<endl;
}

void ref_pic_list_modification(Bitstream oBitstream)
{
    int ref_pic_list_modification_flag_l0;
    int modification_of_pic_nums_idc;
    int ref_pic_list_modification_flag_l1;
    if( slice_type % 5 != 2 && slice_type % 5 != 4 ) {
        ref_pic_list_modification_flag_l0=oBitstream.Readbits(1);
        cout<<" ref_pic_list_modification_flag_l0 : "<<endl;
        if( ref_pic_list_modification_flag_l0)
            do {
                modification_of_pic_nums_idc=oBitstream.uGolombcode();
                cout<<" modification_of_pic_nums_idc : "<<endl;
                if( modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1 )
                    cout<<" abs_diff_pic_num_minus1 : "<<oBitstream.uGolombcode()<<endl;
                
                else if( modification_of_pic_nums_idc == 2 )
                    cout<<" long_term_pic_num : "<<oBitstream.uGolombcode()<<endl;
            } while( modification_of_pic_nums_idc != 3 );
    }
    if( slice_type % 5 == 1 ) {
        ref_pic_list_modification_flag_l1=oBitstream.Readbits(1);
        cout<<" ref_pic_list_modification_flag_l1 : "<<ref_pic_list_modification_flag_l1<<endl;
        if( ref_pic_list_modification_flag_l1 )
            do {
                modification_of_pic_nums_idc=oBitstream.uGolombcode();
                cout<<" modification_of_pic_nums_idc : "<<modification_of_pic_nums_idc<<endl;
                if( modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1 )
                    cout<<" abs_diff_pic_num_minus1 : "<<oBitstream.uGolombcode()<<endl;
                else if( modification_of_pic_nums_idc == 2 )
                    cout<<" long_term_pic_num : "<<oBitstream.uGolombcode()<<endl;
            } while(modification_of_pic_nums_idc != 3 );
    }
    
}
void ref_pic_list_mvc_modification(Bitstream oBitstream)
{
    int modification_of_pic_nums_idc;
    
    if( slice_type % 5 != 2 && slice_type % 5 != 4 )
    {
        int ref_pic_list_modification_flag_l0 = oBitstream.Readbits(1);
        cout<<"ref_pic_list_modification_flag_l0 : "<<ref_pic_list_modification_flag_l0<<endl;
        
        if( ref_pic_list_modification_flag_l0 )
        {
            do
            {
                modification_of_pic_nums_idc =oBitstream.uGolombcode();
                cout<<"modification_of_pic_nums_idc : "<<modification_of_pic_nums_idc<<endl;
                
                if( modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1 )
                {
                    int abs_diff_pic_num_minus1=oBitstream.uGolombcode();
                cout<<"abs_diff_pic_num_minus1 : "<<abs_diff_pic_num_minus1<<endl;
                }
                
                else if( modification_of_pic_nums_idc == 2 ){
                    int long_term_pic_num=oBitstream.uGolombcode();
                cout<<"long_term_pic_num : "<<long_term_pic_num<<endl;
                }
                
                else if( modification_of_pic_nums_idc == 4 || modification_of_pic_nums_idc == 5 )
                {
                    int abs_diff_view_idx_minus1=oBitstream.uGolombcode();
                cout<<"abs_diff_view_idx_minus1 : "<<abs_diff_view_idx_minus1<<endl;
                }
                
            }while( modification_of_pic_nums_idc != 3 );
        }
    }
    
    if( slice_type % 5 == 1 )
    {
        int ref_pic_list_modification_flag_l1=oBitstream.Readbits(1);
        cout<<"ref_pic_list_modification_flag_l1 : "<<ref_pic_list_modification_flag_l1<<endl;
        
        if( ref_pic_list_modification_flag_l1 )
        {
            
            do
            {
                modification_of_pic_nums_idc=oBitstream.uGolombcode();
                cout<<"modification_of_pic_nums_idc : "<<modification_of_pic_nums_idc<<endl;
                
                if( modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1 )
                {
                    int abs_diff_pic_num_minus1=oBitstream.uGolombcode();
                cout<<"abs_diff_pic_num_minus1 : "<<abs_diff_pic_num_minus1<<endl;
                }
                
                else if( modification_of_pic_nums_idc == 2 ) {
                    int long_term_pic_num=oBitstream.uGolombcode();
                cout<<"long_term_pic_num : "<<long_term_pic_num<<endl;
                }
                else if( modification_of_pic_nums_idc == 4 || modification_of_pic_nums_idc == 5 ){
                    int abs_diff_view_idx_minus1=oBitstream.uGolombcode();
                cout<<"abs_diff_view_idx_minus1 : "<<abs_diff_view_idx_minus1<<endl;
                }
                
            }while( modification_of_pic_nums_idc != 3 );
        }
    }
    
}
void vui_parameters(Bitstream oBitstream )
{
  
    int aspect_ratio_info_present_flag= oBitstream.Readbits(1);
    int aspect_ratio_idc;
    cout<<" aspect_ratio_info_present_flag : "<<aspect_ratio_info_present_flag<<endl;
    if( aspect_ratio_info_present_flag ) {
        aspect_ratio_idc=oBitstream.Readbits(8);
        cout<<" aspect_ratio_idc: "<<aspect_ratio_idc<<endl;

        if( aspect_ratio_idc ==255) {
            
            cout<<" sar_width : "<<oBitstream.Readbits(16)<<endl;
            cout<<" sar_height : "<<oBitstream.Readbits(16)<<endl;
        }
    }
    int overscan_info_present_flag=oBitstream.Readbits(1);
    cout<<" overscan_info_present_flag : "<<overscan_info_present_flag<<endl;
    if( overscan_info_present_flag )
        cout<<" overscan_appropriate_flag : "<<oBitstream.Readbits(1)<<endl;
    int video_signal_type_present_flag=oBitstream.Readbits(1);
    cout<<" video_signal_type_present_flag : "<<video_signal_type_present_flag<<endl;
    int colour_description_present_flag;
        if( video_signal_type_present_flag ) {
            cout<<" video_format : "<<oBitstream.Readbits(3)<<endl;
            cout<<" video_full_range_flag : "<<oBitstream.Readbits(1)<<endl;
            colour_description_present_flag=oBitstream.Readbits(1);
            cout<<" colour_description_present_flag : "<<colour_description_present_flag<<endl;
            if( colour_description_present_flag ) {
                cout<<" colour_primaries : "<<oBitstream.Readbits(8)<<endl;
                cout<<" transfer_characteristics : "<<oBitstream.Readbits(8)<<endl;
                cout<<" matrix_coefficients : "<<oBitstream.Readbits(8)<<endl;
            }
            
        }
    
    int chroma_loc_info_present_flag=oBitstream.Readbits(1);
    cout<<" chroma_loc_info_present_flag : "<<chroma_loc_info_present_flag<<endl;
    if( chroma_loc_info_present_flag ) {
        cout<<" chroma_sample_loc_type_top_field : "<<oBitstream.uGolombcode()<<endl;
        cout<<" chroma_sample_loc_type_bottom_field : "<<oBitstream.uGolombcode()<<endl;
    }
    int timing_info_present_flag= oBitstream.Readbits(1);
    cout<<" timing_info_present_flag : "<<timing_info_present_flag<<endl;
    if( timing_info_present_flag ) {
        cout<<" num_units_in_tick : "<<oBitstream.Readbits(32)<<endl;
        cout<<" time_scale : "<<oBitstream.Readbits(32)<<endl;
        cout<<" fixed_frame_rate_flag : "<<oBitstream.Readbits(1)<<endl;
    }
    int nal_hrd_parameters_present_flag=oBitstream.Readbits(1);
    cout<<" nal_hrd_parameters_present_flag : "<<nal_hrd_parameters_present_flag<<endl;
    if( nal_hrd_parameters_present_flag )
        hrd_parameters( oBitstream);
    int vcl_hrd_parameters_present_flag = oBitstream.Readbits(1);
    cout<<" vcl_hrd_parameters_present_flag : "<<vcl_hrd_parameters_present_flag<<endl;
        
    if( vcl_hrd_parameters_present_flag )
        hrd_parameters( oBitstream);
          
    if( nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag )
        cout<<" low_delay_hrd_flag : "<<oBitstream.Readbits(1)<<endl;
    
    cout<<" pic_struct_present_flag : "<<oBitstream.Readbits(1)<<endl;
    int bitstream_restriction_flag=oBitstream.Readbits(1);
    cout<<" bitstream_restriction_flag : "<<bitstream_restriction_flag<<endl;
    
    if( bitstream_restriction_flag ){
        cout<<" motion_vectors_over_pic_boundaries_flag : "<<oBitstream.Readbits(1)<<endl;
        cout<<" max_bytes_per_pic_denom : "<<oBitstream.uGolombcode()<<endl;
        cout<<" max_bits_per_mb_denom : "<<oBitstream.uGolombcode()<<endl;
        cout<<" log2_max_mv_length_horizontal : "<<oBitstream.uGolombcode()<<endl;
        cout<<" log2_max_mv_length_vertical : "<<oBitstream.uGolombcode()<<endl;
        cout<<" max_num_reorder_frames : "<<oBitstream.uGolombcode()<<endl;
        cout<<" max_dec_frame_buffering : "<<oBitstream.uGolombcode()<<endl;
    }
    
}
void hrd_parameters(Bitstream oBitstream )

{
    int cpb_cnt_minus1=oBitstream.uGolombcode();
    cout<<" cpb_cnt_minus1 : "<<cpb_cnt_minus1<<endl;
    cout<<" bit_rate_scale : "<<oBitstream.Readbits(4)<<endl;
    cout<<" cpb_size_scale : "<<oBitstream.Readbits(4)<<endl;
    int bit_rate_value_minus1[cpb_cnt_minus1];
    int cpb_size_value_minus1[cpb_cnt_minus1];
    int cbr_flag[cpb_cnt_minus1];
    for( int SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++ ) {
        bit_rate_value_minus1[SchedSelIdx]=oBitstream.uGolombcode();
        cout<<" bit_rate_value_minus1[ SchedSelIdx] : "<<bit_rate_value_minus1[ SchedSelIdx]<<endl;
        cpb_size_value_minus1[ SchedSelIdx]=oBitstream.uGolombcode();
        cout<<" cpb_size_value_minus1[ SchedSelIdx] : "<<cpb_size_value_minus1[ SchedSelIdx]<<endl;
        cbr_flag[ SchedSelIdx ]=oBitstream.Readbits(1);
        cout<<" cbr_flag[ SchedSelIdx ]: "<<cbr_flag[ SchedSelIdx ]<<endl;
 
    }
    cout<<" initial_cpb_removal_delay_length_minus1 : "<<oBitstream.Readbits(5);
    cout<<" cpb_removal_delay_length_minus1 : "<<oBitstream.Readbits(5);
    cout<<" dpb_output_delay_length_minus1 : "<<oBitstream.Readbits(5);
    cout<<" time_offset_length : "<<oBitstream.Readbits(5);
    
}


  
/*Bitstream pred_weight_table(Bitstream oBitstream)
{
    luma_log2_weight_denom
    if( ChromaArrayType != 0 )
        chroma_log2_weight_denom
        for( i = 0; i <= num_ref_idx_l0_active_minus1; i++ ) {
            luma_weight_l0_flag
            if( luma_weight_l0_flag ) {
                luma_weight_l0[ i ]
                luma_offset_l0[ i ]
            }
            if( ChromaArrayType != 0 ) {
                chroma_weight_l0_flag
                if( chroma_weight_l0_flag )
                    for( j =0; j < 2; j++ ) {
                        chroma_weight_l0[ i ][ j ]
                        chroma_offset_l0[ i ][ j ]
                    } }
        }
    if( slice_type % 5 = = 1 )
        for( i = 0; i <= num_ref_idx_l1_active_minus1; i++ ) {
            luma_weight_l1_flag 2if( luma_weight_l1_flag ) {
                luma_weight_l1[ i ]
                luma_offset_l1[ i ]  }
            if( ChromaArrayType != 0 ) {
                chroma_weight_l1_flag;
                if( chroma_weight_l1_flag )
                    for( j = 0; j < 2; j++ ) {
                        chroma_weight_l1[ i ][ j ]
                        chroma_offset_l1[ i ][ j ]
                    }
            }
            
        } }*/



