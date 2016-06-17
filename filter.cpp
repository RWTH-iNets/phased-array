#include "filter.hpp"

const float filter::fir_hp_016_75[75] = {0.00017115763330366462, 0.0004942481173202395, 0.0007491861470043659, 0.0008721959893591702, 0.000789864978287369, 0.0004424733342602849, -0.00017812830628827214, -0.0009909045184031129, -0.0018085725605487823, -0.0023587881587445736, -0.002350034425035119, -0.0015716211637482047, -4.894707685113614e-18, 0.0021270893048495054, 0.004305804148316383, 0.005852583795785904, 0.006076951511204243, 0.004505050368607044, 0.0010928590781986713, -0.0036424207501113415, -0.008633624762296677, -0.012446527369320393, -0.013616789132356644, -0.011074105277657509, -0.004549249541014433, 0.005153219681233168, 0.01611516810953617, 0.02554774470627308, 0.030297869816422462, 0.027527470141649246, 0.015419143252074718, -0.006260732654482126, -0.03589152917265892, -0.07011181861162186, -0.10432210564613342, -0.13348975777626038, -0.15309277176856995, 0.8399995565414429, -0.15309277176856995, -0.13348975777626038, -0.10432210564613342, -0.07011181861162186, -0.03589152917265892, -0.006260732654482126, 0.015419143252074718, 0.027527470141649246, 0.030297869816422462, 0.02554774470627308, 0.01611516810953617, 0.005153219681233168, -0.004549249541014433, -0.011074105277657509, -0.013616789132356644, -0.012446527369320393, -0.008633624762296677, -0.0036424207501113415, 0.0010928590781986713, 0.004505050368607044, 0.006076951511204243, 0.005852583795785904, 0.004305804148316383, 0.0021270893048495054, -4.894707685113614e-18, -0.0015716211637482047, -0.002350034425035119, -0.0023587881587445736, -0.0018085725605487823, -0.0009909045184031129, -0.00017812830628827214, 0.0004424733342602849, 0.000789864978287369, 0.0008721959893591702, 0.0007491861470043659, 0.0004942481173202395, 0.00017115763330366462};

const float filter::fir_lp_014_75[75] = {-0.00036815545172430575, -9.033926471602172e-05, 0.00024301574740093201, 0.0006068026996217668, 0.0009515243582427502, 0.001197576173581183, 0.001243344391696155, 0.0009892372181639075, 0.00037390843499451876, -0.0005867775762453675, -0.0017711694817990065, -0.0029468433931469917, -0.003798273392021656, -0.003988364711403847, -0.00324519001878798, -0.0014559022383764386, 0.0012563627678900957, 0.004497470334172249, 0.007628210354596376, 0.009858393110334873, 0.010400643572211266, 0.008659277111291885, 0.004416918382048607, -0.0020241383463144302, -0.009785313159227371, -0.01746627688407898, -0.02331642620265484, -0.025504758581519127, -0.02244662679731846, -0.013129542581737041, 0.0026255976408720016, 0.024024400860071182, 0.04931733012199402, 0.0759858638048172, 0.10107406228780746, 0.12161455303430557, 0.13507743179798126, 0.13976436853408813, 0.13507743179798126, 0.12161455303430557, 0.10107406228780746, 0.0759858638048172, 0.04931733012199402, 0.024024400860071182, 0.0026255976408720016, -0.013129542581737041, -0.02244662679731846, -0.025504758581519127, -0.02331642620265484, -0.01746627688407898, -0.009785313159227371, -0.0020241383463144302, 0.004416918382048607, 0.008659277111291885, 0.010400643572211266, 0.009858393110334873, 0.007628210354596376, 0.004497470334172249, 0.0012563627678900957, -0.0014559022383764386, -0.00324519001878798, -0.003988364711403847, -0.003798273392021656, -0.0029468433931469917, -0.0017711694817990065, -0.0005867775762453675, 0.00037390843499451876, 0.0009892372181639075, 0.001243344391696155, 0.001197576173581183, 0.0009515243582427502, 0.0006068026996217668, 0.00024301574740093201, -9.033926471602172e-05, -0.00036815545172430575};

//100 kHz cuttoff at 1 MS/s (0.2 rel to nyq), 127 taps
const float filter::fir_lp_02nyq_127[127] = {
-0.000044, 
-0.000120, 
-0.000161, 
-0.000120, 
0.000016, 
0.000202, 
0.000346, 
0.000345, 
0.000147, 
-0.000201, 
-0.000548, 
-0.000700, 
-0.000514, 
-0.000000, 
0.000651, 
0.001123, 
0.001117, 
0.000522, 
-0.000485, 
-0.001465, 
-0.001897, 
-0.001439, 
-0.000145, 
0.001484, 
0.002672, 
0.002723, 
0.001395, 
-0.000886, 
-0.003131, 
-0.004188, 
-0.003313, 
-0.000616, 
0.002849, 
0.005462, 
0.005763, 
0.003221, 
-0.001351, 
-0.005988, 
-0.008376, 
-0.006955, 
-0.001823, 
0.005054, 
0.010523, 
0.011621, 
0.007075, 
-0.001791, 
-0.011291, 
-0.016789, 
-0.014819, 
-0.004975, 
0.009338, 
0.021854, 
0.025908, 
0.017534, 
-0.002107, 
-0.026131, 
-0.043705, 
-0.043762, 
-0.019357, 
0.028993, 
0.092440, 
0.155933, 
0.202767, 
0.220000, 
0.202767, 
0.155933, 
0.092440, 
0.028993, 
-0.019357, 
-0.043762, 
-0.043705, 
-0.026131, 
-0.002107, 
0.017534, 
0.025908, 
0.021854, 
0.009338, 
-0.004975, 
-0.014819, 
-0.016789, 
-0.011291, 
-0.001791, 
0.007075, 
0.011621, 
0.010523, 
0.005054, 
-0.001823, 
-0.006955, 
-0.008376, 
-0.005988, 
-0.001351, 
0.003221, 
0.005763, 
0.005462, 
0.002849, 
-0.000616, 
-0.003313, 
-0.004188, 
-0.003131, 
-0.000886, 
0.001395, 
0.002723, 
0.002672, 
0.001484, 
-0.000145, 
-0.001439, 
-0.001897, 
-0.001465, 
-0.000485, 
0.000522, 
0.001117, 
0.001123, 
0.000651, 
-0.000000, 
-0.000514, 
-0.000700, 
-0.000548, 
-0.000201, 
0.000147, 
0.000345, 
0.000346, 
0.000202, 
0.000016, 
-0.000120, 
-0.000161, 
-0.000120, 
-0.000044
};

const float filter::fir_hp_04nyq_127[127] = 
{
-0.000044, 
-0.000120, 
-0.000161, 
-0.000120, 
0.000016, 
0.000202, 
0.000346, 
0.000345, 
0.000147, 
-0.000201, 
-0.000548, 
-0.000700, 
-0.000514, 
-0.000000, 
0.000651, 
0.001123, 
0.001117, 
0.000522, 
-0.000485, 
-0.001465, 
-0.001897, 
-0.001439, 
-0.000145, 
0.001484, 
0.002672, 
0.002723, 
0.001395, 
-0.000886, 
-0.003131, 
-0.004188, 
-0.003313, 
-0.000616, 
0.002849, 
0.005462, 
0.005763, 
0.003221, 
-0.001351, 
-0.005988, 
-0.008376, 
-0.006955, 
-0.001823, 
0.005054, 
0.010523, 
0.011621, 
0.007075, 
-0.001791, 
-0.011291, 
-0.016789, 
-0.014819, 
-0.004975, 
0.009338, 
0.021854, 
0.025908, 
0.017534, 
-0.002107, 
-0.026131, 
-0.043705, 
-0.043762, 
-0.019357, 
0.028993, 
0.092440, 
0.155933, 
0.202767, 
0.220000, 
0.202767, 
0.155933, 
0.092440, 
0.028993, 
-0.019357, 
-0.043762, 
-0.043705, 
-0.026131, 
-0.002107, 
0.017534, 
0.025908, 
0.021854, 
0.009338, 
-0.004975, 
-0.014819, 
-0.016789, 
-0.011291, 
-0.001791, 
0.007075, 
0.011621, 
0.010523, 
0.005054, 
-0.001823, 
-0.006955, 
-0.008376, 
-0.005988, 
-0.001351, 
0.003221, 
0.005763, 
0.005462, 
0.002849, 
-0.000616, 
-0.003313, 
-0.004188, 
-0.003131, 
-0.000886, 
0.001395, 
0.002723, 
0.002672, 
0.001484, 
-0.000145, 
-0.001439, 
-0.001897, 
-0.001465, 
-0.000485, 
0.000522, 
0.001117, 
0.001123, 
0.000651, 
-0.000000, 
-0.000514, 
-0.000700, 
-0.000548, 
-0.000201, 
0.000147, 
0.000345, 
0.000346, 
0.000202, 
0.000016, 
-0.000120, 
-0.000161, 
-0.000120, 
-0.000044
};