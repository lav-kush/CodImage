class File {
    protected:
    stack < imageData > prev, next;
};

class Image: File {
    /************************
     *  Image Parameters    *
     ************************/
    string name;
    FILE *img;
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    vec2D(RGBTRIPLE) frame;
    int padding;

    friend class Handler;

    public:
        /************************
         *  Member Functions    *
         ************************/
        Image(string = "");
        ~Image();
        void write(BITMAPFILEHEADER, BITMAPINFOHEADER, vec2D(RGBTRIPLE) &, int);
        void write(Image &);
        void append(vec2D(RGBTRIPLE) &);
        void grow(int = 1);
        void shrink(int = 1);
        void horizontalStretch(int = 1);
        void verticalStretch(int = 1);
        void horizontalCompress(int = 1);
        void verticalCompress(int = 1);
        void horizontalMirror();
        void verticalMirror();
        void rotateClockWise();
        void rotateAntiClockWise();
        void transpose();
        void crop(int = 0, int = 0, int = 0, int = 0);
        void blur();
        void joinFrameHorizontally(Image &);
        void joinFrameVertically(Image &);
        void flooding(int, int, RGBTRIPLE);
        pair <int, int> pattern_search(vec2D(RGBTRIPLE) &);
        void encrypt(string);
        string decrypt();
        void black_white();
        void updateStack();
};

Image::Image(string name)
{
    // empty image
    if (name == "") {
        img = NULL;
        return;
    }

    this->name = name;
    img = fopen(name.c_str(), "r+");
    if( img == NULL )
    {
        img = fopen(name.c_str(), "w+");
        if (img == NULL) {
            error("File could not be opened - " + name);
        }
        return;
    }

    fseek (img, 0, SEEK_END);
    int size = ftell(img);
    fseek (img, 0, SEEK_SET);

    if (size) {
        fread(&bf, sizeof(BITMAPFILEHEADER), 1, img);
        fread(&bi, sizeof(BITMAPINFOHEADER), 1, img);

        if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
                bi.biBitCount != 24 || bi.biCompression != 0) {
            error("File Format not supported - " + name);
            return;
        }

        padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

        frame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
        for(int i = 0; i < abs(bi.biHeight); i++) frame[i] = vector <RGBTRIPLE>(bi.biWidth);

        RGBTRIPLE triple;

        for (int i = 0; i < abs(bi.biHeight); i++) {
            for (int j = 0; j < bi.biWidth; j++) {
                fread(&triple, sizeof(RGBTRIPLE), 1, img);
                frame[i][j] = triple;
            }
            fseek(img, padding, SEEK_CUR);
        }

    }
}

Image::~Image() {
    if (!img) return;
    fclose(img);
}

void Image::updateStack() {
    imageData temp;
    temp.bf = bf;
    temp.bi = bi;
    temp.frame = frame;
    prev.push(temp);
    while(!next.empty()) next.pop();
}

void Image::write(BITMAPFILEHEADER bf, BITMAPINFOHEADER bi, vec2D(RGBTRIPLE) &frame, int padding) {
    this->bf = bf;
    this->bi = bi;
    this->frame = frame;
    this->padding = padding;

    if (!img) return;

    fseek(img, 0, SEEK_SET);
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, img);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, img);

    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    for (int i = 0; i < abs(bi.biHeight); i++)
    {
        for (int j = 0; j < abs(bi.biWidth); j++)
        {
            fwrite(&frame[i][j], sizeof(RGBTRIPLE), 1, img);
        }
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, img);
        }
    }
    ftruncate(fileno(img), bf.bfSize);
    fclose(img);
    img = fopen(name.c_str(), "r+");
}

void Image::write(Image &source) {
    bf = source.bf;
    bi = source.bi;
    frame = source.frame;
    padding = source.padding;

    if (!img) return;

    fseek(img, 0, SEEK_SET);
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, img);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, img);

    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    for (int i = 0; i < abs(bi.biHeight); i++)
    {
        for (int j = 0; j < bi.biWidth; j++)
        {
            fwrite(&frame[i][j], sizeof(RGBTRIPLE), 1, img);
        }
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, img);
        }
    }
    ftruncate(fileno(img), bf.bfSize);
    fclose(img);
    img = fopen(name.c_str(), "r+");
}

void Image::grow(int factor) {
    if (factor <= 0) {
        throw("Invalid Factor");
        return;
    }

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biWidth = bi.biWidth * factor;
    bi.biHeight = bi.biHeight * factor;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) scaledFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) scaledFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(oldHeight); i++) {
        for(int j = 0; j < abs(oldWidth); j++) {
            for(int k = 0; k < factor; k++) {
                for(int l = 0; l < factor; l++) {
                    scaledFrame[i * factor + k][j * factor + l] = frame[i][j];
                }
            }
        }
    }

    write(bf, bi, scaledFrame, padding);
}

void Image::shrink(int factor) {
    if (factor <= 0) {
        throw("Invalid Factor");
        return;
    }

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biWidth = (bi.biWidth + factor - 1) / factor;
    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * ((abs(bi.biHeight) + factor - 1) / factor);
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) scaledFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) scaledFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            int r = 0, g = 0, b = 0;
            int c = 0;
            for(int k = 0; k < factor; k++) {
                if (i * factor + k >= abs(oldHeight)) break;
                for(int l = 0; l < factor; l++) {
                    if (j * factor + l >= abs(oldWidth)) break;
                    r += frame[i * factor + k][j * factor + l].rgbtRed;
                    g += frame[i * factor + k][j * factor + l].rgbtGreen;
                    b += frame[i * factor + k][j * factor + l].rgbtBlue;
                    c++;
                }
            }
            scaledFrame[i][j] = RGBTRIPLE(r / c, g / c, b / c);
        }
    }

    write(bf, bi, scaledFrame, padding);
}

void Image::horizontalStretch(int factor) {
    if (factor <= 0) {
        throw("Invalid Factor");
        return;
    }

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biWidth = bi.biWidth * factor;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) stretchedFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) stretchedFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            stretchedFrame[i][j] = frame[i][j / factor];
        }
    }

    write(bf, bi, stretchedFrame, padding);
}

void Image::horizontalCompress(int factor) {
    if (factor <= 0) {
        throw("Invalid Factor");
        return;
    }

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biWidth = (bi.biWidth + factor - 1) / factor;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) compressedFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) compressedFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            int r = 0, g = 0, b = 0;
            int c = 0;
            for(int k = 0; k < factor; k++) {
                if (j * factor + k >= oldWidth) break;
                r += frame[i][j * factor + k].rgbtRed;
                g += frame[i][j * factor + k].rgbtGreen;
                b += frame[i][j * factor + k].rgbtBlue;
                c++;
            }
            compressedFrame[i][j] = RGBTRIPLE(r / c, g / c, b / c);
        }
    }

    write(bf, bi, compressedFrame, padding);
}

void Image::verticalStretch(int factor) {
    if (factor <= 0) {
        throw("Invalid Factor");
        return;
    }

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biHeight = bi.biHeight * factor;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) stretchedFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) stretchedFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            stretchedFrame[i][j] = frame[i / factor][j];
        }
    }

    write(bf, bi, stretchedFrame, padding);
}

void Image::verticalCompress(int factor) {
    if (factor <= 0) {
        throw("Invalid Factor");
        return;
    }

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * (abs(bi.biHeight) + factor - 1) / factor;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) compressedFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) compressedFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            int r = 0, g = 0, b = 0;
            int c = 0;
            for(int k = 0; k < factor; k++) {
                if (i * factor + k >= abs(oldHeight)) break;
                r += frame[i * factor + k][j].rgbtRed;
                g += frame[i * factor + k][j].rgbtGreen;
                b += frame[i * factor + k][j].rgbtBlue;
                c++;
            }
            compressedFrame[i][j] = RGBTRIPLE(r / c, g / c, b / c);
        }
    }

    write(bf, bi, compressedFrame, padding);
}

void Image::horizontalMirror() {
    updateStack();

    vec2D(RGBTRIPLE) mirrorFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) mirrorFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            mirrorFrame[i][j] = frame[i][j];
        }
    }

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth) / 2; j++) {
            swap(mirrorFrame[i][j], mirrorFrame[i][abs(bi.biWidth) - j - 1]);
        }
    }

    write(bf, bi, mirrorFrame, padding);
}

void Image::verticalMirror() {
    updateStack();

    vec2D(RGBTRIPLE) mirrorFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) mirrorFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            mirrorFrame[i][j] = frame[i][j];
        }
    }

    for(int i = 0; i < abs(bi.biHeight) / 2; i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            swap(mirrorFrame[i][j], mirrorFrame[abs(bi.biHeight) - i - 1][j]);
        }
    }

    write(bf, bi, mirrorFrame, padding);
}

void Image::rotateClockWise() {
    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * abs(oldWidth);
    bi.biWidth = abs(oldHeight);
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            newFrame[i][j] = frame[j][abs(bi.biHeight) - i - 1];
        }
    }

    write(bf, bi, newFrame, padding);
}

void Image::rotateAntiClockWise() {
    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * abs(oldWidth);
    bi.biWidth = abs(oldHeight);
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            newFrame[i][j] = frame[bi.biWidth - j - 1][i];
        }
    }

    write(bf, bi, newFrame, padding);
}

void Image::transpose() {
    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * abs(oldWidth);
    bi.biWidth = oldHeight;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            newFrame[i][j] = frame[bi.biWidth - j - 1][bi.biHeight - i - 1];
        }
    }

    write(bf, bi, newFrame, padding);
}

void Image::crop(int startx, int starty, int width, int height) {
    if (startx < 0 || startx >= bi.biWidth || starty < 0 || starty >= abs(bi.biHeight) ||
        startx + width - 1 >= bi.biWidth || starty + height - 1 >= abs(bi.biHeight)) {
        throw("Invalid Arguments for Crop");
        return;
    }

    updateStack();

    starty = abs(bi.biHeight) - starty - height;

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * height;
    bi.biWidth = width;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            newFrame[i][j] = frame[i + starty][j + startx];
        }
    }

    /*for(int i = 0; i < 50; i++) {
        for(int j = 0; j < 50; j++) {
            cout << frame[i + 304][j] << " ";
        } cout << endl;
    }*/

    write(bf, bi, newFrame, padding);
}

void Image::blur() {

    updateStack();

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            newFrame[i][j] = frame[i][j];
        }
    }

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            int r = 0, g = 0, b = 0;
            int c = 0;
            if (i) {
                r += frame[i - 1][j].rgbtRed;
                g += frame[i - 1][j].rgbtGreen;
                b += frame[i - 1][j].rgbtBlue;
                c++;
            }
            if (j) {
                r += frame[i][j - 1].rgbtRed;
                g += frame[i][j - 1].rgbtGreen;
                b += frame[i][j - 1].rgbtBlue;
                c++;
            }
            if (i + 1 <= abs(bi.biHeight) - 1) {
                r += frame[i + 1][j].rgbtRed;
                g += frame[i + 1][j].rgbtGreen;
                b += frame[i + 1][j].rgbtBlue;
                c++;
            }
            if (j + 1 <= abs(bi.biWidth) - 1) {
                r += frame[i][j + 1].rgbtRed;
                g += frame[i][j + 1].rgbtGreen;
                b += frame[i][j + 1].rgbtBlue;
                c++;
            }
            if (i && j) {
                r += frame[i - 1][j - 1].rgbtRed;
                g += frame[i - 1][j - 1].rgbtGreen;
                b += frame[i - 1][j - 1].rgbtBlue;
                c++;
            }
            if (i && j + 1 <= abs(bi.biWidth) - 1) {
                r += frame[i - 1][j + 1].rgbtRed;
                g += frame[i - 1][j + 1].rgbtGreen;
                b += frame[i - 1][j + 1].rgbtBlue;
                c++;
            }
            if (i + 1 <= abs(bi.biHeight) - 1 && j) {
                r += frame[i + 1][j - 1].rgbtRed;
                g += frame[i + 1][j - 1].rgbtGreen;
                b += frame[i + 1][j - 1].rgbtBlue;
                c++;
            }
            if (i + 1 <= abs(bi.biHeight) - 1 && j + 1 <= abs(bi.biWidth) - 1) {
                r += frame[i + 1][j + 1].rgbtRed;
                g += frame[i + 1][j + 1].rgbtGreen;
                b += frame[i + 1][j + 1].rgbtBlue;
                c++;
            }
            newFrame[i][j] = RGBTRIPLE(r / c, g / c, b / c);
        }
    }

    write(bf, bi, newFrame, padding);
}

void Image::joinFrameHorizontally(Image &newImage) {

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biWidth = max(bi.biWidth, newImage.bi.biWidth);
    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * (abs(bi.biHeight) + abs(newImage.bi.biHeight));
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(newImage.bi.biHeight); i++) {
        for(int j = 0; j < abs(newImage.bi.biWidth); j++) {
            newFrame[i][j] = newImage.frame[i][j];
        }
    }

    for(int i = 0; i < abs(oldHeight); i++) {
        for(int j = 0; j < abs(oldWidth); j++) {
            newFrame[i + abs(newImage.bi.biHeight)][j] = frame[i][j];
        }
    }

    write(bf, bi, newFrame, padding);
}

void Image::joinFrameVertically(Image &newImage) {

    updateStack();

    LONG oldWidth = bi.biWidth;
    LONG oldHeight = bi.biHeight;

    bi.biHeight = (bi.biHeight < 0 ? -1 : 1) * max(abs(bi.biHeight), abs(newImage.bi.biHeight));
    bi.biWidth = bi.biWidth + newImage.bi.biWidth;
    padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((abs(bi.biWidth) * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight));
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(oldHeight); i++) {
        for(int j = 0; j < abs(oldWidth); j++) {
            newFrame[i][j] = frame[i][j];
        }
    }

    for(int i = 0; i < abs(newImage.bi.biHeight); i++) {
        for(int j = 0; j < abs(newImage.bi.biWidth); j++) {
            newFrame[i][j + oldWidth] = newImage.frame[i][j];
        }
    }

    write(bf, bi, newFrame, padding);
}

#define max_diff 20

void Image::flooding(int start_x, int start_y, RGBTRIPLE triple) {

    if (start_x < 0 || start_x >= abs(bi.biHeight) || start_y < 0 || start_y >= abs(bi.biWidth)) {
        throw("Invalid Arguments for flooding");
        return;
    }

    updateStack();

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            newFrame[i][j] = frame[i][j];
        }
    }

    int visited[abs(bi.biHeight)][bi.biWidth];
    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < bi.biWidth; j++) {
            visited[i][j] = 0;
        }
    }
    start_x = abs(bi.biHeight) - start_x - 1;
    stack < pair <int, int> > coord;
    coord.push(make_pair(start_x, start_y));
    while(!coord.empty()) {
        pair <int, int> coordPair = coord.top();
        visited[coordPair.first][coordPair.second] = 1;
        RGBTRIPLE last = frame[coordPair.first][coordPair.second];
        coord.pop();
        if (coordPair.first &&
            !visited[coordPair.first - 1][coordPair.second] &&
            last - frame[coordPair.first - 1][coordPair.second] <= max_diff) {
                coord.push(make_pair(coordPair.first - 1, coordPair.second));
        }
        if (coordPair.second &&
            !visited[coordPair.first][coordPair.second - 1] &&
            last - frame[coordPair.first][coordPair.second - 1] <= max_diff) {
                coord.push(make_pair(coordPair.first, coordPair.second - 1));
        }
        if (coordPair.first < abs(bi.biHeight) - 1 &&
            !visited[coordPair.first + 1][coordPair.second] &&
            last - frame[coordPair.first + 1][coordPair.second] <= max_diff) {
                coord.push(make_pair(coordPair.first + 1, coordPair.second));
        }
        if (coordPair.second < bi.biWidth - 1 &&
            !visited[coordPair.first][coordPair.second + 1] &&
            last - frame[coordPair.first][coordPair.second + 1] <= max_diff) {
                coord.push(make_pair(coordPair.first, coordPair.second + 1));
        }
        newFrame[coordPair.first][coordPair.second] = triple;
    }

    write(bf, bi, newFrame, padding);
}

pair <int, int> Image::pattern_search(vec2D(RGBTRIPLE) &needle) {

    if (abs(bi.biHeight) < needle.size() || bi.biWidth < needle[0].size()) {
        return make_pair(-1, -1);
    }

    for(int i = 0; i <= abs(bi.biHeight) - needle.size(); i++) {
        for(int j = 0; j <= abs(bi.biWidth) - needle[0].size(); j++) {
            bool flag2 = false;
            for(int k = 0; k < needle.size(); k++) {
                for(int l = 0; l < needle[0].size(); l++) {
                    if (!(frame[i + k][j + l] == needle[k][l])) {
                        flag2 = true;
                        break;
                    }
                }
                if (flag2) break;
            }
            if (!flag2) {
                return make_pair(abs(bi.biHeight) - (i + needle.size()), j);
            }
        }
    }
    return make_pair(-1, -1);
}

void Image::encrypt(string msg) {

    updateStack();

    msg += '\\';
    if (4 * msg.size() > abs(bi.biHeight) * abs(bi.biWidth) * 3) {
        throw("Insufficient image size to encrypt message");
        return;
    }

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            newFrame[i][j] = frame[i][j];
        }
    }

    BYTE mask = 0xfc, var_mask = 0xc0;
    int cur_char = 0, c = 3;

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            for(int k = 0; k < 3; k++) {

                if (k == 0) {
                    newFrame[i][j].rgbtRed = (frame[i][j].rgbtRed & mask) | ((msg[cur_char] & var_mask) >> (2 * c));
                } else if(k == 1) {
                    newFrame[i][j].rgbtGreen = (frame[i][j].rgbtGreen & mask) | ((msg[cur_char] & var_mask) >> (2 * c));
                } else if (k == 2) {
                    newFrame[i][j].rgbtBlue = (frame[i][j].rgbtBlue & mask) | ((msg[cur_char] & var_mask) >> (2 * c));
                }

                var_mask >>= 2;
                c--;
                if (!var_mask) {
                    var_mask = 0xc0;
                    c = 3;
                    cur_char++;
                    if (cur_char == msg.size()) {
                        write(bf, bi, newFrame, padding);
                        return;
                    }
                }

            }
        }
    }

    write(bf, bi, newFrame, padding);
}

string Image::decrypt() {
    string msg;
    BYTE mask = 0x03;
    char build = 0x00;
    int c = 3;

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            for(int k = 0; k < 3; k++) {

                if (k == 0) {
                    build |= (frame[i][j].rgbtRed & mask) << (2 * c);
                } else if (k == 1) {
                    build |= (frame[i][j].rgbtGreen & mask) << (2 * c);
                } else if (k == 2) {
                    build |= (frame[i][j].rgbtBlue & mask) << (2 * c);
                }

                c--;
                if (c == -1) {
                    if (build == '\\') return msg;
                    msg += build;
                    build = 0x00;
                    c = 3;
                }

            }
        }
    }
    msg = "";
    return msg;
}

#define TOP_VALUE 255

void Image::black_white() {

    updateStack();

    vec2D(RGBTRIPLE) newFrame = vec2D(RGBTRIPLE)(abs(bi.biHeight));
    for(int i = 0; i < abs(bi.biHeight); i++) newFrame[i] = vector <RGBTRIPLE>(bi.biWidth);

    for(int i = 0; i < abs(bi.biHeight); i++) {
        for(int j = 0; j < abs(bi.biWidth); j++) {
            int x = (int)((frame[i][j].rgbtRed + frame[i][j].rgbtGreen + frame[i][j].rgbtBlue) / 3.0 / 255 * TOP_VALUE);
            newFrame[i][j] = RGBTRIPLE(x, x, x);
        }
    }

    write(bf, bi, newFrame, padding);
}