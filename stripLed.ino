void switchStripOn(int red, int green, int blue, int brightness){
  uint32_t color=strip.Color(red,green,blue);
  strip.setBrightness(brightness);
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, color); 
  }
  strip.show();    
  //yield();
}

void switchStripOff(){
  uint32_t color=strip.Color(0,0,0);
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, color);
  }
  strip.show();    
  //yield();  
}
