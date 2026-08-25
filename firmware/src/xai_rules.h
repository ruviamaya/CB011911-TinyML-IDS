// Auto-generated XAI rule table
// Source: 05_xai_rules.ipynb
// Features: ['Header_Length', 'Number', 'ack_flag_number', 'TCP', 'ack_count', 'Tot size', 'AVG']

#ifndef XAI_RULES_H
#define XAI_RULES_H

struct XAIRule {
  const char* feature;
  float threshold;
  bool greater_than;
  int predicted_class;
  const char* explanation;
};

const int NUM_RULES = 8;
const XAIRule xai_rules[] = {
  {"Number", 0.5406f, true, 1, "DDoS detected: Number is above normal range, consistent with volumetric flooding."},
  {"ack_count", 0.0411f, false, 1, "DDoS detected: ack_count is below normal range, consistent with volumetric flooding."},
  {"ack_flag_number", 0.4001f, false, 1, "DDoS detected: ack_flag_number is below normal range, consistent with volumetric flooding."},
  {"Tot size", 0.0291f, false, 1, "DDoS detected: Tot size is below normal range, consistent with volumetric flooding."},
  {"TCP", 0.8028f, false, 2, "Probe detected: TCP is below normal range, consistent with network reconnaissance."},
  {"Header_Length", 0.4198f, false, 2, "Probe detected: Header_Length is below normal range, consistent with network reconnaissance."},
  {"AVG", 0.0376f, false, 2, "Probe detected: AVG is below normal range, consistent with network reconnaissance."},
  {NULL, 0.0f, true, 0, "No threat detected: traffic features are within normal operating ranges."}
};

#endif // XAI_RULES_H