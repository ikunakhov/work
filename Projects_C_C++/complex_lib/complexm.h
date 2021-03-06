#ifndef complexm_H_
#define complexm_H_


#define PI  3.14159265358979323846

typedef struct complexm
{
    double re;
    double im;
    double r;
    double fi;
}complexm;


void fill_rfi(complexm* in);


void fill_reim(complexm* in);


void init_reim(complexm* c, double re, double im);


void init_rfi(complexm* c, double r, double fi);


void ccopy(complexm* to_c1, complexm* from_c2);


complexm cdiv(complexm c1, complexm c2);


complexm cmul(complexm c1, complexm c2);


complexm csum(complexm c1, complexm c2);


complexm csub(complexm c1, complexm c2);


complexm cmonj(complexm c);


double cmabs(complexm* in);


complexm cmsqrt(complexm in, int deg, int num);


void cswap(complexm* c1,complexm* c2);




#endif /* complexm_H_ */
