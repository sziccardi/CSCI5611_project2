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

    float x = sin(theta) * cos(phi);
    float z = -sin(theta) * sin(phi);
    float y = cos(theta);

    cameraFront.x = x;
    cameraFront.y = y;
    cameraFront.z = z;

    cameraFront = normalize(cameraFront);// maybe not necessary

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
        
        //printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
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
    for (int y = 0; y < mNumRows; y++) {
        for (int x = 0; x < mNumCols; x++) {
            if (x < mNumCols - 1 && y < mNumRows - 1) {
                indices.push_back(y * mNumCols + x);
                indices.push_back(y * mNumCols + x + 1);
                indices.push_back((y + 1) * mNumCols + x);
                indices.push_back((y + 1) * mNumCols + x);
                indices.push_back((y + 1) * mNumCols + x + 1);
                indices.push_back(y * mNumCols + x + 1);
            }

            glm::vec3 pos = glm::vec3(x * mSpringSize - mNumCols*mSpringSize/2.f, mStartHeight, y * mSpringSize - mNumRows * mSpringSize/2.f);
            glm::vec3 norm = glm::vec3(0.f, 1.f, 0.f);
            glm::vec2 tex = glm::vec2(1.f / mNumCols * x, 1.f / mNumRows * y);
            glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
            verts.push_back(Vertex(pos, norm, tex, vel, acc));
        }
    }

    mCloth = new Mesh2D(verts, indices, mClothTexture);
}

void initBackgroundVerts() {
    if (mBackground) {
        auto c = mBackground;
        delete(c);
        mBackground = nullptr;
    }

    vector<Vertex> verts;
    glm::vec3 pos = glm::vec3(-mBackWidth / 2, -mBackHeight / 2, 100.f);
    glm::vec3 norm = glm::vec3(0.f, 0.f, -1.f);
    glm::vec2 tex = glm::vec2(0.f, 1.f);
    glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(mBackWidth / 2, -mBackHeight / 2, 100.f);
    norm = glm::vec3(0.f, 0.f, -1.f);
    tex = glm::vec2(1.f, 1.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(mBackWidth / 2, mBackHeight / 2, 100.f);
    norm = glm::vec3(0.f, 0.f, -1.f);
    tex = glm::vec2(1.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));

    pos = glm::vec3(-mBackWidth / 2, mBackHeight / 2, 100.f);
    norm = glm::vec3(0.f, 0.f, -1.f);
    tex = glm::vec2(0.f, 0.f);
    vel = glm::vec3(0.f, 0.f, 0.f);
    acc = glm::vec3(0.f, 0.f, 0.f);
    verts.push_back(Vertex(pos, norm, tex, vel, acc));


    vector<unsigned int> indices;
    indices = { 1, 0, 2, 2, 3, 0 };

    mBackground = new Mesh2D(verts, indices, mBackgroundTexture);
}

void initSphereVerts() {
    if (mSphere) {
        auto c = mSphere;
        delete(c);
        mSphere = nullptr;
    }

    vector<Vertex> verts;
    vector<unsigned int> indices;

    float dTheta = 2 * M_PI / mSphereNumVertSlices;
    float dPhi = M_PI / mSphereNumHorizSlices;
    for (int i = 0; i < mSphereNumVertSlices; i++) {
        for (int j = 0; j < mSphereNumHorizSlices; j++) {
            float x = mSphereR * sin(dTheta * i) * cos(dPhi * j);
            float z = -mSphereR * sin(dTheta * i) * sin(dPhi * j);
            float y = mSphereR * cos(dTheta * i);

            if (i < mSphereNumVertSlices - 1 && j < mSphereNumHorizSlices - 1) {
                indices.push_back(j * mSphereNumVertSlices + i);
                indices.push_back(j * mSphereNumVertSlices + i + 1);
                indices.push_back((j + 1) * mSphereNumVertSlices + i);
                indices.push_back((j + 1) * mSphereNumVertSlices + i);
                indices.push_back((j + 1) * mSphereNumVertSlices + i + 1);
                indices.push_back(j * mSphereNumVertSlices + i + 1);
            }

            glm::vec3 pos = glm::vec3(x, y, z);
            glm::vec3 norm = glm::vec3(0.f, 1.f, 0.f);
            glm::vec2 tex = glm::vec2(1.f / mNumCols * i, 1.f / mNumRows * j);
            glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
            verts.push_back(Vertex(pos, norm, tex, vel, acc));

        }
    }

    mSphere = new Mesh2D(verts, indices, mSphereTexture);
}

void updateVerts() {
    //Reset accelerations each timestep (momentum only applies to velocity)
    for (int i = 0; i < mNumRows; i++) {
        for (int j = 0; j < mNumCols; j++) {
            auto myVert = mCloth->getVertAt(i * mNumCols + j);
            myVert.mAcceleration = mGravity;//glm::vec3(0.f, 0.f, 0.f);
            //drag
            int iterDiffI = 1;
            int iterDiffJ = 1;
            if (i == mNumRows - 1) iterDiffI = -1;
            if (j == mNumCols - 1) iterDiffJ = -1;
            auto nextIVert = mCloth->getVertAt((i + iterDiffI) * mNumCols + j);
            auto nextJVert = mCloth->getVertAt(i * mNumCols + j + iterDiffJ);
            glm::vec3 averageVel = (nextIVert.mVelocity + nextJVert.mVelocity + myVert.mVelocity) / 3.f;
            glm::vec3 normal = normalize(cross(nextJVert.mPosition - myVert.mPosition, nextIVert.mPosition - myVert.mPosition));
            float area = 0.5f * length(cross((nextJVert.mPosition - myVert.mPosition), (nextIVert.mPosition - myVert.mPosition)));
            if (length(averageVel) == 0.f) area *= 0.f;
            else {
                area *= (dot(averageVel, normal) / length(averageVel));
            }
            glm::vec3 drag = normal * (1.f / 6.f) * mKf * length(averageVel) * length(averageVel) * area;
            myVert.mAcceleration -= drag;
            mCloth->setVertAt(i * mNumCols + j, myVert);
        }
    }

    //Compute (damped) Hooke's law for each spring
    for (int i = 0; i < mNumRows; i++) {
        for (int j = 0; j < mNumCols; j++) {
            auto myVert = mCloth->getVertAt(i * mNumCols + j);
            
            if (i != mNumRows - 1) {
                auto nextVert = mCloth->getVertAt((i + 1) * mNumCols + j);
                glm::vec3 diff = nextVert.mPosition - myVert.mPosition;
                float diffLength = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                float stringF = -mK * (diffLength - mSpringSize);
                //println(stringF,diff.length(),restLen);

                glm::vec3 stringDir = normalize(diff);
                float projVbot = dot(myVert.mVelocity, stringDir);
                float projVtop = dot(nextVert.mVelocity, stringDir);
                float dampF = -mKv * (projVtop - projVbot);

                //Vec2 frictionF = vel[i].times(-kf);

                glm::vec3 force = stringDir*(stringF + dampF);
                //force.add(frictionF);
                myVert.mAcceleration += (force*(-1.0f / mMass));
                nextVert.mAcceleration += (force*(1.f / mMass));


                mCloth->setVertAt(i * mNumCols + j, myVert); //mine
                mCloth->setVertAt((i + 1) * mNumCols + j, nextVert); //down
            }

            if (j != mNumCols - 1) {
                auto nextVert = mCloth->getVertAt(i * mNumCols + j + 1);
                glm::vec3 diff = nextVert.mPosition - myVert.mPosition;
                float diffLength = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                float stringF = -mK * (diffLength - mSpringSize);
                //println(stringF,diff.length(),restLen);

                glm::vec3 stringDir = normalize(diff);
                float projVbot = dot(myVert.mVelocity, stringDir);
                float projVtop = dot(nextVert.mVelocity, stringDir);
                float dampF = -mKv * (projVtop - projVbot);

                //Vec2 frictionF = vel[i].times(-kf);

                glm::vec3 force = stringDir * (stringF + dampF);
                //force.add(frictionF);
                myVert.mAcceleration += (force * (-1.0f / mMass));
                nextVert.mAcceleration += (force * (1.f / mMass));


                mCloth->setVertAt(i * mNumCols + j, myVert); //mine
                mCloth->setVertAt(i * mNumCols + j + 1, nextVert); //right
            }
        }
    }

    //Eulerian integration
    for (int i = 0; i < mNumRows; i++) {
        for (int j = 0; j < mNumCols; j++) {
            auto myVert = mCloth->getVertAt(i * mNumCols + j);

            //auto prevPos = myVert.mPosition

            float posLength = sqrt(myVert.mPosition.x * myVert.mPosition.x + myVert.mPosition.y * myVert.mPosition.y + myVert.mPosition.z * myVert.mPosition.z);
            if (posLength != 0.f && posLength < mSphereR * 1.1f) {
                /*bool log = false;
                if (i == (int)mNumRows / 2 && j == (int)mNumCols / 2) {
                    cout << "i = " << i << " j = " << j << endl << "orig pos dist to center: " << posLength << endl;
                    log = true;
                }*/
                //if (log) cout << "orig pos: " << myVert.mPosition.x << ", " << myVert.mPosition.y << ", " << myVert.mPosition.z << endl;
                glm::vec3 normal = myVert.mPosition / posLength;
                //if (log) cout << "normal: " << normal.x << ", " << normal.y << ", " << normal.z << endl;
                float myDot = myVert.mVelocity.x * normal.x + myVert.mVelocity.y * normal.y + myVert.mVelocity.z * normal.z;
                glm::vec3 bounce = normal * myDot;
                //if (log) cout << "bounce: " << bounce.x << ", " << bounce.y << ", " << bounce.z << endl;

                //if (log) cout << "orig vel: " << myVert.mVelocity.x << ", " << myVert.mVelocity.y << ", " << myVert.mVelocity.z << endl;
                myVert.mVelocity = glm::vec3(0.f, 0.f, 0.f);//myVert.mVelocity - bounce * (1.f + mSphereBounceScale);
                //if (log) cout << "new vel: " << myVert.mVelocity.x << ", " << myVert.mVelocity.y << ", " << myVert.mVelocity.z << endl;
                //myVert.mPosition = myVert.mPosition + (normal * 1.01f);
                //if (log) cout << "new pos: " << myVert.mPosition.x << ", " << myVert.mPosition.y << ", " << myVert.mPosition.z << endl;
                posLength = sqrt(myVert.mPosition.x * myVert.mPosition.x + myVert.mPosition.y * myVert.mPosition.y + myVert.mPosition.z * myVert.mPosition.z);
                //if (log) cout << "new pos dist to center: " << posLength << endl << endl;

                mCloth->setVertAt(i * mNumCols + j, myVert);
            } else {

                myVert.mVelocity += myVert.mAcceleration * deltaTime;
                myVert.mPosition += myVert.mVelocity * deltaTime;

                //drag
                int iterDiffI = 1;
                int iterDiffJ = 1;
                if (i == mNumRows - 1) iterDiffI = -1;
                if (j == mNumCols - 1) iterDiffJ = -1;
                auto nextIVert = mCloth->getVertAt((i + iterDiffI) * mNumCols + j);
                auto nextJVert = mCloth->getVertAt(i * mNumCols + j + iterDiffJ);
                
                myVert.mNormal = normalize(cross(nextJVert.mPosition - myVert.mPosition, nextIVert.mPosition - myVert.mPosition));
                
            }
           /* if (i == 5 && j == 5) {
                cout << "Vel: ";
                cout << "(" << myVert.mVelocity.x << ", " << myVert.mVelocity.y << ", " << myVert.mVelocity.z << endl;
                cout << "Pos: ";
                cout << "(" << prevPos.x << ", " << prevPos.y << ", " << prevPos.z << ") - (";
                cout << myVert.mPosition.x << ", " << myVert.mPosition.y << ", " << myVert.mPosition.z << ")" << endl;
            }*/
            mCloth->setVertAt(i * mNumCols + j, myVert);
        }
    }
    //for (int i = 0; i < mNumRows; i++) {
    //    for (int j = 0; j < mNumCols; j++) {
    //        auto myVert = mCloth->getVertAt(i * mNumCols + j);
    //        float posLength = sqrt(myVert.mPosition.x * myVert.mPosition.x + myVert.mPosition.y * myVert.mPosition.y + myVert.mPosition.z * myVert.mPosition.z);
    //        if (posLength != 0.f && posLength < mSphereR * 1.01f) {
    //            /*bool log = false;
    //            if (i == (int)mNumRows / 2 && j == (int)mNumCols / 2) {
    //                cout << "i = " << i << " j = " << j << endl << "orig pos dist to center: " << posLength << endl;
    //                log = true;
    //            }*/
    //            //if (log) cout << "orig pos: " << myVert.mPosition.x << ", " << myVert.mPosition.y << ", " << myVert.mPosition.z << endl;
    //            glm::vec3 normal = myVert.mPosition / posLength;
    //            //if (log) cout << "normal: " << normal.x << ", " << normal.y << ", " << normal.z << endl;
    //            float myDot = myVert.mVelocity.x * normal.x + myVert.mVelocity.y * normal.y + myVert.mVelocity.z * normal.z;
    //            glm::vec3 bounce = normal * myDot;
    //            //if (log) cout << "bounce: " << bounce.x << ", " << bounce.y << ", " << bounce.z << endl;

    //            //if (log) cout << "orig vel: " << myVert.mVelocity.x << ", " << myVert.mVelocity.y << ", " << myVert.mVelocity.z << endl;
    //            myVert.mVelocity = glm::vec3(0.f, 0.f, 0.f);//myVert.mVelocity - bounce * (1.f + mSphereBounceScale);
    //            //if (log) cout << "new vel: " << myVert.mVelocity.x << ", " << myVert.mVelocity.y << ", " << myVert.mVelocity.z << endl;
    //            myVert.mPosition = myVert.mPosition + (normal * 1.01f);
    //            //if (log) cout << "new pos: " << myVert.mPosition.x << ", " << myVert.mPosition.y << ", " << myVert.mPosition.z << endl;
    //            posLength = sqrt(myVert.mPosition.x * myVert.mPosition.x + myVert.mPosition.y * myVert.mPosition.y + myVert.mPosition.z * myVert.mPosition.z);
    //            //if (log) cout << "new pos dist to center: " << posLength << endl << endl;

    //            mCloth->setVertAt(i * mNumCols + j, myVert);
    //        }

    //    }
    //}

}

void display() {
    glm::vec3 lookAt = cameraFront + cameraPos;

    // render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glutSolidSphere(mSphereR, 50, 50);
    glPopMatrix();

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
        if (mSphere) totalVerts += mSphere->getNumVerts();
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

    updateVerts();

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
    initClothVerts();
    initSphereVerts();
    initBackgroundVerts(); 
    
    /*interactions stuff*/
    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses  
    glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events
    //glutPassiveMotionFunc(mouseMovement);
   // glutMotionFunc(mouseMovement);
    //glutSetCursor(GLUT_CURSOR_NONE);
    glutMouseFunc(mouse);

    //start the animation after 5 seconds as a buffer
    glutTimerFunc(5, animLoop, 1);

    //all done!
    glCheckError();
    glutMainLoop();

    return 0;
}