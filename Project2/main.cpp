#include "main.h"

void initVerts() {
    if (mCloth) {
        auto c = mCloth;
        delete(c);
        mCloth = nullptr;
    }

    vector<Vertex> verts;
    vector<unsigned int> indices;
    for (int i = 0; i < mNumRows; i++) {
        for (int j = 0; j < mNumCols; j++) {
            if (j < mNumCols - 1 && i < mNumRows) {
                indices.push_back(i * mNumCols + j);
                indices.push_back(i * mNumCols + j + 1);
                indices.push_back((i + 1) * mNumCols + j);
                indices.push_back((i + 1) * mNumCols + j);
                indices.push_back((i + 1) * mNumCols + j + 1);
                indices.push_back(i * mNumCols + j + 1);
            }

            glm::vec3 pos = glm::vec3(j * mSpringSize, 0.f, i * mSpringSize);
            glm::vec3 norm = normalize(cameraPos - pos);
            glm::vec2 tex = glm::vec2(1.f / mNumCols * j, 1.f / mNumRows * i);
            glm::vec3 vel = glm::vec3(0.f, 0.f, 0.f);
            glm::vec3 acc = glm::vec3(0.f, 0.f, 0.f);
            verts.push_back(Vertex(pos, norm, tex, vel, acc));

        }
    }

    mCloth = new Mesh2D(verts, indices, mClothTexture);
}

void display() {
    glm::vec3 lookAt = cameraFront + cameraPos;

    // render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glutSwapBuffers();

    //FPS calculations
    framesSinceLast++;
    long deltaTime = glutGet(GLUT_ELAPSED_TIME);

    if (deltaTime - lastTimeSecond > 1000) {
        int fps = (int)(framesSinceLast * 1000.0 / (deltaTime - lastTimeSecond));
        lastTimeSecond = deltaTime;
        framesSinceLast = 0;
        cout << "FPS: " << fps;
        if (mCloth) cout << "\t Vertices: " << mCloth->getNumVerts();
        cout << endl;
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
    framesSinceLast += 1;

    //TODO: update vertices

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
    initVerts();

    //start the animation after 5 seconds as a buffer
    glutTimerFunc(5, animLoop, 1);

    //all done!
    glCheckError();
    glutMainLoop();

    return 0;
}