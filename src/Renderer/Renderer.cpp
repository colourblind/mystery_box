#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/TriMesh.h"

#include <vector>
#include <fstream>
#include <sstream>

using namespace cinder;
using namespace cinder::app;
using namespace std;

class BoxDrawer : public AppBasic
{
public:
    virtual void setup();
    virtual void draw();
    virtual void update();

private:
    Timer timer_;
    TriMesh mesh_;
    float rot_;
};

void BoxDrawer::setup()
{
    ifstream file("out.txt", ios::in);

    float x = 0, y = 0, z = 0;
    char buffer[256];
    file.getline(buffer, 255);
    size_t offset;
    float half_size = 0.25f / 2;
    while (!file.eof())
    {
        memset(buffer, 0, 256);
		file.getline(buffer, 255);
		istringstream line(buffer);
		line >> x >> y >> z;

        size_t offset = mesh_.getNumVertices();

        mesh_.appendVertex(Vec3f(x - half_size, y - half_size, z - half_size));
        mesh_.appendVertex(Vec3f(x + half_size, y - half_size, z - half_size));
        mesh_.appendVertex(Vec3f(x - half_size, y + half_size, z - half_size));
        mesh_.appendVertex(Vec3f(x + half_size, y + half_size, z - half_size));
        mesh_.appendVertex(Vec3f(x - half_size, y - half_size, z + half_size));
        mesh_.appendVertex(Vec3f(x + half_size, y - half_size, z + half_size));
        mesh_.appendVertex(Vec3f(x - half_size, y + half_size, z + half_size));
        mesh_.appendVertex(Vec3f(x + half_size, y + half_size, z + half_size));

        mesh_.appendTriangle(offset + 0, offset + 2, offset + 3); // front 0
        mesh_.appendTriangle(offset + 0, offset + 3, offset + 1); // front 1
        mesh_.appendTriangle(offset + 4, offset + 7, offset + 6); // back 0
        mesh_.appendTriangle(offset + 4, offset + 5, offset + 7); // back 1
        
        mesh_.appendTriangle(offset + 0, offset + 6, offset + 2); // left 0
        mesh_.appendTriangle(offset + 0, offset + 4, offset + 6); // left 1
        mesh_.appendTriangle(offset + 1, offset + 3, offset + 7); // right 0
        mesh_.appendTriangle(offset + 1, offset + 7, offset + 5); // right 1

        mesh_.appendTriangle(offset + 0, offset + 1, offset + 5); // top 0
        mesh_.appendTriangle(offset + 0, offset + 5, offset + 4); // top 1
        mesh_.appendTriangle(offset + 2, offset + 7, offset + 3); // bottom 0
        mesh_.appendTriangle(offset + 2, offset + 6, offset + 7); // bottom 1
    }
    
    file.close();

    rot_ = 0;

    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::enable(GL_CULL_FACE);
}

void BoxDrawer::update()
{
    timer_.stop();
    float msecs = 1000.0f * static_cast<float>(timer_.getSeconds());
    timer_.start();

    rot_ += msecs * 0.004;
}

void BoxDrawer::draw()
{
    gl::clear(Color(0, 0, 0.5f));

    CameraPersp persp = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 1, 25);
    persp.lookAt(Vec3f(0, 0, -18), Vec3f::zero());
    //persp.lookAt(Vec3f(0, 0, -0.5), cameraPos);
    gl::setMatrices(persp);

    gl::rotate(Vec3f(0, rot_, 0));
    gl::draw(mesh_);
}

CINDER_APP_BASIC(BoxDrawer, RendererGl)
