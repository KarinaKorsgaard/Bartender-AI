//
//  feedback.h
//  summerparty
//
//  Created by Karina Korsgaard Jensen on 06/06/2018.
//

#ifndef feedback_h
#define feedback_h
#define WIDTH 1080
#define HEIGHT 1920
struct drawable{
    ofImage* img;
    bool isImage = false;
    double timer = 0.0;
    double transitiontime = 1.0;
    float alpha = 0.f;
    string text;
    bool isLarge = false;
    bool remove = false;
    int iid;
	bool dynamic = false;
    ofColor color;
    int w, h, x, y;
	int maxAlpha = 255;
	float from = 0;
	float delay = 0.0;
};
class Feedback{

    ofTrueTypeFont small;
    ofTrueTypeFont large;
    
public:
    vector<drawable>drawables;
    
    void setup(){
        small.load("Apercu-Bold.ttf", 40);
        large.load("Apercu-Medium.ttf", 66);
    }
    void addImage(ofImage * img, int x, int y, int w, int h, int _iid, float trans = 1., float _delay = 0.0, ofColor col = ofColor(255), int _maxAlpha = 255){
        drawable d;
        d.img = img;
        d.isImage = true;
        d.transitiontime = trans;
        d.iid = _iid;
        d.color = col;
        d.w = w;
        d.h = h;
        d.x = x;
        d.y = y;
		d.maxAlpha = _maxAlpha;
		d.delay = _delay;
        drawables.push_back(d);
    }
	void setMaxAlpha(int _iid, int alpha, ofColor col = NULL) {
		for (int i = 0; i<drawables.size(); i++) {
			if (drawables[i].iid == _iid && !drawables[i].remove) {
				drawables[i].timer = 0.0;
				drawables[i].from = drawables[i].maxAlpha;
				drawables[i].maxAlpha = alpha;
				if (col != NULL)drawables[i].color = col;
				// drawables[i].timer = 0.0;
				// drawables[i].transitiontime = trans;
			}
		}
	}
	bool do_change;
	string changeString;
	int changeId;
	void changeText(int _iid, string t) {
		do_change = true;
		changeId = _iid;
		changeString = t;
	}
	int dynamicText;
    void addText(string str, int x, int y, int _iid, bool isLarge, float trans = 1., ofColor col = ofColor(255), bool isDynamic = false){
        drawable d;
        d.text = str;
        d.x = x;
        d.y = y;
        d.isLarge = isLarge;
        d.transitiontime = trans;
        d.iid = _iid;
        d.color = col;
		d.dynamic = isDynamic;
		//if(isDynamic)dynamicText = drawables.size();
        drawables.push_back(d);
    }
    void removeDrawable(int _iid, double trans = 1.){
        for(int i = 0; i<drawables.size(); i++){
            if(drawables[i].iid == _iid){
                drawables[i].remove = true;
                drawables[i].timer = 0.0;
                drawables[i].transitiontime = trans;
            }
        }
    }
    void update(){
        for(int i = 0; i<drawables.size(); i++){
            drawables[i].timer+=ofGetLastFrameTime();
			if (do_change && drawables[i].dynamic) {
				drawables[i].text = changeString;
				do_change = false;
				//cout << "change text" << endl;
			}
            if(drawables[i].remove){
                drawables[i].alpha = ease(drawables[i].timer, 255, 0, drawables[i].transitiontime);
                if(drawables[i].alpha<0.1)drawables.erase(drawables.begin()+i);
            }
            else if(drawables[i].timer > drawables[i].delay)drawables[i].alpha = ease(drawables[i].timer-drawables[i].delay, drawables[i].from, drawables[i].maxAlpha, drawables[i].transitiontime);
        }
    }
    
    void draw(int x, int y){
        
        ofPushMatrix();
        ofTranslate(x ,y);

        
        for(int i = 0; i<drawables.size(); i++){
            ofSetColor(drawables[i].color, drawables[i].alpha);
            if(drawables[i].isImage){
                drawables[i].img->draw(drawables[i].x-(drawables[i].w/2), drawables[i].y, drawables[i].w, drawables[i].h);
            }
            else if(drawables[i].isLarge){
                large.drawString(drawables[i].text, -large.getStringBoundingBox(drawables[i].text, 0, 0).width/2 + drawables[i].x, drawables[i].y);
            }
            else{
                small.drawString(drawables[i].text, -small.getStringBoundingBox(drawables[i].text, 0, 0).width/2 + drawables[i].x, drawables[i].y);
            }
        }
        ofPopMatrix();

    }

    float ease(float time, float begin, float end, float duration) {
        end -=begin;
        
        if (time > duration) return end+begin;
        time /= duration;
        return end*time*time + begin;   
    }
};
#endif /* feedback_h */
