/* Not working */
#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "col0.h" // look for colorist

/* principal image and block settings */
#define IMWIDTH 1600
#define IMHEIGHT 1600
#define TM 20
#define LM 30
#define RBNUM 77
#define CBNUM 77

const float FONTSZ=10.0; /* the size (number of colours) that our colorscheme has */

int main (int argc, char *argv[])
{
    char *na[4]={ "ONE", "TWO", "TRI", "FOR" };
    float cbsz = (float)(IMWIDTH-LM)/CBNUM /* size of each column-band */;
    float rbsz = (float)(IMHEIGHT-TM)/RBNUM /* size of each row-band */;

    /* SPECIAL NOTE: we use the header file which already has a global colour sturc and array(called "colorist"), please see colorhdr.h */

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

    int i, j, m;
    float rstx /*rect start x-dir */, rsty /*rect start y-dir */;
    for (j=0; j<CBNUM; j++) { /* i and j serve as our block indices */
        cairo_text_extents (cr, na[j%4], &te);
        // cairo_move_to (cr, (LM+cbsz*j+cbsz/2) - te.x_bearing - te.width/2, TM - fe.descent + fe.height/2);
        cairo_move_to (cr, (LM+cbsz*j+cbsz/2) - te.x_bearing - te.width/2, TM - fe.descent-4);
        cairo_set_source_rgb(cr, 0.9, 0.9, .9);
        cairo_show_text (cr, na[j%4]);
    }
    for (i=0; i<RBNUM; i++) {
        cairo_text_extents (cr, na[i%4], &te);
        // cairo_move_to (cr, 4 - te.x_bearing - te.width, TM+i*rbsz +rbsz/2.- fe.descent + fe.height/2);
        cairo_move_to (cr, 4, TM+i*rbsz +rbsz/2.- fe.descent + fe.height/2);
        cairo_set_source_rgb(cr, 0.9, 0.9, .9);
        cairo_show_text (cr, na[i%4]);
        for (j=0; j<CBNUM; j++) { /* i and j serve as our block indices */
            m=(i*CBNUM+j)%32;
            cairo_set_source_rgb(cr, (float)col1[m].r/255., (float)col1[m].g/255., (float)col1[m].b/255.);
            rstx=LM + cbsz*j;
            rsty=TM + rbsz*i;
            cairo_rectangle (cr, rstx, rsty, cbsz, rbsz);
            cairo_fill (cr);
        }
    }

    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "rcol.png");
    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    return 0;
}
