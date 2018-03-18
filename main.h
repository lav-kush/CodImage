class Image;
class Handler;
class RGBTRIPLE;

#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define BG_BLACK    "\x1b[40m"
#define BG_RED  "\x1b[41m"
#define BG_GREEN   "\x1b[42m"
#define BG_YELLOW  "\x1b[43m"
#define BG_BLUE    "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN    "\x1b[46m"
#define BG_WHITE    "\x1b[47m"
#define CLEAR   "\033[H\033[J"

#define vec2D(type) vector < vector <type> >

typedef struct imageData {
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    vec2D(RGBTRIPLE) frame;
} imageData;

void error(string = "");