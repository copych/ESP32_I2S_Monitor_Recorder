
void drawWave() {
  int x, y, oldx, oldy;
  oldx = 0;
  oldy = (int)( (float)disp[0])+offset ;
  for (int i = 0 ; i< SCREEN_WIDTH ; ++i) {
    x = i;
    y =  (int)( (float)disp[i])+offset ;
    display.drawLine(oldx, oldy, x, y, 1);
    oldx = x;
    oldy = y;
  } 
}

void drawPeak() {
  int dispPeak;  
  dispPeak = (100.0f * (float)sampMax / (float)0x7fff);
  display.setTextColor(2);
  display.setCursor(SCREEN_WIDTH - 60,  10 );
  display.print("Peak:");
  display.print(dispPeak);
  display.print("%");
}

void drawFFT () {
  static float posOffset[129] = {
    0, 0.007751465, 0.031005859, 0.069763184, 0.124023438, 0.193786621, 0.279052735, 0.379821778, 
    0.49609375, 0.627868653, 0.775146485, 0.937927247, 1.116210939, 1.30999756, 1.519287111, 1.744079591, 
    1.984375002, 2.240173342, 2.511474612, 2.798278811, 3.10058594, 3.418395999, 3.751708988, 4.100524906, 
    4.464843754, 4.844665532, 5.239990239, 5.650817876, 6.077148443, 6.518981939, 6.976318366, 7.449157722, 
    7.937500007, 8.441345222, 8.960693367, 9.495544442, 10.04589845, 10.61175538, 11.19311524, 11.78997804, 
    12.40234376, 13.03021241, 13.673584, 14.33245851, 15.00683595, 15.69671632, 16.40209962, 17.12298586, 
    17.85937502, 18.61126711, 19.37866213, 20.16156008, 20.95996096, 21.77386477, 22.6032715, 23.44818117, 
    24.30859377, 25.1845093, 26.07592776, 26.98284915, 27.90527346, 28.84320071, 29.79663089, 30.76556399, 
    31.75000003, 32.74993899, 33.76538089, 34.79632571, 35.84277347, 36.90472415, 37.98217777, 39.07513431,
    40.18359379, 41.30755619, 42.44702152, 43.60198979, 44.77246098, 45.9584351, 47.15991215, 48.37689213, 
    49.60937504, 50.85736089, 52.12084966, 53.39984136, 54.69433599, 56.00433355, 57.32983404, 58.67083745, 
    60.0273438, 61.39935308, 62.78686529, 64.18988043, 65.6083985, 67.04241949, 68.49194342, 69.95697028, 
    71.43750006, 72.93353278, 74.44506843, 75.972107, 77.51464851, 79.07269294, 80.64624031, 82.2352906, 
    83.83984383, 85.45989998, 87.09545906, 88.74652108, 90.41308602, 92.09515389, 93.79272469, 95.50579843, 
    97.23437509, 98.97845468, 100.7380372, 102.5131227, 104.303711, 106.1098023, 107.9313966, 109.7684938, 
    111.6210939, 113.4891969, 115.3728028, 117.2719117, 119.1865235, 121.1166383, 123.062256, 125.0233766, 
    127.0000001
  };
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    int j = k1 * posOffset[i] + 1;
    // int j = k1 * i + 1;
    int R = re[j];
    int I = im[j];
 
    int dat = 12.0f * (i * 0.024f + 1.0f) * (float)(sqrt(R * R + I * I)) * downscale ;     // frequency magnitude is the square root of the sum of the squares of the real and imaginary parts of a vector
    if (dat <= old_dat[i]) {
      dat = (4 * old_dat[i] + dat) * 0.2f;
    }
    display.drawLine(i, ylim, i, ylim - dat, 2); 
    old_dat[i] = dat;
  }
}

void drawRec () { 
    display.setTextColor(2);
    display.setCursor( 5,  10 );
    display.print(dataWritten);
}
