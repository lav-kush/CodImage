class Handler {
    public:
        /************************
         *  Image Operations    *
         ************************/
        void getWidth(Image &);
        void getHeight(Image &);
        void copy(Image &, Image &);
        void scale(Image &, int = 1, int = 1);
        void stretch(Image &, int = 1, int = 1, int = 1, int = 1);
        void rotate(Image &);
        void horizontalMirror(Image &);
        void verticalMirror(Image &);
        void rotateClockWise(Image &);
        void rotateAntiClockWise(Image &);
        void transpose(Image &);
        void crop(Image &, int, int, int, int);
        void blur(Image &);
        void undo(Image &);
        void redo(Image &);
        void mergeHorizontally(Image &, Image &);
        void mergeVertically(Image &, Image &);
        void fillColor(Image &, int, int, RGBTRIPLE);
        void find(Image &, Image &);
        void hideMSG(Image &, string);
        void showMSG(Image &);
        void black_white(Image &);
};

void Handler::getWidth(Image &source) {
    cout << "Width of " << source.name << ": " << source.bi.biWidth << " Pixels" << endl;
}

void Handler::getHeight(Image &source) {
    cout << "Height of " << source.name << ": " << abs(source.bi.biHeight) << " Pixels" << endl;
}

void Handler::copy(Image &destination, Image &source) {
    destination.write(source);
    cout << "image: " << source.name << " copied to image: " << destination.name << " successfully" << endl;
}

void Handler::scale(Image &source, int numerator, int denominator) {
    source.grow(numerator);
    source.shrink(denominator);
    cout << "image: " << source.name << " grown by factor: " << numerator << " and shrinked by factor: " << denominator << endl;
}

void Handler::stretch(Image &source, int horizontalNumerator, int horizontalDenominator, int verticalNumerator, int verticalDenominator) {
    if (horizontalNumerator < 0 || horizontalDenominator <= 0 || verticalNumerator < 0 || verticalDenominator <= 0) {
        throw("Invalid Stretching Arguments");
    }
    source.horizontalStretch(horizontalNumerator);
    source.horizontalCompress(horizontalDenominator);
    source.verticalStretch(verticalNumerator);
    source.verticalCompress(verticalDenominator);
    cout << "image: " << source.name << " horizontally stretched by factor: " << horizontalNumerator << ", compressed horizontally by factor: " << horizontalDenominator << ", vertically stretched by factor: " << verticalNumerator << ", compressed vertically by factor: " << verticalDenominator << endl;
}

void Handler::horizontalMirror(Image &source) {
    source.horizontalMirror();
    cout << "image: " << source.name << " mirrored horizontally successfully" << endl;
}

void Handler::verticalMirror(Image &source) {
    source.verticalMirror();
    cout << "image: " << source.name << " mirrored vertically successfully" << endl;
}

void Handler::rotateClockWise(Image &source) {
    source.rotateClockWise();
    cout << "image: " << source.name << " rotated clock wise successfully" << endl;
}

void Handler::rotateAntiClockWise(Image &source) {
    source.rotateAntiClockWise();
    cout << "image: " << source.name << " rotated anti clock wise successfully" << endl;
}

void Handler::transpose(Image &source) {
    source.transpose();
    cout << "image: " << source.name << " transposed successfully" << endl;
}

void Handler::crop(Image &source, int startx, int starty, int width, int height) {
    if (startx < 0 || startx >= source.bi.biWidth || starty < 0 || starty >= abs(source.bi.biHeight) ||
        startx + width - 1 >= source.bi.biWidth || starty + height - 1 >= abs(source.bi.biHeight)) {
        throw("Invalid Arguments for Crop");
    }
    source.crop(startx, starty, width, height);
    cout << "image: " << source.name << " cropped from " << startx << ", " << starty << " to " << (startx + source.bi.biWidth) << ", " << (starty + abs(source.bi.biHeight)) << endl;
}

void Handler::blur(Image &source) {
    source.blur();
    cout << "image: " << source.name << " blurred successfully" << endl;
}

void Handler::undo(Image &source) {
    if (source.prev.empty()) {
        throw("Undo Stack Empty");
    }
    imageData temp;
    temp.bf = source.bf;
    temp.bi = source.bi;
    temp.frame = source.frame;
    source.next.push(temp);
    temp = source.prev.top();
    source.bf = temp.bf;
    source.bi = temp.bi;
    source.frame = temp.frame;
    source.prev.pop();
    source.padding = (4 - (source.bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    source.write(source);
    cout << "image: " << source.name << " undo successfully" << endl;
}

void Handler::redo(Image &source) {
    if (source.next.empty()) {
        throw("Redo Stack Empty");
    }
    imageData temp;
    temp.bf = source.bf;
    temp.bi = source.bi;
    temp.frame = source.frame;
    source.prev.push(temp);
    temp = source.next.top();
    source.bf = temp.bf;
    source.bi = temp.bi;
    source.frame = temp.frame;
    source.next.pop();
    source.padding = (4 - (source.bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    source.write(source);
    cout << "image: " << source.name << " redo successfully" << endl;
}

void Handler::mergeHorizontally(Image &destination, Image &source) {
    Image temp;
    temp.write(source);

    if (destination.bi.biWidth / temp.bi.biWidth > 0) {
        temp.grow(destination.bi.biWidth / temp.bi.biWidth);
    }
    if (temp.bi.biWidth / destination.bi.biWidth > 0) {
        temp.shrink(temp.bi.biWidth / destination.bi.biWidth);
    }
    destination.joinFrameHorizontally(temp);
    cout << "image: " << source.name << " merged horizontally to " << destination.name << " successfully" << endl;
}

void Handler::mergeVertically(Image &destination, Image &source) {
    Image temp;
    temp.write(source);

    if (destination.bi.biHeight / temp.bi.biHeight > 0) {
        temp.grow(destination.bi.biHeight / temp.bi.biHeight);
    }
    if (temp.bi.biHeight / destination.bi.biHeight > 0) {
        temp.shrink(temp.bi.biHeight / destination.bi.biHeight);
    }
    destination.joinFrameVertically(temp);
    cout << "image: " << source.name << " merged vertically to " << destination.name << " successfully" << endl;
}

void Handler::fillColor(Image &source, int x, int y, RGBTRIPLE triple) {

    if (x < 0 || x >= abs(source.bi.biHeight) || y < 0 || y >= abs(source.bi.biWidth)) {
        throw("Invalid Arguments for Filling Color");
    }

    source.flooding(x, y, triple);
    cout << "image: " << source.name << " is filled with color " << triple << " flooded from row#" << x << ", column#" << y << " successfully" << endl;
}

void Handler::find(Image &haystack, Image &needle) {
    pair <int, int> coordPair = haystack.pattern_search(needle.frame);
    if (coordPair.first == -1 && coordPair.second == -1) {
        cout << needle.name << " not found in " << haystack.name << endl;
    } else {
        cout << needle.name << " found in " << haystack.name << " at Row #" << coordPair.first << " Column #" << coordPair.second << endl;
    }
}

void Handler::hideMSG(Image &source, string msg) {
    source.encrypt(msg);
    cout << "image: " << source.name << " is encrypted with message \"" << msg << "\" successfully" << endl;
}

void Handler::showMSG(Image &source) {
    string msg = source.decrypt();
    if (msg == "") {
        cout << "No message found" << endl;
    } else {
        cout << "Message Found: " << msg << endl;
    }
}

void Handler::black_white(Image &source) {
    source.black_white();
    cout << "image: " << source.name << " is converted to pencil sketch successfully" << endl;
}