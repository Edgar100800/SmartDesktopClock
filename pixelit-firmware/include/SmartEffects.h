#pragma once

enum SmartScene : uint8_t { SMART_NONE, SMART_FIRE, SMART_COMETS, SMART_METEORS, SMART_RAIN, SMART_MATRIX_RAIN, SMART_SPACE, SMART_MEADOW, SMART_SEA, SMART_CITY, SMART_SOLID };

class SmartEffects {
public:
    void begin(FastLED_NeoMatrix *display) { matrix = display; }
    void stop() { scene = SMART_NONE; }
    bool active() const { return scene != SMART_NONE; }
    void configure(SmartScene next, bool showClock, bool seconds, uint16_t digits, uint16_t background, uint8_t level, uint16_t speed) {
        scene = next; clock = showClock; withSeconds = seconds; digitColor = digits; backgroundColor = background;
        backgroundLevel = constrain(level, 20, 70); phase = 0; boatX = 36; boatY = 4; boatV = -.45f; boatVy = 0;
        frameInterval = constrain(10000UL / constrain(speed, 25, 200), 50UL, 400UL);
        memset(heat, 0, sizeof(heat)); nextFrame = millis();
    }
    void update() {
        if (!matrix || scene == SMART_NONE) return;
        unsigned long now = millis();
        if ((long)(now - nextFrame) < 0) return;
        nextFrame += frameInterval;
        if ((long)(now - nextFrame) >= (long)frameInterval) nextFrame = now + frameInterval;
        phase++; prepareFrame();
        matrix->clear();
        for (int x=0;x<32;x++) for (int y=0;y<8;y++) matrix->drawPixel(x,y,shade(frame[y*32+x]));
        if (clock) drawClock(); matrix->show();
    }
private:
    FastLED_NeoMatrix *matrix = nullptr; SmartScene scene = SMART_NONE; bool clock=false,withSeconds=false;
    uint16_t frameInterval=100;
    uint16_t digitColor=0xffff,backgroundColor=0x020c,frame[256]={0};
    uint8_t backgroundLevel=40,heat[256]={0},waveY[32]={0}; int8_t grassTip[32]={0}; int meadowFlowerX=23;
    unsigned long nextFrame=0; uint32_t phase=0; float boatX=36,boatY=4,boatV=-.45f,boatVy=0;
    uint16_t shade(uint16_t c) { if (!clock) return c; uint8_t r=(c>>11)&31,g=(c>>5)&63,b=c&31; float f=backgroundLevel/100.0f; return (uint16_t(round(r*f))<<11)|(uint16_t(round(g*f))<<5)|uint16_t(round(b*f)); }
    void prepareFrame() {
        if(scene==SMART_FIRE){for(int x=0;x<32;x++)heat[7*32+x]=150+random(106);for(int y=6;y>=0;y--)for(int x=0;x<32;x++){int b=heat[(y+1)*32+x],b2=y<6?heat[(y+2)*32+x]:b;heat[y*32+x]=max(0,(b*3+b2)/4-18-int(random(18)));}}
        if(scene==SMART_SEA){for(int x=0;x<32;x++)waveY[x]=5+round(sin(x*.55f+phase*.32f)*.7f+sin(x*.19f-phase*.2f)*.45f);updateBoat();}
        if(scene==SMART_MEADOW){meadowFlowerX=23+round(sin(phase*.35f));for(int x=0;x<32;x++)grassTip[x]=x+round(sin(phase*.28f+x*.7f));}
        for(int x=0;x<32;x++)for(int y=0;y<8;y++)frame[y*32+x]=pixel(x,y);
    }
    uint16_t pixel(int x,int y) {
        if(scene==SMART_SOLID) return backgroundColor;
        if(scene==SMART_FIRE){int v=heat[y*32+x];if(v<24)return 0;if(v<95)return min(31,v>>3)<<11;if(v<180)return(31<<11)|(min(63,(v-80)>>1)<<5);return 0xffe0|min(31,(v-170)>>2);}
        if(scene==SMART_RAIN){int head=(phase+x*5)%14-3;if(y==head)return 0x07ff;if(y==head-1||y==head-2)return 0x0210;return 0;}
        if(scene==SMART_MATRIX_RAIN){int head=(phase+x*7)%16;if(y==head%12)return 0x07e0;if(y==head%12-1||y==head%12-2)return 0x0200;return 0;}
        if(scene==SMART_COMETS){for(int m=0;m<3;m++){int p=(phase+m*17)%55,cx=37-p,cy=-2+p/6+(m&1);for(int t=0;t<8;t++)if(x==cx+t&&y==cy-t/3)return t?0x7bef:0xffff;}return 0;}
        if(scene==SMART_METEORS){for(int m=0;m<2;m++){int p=(phase+m*29)%60,cx=37-p,cy=-2+p/6;if((x==cx||x==cx+1)&&(y==cy||y==cy+1))return x==cx&&y==cy?0xffff:0xfd20;for(int t=2;t<12;t++)if(x==cx+t&&y==cy-t/3)return t<6?0xfbe0:0x7800;}return 0;}
        if(scene==SMART_SPACE){if(((x*17+y*29+phase/3)%47)==0)return 0xffff;int px=35-int(phase%45),py=4;if((x-px)*(x-px)+(y-py)*(y-py)<=4)return y<py?0xfde0:0x780f;return 0;}
        if(scene==SMART_MEADOW){if(abs(x-meadowFlowerX)+abs(y-3)==1)return 0xf81f;if(x==meadowFlowerX&&y==3)return 0xffe0;if(y<6)return 0x0208;if(y==7)return 0x03e0;int h=1+(x*7%3);if(y>=7-h&&(x==grassTip[x]||x==23))return 0x0600;return 0x02a0;}
        if(scene==SMART_SEA){int wy=waveY[x];if(y>=wy)return y==wy?0x07ff:0x027f;int bx=round(boatX),by=round(boatY);if(x>=bx&&x<bx+7&&y==by)return 0xf920;if(x>bx&&x<bx+6&&y==by-1)return 0xffe0;if(x==bx+3&&y>=by-4&&y<by)return 0xffff;return 0x020c;}
        if(scene==SMART_CITY){int heights[16]={3,2,4,3,5,2,4,6,3,5,2,4,3,6,4,2};int h=heights[x/2];uint16_t c=y>=8-h?0x1084:0x0008;if(y>=8-h&&y<7&&x%2&&(phase+x+y)%5<3)c=0xffa0;if(x==27&&y==1)c=0xffe0;return c;}
        return 0;
    }
    void updateBoat() { float target=waveY[constrain(int(boatX)+3,0,31)]-1;boatVy+=(target-boatY)*.02f;boatVy*=.96f;boatV+=(-.48f-boatV)*.08f;boatX+=boatV;boatY+=boatVy;if(boatX<-9)boatX=36; }
    void drawGlyph(char c,int &x) { static const char *digits[10]={"111101101101111","010110010010111","111001111100111","111001111001111","101101111001001","111100111001111","111100111101111","111001010010010","111101111101111","111101111001111"}; if(c==':'){matrix->drawPixel(x,2,digitColor);matrix->drawPixel(x,4,digitColor);x+=2;return;}const char *g=digits[c-'0'];for(int y=0;y<5;y++)for(int px=0;px<3;px++)if(g[y*3+px]=='1')matrix->drawPixel(x+px,y+1,digitColor);x+=4; }
    void drawClock() { char value[9]; if(withSeconds)sprintf(value,"%02d:%02d:%02d",hour(),minute(),second());else sprintf(value,"%02d:%02d",hour(),minute());int len=strlen(value),width=0;for(int i=0;i<len;i++)width+=value[i]==':'?2:4;int x=max(0,(32-width)/2);for(int i=0;i<len;i++)drawGlyph(value[i],x); }
};
