#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <string>

class Point {
    public:
        double x;
        double y;
        double z;

        Point() : x(0.0), y(0.0), z(1.0) {};

        Point(double x, double y, double z = 1.0) : x(x), y(y), z(z) {};

        void display() const {
        std::cout << "Point(" << x << ", " << y << ", " << z << ")\n";
        }

};

class ViewBox {
    public:
        std::ofstream svg;

        ViewBox(const std::string& name){
            svg.open(name + ".svg");
            svg << "<svg viewBox='-600 -600 1200 1200' xmlns='http://www.w3.org/2000/svg' fill='white' stroke='blue'>\n";
            svg << "<text x='-50' y='-530' font-size='16' fill='black'>\n";
            svg << "I love SVG\n";
            svg << "<animate attributeName='font-size' from='16' to='30' dur='5s' repeatCount='indefinite' />\n";
            svg << "<animate attributeName='fill' values='black; red; blue; green; black' dur='5s' repeatCount='indefinite' />\n";
            svg << "</text>\n";
            svg << "<text x='"<<-500<<"' y='"<<0<<"' stroke='blue' >("<<-500<<", "<<0<<") </text>\n";
            svg << "<text x='"<<500<<"' y='"<<0<<"' stroke='blue' >("<<500<<", "<<0<<") </text>\n";
            svg << "<text x='"<<0<<"' y='"<<500<<"' stroke='blue' >("<<0<<", "<<500<<") </text>\n";
            svg << "<text x='"<<0<<"' y='"<<-500<<"' stroke='blue' >("<<0<<", "<<-500<<") </text>\n";

            this->drawStraightLine(0, -500, 0, 500);
            this->drawStraightLine(-500, 0, 500, 0);
        };

        void drawPoints(const std::vector<Point>& points) {
              for(const Point& p: points) {
                svg<< "<circle cx='"<<p.x<<"' cy='"<<p.y<<"' r='5' fill='green'/>\n";
                svg << "<text x='"<<p.x<<"' y='"<<p.y<<"' stroke='blue' >("<<p.x<<", "<<p.y<<") </text>\n";
            }          
        }

        void drawStraightLine(auto x1, auto y1, auto x2, auto y2) {
            svg<< "<line x1='"<<x1<<"' y1='"<<y1<<"' x2='"<<x2<<"' y2='"<<y2<<"' stroke='red' stroke-width='2' />\n";
        }

        void drawShape(const std::vector<Point>& points, const std::string& stroke) {
            this->drawPoints(points);
            //TO DO i fix them so that polygon will be in chronoligcal order||
            //will focus on traingle in the mean time
            if (points.size() > 3) {
                std::cout <<"polygon";
                svg <<"<polygon points='";
                for(const Point& p: points) {
                    svg<<p.x<<" ,"<<p.y<<" ";
                }
                svg<< "' stroke='"<<stroke<<"' fill='none' stroke-width='3'>";
                svg <<"</polygon>\n";
               
            }
            else {
                svg<< "<path d='M"<<points[0].x<<" " <<points[0].y<<" ";
                for(const Point& p: points) {
                    svg<< "L"<<p.x<<" "<<p.y<<" ";
                }
                
                svg<<"Z'  stroke='"<<stroke<<"' fill='none' stroke-width='3'/>"; 
            } 
        }

        void closeTag(){
             svg<< "</svg>\n";
        }

        ~ViewBox(){
            if(svg.is_open()) {
                svg.close();
            }
        }
};

std::vector<Point> translation(const std::vector<Point>& points, double tx = 0.0, double ty = 0.0) {
    std::vector<Point> translatedPoints;
    for (const Point& p : points) {
        Point translatedPoint(p.x + tx, p.y + ty); 
        translatedPoints.push_back(translatedPoint);
    }
    return translatedPoints; 
}
std::vector<Point> scale(const std::vector<Point>& points, double tx = 0.0, double ty = 0.0) {
    std::vector<Point> scaledPoints;
    for (const Point& p : points) {
        Point scaledPoint(p.x * tx, p.y * ty); 
        scaledPoints.push_back(scaledPoint); 
    }
    return scaledPoints; 
}

auto toRadians(auto angle) {
    return angle * M_PI / 180.0;
}

std::vector<Point> rotate(const std::vector<Point>& points, double angle) {
    std::vector<Point> rotatedPoints;
    for (const Point& p : points) {
        Point rotatedPoint; 
        rotatedPoint.x = (p.x * cos(angle) - (p.y * sin(angle)));
        rotatedPoint.y = (p.x * sin(angle) + (p.y * cos(angle)));
        rotatedPoints.push_back(rotatedPoint); 
    }
    return rotatedPoints; 
}

std::vector<Point> rotateOnPoint(const std::vector<Point>& points, double angle, double tx = 0.0, double ty = 0.0) {
    std::vector<Point> rotatedPoints;
    std::vector<Point> translatedPoints;

    if (tx != 0.0 || ty != 0.0){
        //translate to the center
        translatedPoints = translation(points, -tx, -ty);
        rotatedPoints = rotate(translatedPoints, angle);

        //translate back to the center
        translatedPoints = translation(rotatedPoints, tx, ty);
    } else {
        translatedPoints = rotate(points, angle);
    }

    return translatedPoints;
}


std::vector<Point> reflect(const std::vector<Point>& points, double tx = 1.0, double ty = 1.0) {
    std::vector<Point> reflectedPoints;
    for (const Point& p : points) {
        Point reflectedPoint(p.x * tx, p.y * ty); 
        reflectedPoints.push_back(reflectedPoint); 
    }
    return reflectedPoints; 
}

std::vector<Point> reflectOnLine(ViewBox& svg, const std::vector<Point>& points, double x1, double y1, double x2, double y2) {
    //draw the line of reflection 
    svg.drawStraightLine(x1, y1, x2, y2);

    //translate using any points
    std::vector<Point> reflectedPoints = translation(points, -x1, -y1);
    reflectedPoints[0].display();

    //find angle of rotation in degrees along x axis
    double angle = atan2(y2 - y1, x2 - x1);
    std::cout << "angle: " << angle << std::endl;

    //rotate back to x axis
    double alongx = angle * -1;
    reflectedPoints = rotate(reflectedPoints, -angle);
    reflectedPoints[0].display();

    //reflect on the x axis 
    reflectedPoints = reflect(reflectedPoints, 1, -1);
    reflectedPoints[0].display();

     //rotate back 
    reflectedPoints = rotate(reflectedPoints, angle);
    reflectedPoints[0].display();

    //translate back to original position
    reflectedPoints = translation(reflectedPoints, x1, y1);
    reflectedPoints[0].display();

    //we are done!!!!!!!!!!!
    return reflectedPoints;

}

std::vector<Point> shear(const std::vector<Point>& points, double tx = 0.0, double ty = 0.0) {
    std::vector<Point> shearedPoints;
    for (const Point& p : points) {
        Point shearedPoint(p.x + (tx * p.y), p.y + (ty * p.x)); 
        shearedPoints.push_back(shearedPoint); 
    }
    return shearedPoints; 
}


int main() {
    std::string shape = "shape";
    std::vector<Point> points;
    int choice;
    double x1, y1, x2, y2, x3, y3;
    char answer;


    std::cout <<"X axis range (-500, 500) , Y axis range(-500, 500)\n";
    std::cout <<"You can use this for testiing (100 300 200 200 300 300)\n";
    std::cout << "Enter the coordinates of the triangle (x1 y1 x2 y2 x3 y3): ";
    std::cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;

    std::cout <<"Enter the name of your svg :";
    std::cin >> shape;

    points.push_back({x1, y1});
    points.push_back({x2, y2});
    points.push_back({x3, y3});

    ViewBox viewbox(shape);
    viewbox.drawShape(points, "red"); 

    std::cout <<"ORIGINAL SHAPE RED\n";
    std::cout <<"Do you want to perform any transformation? (y/n)";
    std::cin >> answer;

    if (answer == 'y') {
        std::cout << "AFTER TRANSFORMATION SHAPE WILL BE GREEN\n";
        std::cout << "Choose a transformation:\n";
        std::cout << "1. --Translation\n";
        std::cout << "2. --Scaling\n";
        std::cout << "3. --Rotation\n";
        std::cout << "4. --Rotation around a point\n";
        std::cout << "5. --Reflection on a line\n";
        std::cout << "6. --Shear\n";
    
        std::cin >> choice;

        std::vector<Point> transformedPoints;

        switch (choice) {
            case 1: {
                double tx, ty;
                std::cout << "Enter translation (tx ty) eg(100 100): ";
                std::cin >> tx >> ty;
                transformedPoints = translation(points, tx, ty);
                break;
            }
            case 2: {
                double sx, sy;
                std::cout << "Enter scaling factors (sx sy) eg(3 3): ";
                std::cin >> sx >> sy;
                transformedPoints = scale(points, sx, sy);
                break;
            }
            case 3: {
                double angle;
                std::cout << "Enter rotation angle (degrees) eg(37): ";
                std::cin >> angle;
                transformedPoints = rotate(points, angle);
                break;
            }
            case 4: {
                double angle, px, py;
                std::cout << "Enter rotation angle (degrees) eg(37): ";
                std::cin >> angle;
                std::cout << "Enter  point (px  py) eg(100 300): ";
                std::cin >> px >> py;
                transformedPoints = rotateOnPoint(points, angle, px, py);
                break;
            }
            case 5: {
                double x1, y1, x2, y2;
                std::cout << "enter mirror line coordinates (x1 y1 x2 y2): ";
                std::cin >> x1 >> y1 >> x2 >> y2;
                transformedPoints = reflectOnLine(viewbox, points, x1, y1, x2, y2);
                break;
            }
            case 6: {
                double shX, shY;
                std::cout << "Enter shear factors or 0  (shX shY) eg(0 3) or (3 0): ";
                std::cin >> shX >> shY;
                transformedPoints = shear(points, shX, shY);
                break;
            }
            default:
                std::cerr << "wrong choice." << std::endl;
                return 1;
        }

        viewbox.drawShape(transformedPoints, "green"); 
    }

    std::cout <<"PLEASE CHECK YOUR :["<<shape<<".svg] in the files to see the transformation\n";
    viewbox.closeTag();
    return 0;
}
