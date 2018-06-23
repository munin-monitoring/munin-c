/* put here extended versions of <ctype.h>
 * Only use #define tricks to avoid the overhead of func call
 */

/* Defined by the ctype(3) in NetBSD */
#define xisdigit(x) isdigit((int)(unsigned char) (x))
#define xisspace(x) isspace((int)(unsigned char) (x))
