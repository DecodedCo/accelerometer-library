int j, delta = 10; // ms
int iterations = 10; // to calculate delta a
String axes[3][2] = {
  {"Forward", "Backward"},
  {"Left", "Right"},
  {"Down", "Up"}
};
//double delta_a[3][20];
double average_delta_a[3];
double threshold = 0.2; // significant delta a
unsigned long previous_event_time[3] = {0,0,0}; // when it happened
unsigned long event_threshold = 250; // in ms, to count as movement
double previous_event[3]; // the delta value

int axis = 0; // x
double previous_acceleration[3];

void loop(){
  
  for (axis=0; axis<3; axis++) {
    average_delta_a[axis]  = 0;
  }
  
  for (i=0; i<iterations; i++) {
    readAccelerometer();
    for (axis=0; axis<3; axis++) {
      //delta_a[axis][i]=current_acceleration[axis] - previous_acceleration[axis];
      //average_delta_a[axis] += delta_a[axis][i];
      average_delta_a[axis] += current_acceleration[axis] - previous_acceleration[axis];
    }
    delay(delta);
  }

  for (axis=0; axis<3; axis++) {
    average_delta_a[axis] = average_delta_a[axis] / iterations;
    
    if (abs(average_delta_a[axis]) > threshold
      && (millis() - previous_event_time[axis]) > event_threshold) {
      if (previous_event[axis] > 0 && average_delta_a[axis] < 0) { // turning point
        Serial.println(axes[axis][0]);
      } else if (previous_event[axis] < 0 && average_delta_a[axis] > 0) { // turning point 
        Serial.println(axes[axis][1]);
      }
      previous_event_time[axis] = millis(); // update last event
      previous_event[axis] = average_delta_a[axis]; // store this last acceleration value
    }
    
  }
  
  // store previous acceleration
  for (i=0; i<NELEMS(current_acceleration); i++) {
    previous_acceleration[i] = current_acceleration[i];
  }
}
