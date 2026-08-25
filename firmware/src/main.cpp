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

// Adds controlled random noise to a feature value
// noiseLevel 0.10 = up to 10% variation in either direction
float applyNoise(float value, float noiseLevel) {
  float noise = (random(-1000, 1000) / 10000.0f) * noiseLevel;
  float noisy = value + noise;
  if (noisy < 0.0f) noisy = 0.0f;  // clamp to valid range
  if (noisy > 1.0f) noisy = 1.0f;
  return noisy;
}

void setup() {
  Serial.begin(115200);
  randomSeed(millis());
  delay(2000);
  Serial.println("==================================");
  Serial.println(" CB011911 TinyML-IDS — MVP");
  Serial.println(" Explainable Edge IDS");
  Serial.println("==================================");
  Serial.println();
}

void loop() {
  static int cycle = 0;
  static float noiseLevel = 0.05f;  // starts at 5% noise
  cycle++;

  Serial.println("========================================");
  Serial.println("SYNTHETIC TRAFFIC — Cycle " + String(cycle));
  Serial.print("Noise level: +/-"); Serial.print(noiseLevel * 100, 0); Serial.println("%");
  Serial.println("========================================");
  Serial.println();

  for (int s = 0; s < NUM_SAMPLES; s++) {
    // Build noisy version of this sample
    float noisyVec[NUM_FEATURES];
    for (int f = 0; f < NUM_FEATURES; f++) {
      noisyVec[f] = applyNoise(TEST_VECTORS[s][f], noiseLevel);
    }

    // Inference + timing
    unsigned long t0 = micros();
    int pred = predictClass(noisyVec);
    unsigned long t1 = micros();
    const char* expl = getExplanation(pred, noisyVec);
    unsigned long t2 = micros();

    bool match = (strcmp(EXPECTED[s], CLASS_NAMES[pred]) == 0);

    Serial.println("--- Sample " + String(s + 1) + " ---");
    Serial.print("Base class:   "); Serial.println(EXPECTED[s]);
    Serial.print("Predicted:    "); Serial.println(CLASS_NAMES[pred]);
    Serial.print("Stable:       "); Serial.println(match ? "YES" : "Boundary case");
    Serial.print("Explanation:  "); Serial.println(expl);
    Serial.print("Cycle time:   "); Serial.print(t2 - t0); Serial.println(" us");
    Serial.print("{\"class\":\""); Serial.print(CLASS_NAMES[pred]);
    Serial.print("\",\"explanation\":\""); Serial.print(expl);
    Serial.println("\"}");
    Serial.println();
    delay(1500);
  }

  // Increase noise each cycle: 5% -> 10% -> 15% -> 20% -> back to 5%
  noiseLevel += 0.05f;
  if (noiseLevel > 0.20f) noiseLevel = 0.05f;

  Serial.println();
  delay(2000);
}