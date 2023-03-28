#ifndef __DPD_H__
#define __DPD_H__
#ifdef __cplusplus
extern "C" {
#endif

#define PI  3.1415926
#define TEST_DPD_LEN  100
/*#if DPD_TEST_5G
#define DPD_LEN  90
#else
#define DPD_LEN  30
#endif*/
#define DPD_LEN  2048
#define LUT_LEN 128

#define MAT_LEN 2048


typedef struct
{
	float R1[MAT_LEN];
	float R1_t[MAT_LEN];
	float mat_tmp[9];
	float mat_tmp2[9];
	// double mat_tmp3[MAT_LEN] = { 0 };
	float mat_tmp4[MAT_LEN];
	float AM_AM[MAT_LEN/2];
	float AM_PM[MAT_LEN/2];
	float PA[48];

}MAT_STRUCT;


#if 1
typedef struct DPDprocessState_
{
	int dump_len; // dump iq data length
	int len;  // number of points in PA AM-AM and AM-PM curve
	float Amin[DPD_LEN];	// AM-AM curve input for PA, output for DPD
	// double DataI[DPD_LEN+64];
	// double DataQ[DPD_LEN+64];
	unsigned int* DumpData_IQ;
	// int dumpDataLen;
	
	float Amout[DPD_LEN];	// AM-AM curve output for PA, input for DPD
	float Pmout[DPD_LEN];	// AM-PM curve input for PA, output for DPD
	// double gain[DPD_LEN];
	// double gain_dB[DPD_LEN];
	// double gain_dB_diff[DPD_LEN-1];
    
	double gin_mini;  // variable g in Matlab code, for normallization, set to 1, seems meaningless
	double ph_delta;
	double gain_delta;
    double gain_dig;

	short amp[LUT_LEN];	// lut0 0:129:16383
	short lut_amp[LUT_LEN];	// output lut1
	short lut_ph[LUT_LEN];	// output lut2
} DPDprocessState;

#else
typedef struct DPDprocessState_
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
} DPDprocessState;

#endif
struct DPDprocessState_;
typedef struct DPDprocessState_ DPDprocessState;
DPDprocessState *DPDprocessState_init(int len);
DPDprocessState *DPDprocessState_reset(DPDprocessState *st);

// int DPD_getDATA(DPDprocessState *st, short *buf);
void DPD_process(DPDprocessState *st);
int DPD_destroy(DPDprocessState *st);
//double dpd_atan_sum(int16_t (*dpd_buf)[2], int size) ;
//double dpd_atan_sum_ext(int16_t (*dpd_buf)[2], int size) ;

float calc_tone_power(unsigned int* psrc, int cycles, int is_large_amp);
float new_triangle_dpd(unsigned char freq_index, float triangle_step, int triangle_gain, DPDprocessState *st);


#ifdef __cplusplus
}
#endif

#endif

