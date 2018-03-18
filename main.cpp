#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <stack>
#include <utility>

using namespace std;

#include "bmp.h"
#include "main.h"
#include "class/RGBTRIPLE.cpp"
#include "class/Image.cpp"
#include "class/Handler.cpp"

void error(string msg)
{
    cout << "Error: " << msg << endl;
}

int main_menu() {
    int choice;
    cout << CLEAR << BG_CYAN << RED;
    for(int i = 0; i < 80; i++) cout << "+"; cout << endl;
    cout << "Main Menu" << endl;
    cout << "1. Select Image" << endl;
    cout << "2. Copy Image" << endl;
    cout << "3. Exit" << endl;
    for(int i = 0; i < 80; i++) cout << "+"; cout << endl;
    cout << BG_YELLOW << "Enter Your Choice: ";
    cin >> choice;
    return choice;
}

int op_menu() {
    int choice;
    cout << CLEAR << BG_CYAN << RED;
    for(int i = 0; i < 80; i++) cout << "+"; cout << endl;
    cout << "Operation Menu" << endl;
    cout << "1. Width?" << endl;
    cout << "2. Height?" << endl;
    cout << "3. Scale" << endl;
    cout << "4. Horizontal Mirror" << endl;
    cout << "5. Vertical Mirror" << endl;
    cout << "6. Rotate Clockwise" << endl;
    cout << "7. Rotate Anti Clockwise" << endl;
    cout << "8. Crop" << endl;
    cout << "9. Blur" << endl;
    cout << "10. Stretch" << endl;
    cout << "11. Merge Horizontally" << endl;
    cout << "12. Merge Vertically" << endl;
    cout << "13. Fill Color" << endl;
    cout << "14. Search Image" << endl;
    cout << "15. Encryption Steganography" << endl;
    cout << "16. Decryption Steganography" << endl;
    cout << "17. Undo" << endl;
    cout << "18. Redo" << endl;
    cout << "19. Black & White" << endl;
    cout << "20. Close Image" << endl;
    for(int i = 0; i < 80; i++) cout << "+"; cout << endl;
    cout << BG_YELLOW << "Enter Your Choice: ";
    cin >> choice;
    return choice;
}

int main(int argc, char *argv[]) {
    Handler handle;
    while(1) {
        int choice = main_menu(), op_choice;
        bool done = false;
        switch (choice) {
            case 1: {
                string name;
                cout << "Enter Image Name: ";
                cin >> name;
                Image image("image/"+name+".bmp");
                done = false;
                sleep(1);
                while(!done) {
                    op_choice = op_menu();
                    try {
                        switch(op_choice) {
                            case 1: {
                                handle.getWidth(image);
                                break;
                            }
                            case 2: {
                                handle.getHeight(image);
                                break;
                            }
                            case 3: {
                                int grow, shrink;
                                cout << "Enter Grow Factor: "; cin >> grow;
                                cout << "Enter Shrink Factor: "; cin >> shrink;
                                handle.scale(image, grow, shrink);
                                break;
                            }
                            case 4: {
                                handle.horizontalMirror(image);
                                break;
                            }
                            case 5: {
                                handle.verticalMirror(image);
                                break;
                            }
                            case 6: {
                                handle.rotateClockWise(image);
                                break;
                            }
                            case 7: {
                                handle.rotateAntiClockWise(image);
                                break;
                            }
                            case 8: {
                                int x, y, width, height;
                                cout << "Enter Starting X-Coordinate: "; cin >> x;
                                cout << "Enter Starting Y-Coordinate: "; cin >> y;
                                cout << "Enter Width: "; cin >> width;
                                cout << "Enter Height: "; cin >> height;
                                handle.crop(image, x, y, width, height);
                                break;
                            }
                            case 9: {
                                handle.blur(image);
                                break;
                            }
                            case 10: {
                                int hs, hc, vs, vc;
                                cout << "Enter Horizontal Stretch Factor: "; cin >> hs;
                                cout << "Enter Horizontal Compress Factor: "; cin >> hc;
                                cout << "Enter Vertical Stretch Factor: "; cin >> vs;
                                cout << "Enter Vertical Compress Factor: "; cin >> vc;
                                handle.stretch(image, hs, hc, vs, vc);
                                break;
                            }
                            case 11: {
                                string m_name;
                                cout << "Enter Image Name to Merge Horizontally: ";
                                cin >> m_name;
                                Image image2("image/"+m_name+".bmp");
                                handle.mergeHorizontally(image, image2);
                                break;
                            }
                            case 12: {
                                string m_name;
                                cout << "Enter Image Name to Merge Verically: ";
                                cin >> m_name;
                                Image image2("image/"+m_name+".bmp");
                                handle.mergeVertically(image, image2);
                                break;
                            }
                            case 13: {
                                int x, y, red, green, blue;
                                cout << "Enter Starting X-Coordinate to Start Flooding: "; cin >> x;
                                cout << "Enter Starting Y-Coordinate to Start Flooding: "; cin >> y;
                                cout << "Enter Intensity of Red (0-255): "; cin >> red;
                                while(red > 255 || red < 0) {
                                    cout << "Please Enter Valid Intensity of Red (0-255): ";
                                    cin >> red;
                                }
                                cout << "Enter Intensity of Green (0-255): "; cin >> green;
                                while(green > 255 || green < 0) {
                                    cout << "Please Enter Valid Intensity of Green (0-255): ";
                                    cin >> green;
                                }
                                cout << "Enter Intensity of Blue (0-255): "; cin >> blue;
                                while(blue > 255 || blue < 0) {
                                    cout << "Please Enter Valid Intensity of Blue (0-255): ";
                                    cin >> blue;
                                }
                                handle.fillColor(image, x, y, RGBTRIPLE(red, green, blue));
                                break;
                            }
                            case 14: {
                                string f_name;
                                cout << "Enter Image Name to Find in the Selected Image: ";
                                cin >> f_name;
                                Image image2("image/"+f_name+".bmp");
                                handle.find(image, image2);
                                break;
                            }
                            case 15: {
                                char msg[10005];
                                cout << "Enter Message to Encrypt in Image: ";
                                scanf(" %[^\n]s", msg);
                                handle.hideMSG(image, msg);
                                break;
                            }
                            case 16: {
                                handle.showMSG(image);
                                break;
                            }
                            case 17: {
                                handle.undo(image);
                                break;
                            }
                            case 18: {
                                handle.redo(image);
                                break;
                            }
                            case 19: {
                                handle.black_white(image);
                                break;
                            }
                            case 20: {
                                done = true;
                                break;
                            }
                            default: {
                                throw("Please Enter Valid Choice");
                            }
                        }
                    }
                    catch(const char *msg) {
                        cout << "Exception Occured: " << msg << endl;
                    }
                    catch(string msg) {
                        cout << "Exception Occured: " << msg << endl;
                    }
                    sleep(3);
                }
                break;
            }
            case 2: {
                string s_name, d_name;
                cout << "Enter Source Image Name: ";
                cin >> s_name;
                cout << "Enter Destination Image Name: ";
                cin >> d_name;
                Image source("image/"+s_name+".bmp");
                Image destination("image/"+d_name+".bmp");
                handle.copy(destination, source);
                break;
            }
            case 3: {
                cout << "Thank you for using";
                return 0;
            }
            default: {
                cout << "Please Enter Valid Choice" << endl;
                break;
            }
        }
        sleep(3);
    }
    //  Image image("image/tiger.bmp");
    //  Image image2("image/newSmiley.bmp");
    //  Handler handle;
    //  handle.copy(image2, image);
    // handle.scale(image2, 128, 1);
    // handle.horizontalMirror(image2);
    // handle.verticalMirror(image2);
    // handle.rotateClockWise(image2);
    // handle.rotateAntiClockWise(image2);
    // handle.crop(image2, 51, 160, 200, 100);
    // handle.blur(image2);
    // handle.undo(image2);
    // handle.redo(image2);
    // handle.stretch(image2, 1, 2, 2, 1);
    // handle.mergeHorizontally(image2, image);
    // handle.mergeVertically(image2, image);
    //  handle.fillColor(image2, 155, 385, RGBTRIPLE(0x00, 0xff, 0x00));
    // handle.find(image, image2);
    // string msg = "Hello World!! Today i am going to encrypt you!!!";
    // handle.hideMSG(image2, msg);
    // handle.showMSG(image2);
    return 0;
}