#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    r.setup(7000);
    
	confettis.setup();

    test.listDevices();
	test.setDeviceID(2);

    test.setup(VIDEO_W, VIDEO_H);
    
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    
    yellowShader.load("shader");
	
	ofDisableArbTex();
	learnedPoses.allocate(WIDTH, HEIGHT, GL_RGBA);
	learnedPoses.begin();
	ofClear(0);
	learnedPoses.end();
	ofEnableArbTex();

	render.allocate(WIDTH, HEIGHT, GL_RGBA);
	render.begin();
	ofClear(0);
	render.end();

    for (int i = 0; i<25; i++) {
        vector<ofVec2f>newAverage;
        newAverage.resize(18);
        averagePoses.push_back(newAverage);
		
		ofDisableArbTex();
        ofImage img;
		
		ofSetColor(255);
		if (img.load("outputs\\" + ofToString(i) + ".png"))
		{
			/*cout << "image loaded" << endl;
			
			learnedPoses.begin();
			ofClear(0);
			yellowShader.begin();
			yellowShader.setUniformTexture("tex", img.getTexture(), 0);
			yellowShader.setUniform2f("res", img.getWidth(), img.getHeight());
			img.draw(0, 0, img.getWidth(), img.getHeight());
			yellowShader.end();
			learnedPoses.end();

			ofPixels pix;
			learnedPoses.getTexture().readToPixels(pix);

			ofSaveImage(pix, "outputs\\" + ofToString(i) + ".png");
			
			img.load("outputs\\" + ofToString(i) + ".png");
			*/
			learnedPoses.begin();
			ofClear(0);
			yellowShader.begin();
			yellowShader.setUniformTexture("tex0", img.getTexture(), 0);
			yellowShader.setUniform2f("res", WIDTH, HEIGHT);
			img.draw(0, 0, WIDTH, HEIGHT);
			yellowShader.end();
			learnedPoses.end();

			ofPixels pix;
			learnedPoses.getTexture().readToPixels(pix);
			ofSaveImage(pix, "outputs\\" + ofToString(i) + ".png");
			img.load("outputs\\" + ofToString(i) + ".png");
			poseImages.push_back(img);
		}
    }
	ofEnableArbTex();

    logo.load("logo.png");
    backgound.load("background.png");
    speech.load("speech.png");
    speech_red.load("speech_idle.png");
    
    
    feedback.setup();
    
    box2d.init();
    box2d.setGravity(0, 0.0);
    box2d.setFPS(30.0);
    box2d.setIterations(10, 2);
    box2d.disableEvents();
    box2d.disableGrabbing();
    
    theUsers.resize(MAX_USERS);
    for (int i = 0; i < MAX_USERS; i++) {
        theUsers[i].setup(&box2d);
    }
    bartender.setup(&box2d);
	bartender.setupAnimation("animation/hurra.txt", "hurra");
	bartender.setupAnimation("animation/hurra3.txt", "celebrate");
	bartender.setupAnimation("animation/ohno.txt", "ohno");
	bartender.setupAnimation("animation/idle2.txt", "idle");
    
    classifier.setup();
    

    chainevent.addEvent(3., BEGIN_LEARNING);
    chainevent.addEvent(3., LEARNING);
    chainevent.addEvent(20., TRAINING, true);
    chainevent.addEvent(3., PLAYING);
	chainevent.addEvent(1., PREPARE, true);
    chainevent.addEvent(3., POSE1);
    chainevent.addEvent(3., POSE2);
    chainevent.addEvent(3., POSE3);
    chainevent.addEvent(2., POUR);
	chainevent.addEvent(5., CELEBRATE);
	chainevent.addEvent(3., RESET);
	chainevent.addEvent(2., TRYAGAIN);
	// chainevent.addEvent(3., TOOMANY, true);
	chainevent.addEvent(3., NOONE, true);
    
    
    chainevent.setTo(PLAYING);
    

    
    gui.setup();
    gui.add(top.set("user_top", 0, 0, 1000));
    gui.add(yellow_box.set("yellow_box", 0, 0, 500));
    gui.add(red_box.set("yellow_red", 0, 0, 800));
    gui.add(bellyThreshold.set("bellyThreshold", 0, 0, 1));
    gui.add(prababilityThreshold.set("prababilityThreshold", 0, 0, 1));
    gui.add(addSamples.set("addsamples", false));
    gui.add(clearSample.set("clearSample", false));
    gui.add(train.set("train", false));
	gui.add(animationSpeed.set("animationSpeed", 1, 1, 30));
    
    bodyGroup.setName("body");
    
    
    bodyGroup.add(torso.set("torso", ofVec2f(0), ofVec2f(-10), ofVec2f(10)));
    bodyGroup.add(head.set("head", ofVec2f(0), ofVec2f(-10), ofVec2f(10)));
    bodyGroup.add(s_torso.set("s_torso", 0, 0, 1));
    bodyGroup.add(s_head.set("s_head", 0, 0, 1));
    
    for(int i = 0; i<9; i++){
        ofParameter<ofVec2f> p;
        pos_parts.push_back(p);
        bodyGroup.add(pos_parts.back().set("part_"+ofToString(i), ofVec2f(0,0), ofVec2f(-10,-10), ofVec2f(10,10)));
        
        ofParameter<float> s;
        s_parts.push_back(s);
        bodyGroup.add(s_parts.back().set("s_part_"+ofToString(i), 0, 0, 1));
    }
    
    gui.add(bodyGroup);
    
    gui.loadFromFile("settings.xml");
    
    
    drinkSequence.clear();
    drinkSequence = {1, 2, 3};
    
    ofDirectory dir;
    dir.allowExt("png");
    dir.listDir("Bartender");
    dir.sort();
    
    for (int i = 0; i<dir.size(); i++ ){
        ofImage img;
        img.load(dir.getPath(i));
        bodyPartImages.push_back(img);
        cout << dir.getName(i) << endl;
    }
    dir.allowExt("png");
    dir.listDir("User");
    dir.sort();
    for (int i = 0; i<dir.size(); i++ ){
        ofImage img;
        img.load(dir.getPath(i));
        bodyPartImages.push_back(img);
        cout << dir.getName(i) << endl;
    }
    chainevent.setTo(NOONE);


}


//--------------------------------------------------------------
void ofApp::update() {
    
    /*
     OFXOSC_TYPE_INT32            = 'i',
     OFXOSC_TYPE_INT64            = 'h',
     OFXOSC_TYPE_FLOAT            = 'f',
     OFXOSC_TYPE_DOUBLE           = 'd',
     OFXOSC_TYPE_STRING           = 's',
     OFXOSC_TYPE_SYMBOL           = 'S',
     OFXOSC_TYPE_CHAR             = 'c',
     OFXOSC_TYPE_MIDI_MESSAGE     = 'm',
     OFXOSC_TYPE_TRUE             = 'T',
     OFXOSC_TYPE_FALSE            = 'F',
     OFXOSC_TYPE_NONE             = 'N',
     OFXOSC_TYPE_TRIGGER          = 'I',
     OFXOSC_TYPE_TIMETAG          = 't',
     OFXOSC_TYPE_BLOB             = 'b',
     OFXOSC_TYPE_RGBA_COLOR       = 'r',
     */
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    isFrameNew = false;
	
	while (r.hasWaitingMessages()) {
		ofxOscMessage m;
		r.getNextMessage(m);
		// vector<int>updatedUsers;

		int id = ofToInt(ofSplitString(m.getAddress(), "person").back());
		//if (id > 0)cout << m.getAddress() << endl;

		if (id<theUsers.size()) {
			float centerX = 0.f;
			float centerY = 0.f;
			int xTotal = 0;
			int yTotal = 0;
			vector<ofVec2f>tempPoints;
			int indx = 0;
			for (int i = 0; i < m.getNumArgs(); i += 3) {
				if (m.getArgType(i) == 102) {
					float x = 1. - (m.getArgAsFloat(i) / SCALE_X);
					float y = m.getArgAsFloat(i + 1) / SCALE_Y;
					if (x > 0 && y>0) {
						centerX += x;
						xTotal++;
						centerY += y;
						yTotal++;
					}
					tempPoints.push_back(ofVec2f(x, y));
					theUsers[id].addPoint(indx, x, y);
					indx++;
				}
			}
			centerX /= xTotal;
			centerY /= yTotal;
			theUsers[id].centerPoint = ofVec2f(centerX, centerY);
			/*float minDist = 10000000;
			int user = -1;
			for (int i = 0; i < theUsers.size(); i++) {
				float dist = ofVec2f(centerX, centerY).distance(theUsers[i].centerPoint);
				//bool isUpdated = false;
				//for (int u = 0; u < updatedUsers.size(); u++)
				//	if (updatedUsers[u] == i) isUpdated = true;
				if (dist < minDist) {
					user = i;
					minDist = dist;
					//cout << "theuser us" << i << endl;
				}
			}
			// cout << user << " s user" << endl;
			theUsers[user].isRemoved = 0.0;
			theUsers[user].centerPoint = ofVec2f(centerX, centerY);
			theUsers[user].points = tempPoints;
			//updatedUsers.push_back(user);
			*/
		}

	}
    
	// numHumansInView = updatedUsers.size();
	// cout << numHumansInView << endl;
    userInView = -1;
	numHumansInView = 0;

	ofVec2f center = ofVec2f(0.5, 0.5);
	float minDist = 10000000;
	
	for (int i = 0; i < theUsers.size(); i++) {
		theUsers[i].update();
		if (theUsers[i].isRemoved < 1.1) numHumansInView++;
		if (numHumansInView > 0) {

			float dist = center.distance(theUsers[i].centerPoint);
			if (dist < minDist) {
				minDist = dist;
				userInView = i;
			}
		}
	}


	if (numHumansInView==0) {
		chainevent.setTo(NOONE);
		// cout << "no one here? " << numHumansInView << " total " << numHumans << endl;
	}
	

    feedback.update();
    
    
    
    int messageX = WIDTH/2;
    int messageY = yellow_box + 155;
    int messageRedX = WIDTH/2;
    int messageRedY = red_box + 100;
    int line1 = 75;
    int line2 = 55;
    
   
    chainevent.update();

    switch (chainevent.getName()) {
        case BEGIN_LEARNING: {
            
            if (chainevent.getDuration()-chainevent.getTime() < 0.1) {
                chainevent.next();
            }
            
            break;
        }
        case LEARNING: {
            user u = theUsers[userInView];
            GRT::VectorFloat sample = getSample(u);
            
            classifier.addSample(sample, numPoses);
            std::transform (averagePoses[numPoses].begin(), averagePoses[numPoses].end(), u.circlePoints.begin(), averagePoses[numPoses].begin(), std::plus<ofVec2f>());
            numSamples ++;

            if (chainevent.getTime()>chainevent.getDuration() - 0.1) {
                
                std::transform(averagePoses[numPoses].begin(), averagePoses[numPoses].end(), averagePoses[numPoses].begin(),
                               std::bind1st(std::multiplies<ofVec2f>(),1.0/double(numSamples+1)));
                
                learnedPoses.begin();
                ofClear(0);
                ofFill();
                ofSetColor(255);
                drawUserWithPngs(averagePoses[numPoses], 0);
                learnedPoses.end();
                
                ofPixels pix;
                learnedPoses.readToPixels(pix);
                ofSaveImage(pix, "outputs\\" + ofToString(numPoses) + ".png");
				ofDisableArbTex();
				ofImage img;
				img.load("outputs\\" + ofToString(numPoses) + ".png");
				ofEnableArbTex;

				learnedPoses.begin();
				ofClear(0);
				yellowShader.begin();
				yellowShader.setUniformTexture("tex0", img.getTexture(), 0);
				yellowShader.setUniform2f("res", WIDTH, HEIGHT);
				img.draw(0, 0, WIDTH, HEIGHT);
				yellowShader.end();
				learnedPoses.end();

				pix.clear();
				learnedPoses.readToPixels(pix);
				ofSaveImage(pix, "outputs\\" + ofToString(numPoses) + ".png");

				ofImage newImg;
				newImg.load("outputs\\" + ofToString(numPoses) + ".png");
                if (numPoses >= poseImages.size())
                    poseImages.push_back(newImg);
                else poseImages[numPoses] = newImg;
                
                numSamples = 0;
                chainevent.next();
                
            }
            break;
        }
            
            
        case TRAINING: {
            classifier.train();
            chainevent.next();
            break;   
        }
        case PLAYING: {
            if(chainevent.isfirstframe){
				feedback.removeDrawable(5, 0.01);
				feedback.removeDrawable(1, 0.2);

				feedback.removeDrawable(7, 0.1); // poses in bottom
				feedback.removeDrawable(8, 0.2); // poses in bottom
				feedback.removeDrawable(9, 0.3); // poses in bottom

				feedback.addImage(&speech_red, WIDTH / 2, yellow_box, 763, 389, 1, 0.2);
				feedback.addImage(&bodyPartImages[8], WIDTH / 2 + (763 / 2 - 20), red_box, 210*0.7, 318*0.7, 1);
                //feedback.addImage(&speech, WIDTH/2, yellow_box, 763, 389, 1);
				// feedback.addImage(&bodyPartImages[8], yellow_box, 863, 50, (1240 / 810) * 50, 1);
                feedback.addText("¡Get ready!", messageX, messageY, 2, true, 1., ofColor(34,38,76), true);
                feedback.addText("Hit these three swag poses", messageX, messageY + line1, 2, false, 1., ofColor(34,38,76));
                feedback.addText("in time to win a drink!", messageX, messageY + line1 + line2, 2, false, 1., ofColor(34,38,76));
                chainevent.isfirstframe = false;

				for (int i = 0; i < 3; i++) {
					drawable d;
					feedback.addImage(&poseImages[drinkSequence[i]], (WIDTH / 3 * i) + WIDTH / 6, HEIGHT - HEIGHT / 3, WIDTH / 3, HEIGHT / 3, 7 + i, 0.3, .5*i);

				}
					//feedback.addImage(&poseImages[drinkSequence[i]], (WIDTH / 3 * i) + WIDTH / 6, HEIGHT - HEIGHT/3, WIDTH / 3, HEIGHT / 3, 7+i, 0.5*i+0.3);

				cout << "sat to start" << endl;
            }
			//unsigned int sec = chainevent.getDuration() - chainevent.getTime();

			/*else if (chainevent.getTime() > 2 * chainevent.getDuration() / 4)feedback.changeText(2, "¡Set!");
			else if(chainevent.getTime() > chainevent.getDuration()  / 4)feedback.changeText(2, "¡Ready!");
			*/
			highScoreCounter = 0.0;
			//cout << "¡Preparar - " + ofToString(chainevent.getDuration() - chainevent.getTime(), 0) + "!" << endl;
            break;
        }
		case PREPARE: {
			highScoreCounter = 0.0;
			if (chainevent.isfirstframe) {
				feedback.removeDrawable(5, 0.01);
				feedback.removeDrawable(2, 0.2);

				feedback.removeDrawable(7, 0.1); // poses in bottom
				feedback.removeDrawable(8, 0.2); // poses in bottom
				feedback.removeDrawable(9, 0.3); // poses in bottom

				//feedback.addImage(&speech_red, WIDTH / 2, yellow_box, 763, 389, 1, 0.2);
				//feedback.addImage(&bodyPartImages[8], WIDTH / 2 + (763 / 2 - 20), red_box, 210 * 0.7, 318 * 0.7, 1);
				//feedback.addImage(&speech, WIDTH/2, yellow_box, 763, 389, 1);
				// feedback.addImage(&bodyPartImages[8], yellow_box, 863, 50, (1240 / 810) * 50, 1);
				feedback.addText("Ready?", messageX, messageY, 2, true, 1., ofColor(34, 38, 76), true);
				feedback.addText("Raise your arms", messageX, messageY + line1, 2, false, 1., ofColor(34, 38, 76));
				feedback.addText("to begin!", messageX, messageY + line1 + line2, 2, false, 1., ofColor(34, 38, 76));
				chainevent.isfirstframe = false;
				feedback.addImage(&poseImages[0], WIDTH / 2, top, WIDTH, HEIGHT, 5, 0.1, 0.0, ofColor(200), 200);

				//feedback.addImage(&poseImages[drinkSequence[i]], (WIDTH / 3 * i) + WIDTH / 6, HEIGHT - HEIGHT/3, WIDTH / 3, HEIGHT / 3, 7+i, 0.5*i+0.3);

				// cout << "sat to start" << endl;
			}

			user u = theUsers[userInView];
			GRT::VectorFloat sample = getSample(u);
			//cout << sample.size() << endl;
			classifier.updateSample(sample);
			int currentPose = classifier.label;

			if ((currentPose == 0 && classifier.probability > prababilityThreshold) || jumpToNext) {
				cout << "raised arm" << endl;
				chainevent.next();
			}
			break;

		}
        case POSE1: {
			

            if(chainevent.isfirstframe){
				// feedback.setMaxAlpha(7, 255);
                //feedback.addImage(speech,WIDTH/2 - 763/2, yellow_box, 763, 389, 1);
                feedback.removeDrawable(2, 0.5);
				feedback.removeDrawable(5, 0.01);

				feedback.removeDrawable(7, 0.1); // poses in bottom
				feedback.removeDrawable(8, 0.2); // poses in bottom
				feedback.removeDrawable(9, 0.3); // poses in bottom
				//feedback.removeDrawable(1, 0.2);
				//feedback.addImage(&speech_red, WIDTH / 2, yellow_box, 763, 389, 1);
				//feedback.addImage(&bodyPartImages[8], WIDTH/2 + (763/2-105), red_box, 210, 318, 1);

				feedback.addImage(&poseImages[drinkSequence[0]], WIDTH / 2, top, WIDTH, HEIGHT, 5, 0.1);
                feedback.addText("¡Plantear Uno!", messageX, messageY, 2, true, 1., ofColor(34,38,76));
                feedback.addText("Flamingo, Flamingo", messageX, messageY + line1, 2, false, 1., ofColor(34,38,76));
                //feedback.addText("open the drink valve", messageX, messageY + 60 + 40, 2, false, 1., ofColor(34,38,76));
                chainevent.isfirstframe = false;
            }
			if(chainevent.getTime()>0.5)pose(userInView, 0);
			highScoreCounter += ofGetLastFrameTime();
			unsigned int sec = chainevent.getDuration() - chainevent.getTime();
			//feedback.changeText(2, "¡Plantear Uno - " + ofToString(sec) + "!");

            break;
        }
    
        case POSE2: {
			

            if(chainevent.isfirstframe){
				// feedback.setMaxAlpha(8, 255);
				// feedback.setMaxAlpha(7, 255, ofColor(255));

                feedback.removeDrawable(2, 0.5);
				feedback.removeDrawable(5, 0.01);
				feedback.addImage(&poseImages[drinkSequence[1]], WIDTH / 2, top, WIDTH, HEIGHT, 5, 0.1);
                feedback.addText("¡Next pose - " + ofToString(chainevent.getDuration() - chainevent.getTime(), 0) + "!", messageX, messageY, 2, true, 1., ofColor(34,38,76), true);
				feedback.addText("Get it right before", messageX, messageY + line1, 2, false, 1., ofColor(34, 38, 76));
				feedback.addText("the time runs out", messageX, messageY + line1 + line2, 2, false, 1., ofColor(34, 38, 76));
                chainevent.isfirstframe = false;
            }
			unsigned int sec = chainevent.getDuration() - chainevent.getTime();
			feedback.changeText(2,"¡Next pose - " + ofToString(sec) +"!");
			pose(userInView, 1);
			highScoreCounter += ofGetLastFrameTime();
            break;
        }
        case POSE3: {
			
            if(chainevent.isfirstframe){
				// feedback.setMaxAlpha(9, 255);
				// feedback.setMaxAlpha(8, 255, ofColor(255));

                feedback.removeDrawable(2, 0.5);
				feedback.removeDrawable(5, 0.01);
				feedback.addImage(&poseImages[drinkSequence[2]], WIDTH / 2, top, WIDTH, HEIGHT, 5, 0.1);
                feedback.addText("¡Final one - " + ofToString(chainevent.getDuration() - chainevent.getTime(), 0) + "!", messageX, messageY, 2, true, 1., ofColor(34,38,76), true);
				feedback.addText("You are almost there!", messageX, messageY + line1, 2, false, 1., ofColor(34, 38, 76));
                chainevent.isfirstframe = false;
            }
			unsigned int sec = chainevent.getDuration() - chainevent.getTime();
			feedback.changeText(2,"¡Final one - " + ofToString(sec) + "!");
			pose(userInView, 2);
			highScoreCounter += ofGetLastFrameTime();
            break;
        }
        case POUR: {
			bartender.playAnimation("hurra", animationSpeed);
            if(chainevent.isfirstframe){
				//feedback.setMaxAlpha(9, 255, ofColor(255));

				//feedback.removeDrawable(7, 0.5); // poses in bottom
				//feedback.removeDrawable(8, 0.5); // poses in bottom
				//feedback.removeDrawable(9, 0.5); // poses in bottom

                feedback.removeDrawable(2, 0.5);
				feedback.removeDrawable(5, 0.01);
				feedback.removeDrawable(1, 0.2);
				feedback.addImage(&speech, WIDTH / 2, yellow_box, 763, 389, 1);

                feedback.addText("¡Hurra!", messageX, messageY, 2, true, 1., ofColor(34,38,76));
                feedback.addText("Your drink is being poured!", messageX, messageY + line1, 2, false, 1., ofColor(34,38,76));
				serial.writeByte('o');
				
				

				bool popped = false;
				// cout << highscores.size() << endl;
				if (highscores.size() == NUM_HIGHSCORE) {
					if(highScoreCounter < highscores.back().time) {
						highscores.pop_back();
						popped = true;
					}
				}
				if (popped || highscores.size() < NUM_HIGHSCORE) {
					highscore h;
					h.time = highScoreCounter;
					h.person = session;
					for (int i = 0; i<3; i++) {
						
						ofImage img;
						img.load("images\\session_" + ofToString(session) + ofToString(i) + ".png");
						
						h.imgs.push_back(img);
					}
					
					highscores.push_back(h);
					sort();

					session++;
				}

				

				chainevent.isfirstframe = false;

            }
            
            break;
        }
		case CELEBRATE: {
			// cout << "elebrate" << endl;
			if (chainevent.isfirstframe) {
				feedback.removeDrawable(2, 0.5);
				float hScore = highscores.size() > 0 ? highscores[0].time : 10000;
				float sec = highScoreCounter - hScore;
				string s = sec <= 0 ? "Amazing!" : "Pretty good!";

				string ss = sec <= 0 ? "You were the fastest so far!" : ofToString(MAX(sec, 0.1), 1) + " sec from the highscore!";
				
				feedback.addText(s, messageX, messageY, 2, true, 1., ofColor(34, 38, 76));
				feedback.addText(ofToString(highScoreCounter, 1) + " seconds!", messageX, messageY + line1, 2, false, 1., ofColor(34, 38, 76));
				feedback.addText(ss, messageX, messageY + line1 + line2, 2, false, 1., ofColor(34, 38, 76));
				chainevent.isfirstframe = false;
			}
			if(session%2 == 0)bartender.playAnimation("hurra", animationSpeed);
			else bartender.playAnimation("celebrate", animationSpeed);
			serial.writeByte('c');
			break;
		}

        case RESET: {
			// bartender.playAnimation("hurra", animationSpeed);
            if(chainevent.isfirstframe){
                feedback.removeDrawable(2, 0.5);
                feedback.removeDrawable(1, 0.5);

				feedback.removeDrawable(7, 0.3); // poses in bottom
				feedback.removeDrawable(8, 0.4); // poses in bottom
				feedback.removeDrawable(9, 0.5); // poses in bottom
            }
			highScoreCounter = 0.0;
				
            int i = ofRandom(classifier.getNumCLasses());
            int j = ofRandom(classifier.getNumCLasses() -1);
            int k = ofRandom(classifier.getNumCLasses() -2);
            j += j >= i;
            k += k >= std::min(i, j);
            k += k >= std::max(i, j);
            
            drinkSequence.clear();
			if (i == 0) {
				int pj = j;
				j = i;
				i = pj;
			}
            drinkSequence = {i, j, k};
            cout << i<<j<<k << endl;
            
            chainevent.setTo(PLAYING);
			cout << "playing from reset" << endl;
            break;
        }
		case TRYAGAIN: {
			bartender.playAnimation("ohno", animationSpeed);
			if (chainevent.isfirstframe) {
				feedback.removeDrawable(2, 0.5);
				feedback.removeDrawable(5, 0.01);
				feedback.removeDrawable(1, 0.2);
				
				feedback.removeDrawable(7, 0.1); // poses in bottom
				feedback.removeDrawable(8, 0.2); // poses in bottom
				feedback.removeDrawable(9, 0.3); // poses in bottom

				feedback.addImage(&speech, WIDTH / 2, yellow_box, 763, 389, 1);
				feedback.addText("¡Rapido rapido!", messageX, messageY, 2, true, 1., ofColor(34, 38, 76));
				feedback.addText("Better luck next time!", messageX, messageY + line1, 2, false, 1., ofColor(34, 38, 76));
				chainevent.isfirstframe = false;
			} if (chainevent.getTime() > chainevent.getDuration() - 0.1) chainevent.setTo(RESET);

			break;
		}
        /*case TOOMANY: {
            if(chainevent.isfirstframe){
                feedback.removeDrawable(2, 0.5);
                feedback.removeDrawable(1, 0.5);
				feedback.removeDrawable(5, 0.5);
                feedback.addImage(&speech,WIDTH/2, yellow_box, 763, 389, 1);
                feedback.addText("¡Uno por favor!", messageX, messageY, 2, true, 1., ofColor(34,38,76));
                feedback.addText("One of you, step away!", messageX, messageY + line1, 2, false, 1., ofColor(34,38,76));
                chainevent.isfirstframe = false;
            }
			if (numHumansInView == 0) {
				chainevent.setTo(PLAYING);
				cout << "playing from too many" << endl;
			}
			break;
        }*/
        case NOONE: {
			bartender.playAnimation("idle", animationSpeed);
            if(chainevent.isfirstframe){
                feedback.removeDrawable(2, 0.5);
                feedback.removeDrawable(1, 0.5);
				feedback.removeDrawable(5, 0.5);

				feedback.removeDrawable(7, 0.1); // poses in bottom
				feedback.removeDrawable(8, 0.2); // poses in bottom
				feedback.removeDrawable(9, 0.3); // poses in bottom

                feedback.addImage(&speech, WIDTH/2, yellow_box, 763, 389, 1);
				string s = highscores.size() > 0 ? "The highscore" : "¡Hola Senor!";
				if (highscores.size() > 0) {
					for(int i = 0; i<highscores.size(); i++)
						feedback.addText("#"+ofToString(i+1) +": " + ofToString(highscores[i].time, 1), (WIDTH / 3 * i) + WIDTH / 6, HEIGHT - 40, 2, false, 1., ofColor(255, 200, 6));
				}
				
				feedback.addText("Step right up beat the", messageX, messageY + line1, 2, false, 1., ofColor(34, 38, 76));
				feedback.addText("highscore and get a drink", messageX, messageY + line1 + line2, 2, false, 1., ofColor(34, 38, 76));
				
                feedback.addText(s, messageX, messageY, 2, true, 1., ofColor(34,38,76));
                chainevent.isfirstframe = false;
            }
			if (numHumansInView > 0) {
				chainevent.setTo(PLAYING);
			}
			break;
        }
        default:
            break;
    }
    

    
    
    
    box2d.update();
    
    if(addSamples){
        chainevent.beginEvents();
        addSamples = false;
    }
    if(clearSample){
        classifier.clearSample(numPoses);
	//	clearSample = false;
    }
    if(train){
        chainevent.setTo(TRAINING);
        train = false;
    }

	echoArduino();
	if (serial.isInitialized())readArduino();
}
void ofApp::pose(int _user, int _posenum) {
	user u = theUsers[_user];
	GRT::VectorFloat sample = getSample(u);
	//cout << sample.size() << endl;
	classifier.updateSample(sample);
	int currentPose = classifier.label;

	if ((currentPose == drinkSequence[_posenum] && classifier.probability > prababilityThreshold) || jumpToNext)

	{
		test.update();
		ofSaveImage(test.getPixels(), "images\\session_" + ofToString(session)+ ofToString(_posenum) + ".png");
		//cout << "pose is right, from: " << chainevent.getName() << " too: ";
		chainevent.next();
		//cout << chainevent.getName() << endl;
		jumpToNext = false;
	}
	if (chainevent.getDuration() - chainevent.getTime() < 0.1) {
		chainevent.setTo(TRYAGAIN);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    render.begin();
    ofClear(0);
	float aspect = logo.getHeight() / logo.getWidth();
	//feedback.addImage(&logo, WIDTH / 2, 200, 817, aspect * 817, 100, 5.);
	logo.draw(WIDTH / 2 - 817 / 2, 200, 817, 817 * aspect);
	//aspect = backgound.getHeight() / backgound.getWidth();
	//feedback.addImage(&backgound, WIDTH / 2, HEIGHT - aspect*WIDTH, WIDTH, aspect*WIDTH, 100, 5.);
	backgound.draw(0, HEIGHT - aspect*WIDTH, WIDTH, aspect*WIDTH);
	
	if(chainevent.getName()!=PLAYING)feedback.draw(0, 0);
    
	switch (chainevent.getName()) {
		case NOONE: {
			ofPushMatrix();
			ofTranslate(0, top);
			ofSetColor(255);
			ofSetColor(255, MAX(chainevent.getTime() * 100, 255));
			drawUserWithPngs(bartender.circlePoints, false);
			ofPopMatrix();

			confettis.draw();

			
			if (highscores.size() > 0) {
				frameNum += ofGetLastFrameTime() * 3.;
				int f = frameNum;
				f = f % 3;
				int w = WIDTH / 4;
				int h = w * VIDEO_H / VIDEO_W;
				ofSetColor(255);
				for (int i = 0; i < highscores.size(); i++) {
					ofPushMatrix();
					ofTranslate((WIDTH / 3 * i) + WIDTH / 6, HEIGHT - h / 2 - 150);
					ofRotateDeg(-90);
					highscores[i].imgs[f].draw(-w / 2, -h / 2, w, h);
					ofPopMatrix();
				}
			}
			break;
		}
		case POUR: {}
		case CELEBRATE: {
			confettis.draw();
		}
		case TRYAGAIN: {
			ofPushMatrix();
			ofTranslate(0, top);
			ofSetColor(255);
			ofSetColor(255, MAX(chainevent.getTime() * 100, 255));
			drawUserWithPngs(bartender.circlePoints, false);
			ofPopMatrix();
			break;
		}

		default: {
			ofSetColor(255);
			ofPushMatrix();
			ofTranslate(0, top);
			ofSetColor(255);
			for (int i = 0; i < MIN(numHumansInView, theUsers.size()); i++) {
				if (i == userInView) ofSetColor(255);
				else ofSetColor(255, 200);
					//ofDrawBitmapStringHighlight(ofToString(i), theUsers[i].circlePoints[0]);
				drawUserWithPngs(theUsers[i].circlePoints, true);
				//ofDrawBitmapStringHighlight(ofToString(i), theUsers[i].circlePoints[0]);

			}
			ofPopMatrix();
		}
	}
	if (chainevent.getName() == PLAYING)feedback.draw(0, 0);

    render.end();
    
    

	ofBackgroundHex(0x22264C);
	ofSetColor(255);
	render.draw(0,0);

    
    if(!fullscreen)gui.draw();
    
    
    if(debug){
        classifier.drawAllLabels();
        ofNoFill();
        ofSetLineWidth(5);
        ofPushMatrix();
        ofTranslate((ofGetWidth()/5)*(numPoses%5), (ofGetHeight()/5)*int(numPoses/5));
        ofDrawRectangle(0, 0, ofGetWidth()/5, ofGetHeight()/5);
        ofPopMatrix();
        ofSetLineWidth(1);
		ofSetColor(0);
        ofDrawBitmapString("Classifier: " + classifier.currentClassifier, gui.getWidth() + 40, 10);
		ofDrawBitmapString(ofToString(drinkSequence[0]) + " " + ofToString(drinkSequence[1]) + " " + ofToString(drinkSequence[2]), gui.getWidth() + 40, 30);
    }
}
void ofApp::drawUserWithPngs(vector<ofVec2f> p, int pngs){
    

	// cout << p[0].x << " " << p[0].y << endl;

    for (int i = 0; i < indxes.size(); i++) {
        float rotation = getAngle(p[indxes[i][0]], p[indxes[i][1]]);
        ofVec2f position = getMean(p[indxes[i][0]], p[indxes[i][1]]);
        float w = bodyPartImages[i + pngs * 14].getWidth();
        float h = (p[indxes[i][1]] - p[indxes[i][0]]).length();
        ofPushMatrix();
        ofTranslate(position.x + pos_parts[i].get().x, position.y + pos_parts[i].get().y);
        ofRotate(rotation - 90);
        bodyPartImages[i + pngs*14].draw(-(w*s_parts[i]) / 2, -(h)/2, w*s_parts[i], h);
        ofPopMatrix();
        
        if(indxes[i][2]>0) {
            h = bodyPartImages[indxes[i][2] + pngs * 14].getHeight();
            w = bodyPartImages[indxes[i][2] + pngs * 14].getWidth();
            ofPushMatrix();
            ofTranslate(p[indxes[i][0]].x , p[indxes[i][0]].y );
            ofRotate(rotation - 90);
            bodyPartImages[indxes[i][2] + pngs*14].draw(-(w*s_parts[8]/2), -(h*s_parts[8]/2), w*s_parts[8], h*s_parts[8]);
            ofPopMatrix();
        }
    }
	// torso
	ofVec2f mean = (p[2] + p[5] + p[8] + p[11]) / 4;
	ofPushMatrix();
	ofTranslate(mean.x, mean.y);
	ofRotate(getAngle(p[1], p[14]) - 90);
	float w = (p[5] - p[2]).length();
	float h = (p[1] - getMean(p[8], p[11])).length();
	int img = 13;
	if (w / h < bellyThreshold)img = 1;
	w = MAX(100, w);
	//cout << w << " " << h << endl;

	bodyPartImages[img + pngs * 14].draw(-w / 2, -h / 2, w, h);
	ofPopMatrix();
    //head
    ofPushMatrix();
    ofTranslate(getMean(p[0], p[1]).x , getMean(p[0], p[1]).y );
    ofRotate(getAngle(p[0], p[1])-90);
    bodyPartImages[8 + pngs*14].draw(-(bodyPartImages[8+ pngs*14].getWidth()*s_head) / 2, -(bodyPartImages[8+ pngs*14].getHeight()*s_head)/2, bodyPartImages[8+ pngs*14].getWidth()*s_head, bodyPartImages[8+ pngs*14].getHeight()*s_head);
    ofPopMatrix();
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == 'l')chainevent.beginEvents();
    if (key == 'd')debug = !debug;
    if (key == 's')classifier.save();
	if (key == 'j')jumpToNext = true;
	if (key == 'f')fullscreen = !fullscreen;
	// ofSetFullscreen(fullscreen);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if(key == OF_KEY_TAB){
        classifier.tryNewClassifier();
    }
    if (key == 'b')drawUserOrbartender = !drawUserOrbartender;
}
void ofApp::echoArduino() {
    
    echoTimer += ofGetLastFrameTime();
    if(echo && echoTimer > 30.){
        if(serial.isInitialized())serial.writeByte('q');
        echo = false;
        echoTimer = 0.0;
    }
    if(echoTimer>5. && !echo) {
        serial.listDevices();
        vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
		for (int i = 0; i < deviceList.size(); i++)cout << deviceList[i].getDeviceName()<<endl;
        echoTimer = 0.0;
        int baud = 9600;
        if(deviceList.size()>0)
            serial.setup(deviceList[deviceCount%deviceList.size()].getDeviceID(), baud);
        
        nTimesRead = 0;
        nBytesRead = 0;
        readTime = 0;
        memset(bytesReadString, 0, 4);
        
        if(serial.isInitialized())
            serial.writeByte('q');
        
        deviceCount++;
        cout<<"no echo"<<endl;
    }
}

void ofApp::readArduino(){
    
    int tempInput = -1;
    nTimesRead = 0;
    nBytesRead = 0;
    int nRead  = 0;  // a temp variable to keep count per read
    
    unsigned char bytesReturned[3];
    
    memset(bytesReadString, 0, 4);
    memset(bytesReturned, 0, 3);
    
    while( (nRead = serial.readBytes( bytesReturned, 3)) > 0){
        nTimesRead++;
        nBytesRead = nRead;
    };
    
    //if(nBytesRead>0){
    memcpy(bytesReadString, bytesReturned, 3);
    
    bSendSerialMessage = false;
    readTime = ofGetElapsedTimef();
    
    string fromArduino = string(bytesReadString);
    
    if(fromArduino == "w") {
        echo = true;
        cout<< "arduino is on"<<endl;
    }
    
    char fa = fromArduino[0];
    tempInput = fa;
    
    if(tempInput>0) {
        echoTimer = 0.0;
        echo = true;
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    numPoses =(x/(ofGetWidth()/5)) + y/(ofGetHeight()/5) * 5;
    cout <<"x: "<<x/(ofGetWidth()/5)<<" y: "<<y/(ofGetHeight()/5) << " pose number: "<< numPoses << endl;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}
void ofApp::exit(){
    for(int i = 0; i<theUsers.size(); i++)theUsers[i].exit();
}
