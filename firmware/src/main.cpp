#include <Arduino.h>
#include "xai_rules.h"

const char* FEATURE_NAMES[] = {
  "Header_Length", "Number", "TCP", "ack_flag_number",
  "Tot size", "ack_count", "AVG"
};
const int NUM_FEATURES = 7;
const char* CLASS_NAMES[] = {"Benign", "DDoS", "Reconnaissance"};

const float TEST_VECTORS[3][7] = {
  {0.5333f, 0.0816f, 1.0000f, 1.0000f, 0.1397f, 0.1000f, 0.1397f},  // Benign
  {0.1320f, 1.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f},  // DDoS
  {0.3867f, 0.0816f, 1.0000f, 0.5000f, 0.0010f, 0.0500f, 0.0010f}   // Recon
};
const char* EXPECTED[] = {"Benign", "DDoS", "Reconnaissance"};
const int NUM_SAMPLES = 3;

int getFeatureIdx(const char* name) {
  for (int j = 0; j < NUM_FEATURES; j++) {
    if (strcmp(name, FEATURE_NAMES[j]) == 0) return j;
  }
  return -1;
}

int predictClass(const float* feat) {
  for (int i = 0; i < NUM_RULES; i++) {
    if (xai_rules[i].feature == NULL)
      return xai_rules[i].predicted_class;
    int idx = getFeatureIdx(xai_rules[i].feature);
    if (idx < 0) continue;
    float v = feat[idx];
    if (xai_rules[i].greater_than && v > xai_rules[i].threshold)
      return xai_rules[i].predicted_class;
    if (!xai_rules[i].greater_than && v < xai_rules[i].threshold)
      return xai_rules[i].predicted_class;
  }
  return 0;
}

const char* getExplanation(int cls, const float* feat) {
  for (int i = 0; i < NUM_RULES; i++) {
    if (xai_rules[i].predicted_class != cls) continue;
    if (xai_rules[i].feature == NULL)
      return xai_rules[i].explanation;
    int idx = getFeatureIdx(xai_rules[i].feature);
    if (idx < 0) continue;
    float v = feat[idx];
    if (xai_rules[i].greater_than && v > xai_rules[i].threshold)
      return xai_rules[i].explanation;
    if (!xai_rules[i].greater_than && v < xai_rules[i].threshold)
      return xai_rules[i].explanation;
  }
  return "Traffic within expected parameters.";
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("==================================");
  Serial.println(" CB011911 TinyML-IDS — MVP");
  Serial.println(" Explainable Edge IDS");
  Serial.println("==================================");
  Serial.println();
}

void loop() {
  for (int s = 0; s < NUM_SAMPLES; s++) {
    const float* vec = TEST_VECTORS[s];

    // Time inference
    unsigned long t0 = micros();
    int pred = predictClass(vec);
    unsigned long t1 = micros();

    // Time XAI lookup
    unsigned long t2 = micros();
    const char* expl = getExplanation(pred, vec);
    unsigned long t3 = micros();

    unsigned long inferUs = t1 - t0;
    unsigned long xaiUs   = t3 - t2;
    unsigned long totalUs = inferUs + xaiUs;

    Serial.println("--- Inference Cycle ---");
    Serial.print("Expected:    "); Serial.println(EXPECTED[s]);
    Serial.print("Predicted:   "); Serial.println(CLASS_NAMES[pred]);
    Serial.print("Explanation: "); Serial.println(expl);
    Serial.print("MQTT payload: {\"class\":\"");
    Serial.print("Inference time: "); Serial.print(inferUs); Serial.println(" us");
    Serial.print("XAI time:       "); Serial.print(xaiUs);   Serial.println(" us");
    Serial.print("Total time:     "); Serial.print(totalUs); Serial.println(" us");
    Serial.println("{\"class\":\"" + String(CLASS_NAMES[pred]) + "\",\"explanation\":\"" + String(expl) + "\"}");
    Serial.println();
    delay(2000);
  }
  
  Serial.println("--- Cycle complete. Restarting... ---");
  Serial.println();
  delay(3000);
}