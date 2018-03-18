class RGBTRIPLE {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;

    friend ostream & operator<<(ostream &, RGBTRIPLE &);
    friend void swap(RGBTRIPLE &, RGBTRIPLE &);
    friend bool operator==(const RGBTRIPLE, const RGBTRIPLE);
    friend class Image;

    public:
        RGBTRIPLE(BYTE = 0x00, BYTE = 0x00, BYTE = 0x00);
        RGBTRIPLE operator+(RGBTRIPLE);
        RGBTRIPLE operator/(int);
        int operator-(RGBTRIPLE);
};

RGBTRIPLE::RGBTRIPLE(BYTE red, BYTE green, BYTE blue) {
    rgbtRed = red;
    rgbtGreen = green;
    rgbtBlue = blue;
}

RGBTRIPLE RGBTRIPLE::operator+(RGBTRIPLE triple) {
    return RGBTRIPLE(rgbtRed + triple.rgbtRed, rgbtGreen + triple.rgbtGreen, rgbtBlue + triple.rgbtBlue);
}

RGBTRIPLE RGBTRIPLE::operator/(int divisor) {
    return RGBTRIPLE(rgbtRed / divisor, rgbtGreen / divisor, rgbtBlue / divisor);
}

int RGBTRIPLE::operator-(RGBTRIPLE triple) {
    return abs(rgbtRed - triple.rgbtRed) + abs(rgbtGreen - triple.rgbtGreen) + abs(rgbtBlue - triple.rgbtBlue);
}

bool operator==(RGBTRIPLE triple1, RGBTRIPLE triple2) {
    return ((triple1.rgbtRed == triple2.rgbtRed) & (triple1.rgbtGreen == triple2.rgbtGreen) & (triple1.rgbtBlue == triple2.rgbtBlue));
}

ostream & operator<<(ostream &obj, RGBTRIPLE &triple) {
    obj << (int)triple.rgbtRed;
    obj << " ";
    obj << (int)triple.rgbtGreen;
    obj << " ";
    obj << (int)triple.rgbtBlue;
    obj << endl;
    return obj;
}

void swap(RGBTRIPLE &t1, RGBTRIPLE &t2) {
    RGBTRIPLE triple = t1;
    t1 = t2;
    t2 = triple;
}