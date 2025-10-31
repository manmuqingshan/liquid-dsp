char __docstr__[] =
"Automatic gain control example demonstrating its transient"
" response on real-valued inputs";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char *,   filename, "agc_crcf_squelch_example.m",'o', "output filename",NULL);
    liquid_argparse_add(float,    bt,          0.01f,  'b', "agc loop bandwidth", NULL);
    liquid_argparse_add(float,    gamma,       0.001f, 'g', "initial signal level", NULL);
    liquid_argparse_add(unsigned, num_samples, 2000,   'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (bt < 0.0f)
        fprintf(stderr,"error: bandwidth must be positive\n");
    if (num_samples == 0)
        fprintf(stderr,"error: number of samples must be greater than zero\n");
    
    unsigned int i;

    // create objects
    agc_rrrf q = agc_rrrf_create();
    agc_rrrf_set_bandwidth(q, bt);

    float x[num_samples];       // input
    float y[num_samples];       // output
    float rssi[num_samples];    // received signal strength

    // print info
    printf("automatic gain control // loop bandwidth: %4.2e\n",bt);

    // generate signal
    for (i=0; i<num_samples; i++)
        x[i] = gamma * cosf(2*M_PI*0.093f*i);

    // run agc
    for (i=0; i<num_samples; i++) {
        agc_rrrf_execute(q, x[i], &y[i]);
        rssi[i] = agc_rrrf_get_rssi(q);
    }

    // destroy AGC object
    agc_rrrf_destroy(q);

    // 
    // export results
    //
    FILE* fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename);
        exit(1);
    }
    fprintf(fid,"%% %s: auto-generated file\n\n",filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e;\n", i+1, x[i]);
        fprintf(fid,"y(%4u) = %12.4e;\n", i+1, y[i]);
        fprintf(fid,"rssi(%4u)  = %12.4e;\n", i+1, rssi[i]);
    }

    // plot results
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,x, '-','Color',[0 0.2 0.5]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('input');\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,y, '-','Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('output');\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,rssi,'-','LineWidth',1.2,'Color',[0.5 0 0]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('rssi [dB]');\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}

