/* modification of matread but operating on words instead of floats */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <cairo/cairo.h>
#include "txtread.h"
#include "col0.h" // look for colorist
/* principal image and block settings */
#define IMWIDTH 800
#define IMHEIGHT 800
#define TM 20
#define LM 30
const float FONTSZ=10.0; /* the size (number of colours) that our colorscheme has */

typedef struct /*pwma_t */
{
    int *v;
    int nr, nc; /* num rows, numcols */
    char **r1;
    char **c1;
} pwma_t;

w_c *crea_wc(unsigned initsz)
{
    w_c *wc=malloc(sizeof(w_c));
    wc->lp1=initsz;
    wc->t=STRG;
    wc->w=malloc(wc->lp1*sizeof(char));
    return wc;
}

void reall_wc(w_c **wc, unsigned *cbuf)
{
    w_c *twc=*wc;
    unsigned tcbuf=*cbuf;
    tcbuf += CBUF;
    twc->lp1=tcbuf;
    twc->w=realloc(twc->w, tcbuf*sizeof(char));
    *wc=twc; /* realloc can often change the ptr */
    *cbuf=tcbuf;
    return;
}

void norm_wc(w_c **wc)
{
    w_c *twc=*wc;
    twc->w=realloc(twc->w, twc->lp1*sizeof(char));
    *wc=twc; /* realloc can often change the ptr */
    return;
}

void free_wc(w_c **wc)
{
    w_c *twc=*wc;
    free(twc->w);
    free(twc);
    return;
}

aw_c *crea_awc(unsigned initsz)
{
    int i;
    aw_c *awc=malloc(sizeof(aw_c));
    awc->ab=initsz;
    awc->al=awc->ab;
    awc->stsps=0;
    awc->sttbs=0;
    awc->aw=malloc(awc->ab*sizeof(w_c*));
    for(i=0;i<awc->ab;++i) 
        awc->aw[i]=crea_wc(CBUF);
    return awc;
}

void reall_awc(aw_c **awc, unsigned buf)
{
    int i;
    aw_c *tawc=*awc;
    tawc->ab += buf;
    tawc->al=tawc->ab;
    tawc->stsps=0;
    tawc->sttbs=0;
    tawc->aw=realloc(tawc->aw, tawc->ab*sizeof(aw_c*));
    for(i=tawc->ab-buf;i<tawc->ab;++i)
        tawc->aw[i]=crea_wc(CBUF);
    *awc=tawc;
    return;
}

void norm_awc(aw_c **awc)
{
    int i;
    aw_c *tawc=*awc;
    /* free the individual w_c's */
    for(i=tawc->al;i<tawc->ab;++i) 
        free_wc(tawc->aw+i);
    /* now release the pointers to those freed w_c's */
    tawc->aw=realloc(tawc->aw, tawc->al*sizeof(aw_c*));
    *awc=tawc;
    return;
}

void free_awc(aw_c **awc)
{
    int i;
    aw_c *tawc=*awc;
    for(i=0;i<tawc->al;++i) 
        free_wc(tawc->aw+i);
    free(tawc->aw); /* unbelieveable: I left this out, couldn't find where I leaking the memory! */
    free(tawc);
    return;
}

aaw_c *crea_aawc(unsigned initsz)
{
    int i;
    unsigned lbuf=initsz;
    aaw_c *aawc=malloc(sizeof(aaw_c));
    aawc->numl=0;
    aawc->aaw=malloc(lbuf*sizeof(aw_c*));
    for(i=0;i<initsz;++i) 
        aawc->aaw[i]=crea_awc(WABUF);
    /* ppa, this guys ia bit independent: will use CBUF as buffer increaser */
    aawc->ppb=CBUF;
    aawc->ppsz=0;
    aawc->ppa=malloc(CBUF*sizeof(int));
    return aawc;
}

void free_aawc(aaw_c **aw)
{
    int i;
    aaw_c *taw=*aw;
    for(i=0;i<taw->numl;++i) /* tried to release 1 more, no go */
        free_awc(taw->aaw+i);
    free(taw->ppa);
    free(taw->aaw);
    free(taw);
}

void prtaawcdbg(aaw_c *aawc)
{
    int i, j, k;
    for(i=0;i<aawc->numl;++i) {
        printf("l.%u(%u): ", i, aawc->aaw[i]->al); 
        for(j=0;j<aawc->aaw[i]->al;++j) {
            printf("w_%u: ", j); 
            if(aawc->aaw[i]->aw[j]->t == NUMS) {
                printf("NUM! "); 
                continue;
            } else if(aawc->aaw[i]->aw[j]->t == PNI) {
                printf("PNI! "); 
                continue;
            } else if(aawc->aaw[i]->aw[j]->t == STCP) {
                printf("STCP! "); 
                continue;
            }
            for(k=0;k<aawc->aaw[i]->aw[j]->lp1-1; k++)
                putchar(aawc->aaw[i]->aw[j]->w[k]);
            printf("/%u ", aawc->aaw[i]->aw[j]->lp1-1); 
        }
        printf("\n"); 
    }
}

pwma_t *c2pwma(aaw_c *aawc) /* convert to pwma_t */
{
    int i, j;
    pwma_t *pwma=malloc(sizeof(pwma_t));
    pwma->nr=aawc->numl-1;
    pwma->nc=aawc->numl-1;
    pwma->v=malloc(pwma->nr*pwma->nc*sizeof(int));
    pwma->r1=malloc(pwma->nr*sizeof(char*));
    pwma->c1=malloc(pwma->nc*sizeof(char*));
    for(i=0;i<aawc->numl;++i) {
        if(i==0) {
            for(j=0;j<aawc->aaw[i]->al;++j) {
                pwma->r1[j]=calloc(aawc->aaw[i]->aw[j]->lp1, sizeof(char));
                strcpy(pwma->r1[j], aawc->aaw[i]->aw[j]->w);
            }
        } else {
            for(j=0;j<aawc->aaw[i]->al;++j) {
                if(j==0) {
                    pwma->c1[i-1]=calloc(aawc->aaw[i]->aw[j]->lp1, sizeof(char));
                    strcpy(pwma->c1[i-1], aawc->aaw[i]->aw[j]->w);
                } else
                    pwma->v[(i-1)*pwma->nc+(j-1)]=atoi(aawc->aaw[i]->aw[j]->w);
            }
        }
    }
    return pwma;
}

void prtpwma(pwma_t *pwma)
{
    int i, j;
    for(i=0;i<pwma->nr;++i) {
        if(i==0)
            for(j=0;j<pwma->nc; j++) 
                printf("%s ", pwma->r1[j]);
        else {
            for(j=0;j<pwma->nc; j++) {
                if(j==0)
                    printf("%s ", pwma->c1[i]);
                else
                    printf("%i ", pwma->v[(i-1)*pwma->nc+(j-1)]);
            }
        }
        printf("\n"); 
    }
    return;
}

void prtpwma2(pwma_t *pwma)
{
    int i;
    for(i=0;i<pwma->nr;++i)
        printf("%s ", pwma->r1[i]);
    printf("\n"); 
    for(i=0;i<pwma->nr;++i)
        printf("%s ", pwma->c1[i]);
    printf("\n"); 
    return;
}

void prt_tnum(aaw_c *aawc) /* will print matching numbers of mm:ss style */
{
    int i, j, k;
    for(i=0;i<aawc->numl;++i) {
        for(j=0;j<aawc->aaw[i]->al;++j) {
            if(aawc->aaw[i]->aw[j]->t == TNUM) {
                for(k=0;k<aawc->aaw[i]->aw[j]->lp1-1; k++)
                    putchar(aawc->aaw[i]->aw[j]->w[k]);
                printf("\n"); 
            }
        }
    }
}

aaw_c *processinpf(char *fname)
{
    /* declarations */
    FILE *fp=fopen(fname,"r");
    int i;
    size_t couc /*count chars per line */, couw=0 /* count words */;
    int c, oldc='\0', ooldc='\0' /* pcou=0 paragraph counter */;
    boole inword=0;
    boole linestart=1; /* want to catch number of stating spaces or tabs */
    unsigned lbuf=LBUF /* buffer for number of lines */, cbuf=CBUF /* char buffer for size of w_c's: reused for every word */;
    aaw_c *aawc=crea_aawc(lbuf); /* array of words per line */

    while( (c=fgetc(fp)) != EOF) {
        if( (c== '\n') | (c == ' ') | (c == '\t') ) {
            if( inword==1) { /* cue word-ending procedure */
                aawc->aaw[aawc->numl]->aw[couw]->w[couc++]='\0';
                aawc->aaw[aawc->numl]->aw[couw]->lp1=couc;
                SETCPTYPE(oldc, aawc->aaw[aawc->numl]->aw[couw]->t);
                norm_wc(aawc->aaw[aawc->numl]->aw+couw);
                couw++; /* verified: this has to be here */
            }
            if(c=='\n') { /* cue line-ending procedure */
                if(aawc->numl ==lbuf-1) {
                    lbuf += LBUF;
                    aawc->aaw=realloc(aawc->aaw, lbuf*sizeof(aw_c*));
                    for(i=lbuf-LBUF; i<lbuf; ++i)
                        aawc->aaw[i]=crea_awc(WABUF);
                }
                aawc->aaw[aawc->numl]->al=couw;
                norm_awc(aawc->aaw+aawc->numl);
                if(oldc=='\n') {
                    CONDREALLOC(aawc->ppsz, aawc->ppb, CBUF, aawc->ppa, int);
                    aawc->ppa[aawc->ppsz++]=aawc->numl;
                }
                aawc->numl++;
                couw=0;
                linestart=1;
            } else if (linestart) /* must be a space or a tab */
                (c == ' ')? aawc->aaw[aawc->numl]->stsps++ : aawc->aaw[aawc->numl]->sttbs++;
            inword=0;
        } else if(inword==0) { /* a normal character opens word */
            if(couw ==aawc->aaw[aawc->numl]->ab-1) /* new word opening */
                reall_awc(aawc->aaw+aawc->numl, WABUF);
            couc=0;
            cbuf=CBUF;
            aawc->aaw[aawc->numl]->aw[couw]->w[couc++]=c;
            GETLCTYPE(c, aawc->aaw[aawc->numl]->aw[couw]->t); /* MACRO: the leading character gives a clue */
            inword=1;
            linestart=0;
        } else if(inword) { /* simply store */
            if(couc == cbuf-1)
                reall_wc(aawc->aaw[aawc->numl]->aw+couw, &cbuf);
            aawc->aaw[aawc->numl]->aw[couw]->w[couc++]=c;
            /* if word is a candidate for a NUM or PNI (i.e. via its first character), make sure it continues to obey rules: a MACRO */
            IWMODTYPEIF(c, aawc->aaw[aawc->numl]->aw[couw]->t);
        }
        ooldc=oldc;
        oldc=c;
    } /* end of big for statement */
    fclose(fp);

    /* normalization stage */
    for(i=aawc->numl; i<lbuf; ++i) {
        free_awc(aawc->aaw+i);
    }
    aawc->aaw=realloc(aawc->aaw, aawc->numl*sizeof(aw_c*));
    aawc->ppa=realloc(aawc->ppa, aawc->ppsz*sizeof(int));

    return aawc;
}

int main(int argc, char *argv[])
{
    /* argument accounting */
    if(argc!=2) {
        printf("Error. Pls supply argument (name of text file).\n");
        exit(EXIT_FAILURE);
    }
    int i, j, m;

    aaw_c *aawc=processinpf(argv[1]);

    printf("Numlines: %zu\n", aawc->numl); 
    printf("Numparas: %d\n", aawc->ppsz); 

    pwma_t *pwma=c2pwma(aawc);
    free_aawc(&aawc);
    prtpwma2(pwma);
    int min=0x7FFFFFFF, max=0;
    int rbyc=pwma->nr * pwma->nc;
    for(i=0;i<rbyc;++i) {
        if((pwma->v[i] ==999) & (pwma->v[i] == 0) )
            continue;
        if(pwma->v[i] < min) 
            min=pwma->v[i];
        if(pwma->v[i] > max)
            max=pwma->v[i];
    }

    float cbsz = (float)(IMWIDTH-LM)/(pwma->nc-1); /* size of each column-band */;
    float rbsz = (float)(IMHEIGHT-TM)/(pwma->nr-1); /* size of each row-band */;

    /*** cairo gear ***/
    cairo_surface_t *surface;
    cairo_t *cr;
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, IMWIDTH, IMHEIGHT);
    cr = cairo_create (surface);
    cairo_rectangle (cr, 0, 0, IMWIDTH, IMHEIGHT); /* arg explan: topleftcorner and size of shape  */
    cairo_set_source_rgba (cr, 0, 0, 0, 0.95); /*  final number is alpha, 1.0 is opaque */
    cairo_fill (cr);
    /* OK, now, text consideration */
    cairo_select_font_face (cr, "Monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, FONTSZ);
    cairo_font_extents_t fe;
    cairo_text_extents_t te;

    float rstx /*rect start x-dir */, rsty /*rect start y-dir */;
    for (j=0; j<pwma->nc-1; j++) { /* i and j serve as our block indices */
        cairo_text_extents (cr, pwma->r1[j], &te);
        cairo_move_to (cr, (LM+cbsz*j+cbsz/2) - te.x_bearing - te.width/2, TM - fe.descent-4);
        cairo_set_source_rgb(cr, 0.9, 0.9, .9);
        cairo_show_text (cr, pwma->r1[j]);
    }
    for (i=0; i<pwma->nr-1; i++) {
        cairo_text_extents (cr, pwma->c1[i], &te);
        // cairo_move_to (cr, 4 - te.x_bearing - te.width, TM+i*rbsz +rbsz/2.- fe.descent + fe.height/2);
        cairo_move_to (cr, 4, TM+i*rbsz +rbsz/2.- fe.descent + fe.height/2);
        cairo_set_source_rgb(cr, 0.9, 0.9, .9);
        cairo_show_text (cr, pwma->c1[i]);
        for (j=0; j<pwma->nc-1; j++) { /* i and j serve as our block indices */
            if((pwma->v[i*pwma->nc+j] ==999) & (pwma->v[i*pwma->nc+j] == 0) )
                continue;
            m=32*(pwma->v[i*pwma->nc+j] - min) / (max - min);
#ifdef DBG
            printf("%s vs. %s: m is %i\n", pwma->r1[i], pwma->c1[j], m); 
#endif
            cairo_set_source_rgb(cr, (float)col0[m].r/255., (float)col0[m].g/255., (float)col0[m].b/255.);
            rstx=LM + cbsz*j;
            rsty=TM + rbsz*i;
            cairo_rectangle (cr, rstx, rsty, cbsz, rbsz);
            cairo_fill (cr);
        }
    }
    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "rcol3.png");
    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    /* freeing stuff */
    free(pwma->v);
    for(i=0;i<pwma->nr; i++) {
        free(pwma->r1[i]);
        free(pwma->c1[i]);
    }
    free(pwma->r1);
    free(pwma->c1);
    free(pwma);

    return 0;
}
