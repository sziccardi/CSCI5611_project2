#include "main.h"

/*INPUTS*/
void keyPressed(unsigned char key, int x, int y) {
    keyStates[key] = true;
}

void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

void keyOperations(void) {
    float dCam = cameraSpeed * deltaTime;
    //int ww = glutGet(GLUT_WINDOW_WIDTH);
    //int wh = glutGet(GLUT_WINDOW_HEIGHT);

    //OutputDebugStringA(cameraFront.toString().c_str());

    float phi = atan2(-cameraFront.z, cameraFront.x);
    float theta = atan2(sqrt(cameraFront.z * cameraFront.z + cameraFront.x * cameraFront.x), cameraFront.y);
    float oldPhi = phi;
    float oldTheta = theta;


    if (keyStates['w']) { // If the 'a' key has been pressed  
        //turn up
        //in spherical, -- theta
        theta -= dCam;
    }

    if (keyStates['a']) { // If the 'a' key has been pressed  
        //turn left
        //in spherical, ++ phi
        phi += dCam;
    }

    if (keyStates['d']) { // If the 'a' key has been pressed  
        //turn right
        //in spherical, -- phi
        phi -= dCam;
    }

    if (keyStates['s']) { // If the 'a' key has been pressed  
        //turn down
        //in spherical, ++ theta
        //TODO: need a limit!?
        theta += dCam;
    }

    float xFront = sin(theta) * cos(phi);
    float zFront = -sin(theta) * sin(phi);
    float yFront = cos(theta);

    cameraFront.x = xFront;
    cameraFront.y = yFront;
    cameraFront.z = zFront;

    cameraFront = normalize(cameraFront);// maybe not necessary

    //up is phi + M_PI / 2
    float xUp = sin(theta - M_PI / 2) * cos(phi);
    float zUp = -sin(theta - M_PI / 2) * sin(phi);
    float yUp = cos(theta - M_PI / 2);

    cameraUp.x = xUp;
    cameraUp.y = yUp;
    cameraUp.z = zUp;

    cameraUp = normalize(cameraUp);// maybe not necessary

    //right is theta + M_PI / 2
    float xRight = sin(theta) * cos(phi + M_PI / 2);
    float zRight = -sin(theta) * sin(phi + M_PI / 2);
    float yRight = cos(theta);

    cameraRight.x = xRight;
    cameraRight.y = yRight;
    cameraRight.z = zRight;

    cameraRight = normalize(cameraRight);// maybe not necessary

    if (keyStates[' ']) {
        cameraPos += cameraFront * dCam * cameraSpeedScale;
    }

    //cout << cameraPos.toString() << endl;
}

void mouse(int button, int state, int x, int y) {
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if ((button == 3) || (button == 4)) { // It's a wheel event 
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
        printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
    }
    else {  // normal button event
        // Mouse click, spawn particles
        mMouseDown = (state == GLUT_DOWN);
        mGhostSpherePosDiff = glm::vec3(0.f, 0.f, 0.f);
        mMousePos = glm::vec2(-1.f, -1.f);
        //printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
    }
}

void drag(int x, int y) {
    if (mMouseDown) {
        auto screenPos = glm::vec2(x, y) - mMousePos;//glm::vec2(x / (float)glutGet(GLUT_WINDOW_WIDTH), y / (float)glutGet(GLUT_WINDOW_HEIGHT));
        if (!(mMousePos.x < 0 && mMousePos.y < 0)) {
            auto n = cameraFront;
            auto a = cameraRight;
            auto b = cameraUp;
            auto newPos = mGhostSpherePos + screenPos.x * a  + screenPos.y * b;
            glm::vec3 diff = mGhostSpherePos - newPos;
            if (length(diff) > mMaxDiff) {
                diff = normalize(diff) * mMaxDiff;
            }
            //cout << "diff : " << diff.x * 0.25f << ", " << diff.y * 0.25f << endl;
            mGhostSpherePosDiff = diff; 
            mGhostSpherePos += mGhostSpherePosDiff;
        }
        mMousePos += screenPos;
    } 
}

void initClothVerts() {
    if (mCloth) {
        auto c = mCloth;
        delete(c);
        mCloth = nullptr;
    }

    vector<Vertex> verts;
    vector<unsigned int> indices;
    for (int y = 0; y < mClothNumRows; y++) {
        for (int x = 0; x < mClothNumCols; x++) {
            if (x < mClothNumCols - 1 && y < mClothNumRows - 1) {
                indices.push_back(y * mClothNumCols + x);
                indices.push_back(y * mClothNumCols + x + 1);
                indices.push_back((y + 1) * mClothNumCols + x);
                indices.push_back((y + 1) * mClothNumCols + x);
                indices.push_back((y + 1) * mClothNumCols + x + 1);
                indices.push_back(y * mClothNumCols + x + 1);
            }

            glm::vec3 pos = glm::vec3(x * mClothSpringSize - mClothNumCols*mClothSpringSize/2.f, mClothStartHeight, y * mClothSpringSize - mClothNumRows * mClothSpringSize/2.f);
            glm::vec3 norm = glm::vec3(0.f, 1.f, 0.f);
            glm::vec2 tex = glm::vec2(1.f / mClothNumCols * x, 1.f / mClothNumRows * y);
            glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
            verts.push_back(Vertex(pos, norm, tex, vel, acc));
        }
    }

    mCloth = new Mesh2D(verts, indices, mClothTexture);
}

void initGhostSphereVerts() {
    if (mGhostSphere) {
        auto c = mGhostSphere;
        delete(c);
        mGhostSphere = nullptr;
    }

    vector<Vertex> verts;
    vector<unsigned int> indices;

    float dTheta = 2 * M_PI / mGhostSphereNumVertSlices;
    float dPhi = M_PI / mGhostSphereNumHorizSlices;
    for (int i = 0; i < mGhostSphereNumVertSlices; i++) {
        for (int j = 0; j < mGhostSphereNumHorizSlices; j++) {
            float x = mGhostSphereR * sin(dTheta * i) * cos(dPhi * j);
            float z = -mGhostSphereR * sin(dTheta * i) * sin(dPhi * j);
            float y = mGhostSphereR * cos(dTheta * i);

            if (i < mGhostSphereNumVertSlices - 1 && j < mGhostSphereNumHorizSlices - 1) {
                indices.push_back(j * mGhostSphereNumVertSlices + i);
                indices.push_back(j * mGhostSphereNumVertSlices + i + 1);
                indices.push_back((j + 1) * mGhostSphereNumVertSlices + i);
                indices.push_back((j + 1) * mGhostSphereNumVertSlices + i);
                indices.push_back((j + 1) * mGhostSphereNumVertSlices + i + 1);
                indices.push_back(j * mGhostSphereNumVertSlices + i + 1);
            }

            glm::vec3 pos = glm::vec3(x, y, z);
            glm::vec3 norm = glm::vec3(0.f, 1.f, 0.f);
            glm::vec2 tex = glm::vec2(1.f / mClothNumCols * i, 1.f / mClothNumRows * j);
            glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
            verts.push_back(Vertex(pos, norm, tex, vel, acc));

        }
    }

    mGhostSphere = new Mesh2D(verts, indices, mGhostSphereTexture);
}

void initBackgroundVerts() {
    if (mBackground) {
        auto c = mBackground;
        delete(c);
        mBackground = nullptr;
    }

    vector<Vertex> verts;
    glm::vec3 pos = glm::vec3(-mBackWidth / 2, -mBackHeight / 2, mBackHeight / 2);
    glm::vec3 norm = glm::vec3(0.f, 0.f, -1.f);
    glm::vec2 tex = glm::vec2(0.f, 1.f);
    glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(mBackWidth / 2, -mBackHeight / 2, mBackHeight / 2);
    norm = glm::vec3(0.f, 0.f, -1.f);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(mBackWidth / 2, mBackHeight / 2, mBackHeight / 2);
    norm = glm::vec3(0.f, 0.f, -1.f);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(-mBackWidth / 2, mBackHeight / 2, mBackHeight / 2);
    norm = glm::vec3(0.f, 0.f, -1.f);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(-mBackWidth / 2, -mBackHeight / 2, -mBackHeight / 2);
    norm = glm::vec3(0.f, 0.f, 1.f);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(mBackWidth / 2, -mBackHeight / 2, -mBackHeight / 2);
    norm = glm::vec3(0.f, 0.f, 1.f);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(mBackWidth / 2, mBackHeight / 2, -mBackHeight / 2);
    norm = glm::vec3(0.f, 0.f, 1.f);
    tex = glm::vec2(0.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(-mBackWidth / 2, mBackHeight / 2, -mBackHeight / 2);
    norm = glm::vec3(0.f, 0.f, 1.f);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));


    vector<unsigned int> indices;
    //              back                front
    indices = { 1, 0, 2, 2, 3, 0,  5, 4, 6, 6, 7, 4,  0, 1, 4, 4, 5, 1,  1, 2, 5, 5, 6, 2,  2, 3, 6, 6, 7, 3,  3, 0, 7, 7, 4, 0};

    mBackground = new Mesh2D(verts, indices, mBackgroundTexture);
}

void initJellyVerts() {
    if (mJelly) {
        auto c = mJelly;
        delete(c);
        mJelly = nullptr;
    }

    vector<Vertex> verts;
    for (int x = 0; x < mJellyNumSprings; x++) {
        for (int y = 0; y < mJellyNumSprings; y++) {
            for (int z = 0; z < mJellyNumSprings; z++) {
                glm::vec3 pos = glm::vec3(x * mJellySpringSize - mJellySpringSize / 2, y * mJellySpringSize - mJellySpringSize / 2, z * mJellySpringSize - mJellySpringSize / 2);
                glm::vec3 norm = glm::vec3(0.f, 1.f, 0.f);
                //only texture the edges
                //middle bits have tex coords (-1,-1)
                //based on this picture below (X's show where the box is 'unrolled' with texture tiled
                /*
                     ___ ___ ___ ___
                    |___|_5_|___|___|
                    |_1_|_2_|_3_|_4_|
                    |___|_6_|___|___|
                
                */
                glm::vec2 tex = glm::vec2(-1.f, -1.f);
                //(1)
                if (x == 0 && z != 0 && y != mJellyNumSprings && z != mJellyNumSprings) {
                    tex = glm::vec2(1.f / mJellyNumSprings * y, 1.f / mJellyNumSprings * z + 1.f);
                } 
                //(2)
                else if (y == 0 && z != 0 && x != mJellyNumSprings && z != mJellyNumSprings) {
                    tex = glm::vec2(1.f / mJellyNumSprings * x + 1.f, 1.f / mJellyNumSprings * z + 1.f);
                } 
                //(3)
                else if (x == mJellyNumSprings && y != mJellyNumSprings && z != mJellyNumSprings) {
                    tex = glm::vec2(1.f / mJellyNumSprings * y + 2.f, 1.f / mJellyNumSprings * z + 1.f);
                }
                //(4)
                else if (y == mJellyNumSprings && x != mJellyNumSprings) {
                    tex = glm::vec2(1.f / mJellyNumSprings * x + 3.f, 1.f / mJellyNumSprings * z + 1.f);
                }
                //(5)
                else if (z == 0 && y != mJellyNumSprings) {
                    tex = glm::vec2(1.f / mJellyNumSprings * x + 1.f, 1.f / mJellyNumSprings * y);
                } 
                //(6)
                else if (z == mJellyNumSprings && y != 0 && x != mJellyNumSprings) {
                    tex = glm::vec2(1.f / mJellyNumSprings * x + 1.f, 1.f / mJellyNumSprings * y + 2.f);
                }
                glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
                glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
                verts.push_back(Vertex(pos, norm, tex, vel, acc));
            }
        }
    }

    vector<unsigned int> indices;
    //left
    int z = 0;
    int y = 0;
    int x = 0;
    for (y = 0; y < mJellyNumSprings; y++) {
        for (z = 0; z < mJellyNumSprings; z++) {
            if (z < mJellyNumSprings - 1 && y < mJellyNumSprings - 1) {
                indices.push_back((y + 1) * mJellyNumSprings + z);
                indices.push_back((y + 1) * mJellyNumSprings + z + 1);
                indices.push_back(y * mJellyNumSprings + z);
                indices.push_back(y * mJellyNumSprings + z);
                indices.push_back(y * mJellyNumSprings + z + 1);
                indices.push_back((y + 1) * mJellyNumSprings + z + 1);
            }
        }
    }
    //front
    z = 0;
    y = 0;
    x = 0;
    for (x = 0; x < mJellyNumSprings; x++) {
        for (y = 0; y < mJellyNumSprings; y++) {
            if (x < mJellyNumSprings - 1 && y < mJellyNumSprings - 1) {
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + y * mJellyNumSprings + z);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + y * mJellyNumSprings);
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + (y + 1) * mJellyNumSprings);
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + (y + 1) * mJellyNumSprings);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + (y + 1) * mJellyNumSprings);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + y * mJellyNumSprings);
            }
        }
    }
    //right
    z = 0;
    y = 0;
    x = mJellyNumSprings;
    for (y = 0; y < mJellyNumSprings; y++) {
        for (z = 0; z < mJellyNumSprings; z++) {
            if (z < mJellyNumSprings - 1 && y < mJellyNumSprings - 1) {
                indices.push_back(y * mJellyNumSprings + z);
                indices.push_back(y * mJellyNumSprings + z + 1);
                indices.push_back((y + 1) * mJellyNumSprings + z);
                indices.push_back((y + 1) * mJellyNumSprings + z);
                indices.push_back((y + 1) * mJellyNumSprings + z + 1);
                indices.push_back(y * mJellyNumSprings + z + 1);
            }
        }
    }    
    //back
    z = mJellyNumSprings;
    y = 0;
    x = 0;
    for (x = 0; x < mJellyNumSprings; x++) {
        for (y = 0; y < mJellyNumSprings; y++) {
            if (x < mJellyNumSprings - 1 && y < mJellyNumSprings - 1) {
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + y * mJellyNumSprings + z);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + y * mJellyNumSprings);
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + (y + 1) * mJellyNumSprings);
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + (y + 1) * mJellyNumSprings);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + (y + 1) * mJellyNumSprings);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + y * mJellyNumSprings);
            }
        }
    }
    //top
    z = 0;
    y = mJellyNumSprings;
    x = 0;
    for (x = 0; x < mJellyNumSprings; x++) {
        for (z = 0; z < mJellyNumSprings; z++) {
            if (x < mJellyNumSprings - 1 && z < mJellyNumSprings - 1) {
                //(x, z + 1) -> (x + 1, z + 1) -> (x, z) -> (x, z) -> (x + 1, z) -> (x + 1, z + 1)

                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + z + 1);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + z + 1);
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + z);
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + z + 1);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + z);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + z + 1);
            }
        }
    }
    //bottom
    z = 0;
    y = 0;
    x = 0;
    for (x = 0; x < mJellyNumSprings; x++) {
        for (z = 0; z < mJellyNumSprings; z++) {
            if (x < mJellyNumSprings - 1 && z < mJellyNumSprings - 1) {
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + z);
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + z);
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + (z + 1));
                indices.push_back(x * mJellyNumSprings * mJellyNumSprings + (z + 1));
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + (z + 1));
                indices.push_back((x + 1) * mJellyNumSprings * mJellyNumSprings + z);
            }
        }
    }

    mJelly = new Mesh2D(verts, indices, mJellyTexture);
}

void initPumpkinVerts() {
    if (mPumpkin) {
        auto c = mPumpkin;
        delete(c);
        mPumpkin = nullptr;
    }

    vector<Vertex> verts;
    vector<unsigned int> indices;

    float dTheta = 2 * M_PI / mPumpkinNumVertSlices;
    float dPhi = M_PI / mPumpkinNumHorizSlices;
    for (int i = 0; i < mPumpkinNumVertSlices; i++) {
        for (int j = 0; j < mPumpkinNumHorizSlices; j++) {
            float x = mPumpkinR * sin(dTheta * i) * cos(dPhi * j);
            float z = -mPumpkinR * sin(dTheta * i) * sin(dPhi * j);
            float y = mPumpkinR * cos(dTheta * i);

            if (i < mPumpkinNumVertSlices - 1 && j < mPumpkinNumHorizSlices - 1) {
                indices.push_back(j * mPumpkinNumVertSlices + i);
                indices.push_back(j * mPumpkinNumVertSlices + i + 1);
                indices.push_back((j + 1) * mPumpkinNumVertSlices + i);
                indices.push_back((j + 1) * mPumpkinNumVertSlices + i);
                indices.push_back((j + 1) * mPumpkinNumVertSlices + i + 1);
                indices.push_back(j * mPumpkinNumVertSlices + i + 1);
            }

            glm::vec3 pos = glm::vec3(x, y, z) + mPumpkinPos;
            glm::vec3 norm = glm::vec3(0.f, 1.f, 0.f);
            glm::vec2 tex = glm::vec2(1.f / mClothNumCols * i, 1.f / mClothNumRows * j);
            glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
            verts.push_back(Vertex(pos, norm, tex, vel, acc));

        }
    }

    mPumpkin = new Mesh2D(verts, indices, mPumpkinTexture);
}


void updateJellyVerts() {
    for (int i = 0; i < mJellyNumSprings; i++) {
        for (int j = 0; j < mJellyNumSprings; j++) {
            for (int k = 0; k < mJellyNumSprings; k++) {
                auto myVert = (i * mJellyNumSprings * mJellyNumSprings + j * mJellyNumSprings + k);
                mJelly->getVertAt(myVert).mAcceleration = mGravity;
                
                int iterDiffI = 1;
                int iterDiffJ = 1;
                int iterDiffK = 1;
                if (i == mJellyNumSprings - 1) iterDiffI = -1;
                if (j == mJellyNumSprings - 1) iterDiffJ = -1;
                if (k == mJellyNumSprings - 1) iterDiffK = -1;
                auto nextIVert = ((i + iterDiffI) * mJellyNumSprings * mJellyNumSprings + j * mJellyNumSprings + k);
                auto nextJVert = (i * mJellyNumSprings * mJellyNumSprings + (j + iterDiffJ) * mJellyNumSprings + k);
                auto nextKVert = (i * mJellyNumSprings * mJellyNumSprings + j * mJellyNumSprings + k + iterDiffK);
                if (i != (mJellyNumSprings - 1)) {
                    updateAccJellyVerts(myVert, nextIVert, mJellySpringSize);
                }
                if (j != (mJellyNumSprings - 1)) {
                    updateAccJellyVerts(myVert, nextJVert, mJellySpringSize);
                }
                if (k != (mJellyNumSprings - 1)) {
                    updateAccJellyVerts(myVert, nextKVert, mJellySpringSize);
                }
            }
        }
    }

    float diagRestLen = sqrt(2) * restLen;
    for (int i = 0; i < mJellyNumSprings; i++) {
        for (int j = 0; j < mJellyNumSprings; j++) {
            updateAccJellyVerts(i * mJellyNumSprings * mJellyNumSprings + j, i * mJellyNumSprings * mJellyNumSprings + mJellyNumSprings + j ^ 1, diagRestLen);
            updateAccJellyVerts(i * mJellyNumSprings + j, mJellyNumSprings * mJellyNumSprings + i * mJellyNumSprings + j ^ 1, diagRestLen);
            updateAccJellyVerts(j * mJellyNumSprings + i, mJellyNumSprings * mJellyNumSprings + j ^ 1 * mJellyNumSprings + i, diagRestLen);
        }
    }

    //collision 
    auto myVert = (mJellyNumSprings * mJellyNumSprings * (mJellyNumSprings / 2) + mJellyNumSprings * mJellyNumSprings + (mJellyNumSprings / 2));
    if (mPumpkinPos.y < mJelly->getVertAt(myVert).mPosition.y) {
        glm::vec3 v1 = mJelly->getVertAt(1).mPosition - mJelly->getVertAt(0).mPosition;
        glm::vec3 v2 = mJelly->getVertAt(mJellyNumSprings * mJellyNumSprings).mPosition - mJelly->getVertAt(0).mPosition;
        glm::vec3 N = normalize(cross(v1, v2)); //normal
        glm::vec3 dx = (N * ((diffDist + 0.01f)));
        mPumpkinPos -= dx;
        glm::vec3 velProj = N*(dot(mPumpkin->getVertAt(0).mVelocity, N));
        glm::vec3 dv = (velProj * (1 + mPumpkinCOR));

        for (int theta = 0; theta < mPumpkinNumVertSlices; theta++) {
            for (int phi = 0; phi < mPumpkinNumHorizSlices; phi++) {
                auto theVert = mPumpkin->getVertAt(theta * mPumpkinNumHorizSlices + phi);
                theVert.mPosition -= dx;
                theVert.mVelocity -= dv;
            }
        }


        float fact = ((rand() % 10) / 20.f + 0.6) * COR2;
        for (int i = 0; i < mJellyNumSprings; i++) {
            for (int k = 0; k < mJellyNumSprings; k++) {
                auto theVert = mPumpkin->getVertAt(i * mJellyNumSprings * mJellyNumSprings + k);
                theVert.mVelocity += (velProj * (fact));
                //vel[i][1][k].add(velProj.times(COR2/2));
            }
        }
    }

    //mid-point integration
    for (int theta = 0; theta < mPumpkinNumVertSlices; theta++) {
        for (int phi = 0; phi < mPumpkinNumHorizSlices; phi++) {
            auto theVert = mPumpkin->getVertAt(theta * mPumpkinNumHorizSlices + phi);
            theVert.mVelocity += (mGravity * (deltaTime / 2));
            theVert.mPosition += (theVert.mVelocity * (deltaTime));
            theVert.mVelocity += (mGravity * (deltaTime / 2));
        }
    }

    

    mPumpkinAngle += mPumpkinAngle * deltaTime;

    for (int i = 0; i < mJellyNumSprings; i++) {
        for (int j = 0; j < mJellyNumSprings; j++) {
            for (int k = 0; k < mJellyNumSprings; k++) {
                auto myVert = mPumpkin->getVertAt(i * mJellyNumSprings * mJellyNumSprings + j * mJellyNumSprings + k);

                float tmp = myVert.mPosition.y;
                myVert.mVelocity += (myVert.mAcceleration * (deltaTime / 2));
                myVert.mPosition += (myVert.mVelocity * (deltaTime));
                myVert.mVelocity += (myVert.mAcceleration * (deltaTime / 2));
                if (j == (mJellyNumSprings - 1)) {
                    myVert.mPosition.y = min(myVert.mPosition.y, tmp);
                }
                if (i == 0 && j == 0 && k == 0) {
                    mMaxY = max(mMaxY, myVert.mPosition.y);
                    //println(maxY);
                }
            }
        }
    }
}

void updateAccJellyVerts(int origIter, int nextIter, float restLen) {
    auto myVert = mJelly->getVertAt(origIter);
    auto nextVert = mJelly->getVertAt(nextIter);

    glm::vec3 diff = nextVert.mPosition - myVert.mPosition;
    float stringF = -k * (diff.length() - restLen);

    glm::vec3 stringDir = normalize(diff);
    float projVbot = dot(myVert.mVelocity, stringDir);
    float projVtop = dot(nextVert.mVelocity, stringDir);
    float dampF = -kv * (projVtop - projVbot);

    glm::vec3 force = stringDir * (stringF + dampF);

    myVert.mAcceleration += (force * (-1.0f / mass));
    nextVert.mAcceleration += (force * (1.0f / mass));

    mJelly->setVertAt(origIter, myVert);
    mJelly->setVertAt(nextIter, nextVert);
}

void updateClothVerts() {
    //Reset accelerations each timestep (momentum only applies to velocity)
    for (int i = 0; i < mClothNumRows; i++) {
        for (int j = 0; j < mClothNumCols; j++) {
            auto myVert = mCloth->getVertAt(i * mClothNumCols + j);
            myVert.mAcceleration = mGravity;//glm::vec3(0.f, 0.f, 0.f);
            //drag
            int iterDiffI = 1;
            int iterDiffJ = 1;
            if (i == mClothNumRows - 1) iterDiffI = -1;
            if (j == mClothNumCols - 1) iterDiffJ = -1;
            auto nextIVert = mCloth->getVertAt((i + iterDiffI) * mClothNumCols + j);
            auto nextJVert = mCloth->getVertAt(i * mClothNumCols + j + iterDiffJ);
            glm::vec3 averageVel = (nextIVert.mVelocity + nextJVert.mVelocity + myVert.mVelocity) / 3.f;
            glm::vec3 normal = normalize(cross(nextJVert.mPosition - myVert.mPosition, nextIVert.mPosition - myVert.mPosition));
            float area = 0.5f * length(cross((nextJVert.mPosition - myVert.mPosition), (nextIVert.mPosition - myVert.mPosition)));
            if (length(averageVel) == 0.f) area *= 0.f;
            else {
                area *= (dot(averageVel, normal) / length(averageVel));
            }
            glm::vec3 drag = normal * (1.f / 6.f) * mClothKf * length(averageVel) * length(averageVel) * area;
            myVert.mAcceleration -= drag;
            
            /*auto diff = myVert.mPosition - mGhostSpherePos;
            float posLength = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
            if (posLength != 0.f && posLength < mGhostSphereR + 1.f && myVert.mPosition.y > mGhostSpherePos.y) {
                
                glm::vec3 magneticF = normalize(diff * -1.f) * (2.f / (posLength - mGhostSphereR - 1.f));
                myVert.mAcceleration += magneticF;
            }*/

            mCloth->setVertAt(i * mClothNumCols + j, myVert);
        }
    }

    //Compute (damped) Hooke's law for each spring
    for (int i = 0; i < mClothNumRows; i++) {
        for (int j = 0; j < mClothNumCols; j++) {
            auto myVert = mCloth->getVertAt(i * mClothNumCols + j);
            
            if (i != mClothNumRows - 1) {
                auto nextVert = mCloth->getVertAt((i + 1) * mClothNumCols + j);
                glm::vec3 diff = nextVert.mPosition - myVert.mPosition;
                float diffLength = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                float stringF = -mClothK * (diffLength - mClothSpringSize);
                //println(stringF,diff.length(),restLen);

                glm::vec3 stringDir = normalize(diff);
                float projVbot = dot(myVert.mVelocity, stringDir);
                float projVtop = dot(nextVert.mVelocity, stringDir);
                float dampF = -mClothKv * (projVtop - projVbot);

                //Vec2 frictionF = vel[i].times(-kf);

                glm::vec3 force = stringDir*(stringF + dampF);
                //force.add(frictionF);
                myVert.mAcceleration += (force*(-1.0f / mClothMass));
                nextVert.mAcceleration += (force*(1.f / mClothMass));


                mCloth->setVertAt(i * mClothNumCols + j, myVert); //mine
                mCloth->setVertAt((i + 1) * mClothNumCols + j, nextVert); //down
            }

            if (j != mClothNumCols - 1) {
                auto nextVert = mCloth->getVertAt(i * mClothNumCols + j + 1);
                glm::vec3 diff = nextVert.mPosition - myVert.mPosition;
                float diffLength = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                float stringF = -mClothK * (diffLength - mClothSpringSize);
                //println(stringF,diff.length(),restLen);

                glm::vec3 stringDir = normalize(diff);
                float projVbot = dot(myVert.mVelocity, stringDir);
                float projVtop = dot(nextVert.mVelocity, stringDir);
                float dampF = -mClothKv * (projVtop - projVbot);

                //Vec2 frictionF = vel[i].times(-kf);

                glm::vec3 force = stringDir * (stringF + dampF);
                //force.add(frictionF);
                myVert.mAcceleration += (force * (-1.0f / mClothMass));
                nextVert.mAcceleration += (force * (1.f / mClothMass));


                mCloth->setVertAt(i * mClothNumCols + j, myVert); //mine
                mCloth->setVertAt(i * mClothNumCols + j + 1, nextVert); //right
            }
        }
    }

    //integration
    for (int i = 0; i < mClothNumRows; i++) {
        for (int j = 0; j < mClothNumCols; j++) {
            auto myVert = mCloth->getVertAt(i * mClothNumCols + j);

            int iterDiffI = 1;
            int iterDiffJ = 1;
            if (i == mClothNumRows - 1) iterDiffI = -1;
            if (j == mClothNumCols - 1) iterDiffJ = -1;
            auto nextIVert = mCloth->getVertAt((i + iterDiffI) * mClothNumCols + j);
            auto nextJVert = mCloth->getVertAt(i * mClothNumCols + j + iterDiffJ);

            //Midpoint
            /*auto dV = myVert.mAcceleration * deltaTime / 2.f;
            myVert.mVelocity += dV;
            auto dX = myVert.mVelocity * deltaTime;
            myVert.mPosition += dX; 
            dV = myVert.mAcceleration * deltaTime / 2.f;
            myVert.mVelocity += dV;*/

            //Eulerian
            auto dV = myVert.mAcceleration * deltaTime;
            myVert.mVelocity += dV;
            auto dX = myVert.mVelocity * deltaTime;
            myVert.mPosition += dX;

            myVert.mNormal = normalize(cross(nextJVert.mPosition - myVert.mPosition, nextIVert.mPosition - myVert.mPosition));

            auto diff = myVert.mPosition - mGhostSpherePos;
            float posLength = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
            if (posLength != 0.f && posLength < mGhostSphereR && myVert.mPosition.y > mGhostSpherePos.y) {
                glm::vec3 normal = diff / posLength;
                
                float myDot = myVert.mVelocity.x * normal.x + myVert.mVelocity.y * normal.y + myVert.mVelocity.z * normal.z;
                glm::vec3 bounce = normal * myDot;
                //pin the top if its on the ball so the cloth doesnt fall off
                if (i < (int)mClothNumRows / 2 + 2 && j < (int)mClothNumCols / 2 + 2 && 
                    i > (int)mClothNumRows / 2 - 2 && j > (int)mClothNumCols / 2 - 2) {
                    myVert.mVelocity -= dV;
                    myVert.mPosition -= dX;
                    //myVert.mVelocity.y = 0.f;
                } 
                
                if (!(i < (int)mClothNumRows / 2 + 2 && j < (int)mClothNumCols / 2 + 2 &&
                    i >(int)mClothNumRows / 2 - 2 && j >(int)mClothNumCols / 2 - 2)
                    || (mGhostSpherePosDiff.x != 0 && mGhostSpherePosDiff.y != 0)) {
                    myVert.mVelocity = myVert.mVelocity - bounce * (1.f + mGhostSphereBounceScale);
                    myVert.mPosition = normal * mGhostSphereR * 1.0000001f + mGhostSpherePos;
                }
                
                   myVert.mPosition += mGhostSpherePosDiff;
               
            } 

            mCloth->setVertAt(i * mClothNumCols + j, myVert);
        }
    }
}

void display() {
    glm::vec3 lookAt = cameraFront + cameraPos;

    // render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glutSolidSphere(mSphereR, 50, 50);
    glPopMatrix();*/

    glUseProgram(mShaderProgram);

    glLoadIdentity();
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, lookAt.x, lookAt.y, lookAt.z, cameraUp.x, cameraUp.y, cameraUp.z);

    // view/projection/model transformations
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, cameraDepth);
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    glm::vec3 newCameraPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z);
    glm::vec3 newCameraFront = glm::vec3(cameraFront.x, cameraFront.y, cameraFront.z);
    glm::vec3 newCameraUp = glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z);
    glm::mat4 view = glm::lookAt(newCameraPos, newCameraPos + newCameraFront, newCameraUp);
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

    glm::mat4 model = glm::mat4(1.0f); //Use if we need to rotate/scale/translate the mesh as a whole
    glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    if (mCloth) mCloth->draw();
    //if (mSphere) mSphere->draw();
    if (mBackground) mBackground->draw();
    //if (mPumpkin) mPumpkin->draw();
    //if (mJelly) mJelly->draw();
    
    glutSwapBuffers();

    //FPS calculations
    framesSinceLast++;
    long deltaTime = glutGet(GLUT_ELAPSED_TIME);

    if (deltaTime - lastTimeSecond > 1000) {
        int fps = (int)(framesSinceLast * 1000.0 / (deltaTime - lastTimeSecond));
        lastTimeSecond = deltaTime;
        framesSinceLast = 0;
        cout << "FPS: " << fps;
        int totalVerts = 0;
        cout << "\t Vertices: ";
        if (mCloth) totalVerts += mCloth->getNumVerts();
        if (mGhostSphere) totalVerts += mGhostSphere->getNumVerts();
        cout << totalVerts << endl;
    }
}

void initGL() {
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(908, 640);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("5611 Project 2");
    glutDisplayFunc(display); // declaring the "draw" function
    glutReshapeFunc(reshape);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glCheckError();    
}

void initShader() {
    // compile shaders!
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glCheckError();
    glShaderSource(vertex, 1, &vertexShaderSource, NULL);
    glCheckError();
    glCompileShader(vertex);
    glCheckError();
    glCheckError();
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glCheckError();
    glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
    glCheckError();
    glCompileShader(fragment);
    glCheckError();

    // shader Program
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertex);
    glAttachShader(mShaderProgram, fragment);

    glLinkProgram(mShaderProgram);
    glCheckError();
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void reshape(GLsizei width, GLsizei height) {
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix

    glCheckError();
    glLoadIdentity();             // Reset
    glCheckError();
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, cameraDepth);
    glCheckError();
}

void animLoop(int val) {
    keyOperations();

    framesSinceLast += 1;
    
    for (int i = 0; i < 4; i++) {
        updateClothVerts();
        //updateJellyVerts();
    }

    glutPostRedisplay();
    glutTimerFunc(16, animLoop, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    //init the lot
    glewExperimental = GL_TRUE;
    initGL();
    glewInit();
    initShader();
    
    //initPumpkinVerts();
    //initJellyVerts();
    initClothVerts();
    initGhostSphereVerts();
    initBackgroundVerts(); 
    
    /*interactions stuff*/
    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
    glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events
    glutMouseFunc(mouse);
    glutMotionFunc(drag);

    //start the animation after 5 milliseconds as a buffer
    glutTimerFunc(5, animLoop, 1);

    //all done!
    glCheckError();
    glutMainLoop();

    return 0;
}