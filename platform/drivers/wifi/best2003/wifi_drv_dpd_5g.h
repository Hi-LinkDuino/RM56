#ifndef __DPD_5G_H__
#define __DPD_5G_H__
#ifdef __cplusplus
extern "C" {
#endif

#define PI_5G  3.1415926
#define TEST_DPD_LEN_5G  100
/*#if DPD_TEST_5G
#define DPD_LEN  90
#else
#define DPD_LEN  30
#endif*/
#define DPD_LEN_5G  2048
#define LUT_LEN_5G 128

#define MAT_LEN_5G 2048


typedef struct
{
	float R1[MAT_LEN_5G];
	float R1_t[MAT_LEN_5G];
	float mat_tmp[9];
	float mat_tmp2[9];
	// double mat_tmp3[MAT_LEN] = { 0 };
	float mat_tmp4[MAT_LEN_5G];
	float AM_AM[MAT_LEN_5G/2];
	float AM_PM[MAT_LEN_5G/2];
	float PA[48];

}MAT_STRUCT_5G;


#if 1
typedef struct DPDprocessState_5G_
{
	int dump_len; // dump iq data length
	int len;  // number of points in PA AM-AM and AM-PM curve
	float Amin[DPD_LEN_5G];	// AM-AM curve input for PA, output for DPD
	// double DataI[DPD_LEN+64];
	// double DataQ[DPD_LEN+64];
	unsigned int* DumpData_IQ;
	// int dumpDataLen;
	
	float Amout[DPD_LEN_5G];	// AM-AM curve output for PA, input for DPD
	float Pmout[DPD_LEN_5G];	// AM-PM curve input for PA, output for DPD
	// double gain[DPD_LEN];
	// double gain_dB[DPD_LEN];
	// double gain_dB_diff[DPD_LEN-1];
    
	double gin_mini;  // variable g in Matlab code, for normallization, set to 1, seems meaningless
	double ph_delta;
	double gain_delta;
    double gain_dig;

	short amp[LUT_LEN_5G];	// lut0 0:129:16383
	short lut_amp[LUT_LEN_5G];	// output lut1
	short lut_ph[LUT_LEN_5G];	// output lut2
} DPDprocessState_5G;

#else
typedef struct DPDprocessState_5G_
{
	int len;
	double *Amin;
	double *DataI;
	double *DataQ;
	double *Amout;
	double *Pmout;

	double *gain;
	double gin_mini;
	double ph_delta;
	double gain_delta;
    double gain_dig;

	short *amp;
	short *lut_amp;
	short *lut_ph;
} DPDprocessState_5G;

#endif
struct DPDprocessState_5G_;
typedef struct DPDprocessState_5G_ DPDprocessState_5G;
DPDprocessState_5G *DPDprocessState_init_5G(int len);
DPDprocessState_5G *DPDprocessState_reset_5G(DPDprocessState_5G *st);

// int DPD_getDATA(DPDprocessState *st, short *buf);
void DPD_process_5G(DPDprocessState_5G *st);
int DPD_destroy_5G(DPDprocessState_5G *st);
//double dpd_atan_sum(int16_t (*dpd_buf)[2], int size) ;
//double dpd_atan_sum_ext(int16_t (*dpd_buf)[2], int size) ;

float calc_tone_power_5G(unsigned int* psrc, int cycles, int is_large_amp);
float new_triangle_dpd_5G(unsigned char freq_index, unsigned char band, float triangle_step, int triangle_gain, DPDprocessState_5G *st, int if_disconnect_fb);


#ifdef __cplusplus
}
#endif

#endif

