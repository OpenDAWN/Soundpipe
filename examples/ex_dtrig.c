#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "soundpipe.h"

typedef struct {
    sp_osc *osc;
    sp_ftbl *ft, *delta;
    sp_tenv *tenv;
    sp_dtrig *dt;
} udata;

void write_osc(sp_data *data, void *ud) {
    udata *udp = ud;
    SPFLOAT env = 0;
    SPFLOAT osc = 0;
    SPFLOAT dry = 0;
    SPFLOAT trig;
    SPFLOAT dtrig = 0;
    if(data->pos == 0){
        trig = 1.0;
    }else{
        trig = 0.0;
    }
    sp_dtrig_compute(data, udp->dt, &trig, &dtrig);
    sp_tenv_compute(data, udp->tenv, &dtrig, &env);
    sp_osc_compute(data, udp->osc, NULL, &osc);
    data->out[0] = osc * env;
}

int main() {
    srand(time(NULL));
    udata ud;
    uint32_t bufsize;
    sp_data *sp;
    sp_create(&sp);
    sp_auxdata rvaux;

    sp_dtrig_create(&ud.dt);
    sp_tenv_create(&ud.tenv);
    sp_ftbl_create(sp, &ud.ft, 2048);
    sp_ftbl_create(sp, &ud.delta, 4);
    sp_osc_create(&ud.osc);

    ud.delta->tbl[0] = 1.0;
    ud.delta->tbl[1] = 1.0;
    ud.delta->tbl[2] = 0.5;
    ud.delta->tbl[3] = 0.5;
    sp_dtrig_init(sp, ud.dt, ud.delta);
    sp_tenv_init(sp, ud.tenv);
    ud.tenv->atk = 0.03;
    ud.tenv->hold = 0.01;
    ud.tenv->rel =  0.3;

    sp_gen_sine(sp, ud.ft);
    sp_osc_init(sp, ud.osc, ud.ft, 0);
    ud.osc->freq = 1000;
    ud.osc->amp = 0.5;
    sp->len = 44100 * 5;
    sp_process(sp, &ud, write_osc);

    sp_dtrig_destroy(&ud.dt);
    sp_tenv_destroy(&ud.tenv);
    sp_ftbl_destroy(&ud.ft);
    sp_ftbl_destroy(&ud.delta);
    sp_osc_destroy(&ud.osc);
    sp_destroy(&sp);
    return 0;
}
