#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetCircleResolution(3);
    
    depth = 50; //テキストの奥行き
    ro = 0; //回転率
    bRotation = true; //テキストの回転
    renderingMode = false; 
    ofEnableDepthTest(); //深度テストを有効に
   font.loadFont("mona.ttf", 36, true, true, 0.3f, 0, true); //フォント名、フォントサイズ
    font.setLineHeight(font.getFontSize()*1.4); //行の高さ
    ofBuffer buffer = ofBufferFromFile("aa.txt"); //テキストファイルの読み込み
    typeStr = buffer.getText();
    letterPaths = font.getStringAsPoints(typeStr);
    bbox = font.getStringBoundingBox(typeStr, 0, 0);
    
    for (int i = 0; i < letterPaths.size(); i++) {
        ofVboMesh front = letterPaths[i].getTessellation();
        ofVboMesh back = front;
        auto v = back.getVerticesPointer();
        for (int j = 0; j < (int)back.getNumVertices(); j++) {
            v[j].z += depth; //テキストの奥行き
            
            front.addNormal(ofVec3f(0, 0, -1));
            back.addNormal(ofVec3f(0, 0, 1));
        }
        textMesh.push_back(front);
        textMesh.push_back(back);
    }
    
    for (int i = 0; i < (int)letterPaths.size(); i++) {
        lines = letterPaths[i].getOutline();
        for (int j = 0; j < (int)lines.size(); j++) {
            if( j!= 0)ofNextContour(true) ;
            vector<glm::vec3> points = lines[j].getVertices();
            int k = 0;
            for (k = 0; k < (int)points.size()-1; k++) {
                ofPoint p1 = points[k+0];
                ofPoint p2 = points[k+1];
                
                side.addVertex(p1);
                side.addVertex(p2);
                side.addVertex(ofPoint(p1.x, p1.y, p1.z+depth));
                
                side.addVertex(ofPoint(p1.x, p1.y, p1.z+depth));
                side.addVertex(p2);
                side.addVertex(ofPoint(p2.x, p2.y, p2.z+depth));
                 
            }
            // Connect the last to the first
            ofPoint p1 = points[k];
            ofPoint p2 = points[0];
            
            side.addVertex(p1);
            side.addVertex(p2);
            side.addVertex(ofPoint(p1.x, p1.y, p1.z+depth));
            
            side.addVertex(ofPoint(p1.x, p1.y, p1.z+depth));
            side.addVertex(p2);
            side.addVertex(ofPoint(p2.x, p2.y, p2.z+depth));
        }
    }
    side.setupIndicesAuto();
    setNormals(side);
    textMesh.push_back(side);
    
    myFbo.allocate(ofGetWidth(), ofGetHeight());
    myGlitch.setup(&myFbo);
}

//--------------------------------------------------------------
void ofApp::update(){
    myFbo.begin();
    
    ofClear(0, 0);
    
    ofBackground(166, 217, 226); //水色
    cam.begin(); //カメラ開始
    ofPushMatrix();
    ofRotateDeg(180, 1, 0, 0);
     if(bRotation){
    ofRotateDeg(ofGetFrameNum(), 0, 1.0, 0);
     }
    ofTranslate(-bbox.width/2+kx, -bbox.height/2+ky);

    
    if(renderingMode){
        for (int i = 0; i < textMesh.size(); i++) {
            
            if        (i % 5 == 0)ofSetColor(250, 210, 219);
            else if (i % 9 == 0)ofSetColor(5, 165, 210);
            else                ofSetColor(242, 231, 61);
             
            textMesh[i].draw();
        }
    } else {
     
        for (int i = 0; i < textMesh.size(); i++) {
            
            if        (i % 5 == 0)ofSetColor(250, 210, 219);
            else if (i % 9 == 0)ofSetColor(5, 165, 210);
            else                ofSetColor(242, 231, 61);
             
            ofSetLineWidth(3);
            textMesh[i].drawWireframe(); //ワイアーフレーム表示
        }
    }
    ofPopMatrix();
    cam.end(); //カメラ終了
    myFbo.end();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
/* draw effected view */
    ofSetColor(255);
        /* Apply effects */
    myGlitch.generateFx();
    myFbo.draw(0, 0);
    

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'r': //回転
        case 'R':
            bRotation =!bRotation;
            break;
        case 'e':
        case 'E':
            ro = 0; //回転リセット
            break;
        case 'w':
        case 'W':
            renderingMode =!renderingMode;
            break;
        case OF_KEY_DOWN:
            ky += 10;
            break;
        case OF_KEY_UP:
            ky -= 10;
            break;
        case OF_KEY_RIGHT:
            kx += 10;
            break;
        case OF_KEY_LEFT:
            kx -= 10;
            break;
        case 's': //take a screenshot
        case 'S':
            myImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
            myImage.save(ofGetTimestampString("%Y%m%d%H%M%S")+"##.png");
            break;
    }

    if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , true);
    if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW            , true);
    if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER            , true);
    if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , true);
    if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST            , true);
    if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , true);
    if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE            , true);
    if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , true);
    if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL            , true);
    if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT            , true);

    if (key == 'z') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
    if (key == 'x') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE    , true);
    if (key == 'c') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE    , true);
    if (key == 'v') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE    , true);
    if (key == 'b') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT    , true);
    if (key == 'n') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT    , true);
    if (key == 'm') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT    , true);
}
//--------------------------------------------------------------
void ofApp::setNormals(ofMesh &mesh){
    //The number of the vertices
    int nV = (int)mesh.getNumVertices();
    
    //The number of the triangles
    int nT = (int)mesh.getNumIndices()/3;
    
    vector<glm::vec3> norm(nV); //Array for the normals
    
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t = 0; t < nT; t++) {
        //Get indices of the triangle t
        int i1 = mesh.getIndex(3*t);
        int i2 = mesh.getIndex(3*t+1);
        int i3 = mesh.getIndex(3*t+2);
        
        //Get vertices of the triangle
        const glm::vec3 &v1 = mesh.getVertex(i1);
        const glm::vec3 &v2 = mesh.getVertex(i2);
        const glm::vec3 &v3 = mesh.getVertex(i3);
        
        //Compute the triangle's normal
        glm::vec3 dir = glm::normalize(glm::cross(v2-v1, v3-v1));
        
        //Accumulate it to norm array for i1, i2, i3
        norm[i1] += dir;
        norm[i2] += dir;
        norm[i3] += dir;
    }
    
    //Normalixe the normal's length
    for (int i = 0; i < nV; i++) {
        norm[i] = glm::normalize(norm[i]);
    }
    
    //Set the normals to mesh
    mesh.clearNormals();
    mesh.addNormals(norm);
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , false);
    if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW            , false);
    if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER            , false);
    if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , false);
    if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST            , false);
    if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , false);
    if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE            , false);
    if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , false);
    if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL            , false);
    if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT            , false);

    if (key == 'z') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
    if (key == 'x') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE    , false);
    if (key == 'c') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE    , false);
    if (key == 'v') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE    , false);
    if (key == 'b') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT    , false);
    if (key == 'n') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT    , false);
    if (key == 'm') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT    , false);
    

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
