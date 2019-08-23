#ifndef confetti_h
class confet {
public:
	void setup(ofColor c) {
		col = c;
		pos = ofVec2f(ofRandom(0, 1080), ofRandom(-10, -100));
		vel = ofVec2f(ofRandom(-100, 100), ofRandom(100, 300));
		rot = ofRandom(360);
		orgVelx = vel.x;
		size = ofRandom(20, 50);
	}
	void update() {
		pos += vel * ofGetLastFrameTime();
		rot += vel.y * ofGetLastFrameTime();
		// vel.x = orgVelx + sin(ofGetElapsedTimef()*vel.y) * (vel.y + vel.x);
		if (pos.y > 1920) pos.y = -30;
		if (pos.x > 1080 + 50) pos.x = -10;
		if (pos.x < -50) pos.x = 1090;
	}
	void draw() {
		ofSetColor(col);
		ofPushMatrix();
		ofTranslate(pos.x, pos.y);
		ofRotateDeg(rot);
		ofDrawRectangle(0, 0, size, size / 2);
		ofPopMatrix();
	}
private:
	ofVec2f pos;
	ofVec2f vel;
	float orgVelx;
	float rot;
	float size;
	ofColor col;
};

class confetti {
public:
	void setup(int num = 100) {
		vector<ofColor>col;
		col.push_back(ofColor(245, 66, 53)); // orange
		col.push_back(ofColor(239, 12, 147)); // pink
		col.push_back(ofColor(144, 35, 141)); // purple
		col.push_back(ofColor(254, 185, 24)); // yellow

		for (int i = 0; i < num; i++) {
			confet c;
			c.setup(col[i%col.size()]);
			confettis.push_back(c);
		}
	}
	void draw() {
		ofFill();
		for (int i = 0; i < confettis.size(); i++) {
			confettis[i].update();
			confettis[i].draw();
		}
	}
    
private:
	vector<confet>confettis;
};
#endif /* confettis_h */
